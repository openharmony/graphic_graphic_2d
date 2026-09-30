/*
 * Copyright (c) 2024-2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_profiler.h"

#include <cstddef>
#include <cerrno>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <system_error>
#include <unistd.h>

#include "rs_profiler_archive.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_capture_recorder.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_command.h"
#include "rs_profiler_file.h"
#include "rs_profiler_json.h"
#include "rs_profiler_log.h"
#include "rs_profiler_network.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_pixelmap.h"
#include "rs_profiler_settings.h"
#include "rs_profiler_telemetry.h"
#include "rs_profiler_test_tree.h"

#include "common/rs_common_def.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "render/rs_typeface_cache.h"
#include "surface_capture_param.h"
#include "graphic_feature_param_manager.h"
#include "parameters.h"

#ifndef TRACE3D_CORE_API_NO_NAMESPACE
#define TRACE3D_CORE_API_NO_NAMESPACE
#endif

#define TRACE3D_CORE_API_INIT() Trace3DCoreInitRS()
#include "trace3d/api_core/trace3d_api_core.h"

const TRACE3D_CORE_API_TABLE* Trace3DCoreInitRS()
{
    static bool initDone = false;
    static const TRACE3D_CORE_API_TABLE *apiTablePtr = nullptr;
    if (initDone) {
        return apiTablePtr;
    }
    if (OHOS::system::GetParameter("graphic.profiler.trace3d.enabled", "0") == "1" ||
        OHOS::system::GetParameter("persist.graphic.profiler.trace3d.enabled", "0") == "1") {
        apiTablePtr = TRACE3D_CoreInitImpl();
    }
    initDone = true;
    return apiTablePtr;
}

#include "transaction/rs_client_to_render_connection.h"
#include "transaction/rs_service_to_render_connection.h"

namespace OHOS::Rosen {

namespace {
// (user): Move to RSProfiler
static std::atomic<int32_t> g_renderServiceCpuId = 0;
static std::atomic<int32_t> g_renderServiceRenderCpuId = 0;
static uint64_t g_frameSyncTimestamp = 0u;
static uint64_t g_frameBeginTimestamp = 0u;
static uint64_t g_frameRenderBeginTimestamp = 0u;
static double g_dirtyRegionPercentage = 0.0;
static std::stringstream g_dirtyRegionList;
static bool g_rdcSent = true;
static uint64_t g_recordMinVsync = 0;
static uint64_t g_recordMaxVsync = 0;

static std::atomic<uint32_t> g_lastCacheImageCount = 0;

static RSFile g_recordFile {};
static std::atomic<double> g_recordStartTime = 0.0;
static std::atomic<uint32_t> g_frameNumber = 0;
static uint32_t g_renderFrameNumber = 0;

static RSFile g_playbackFile {};
static std::atomic<bool> g_playbackShouldBeTerminated = true;
static double g_playbackPauseTime = 0;
static int g_playbackWaitFrames = 0;
static std::atomic_bool g_playbackImmediate = false;
static std::atomic<double> g_replayLastPauseTimeReported = 0;

static std::vector<std::pair<NodeId, uint32_t>> g_nodeListPerf;
static std::unordered_map<NodeId, uint32_t> g_mapNode2Count;
static std::unordered_map<NodeId, uint32_t> g_mapNode2UpTime;
static std::unordered_map<NodeId, uint32_t> g_mapNode2UpDownTime;
static NodeId g_calcPerfNode = 0;
static uint32_t g_calcPerfNodeTry = 0;
static bool g_calcPerfNodeExcludeDown = false;

static std::vector<std::pair<std::shared_ptr<RSRenderNode>, std::vector<std::shared_ptr<RSRenderNode>>>>
    g_calcPerfSavedChildren;
static NodeId g_blinkNodeId = 0;
static uint32_t g_blinkNodeCount = 0;
static std::vector<std::shared_ptr<RSRenderNode>> g_blinkSavedParentChildren;

constexpr int CALC_PERF_NODE_TIME_COUNT_MIN = 128;  // min render tries
constexpr int CALC_PERF_NODE_TIME_COUNT_MAX = 4096; // max render tries
static uint32_t g_effectiveNodeTimeCount = CALC_PERF_NODE_TIME_COUNT_MAX;
static uint64_t g_calcPerfNodeTime[CALC_PERF_NODE_TIME_COUNT_MAX];
static int g_nodeListPerfCalcIndex = -1;

static std::string g_testDataFrame;
static std::atomic<uint32_t> g_recordParcelNumber = 0;
static std::unordered_map<std::string, std::string> g_recordRsMetric;
static std::mutex g_mutexFirstFrameMarshalling;
static std::mutex g_mutexJobMarshallingTick;
static std::atomic<uint32_t> g_jobTickTaskCount = 0;
static std::atomic<uint64_t> g_counterOnRemoteRequest = 0;

uint64_t ExtractTrace3DNumber(const std::string& str)
{
    size_t colonPos = str.find_last_of(':');
    if (colonPos == std::string::npos || colonPos + 1 >= str.length()) {
        return static_cast<uint64_t>(-1);
    }
    const char* start = str.c_str() + colonPos + 1;
    if (*start == '-') {
        return static_cast<uint64_t>(-1);
    }
    char* end = nullptr;
    errno = 0;
    uint64_t result = strtoull(start, &end, 10);
    if (errno != 0 || end == start || *end != '\0') {
        return static_cast<uint64_t>(-1);
    }
    return result;
}

Trace3DCoreParamValue CreateAndUpdateTraceParam(const std::vector<std::string>& args,
    const TRACE3D_CORE_API_TABLE* api, const std::string& flag,
    Trace3DCoreParamType pType, trace3d::api::ParamValueType vType)
{
    Trace3DCoreParamValue param{};

    auto it = std::find_if(args.begin(), args.end(),
        [&flag](const std::string& s) {
            return s.find(flag) != std::string::npos;
        });
    if (it != args.end()) {
        param.type = pType;
        param.valueType = vType;
        param.value[0].uint64 = ExtractTrace3DNumber(*it);
        if (param.value[0].uint64 != static_cast<uint64_t>(-1)) {
            if (api) {
                api->UpdateParam(&param);
            }
        }
    }

    return param;
}

void LoadAnimationTimestamps(std::unordered_map<AnimationId, std::vector<int64_t>>& map)
{
    for (const auto& [id, time] : g_playbackFile.GetAnimeStartTimes()) {
        const auto mockId = Utils::PatchNodeId(id);
        if (map.count(mockId)) {
            map[mockId].push_back(time);
        } else {
            map[mockId] = { time };
        }
    }
}

pid_t GetMainThreadId(RSMainThread* mainThread)
{
    pid_t tid = 0;
    if (mainThread) {
        mainThread->PostSyncTask([&tid]() { tid = gettid(); });
    }
    return tid;
}

template<class Thread>
void PostTask(Thread& thread, pid_t tid, std::function<void()>&& task, bool wait)
{
    if (task && wait) {
        tid == gettid() ? task() : (void)thread.PostSyncTask(std::move(task));
    } else if (task) {
        thread.PostTask(std::move(task));
    }
}

} // namespace

std::shared_ptr<RSRenderPipeline> RSProfiler::renderPipeline_;
RSMainThread* RSProfiler::mainThread_ = nullptr;
RSContext* RSProfiler::context_ = nullptr;
sptr<RSIServiceToRenderConnection> RSProfiler::serviceToRenderConnection_;
RSProfiler::ConnectionList RSProfiler::connections_;
std::mutex RSProfiler::connectionMutex_;

#pragma pack(push, 1)
struct AlignedMessageParcel {
    uint8_t pad = 0u;
    MessageParcel parcel;
};
#pragma pack(pop)

void RSProfiler::JobMarshallingKillPid(pid_t pid)
{
    // FirstFrameMarshalling can be executing same time resulting the crash
    g_mutexFirstFrameMarshalling.lock();
    std::vector<NodeId> list;
    {
        auto& jobGlobal = GetMarshallingJob();
        std::unique_lock<std::mutex> lockJob(g_mutexJobMarshallingTick);
        auto job = jobGlobal;
        if (!job) {
            return;
        }
        list = job->GetListForPid(pid);
    }
    for (auto nodeId : list) {
        // save without drawcmdmodifiers
        JobMarshallingTick(nodeId, true);
    }
}

void RSProfiler::JobMarshallingKillPidEnd()
{
    g_mutexFirstFrameMarshalling.unlock();
}

void RSProfiler::JobMarshallingTick(NodeId nodeId, bool skipDrawCmdModifiers)
{
    auto& jobGlobal = GetMarshallingJob();

    std::unique_lock<std::mutex> guard(g_mutexJobMarshallingTick); // JobMarshallingTick call from different threads
    auto job = jobGlobal;
    if (!job || (nodeId && !job->IsUnfinished(nodeId))) {
        return;
    }

    if (!nodeId) {
        nodeId = job->GetNextUnfinished();
        if (!nodeId) {
            // ERROR: no next NodeId
            return;
        }
    }

    const auto node = GetRenderNode(nodeId);
    if (!node) {
        job->MarkFinished(nodeId, {});
        return;
    }

    guard.unlock();
    std::stringstream data;
    MarshalNode(*node, data, RSFILE_VERSION_LATEST, skipDrawCmdModifiers, true);
    guard.lock();

    job->MarkFinished(nodeId, data.str());
    if (!job->GetUnfinishedCount()) {
        g_recordFile.SetHeaderFailedNodeCount(job->offsetNodeCount, job->failedNodeCount);
        g_recordFile.InsertHeaderData(job->offsetNodes, job->GetNodeData());
        jobGlobal = nullptr;
    }
}

std::shared_ptr<ProfilerMarshallingJob> RSProfiler::GetJobForExecution()
{
    auto& jobGlobal = GetMarshallingJob();
    std::shared_ptr<ProfilerMarshallingJob> job = nullptr;
    {
        std::unique_lock<std::mutex> lockJob(g_mutexJobMarshallingTick);
        job = jobGlobal;
    }
    if (job && job->GetUnfinishedCount() && job->marshallingTick) {
        return job;
    }
    return nullptr;
}

void RSProfiler::MarshalFirstFrameNodesLoop()
{
    constexpr auto waitTime = 100;
    while (auto job = GetJobForExecution()) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(waitTime));
        if (g_jobTickTaskCount) {
            continue;
        }
        g_jobTickTaskCount++;
        PostMainThreadTask([]() {
            g_jobTickTaskCount--;
            if (context_ && !context_->animatingNodeList_.empty()) {
                // don't execute tick during animation
                return;
            }
            // executes in main thread when mainLoop is sleeping
            auto job = GetJobForExecution();
            if (job) {
                job->marshallingTick(0, false);
            }
        });
    }
}

bool RSProfiler::IsPowerOffScreen()
{
    if (!context_) {
        HRPE("RSProfiler::IsPowerOffScreen context is nullptr");
        return false;
    }
    const auto& nodeMap = context_->GetNodeMap();
    if (nodeMap.screenNodeMap_.empty()) {
        HRPI("RSProfiler::IsPowerOffScreen has no screenRenderNode");
        return false;
    }
    bool hasScreenPowerOn = false;
    nodeMap.TraverseScreenNodes(
        [&hasScreenPowerOn](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
            if (!screenRenderNode) {
                return;
            }
            ScreenPowerStatus powerStatus = screenRenderNode->GetScreenProperty().GetScreenInfo().powerStatus;
            if (powerStatus != ScreenPowerStatus::POWER_STATUS_OFF &&
                powerStatus != ScreenPowerStatus::POWER_STATUS_SUSPEND) {
                hasScreenPowerOn = true;
            }
        }
    );
    return !hasScreenPowerOn;
}

void DeviceInfoToCaptureData(double time, const DeviceInfo& in, RSCaptureData& out)
{
    std::string frequency;
    std::string load;
    for (uint32_t i = 0; i < in.cpu.cores; i++) {
        frequency += std::to_string(in.cpu.coreFrequencyLoad[i].current);
        load += std::to_string(in.cpu.coreFrequencyLoad[i].load);
        if (i + 1 < in.cpu.cores) {
            frequency += ";";
            load += ";";
        }
    }

    out.SetTime(static_cast<float>(time));
    out.SetProperty(RSCaptureData::KEY_CPU_TEMP, in.cpu.temperature);
    out.SetProperty(RSCaptureData::KEY_CPU_CURRENT, in.cpu.current);
    out.SetProperty(RSCaptureData::KEY_CPU_LOAD, load);
    out.SetProperty(RSCaptureData::KEY_CPU_FREQ, frequency);
    out.SetProperty(RSCaptureData::KEY_GPU_LOAD, in.gpu.frequencyLoad.load);
    out.SetProperty(RSCaptureData::KEY_GPU_FREQ, in.gpu.frequencyLoad.current);
    out.SetProperty(RSCaptureData::KEY_CPU_ID, g_renderServiceRenderCpuId.load());
}

static pid_t GetPid(const std::shared_ptr<RSRenderNode>& node)
{
    return node ? Utils::ExtractPid(node->GetId()) : 0;
}

static NodeId GetNodeId(const std::shared_ptr<RSRenderNode>& node)
{
    return node ? Utils::ExtractNodeId(node->GetId()) : 0;
}

static void SendTelemetry(double time)
{
    if (time >= 0.0) {
        RSCaptureData captureData;
        DeviceInfoToCaptureData(time, RSTelemetry::GetDeviceInfo(), captureData);
        Network::SendCaptureData(captureData);
    }
}

/*
    To visualize the damage region (as it's set for KHR_partial_update), you can set the following variable:
    'hdc shell param set rosen.dirtyregiondebug.enabled 6'
*/
void RSProfiler::SetDirtyRegion(const Occlusion::Region& dirtyRegion)
{
    if (!IsRecording()) {
        return;
    }

    const double maxPercentValue = 100.0;
    g_dirtyRegionPercentage = maxPercentValue;

    if (!context_) {
        return;
    }
    std::shared_ptr<RSScreenRenderNode> screenNode = GetScreenNode(*context_);
    if (!screenNode) {
        return;
    }
    // the following logic calcuate the percentage of dirtyRegion
    auto params = static_cast<RSScreenRenderParams*>(screenNode->GetRenderParams().get());
    if (!params) {
        return;
    }
    auto screenInfo = params->GetScreenInfo();
    const uint64_t displayArea = static_cast<uint64_t>(screenInfo.width * screenInfo.height);

    auto rects = RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    uint64_t dirtyRegionArea = 0;
    g_dirtyRegionList.str("");
    for (const auto& rect : rects) {
        dirtyRegionArea += static_cast<uint64_t>(rect.GetWidth() * rect.GetHeight());
        int32_t value = rect.GetLeft();
        g_dirtyRegionList.write(reinterpret_cast<const char*>(&value), sizeof(value));
        value = rect.GetTop();
        g_dirtyRegionList.write(reinterpret_cast<const char*>(&value), sizeof(value));
        value = rect.GetWidth();
        g_dirtyRegionList.write(reinterpret_cast<const char*>(&value), sizeof(value));
        value = rect.GetHeight();
        g_dirtyRegionList.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    if (displayArea > 0) {
        g_dirtyRegionPercentage =
            maxPercentValue * static_cast<double>(dirtyRegionArea) / static_cast<double>(displayArea);
    }
    if (g_dirtyRegionPercentage > maxPercentValue) {
        g_dirtyRegionPercentage = maxPercentValue;
    }
}

void RSProfiler::Init(const std::shared_ptr<RSRenderPipeline>& renderPipeline,
    const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection)
{
    renderPipeline_ = renderPipeline;
    mainThread_ = renderPipeline ? renderPipeline->mainThread_ : nullptr;
    context_ = mainThread_ ? mainThread_->context_.get() : nullptr;
    serviceToRenderConnection_ = serviceToRenderConnection;
    trace3dApi_ = Trace3DCoreInitRS();

    mainThreadTaskRunner_ = [tid = GetMainThreadId(mainThread_)](std::function<void()>&& task, bool wait) {
        if (mainThread_) {
            PostTask(*mainThread_, tid, std::move(task), wait);
        }
    };

    renderThreadTaskRunner_ = [](std::function<void()>&& task, bool wait) {
        PostTask(RSUniRenderThread::Instance(), RSUniRenderThread::Instance().GetTid(), std::move(task), wait);
    };

    messageSender_ = Network::SendMessage;

    RSSystemProperties::SetProfilerDisabled();
    RSSystemProperties::WatchSystemProperty(SYS_KEY_ENABLED, OnFlagChangedCallback, nullptr);
    RSSystemProperties::WatchSystemProperty(SYS_KEY_BETARECORDING, OnFlagChangedCallback, nullptr);
    bool isEnabled = RSSystemProperties::GetProfilerEnabled();
    bool isBetaRecord = RSSystemProperties::GetBetaRecordingMode() != 0;
    HRPI("Profiler flags changed enabled=%{public}d beta_record=%{public}d", isEnabled ? 1 : 0, isBetaRecord ? 1 : 0);

    if (!isBetaRecord) {
        ClearBetaRecordFiles();
    }
}

void RSProfiler::StartNetworkThread()
{
    if (!Network::IfThreadCanBeStarted()) {
        return;
    }
    auto networkRunLambda = []() {
        Network::Run();
    };
    std::thread thread(networkRunLambda);
    thread.detach();
}

void RSProfiler::OnCreateConnection(pid_t pid)
{
    if (!IsEnabled()) {
        return;
    }

    if (IsRecording()) {
        g_recordFile.AddHeaderPid(pid);
    }
}

uint64_t RSProfiler::WriteRemoteRequest(pid_t pid, uint32_t code, MessageParcel& parcel, MessageOption& option)
{
    const double deltaTime = Now() - g_recordStartTime;

    std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);

    char headerType = 1; // parcel data
    stream.write(reinterpret_cast<const char*>(&headerType), sizeof(headerType));
    stream.write(reinterpret_cast<const char*>(&deltaTime), sizeof(deltaTime));

    // set sending pid
    stream.write(reinterpret_cast<const char*>(&pid), sizeof(pid));
    stream.write(reinterpret_cast<const char*>(&code), sizeof(code));

    const size_t dataSize = parcel.GetDataSize();
    stream.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
    stream.write(reinterpret_cast<const char*>(parcel.GetData()), dataSize);

    const int32_t flags = option.GetFlags();
    stream.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
    const int32_t waitTime = option.GetWaitTime();
    stream.write(reinterpret_cast<const char*>(&waitTime), sizeof(waitTime));
    g_recordParcelNumber++;
    stream.write(reinterpret_cast<const char*>(&g_recordParcelNumber), sizeof(g_recordParcelNumber));

    const std::string out = stream.str();
    constexpr size_t headerOffset = 8 + 1;
    if (out.size() >= headerOffset) {
        g_recordFile.AddHeaderPid(pid);
        g_recordFile.WriteRSData(deltaTime, out.data() + headerOffset, out.size() - headerOffset);
        BetaRecordSetLastParcelTime();
    }
    Network::SendBinary(out.data(), out.size());
    return g_recordParcelNumber;
}

static bool IsAshmemParcel(uint32_t code, const MessageParcel& parcel)
{
    if (code == static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::COMMIT_TRANSACTION)) {
        int32_t flag = 0;
        return parcel.GetReadableBytes() > sizeof(flag) &&
               Utils::Move(&flag, sizeof(flag), reinterpret_cast<const void*>(parcel.GetData()), sizeof(flag)) && flag;
    }
    return false;
}

uint64_t RSProfiler::ProcessRemoteRequest(pid_t pid, uint32_t code, MessageParcel& parcel, MessageOption& option)
{
    g_counterOnRemoteRequest++;
    if (!IsEnabled()) {
        return 0;
    }

    if (IsRecording()) {
        return !IsAshmemParcel(code, parcel) ? WriteRemoteRequest(pid, code, parcel, option) : 0;
    }

    g_recordParcelNumber = 0;
    return 0;
}

uint64_t RSProfiler::OnRemoteRequest(RSClientToRenderConnectionStub* connection, uint32_t code,
    MessageParcel& parcel, MessageParcel& reply, MessageOption& option)
{
    return connection ? ProcessRemoteRequest(connection->GetCallingPid(), code, parcel, option) : 0u;
}

uint64_t RSProfiler::OnRemoteRequest(RSClientToServiceConnectionStub* connection, uint32_t code, MessageParcel& parcel,
    MessageParcel& reply, MessageOption& option)
{
    return 0;
}

uint64_t RSProfiler::OnRemoteRequest(RSServiceToRenderConnectionStub* connection, uint32_t code, MessageParcel& parcel,
    MessageParcel& reply, MessageOption& option)
{
    return ProcessRemoteRequest(0u, code, parcel, option);
}

pid_t RSProfiler::GetCallingPid(RSClientToRenderConnectionStub* connection)
{
    const auto pid = connection ? connection->GetCallingPid() : 0;
    if (!IsEnabled() || !IsPlaying()) {
        return pid;
    }

    const std::lock_guard<std::mutex> guard(connectionMutex_);
    const auto found = std::find_if(connections_.begin(), connections_.end(),
        [&connection](const auto& pair) { return pair.second.GetRefPtr() == connection; });
    return (found != connections_.end()) ? found->first : pid;
}

void RSProfiler::CreateMockConnection(pid_t pid)
{
    if (!IsEnabled() || !renderPipeline_ || GetMockConnection(pid)) {
        return;
    }

    constexpr auto needRefresh = false;
    const auto token = new IRemoteStub<RSIConnectionToken>();
    if (!token) {
        HRPE("%{public}s: Cannot create connection token: %d", __func__, pid);
        return;
    }

    const auto connection =
        new RSClientToRenderConnection(pid, new RSRenderPipelineAgent(renderPipeline_), token->AsObject(), needRefresh);
    if (!connection) {
        HRPE("%{public}s: Cannot create mock connection: %d", __func__, pid);
        return;
    }

    renderPipeline_->AddTransactionDataPidInfo(pid);
    const std::lock_guard<std::mutex> guard(connectionMutex_);
    connections_[pid] = connection;
}

void RSProfiler::PurgeMockConnections()
{
    const std::lock_guard<std::mutex> guard(connectionMutex_);
    // Make the connections destruction threaded due to its extreme cost
    std::thread([connections = connections_]() {}).detach();
    connections_.clear();
}

sptr<IRemoteObject> RSProfiler::GetMockConnection(pid_t pid)
{
    if (Utils::GetMockPid(0) == pid) {
        return serviceToRenderConnection_ ? serviceToRenderConnection_->AsObject() : nullptr;
    }

    const std::lock_guard<std::mutex> guard(connectionMutex_);
    const auto found = connections_.find(pid);
    return (found != connections_.end()) ? found->second->AsObject() : nullptr;
}

void RSProfiler::OnFlagChangedCallback(const char *key, const char *value, void *context)
{
    constexpr int8_t two = 2;
    if (!strcmp(key, SYS_KEY_ENABLED)) {
        signalFlagChanged_ = two;
        AwakeRenderServiceThread();
    }
    if (!strcmp(key, SYS_KEY_BETARECORDING)) {
        signalFlagChanged_ = two;
        AwakeRenderServiceThread();
    }
}

void RSProfiler::OnWorkModeChanged()
{
    if (IsEnabled()) {
        if (IsBetaRecordEnabled()) {
            HRPD("RSProfiler: Stop network. Start beta-recording.");
            Network::Stop();
            StartBetaRecord();
        } else {
            StopBetaRecord();
            StartNetworkThread();
            HRPD("RSProfiler: Stop beta-recording (if running). Start network.");
        }
    } else {
        HRPD("RSProfiler: Stop recording. Stop network.");
        StopBetaRecord();
        Network::Stop();
        RecordStop(ArgList());
        PlaybackStop(ArgList());

        g_playbackFile.Close();
        if (GetMode() == Mode::NONE) {
            Utils::FileDelete(RSFile::GetDefaultPath());
        }
    }
}

void RSProfiler::ProcessSignalFlag()
{
    if (signalFlagChanged_ <= 0) {
        return;
    }

    signalFlagChanged_--;
    if (!signalFlagChanged_) {
        if (GetMode() == Mode::SAVING) {
            constexpr int8_t flag = 2;
            signalFlagChanged_ = flag;
            return;
        }
        bool newEnabled = RSSystemProperties::GetProfilerEnabled();
        bool newBetaRecord = RSSystemProperties::GetBetaRecordingMode() != 0;
        HRPD("Profiler flags changed enabled=%{public}d beta_record=%{public}d", newEnabled ? 1 : 0,
            newBetaRecord ? 1 : 0);
        if (enabled_ && !newEnabled) {
            StopBetaRecord();
            RecordStop(ArgList());
            PlaybackStop(ArgList());
        }
        if (enabled_ != newEnabled || IsBetaRecordEnabled() != newBetaRecord) {
            enabled_ = newEnabled;
            betaRecordingEnabled_ = newBetaRecord;
            RSCaptureRecorder::GetInstance().SetProfilerEnabled(enabled_);
            OnWorkModeChanged();
        }
    }
    AwakeRenderServiceThread();
}

void RSProfiler::OnProcessCommand()
{
    RS_TRACE_NAME("Profiler OnProcessCommand");
    ProcessSignalFlag();

    if (!IsEnabled()) {
        return;
    }

    if (IsPlaying()) {
        ResetAnimationStamp();
    }
}

bool RSProfiler::IsSecureScreen()
{
    const auto screenNode = context_ ? GetScreenNode(*context_) : nullptr;
    if (!screenNode) {
        return false;
    }
    for (auto& child : *screenNode->GetChildren()) {
        auto displayNode = child->ReinterpretCastTo<RSLogicalDisplayRenderNode>();
        if (!displayNode) {
            continue;
        }
        if (displayNode->GetMultableSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY)) {
            return true;
        }
    }
    return false;
}

void RSProfiler::OnRenderBegin()
{
    if (!IsEnabled()) {
        return;
    }

    RS_TRACE_NAME("Profiler OnRenderBegin");
    HRPD("OnRenderBegin()");
    g_renderServiceCpuId = Utils::GetCpuId();

    ProcessCommands();
    ProcessSendingRdc();
}

void RSProfiler::OnRenderEnd()
{
    if (!IsEnabled()) {
        return;
    }

    RS_TRACE_NAME("Profiler OnRenderEnd");
    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::OnParallelRenderBegin(uint32_t renderFrameNumber)
{
    if (!IsEnabled()) {
        return;
    }

    g_renderFrameNumber = renderFrameNumber;

    if (g_calcPerfNode > 0) {
        // force render thread to be on fastest CPU
        constexpr uint32_t spamCount = 10'000;
        std::vector<int> data;
        data.resize(spamCount);
        for (uint32_t i = 0; i < spamCount; i++) {
            for (uint32_t j = i + 1; j < spamCount; j++) {
                std::swap(data[i], data[j]);
            }
        }

        constexpr uint32_t trashCashStep = 64;
        constexpr uint32_t newCount = spamCount * trashCashStep;
        constexpr int trashNum = 0x7F;
        data.resize(newCount);
        for (uint32_t i = 0; i < spamCount; i++) {
            data[i * trashCashStep] = trashNum;
        }
    }

    g_frameRenderBeginTimestamp = RawNowNano();
}

void RSProfiler::OnParallelRenderEnd(uint32_t frameNumber)
{
    g_renderServiceRenderCpuId = Utils::GetCpuId();
    const uint64_t frameLengthNanosecs = RawNowNano() - g_frameRenderBeginTimestamp;
    CalcNodeWeigthOnFrameEnd(frameLengthNanosecs);

    if (!IsRecording()) {
        return;
    }

    const double currentTime = Utils::ToSeconds(g_frameRenderBeginTimestamp);
    const double timeSinceRecordStart = currentTime - g_recordStartTime;

    if (timeSinceRecordStart > 0.0) {
        RSCaptureData captureData;
        captureData.SetTime(timeSinceRecordStart);
        captureData.SetProperty(RSCaptureData::KEY_RENDER_FRAME_NUMBER, frameNumber);
        captureData.SetProperty(RSCaptureData::KEY_RENDER_FRAME_LEN, frameLengthNanosecs);

        std::vector<char> out;
        DataWriter archive(out);
        char headerType = static_cast<char>(PackageID::RS_PROFILER_RENDER_METRICS);
        archive.Serialize(headerType);
        captureData.Serialize(archive);

        Network::SendBinary(out.data(), out.size());
        g_recordFile.WriteRenderMetrics(0, timeSinceRecordStart, out.data(), out.size());
    }
}

std::shared_ptr<trace3d::api::DebugScope> RSProfiler::Trace3DDebugScopeCreate(uint64_t rsNodeId)
{
    if (!trace3dApi_) {
        return nullptr;
    }

    auto rsFrame = g_renderFrameNumber;

    Trace3DCoreDebugTagParamValue dbgParam[2] = {};
    dbgParam[0].type = TRACE3D_CORE_DEBUG_TAG_PARAM_WM_NODE;
    dbgParam[0].wmNode.id = rsNodeId;
    dbgParam[1].type = TRACE3D_CORE_DEBUG_TAG_PARAM_WM_FRAME;
    dbgParam[1].wmFrame.number = (uint64_t)rsFrame;

    auto dbgScope =
        std::make_shared<trace3d::api::DebugScope>(trace3dApi_, "", TRACE3D_DEBUG_TAG_POOL_GPU_ZONE, dbgParam[0]);
    dbgScope->SetTagParam(dbgParam[1]);
    return dbgScope;
}

bool RSProfiler::ShouldBlockHWCNode()
{
    return IsEnabled() && IsReadMode();
}

void RSProfiler::OnFrameBegin(uint64_t syncTime)
{
    if (!IsEnabled()) {
        return;
    }

    RS_TRACE_NAME("Profiler OnFrameBegin");
    g_frameSyncTimestamp = syncTime;
    g_frameBeginTimestamp = RawNowNano();
    g_renderServiceCpuId = Utils::GetCpuId();
    g_frameNumber++;

    LogEventVSync(syncTime);

    BetaRecordOnFrameBegin();
    StartBetaRecord();
}

void RSProfiler::ProcessPauseMessage()
{
    if (!IsPlaying()) {
        return;
    }

    const uint64_t pauseAtTime = TimePauseGet();
    const uint64_t nowTime = RawNowNano();
    if (pauseAtTime > 0 && nowTime > pauseAtTime) {
        const double deltaTime = PlaybackDeltaTime();
        if (deltaTime > g_replayLastPauseTimeReported) {
            int64_t vsyncId = g_playbackFile.ConvertTime2VsyncId(deltaTime);
            if (vsyncId) {
                SendMessage("Replay timer paused vsyncId=%" PRId64, vsyncId); // RESERVED!
            }
            g_replayLastPauseTimeReported = deltaTime;
        }
    }
}

void RSProfiler::OnFrameEnd()
{
    // must be always called mutex must be freed inside no matter happens
    BetaRecordOnFrameEnd();

    if (!IsEnabled()) {
        return;
    }

    RS_TRACE_NAME("Profiler OnFrameEnd");
    g_renderServiceCpuId = Utils::GetCpuId();
    RecordUpdate();

    UpdateDirtyRegionBetaRecord(g_dirtyRegionPercentage);
    if (context_) {
        UpdateBetaRecord(*context_);
    }
    BlinkNodeUpdate();
    CalcPerfNodeUpdate();

    ProcessPauseMessage();

    g_renderServiceCpuId = Utils::GetCpuId();

    if (IsRecordingMode()) {
        constexpr uint32_t maxCount = 32;
        for (uint32_t i = 0; i < maxCount; i++) {
            const auto message = ReceiveRSLogBase();
            if (!message.msg_.length()) {
                break;
            }
            LogEventMsg(message.time_, message.type_, message.msg_);
        }
    }

    BetaRecordOnFrameEnd();
}

void RSProfiler::CalcNodeWeigthOnFrameEnd(uint64_t frameLength)
{
    if (g_calcPerfNode == 0 || g_calcPerfNodeTry >= CALC_PERF_NODE_TIME_COUNT_MAX) {
        return;
    }

    g_calcPerfNodeTime[g_calcPerfNodeTry] = frameLength;
    g_calcPerfNodeTry++;
    if (g_calcPerfNodeTry < g_effectiveNodeTimeCount) {
        AwakeRenderServiceThread();
        return;
    }

    std::sort(g_calcPerfNodeTime, g_calcPerfNodeTime + g_effectiveNodeTimeCount);

    const uint16_t startCnt = g_effectiveNodeTimeCount / 4;
    const uint16_t endCnt = g_effectiveNodeTimeCount - startCnt;
    uint64_t avgValue = 0;
    for (auto i = startCnt; i < endCnt; i++) {
        avgValue += g_calcPerfNodeTime[i];
    }
    avgValue /= endCnt - startCnt;

    if (g_calcPerfNodeExcludeDown) {
        g_mapNode2UpTime[g_calcPerfNode] = avgValue;
    } else {
        g_mapNode2UpDownTime[g_calcPerfNode] = avgValue;
    }

    constexpr float nanoToMs = 1000'000.0f;
    SendMessage("CALC_PERF_NODE_RESULT: %s->%lu %lu inMS=%f tries=%d", g_calcPerfNodeExcludeDown ? "U" : "UD",
        g_calcPerfNode, avgValue, static_cast<float>(avgValue) / nanoToMs, g_effectiveNodeTimeCount);
    g_calcPerfNode = 0;

    AwakeRenderServiceThread();
    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::RenderServiceTreeDump(JsonWriter& out, pid_t pid)
{
    RS_TRACE_NAME("GetDumpTreeJSON");

    if (!context_) {
        return;
    }

    const bool useMockPid = (pid > 0) && IsReadMode();
    if (useMockPid) {
        pid = Utils::GetMockPid(pid);
    }

    auto& animation = out["Animation Node"];
    animation.PushArray();
    for (auto& [nodeId, _] : context_->animatingNodeList_) {
        if (!pid) {
            animation.Append(nodeId);
        } else if (pid == Utils::ExtractPid(nodeId)) {
            animation.Append(nodeId);
        }
    }
    animation.PopArray();

    auto rootNode = context_->GetGlobalRootRenderNode();
    if (pid) {
        rootNode = nullptr;
        auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
        nodeMap.TraversalNodes([&rootNode, pid](const std::shared_ptr<RSBaseRenderNode>& node) {
            if (!node || !node->GetSortedChildren()) {
                return;
            }
            auto parentPtr = node->GetParent().lock();
            if (parentPtr != nullptr && !rootNode &&
                Utils::ExtractPid(node->GetId()) != Utils::ExtractPid(parentPtr->GetId()) &&
                Utils::ExtractPid(node->GetId()) == pid && node->GetSortedChildren()->size() > 0) {
                rootNode = node;
                SendMessage("Root node found: %lu", rootNode->GetId());
            }
        });
    }

    auto& root = out["Root node"];
    if (rootNode) {
        DumpNode(*rootNode, root, useMockPid, pid > 0);
    } else {
        SendMessage("rootNode not found");
        root.PushObject();
        root.PopObject();
    }

    if (context_) {
        auto& rootOffscreen = out["Offscreen node"];
        DumpOffscreen(*context_, rootOffscreen, useMockPid, pid);
    }
}

uint64_t RSProfiler::RawNowNano()
{
    return Utils::Now();
}

uint64_t RSProfiler::NowNano()
{
    return PatchTime(RawNowNano());
}

double RSProfiler::Now()
{
    return Utils::ToSeconds(NowNano());
}

bool RSProfiler::IsRecording()
{
    return IsEnabled() && IsWriteMode() && g_recordFile.IsOpen() && (g_recordStartTime > 0);
}

bool RSProfiler::IsPlaying()
{
    return IsEnabled() && IsReadMode() && g_playbackFile.IsOpen() && !g_playbackShouldBeTerminated;
}

void RSProfiler::RequestNextVSync()
{
    if (mainThread_) {
        mainThread_->RequestNextVSync();
        PostMainThreadTask([]() { mainThread_->RequestNextVSync(); });
    }
}

void RSProfiler::AwakeRenderServiceThread()
{
    if (!mainThread_) {
        return;
    }
    mainThread_->RequestNextVSync();
    mainThread_->SetAccessibilityConfigChanged();
    mainThread_->SetDirtyFlag();
    PostMainThreadTask([]() {
        mainThread_->SetAccessibilityConfigChanged();
        mainThread_->SetDirtyFlag();
        mainThread_->RequestNextVSync();
    });
}

void RSProfiler::ResetAnimationStamp()
{
    if (mainThread_ && context_) {
        mainThread_->lastAnimateTimestamp_ = context_->GetCurrentTimestamp();
    }
}

std::shared_ptr<RSRenderNode> RSProfiler::GetRenderNode(uint64_t id)
{
    return context_ ? context_->GetMutableNodeMap().GetRenderNode(id) : nullptr;
}

bool RSProfiler::IsLoadSaveFirstScreenInProgress()
{
    return IsWriteEmulationMode() || IsReadEmulationMode();
}

void RSProfiler::MarshalSelfDrawingNodes()
{
    class SurfaceCaptureCallback final : public RSISurfaceCaptureCallback {
        void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig, Media::PixelMap* pixelmap,
            CaptureError captureErrorCode, Media::PixelMap* pixelmapHDR) override
        {
            if (pixelmap) {
                PixelMapStorage::Push(Utils::PatchSelfDrawingImageId(id), *pixelmap);
            }
        }

        sptr<IRemoteObject> AsObject() override
        {
            return nullptr;
        }
    };

    if (!context_) {
        return;
    }

    context_->GetNodeMap().TraverseSurfaceNodes([](const std::shared_ptr<RSSurfaceRenderNode>& node) {
        if (!node || !node->IsSelfDrawingType() || node->GetAbsRect().IsEmpty()) {
            return;
        }

        constexpr auto useCurrentWindow = false;
        bool needSyncSurface = false;
        RSSurfaceCaptureTaskParallel::CheckModifiers(node->GetId(), useCurrentWindow, &needSyncSurface);

        RSSurfaceCaptureParam param;
        param.id = node->GetId();
        param.isSystemCalling = false;
        param.isSelfCapture = true;
        param.config.useDma = true;
        param.config.useCurWindow = useCurrentWindow;
        param.config.isHdrCapture = false;
        param.hasDirtyContentInSurfaceCapture = (needSyncSurface || node->IsDirty() || node->IsSubTreeDirty());
        RSSurfaceCaptureTaskParallel::Capture(sptr<SurfaceCaptureCallback>::MakeSptr(), param);
    });
}

void RSProfiler::UnmarshalSelfDrawingNodes()
{
    if (!context_) {
        return;
    }

    context_->GetNodeMap().TraverseSurfaceNodes([](const std::shared_ptr<RSSurfaceRenderNode>& node) {
        if (!node || !node->IsSelfDrawingType() || node->GetAbsRect().IsEmpty() ||
            !Utils::IsNodeIdPatched(node->GetId())) {
            return;
        }

        const auto handler = node->GetRSSurfaceHandler();
        if (!handler) {
            HRPE("UnmarshalSelfDrawingNode: Invalid surface handler: %{public}" PRId64, node->GetId());
            return;
        }

        const auto buffer = SurfaceBuffer::Create();
        if (!buffer || !PixelMapStorage::Pull(Utils::PatchSelfDrawingImageId(node->GetId()), *buffer)) {
            HRPE("UnmarshalSelfDrawingNode: Invalid surface buffer: %{public}" PRId64, node->GetId());
            return;
        }

        const Rect region {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        };
        handler->SetBuffer(buffer, SyncFence::InvalidFence(), region, 0, nullptr);
        node->UpdateBufferInfo(handler->GetBuffer(), handler->GetBufferOwnerCount(), region, handler->GetAcquireFence(),
            handler->GetPreBuffer(), handler->GetPreBufferOwnerCount());
        node->SetNodeDirty(true);
        node->SetContentDirty();
    });
}

static bool WriteFocusAppInfo(std::stringstream& stream, const FocusAppInfo& info)
{
    stream.write(reinterpret_cast<const char*>(&info.pid), sizeof(info.pid));
    stream.write(reinterpret_cast<const char*>(&info.uid), sizeof(info.uid));
    stream.write(reinterpret_cast<const char*>(&info.focusNodeId), sizeof(info.focusNodeId));

    size_t size = info.bundleName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(info.bundleName.data(), static_cast<std::streamsize>(size));

    size = info.abilityName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(info.abilityName.data(), static_cast<std::streamsize>(size));
    return !!stream;
}

std::string RSProfiler::FirstFrameMarshalling(uint32_t fileVersion, bool betaRecord)
{
    if (!mainThread_ || !context_) {
        HRPE("FirstFrameMarshalling: Invalid main thread or context");
        return {};
    }

    RS_TRACE_NAME("Profiler FirstFrameMarshalling");
    std::stringstream stream;
    stream.exceptions(0);

    TypefaceMarshalling(stream, fileVersion);
    if (!stream) {
        HRPE("FirstFrameMarshalling: TypefaceMarshalling failed");
        return {};
    }

    const auto job = betaRecord ? std::make_shared<ProfilerMarshallingJob>(JobMarshallingTick) : nullptr;
    MarshalNodes(*context_, stream, fileVersion, job);
    if (!stream) {
        HRPE("FirstFrameMarshalling: MarshalNodes failed");
        return {};
    }

    if (!betaRecord) {
        MarshalSelfDrawingNodes();
    }

    const FocusAppInfo info {
        .pid = mainThread_->focusAppPid_,
        .uid = mainThread_->focusAppUid_,
        .bundleName = mainThread_->focusAppBundleName_,
        .abilityName = mainThread_->focusAppAbilityName_,
        .focusNodeId = mainThread_->focusNodeId_,
    };
    if (!WriteFocusAppInfo(stream, info)) {
        HRPE("FirstFrameMarshalling: Cannot write focus app info");
        return {};
    }
    return stream.str();
}

static bool ReadFocusAppInfo(std::stringstream& stream, FocusAppInfo& info)
{
    constexpr size_t maxSize = 4096u;
    if (!stream.read(reinterpret_cast<char*>(&info.pid), sizeof(info.pid)) ||
        !stream.read(reinterpret_cast<char*>(&info.uid), sizeof(info.uid)) ||
        !stream.read(reinterpret_cast<char*>(&info.focusNodeId), sizeof(info.focusNodeId))) {
        return false;
    }

    info.pid = Utils::GetMockPid(info.pid);
    info.focusNodeId = Utils::PatchNodeId(info.focusNodeId);

    size_t size = 0u;
    if (!stream.read(reinterpret_cast<char*>(&size), sizeof(size)) || (size > maxSize)) {
        return false;
    }

    info.bundleName.resize(size);
    if (!stream.read(info.bundleName.data(), static_cast<std::streamsize>(info.bundleName.size()))) {
        return false;
    }

    size = 0u;
    if (!stream.read(reinterpret_cast<char*>(&size), sizeof(size)) || (size > maxSize)) {
        return false;
    }

    info.abilityName.resize(size);
    return !!stream.read(info.abilityName.data(), static_cast<std::streamsize>(info.abilityName.size()));
}

std::string RSProfiler::FirstFrameUnmarshalling(const std::string& data, uint32_t fileVersion)
{
    if (!mainThread_ || !context_) {
        return "FirstFrameUnmarshalling: Invalid mainThread or context";
    }

    std::stringstream stream(data);
    auto error = TypefaceUnmarshalling(stream, fileVersion);
    if (!error.empty()) {
        return error;
    }

    SetThreadSubMode(SubMode::READ_EMUL);
    DisableThreadSharedMemory();
    error = UnmarshalNodes(*context_, stream, fileVersion);
    if (error.empty()) {
        UnmarshalSelfDrawingNodes();
    }
    EnableThreadSharedMemory();
    SetThreadSubMode(SubMode::NONE);

    FocusAppInfo info;
    if (error.empty() && !ReadFocusAppInfo(stream, info)) {
        FilterMockNode(*context_);
        error = "FirstFrameUnmarshalling: Cannot read focus app info";
    }

    if (error.empty()) {
        mainThread_->SetFocusAppInfo(info);
    } else {
        RSTypefaceCache::Instance().ReplayClear();
    }
    return error;
}

void RSProfiler::TypefaceMarshalling(std::stringstream& stream, uint32_t fileVersion)
{
    if (fileVersion >= RSFILE_VERSION_RENDER_TYPEFACE_FIX) {
        const size_t flags = !IsBetaRecordEnabled();
        stream.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
        if (!IsBetaRecordEnabled()) {
            RSTypefaceCache::Instance().ReplaySerialize(stream);
        }
    }
}

std::string RSProfiler::TypefaceUnmarshalling(std::stringstream& stream, uint32_t fileVersion)
{
    if (fileVersion >= RSFILE_VERSION_RENDER_TYPEFACE_FIX) {
        size_t flags = 0u;
        if (!stream.read(reinterpret_cast<char*>(&flags), sizeof(flags))) {
            return "TypefaceUnmarshalling: Cannot read flags";
        }

        if (flags > 0) {
            return RSTypefaceCache::Instance().ReplayDeserialize(stream);
        }
    }
    return "";
}

void RSProfiler::PrintVsync(const ArgList& args)
{
    if (!g_playbackFile.IsOpen()) {
        SendMessage("Replay file wasn't opened");
        return;
    }

    std::string out("VSYNC");
    for (const auto& id : g_playbackFile.GetVsyncList()) {
        out += " " + std::to_string(id);
    }
    SendMessage("%s", out.data());
}

void RSProfiler::PrintTime(const ArgList& args)
{
    if (!g_playbackFile.IsOpen()) {
        SendMessage("File is not open");
    } else {
        const auto [start, end] = g_playbackFile.GetStartAndEndTime();
        SendMessage("Start: %f End: %f", start, end);
    }
}

std::vector<std::shared_ptr<RSLogicalDisplayRenderNode>> RSProfiler::GetLogicalDisplayNodes()
{
    if (!context_) {
        return {};
    }
    std::vector<std::shared_ptr<RSLogicalDisplayRenderNode>> displays;
    context_->GetNodeMap().TraverseLogicalDisplayNodes(
        [&displays](const std::shared_ptr<RSLogicalDisplayRenderNode>& display) {
            if (display) {
                displays.push_back(display);
            }
        });

    std::sort(displays.begin(), displays.end(),
        [](const std::shared_ptr<RSLogicalDisplayRenderNode>& a, const std::shared_ptr<RSLogicalDisplayRenderNode>& b) {
            return a->GetScreenId() < b->GetScreenId();
        });
    return displays;
}

void RSProfiler::HiddenSpaceTurnOn()
{
    if (IsHiddenSpaceEnabled()) {
        HiddenSpaceTurnOff();
    }

    std::vector<RSRenderNode::SharedPtr> mockDisplays;
    if (const auto mockRoot = GetRenderNode(Utils::PatchNodeId(0))) {
        for (const auto& child : mockRoot->GetChildrenList()) {
            const auto screen = child.lock();
            const auto display = screen ? screen->GetFirstChild() : nullptr;
            if (display && display->GetChildrenCount()) {
                mockDisplays.push_back(display);
            }
        }
    }

    if (mockDisplays.empty()) {
        HRPE("HiddenSpaceTurnOn: Mock logical displays not found");
        return;
    }

    const auto displays = GetLogicalDisplayNodes();
    if (displays.empty()) {
        HRPE("HiddenSpaceTurnOn: Logical displays not found");
        return;
    }

    for (uint32_t i = 0; i < std::min(mockDisplays.size(), displays.size()); i++) {
        const auto& display = displays[i];
        const auto children = display->GetChildren();
        displayChildren_[display] = children ? *children : std::vector<RSRenderNode::SharedPtr> {};
        display->ClearChildren();
        for (const auto& node : mockDisplays[i]->GetChildrenList()) {
            display->AddChild(node.lock());
        }
    }

    AwakeRenderServiceThread();
}

void RSProfiler::HiddenSpaceTurnOff()
{
    if (!context_) {
        return;
    }
    for (const auto& [display, children] : displayChildren_) {
        display->ClearChildren();
        for (const auto& child : children) {
            display->AddChild(child);
        }
    }
    displayChildren_.clear();

    FilterMockNode(*context_);
    RSTypefaceCache::Instance().ReplayClear();

    AwakeRenderServiceThread();
}

bool RSProfiler::IsHiddenSpaceEnabled()
{
    return !displayChildren_.empty();
}

void RSProfiler::SaveRdc(const ArgList& args)
{
    g_rdcSent = false;
    RSSystemProperties::SetSaveRDC(true);
    RSSystemProperties::SetInstantRecording(true);

    AwakeRenderServiceThread();
    SendMessage("Recording current frame cmds (for .rdc) into : /data/default.drawing");
}

void RSProfiler::ProcessSendingRdc()
{
    if (!RSSystemProperties::GetSaveRDC() || g_rdcSent) {
        return;
    }
    AwakeRenderServiceThread();

    if (!RSCaptureRecorder::PullAndSendRdc()) {
        return;
    }
    RSSystemProperties::SetSaveRDC(false);
    g_rdcSent = true;
}

static uint32_t GetImagesAdded()
{
    const size_t count = ImageCache::Size();
    const uint32_t added = count - g_lastCacheImageCount;
    g_lastCacheImageCount = count;
    return added;
}

void RSProfiler::WriteRSMetricsToRecordFile(double timeSinceRecordStart, double syncTime, uint64_t frameLen)
{
    RSCaptureData captureData;
    captureData.SetTime(timeSinceRecordStart);
    captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_NUMBER, g_frameNumber.load());
    captureData.SetProperty(RSCaptureData::KEY_RS_SYNC_TIME, syncTime);
    captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_LEN, frameLen);
    captureData.SetProperty(RSCaptureData::KEY_RS_CMD_COUNT, GetCommandCount());
    captureData.SetProperty(RSCaptureData::KEY_RS_CMD_EXECUTE_COUNT, GetCommandExecuteCount());
    captureData.SetProperty(RSCaptureData::KEY_RS_PARCEL_CMD_LIST, GetParcelCommandList());
    captureData.SetProperty(RSCaptureData::KEY_RS_PIXEL_IMAGE_ADDED, GetImagesAdded());
    captureData.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION, floor(g_dirtyRegionPercentage));
    captureData.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION_LIST, g_dirtyRegionList.str());
    captureData.SetProperty(RSCaptureData::KEY_RS_CPU_ID, g_renderServiceCpuId.load());
    const uint64_t vsyncId = mainThread_ ? mainThread_->vsyncId_ : 0;
    captureData.SetProperty(RSCaptureData::KEY_RS_VSYNC_ID, vsyncId);

    if (!g_recordMinVsync) {
        g_recordMinVsync = vsyncId;
    }
    if (g_recordMaxVsync < vsyncId) {
        g_recordMaxVsync = vsyncId;
    }

    auto& customMetrics = GetCustomMetrics();
    float displayAreaToPercent = 100.f / static_cast<float>(GetDisplayArea());
    customMetrics.MultiplyFloat(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS, displayAreaToPercent);
    customMetrics.MultiplyFloat(RSPROFILER_METRIC_BLUR_AREA_SHADER_CALLS, displayAreaToPercent);
    customMetrics.MultiplyFloat(RSPROFILER_METRIC_ANIMATION_NODE_SIZE, displayAreaToPercent);

    for (size_t i = 0; i < customMetrics.GetCount(); i++) {
        RsMetricSet(std::to_string(i + 1) + "m", customMetrics.Get(i));
    }

    ResetCustomMetrics();

    // update general metrics
    for (const auto& metric : g_recordRsMetric) {
        captureData.SetProperty(metric.first, metric.second);
    }
    RsMetricClear();

    std::vector<char> out;
    DataWriter archive(out);
    char headerType = static_cast<char>(PackageID::RS_PROFILER_RS_METRICS);
    archive.Serialize(headerType);
    captureData.Serialize(archive);

    Network::SendBinary(out.data(), out.size());
    g_recordFile.WriteRSMetrics(0, timeSinceRecordStart, out.data(), out.size());
}

void RSProfiler::RecordUpdate()
{
    if (!IsRecording()) {
        return;
    }

    if (IsRecordAbortRequested()) {
        recordAbortRequested_ = false;
        SendMessage("Record: Exceeded memory limit. Abort"); // RESERVED!
        RecordStop(ArgList {});
        return;
    }

    const uint64_t frameLengthNanosecs = RawNowNano() - g_frameBeginTimestamp;
    const double currentTime = Utils::ToSeconds(g_frameBeginTimestamp);
    const double timeSinceRecordStart = currentTime - g_recordStartTime;
    const uint64_t recordStartTimeNano = Utils::ToNanoseconds(g_recordStartTime);
    double timeSinceRecordStartToSync = Utils::ToSeconds(g_frameSyncTimestamp - recordStartTimeNano);
    if (g_frameSyncTimestamp < recordStartTimeNano) {
        timeSinceRecordStartToSync = -Utils::ToSeconds(recordStartTimeNano - g_frameSyncTimestamp);
    }

    if (timeSinceRecordStart > 0.0) {
        WriteRSMetricsToRecordFile(timeSinceRecordStart, timeSinceRecordStartToSync, frameLengthNanosecs);
    }

    WriteBetaRecordMetrics(g_recordFile, timeSinceRecordStart);
}

void RSProfiler::RecordSave()
{
    SendMessage("Record: Prepping data for sending...");

    std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
    // DOUBLE WORK - send header of file
    const char headerType = 0;
    stream.write(reinterpret_cast<const char*>(&headerType), sizeof(headerType));
    stream.write(reinterpret_cast<const char*>(&g_recordStartTime), sizeof(g_recordStartTime));

    const uint32_t pidCount = g_recordFile.GetHeaderPids().size();
    stream.write(reinterpret_cast<const char*>(&pidCount), sizeof(pidCount));
    for (auto item : g_recordFile.GetHeaderPids()) {
        stream.write(reinterpret_cast<const char*>(&item), sizeof(item));
    }

    // FIRST FRAME HEADER
    const uint32_t sizeFirstFrame = static_cast<uint32_t>(g_recordFile.GetHeaderFirstFrame().size());
    stream.write(reinterpret_cast<const char*>(&sizeFirstFrame), sizeof(sizeFirstFrame));
    stream.write(reinterpret_cast<const char*>(&g_recordFile.GetHeaderFirstFrame()[0]), sizeFirstFrame);
    SendMessage("Record: First frame size: %d", sizeFirstFrame);

    // ANIME START TIMES
    const auto headerAnimeStartTimes = AnimeGetStartTimesFlattened(g_recordStartTime);

    const uint32_t startTimesSize = headerAnimeStartTimes.size();
    stream.write(reinterpret_cast<const char*>(&startTimesSize), sizeof(startTimesSize));
    stream.write(reinterpret_cast<const char*>(headerAnimeStartTimes.data()),
        startTimesSize * sizeof(std::pair<uint64_t, int64_t>));

    const auto imageCacheConsumption = ImageCache::Consumption();
    SendMessage("Record: Image cache memory usage: %.2fMB (%zu)", Utils::Megabytes(imageCacheConsumption),
        imageCacheConsumption);
    ImageCache::Serialize(stream);

    const auto binary = stream.str();
    Network::SendBinary(binary);

    SendMessage("Record: Sent: %.2fMB (%zu)", Utils::Megabytes(binary.size()), binary.size());
}


void RSProfiler::DumpConnections(const ArgList& args)
{
    if (!renderPipeline_) {
        return;
    }

    std::string out;
    const std::lock_guard<std::mutex> guard(renderPipeline_->renderConnectionMutex_);
    for (const auto& [pid, info] : renderPipeline_->renderConnections_) {
        const auto name = Utils::GetProcessName(pid);
        SendMessage("%s %d", !name.empty() ? name.data() : "<unknown>", pid);
    }
}

void RSProfiler::DumpDrawingCanvasNodes(const ArgList& args)
{
    if (!context_) {
        return;
    }
    const auto& map = const_cast<RSContext&>(*context_).GetMutableNodeMap();
    for (const auto& [_, subMap] : map.renderNodeMap_) {
        for (const auto& [_, node] : subMap) {
            if (node && node->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
                SendMessage("CANVAS_DRAWING_NODE: %lu", node->GetId());
            }
        }
    }
}

uint64_t RSProfiler::GetDisplayArea()
{
    if (!context_) {
        return 1;
    }
    std::shared_ptr<RSScreenRenderNode> displayNode = GetScreenNode(*context_);
    if (!displayNode) {
        return 1;
    }

    auto params = static_cast<RSScreenRenderParams*>(displayNode->GetRenderParams().get());
    if (!params) {
        return 1;
    }
    auto screenInfo = params->GetScreenInfo();
    return static_cast<uint64_t>(screenInfo.width * screenInfo.height);
}

void RSProfiler::DumpTreeToJson(const ArgList& args)
{
    if (!mainThread_ || !context_) {
        return;
    }

    JsonWriter json;
    json.PushObject();
    pid_t pid = args.Pid();
    RenderServiceTreeDump(json, pid);

    auto& display = json["Display"];
    auto screenNode = GetScreenNode(*context_);
    auto dirtyManager = screenNode ? screenNode->GetDirtyManager() : nullptr;
    if (dirtyManager) {
        const auto displayRect = dirtyManager->GetSurfaceRect();
        display = { displayRect.GetLeft(), displayRect.GetTop(), displayRect.GetRight(), displayRect.GetBottom() };
    } else {
        display = { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    json["transactionFlags"] = mainThread_->transactionFlags_;
    json["timestamp"] = mainThread_->timestamp_;
    json["vsyncID"] = mainThread_->vsyncId_;

    json.PopObject();
    Network::SendRSTreeDumpJSON(json.GetDumpString());

    if (args.String() != "NOLOG") {
        Network::SendMessage(json.GetDumpString());
    }
}

void RSProfiler::DumpNodeSurface(const ArgList& args)
{
    if (context_) {
        const auto node = context_->GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(Utils::GetRootNodeId(args.Node()));
        SendMessage("%s", node ? DumpSurfaceNode(*node).data() : "Invalid node id");
    }
}

void RSProfiler::BlinkNode(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        const auto parent = node->GetParent().lock();
        auto parentChildren = parent ? parent->GetChildren() : nullptr;
        if (!parentChildren) {
            return;
        }

        g_blinkNodeId = args.Node();
        g_blinkNodeCount = 0;

        g_blinkSavedParentChildren.clear();
        g_blinkSavedParentChildren.push_back(parent);

        for (const auto& child : *parentChildren) {
            if (child != nullptr) {
                g_blinkSavedParentChildren.push_back(child);
            }
        }

        AwakeRenderServiceThread();
        SendMessage("OK");
    }
}

void RSProfiler::KillPid(const ArgList& args)
{
    if (!context_) {
        return;
    }
    const pid_t pid = args.Pid();
    if (const auto node = GetRenderNode(Utils::GetRootNodeId(static_cast<uint64_t>(pid)))) {
        const auto parent = node->GetParent().lock();
        SendMessage("parentPid=%d parentNode=%lu", GetPid(parent), GetNodeId(parent));
        context_->GetMutableNodeMap().FilterNodeByPid(pid, true);
        AwakeRenderServiceThread();
    }
}

static void DumpParameter(std::string& out, int& usual, const std::string& name, bool value)
{
    out += " " + name + "=" + (value ? "true" : "false");
    usual += value ? 0 : 1;
}

void RSProfiler::PrintNodeCacheLo(const std::shared_ptr<RSRenderNode>& node)
{
    std::string nodeStr = std::to_string(node->id_) + ": ";
    int usual = 0;

    const std::string cacheTypes[] = { "CacheType::NONE", "CacheType::DISABLED_CACHE", "CacheType::CONTENT" };
    const auto cacheType = static_cast<int>(node->GetCacheType());
    nodeStr += "CacheType=" +
               (cacheType > static_cast<int>(CacheType::CONTENT) ? std::to_string(cacheType) : cacheTypes[cacheType]);
    usual += cacheType == static_cast<int>(CacheType::NONE) ? 1 : 0;

    const std::string drawingCacheTypes[] = { "RSDrawingCacheType::DISABLED_CACHE", "RSDrawingCacheType::FORCED_CACHE",
        "RSDrawingCacheType::TARGETED_CACHE", "RSDrawingCacheType::FOREGROUND_FILTER_CACHE" };
    const int drawingCacheType = node->GetDrawingCacheType();
    nodeStr += "DrawingCacheType=" + (drawingCacheType > RSDrawingCacheType::FOREGROUND_FILTER_CACHE
                                             ? std::to_string(drawingCacheType)
                                             : drawingCacheTypes[drawingCacheType]);
    usual += (cacheType == RSDrawingCacheType::DISABLED_CACHE) ? 1 : 0;

    DumpParameter(nodeStr, usual, "DrawInGroup", node->IsSuggestedDrawInGroup());
    if (constexpr int usualParamCnt = 6; usual >= usualParamCnt) {
        return;
    }
    SendMessage("%s", nodeStr.data());
}

void RSProfiler::TestSaveFrame(const ArgList& args)
{
    g_testDataFrame = FirstFrameMarshalling(RSFILE_VERSION_LATEST);
    SendMessage("Test: Saved frame size: %lu", g_testDataFrame.size());
}

void RSProfiler::TestLoadFrame(const ArgList& args)
{
    const auto error = FirstFrameUnmarshalling(g_testDataFrame, RSFILE_VERSION_LATEST);
    if (error.empty()) {
        SendMessage("Test: Loaded frame size: %lu", g_testDataFrame.size());
    } else {
        SendMessage("Test: Load failed: %s", error.data());
    }
}

void RSProfiler::TestSwitch(const ArgList& args)
{
    if (!IsHiddenSpaceEnabled()) {
        HiddenSpaceTurnOn();
        SendMessage("Test: HiddenSpaceTurnOn");
    } else {
        HiddenSpaceTurnOff();
        SendMessage("Test: HiddenSpaceTurnOff");
    }
}

void RSProfiler::BuildTestTree(const ArgList& args)
{
    if (!context_) {
        return;
    }

    if (!RSProfiler::testTree_.empty()) {
        SendMessage("Test tree already build");
        return;
    }

    const NodeId defaultTopId = 54321000;
    NodeId topId = args.Count() == 0 ? defaultTopId : args.Node();
    bool withDisplay = args.Count() > 1;

    auto testTreeBuilder = TestTreeBuilder();

    RSProfiler::testTree_ = testTreeBuilder.Build(*context_, topId, withDisplay);

    SendMessage("Build test tree");
}

void RSProfiler::ClearTestTree(const ArgList& args)
{
    for (auto iter = RSProfiler::testTree_.rbegin(); iter != RSProfiler::testTree_.rend(); ++iter) {
        (*iter)->RemoveFromTree();
        (*iter)->RemoveAllModifiersNG();
    }
    RSProfiler::testTree_.clear();
    SendMessage("Test tree cleared");
}

void RSProfiler::InitTrace3D(const ArgList& args)
{
    auto& listArgs = args.GetList();
    auto mode = std::find_if(listArgs.begin(), listArgs.end(),
    [](const std::string& s) { return s.find("trace3d_disp_mode") != std::string::npos; });
    if (!trace3dApi_ || mode == listArgs.end()) {
        return;
    }

    trace3DEnabled_ = true;

    Trace3DCoreParamValue paramMode{};
    paramMode.type = TRACE3D_CORE_PARAM_MODE;
    paramMode.valueType = trace3d::api::ParamValueType::UINT32;
    paramMode.value[0].uint32 = static_cast<uint32_t>(ExtractTrace3DNumber(*mode));
    if (paramMode.value[0].uint32 != static_cast<uint32_t>(-1)) {
        trace3dApi_->UpdateParam(&paramMode);
    }

    Trace3DCoreParamValue paramLog = CreateAndUpdateTraceParam(listArgs, trace3dApi_,
        "trace3d_log_flags", TRACE3D_CORE_PARAM_LOG_FLAGS,
        trace3d::api::ParamValueType::UINT64);

    Trace3DCoreParamValue paramStat = CreateAndUpdateTraceParam(listArgs, trace3dApi_,
        "trace3d_stat_flags", TRACE3D_CORE_PARAM_STAT_FLAGS,
        trace3d::api::ParamValueType::UINT64);

    Trace3DCoreParamValue paramMetric = CreateAndUpdateTraceParam(listArgs, trace3dApi_,
        "trace3d_metric_flags", TRACE3D_CORE_PARAM_METRIC_FLAGS,
        trace3d::api::ParamValueType::UINT64);

    trace3dApi_->ResetStatisticTime();

    Trace3DCoreParamValue p0{};
    Trace3DCoreParamValue p1{};
    Trace3DCoreParamValue p2{};
    trace3dApi_->GetParam(TRACE3D_CORE_PARAM_STATISTIC_ORIGIN_TIME_MICROSECONDS, &p0);
    trace3dApi_->GetParam(TRACE3D_CORE_PARAM_STATISTIC_TIME_MICROSECONDS, &p1);
    trace3dApi_->GetParam(TRACE3D_CORE_PARAM_TIME_MICROSECONDS, &p2);

    trace3dApi_->DebugTraceLogLevelMsg(TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_INFO,
        TRACE3D_CORE_DEBUG_TRACE_MESSAGE_FLAG_BACKTRACE,
        "%s:%d TRACE3D Start Record statOrigTimeMcs:%" PRIu64 ", statTimeMcs:%" PRIu64 ", "
        "timeMcs:%" PRIu64 ", mode: %u, logFlags: %" PRIu64 ", statFlags: %" PRIu64 ", "
        "metricFlags: %" PRIu64 "", __FUNCTION__, __LINE__,
        p0.value[0].uint64, p1.value[0].uint64, p2.value[0].uint64, paramMode.value[0].uint32,
        paramLog.value[0].uint64, paramStat.value[0].uint64, paramMetric.value[0].uint64);
}

void RSProfiler::StopTrace3D()
{
    if (trace3dApi_ && trace3DEnabled_) {
        trace3DEnabled_ = false;
        Trace3DCoreParamValue param{};
        param.type = TRACE3D_CORE_PARAM_MODE;
        param.valueType = trace3d::api::ParamValueType::UINT32;
        param.value[0].uint32 = 0; // passthrough mode

        Trace3DCoreParamValue p0{};
        Trace3DCoreParamValue p1{};
        Trace3DCoreParamValue p2{};
        trace3dApi_->GetParam(TRACE3D_CORE_PARAM_STATISTIC_ORIGIN_TIME_MICROSECONDS, &p0);
        trace3dApi_->GetParam(TRACE3D_CORE_PARAM_STATISTIC_TIME_MICROSECONDS, &p1);
        trace3dApi_->GetParam(TRACE3D_CORE_PARAM_TIME_MICROSECONDS, &p2);

        trace3dApi_->DebugTraceLogLevelMsg(TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_WARN,
            TRACE3D_CORE_DEBUG_TRACE_MESSAGE_FLAG_BACKTRACE,
            "%s:%d TRACE3D Stop Record statOrigTimeMcs:%u, statTimeMcs:%u, timeMcs:%u, mode: %u", __FUNCTION__,
            __LINE__, p0.value[0].uint64, p1.value[0].uint64, p2.value[0].uint64, param.value[0].uint32);

        trace3dApi_->UpdateParam(&param);
    }
}

void RSProfiler::RecordStart(const ArgList& args)
{
    if (!mainThread_ || !context_) {
        return;
    }
    if (!IsNoneMode() || IsHiddenSpaceEnabled()) {
        SendMessage("Record: Cannot start during playback or saving");
        return;
    }

    const auto betaRecordStarted = (args.String() == "BETAREC");

    bool transactionMutexLocked = false;
    uint64_t counterParseTransactionDataStart = 0;
    uint64_t counterParseTransactionDataEnd = 0;
    uint64_t counterOnRemoteRequest = 0;
    if (betaRecordStarted) {
        counterParseTransactionDataStart = GetParseTransactionDataStartCounter();
        counterParseTransactionDataEnd = GetParseTransactionDataEndCounter();
        counterOnRemoteRequest = g_counterOnRemoteRequest;
        if (counterParseTransactionDataStart != counterParseTransactionDataEnd) {
            // transaction data unmarshalling is in progress
            return;
        }
        transactionMutexLocked = mainThread_->TransitionDataMutexLockIfNoCommands();
        if (!transactionMutexLocked) {
            // there are unmarshalled commands in a queue to execute
            return;
        }
    }

    g_recordStartTime = 0.0;
    g_recordParcelNumber = 0;

    MetricRenderNodeInit(context_);
    ResetCustomMetrics();

    ImageCache::Reset();
    g_lastCacheImageCount = 0;

    if (!OpenBetaRecordFile(g_recordFile)) {
        const auto path = (args.String(0) == "-f") ? args.String(1) : "";
        g_recordFile.SetVersion(RSFILE_VERSION_LATEST);
        g_recordFile.Create(Utils::IsSandboxPath(path) ? path : RSFile::GetDefaultPath());
    }

    if (!g_recordFile.IsOpen()) {
        SendMessage("Record: Cannot create file");
        if (transactionMutexLocked) {
            mainThread_->TransitionDataMutexUnlock();
        }
        return;
    }

    g_recordMinVsync = g_recordMaxVsync = 0;
    g_recordFile.AddLayer(); // add 0 layer

    {
        const std::unique_lock<std::mutex> guard(g_mutexFirstFrameMarshalling);
        FilterMockNode(*context_);
        RSTypefaceCache::Instance().ReplayClear();
        g_recordFile.AddHeaderFirstFrame(FirstFrameMarshalling(g_recordFile.GetVersion(), betaRecordStarted));
    }

    if (transactionMutexLocked) {
        const auto countersChanged = (GetParseTransactionDataStartCounter() != counterParseTransactionDataStart ||
                                      GetParseTransactionDataEndCounter() != counterParseTransactionDataEnd ||
                                      g_counterOnRemoteRequest != counterOnRemoteRequest);
        mainThread_->TransitionDataMutexUnlock();
        if (countersChanged) {
            g_recordStartTime = 0.0;
            g_lastCacheImageCount = 0;
            ImageCache::Reset();
            g_recordFile.Close();
            auto& jobGlobal = GetMarshallingJob();
            jobGlobal = nullptr;
            SetMode(Mode::NONE);
            SetThreadSubMode(SubMode::NONE);
            return;
        }
    }

    if (betaRecordStarted) {
        std::thread([]() {
            SetMarshalFirstFrameThreadFlag(true);
            MarshalFirstFrameNodesLoop();
            SetMarshalFirstFrameThreadFlag(false);
        }).detach();
    }

    g_recordFile.LayerAddHeaderProperty(0, "MetricsList", RsMetricGetList());

    InitTrace3D(args);

    g_recordStartTime = Now();
    g_frameNumber = 0;
    SetMode(Mode::WRITE);

    if (betaRecordStarted) {
        return;
    }

    std::thread([]() {
        while (IsRecording()) {
            SendTelemetry(Now() - g_recordStartTime);
            static constexpr int32_t GFX_METRICS_SEND_INTERVAL = 8;
            std::this_thread::sleep_for(std::chrono::milliseconds(GFX_METRICS_SEND_INTERVAL));
        }
    }).detach();
    SendMessage("Network: Record start"); // RESERVED!
}

void RSProfiler::RecordStop(const ArgList& args)
{
    if (!IsRecording()) {
        SendMessage("Record: Already stopped");
        return;
    }

    StopTrace3D();

    SetMode(Mode::SAVING);
    if (args.String() == "REMOVELAST") {
        g_recordFile.UnwriteRSData(); // remove last commands they may start animation with password depiction
        g_recordFile.UnwriteRSData();
    }

    std::thread thread([betaRecord = IsBetaRecordStarted()]() {
        g_recordFile.SetWriteTime(g_recordStartTime);

        // force finish first frame marshalling
        while (GetMarshalFirstFrameThreadFlag()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if (betaRecord) {
            SaveBetaRecordFile(g_recordFile);
        } else {
            RecordSave();
        }

        g_recordFile.Close();
        g_recordStartTime = 0.0;
        g_lastCacheImageCount = 0;

        ImageCache::Reset();

        SendMessage("Record: Stopped");
        SendMessage("Network: record_vsync_range %" PRIu64 " %" PRIu64,
            g_recordMinVsync, g_recordMaxVsync); // RESERVED!

        SetMode(Mode::NONE);
        if (!IsEnabled()) {
            Utils::FileDelete(RSFile::GetDefaultPath());
        }
    });

    if (args.Includes("WAITFORFINISH")) {
        thread.join();
    } else {
        thread.detach();
    }
}

void RSProfiler::PlaybackSetImmediate(const ArgList& args)
{
    g_playbackImmediate = args.Bool();
    SendMessage("Playback immediate mode: %d", g_playbackImmediate.load()); // RESERVED!
}

void RSProfiler::PlaybackPrepareFirstFrame(const ArgList& args)
{
    if (!mainThread_ || !context_) {
        SendMessage("Playback: Invalid main thread or context");
        return;
    }

    if (!IsNoneMode()) {
        SendMessage("Playback: Cannot initialize during record or saving");
        return;
    }

    if (g_playbackFile.IsOpen() || IsHiddenSpaceEnabled()) {
        SendMessage("Playback: Already initialized");
        return;
    }

    SetReplayStartTimeNano(0);
    constexpr auto pathArgument = 2;
    auto path = Utils::GetRealPath(args.String(pathArgument));
    if (!Utils::IsSandboxPath(path)) {
        SendMessage("Playback: File must be under /data directory");
        path = RSFile::GetDefaultPath();
    }

    RSTypefaceCache::Instance().ReplayClear();
    ImageCache::Reset();
    AnimeGetStartTimes().clear();

    std::string error;
    if (!g_playbackFile.Open(path, error)) {
        SendMessage("Playback: Cannot open file: %s", error.data());
        return;
    }
    SendMessage("Playback: File open: %s", path.data());

    if (args.String() == "VSYNC") {
        g_playbackFile.CacheVsyncId2Time(0);
        const auto vsync = g_playbackFile.GetClosestVsyncId(args.Int64(1));
        g_playbackPauseTime = g_playbackFile.ConvertVsyncId2Time(vsync);
        SendMessage("Playback: Use VSYNC %" PRId64 " (requested %" PRId64 ")", vsync, args.Int64(1));
    } else {
        g_playbackPauseTime = args.Fp64(1);
    }
    SendMessage("Playback: Pause time: %f", g_playbackPauseTime);

    LoadAnimationTimestamps(AnimeGetStartTimes());

    error = FirstFrameUnmarshalling(g_playbackFile.GetHeaderFirstFrame(), g_playbackFile.GetVersion());
    if (!error.empty()) {
        SendMessage("Playback: %s", error.data());
        FilterMockNode(*context_);
        g_playbackFile.Close();
        return;
    }

    constexpr int defaultWaitFrames = 5;
    g_playbackWaitFrames = defaultWaitFrames;
    SendMessage("awake_frame %d", g_playbackWaitFrames); // RESERVED!
    AwakeRenderServiceThread();
}

void RSProfiler::PlaybackStart(const ArgList& args)
{
    if (!mainThread_ || !context_) {
        SendMessage("Playback: Cannot start. Invalid main thread or context");
        return;
    }

    if (!IsNoneMode()) {
        SendMessage("Playback: Cannot start during record or saving");
        return;
    }

    if (!g_playbackFile.IsOpen() || IsHiddenSpaceEnabled()) {
        SendMessage("Playback: Already started");
        return;
    }

    HiddenSpaceTurnOn();

    PurgeMockConnections();
    for (const auto pid : g_playbackFile.GetHeaderPids()) {
        CreateMockConnection(Utils::GetMockPid(pid));
    }

    const auto currentTime = mainThread_->GetCurrentVsyncTime();
    SetReplayStartTimeNano(currentTime);
    SetTransactionTimeCorrection(g_playbackFile.GetWriteTime());
    if (g_playbackPauseTime > 0.0) {
        const auto pauseTime = currentTime + Utils::ToNanoseconds(g_playbackPauseTime / BaseGetPlaybackSpeed());
        TimePauseAt(currentTime, pauseTime, g_playbackImmediate);
    }
    AwakeRenderServiceThread();

    g_playbackShouldBeTerminated = false;
    g_replayLastPauseTimeReported = 0;
    SetMode(Mode::READ);

    std::thread thread([]() {
        constexpr int fps = 120;
        double advanceTime = 1.0 / fps;
        const auto timeoutLimit = Utils::ToNanoseconds(1.0) / (2 * fps);
        double eofTime = g_playbackFile.GetEOFTime();
        while (IsPlaying()) {
            int64_t nextPacketTimeNano =
                static_cast<int64_t>(Utils::ToNanoseconds(PlaybackUpdate(PlaybackDeltaTime(), eofTime, advanceTime)));
            // timeout - sleep time to next packet
            int64_t timeout = nextPacketTimeNano - PlaybackDeltaTimeNano();
            if (timeout < 0) {
                timeout = 0;
            }
            timeout = timeout - (timeout % timeoutLimit) + timeoutLimit;
            if (timeout > 0) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(timeout));
            }
        }
        if (g_playbackFile.IsOpen()) {
            if (const auto vsync = g_playbackFile.ConvertTime2VsyncId(PlaybackDeltaTime())) {
                SendMessage("Replay timer paused vsyncId=%" PRId64 "", vsync); // RESERVED!
            }
            g_playbackFile.Close();
        }
        SetReplayStartTimeNano(0);
        TimePauseClear();
        g_playbackShouldBeTerminated = false;
        SendMessage("Playback: Thread terminated");
    });
    thread.detach();

    SendMessage("Playback start"); // RESERVED!
}

void RSProfiler::PlaybackStop(const ArgList& args)
{
    if (!mainThread_ || !context_) {
        SendMessage("Playback: Cannot stop. Invalid main thread or context");
        return;
    }

    if (!g_playbackFile.IsOpen() && !IsHiddenSpaceEnabled()) {
        SendMessage("Playback: Already stopped");
        return;
    }
    SetMode(Mode::NONE);
    if (!IsHiddenSpaceEnabled()) {
        g_playbackFile.Close(); // PlaybackPrepareFirstFrame only was called
    }
    g_playbackShouldBeTerminated = true;
    HiddenSpaceTurnOff();
    FilterMockNode(*context_);
    constexpr int maxCountForSecurity = 1000;
    for (int i = 0; !RSRenderNodeGC::Instance().IsBucketQueueEmpty() && i < maxCountForSecurity; i++) {
        RSRenderNodeGC::Instance().ReleaseNodeBucket();
    }
    RSTypefaceCache::Instance().ReplayClear();
    ImageCache::Reset();
    g_replayLastPauseTimeReported = 0;
    PurgeMockConnections();
    SendMessage("Playback stop"); // RESERVED!
}

double RSProfiler::PlaybackDeltaTime()
{
    return Utils::ToSeconds(PlaybackDeltaTimeNano());
}

int64_t RSProfiler::PlaybackDeltaTimeNano()
{
    return static_cast<int64_t>(NowNano()) - static_cast<int64_t>(GetReplayStartTimeNano());
}

static bool ReadRemoteRequest(
    const std::vector<uint8_t>& data, pid_t& pid, uint32_t& code, MessageParcel& parcel, MessageOption& option)
{
    std::stringstream stream;
    stream.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    stream.seekg(0);

    if (!stream.read(reinterpret_cast<char*>(&pid), sizeof(pid)) ||
        !stream.read(reinterpret_cast<char*>(&code), sizeof(code))) {
        return false;
    }

    static const auto MAX_SIZE = std::min(std::numeric_limits<size_t>::max() - 1, std::vector<char>().max_size());
    size_t size = 0;
    if (!stream.read(reinterpret_cast<char*>(&size), sizeof(size)) || (size > MAX_SIZE)) {
        return false;
    }

    std::vector<char> buffer(size);
    if (!stream.read(buffer.data(), static_cast<std::streamsize>(size))) {
        return false;
    }

    int32_t flags = 0;
    if (!stream.read(reinterpret_cast<char*>(&flags), sizeof(flags))) {
        return false;
    }
    option.SetFlags(flags);

    int32_t waitTime = 0;
    if (!stream.read(reinterpret_cast<char*>(&waitTime), sizeof(waitTime))) {
        return false;
    }
    option.SetWaitTime(waitTime);

    return (parcel.GetMaxCapacity() >= buffer.size() || parcel.SetMaxCapacity(buffer.size())) &&
           parcel.WriteBuffer(buffer.data(), buffer.size());
}

double RSProfiler::PlaybackUpdate(double deltaTime, double eofTime, double advanceTime)
{
    std::vector<uint8_t> data;
    double time = 0.0;
    while (!g_playbackShouldBeTerminated && g_playbackFile.ReadRSData(deltaTime + advanceTime, data, time)) {
        pid_t pid = 0;
        uint32_t code = 0;
        AlignedMessageParcel parcel;
        MessageOption option;
        if (ReadRemoteRequest(data, pid, code, parcel.parcel, option)) {
            if (const auto connection = GetMockConnection(Utils::GetMockPid(pid))) {
                MessageParcel reply;
                connection->SendRequest(code, parcel.parcel, reply, option);
            }
        }

        if (g_playbackImmediate) {
            deltaTime = time;
            break;
        }
    }

    if (deltaTime >= eofTime) {
        g_playbackShouldBeTerminated = true;
    }
    return time;
}

void RSProfiler::PlaybackPrepare(const ArgList& args)
{
    const pid_t pid = args.Pid();
    if (!context_ || (pid == 0)) {
        return;
    }

    AwakeRenderServiceThread();
    SendMessage("OK");
}

void RSProfiler::PlaybackPause(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    const uint64_t currentTime = RawNowNano();
    const double pause = Utils::ToSeconds(PatchTime(currentTime) - GetReplayStartTimeNano());
    TimePauseAt(g_frameBeginTimestamp, currentTime, g_playbackImmediate);
    SendMessage("OK: %f", pause); // RESERVED!

    if (const auto vsync = g_playbackFile.ConvertTime2VsyncId(pause)) {
        SendMessage("Replay timer paused vsyncId=%" PRId64, vsync); // RESERVED!
    }
    g_replayLastPauseTimeReported = pause;
}

void RSProfiler::PlaybackPauseAt(const ArgList& args)
{
    if (!IsPlaying()) {
        SendMessage("Playback: Cannot pause. Playback is not started");
        return;
    }

    double pauseAtTimeSec = 0.0;
    if (args.String(0) == "VSYNC") {
        const auto vsync = g_playbackFile.GetClosestVsyncId(args.Int64(1));
        SendMessage("Playback: Pause at VSYNC %" PRId64 " (requested %" PRId64 ")",
            vsync, args.Int64(1));
        pauseAtTimeSec = g_playbackFile.ConvertVsyncId2Time(vsync);
    } else {
        pauseAtTimeSec = args.Fp64();
    }

    const uint64_t currentTimeNano = RawNowNano();
    const double alreadyPlayedTimeSec = Utils::ToSeconds(PatchTime(currentTimeNano) - GetReplayStartTimeNano());
    if (alreadyPlayedTimeSec > pauseAtTimeSec) {
        return;
    }

    // set 2nd pause
    const uint64_t pauseAfterTimeNano = currentTimeNano + Utils::ToNanoseconds(pauseAtTimeSec - alreadyPlayedTimeSec)
         / BaseGetPlaybackSpeed();

    TimePauseAt(currentTimeNano, pauseAfterTimeNano, g_playbackImmediate);
    ResetAnimationStamp();
    SendMessage("Playback: Paused");
}

void RSProfiler::ProcessCommands()
{
    if (g_playbackWaitFrames > 0) {
        g_playbackWaitFrames--;
        SendMessage("awake_frame %d", g_playbackWaitFrames); // RESERVED!
        if (context_) {
            MarkReplayNodesDirty(*context_);
        }
        AwakeRenderServiceThread();
        return;
    }

    std::vector<std::string> line;
    if (Network::PopCommand(line)) {
        Invoke(line);
    }
}

uint32_t RSProfiler::GetFrameNumber()
{
    return g_frameNumber;
}

uint32_t RSProfiler::GetRenderFrameNumber()
{
    return g_renderFrameNumber;
}

void RSProfiler::BlinkNodeUpdate()
{
    if (g_blinkSavedParentChildren.empty()) {
        return;
    }

    constexpr uint32_t blinkTotalCount = 20;
    constexpr uint32_t blinkFrameDelay = 5;
    const uint32_t blinkStage = g_blinkNodeCount / blinkFrameDelay;
    if (const bool isOdd = blinkStage % 2) {
        // restore node
        const auto parentNode = g_blinkSavedParentChildren[0];
        if (!parentNode) {
            return;
        }
        for (size_t i = 1; i < g_blinkSavedParentChildren.size(); i++) {
            const auto child = g_blinkSavedParentChildren[i];
            parentNode->RemoveChild(child);
            child->ResetParent();
        }
        for (size_t i = 1; i < g_blinkSavedParentChildren.size(); i++) {
            const auto child = g_blinkSavedParentChildren[i];
            parentNode->AddChild(child);
        }
        if (blinkStage > blinkTotalCount) {
            g_blinkNodeCount = 0;
            g_blinkSavedParentChildren.clear();
        }
    } else {
        // remove node
        auto blinkNode = GetRenderNode(g_blinkNodeId);
        if (const auto parentNode = g_blinkSavedParentChildren[0]) {
            parentNode->RemoveChild(blinkNode);
        }
        if (blinkNode) {
            blinkNode->ResetParent();
        }
    }

    g_blinkNodeCount++;
    AwakeRenderServiceThread();
}

void RSProfiler::CalcPerfNodeUpdate()
{
    if (g_calcPerfNode != 0 || g_nodeListPerfCalcIndex < 0) {
        return;
    }

    for (const auto& item : g_calcPerfSavedChildren) {
        const auto parentNode = item.first;
        if (!parentNode) {
            continue;
        }
        for (const auto& child : item.second) {
            parentNode->RemoveChild(child);
            child->ResetParent();
        }
        for (const auto& child : item.second) {
            parentNode->AddChild(child);
        }
    }

    if (g_calcPerfNodeTry != 0) {
        g_calcPerfNodeTry = 0;
        g_nodeListPerfCalcIndex++;
        CalcPerfNodeAllStep();
        return;
    }

    g_calcPerfSavedChildren.clear();
    g_calcPerfNodeTry++;
    AwakeRenderServiceThread();
}

bool RSProfiler::LogEventStart(uint64_t curTime, RSCaptureData& captureData, double& timeSinceRecordStart)
{
    if (!IsEnabled() || !IsRecording()) {
        return false;
    }

    uint64_t nanoRecordStartTime = Utils::ToNanoseconds(g_recordStartTime);
    if (curTime < nanoRecordStartTime) {
        curTime = nanoRecordStartTime;
    }
    timeSinceRecordStart = Utils::ToSeconds(curTime - nanoRecordStartTime);

    captureData.SetTime(timeSinceRecordStart);
    captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_VSYNC);
    return true;
}

void RSProfiler::LogEventFinish(RSCaptureData& captureData, double timeSinceRecordStart)
{
    std::vector<char> out;
    DataWriter archive(out);
    char headerType = static_cast<char>(PackageID::RS_PROFILER_RS_EVENT);
    archive.Serialize(headerType);
    captureData.Serialize(archive);

    Network::SendBinary(out.data(), out.size());
    g_recordFile.WriteLogEvent(0, timeSinceRecordStart, out.data(), out.size());
}

void RSProfiler::LogEventVSync(uint64_t syncTime)
{
    RSCaptureData captureData;
    double timeSinceRecordStart;
    if (!LogEventStart(syncTime, captureData, timeSinceRecordStart)) {
        return;
    }
    captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_VSYNC);
    LogEventFinish(captureData, timeSinceRecordStart);
}

void RSProfiler::LogEventMsg(uint64_t curTime, RSProfilerLogType type, const std::string& msg)
{
    RSCaptureData captureData;
    double timeSinceRecordStart;
    if (!LogEventStart(curTime, captureData, timeSinceRecordStart)) {
        return;
    }

    if (type == RSProfilerLogType::ERROR) {
        captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_MSGERR);
    } else if (type == RSProfilerLogType::WARNING) {
        captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_MSGWARN);
    } else if (type == RSProfilerLogType::INFO) {
        captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_INFO);
    } else if (type == RSProfilerLogType::PIXELMAP) {
        captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_PIXELMAP);
    } else if (type == RSProfilerLogType::PARCEL_UNMARSHALLING_START) {
        captureData.SetProperty(
            RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_PARCEL_UNMARSHALLING_START);
    } else if (type == RSProfilerLogType::PARCEL_UNMARSHALLING_END) {
        captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_PARCEL_UNMARSHALLING_END);
    } else if (type == RSProfilerLogType::DEBUG) {
        captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_DEBUG);
    } else if (type == RSProfilerLogType::PIXELMAP_YUV) {
        captureData.SetProperty(RSCaptureData::KEY_EVENT_TYPE, RSCaptureData::VAL_EVENT_TYPE_PIXELMAP_YUV);
    }

    captureData.SetProperty(RSCaptureData::KEY_EVENT_MSG, msg);
    LogEventFinish(captureData, timeSinceRecordStart);
}

void RSProfiler::RsMetricClear()
{
    g_recordRsMetric.clear();
}

void RSProfiler::RsMetricSet(const std::string& name, std::string value)
{
    g_recordRsMetric[name] = std::move(value);
}

void RSProfiler::TestSaveSubTree(const ArgList& args)
{
    if (!mainThread_ || !context_) {
        SendMessage("Test: SaveSubTree: Invalid main thread or context");
        return;
    }

    uint64_t nodeId = args.Node();
    std::string path = "/data/rssbtree_test_" + std::to_string(nodeId);
    if (args.String() == "-f") {
        path = args.String(1);
        nodeId = GetRootNodeId();
        SendMessage("Test: SaveSubTree: Current root node: %" PRIu64, nodeId);
    }

    const auto node = GetRenderNode(nodeId);
    if (!node) {
        SendMessage("Test: SaveSubTree: Invalid node");
        return;
    }

    if (!Utils::IsSandboxPath(path)) {
        SendMessage("Test: SaveSubTree: Invalid path");
        return;
    }

    // needs setenforce 0
    std::ofstream file(path);
    if (!file) {
        SendMessage("Test: SaveSubTree: Cannot open file");
        return;
    }

    std::stringstream stream;
    uint32_t fileVersion = RSFILE_VERSION_LATEST;
    stream.write(reinterpret_cast<const char*>(&fileVersion), sizeof(fileVersion));
    RSSystemProperties::SetProfilerPixelCheckMode(true);
    MarshalSubTree(*context_, stream, *node, fileVersion);
    RSSystemProperties::SetProfilerPixelCheckMode(false);

    file << stream.rdbuf();
    SendMessage("Save SubTree Size: %zu", static_cast<size_t>(file.tellp()));
    SendMessage("Save subTree Success, file path: %s", path.data());
}

void RSProfiler::TestLoadSubTree(const ArgList& args)
{
    if (!mainThread_ || !context_) {
        SendMessage("Test: LoadSubTree: Invalid main thread or context");
        return;
    }

    const auto node = GetRenderNode(args.Node());
    if (!node) {
        SendMessage("Test: LoadSubTree: Invalid node");
        return;
    }

    const auto path = Utils::GetRealPath(args.String(1));
    if (!Utils::IsSandboxPath(path)) {
        SendMessage("Test: LoadSubTree: File must be under /data directory");
        return;
    }

    std::ifstream file(path);
    if (!file) {
        const std::error_code code(errno, std::system_category());
        RS_LOGE("Test: LoadSubTree: Cannot open file: %{public}s", code.message().data()); // NOLINT
        SendMessage("Test: LoadSubTree: Cannot open file: %s", code.message().data());
        return;
    }

    std::stringstream stream;
    stream << file.rdbuf();
    file.close();

    uint32_t fileVersion = 0u;
    if (!stream.read(reinterpret_cast<char*>(&fileVersion), sizeof(fileVersion))) {
        SendMessage("Test: LoadSubTree: Cannot read file version");
        return;
    }

    const auto error = UnmarshalSubTree(*context_, stream, *node, fileVersion);
    SendMessage("Load subTree result: %s", error.data()); // RESERVED!
}

void RSProfiler::TestClearSubTree(const ArgList& args)
{
    if (!context_) {
        return;
    }
    FilterMockNode(*context_);
    constexpr int maxCountForSecurity = 1000;
    for (int i = 0; !RSRenderNodeGC::Instance().IsBucketQueueEmpty() && i < maxCountForSecurity; i++) {
        RSRenderNodeGC::Instance().ReleaseNodeBucket();
    }
    RSTypefaceCache::Instance().ReplayClear();
    ImageCache::Reset();
    SendMessage("Clear SubTree Success"); // RESERVED!
}

void RSProfiler::MarshalSubTree(
    RSContext& context, std::stringstream& data, const RSRenderNode& node, uint32_t fileVersion, bool clearImageCache)
{
    if (clearImageCache) {
        ImageCache::Reset();
    }

    std::stringstream nodes;
    MarshalSubTreeLo(context, nodes, node, fileVersion);
    TypefaceMarshalling(data, fileVersion);

    std::stringstream images;
    ImageCache::Serialize(images);
    ImageCache::Reset();

    const uint32_t imagesSize = images.str().size();
    data.write(reinterpret_cast<const char*>(&imagesSize), sizeof(imagesSize));
    data.write(images.str().data(), static_cast<std::streamsize>(imagesSize));

    const uint32_t nodesSize = nodes.str().size();
    data.write(reinterpret_cast<const char*>(&nodesSize), sizeof(nodesSize));
    data.write(nodes.str().data(), static_cast<std::streamsize>(nodesSize));
}

std::string RSProfiler::UnmarshalSubTree(
    RSContext& context, std::stringstream& data, RSRenderNode& attachNode, uint32_t fileVersion, bool clearImageCache)
{
    if (clearImageCache) {
        ImageCache::Reset();
    }

    auto error = TypefaceUnmarshalling(data, fileVersion);
    if (!error.empty()) {
        return error;
    }

    uint32_t imagesSize = 0u;
    if (!data.read(reinterpret_cast<char*>(&imagesSize), sizeof(imagesSize)) || !ImageCache::Deserialize(data)) {
        RSTypefaceCache::Instance().ReplayClear();
        return "UnmarshalSubTree: Cannot read image cache";
    }

    uint32_t nodesSize = 0u;
    if (!data.read(reinterpret_cast<char*>(&nodesSize), sizeof(nodesSize))) {
        RSTypefaceCache::Instance().ReplayClear();
        ImageCache::Reset();
        return "UnmarshalSubTree: Cannot read node count";
    }

    SetThreadSubMode(SubMode::READ_EMUL);
    DisableThreadSharedMemory();
    error = UnmarshalSubTreeLo(context, data, attachNode, fileVersion);
    EnableThreadSharedMemory();
    SetThreadSubMode(SubMode::NONE);

    if (!error.empty()) {
        RSTypefaceCache::Instance().ReplayClear();
        ImageCache::Reset();
        return error;
    }

    MarkReplayNodesDirty(context);
    AwakeRenderServiceThread();
    return "";
}

uint64_t RSProfiler::GetRootNodeId()
{
    if (!context_) {
        return 0;
    }
    auto pid = 0;
    if (auto mainThread = RSMainThread::Instance()) {
        const auto nodeId = mainThread->GetFocusNodeId();
        if (auto renderNode = GetRenderNode(nodeId)) {
            pid = GetPid(renderNode);
            SendMessage("Focus Node pid: %d", pid);
        }
    }

    if (pid == 0) {
        return 0;
    }

    std::optional<NodeId> rootNodeId;
    size_t rootNodeChildrenCount = 0;
    auto rootNode = context_->GetGlobalRootRenderNode();
    if (!rootNode) {
        SendMessage("Cannot get root node");
        return 0;
    }

    std::queue<const std::shared_ptr<RSRenderNode>> nodes;
    nodes.push(rootNode);

    while (!nodes.empty()) {
        const auto node = nodes.front();
        nodes.pop();

        if (!node) {
            continue;
        }

        if (pid == GetPid(node) && rootNodeChildrenCount < node->GetChildrenCount()) {
            const auto parentNode = node->GetParent().lock();
            if (!parentNode || pid != GetPid(parentNode)) {
                rootNodeId = node->GetId();
                rootNodeChildrenCount = node->GetChildrenCount();
            }
        }
        if (const auto children = node->GetChildren()) {
            for (const auto& child: *children) {
                nodes.push(child);
            }
        }
    }

    if (rootNodeId) {
        return *rootNodeId;
    }
    return 0;
}
} // namespace OHOS::Rosen

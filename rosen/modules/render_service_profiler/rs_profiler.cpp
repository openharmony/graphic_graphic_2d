/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <filesystem>
#include <numeric>

#include "foundation/graphic/graphic_2d/utils/log/rs_trace.h"
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
#include "rs_profiler_settings.h"
#include "rs_profiler_telemetry.h"

#include "params/rs_display_render_params.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_uni_render_util.h"
#include "render/rs_typeface_cache.h"

namespace OHOS::Rosen {

// (user): Move to RSProfiler
static RSRenderService* g_renderService = nullptr;
static std::atomic<int32_t> g_renderServiceCpuId = 0;
static std::atomic<int32_t> g_renderServiceRenderCpuId = 0;
static RSMainThread* g_mainThread = nullptr;
static RSContext* g_context = nullptr;
static uint64_t g_frameBeginTimestamp = 0u;
static uint64_t g_frameRenderBeginTimestamp = 0u;
static double g_dirtyRegionPercentage = 0.0;
static bool g_rdcSent = true;
static uint64_t g_recordMinVsync = 0;
static uint64_t g_recordMaxVsync = 0;

static std::atomic<uint32_t> g_lastCacheImageCount = 0;

static RSFile g_recordFile {};
static double g_recordStartTime = 0.0;
static uint32_t g_frameNumber = 0;

static RSFile g_playbackFile {};
static double g_playbackStartTime = 0.0;
static NodeId g_playbackParentNodeId = 0;
static int g_playbackPid = 0;
static bool g_playbackShouldBeTerminated = true;
static double g_playbackPauseTime = 0;
static int g_playbackWaitFrames = 0;
static double g_replayLastPauseTimeReported = 0;

static std::unordered_set<NodeId> g_nodeSetPerf;
static std::unordered_map<NodeId, int> g_mapNode2Count;
static NodeId g_calcPerfNode = 0;
static int g_calcPerfNodeTry = 0;
static bool g_calcPerfNodeExcludeDown = false;

constexpr int CALC_PERF_NODE_TIME_COUNT = 64; // increased to improve reliability of perfomance measurement
static uint64_t g_calcPerfNodeTime[CALC_PERF_NODE_TIME_COUNT];
static NodeId g_calcPerfNodeParent = 0;
static int g_calcPerfNodeIndex = 0;
static int g_nodeSetPerfCalcIndex = -1;
static std::string g_testDataFrame;
static std::vector<RSRenderNode::SharedPtr> g_childOfDisplayNodes;

#pragma pack(push, 1)
struct AlignedMessageParcel {
    uint8_t pad = 0u;
    MessageParcel parcel;
};
#pragma pack(pop)

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

    if (!g_context) {
        return;
    }
    std::shared_ptr<RSDisplayRenderNode> displayNode = GetDisplayNode(*g_context);
    if (!displayNode) {
        return;
    }
    auto params = static_cast<RSDisplayRenderParams*>(displayNode->GetRenderParams().get());
    if (!params) {
        return;
    }

    auto screenInfo = params->GetScreenInfo();
    const uint64_t displayWidth = screenInfo.width;
    const uint64_t displayHeight = screenInfo.height;
    const uint64_t displayArea = displayWidth * displayHeight;

    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    uint64_t dirtyRegionArea = 0;
    for (const auto& rect : rects) {
        dirtyRegionArea += static_cast<uint64_t>(rect.GetWidth() * rect.GetHeight());
    }

    if (displayArea > 0) {
        g_dirtyRegionPercentage =
            maxPercentValue * static_cast<double>(dirtyRegionArea) / static_cast<double>(displayArea);
    }
    if (g_dirtyRegionPercentage > maxPercentValue) {
        g_dirtyRegionPercentage = maxPercentValue;
    }
}

void RSProfiler::Init(RSRenderService* renderService)
{
    g_renderService = renderService;
    g_mainThread = g_renderService ? g_renderService->mainThread_ : nullptr;
    g_context = g_mainThread ? g_mainThread->context_.get() : nullptr;

    RSSystemProperties::WatchSystemProperty(SYS_KEY_ENABLED, OnFlagChangedCallback, nullptr);
    RSSystemProperties::WatchSystemProperty(SYS_KEY_BETARECORDING, OnFlagChangedCallback, nullptr);
    bool isEnabled = RSSystemProperties::GetProfilerEnabled();
    bool isBetaRecord = RSSystemProperties::GetBetaRecordingMode() != 0;
    HRPD("Profiler flags changed enabled=%{public}d beta_record=%{public}d", isEnabled ? 1 : 0, isBetaRecord ? 1 : 0);

    if (!IsEnabled()) {
        return;
    }

    OnWorkModeChanged();
}

void RSProfiler::StartNetworkThread()
{
    if (Network::IsRunning()) {
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

void RSProfiler::OnRemoteRequest(RSIRenderServiceConnection* connection, uint32_t code, MessageParcel& parcel,
    MessageParcel& /*reply*/, MessageOption& option)
{
    if (!IsEnabled()) {
        return;
    }

    if (IsRecording() && (g_recordStartTime > 0.0)) {
        const pid_t pid = GetConnectionPid(connection);
        const auto& pids = g_recordFile.GetHeaderPids();
        if (std::find(std::begin(pids), std::end(pids), pid) != std::end(pids)) {
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

            const std::string out = stream.str();
            constexpr size_t headerOffset = 8 + 1;
            if (out.size() >= headerOffset) {
                g_recordFile.WriteRSData(deltaTime, out.data() + headerOffset, out.size() - headerOffset);
            }
            Network::SendBinary(out.data(), out.size());
        }
    }

    if (IsLoadSaveFirstScreenInProgress()) {
        // saving screen right now
    }
    if (IsPlaying()) {
        SetTransactionTimeCorrection(g_playbackStartTime, g_playbackFile.GetWriteTime());
        SetSubstitutingPid(g_playbackFile.GetHeaderPids(), g_playbackPid, g_playbackParentNodeId);
        SetMode(Mode::READ);
    } else if (IsRecording()) {
        SetMode(Mode::WRITE);
    } else {
        SetMode(Mode::NONE);
    }
}

void RSProfiler::OnRecvParcel(const MessageParcel* parcel, RSTransactionData* data)
{
    if (!IsEnabled()) {
        return;
    }

    if (parcel && IsParcelMock(*parcel)) {
        data->SetSendingPid(Utils::GetMockPid(data->GetSendingPid()));
    }
}

void RSProfiler::CreateMockConnection(pid_t pid)
{
    if (!IsEnabled() || !g_renderService) {
        return;
    }

    auto tokenObj = new IRemoteStub<RSIConnectionToken>();

    sptr<RSIRenderServiceConnection> newConn(new RSRenderServiceConnection(pid, g_renderService, g_mainThread,
        g_renderService->screenManager_, tokenObj, g_renderService->appVSyncDistributor_));

    sptr<RSIRenderServiceConnection> tmp;

    std::unique_lock<std::mutex> lock(g_renderService->mutex_);
    // if connections_ has the same token one, replace it.
    if (g_renderService->connections_.count(tokenObj) > 0) {
        tmp = g_renderService->connections_.at(tokenObj);
    }
    g_renderService->connections_[tokenObj] = newConn;
    lock.unlock();
    g_mainThread->AddTransactionDataPidInfo(pid);
}

RSRenderServiceConnection* RSProfiler::GetConnection(pid_t pid)
{
    if (!g_renderService) {
        return nullptr;
    }

    for (const auto& pair : g_renderService->connections_) {
        auto connection = static_cast<RSRenderServiceConnection*>(pair.second.GetRefPtr());
        if (connection->remotePid_ == pid) {
            return connection;
        }
    }

    return nullptr;
}

pid_t RSProfiler::GetConnectionPid(RSIRenderServiceConnection* connection)
{
    if (!g_renderService || !connection) {
        return 0;
    }

    for (const auto& pair : g_renderService->connections_) {
        auto renderServiceConnection = static_cast<RSRenderServiceConnection*>(pair.second.GetRefPtr());
        if (renderServiceConnection == connection) {
            return renderServiceConnection->remotePid_;
        }
    }

    return 0;
}

std::vector<pid_t> RSProfiler::GetConnectionsPids()
{
    if (!g_renderService) {
        return {};
    }

    std::vector<pid_t> pids;
    pids.reserve(g_renderService->connections_.size());
    for (const auto& pair : g_renderService->connections_) {
        pids.push_back(static_cast<RSRenderServiceConnection*>(pair.second.GetRefPtr())->remotePid_);
    }
    return pids;
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
        RecordStop(ArgList());
        Network::Stop();
    }
}

void RSProfiler::ProcessSignalFlag()
{
    if (signalFlagChanged_ <= 0) {
        return;
    }

    signalFlagChanged_--;
    if (!signalFlagChanged_) {
        bool newEnabled = RSSystemProperties::GetProfilerEnabled();
        bool newBetaRecord = RSSystemProperties::GetBetaRecordingMode() != 0;
        HRPD("Profiler flags changed enabled=%{public}d beta_record=%{public}d", newEnabled ? 1 : 0,
            newBetaRecord ? 1 : 0);
        if (enabled_ && !newEnabled) {
            const ArgList dummy;
            if (GetMode() == Mode::READ) {
                PlaybackStop(dummy);
            }
            if (GetMode() == Mode::WRITE) {
                RecordStop(dummy);
            }
        }
        if (enabled_ != newEnabled || IsBetaRecordEnabled() != newBetaRecord) {
            enabled_ = newEnabled;
            betaRecordingEnabled_ = newBetaRecord;
            OnWorkModeChanged();
        }
    }
    AwakeRenderServiceThread();
}

void RSProfiler::OnProcessCommand()
{
    ProcessSignalFlag();

    if (!IsEnabled()) {
        return;
    }

    if (IsPlaying()) {
        ResetAnimationStamp();
    }
}

void RSProfiler::OnRenderBegin()
{
    if (!IsEnabled()) {
        return;
    }
    HRPD("OnRenderBegin()");
    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::OnRenderEnd()
{
    if (!IsEnabled()) {
        return;
    }
    
    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::OnParallelRenderBegin()
{
    if (!IsEnabled()) {
        return;
    }

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
    const uint64_t frameLengthNanosecs = RawNowNano() - g_frameRenderBeginTimestamp;
    CalcNodeWeigthOnFrameEnd(frameLengthNanosecs);

    if (!IsRecording() || (g_recordStartTime <= 0.0)) {
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
        g_recordFile.WriteRSMetrics(0, timeSinceRecordStart, out.data(), out.size());
    }
}

bool RSProfiler::ShouldBlockHWCNode()
{
    if (!IsEnabled()) {
        return false;
    }

    return GetMode() == Mode::READ;
}

void RSProfiler::OnFrameBegin()
{
    if (!IsEnabled()) {
        return;
    }

    g_frameBeginTimestamp = RawNowNano();
    g_renderServiceCpuId = Utils::GetCpuId();
    g_frameNumber++;

    StartBetaRecord();
}

void RSProfiler::ProcessPauseMessage()
{
    if (!IsPlaying()) {
        return;
    }

    uint64_t pauseAtTime = TimePauseGet();
    uint64_t nowTime = RawNowNano();
    if (pauseAtTime > 0 && nowTime > pauseAtTime) {
        const double recordPlayTime = Now() - g_playbackStartTime;
        if (recordPlayTime > g_replayLastPauseTimeReported) {
            int64_t vsyncId = g_playbackFile.ConvertTime2VsyncId(recordPlayTime);
            if (vsyncId) {
                Respond("Replay timer paused vsyncId=" + std::to_string(vsyncId));
            }
            g_replayLastPauseTimeReported = recordPlayTime;
        }
    }
}

void RSProfiler::OnFrameEnd()
{
    if (!IsEnabled()) {
        return;
    }

    g_renderServiceCpuId = Utils::GetCpuId();
    ProcessCommands();
    ProcessSendingRdc();
    RecordUpdate();

    UpdateDirtyRegionBetaRecord(g_dirtyRegionPercentage);
    UpdateBetaRecord();

    ProcessPauseMessage();

    g_renderServiceCpuId = Utils::GetCpuId();

    std::string value;
    constexpr int maxMsgPerFrame = 32;
    value = SendMessageBase();
    for (int i = 0; value != "" && i < maxMsgPerFrame; value = SendMessageBase(), i++) {
        if (!value.length()) {
            break;
        }
        Network::SendMessage(value);
    }
}

void RSProfiler::CalcNodeWeigthOnFrameEnd(uint64_t frameLength)
{
    g_renderServiceRenderCpuId = Utils::GetCpuId();

    if (g_calcPerfNode == 0 || g_calcPerfNodeTry < 0 || g_calcPerfNodeTry >= CALC_PERF_NODE_TIME_COUNT) {
        return;
    }

    g_calcPerfNodeTime[g_calcPerfNodeTry] = frameLength;
    g_calcPerfNodeTry++;
    if (g_calcPerfNodeTry < CALC_PERF_NODE_TIME_COUNT) {
        AwakeRenderServiceThreadResetCaches();
        return;
    }

    constexpr NodeId renderEntireFrame = 1;

    std::sort(std::begin(g_calcPerfNodeTime), std::end(g_calcPerfNodeTime));
    constexpr int32_t middle = CALC_PERF_NODE_TIME_COUNT / 2;
    Respond("CALC_PERF_NODE_RESULT: " + std::to_string(g_calcPerfNode) + " " + "cnt=" +
            std::to_string(g_mapNode2Count[g_calcPerfNode]) + " " + std::to_string(g_calcPerfNodeTime[middle]));
    Network::SendRSTreeSingleNodePerf(g_calcPerfNode, g_calcPerfNodeTime[middle]);

    if (g_calcPerfNode != renderEntireFrame) {
        auto parent = GetRenderNode(g_calcPerfNodeParent);
        auto child = parent ? GetRenderNode(g_calcPerfNode) : nullptr;
        if (child) {
            parent->AddChild(child, g_calcPerfNodeIndex);
        }
    }

    g_calcPerfNode = 0;
    g_calcPerfNodeParent = 0;
    g_calcPerfNodeIndex = 0;
    g_calcPerfNodeTry = 0;

    if (g_nodeSetPerfCalcIndex >= 0) {
        g_nodeSetPerfCalcIndex++;
        CalcPerfNodeAllStep();
    }

    AwakeRenderServiceThread();
    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::RenderServiceTreeDump(JsonWriter& out, pid_t pid)
{
    RS_TRACE_NAME("GetDumpTreeJSON");

    if (!g_context) {
        return;
    }

    const bool useMockPid = pid > 0 && GetMode() == Mode::READ;
    if (useMockPid) {
        pid = Utils::GetMockPid(pid);
    }

    auto& animation = out["Animation Node"];
    animation.PushArray();
    for (auto& [nodeId, _] : g_context->animatingNodeList_) {
        if (!pid) {
            animation.Append(nodeId);
        } else if (pid == Utils::ExtractPid(nodeId)) {
            animation.Append(nodeId);
        }
    }
    animation.PopArray();

    auto rootNode = g_context->GetGlobalRootRenderNode();
    if (pid) {
        rootNode = nullptr;
        auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
        nodeMap.TraversalNodes([&rootNode, pid](const std::shared_ptr<RSBaseRenderNode>& node) {
            if (node == nullptr) {
                return;
            }
            auto parentPtr = node->GetParent().lock();
            if (parentPtr != nullptr && !rootNode &&
                Utils::ExtractPid(node->GetId()) != Utils::ExtractPid(parentPtr->GetId()) &&
                Utils::ExtractPid(node->GetId()) == pid) {
                rootNode = node;
                Respond("Root node found: " + std::to_string(rootNode->GetId()));
            }
        });
    }

    auto& root = out["Root node"];
    if (rootNode) {
        DumpNode(*rootNode, root, useMockPid, pid > 0);
    } else {
        Respond("rootNode not found");
        root.PushObject();
        root.PopObject();
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
    return IsEnabled() && g_recordFile.IsOpen();
}

bool RSProfiler::IsPlaying()
{
    return IsEnabled() && g_playbackFile.IsOpen();
}

void RSProfiler::ScheduleTask(std::function<void()> && task)
{
    if (g_mainThread) {
        g_mainThread->PostTask(std::move(task));
    }
}

void RSProfiler::RequestNextVSync()
{
    if (g_mainThread) {
        g_mainThread->RequestNextVSync();
    }
    ScheduleTask([]() { g_mainThread->RequestNextVSync(); });
}

void RSProfiler::AwakeRenderServiceThread()
{
    if (g_mainThread) {
        g_mainThread->RequestNextVSync();
        g_mainThread->SetAccessibilityConfigChanged();
        g_mainThread->SetDirtyFlag();
    }
    ScheduleTask([]() {
        g_mainThread->SetAccessibilityConfigChanged();
        g_mainThread->SetDirtyFlag();
        g_mainThread->RequestNextVSync();
    });
}

void RSProfiler::AwakeRenderServiceThreadResetCaches()
{
    RSMainThread::Instance()->PostTask([]() {
        RSMainThread::Instance()->SetAccessibilityConfigChanged();

        auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
        nodeMap.TraversalNodes([](const std::shared_ptr<RSBaseRenderNode>& node) {
            if (node == nullptr) {
                return;
            }
            node->SetSubTreeDirty(true);
            node->SetContentDirty();
            node->SetDirty();
        });

        nodeMap.TraverseSurfaceNodes([](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if (surfaceNode == nullptr) {
                return;
            }
            surfaceNode->NeedClearBufferCache();
            surfaceNode->GetRSSurfaceHandler()->ResetBufferAvailableCount();
            surfaceNode->GetRSSurfaceHandler()->CleanCache();
            surfaceNode->UpdateBufferInfo(nullptr, {}, nullptr, nullptr);
            surfaceNode->SetNotifyRTBufferAvailable(false);
            surfaceNode->SetContentDirty();
            surfaceNode->ResetHardwareEnabledStates();
        });

        RSMainThread::Instance()->RequestNextVSync();
    });
}


void RSProfiler::ResetAnimationStamp()
{
    if (g_mainThread && g_context) {
        g_mainThread->lastAnimateTimestamp_ = g_context->GetCurrentTimestamp();
    }
}

std::shared_ptr<RSRenderNode> RSProfiler::GetRenderNode(uint64_t id)
{
    return g_context ? g_context->GetMutableNodeMap().GetRenderNode(id) : nullptr;
}

bool RSProfiler::IsLoadSaveFirstScreenInProgress()
{
    return (GetMode() == Mode::WRITE_EMUL || GetMode() == Mode::READ_EMUL);
}

void RSProfiler::HiddenSpaceTurnOn()
{
    if (!g_childOfDisplayNodes.empty()) {
        HiddenSpaceTurnOff();
    }

    const auto& rootRenderNode = g_context->GetGlobalRootRenderNode();
    if (rootRenderNode == nullptr) {
        RS_LOGE("RSProfiler::HiddenSpaceTurnOn rootRenderNode is nullptr");
        return;
    }
    auto& children = *rootRenderNode->GetChildren();
    if (children.empty()) {
        return;
    }
    if (auto& displayNode = children.front()) {
        if (auto rootNode = GetRenderNode(Utils::PatchNodeId(0))) {
            g_childOfDisplayNodes = *displayNode->GetChildren();

            displayNode->ClearChildren();
            displayNode->AddChild(rootNode);
        }
    }

    g_mainThread->SetDirtyFlag();
    AwakeRenderServiceThread();
}

void RSProfiler::HiddenSpaceTurnOff()
{
    const auto& rootRenderNode = g_context->GetGlobalRootRenderNode();
    if (rootRenderNode == nullptr) {
        RS_LOGE("RSProfiler::HiddenSpaceTurnOff rootRenderNode is nullptr");
        return;
    }
    const auto& children = *rootRenderNode->GetChildren();
    if (children.empty()) {
        return;
    }
    if (auto& displayNode = children.front()) {
        displayNode->ClearChildren();
        for (const auto& child : g_childOfDisplayNodes) {
            displayNode->AddChild(child);
        }
        FilterMockNode(*g_context);
        RSTypefaceCache::Instance().ReplayClear();
        g_childOfDisplayNodes.clear();
    }

    g_mainThread->SetDirtyFlag();

    AwakeRenderServiceThread();
}

std::string RSProfiler::FirstFrameMarshalling(uint32_t fileVersion)
{
    if (!g_context) {
        return "";
    }

    std::stringstream stream;
    TypefaceMarshalling(stream, fileVersion);

    SetMode(Mode::WRITE_EMUL);
    DisableSharedMemory();
    MarshalNodes(*g_context, stream, fileVersion);
    EnableSharedMemory();
    SetMode(Mode::NONE);

    const int32_t focusPid = g_mainThread->focusAppPid_;
    stream.write(reinterpret_cast<const char*>(&focusPid), sizeof(focusPid));

    const int32_t focusUid = g_mainThread->focusAppUid_;
    stream.write(reinterpret_cast<const char*>(&focusUid), sizeof(focusUid));

    const uint64_t focusNodeId = g_mainThread->focusNodeId_;
    stream.write(reinterpret_cast<const char*>(&focusNodeId), sizeof(focusNodeId));

    const std::string bundleName = g_mainThread->focusAppBundleName_;
    size_t size = bundleName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(bundleName.data()), size);

    const std::string abilityName = g_mainThread->focusAppAbilityName_;
    size = abilityName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(abilityName.data()), size);

    return stream.str();
}

void RSProfiler::FirstFrameUnmarshalling(const std::string& data, uint32_t fileVersion)
{
    std::stringstream stream;
    stream.str(data);

    TypefaceUnmarshalling(stream, fileVersion);

    SetMode(Mode::READ_EMUL);

    DisableSharedMemory();
    UnmarshalNodes(*g_context, stream, fileVersion);
    EnableSharedMemory();

    SetMode(Mode::NONE);

    int32_t focusPid = 0;
    stream.read(reinterpret_cast<char*>(&focusPid), sizeof(focusPid));

    int32_t focusUid = 0;
    stream.read(reinterpret_cast<char*>(&focusUid), sizeof(focusUid));

    uint64_t focusNodeId = 0;
    stream.read(reinterpret_cast<char*>(&focusNodeId), sizeof(focusNodeId));

    size_t size = 0;
    stream.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::string bundleName;
    bundleName.resize(size, ' ');
    stream.read(reinterpret_cast<char*>(bundleName.data()), size);

    stream.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::string abilityName;
    abilityName.resize(size, ' ');
    stream.read(reinterpret_cast<char*>(abilityName.data()), size);

    focusPid = Utils::GetMockPid(focusPid);
    focusNodeId = Utils::PatchNodeId(focusNodeId);

    CreateMockConnection(focusPid);
    g_mainThread->SetFocusAppInfo(focusPid, focusUid, bundleName, abilityName, focusNodeId);
}

void RSProfiler::TypefaceMarshalling(std::stringstream& stream, uint32_t fileVersion)
{
    if (fileVersion >= RSFILE_VERSION_RENDER_TYPEFACE_FIX) {
        std::stringstream fontStream;
        RSTypefaceCache::Instance().ReplaySerialize(fontStream);
        size_t fontStreamSize = fontStream.str().size();
        stream.write(reinterpret_cast<const char*>(&fontStreamSize), sizeof(fontStreamSize));
        stream.write(reinterpret_cast<const char*>(fontStream.str().c_str()), fontStreamSize);
    }
}

void RSProfiler::TypefaceUnmarshalling(std::stringstream& stream, uint32_t fileVersion)
{
    if (fileVersion >= RSFILE_VERSION_RENDER_TYPEFACE_FIX) {
        std::vector<uint8_t> fontData;
        std::stringstream fontStream;
        size_t fontStreamSize;
        stream.read(reinterpret_cast<char*>(&fontStreamSize), sizeof(fontStreamSize));
        fontData.resize(fontStreamSize);
        stream.read(reinterpret_cast<char*>(fontData.data()), fontStreamSize);
        fontStream.write(reinterpret_cast<const char*>(fontData.data()), fontStreamSize);
        RSTypefaceCache::Instance().ReplayDeserialize(fontStream);
    }
}

void RSProfiler::SaveRdc(const ArgList& args)
{
    g_rdcSent = false;
    RSSystemProperties::SetSaveRDC(true);
    RSSystemProperties::SetInstantRecording(true);

    AwakeRenderServiceThread();
    Respond("Recording current frame cmds (for .rdc) into : /data/default.drawing");
}

void RSProfiler::SaveSkp(const ArgList& args)
{
    const auto nodeId = args.Node();
    RSCaptureRecorder::GetInstance().SetDrawingCanvasNodeId(nodeId);
    if (nodeId == 0) {
        RSSystemProperties::SetInstantRecording(true);
        Respond("Recording full frame .skp");
    } else {
        Respond("Recording .skp for DrawingCanvasNode: id=" + std::to_string(nodeId));
    }
    AwakeRenderServiceThread();
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

void RSProfiler::RecordUpdate()
{
    if (!IsRecording() || (g_recordStartTime <= 0.0)) {
        return;
    }

    const uint64_t frameLengthNanosecs = RawNowNano() - g_frameBeginTimestamp;

    const double currentTime = Utils::ToSeconds(g_frameBeginTimestamp);
    const double timeSinceRecordStart = currentTime - g_recordStartTime;

    if (timeSinceRecordStart > 0.0) {
        RSCaptureData captureData;
        captureData.SetTime(timeSinceRecordStart);
        captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_NUMBER, g_frameNumber);
        captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_LEN, frameLengthNanosecs);
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_COUNT, GetCommandCount());
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_EXECUTE_COUNT, GetCommandExecuteCount());
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_PARCEL_LIST, GetCommandParcelList(g_recordStartTime));
        captureData.SetProperty(RSCaptureData::KEY_RS_PIXEL_IMAGE_ADDED, GetImagesAdded());
        captureData.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION, floor(g_dirtyRegionPercentage));
        captureData.SetProperty(RSCaptureData::KEY_RS_CPU_ID, g_renderServiceCpuId.load());
        uint64_t vsyncId = g_mainThread ? g_mainThread->vsyncId_ : 0;
        captureData.SetProperty(RSCaptureData::KEY_RS_VSYNC_ID, vsyncId);
        if (!g_recordMinVsync) {
            g_recordMinVsync = vsyncId;
        }
        if (g_recordMaxVsync < vsyncId) {
            g_recordMaxVsync = vsyncId;
        }

        std::vector<char> out;
        DataWriter archive(out);
        char headerType = static_cast<char>(PackageID::RS_PROFILER_RS_METRICS);
        archive.Serialize(headerType);
        captureData.Serialize(archive);

        Network::SendBinary(out.data(), out.size());
        g_recordFile.WriteRSMetrics(0, timeSinceRecordStart, out.data(), out.size());
    }

    WriteBetaRecordMetrics(g_recordFile, timeSinceRecordStart);
}

void RSProfiler::Respond(const std::string& message)
{
    if (!message.empty()) {
        Network::SendMessage(message);
        HRPI("%s", message.data());
    }
}

void RSProfiler::SetSystemParameter(const ArgList& args)
{
    if (!SystemParameter::Set(args.String(0), args.String(1))) {
        Respond("There is no such a system parameter");
    }
}

void RSProfiler::GetSystemParameter(const ArgList& args)
{
    const auto parameter = SystemParameter::Find(args.String());
    Respond(parameter ? parameter->ToString() : "There is no such a system parameter");
}

void RSProfiler::Reset(const ArgList& args)
{
    const ArgList dummy;
    RecordStop(dummy);
    PlaybackStop(dummy);

    Utils::FileDelete(RSFile::GetDefaultPath());

    Respond("Reset");
}

void RSProfiler::DumpSystemParameters(const ArgList& args)
{
    Respond(SystemParameter::Dump());
}

void RSProfiler::DumpConnections(const ArgList& args)
{
    if (!g_renderService) {
        return;
    }

    std::string out;
    for (const auto& pid : GetConnectionsPids()) {
        out += "pid=" + std::to_string(pid);

        const std::string path = "/proc/" + std::to_string(pid) + "/cmdline";
        FILE* file = Utils::FileOpen(path, "r");
        if (const size_t size = Utils::FileSize(file)) {
            std::string content;
            content.resize(size);
            Utils::FileRead(file, content.data(), content.size());
            Utils::FileClose(file);
            out += " ";
            out += content;
        }
        if (Utils::IsFileValid(file)) {
            Utils::FileClose(file);
        }
        out += "\n";
    }
    Respond(out);
}

void RSProfiler::DumpNodeModifiers(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        Respond("Modifiers=" + DumpModifiers(*node));
    }
}

void RSProfiler::DumpNodeProperties(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        Respond("RenderProperties=" + DumpRenderProperties(*node));
    }
}

void RSProfiler::DumpDrawingCanvasNodes(const ArgList& args)
{
    if (!g_context) {
        return;
    }
    const auto& map = const_cast<RSContext&>(*g_context).GetMutableNodeMap();
    for (const auto& item : map.renderNodeMap_) {
        if (item.second->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
            Respond("CANVAS_DRAWING_NODE: " + std::to_string(item.second->GetId()));
        }
    }
}

void RSProfiler::DrawingCanvasRedrawEnable(const ArgList& args)
{
    const auto enable = args.Uint64(); // 0 - disabled, >0 - enabled
    RSProfiler::SetDrawingCanvasNodeRedraw(enable > 0);
}

void RSProfiler::DumpTree(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    std::map<std::string, std::tuple<NodeId, std::string>> list;
    GetSurfacesTrees(*g_context, list);

    std::string out = "Tree: count=" + std::to_string(static_cast<int>(GetRenderNodeCount(*g_context))) +
                      " time=" + std::to_string(Now()) + "\n";

    const std::string& node = args.String();
    for (const auto& item : list) {
        if (std::strstr(item.first.data(), node.data())) {
            out += "*** " + item.first + " pid=" + std::to_string(ExtractPid(std::get<0>(item.second))) +
                   " lowId=" + std::to_string(Utils::ExtractNodeId(std::get<0>(item.second))) + "\n" +
                   std::get<1>(item.second) + "\n";
        }
    }

    Respond(out);
}

void RSProfiler::DumpTreeToJson(const ArgList& args)
{
    if (!g_context || !g_mainThread) {
        return;
    }

    JsonWriter json;
    json.PushObject();
    pid_t pid = args.Pid();
    RenderServiceTreeDump(json, pid);

    auto& display = json["Display"];
    auto displayNode = GetDisplayNode(*g_context);
    auto dirtyManager = displayNode ? displayNode->GetDirtyManager() : nullptr;
    if (dirtyManager) {
        const auto displayRect = dirtyManager->GetSurfaceRect();
        display = { displayRect.GetLeft(), displayRect.GetTop(), displayRect.GetRight(), displayRect.GetBottom() };
    } else {
        display = { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    json["transactionFlags"] = g_mainThread->transactionFlags_;
    json["timestamp"] = g_mainThread->timestamp_;
    json["vsyncID"] = g_mainThread->vsyncId_;

    json.PopObject();
    Network::SendRSTreeDumpJSON(json.GetDumpString());

    Respond(json.GetDumpString());
}

void RSProfiler::DumpSurfaces(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    std::map<NodeId, std::string> surfaces;
    GetSurfacesTrees(*g_context, args.Pid(), surfaces);

    std::string out;
    for (const auto& item : surfaces) {
        out += "*** " + std::to_string(item.first) + " pid=" + std::to_string(ExtractPid(item.first)) +
               " lowId=" + std::to_string(Utils::ExtractNodeId(item.first)) + "\n" + item.second + "\n";
    }

    out += "TREE: count=" + std::to_string(static_cast<int32_t>(GetRenderNodeCount(*g_context))) +
           " time=" + std::to_string(Now()) + "\n";

    Respond(out);
}

void RSProfiler::DumpNodeSurface(const ArgList& args)
{
    if (g_renderService) {
        std::string out;
        g_renderService->DumpSurfaceNode(out, Utils::GetRootNodeId(args.Node()));
        Respond(out);
    }
}

void RSProfiler::PatchNode(const ArgList& args)
{
    const auto node = GetRenderNode(args.Node());
    if (!node) {
        return;
    }

    const Vector4f screenRect = GetScreenRect(*g_context);

    auto surfaceNode = static_cast<RSSurfaceRenderNode*>(node.get());
    {
        auto& region = const_cast<Occlusion::Region&>(surfaceNode->GetVisibleRegion());
        region = Occlusion::Region({ screenRect[0], screenRect[1], screenRect[2], screenRect[3] }); // NOLINT
    }

    RSProperties& properties = node->GetMutableRenderProperties();
    properties.SetBounds(screenRect);
    properties.SetFrame(screenRect);
    properties.SetDrawRegion(std::make_shared<RectF>(screenRect));

    AwakeRenderServiceThread();
}

void RSProfiler::KillNode(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        node->RemoveFromTree(false);
        AwakeRenderServiceThread();
        Respond("OK");
    }
}

void RSProfiler::AttachChild(const ArgList& args)
{
    constexpr size_t parentIndex = 0;
    constexpr size_t childIndex = parentIndex + 2;

    auto parent = GetRenderNode(Utils::ComposeNodeId(args.Pid(parentIndex), args.Node(parentIndex + 1)));
    auto child = GetRenderNode(Utils::ComposeNodeId(args.Pid(childIndex), args.Node(childIndex + 1)));
    if (parent && child) {
        parent->AddChild(child);
        AwakeRenderServiceThread();
        Respond("OK");
    }
}

void RSProfiler::KillPid(const ArgList& args)
{
    const pid_t pid = args.Pid();
    if (const auto node = GetRenderNode(Utils::GetRootNodeId(static_cast<uint64_t>(pid)))) {
        const auto parent = node->GetParent().lock();
        const std::string out =
            "parentPid=" + std::to_string(GetPid(parent)) + " parentNode=" + std::to_string(GetNodeId(parent));

        g_context->GetMutableNodeMap().FilterNodeByPid(pid);
        AwakeRenderServiceThread();
        Respond(out);
    }
}

void RSProfiler::GetRoot(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    std::string out;

    const RSRenderNodeMap& map = g_context->GetMutableNodeMap();
    std::shared_ptr<RSRenderNode> node = map.GetRenderNode<RSRenderNode>(GetRandomSurfaceNode(*g_context));
    while (node && (node->GetId() != 0)) {
        std::string type;
        const RSRenderNodeType nodeType = node->GetType();
        if (nodeType == RSRenderNodeType::UNKNOW) {
            type = "UNKNOWN";
        } else if (nodeType == RSRenderNodeType::RS_NODE) {
            type = "NONE";
        } else if (nodeType == RSRenderNodeType::DISPLAY_NODE) {
            type = "DISPLAY_NODE";
        } else if (nodeType == RSRenderNodeType::EFFECT_NODE) {
            type = "EFFECT_NODE";
        } else if (nodeType == RSRenderNodeType::ROOT_NODE) {
            type = "ROOT_NODE";
        } else if (nodeType == RSRenderNodeType::CANVAS_DRAWING_NODE) {
            type = "CANVAS_DRAWING_NODE";
        }

        if (!type.empty()) {
            out += "pid=" + std::to_string(GetPid(node)) + " node_id=" + std::to_string(GetNodeId(node)) + "|" + type +
                   ";\n";
        }

        node = node->GetParent().lock();
    };

    if (node) {
        out += "ROOT_ID=" + std::to_string(node->GetId()); // DISPLAY_NODE;ohos.sceneboard
    }

    Respond(out);
}

void RSProfiler::GetDeviceInfo(const ArgList& args)
{
    Respond(RSTelemetry::GetDeviceInfoString());
}

void RSProfiler::GetDeviceFrequency(const ArgList& args)
{
    Respond(RSTelemetry::GetDeviceFrequencyString());
    Respond(RSTelemetry::GetCpuAffinityString());
}

void RSProfiler::FixDeviceEnv(const ArgList& args)
{
    constexpr int32_t cpu = 8;
    Utils::SetCpuAffinity(cpu);
    Respond("OK");
}

void RSProfiler::GetPerfTree(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    g_nodeSetPerf.clear();
    g_mapNode2Count.clear();

    auto& rootNode = g_context->GetGlobalRootRenderNode();
    auto rootNodeChildren = rootNode ? rootNode->GetSortedChildren() : nullptr;
    if (!rootNodeChildren) {
        Respond("ERROR");
        return;
    }

    for (const auto& child : *rootNodeChildren) {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(child);
        if (!displayNode) {
            continue;
        }
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        displayNode->CollectSurface(displayNode, curAllSurfaces, true, false);
        for (auto& node : curAllSurfaces) {
            if (node) {
                PerfTreeFlatten(*node, g_nodeSetPerf, g_mapNode2Count);
            }
        }
    }

    std::string outString;
    auto& nodeMap = g_context->GetMutableNodeMap();
    for (auto it = g_nodeSetPerf.begin(); it != g_nodeSetPerf.end(); it++) {
        auto node = nodeMap.GetRenderNode(*it);
        if (!node) {
            continue;
        }
        std::string sNodeType;
        RSRenderNode::DumpNodeType(node->GetType(), sNodeType);
        outString += (it != g_nodeSetPerf.begin() ? ", " : "") + std::to_string(*it) + ":" +
                     std::to_string(g_mapNode2Count[*it]) + " [" + sNodeType + "]";
    }

    Network::SendRSTreePerfNodeList(g_nodeSetPerf);
    Respond("OK: Count=" + std::to_string(g_nodeSetPerf.size()) + " LIST=[" + outString + "]");
}

void RSProfiler::CalcPerfNode(const ArgList& args)
{
    g_calcPerfNode = args.Uint64();
    auto node = GetRenderNode(g_calcPerfNode);
    const auto parent = node ? node->GetParent().lock() : nullptr;
    auto parentChildren = parent ? parent->GetChildren() : nullptr;
    if (!parentChildren) {
        return;
    }

    int index = 0;
    g_calcPerfNodeParent = parent->GetId();
    for (const auto& child : *parentChildren) {
        if (child && child->GetId() == node->GetId()) {
            g_calcPerfNodeIndex = index;
        }
    }

    parent->RemoveChild(node);
    Respond("CalcPerfNode: NodeRemoved index=" + std::to_string(index));
    AwakeRenderServiceThread();
}

void RSProfiler::SocketShutdown(const ArgList& args)
{
    Network::ForceShutdown();
}

void RSProfiler::Version(const ArgList& args)
{
    Respond("Version: " + std::to_string(RSFILE_VERSION_LATEST));
}

void RSProfiler::FileVersion(const ArgList& args)
{
    Respond("File version: " + std::to_string(RSFILE_VERSION_LATEST));
}

void RSProfiler::CalcPerfNodeAll(const ArgList& args)
{
    if (g_nodeSetPerf.empty()) {
        Respond("ERROR");
        return;
    }

    g_nodeSetPerfCalcIndex = 0;
    CalcPerfNodeAllStep();

    AwakeRenderServiceThread();
}

void RSProfiler::CalcPerfNodeAllStep()
{
    if (g_nodeSetPerfCalcIndex < 0) {
        return;
    }

    if (g_nodeSetPerfCalcIndex == 0) {
        g_calcPerfNode = 1;
        AwakeRenderServiceThread();
        return;
    }

    if (g_nodeSetPerfCalcIndex - 1 < static_cast<int32_t>(g_nodeSetPerf.size())) {
        auto it = g_nodeSetPerf.begin();
        std::advance(it, g_nodeSetPerfCalcIndex - 1);
        g_calcPerfNode = *it;
    } else {
        g_nodeSetPerfCalcIndex = -1;
        return;
    }

    auto node = GetRenderNode(g_calcPerfNode);
    const auto parent = node ? node->GetParent().lock() : nullptr;
    auto parentChildren = parent ? parent->GetChildren() : nullptr;
    if (!parentChildren) {
        g_nodeSetPerfCalcIndex = -1;
        return;
    }

    int index = 0;
    g_calcPerfNodeParent = parent->GetId();
    for (const auto& child : *parentChildren) {
        if (child && child->GetId() == node->GetId()) {
            g_calcPerfNodeIndex = index;
        }
    }

    parent->RemoveChild(node);
    AwakeRenderServiceThread();
}

void RSProfiler::TestSaveFrame(const ArgList& args)
{
    g_testDataFrame = FirstFrameMarshalling(RSFILE_VERSION_LATEST);
    Respond("Save Frame Size: " + std::to_string(g_testDataFrame.size()));
}

void RSProfiler::TestLoadFrame(const ArgList& args)
{
    FirstFrameUnmarshalling(g_testDataFrame, RSFILE_VERSION_LATEST);
    Respond("Load Frame Size: " + std::to_string(g_testDataFrame.size()));
}

void RSProfiler::TestSwitch(const ArgList& args)
{
    if (g_childOfDisplayNodes.empty()) {
        HiddenSpaceTurnOn();
        Respond("OK: HiddenSpaceTurnOn");
    } else {
        HiddenSpaceTurnOff();
        Respond("OK: HiddenSpaceTurnOff");
    }
}

void RSProfiler::RecordStart(const ArgList& args)
{
    g_recordStartTime = 0.0;

    ImageCache::Reset();
    g_lastCacheImageCount = 0;

    if (!OpenBetaRecordFile(g_recordFile)) {
        g_recordFile.SetVersion(RSFILE_VERSION_LATEST);
        g_recordFile.Create(RSFile::GetDefaultPath());
    }

    g_recordMinVsync = 0;
    g_recordMaxVsync = 0;

    g_recordFile.AddLayer(); // add 0 layer

    FilterMockNode(*g_context);
    RSTypefaceCache::Instance().ReplayClear();

    g_recordFile.AddHeaderFirstFrame(FirstFrameMarshalling(g_recordFile.GetVersion()));

    const std::vector<pid_t> pids = GetConnectionsPids();
    for (pid_t pid : pids) {
        g_recordFile.AddHeaderPid(pid);
    }

    SetMode(Mode::WRITE);

    g_recordStartTime = Now();
    g_frameNumber = 0;

    if (IsBetaRecordStarted()) {
        return;
    }

    std::thread thread([]() {
        while (IsRecording()) {
            if (g_recordStartTime >= 0) {
                SendTelemetry(Now() - g_recordStartTime);
            }
            static constexpr int32_t GFX_METRICS_SEND_INTERVAL = 8;
            std::this_thread::sleep_for(std::chrono::milliseconds(GFX_METRICS_SEND_INTERVAL));
        }
    });
    thread.detach();

    Respond("Network: Record start");
}

void RSProfiler::RecordStop(const ArgList& args)
{
    if (!IsRecording()) {
        return;
    }

    g_recordFile.SetWriteTime(g_recordStartTime);

    std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);

    if (!IsBetaRecordStarted()) {
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
        uint32_t sizeFirstFrame = static_cast<uint32_t>(g_recordFile.GetHeaderFirstFrame().size());
        stream.write(reinterpret_cast<const char*>(&sizeFirstFrame), sizeof(sizeFirstFrame));
        stream.write(reinterpret_cast<const char*>(&g_recordFile.GetHeaderFirstFrame()[0]), sizeFirstFrame);

        // ANIME START TIMES
        std::vector<std::pair<uint64_t, int64_t>> headerAnimeStartTimes;
        std::unordered_map<AnimationId, std::vector<int64_t>> &headerAnimeStartTimesMap = AnimeGetStartTimes();
        for (const auto& item : headerAnimeStartTimesMap) {
            for (const auto time : item.second) {
                headerAnimeStartTimes.push_back({
                    Utils::PatchNodeId(item.first),
                    time - Utils::ToNanoseconds(g_recordStartTime)
                });
            }
        }

        uint32_t startTimesSize = headerAnimeStartTimes.size();
        stream.write(reinterpret_cast<const char*>(&startTimesSize), sizeof(startTimesSize));
        stream.write(reinterpret_cast<const char*>(headerAnimeStartTimes.data()),
            startTimesSize * sizeof(std::pair<uint64_t, int64_t>));

        ImageCache::Serialize(stream);
        Network::SendBinary(stream.str().data(), stream.str().size());
    }
    SaveBetaRecordFile(g_recordFile);
    g_recordFile.Close();
    g_recordStartTime = 0.0;

    ImageCache::Reset();
    g_lastCacheImageCount = 0;

    Respond("Network: Record stop (" + std::to_string(stream.str().size()) + ")");
    Respond("Network: record_vsync_range " + std::to_string(g_recordMinVsync) + " " + std::to_string(g_recordMaxVsync));
}

void RSProfiler::PlaybackPrepareFirstFrame(const ArgList& args)
{
    g_playbackPid = args.Pid();
    g_playbackStartTime = 0.0;
    g_playbackPauseTime = args.Fp64(1);
    std::string path = args.String(2);
    if (!Utils::FileExists(path)) {
        Respond("Can't playback non existing file '" + path + "'");
        path = RSFile::GetDefaultPath();
    }

    ImageCache::Reset();

    auto &animeMap = RSProfiler::AnimeGetStartTimes();
    animeMap.clear();

    Respond("Opening file " + path);
    g_playbackFile.Open(path);
    if (!g_playbackFile.IsOpen()) {
        Respond("Can't open file.");
        return;
    }

    if (args.String(0) == "VSYNC") {
        g_playbackFile.CacheVsyncId2Time(0);
        g_playbackPauseTime = g_playbackFile.ConvertVsyncId2Time(args.Int64(1));
    }

    const auto& fileAnimeStartTimes = g_playbackFile.GetAnimeStartTimes();
    for (const auto& item : fileAnimeStartTimes) {
        if (animeMap.count(item.first)) {
            animeMap[Utils::PatchNodeId(item.first)].push_back(item.second);
        } else {
            std::vector<int64_t> list;
            list.push_back(item.second);
            animeMap.insert({ Utils::PatchNodeId(item.first), list });
        }
    }

    std::string dataFirstFrame = g_playbackFile.GetHeaderFirstFrame();

    // get first frame data
    FirstFrameUnmarshalling(dataFirstFrame, g_playbackFile.GetVersion());
    // The number of frames loaded before command processing
    constexpr int defaultWaitFrames = 5;
    g_playbackWaitFrames = defaultWaitFrames;
    Respond("awake_frame " + std::to_string(g_playbackWaitFrames));
    AwakeRenderServiceThread();
}

void RSProfiler::RecordSendBinary(const ArgList& args)
{
    bool flag = args.Int8(0);
    Network::SetBlockBinary(!flag);
    if (flag) {
        Respond("Result: data will be sent to client during recording");
    } else {
        Respond("Result: data will NOT be sent to client during recording");
    }
}

void RSProfiler::PlaybackStart(const ArgList& args)
{
    HiddenSpaceTurnOn();

    for (size_t pid : g_playbackFile.GetHeaderPids()) {
        CreateMockConnection(Utils::GetMockPid(pid));
    }

    g_playbackStartTime = Now();

    const double pauseTime = g_playbackPauseTime;
    if (pauseTime > 0.0) {
        const uint64_t currentTime = RawNowNano();
        const uint64_t pauseTimeStart = currentTime + Utils::ToNanoseconds(pauseTime);
        TimePauseAt(currentTime, pauseTimeStart);
    }

    AwakeRenderServiceThread();

    g_playbackShouldBeTerminated = false;
    g_replayLastPauseTimeReported = 0;

    const auto timeoutLimit = args.Int64();
    std::thread thread([timeoutLimit]() {
        while (IsPlaying()) {
            const int64_t timestamp = static_cast<int64_t>(RawNowNano());

            const double deltaTime = Now() - g_playbackStartTime;
            const double readTime = PlaybackUpdate(deltaTime);

            const int64_t timeout = timeoutLimit - static_cast<int64_t>(RawNowNano()) + timestamp;
            if (timeout > 0) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(timeout));
            }
        }
    });
    thread.detach();

    Respond("Playback start");
}

void RSProfiler::PlaybackStop(const ArgList& args)
{
    if (g_childOfDisplayNodes.empty()) {
        return;
    }
    HiddenSpaceTurnOff();
    FilterMockNode(*g_context);
    RSTypefaceCache::Instance().ReplayClear();
    g_playbackShouldBeTerminated = true;
    g_replayLastPauseTimeReported = 0;

    Respond("Playback stop");
}

double RSProfiler::PlaybackUpdate(double deltaTime)
{
    std::vector<uint8_t> data;
    double readTime = 0.0;
    if (!g_playbackShouldBeTerminated && g_playbackFile.ReadRSData(deltaTime, data, readTime)) {
        std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
        stream.write(reinterpret_cast<const char*>(data.data()), data.size());
        stream.seekg(0);

        pid_t pid = 0;
        stream.read(reinterpret_cast<char*>(&pid), sizeof(pid));

        RSRenderServiceConnection* connection = GetConnection(Utils::GetMockPid(pid));
        if (!connection) {
            const std::vector<pid_t>& pids = g_playbackFile.GetHeaderPids();
            if (!pids.empty()) {
                connection = GetConnection(Utils::GetMockPid(pids[0]));
            }
        }

        if (connection) {
            uint32_t code = 0;
            stream.read(reinterpret_cast<char*>(&code), sizeof(code));

            size_t dataSize = 0;
            stream.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

            auto* data = new uint8_t[dataSize];
            stream.read(reinterpret_cast<char*>(data), dataSize);

            int32_t flags = 0;
            stream.read(reinterpret_cast<char*>(&flags), sizeof(flags));

            int32_t waitTime = 0;
            stream.read(reinterpret_cast<char*>(&waitTime), sizeof(waitTime));

            AlignedMessageParcel parcel;
            parcel.parcel.SetMaxCapacity(dataSize + 1);
            parcel.parcel.WriteBuffer(data, dataSize);

            delete[] data;

            MessageOption option;
            option.SetFlags(flags);
            option.SetWaitTime(waitTime);

            MessageParcel reply;
            connection->OnRemoteRequest(code, parcel.parcel, reply, option);
        }
    }

    if (g_playbackShouldBeTerminated || g_playbackFile.RSDataEOF()) {
        if (auto vsyncId = g_playbackFile.ConvertTime2VsyncId(deltaTime)) {
            Respond("Replay timer paused vsyncId=" + std::to_string(vsyncId));
        }
        g_playbackStartTime = 0.0;
        g_playbackFile.Close();
        g_playbackPid = 0;
        TimePauseClear();
        g_playbackShouldBeTerminated = false;
    }
    return readTime;
}

void RSProfiler::PlaybackPrepare(const ArgList& args)
{
    const pid_t pid = args.Pid();
    if (!g_context || (pid == 0)) {
        return;
    }

    AwakeRenderServiceThread();
    Respond("OK");
}

void RSProfiler::PlaybackPause(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    const uint64_t currentTime = RawNowNano();
    const double recordPlayTime = Utils::ToSeconds(PatchTime(currentTime)) - g_playbackStartTime;
    TimePauseAt(g_frameBeginTimestamp, currentTime);
    Respond("OK: " + std::to_string(recordPlayTime));

    int64_t vsyncId = g_playbackFile.ConvertTime2VsyncId(recordPlayTime);
    if (vsyncId) {
        Respond("Replay timer paused vsyncId=" + std::to_string(vsyncId));
    }
    g_replayLastPauseTimeReported = recordPlayTime;
}

void RSProfiler::PlaybackPauseAt(const ArgList& args)
{
    if (!IsPlaying()) {
        Respond("PlaybackPauseAt REJECT is_playing=false");
        return;
    }

    double pauseTime;
    if (args.String(0) == "VSYNC") {
        int64_t vsyncId = args.Int64(1);
        pauseTime = g_playbackFile.ConvertVsyncId2Time(vsyncId);
    } else {
        pauseTime = args.Fp64();
    }

    const uint64_t currentTime = RawNowNano();
    const double recordPlayTime = Utils::ToSeconds(PatchTime(currentTime)) - g_playbackStartTime;
    if (recordPlayTime > pauseTime) {
        return;
    }

    const uint64_t pauseTimeStart = currentTime + Utils::ToNanoseconds(pauseTime - recordPlayTime);

    TimePauseAt(currentTime, pauseTimeStart);
    ResetAnimationStamp();
    Respond("OK");
}

void RSProfiler::PlaybackPauseClear(const ArgList& args)
{
    TimePauseClear();
    Respond("OK");
}

void RSProfiler::PlaybackResume(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    TimePauseResume(Utils::Now());
    ResetAnimationStamp();
    Respond("OK");
}

void RSProfiler::ProcessCommands()
{
    if (g_playbackWaitFrames > 0) {
        g_playbackWaitFrames--;
        Respond("awake_frame " + std::to_string(g_playbackWaitFrames));
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

} // namespace OHOS::Rosen
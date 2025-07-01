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
#include <fstream>
#include <filesystem>
#include <numeric>
#include <system_error>

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
#include "rs_profiler_test_tree.h"

#include "common/rs_common_def.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "render/rs_typeface_cache.h"

namespace OHOS::Rosen {

// (user): Move to RSProfiler
static RSRenderService* g_renderService = nullptr;
RSContext* RSProfiler::context_ = nullptr;
static RSMainThread* g_mainThread = nullptr;
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

static std::vector<std::pair<NodeId, uint32_t>> g_nodeListPerf;
static std::unordered_map<NodeId, uint32_t> g_mapNode2Count;
static std::unordered_map<NodeId, uint32_t> g_mapNode2UpTime;
static std::unordered_map<NodeId, uint32_t> g_mapNode2UpDownTime;
static NodeId g_calcPerfNode = 0;
static uint32_t g_calcPerfNodeTry = 0;
static bool g_calcPerfNodeExcludeDown = false;

static std::vector<std::pair<std::shared_ptr<RSRenderNode>,
            std::vector<std::shared_ptr<RSRenderNode>>>> g_calcPerfSavedChildren;
static NodeId g_blinkNodeId = 0;
static uint32_t g_blinkNodeCount = 0;
static std::vector<std::shared_ptr<RSRenderNode>> g_blinkSavedParentChildren;

constexpr int CALC_PERF_NODE_TIME_COUNT_MIN = 128; // min render tries
constexpr int CALC_PERF_NODE_TIME_COUNT_MAX = 4096; // max render tries
static uint32_t g_effectiveNodeTimeCount = CALC_PERF_NODE_TIME_COUNT_MAX;
static uint64_t g_calcPerfNodeTime[CALC_PERF_NODE_TIME_COUNT_MAX];
static int g_nodeListPerfCalcIndex = -1;

static std::string g_testDataFrame;
static std::vector<RSRenderNode::SharedPtr> g_childOfDisplayNodes;
static uint32_t g_recordParcelNumber = 0;
static bool g_playbackImmediate = false;

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
#ifdef RS_ENABLE_GPU
    if (!IsRecording()) {
        return;
    }

    const double maxPercentValue = 100.0;
    g_dirtyRegionPercentage = maxPercentValue;

    if (!context_) {
        return;
    }
    std::shared_ptr<RSScreenRenderNode> displayNode = GetScreenNode(*context_);
    if (!displayNode) {
        return;
    }
    // the following logic calcuate the percentage of dirtyRegion
    auto params = static_cast<RSScreenRenderParams*>(displayNode->GetRenderParams().get());
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
#endif
}

void RSProfiler::Init(RSRenderService* renderService)
{
    g_renderService = renderService;
    g_mainThread = g_renderService ? g_renderService->mainThread_ : nullptr;
    context_ = g_mainThread ? g_mainThread->context_.get() : nullptr;

    RSSystemProperties::SetProfilerDisabled();
    RSSystemProperties::WatchSystemProperty(SYS_KEY_ENABLED, OnFlagChangedCallback, nullptr);
    RSSystemProperties::WatchSystemProperty(SYS_KEY_BETARECORDING, OnFlagChangedCallback, nullptr);
    bool isEnabled = RSSystemProperties::GetProfilerEnabled();
    bool isBetaRecord = RSSystemProperties::GetBetaRecordingMode() != 0;
    HRPI("Profiler flags changed enabled=%{public}d beta_record=%{public}d", isEnabled ? 1 : 0, isBetaRecord ? 1 : 0);

    if (!IsEnabled()) {
        return;
    }
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
        g_recordFile.WriteRSData(deltaTime, out.data() + headerOffset, out.size() - headerOffset);
        BetaRecordSetLastParcelTime();
    }
    Network::SendBinary(out.data(), out.size());
    return g_recordParcelNumber;
}

uint64_t RSProfiler::OnRemoteRequest(RSIRenderServiceConnection* connection, uint32_t code,
    MessageParcel& parcel, MessageParcel& /*reply*/, MessageOption& option)
{
    if (!IsEnabled()) {
        return 0;
    }

    if (IsRecording()) {
        constexpr size_t BYTE_SIZE_FOR_ASHMEM = 4;
        if (code == static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION) &&
            parcel.GetDataSize() >= BYTE_SIZE_FOR_ASHMEM) {
            const uint32_t *data = reinterpret_cast<const uint32_t*>(parcel.GetData());
            if (data && *data) {
                // ashmem parcel - don't save
                return 0;
            }
        }
        const pid_t pid = GetConnectionPid(connection);
        const auto& pids = g_recordFile.GetHeaderPids();
        if (std::find(std::begin(pids), std::end(pids), pid) != std::end(pids)) {
            return WriteRemoteRequest(pid, code, parcel, option);
        }
    } else {
        g_recordParcelNumber = 0;
    }

    if (IsLoadSaveFirstScreenInProgress()) {
        // saving screen right now
    }
    if (IsPlaying()) {
        SetTransactionTimeCorrection(g_playbackStartTime, g_playbackFile.GetWriteTime());
        SetSubstitutingPid(g_playbackFile.GetHeaderPids(), g_playbackPid, g_playbackParentNodeId);
        SetMode(Mode::READ);
    }
    return 0;
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

    sptr<RSIRenderServiceConnection> newConn(new RSRenderServiceConnection(pid, g_renderService,
        g_mainThread, g_renderService->screenManager_, tokenObj, g_renderService->appVSyncDistributor_));

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
            if (IsReadMode()) {
                PlaybackStop(dummy);
            }
            if (IsWriteMode()) {
                RecordStop(dummy);
            }
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
    std::shared_ptr<RSScreenRenderNode> screenNode = GetScreenNode(*context_);
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

    BetaRecordOnFrameBegin();
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
        const double deltaTime = PlaybackDeltaTime();
        if (deltaTime > g_replayLastPauseTimeReported) {
            int64_t vsyncId = g_playbackFile.ConvertTime2VsyncId(deltaTime);
            if (vsyncId) {
                SendMessage("Replay timer paused vsyncId=%" PRId64, vsyncId); // DO NOT TOUCH!
            }
            g_replayLastPauseTimeReported = deltaTime;
        }
    }
}

void RSProfiler::OnFrameEnd()
{
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

    std::string value;
    constexpr int maxMsgPerFrame = 1024;
    value = SendMessageBase();
    for (int i = 0; value != "" && i < maxMsgPerFrame; value = SendMessageBase(), i++) {
        if (!value.length()) {
            break;
        }
        Network::SendMessage(value);
    }
    BetaRecordOnFrameEnd();
}

void RSProfiler::CalcNodeWeigthOnFrameEnd(uint64_t frameLength)
{
    if (g_calcPerfNode == 0) {
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

    if (g_calcPerfNodeExcludeDown) {
        Respond("CALC_PERF_NODE_RESULT: U->" + std::to_string(g_calcPerfNode) + " " + std::to_string(avgValue) +
            " inMS=" + std::to_string(avgValue / nanoToMs) + " tries=" + std::to_string(g_effectiveNodeTimeCount));
    } else {
        Respond("CALC_PERF_NODE_RESULT: UD->" + std::to_string(g_calcPerfNode) + " " + std::to_string(avgValue) +
            " inMS=" + std::to_string(avgValue / nanoToMs) + " tries=" + std::to_string(g_effectiveNodeTimeCount));
    }

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

void RSProfiler::ResetAnimationStamp()
{
    if (g_mainThread && context_) {
        g_mainThread->lastAnimateTimestamp_ = context_->GetCurrentTimestamp();
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

std::string RSProfiler::FirstFrameMarshalling(uint32_t fileVersion)
{
    if (!context_) {
        return "";
    }

    RS_TRACE_NAME("Profiler FirstFrameMarshalling");
    std::stringstream stream;
    stream.exceptions(0); // 0: disable all exceptions for stringstream
    TypefaceMarshalling(stream, fileVersion);
    if (!stream.good()) {
        HRPD("strstream error with typeface marshalling");
    }

    SetMode(Mode::WRITE_EMUL);
    DisableSharedMemory();
    MarshalNodes(*context_, stream, fileVersion);
    if (!stream.good()) {
        HRPD("strstream error with marshalling nodes");
    }
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

    if (!stream.good()) {
        HRPE("error with stringstream in FirstFrameMarshalling");
    }
    return stream.str();
}

std::string RSProfiler::FirstFrameUnmarshalling(const std::string& data, uint32_t fileVersion)
{
    std::stringstream stream;
    std::string errReason;

    stream.str(data);

    errReason = TypefaceUnmarshalling(stream, fileVersion);
    if (errReason.size()) {
        return errReason;
    }

    SetMode(Mode::READ_EMUL);

    DisableSharedMemory();
    errReason = UnmarshalNodes(*context_, stream, fileVersion);
    EnableSharedMemory();

    SetMode(Mode::NONE);

    if (errReason.size()) {
        return errReason;
    }

    int32_t focusPid = 0;
    stream.read(reinterpret_cast<char*>(&focusPid), sizeof(focusPid));

    int32_t focusUid = 0;
    stream.read(reinterpret_cast<char*>(&focusUid), sizeof(focusUid));

    uint64_t focusNodeId = 0;
    stream.read(reinterpret_cast<char*>(&focusNodeId), sizeof(focusNodeId));

    constexpr size_t nameSizeMax = 4096;
    size_t size = 0;
    stream.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (size > nameSizeMax) {
        return "FirstFrameUnmarshalling failed, file is damaged";
    }
    std::string bundleName;
    bundleName.resize(size, ' ');
    stream.read(reinterpret_cast<char*>(bundleName.data()), size);

    stream.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (size > nameSizeMax) {
        return "FirstFrameUnmarshalling failed, file is damaged";
    }

    std::string abilityName;
    abilityName.resize(size, ' ');
    stream.read(reinterpret_cast<char*>(abilityName.data()), size);

    focusPid = Utils::GetMockPid(focusPid);
    focusNodeId = Utils::PatchNodeId(focusNodeId);

    CreateMockConnection(focusPid);
    FocusAppInfo info = {
        .pid = focusPid,
        .uid = focusUid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId};
    g_mainThread->SetFocusAppInfo(info);

    return "";
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

std::string RSProfiler::TypefaceUnmarshalling(std::stringstream& stream, uint32_t fileVersion)
{
    if (fileVersion >= RSFILE_VERSION_RENDER_TYPEFACE_FIX) {
        std::vector<uint8_t> fontData;
        std::stringstream fontStream;
        size_t fontStreamSize;
        constexpr size_t fontStreamSizeMax = 100'000'000;
        
        stream.read(reinterpret_cast<char*>(&fontStreamSize), sizeof(fontStreamSize));
        if (fontStreamSize > fontStreamSizeMax) {
            return "Typeface track is damaged";
        }
        fontData.resize(fontStreamSize);
        stream.read(reinterpret_cast<char*>(fontData.data()), fontData.size());
        fontStream.write(reinterpret_cast<const char*>(fontData.data()), fontData.size());
        return RSTypefaceCache::Instance().ReplayDeserialize(fontStream);
    }
    return "";
}

void RSProfiler::HiddenSpaceTurnOn()
{
    if (!g_childOfDisplayNodes.empty()) {
        HiddenSpaceTurnOff();
    }

    const auto& rootRenderNode = context_->GetGlobalRootRenderNode();
    if (rootRenderNode == nullptr) {
        HRPE("RSProfiler::HiddenSpaceTurnOn rootRenderNode is nullptr");
        return;
    }
    const auto& children = *rootRenderNode->GetChildren();
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
    const auto& rootRenderNode = context_->GetGlobalRootRenderNode();
    if (rootRenderNode == nullptr) {
        HRPE("RSProfiler::HiddenSpaceTurnOff rootRenderNode is nullptr");
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
        auto& listPostponed = RSProfiler::GetChildOfDisplayNodesPostponed();
        for (const auto& childWeak : listPostponed) {
            if (auto child = childWeak.lock()) {
                displayNode->AddChild(child);
            }
        }
        listPostponed.clear();
        FilterMockNode(*context_);
        RSTypefaceCache::Instance().ReplayClear();
        g_childOfDisplayNodes.clear();
    }

    g_mainThread->SetDirtyFlag();

    AwakeRenderServiceThread();
}

void RSProfiler::SaveRdc(const ArgList& args)
{
    g_rdcSent = false;
    RSSystemProperties::SetSaveRDC(true);
    RSSystemProperties::SetInstantRecording(true);

    AwakeRenderServiceThread();
    Respond("Recording current frame cmds (for .rdc) into : /data/default.drawing");
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
    if (!IsRecording()) {
        return;
    }

    if (IsRecordAbortRequested()) {
        recordAbortRequested_ = false;
        SendMessage("Record: Exceeded memory limit. Abort"); // DO NOT TOUCH!
        RecordStop(ArgList{});
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
        RSCaptureData captureData;
        captureData.SetTime(timeSinceRecordStart);
        captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_NUMBER, g_frameNumber);
        captureData.SetProperty(RSCaptureData::KEY_RS_SYNC_TIME, timeSinceRecordStartToSync);
        captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_LEN, frameLengthNanosecs);
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_COUNT, GetCommandCount());
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_EXECUTE_COUNT, GetCommandExecuteCount());
        captureData.SetProperty(RSCaptureData::KEY_RS_PARCEL_CMD_LIST, GetParcelCommandList());
        captureData.SetProperty(RSCaptureData::KEY_RS_PIXEL_IMAGE_ADDED, GetImagesAdded());
        captureData.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION, floor(g_dirtyRegionPercentage));
        captureData.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION_LIST, g_dirtyRegionList.str());
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

// Deprecated: Use SendMessage instead
void RSProfiler::Respond(const std::string& message)
{
    Network::SendMessage(message);
}

void RSProfiler::SendMessage(const char* format, ...)
{
    if (!format) {
        return;
    }

    va_list args;
    va_start(args, format);
    const auto out = Utils::Format(format, args);
    va_end(args);

    Network::SendMessage(out);
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

void RSProfiler::DumpDrawingCanvasNodes(const ArgList& args)
{
    if (!context_) {
        return;
    }
    const auto& map = const_cast<RSContext&>(*context_).GetMutableNodeMap();
    for (const auto& [_, subMap] : map.renderNodeMap_) {
        for (const auto& [_, node] : subMap) {
            if (node->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
                Respond("CANVAS_DRAWING_NODE: " + std::to_string(node->GetId()));
            }
        }
    }
}

void RSProfiler::PlaybackSetImmediate(const ArgList& args)
{
    g_playbackImmediate = args.Int64(0) ? true : false;
    Respond("Playback immediate mode: " + std::to_string(g_playbackImmediate));
}

void RSProfiler::DumpTree(const ArgList& args)
{
    if (!context_) {
        return;
    }

    std::map<std::string, std::tuple<NodeId, std::string>> list;
    GetSurfacesTrees(*context_, list);

    std::string out = "Tree: count=" + std::to_string(static_cast<int>(GetRenderNodeCount(*context_))) +
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
    if (!context_ || !g_mainThread) {
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

    json["transactionFlags"] = g_mainThread->transactionFlags_;
    json["timestamp"] = g_mainThread->timestamp_;
    json["vsyncID"] = g_mainThread->vsyncId_;

    json.PopObject();
    Network::SendRSTreeDumpJSON(json.GetDumpString());

    if (args.String() != "NOLOG") {
        Network::SendMessage(json.GetDumpString());
    }
}

void RSProfiler::DumpSurfaces(const ArgList& args)
{
    if (!context_) {
        return;
    }

    std::map<NodeId, std::string> surfaces;
    GetSurfacesTrees(*context_, args.Pid(), surfaces);

    std::string out;
    for (const auto& item : surfaces) {
        out += "*** " + std::to_string(item.first) + " pid=" + std::to_string(ExtractPid(item.first)) +
               " lowId=" + std::to_string(Utils::ExtractNodeId(item.first)) + "\n" + item.second + "\n";
    }

    out += "TREE: count=" + std::to_string(static_cast<int32_t>(GetRenderNodeCount(*context_))) +
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

    const Vector4f screenRect = GetScreenRect(*context_);

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

        context_->GetMutableNodeMap().FilterNodeByPid(pid, true);
        AwakeRenderServiceThread();
        Respond(out);
    }
}

void RSProfiler::GetRoot(const ArgList& args)
{
    if (!context_) {
        return;
    }

    std::string out;

    const RSRenderNodeMap& map = context_->GetMutableNodeMap();
    std::shared_ptr<RSRenderNode> node = map.GetRenderNode<RSRenderNode>(GetRandomSurfaceNode(*context_));
    while (node && (node->GetId() != 0)) {
        std::string type;
        const RSRenderNodeType nodeType = node->GetType();
        if (nodeType == RSRenderNodeType::UNKNOW) {
            type = "UNKNOWN";
        } else if (nodeType == RSRenderNodeType::RS_NODE) {
            type = "NONE";
        } else if (nodeType == RSRenderNodeType::SCREEN_NODE) {
            type = "SCREEN_NODE";
        } else if (nodeType == RSRenderNodeType::LOGICAL_DISPLAY_NODE) {
            type = "LOGICAL_DISPLAY_NODE";
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
        out += "ROOT_ID=" + std::to_string(node->GetId()); // SCREEN_NODE;ohos.sceneboard
    }

    Respond(out);
}

void RSProfiler::GetPerfTree(const ArgList& args)
{
    if (!context_) {
        return;
    }

    g_nodeListPerf.clear();
    g_mapNode2Count.clear();

    auto rootNode = GetRenderNode(Utils::PatchNodeId(0));
    if (!rootNode) {
        Respond("ERROR");
        return;
    }

    PerfTreeFlatten(rootNode, g_nodeListPerf, g_mapNode2Count, 0);

    std::string outString;
    auto& nodeMap = context_->GetMutableNodeMap();
    for (auto it = g_nodeListPerf.begin(); it != g_nodeListPerf.end(); it++) {
        auto node = nodeMap.GetRenderNode(it->first);
        if (!node) {
            continue;
        }
        std::string sNodeType;
        RSRenderNode::DumpNodeType(node->GetType(), sNodeType);
        outString += (it != g_nodeListPerf.begin() ? ", " : "") + std::to_string(it->first) + ":" +
                     std::to_string(g_mapNode2Count[it->first]) + " [" + sNodeType + "]";
    }

    std::unordered_set<uint64_t> perfNodesSet;
    for (const auto& item : g_nodeListPerf) {
        perfNodesSet.insert(item.first);
    }

    Network::SendRSTreePerfNodeList(perfNodesSet);
    Respond("OK: Count=" + std::to_string(g_nodeListPerf.size()) + " LIST=[" + outString + "]");
}

void RSProfiler::CalcPerfNodePrepareLo(const std::shared_ptr<RSRenderNode>& node, bool forceExcludeNode)
{
    if (!node || node->id_ == Utils::PatchNodeId(0)) {
        return;
    }

    const auto parent = node->GetParent().lock();
    auto parentChildren = parent ? parent->GetChildren() : nullptr;
    if (!parentChildren) {
        return;
    }

    std::vector<std::shared_ptr<RSRenderNode>> childList;
    for (const auto& child : *parentChildren) {
        if (child != nullptr) {
            childList.push_back(child);
        }
    }

    if (forceExcludeNode) {
        g_calcPerfSavedChildren.emplace_back(parent, childList);
        parent->RemoveChild(node);
        node->ResetParent();
    } else {
        g_calcPerfSavedChildren.clear();
    }
}

void RSProfiler::CalcPerfNodePrepare(NodeId nodeId, uint32_t timeCount, bool excludeDown)
{
    g_calcPerfNode = nodeId;
    g_effectiveNodeTimeCount = timeCount;
    g_calcPerfNodeExcludeDown = excludeDown;
    g_calcPerfSavedChildren.clear();

    auto node = GetRenderNode(g_calcPerfNode);
    CalcPerfNodePrepareLo(node, excludeDown);
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

    DumpParameter(nodeStr, usual, "HasCachedTexture", node->HasCachedTexture());
    DumpParameter(nodeStr, usual, "DrawInGroup", node->IsSuggestedDrawInGroup());
    DumpParameter(nodeStr, usual, "CacheSurfaceValid", node->IsCacheSurfaceValid());
    DumpParameter(nodeStr, usual, "CacheCompletedSurfaceValid", node->IsCacheCompletedSurfaceValid());
    if (constexpr int usualParamCnt = 6; usual >= usualParamCnt) {
        return;
    }
    Respond(nodeStr);
}

void RSProfiler::CalcPerfNode(const ArgList& args)
{
    NodeId nodeId = args.Uint64();
    uint32_t timeCount = args.Uint32(1);
    const bool excludeDown = args.Uint32(2) > 0;

    if (timeCount < CALC_PERF_NODE_TIME_COUNT_MIN) {
        timeCount = CALC_PERF_NODE_TIME_COUNT_MAX;
    }
    if (timeCount > CALC_PERF_NODE_TIME_COUNT_MAX) {
        timeCount = CALC_PERF_NODE_TIME_COUNT_MAX;
    }

    Respond("CalcPerfNode: calculating...");

    g_nodeListPerf.clear();
    g_nodeListPerf.emplace_back(nodeId, 0);
    g_mapNode2UpTime.clear();
    g_mapNode2UpDownTime.clear();
    g_nodeListPerfCalcIndex = 0;
    g_effectiveNodeTimeCount = timeCount;
    CalcPerfNodeAllStep();
    AwakeRenderServiceThread();
}

void RSProfiler::CalcPerfNodeAll(const ArgList& args)
{
    if (g_nodeListPerf.empty()) {
        Respond("ERROR");
        return;
    }

    g_mapNode2UpTime.clear();
    g_mapNode2UpDownTime.clear();
    g_nodeListPerfCalcIndex = 0;
    g_effectiveNodeTimeCount = CALC_PERF_NODE_TIME_COUNT_MIN;
    CalcPerfNodeAllStep();
    AwakeRenderServiceThread();
}

void RSProfiler::CalcPerfNodeAllStep()
{
    if (g_nodeListPerfCalcIndex < 0) {
        return;
    }

    const auto doublePerfListSize = static_cast<int32_t>(g_nodeListPerf.size()) * 2;
    if (g_nodeListPerfCalcIndex >= doublePerfListSize) {
        g_nodeListPerfCalcIndex = -1;

        for (auto it : g_nodeListPerf) {
            const auto node = GetRenderNode(it.first);
            if (!node) {
                continue;
            }
            const auto parent = node->GetParent().lock();
            if (!parent || !g_mapNode2UpTime.count(node->id_) || !g_mapNode2UpDownTime.count(node->id_)) {
                Respond("CALC_RESULT [" + std::to_string(node->id_) + "] error");
                Network::SendRSTreeSingleNodePerf(node->id_, 0);
                continue;
            }

            int64_t ns = (int64_t)g_mapNode2UpDownTime[node->id_] - (int64_t)g_mapNode2UpTime[node->id_];
            constexpr float nsToMs = 1000.0f;
            double ms = static_cast<double>(ns) / nsToMs;
            ms /= nsToMs;

            Respond("CALC_RESULT [" + std::to_string(node->id_) + "] time=" + std::to_string(ms));
            Network::SendRSTreeSingleNodePerf(node->id_, ms);
        }

        return;
    }

    const NodeId nodeId = g_nodeListPerf[g_nodeListPerfCalcIndex / 2].first;
    const bool excludeDown = g_nodeListPerfCalcIndex % 2;
    CalcPerfNodePrepare(nodeId, g_effectiveNodeTimeCount, excludeDown);
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

void RSProfiler::BuildTestTree(const ArgList& args)
{
    if (!context_ || !g_mainThread) {
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
        (*iter)->RemoveAllModifiers();
    }
    RSProfiler::testTree_.clear();
    SendMessage("Test tree cleared");
}

void RSProfiler::RecordStart(const ArgList& args)
{
    if (!IsNoneMode() || !g_childOfDisplayNodes.empty()) {
        SendMessage("Record: Start failed. Playback/Saving is in progress");
        return;
    }

    g_recordStartTime = 0.0;
    g_recordParcelNumber = 0;

    ImageCache::Reset();
    g_lastCacheImageCount = 0;

    if (!OpenBetaRecordFile(g_recordFile)) {
        g_recordFile.SetVersion(RSFILE_VERSION_LATEST);
        g_recordFile.Create(RSFile::GetDefaultPath());
    }

    g_recordMinVsync = 0;
    g_recordMaxVsync = 0;

    g_recordFile.AddLayer(); // add 0 layer

    FilterMockNode(*context_);
    RSTypefaceCache::Instance().ReplayClear();

    g_recordFile.AddHeaderFirstFrame(FirstFrameMarshalling(g_recordFile.GetVersion()));

    const std::vector<pid_t> pids = GetConnectionsPids();
    for (pid_t pid : pids) {
        g_recordFile.AddHeaderPid(pid);
    }

    g_recordStartTime = Now();
    g_frameNumber = 0;
    SetMode(Mode::WRITE);

    SendMessage("Record: Started");

    if (IsBetaRecordStarted()) {
        return;
    }

    std::thread thread([]() {
        while (IsRecording()) {
            SendTelemetry(Now() - g_recordStartTime);
            static constexpr int32_t GFX_METRICS_SEND_INTERVAL = 8;
            std::this_thread::sleep_for(std::chrono::milliseconds(GFX_METRICS_SEND_INTERVAL));
        }
    });
    thread.detach();

    SendMessage("Network: Record start"); // DO NOT TOUCH!
}

void RSProfiler::RecordStop(const ArgList& args)
{
    if (!IsRecording()) {
        SendMessage("Record: Stop failed. Record is not in progress");
        return;
    }

    SetMode(Mode::SAVING);
    if (args.String() == "REMOVELAST") {
        g_recordFile.UnwriteRSData(); // remove last commands they may start animation with password depiction
        g_recordFile.UnwriteRSData();
    }

    bool isBetaRecordingStarted = IsBetaRecordStarted();
    std::thread thread([isBetaRecordingStarted]() {
        g_recordFile.SetWriteTime(g_recordStartTime);

        if (isBetaRecordingStarted) {
            SaveBetaRecordFile(g_recordFile);
        } else {
            RecordSave();
        }

        g_recordFile.Close();
        g_recordStartTime = 0.0;
        g_lastCacheImageCount = 0;

        ImageCache::Reset();

        SendMessage("Record: Stopped");
        SendMessage("Network: record_vsync_range %" PRIu64 "%" PRIu64, g_recordMinVsync, g_recordMaxVsync); // DO NOT TOUCH!

        SetMode(Mode::NONE);
    });
    thread.detach();
}

void RSProfiler::PlaybackPrepareFirstFrame(const ArgList& args)
{
    if (!IsNoneMode()) {
        SendMessage("Playback: PrepareFirstFrame failed. Record/Saving is in progress");
        return;
    }

    if (g_playbackFile.IsOpen() || !g_childOfDisplayNodes.empty()) {
        Respond("FAILED: rsrecord_replay_prepare was already called");
        return;
    }
    g_playbackPid = args.Pid();
    g_playbackStartTime = 0.0;
    g_playbackPauseTime = args.Fp64(1);
    constexpr int pathArgPos = 2;
    std::string path = args.String(pathArgPos);
    if (!Utils::FileExists(path)) {
        Respond("Can't playback non existing file '" + path + "'");
        path = RSFile::GetDefaultPath();
    }

    RSTypefaceCache::Instance().ReplayClear();
    ImageCache::Reset();

    auto &animeMap = RSProfiler::AnimeGetStartTimes();
    animeMap.clear();

    Respond("Opening file " + path);
    g_playbackFile.Open(path);
    if (!g_playbackFile.IsOpen()) {
        Respond("Can't open file: not found");
        return;
    }

    if (args.String(0) == "VSYNC") {
        g_playbackFile.CacheVsyncId2Time(0);
        g_playbackPauseTime = g_playbackFile.ConvertVsyncId2Time(args.Int64(1));
    }

    AnimeGetStartTimesFromFile(animeMap);
    std::string dataFirstFrame = g_playbackFile.GetHeaderFirstFrame();

    // get first frame data
    std::string errReason = FirstFrameUnmarshalling(dataFirstFrame, g_playbackFile.GetVersion());
    if (errReason.size()) {
        Respond("Can't open file: " + errReason);
        FilterMockNode(*context_);
        g_playbackFile.Close();
        return;
    }
    // The number of frames loaded before command processing
    constexpr int defaultWaitFrames = 5;
    g_playbackWaitFrames = defaultWaitFrames;
    SendMessage("awake_frame %d", g_playbackWaitFrames); // DO NOT TOUCH!
    AwakeRenderServiceThread();
}

void RSProfiler::AnimeGetStartTimesFromFile(std::unordered_map<AnimationId, std::vector<int64_t>>& animeMap)
{
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
}

void RSProfiler::PlaybackStart(const ArgList& args)
{
    if (!IsNoneMode()) {
        SendMessage("Playback: Start failed. Record/Saving is in progress");
        return;
    }

    if (!g_playbackFile.IsOpen() || !g_childOfDisplayNodes.empty()) {
        Respond("FAILED: rsrecord_replay was already called");
        return;
    }

    HiddenSpaceTurnOn();

    for (size_t pid : g_playbackFile.GetHeaderPids()) {
        CreateMockConnection(Utils::GetMockPid(pid));
    }

    g_playbackStartTime = Now();

    const double pauseTime = g_playbackPauseTime;
    if (pauseTime > 0.0) {
        const uint64_t currentTime = RawNowNano();
        const uint64_t pauseTimeStart = currentTime + Utils::ToNanoseconds(pauseTime) / BaseGetPlaybackSpeed();
        TimePauseAt(currentTime, pauseTimeStart, g_playbackImmediate);
    }

    AwakeRenderServiceThread();

    g_playbackShouldBeTerminated = false;
    g_replayLastPauseTimeReported = 0;
    SetMode(Mode::READ);

    const auto timeoutLimit = args.Int64();
    std::thread thread([timeoutLimit]() {
        while (IsPlaying()) {
            const int64_t timestamp = static_cast<int64_t>(RawNowNano());

            PlaybackUpdate(PlaybackDeltaTime());

            const int64_t timeout = timeoutLimit - static_cast<int64_t>(RawNowNano()) + timestamp;
            if (timeout > 0) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(timeout));
            }
        }
        if (g_playbackFile.IsOpen()) {
            if (auto vsyncId = g_playbackFile.ConvertTime2VsyncId(PlaybackDeltaTime())) {
                SendMessage("Replay timer paused vsyncId=%" PRId64 "", vsyncId); // DO NOT TOUCH!
            }
            g_playbackFile.Close();
        }
        g_playbackStartTime = 0.0;
        g_playbackPid = 0;
        TimePauseClear();
        g_playbackShouldBeTerminated = false;
        Respond("Playback thread terminated");
    });
    thread.detach();

    Respond("Playback start");
}

void RSProfiler::PlaybackStop(const ArgList& args)
{
    if (!g_playbackFile.IsOpen() && g_childOfDisplayNodes.empty()) {
        Respond("FAILED: Playback stop - no rsrecord_replay_* was called previously");
        return;
    }
    SetMode(Mode::NONE);
    if (g_childOfDisplayNodes.empty()) {
        // rsrecord_replay_prepare was called but rsrecord_replay_start was not
        g_playbackFile.Close();
        g_childOfDisplayNodes.clear();
    } else {
        g_playbackShouldBeTerminated = true;
        HiddenSpaceTurnOff();
    }
    FilterMockNode(*context_);
    constexpr int maxCountForSecurity = 1000;
    for (int i = 0; !RSRenderNodeGC::Instance().IsBucketQueueEmpty() && i < maxCountForSecurity; i++) {
        RSRenderNodeGC::Instance().ReleaseNodeBucket();
    }
    RSTypefaceCache::Instance().ReplayClear();
    ImageCache::Reset();
    g_replayLastPauseTimeReported = 0;

    SendMessage("Playback stop"); // DO NOT TOUCH!
}

double RSProfiler::PlaybackDeltaTime()
{
    return Now() - g_playbackStartTime;
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

    if (g_playbackFile.RSDataEOF()) {
        g_playbackShouldBeTerminated = true;
    }
    return readTime;
}

void RSProfiler::PlaybackPrepare(const ArgList& args)
{
    const pid_t pid = args.Pid();
    if (!context_ || (pid == 0)) {
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
    TimePauseAt(g_frameBeginTimestamp, currentTime, g_playbackImmediate);
    Respond("OK: " + std::to_string(recordPlayTime));

    int64_t vsyncId = g_playbackFile.ConvertTime2VsyncId(recordPlayTime);
    if (vsyncId) {
        SendMessage("Replay timer paused vsyncId=%" PRId64, vsyncId); // DO NOT TOUCH!
    }
    g_replayLastPauseTimeReported = recordPlayTime;
}

void RSProfiler::PlaybackPauseAt(const ArgList& args)
{
    if (!IsPlaying()) {
        Respond("PlaybackPauseAt REJECT is_playing=false");
        return;
    }

    double pauseAtTimeSec;
    if (args.String(0) == "VSYNC") {
        int64_t vsyncId = args.Int64(1);
        pauseAtTimeSec = g_playbackFile.ConvertVsyncId2Time(vsyncId);
    } else {
        pauseAtTimeSec = args.Fp64();
    }

    const uint64_t currentTimeNano = RawNowNano();
    const double alreadyPlayedTimeSec = Utils::ToSeconds(PatchTime(currentTimeNano)) - g_playbackStartTime;
    if (alreadyPlayedTimeSec > pauseAtTimeSec) {
        return;
    }

    // set 2nd pause
    const uint64_t pauseAfterTimeNano = currentTimeNano + Utils::ToNanoseconds(pauseAtTimeSec - alreadyPlayedTimeSec)
         / BaseGetPlaybackSpeed();

    TimePauseAt(currentTimeNano, pauseAfterTimeNano, g_playbackImmediate);
    ResetAnimationStamp();
    Respond("PlaybackPauseAt OK");
}


void RSProfiler::ProcessCommands()
{
    if (g_playbackWaitFrames > 0) {
        g_playbackWaitFrames--;
        SendMessage("awake_frame %d", g_playbackWaitFrames); // DO NOT TOUCH!
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

std::vector<std::pair<uint64_t, int64_t>> RSProfiler::AnimeGetStartTimesFlattened(double recordStartTime)
{
    std::vector<std::pair<uint64_t, int64_t>> headerAnimeStartTimes;
    const std::unordered_map<AnimationId, std::vector<int64_t>> &headerAnimeStartTimesMap = AnimeGetStartTimes();
    for (const auto& item : headerAnimeStartTimesMap) {
        for (const auto time : item.second) {
            headerAnimeStartTimes.push_back({
                Utils::PatchNodeId(item.first),
                time - Utils::ToNanoseconds(recordStartTime)
            });
        }
    }
    return headerAnimeStartTimes;
}

void RSProfiler::TestSaveSubTree(const ArgList& args)
{
    const auto nodeId = args.Node();
    auto node = GetRenderNode(nodeId);
    if (!node) {
        Respond("Error: node not found");
        return;
    }

    std::stringstream stream;

    // Save RSFILE_VERSION
    uint32_t fileVersion = RSFILE_VERSION_LATEST;
    stream.write(reinterpret_cast<const char*>(&fileVersion), sizeof(fileVersion));

    MarshalSubTree(*context_, stream, *node, fileVersion);
    std::string testDataSubTree = stream.str();

    Respond("Save SubTree Size: " + std::to_string(testDataSubTree.size()));

    // save file need setenforce 0
    std::string rootPath = "/data";
    char realRootPath[PATH_MAX] = {0};
    if (!realpath(rootPath.c_str(), realRootPath)) {
        Respond("Error: data path is invalid");
        return;
    }
    std::string filePath = realRootPath;
    filePath = filePath + "/rssbtree_test_" + std::to_string(nodeId);
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << testDataSubTree;
        file.close();
        Respond("Save subTree Success, file path:" + filePath);
    } else {
        Respond("Save subTree Failed: save file faild!");
    }
}

void RSProfiler::TestLoadSubTree(const ArgList& args)
{
    const auto nodeId = args.Node();
    auto node = GetRenderNode(nodeId);
    if (!node) {
        Respond("Error: node not found");
        return;
    }

    const auto filePath = args.String(1);
    if (filePath.empty()) {
        Respond("Error: Path is empty");
        return;
    }

    char realPath[PATH_MAX] = {0};
    if (!realpath(filePath.c_str(), realPath)) {
        Respond("Error: Path is invalid");
        return;
    }

    std::ifstream file(realPath);
    if (!file.is_open()) {
        std::error_code ec(errno, std::system_category());
        RS_LOGE("RSProfiler::TestLoadSubTree read file failed: %{public}s", ec.message().c_str());
        Respond("RSProfiler::TestLoadSubTree read file failed: " + ec.message());
        return;
    }
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();

    // Load RSFILE_VERSION
    uint32_t fileVersion = 0u;
    stream.read(reinterpret_cast<char*>(&fileVersion), sizeof(fileVersion));

    std::string errorReason = UnmarshalSubTree(*context_, stream, *node, fileVersion);
    if (errorReason.size()) {
        RS_LOGE("RSProfiler::TestLoadSubTree failed: %{public}s", errorReason.c_str());
        Respond("RSProfiler::TestLoadSubTree failed: " + errorReason);
    }
    Respond("Load subTree result: " + errorReason);
}

void RSProfiler::TestClearSubTree(const ArgList& args)
{
    FilterMockNode(*context_);
    constexpr int maxCountForSecurity = 1000;
    for (int i = 0; !RSRenderNodeGC::Instance().IsBucketQueueEmpty() && i < maxCountForSecurity; i++) {
        RSRenderNodeGC::Instance().ReleaseNodeBucket();
    }
    RSTypefaceCache::Instance().ReplayClear();
    ImageCache::Reset();
    Respond("Clear SubTree Success");
}

void RSProfiler::MarshalSubTree(RSContext& context, std::stringstream& data,
    const RSRenderNode& node, uint32_t fileVersion, bool clearImageCache)
{
    if (clearImageCache) {
        ImageCache::Reset();
    }

    SetMode(Mode::WRITE_EMUL);
    DisableSharedMemory();

    std::stringstream dataNodes(std::ios::in | std::ios::out | std::ios::binary);
    MarshalSubTreeLo(context, dataNodes, node, fileVersion);

    EnableSharedMemory();
    SetMode(Mode::NONE);

    std::stringstream dataPixelMaps(std::ios::in | std::ios::out | std::ios::binary);
    ImageCache::Serialize(dataPixelMaps);

    // SAVE TO STREAM
    TypefaceMarshalling(data, fileVersion);

    uint32_t pixelMapSize = dataPixelMaps.str().size();
    data.write(reinterpret_cast<const char*>(&pixelMapSize), sizeof(pixelMapSize));
    data.write(dataPixelMaps.str().data(), dataPixelMaps.str().size());

    uint32_t nodesSize = dataNodes.str().size();
    data.write(reinterpret_cast<const char*>(&nodesSize), sizeof(nodesSize));
    data.write(dataNodes.str().data(), dataNodes.str().size());
}

std::string RSProfiler::UnmarshalSubTree(RSContext& context, std::stringstream& data,
    RSRenderNode& attachNode, uint32_t fileVersion, bool clearImageCache)
{
    if (clearImageCache) {
        ImageCache::Reset();
    }

    TypefaceUnmarshalling(data, fileVersion);

    uint32_t pixelMapSize = 0u;
    data.read(reinterpret_cast<char*>(&pixelMapSize), sizeof(pixelMapSize));

    // read Cache
    ImageCache::Deserialize(data);

    uint32_t nodesSize = 0u;
    data.read(reinterpret_cast<char*>(&nodesSize), sizeof(nodesSize));

    SetMode(Mode::READ_EMUL);
    DisableSharedMemory();

    std::string errReason = UnmarshalSubTreeLo(context, data, attachNode, fileVersion);

    EnableSharedMemory();
    SetMode(Mode::NONE);

    auto& nodeMap = context.GetMutableNodeMap();
    nodeMap.TraversalNodes([](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr) {
            return;
        }
        if (Utils::IsNodeIdPatched(node->GetId())) {
            node->SetContentDirty();
            node->SetDirty();
        }
    });
    g_mainThread->SetDirtyFlag();
    AwakeRenderServiceThread();
    return errReason;
}
} // namespace OHOS::Rosen
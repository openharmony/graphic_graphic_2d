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

#include <filesystem>

#include "foundation/graphic/graphic_2d/utils/log/rs_trace.h"
#include "rs_profiler_base.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_file.h"
#include "rs_profiler_network.h"
#include "rs_profiler_telemetry.h"
#include "rs_profiler_utils.h"
#include "rs_profiler_capture_recorder.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_uni_render_util.h"

namespace OHOS::Rosen {

constexpr int NANOSECONDS_IN_SECONDS = 1'000'000'000;

// (user): Move to RSProfiler
static RSRenderService* g_renderService = nullptr;
static RSMainThread* g_renderServiceThread = nullptr;
static RSContext* g_renderServiceContext = nullptr;
static uint64_t g_frameBeginTimestamp = 0u;
static double g_dirtyRegionPercentage = 0.0;
static bool g_rdcSent = true;

static RSFile g_recordFile {};
static double g_recordStartTime = 0.0;

static RSFile g_playbackFile {};
static double g_playbackStartTime = 0.0;
static NodeId g_playbackParentNodeId = 0;
static int g_playbackPid = 0;
static bool g_playbackShouldBeTerminated = false;

#pragma pack(push, 1)
struct AlignedMessageParcel {
    uint8_t pad = 0u;
    MessageParcel parcel;
};
#pragma pack(pop)

namespace {
pid_t GetPid(const std::shared_ptr<RSRenderNode>& node)
{
    return node ? Utils::ExtractPid(node->GetId()) : 0;
}

NodeId GetNodeId(const std::shared_ptr<RSRenderNode>& node)
{
    return node ? Utils::ExtractNodeId(node->GetId()) : 0;
}

} // namespace

/*
    To visualize the damage region (as it's set for KHR_partial_update), you can set the following variable:
    'hdc shell param set rosen.dirtyregiondebug.enabled 6'
*/
static double GetDirtyRegionRelative(RSContext& context)
{
    const std::shared_ptr<RSBaseRenderNode>& rootNode = context.GetGlobalRootRenderNode();
    // without these checks device might get stuck on startup
    if (!rootNode || (rootNode->GetChildrenCount() != 1)) {
        return -1.0;
    }

    std::shared_ptr<RSDisplayRenderNode> displayNode =
        RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(rootNode->GetSortedChildren().front());
    if (!displayNode) {
        return -1.0;
    }

    std::shared_ptr<RSBaseRenderNode> nodePtr = displayNode->shared_from_this();
    std::shared_ptr<RSDisplayRenderNode> displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        return -1.0;
    }

    const RectI displayResolution = displayNodePtr->GetDirtyManager()->GetSurfaceRect();
    const double displayWidth = displayResolution.GetWidth();
    const double displayHeight = displayResolution.GetHeight();
    const uint32_t bufferAge = 3;
    // not to update the RSRenderFrame/DirtyManager and just calculate dirty region
    const bool isAlignedDirtyRegion = false;
    RSUniRenderUtil::MergeDirtyHistory(displayNodePtr, bufferAge, isAlignedDirtyRegion);
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;
    const Occlusion::Region dirtyRegion =
        RSUniRenderUtil::MergeVisibleDirtyRegion(displayNodePtr, hasVisibleDirtyRegionSurfaceVec, isAlignedDirtyRegion);

    const std::vector<Occlusion::Rect>& visibleDirtyRects = dirtyRegion.GetRegionRects();
    double accumulatedArea = 0.0;
    for (const Occlusion::Rect& rect : visibleDirtyRects) {
        const int width = rect.right_ - rect.left_;
        const int height = rect.bottom_ - rect.top_;
        accumulatedArea += width * height;
    }

    const double dirtyRegionPercentage = accumulatedArea / (displayWidth * displayHeight);
    return dirtyRegionPercentage;
}

void RSProfiler::Init(RSRenderService* renderService)
{
    g_renderService = renderService;
    g_renderServiceThread = renderService ? RSMainThread::Instance() : nullptr;
    g_renderServiceContext = g_renderServiceThread ? &g_renderServiceThread->GetContext() : nullptr;

    static std::thread const THREAD(Network::Run);
}

void RSProfiler::RenderServiceOnCreateConnection(pid_t pid)
{
    if (IsRecording()) {
        if (g_recordStartTime == 0.0) {
            g_recordStartTime = Utils::Now();
        }

        g_recordFile.AddHeaderPID(pid);
    }
}

void RSProfiler::RenderServiceConnectionOnRemoteRequest(RSRenderServiceConnection* connection, uint32_t code,
    MessageParcel& parcel, MessageParcel& /*reply*/, MessageOption& option)
{
    if (IsRecording() && (g_recordStartTime > 0.0)) {
        const pid_t pid = g_renderService->GetConnectionPID(connection);
        const auto& pids = g_recordFile.GetHeaderPIDList();
        if (std::find(std::begin(pids), std::end(pids), pid) != std::end(pids)) {
            const double deltaTime = Utils::Now() - g_recordStartTime;

            std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);

            char headerType = 1; // parcel data
            stream.write(reinterpret_cast<const char*>(&headerType), sizeof(headerType));
            stream.write(reinterpret_cast<const char*>(&deltaTime), sizeof(deltaTime));

            // set sending pid
            stream.write(reinterpret_cast<const char*>(&pid), sizeof(pid));
            stream.write(reinterpret_cast<const char*>(&code), sizeof(code));

            const int32_t dataSize = parcel.GetDataSize();
            stream.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
            stream.write(reinterpret_cast<const char*>(parcel.GetData()), parcel.GetDataSize());

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

    if (IsPlaying()) {
        RSProfilerBase::SpecParseModeSet(SpecParseMode::READ);
        RSProfilerBase::SpecParseReplacePIDSet(
            g_playbackFile.GetHeaderPIDList(), g_playbackPid, g_playbackParentNodeId);
    } else if (IsRecording()) {
        RSProfilerBase::SpecParseModeSet(SpecParseMode::WRITE);
    } else {
        RSProfilerBase::SpecParseModeSet(SpecParseMode::NONE);
    }
}

void RSProfiler::UnmarshalThreadOnRecvParcel(const MessageParcel* parcel, RSTransactionData* data)
{
    if (parcel && RSProfilerBase::IsParcelMock(*parcel)) {
        data->SetSendingPid(Utils::GetMockPid(data->GetSendingPid()));
    }
}

uint64_t RSProfiler::TimePauseApply(uint64_t time)
{
    return RSProfilerBase::TimePauseApply(time);
}

void RSProfiler::MainThreadOnProcessCommand()
{
    if (IsPlaying()) {
        g_renderServiceThread->ResetAnimationStamp();
    }
}

void RSProfiler::MainThreadOnRenderBegin()
{
    if (IsRecording()) {
        constexpr double ratioToPercent = 100.0;
        g_dirtyRegionPercentage = GetDirtyRegionRelative(*g_renderServiceContext) * ratioToPercent;
    }
}

void RSProfiler::MainThreadOnRenderEnd() {}

void RSProfiler::MainThreadOnFrameBegin()
{
    g_frameBeginTimestamp = Utils::RawNowNano();
}

void RSProfiler::MainThreadOnFrameEnd()
{
    ProcessCommands();
    ProcessSendingRdc();
    RecordUpdate();
}

bool RSProfiler::IsEnabled()
{
    return g_renderService && g_renderServiceThread && g_renderServiceContext;
}

bool RSProfiler::IsRecording()
{
    return IsEnabled() && g_recordFile.IsOpen();
}

bool RSProfiler::IsPlaying()
{
    return IsEnabled() && g_playbackFile.IsOpen();
}

void RSProfiler::AwakeRenderServiceThread()
{
    if (g_renderServiceThread) {
        g_renderServiceThread->PostTask([]() {
            g_renderServiceThread->SetAccessibilityConfigChanged();
            g_renderServiceThread->RequestNextVSync();
        });
    }
}

std::shared_ptr<RSRenderNode> RSProfiler::GetRenderNode(uint64_t id)
{
    return g_renderServiceContext ? g_renderServiceContext->GetMutableNodeMap().GetRenderNode(id) : nullptr;
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
    RSSystemProperties::SetInstantRecording(true);
    AwakeRenderServiceThread();
    Respond("Recording current frame cmds into : /data/default.skp");
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

void RSProfiler::RecordUpdate()
{
    if (!IsRecording() || (g_recordStartTime <= 0.0)) {
        return;
    }

    const uint64_t frameLengthNanosecs = Utils::RawNowNano() - g_frameBeginTimestamp;

    const double currentTime = g_frameBeginTimestamp * 1e-9; // Utils::Now();
    const double timeSinceRecordStart = currentTime - g_recordStartTime;

    if (timeSinceRecordStart > 0.0) {
        RSCaptureData captureData;
        captureData.SetTime(timeSinceRecordStart);
        captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_LEN, frameLengthNanosecs);
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_COUNT, RSProfilerBase::ParsedCmdCountGet());
        captureData.SetProperty(RSCaptureData::KEY_RS_PIXEL_IMAGE_ADDED, RSProfilerBase::ImagesAddedCountGet());
        captureData.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION, floor(g_dirtyRegionPercentage));

        std::vector<char> out;
        captureData.Serialize(out);

        const char headerType = 2; // TYPE: RS METRICS
        out.insert(out.begin(), headerType);

        Network::SendBinary(out.data(), out.size());
        g_recordFile.WriteRSMetrics(0, timeSinceRecordStart, out.data(), out.size());
    }
}

void RSProfiler::Respond(const std::string& message)
{
    Network::SendMessage(message);
}

void RSProfiler::DumpConnections(const ArgList& args)
{
    if (g_renderService) {
        std::string out;
        g_renderService->DumpConnections(out);
        Respond(out);
    }
}

void RSProfiler::DumpNodeModifiers(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        std::string out;
        node->DumpModifiers(out);
        Respond("Modifiers=" + out);
    }
}

void RSProfiler::DumpNodeProperties(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        std::string out;
        node->DumpRenderProperties(out);
        Respond("RenderProperties=" + out);
    }
}

void RSProfiler::DumpTree(const ArgList& args)
{
    if (!g_renderServiceContext) {
        return;
    }

    const RSRenderNodeMap& nodeMap = g_renderServiceContext->GetMutableNodeMap();

    std::map<std::string, std::tuple<NodeId, std::string>> list;
    nodeMap.GetSurfacesTrees(list);

    std::string out = "Tree: count=" + std::to_string(static_cast<int>(nodeMap.GetRenderNodeCount())) +
                      " time=" + std::to_string(Utils::Now()) + "\n";

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

void RSProfiler::DumpSurfaces(const ArgList& args)
{
    if (!g_renderServiceContext) {
        return;
    }

    const RSRenderNodeMap& nodeMap = g_renderServiceContext->GetMutableNodeMap();

    std::map<NodeId, std::string> surfaces;
    nodeMap.GetSurfacesTreesByPid(args.Pid(), surfaces);

    std::string out;
    for (const auto& item : surfaces) {
        out += "*** " + std::to_string(item.first) + " pid=" + std::to_string(ExtractPid(item.first)) +
               " lowId=" + std::to_string(Utils::ExtractNodeId(item.first)) + "\n" + item.second + "\n";
    }

    out += "TREE: count=" + std::to_string(static_cast<int32_t>(nodeMap.GetRenderNodeCount())) +
           " time=" + std::to_string(Utils::Now()) + "\n";

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

    const Vector4f screenRect = RSProfilerBase::GetScreenRect();

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
    const uint32_t pid = args.Pid();
    if (const auto node = GetRenderNode(Utils::GetRootNodeId(pid))) {
        const auto parent = node->GetParent().lock();
        const std::string out =
            "parentPid=" + std::to_string(GetPid(parent)) + " parentNode=" + std::to_string(GetNodeId(parent));

        g_renderServiceContext->GetMutableNodeMap().FilterNodeByPid(pid);
        AwakeRenderServiceThread();
        Respond(out);
    }
}

void RSProfiler::GetRoot(const ArgList& args)
{
    if (!g_renderServiceContext) {
        return;
    }

    std::string out;

    const RSRenderNodeMap& nodeMap = g_renderServiceContext->GetMutableNodeMap();
    std::shared_ptr<RSRenderNode> node = nodeMap.GetRenderNode<RSRenderNode>(nodeMap.GetRandomSurfaceNode());
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

void RSProfiler::RecordStart(const ArgList& args)
{
    auto& imageMap = RSProfilerBase::GetImageCache();
    imageMap.clear();

    g_recordFile.Create(RSFile::GetDefaultPath());
    g_recordFile.SetImageMapPtr(reinterpret_cast<std::map<uint64_t, ReplayImageCacheRecordFile>*>(&imageMap));
    g_recordFile.AddLayer(); // add 0 layer

    auto& nodeMap = g_renderServiceContext->GetMutableNodeMap();
    nodeMap.FilterNodeReplayed();

    g_recordStartTime = 0.0;

    std::thread thread([]() {
        while (IsRecording()) {
            Network::SendTelemetry(g_recordStartTime);
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

    // DOUBLE WORK - send header of file
    const char headerType = 0;
    stream.write(reinterpret_cast<const char*>(&headerType), sizeof(headerType));
    stream.write(reinterpret_cast<const char*>(&g_recordStartTime), sizeof(g_recordStartTime));

    const int32_t pidCount = g_recordFile.GetHeaderPIDList().size();
    stream.write(reinterpret_cast<const char*>(&pidCount), sizeof(pidCount));
    for (auto item : g_recordFile.GetHeaderPIDList()) {
        stream.write(reinterpret_cast<const char*>(&item), sizeof(item));
    }

    auto& imageMap = RSProfilerBase::GetImageCache();

    const uint32_t imageMapCount = imageMap.size();
    stream.write(reinterpret_cast<const char*>(&imageMapCount), sizeof(imageMapCount));
    for (auto item : imageMap) {
        stream.write(reinterpret_cast<const char*>(&item.first), sizeof(item.first));
        stream.write(reinterpret_cast<const char*>(&item.second.skipBytes), sizeof(item.second.skipBytes));
        stream.write(reinterpret_cast<const char*>(&item.second.imageSize), sizeof(item.second.imageSize));
        stream.write(reinterpret_cast<const char*>(item.second.image.get()), item.second.imageSize);
    }

    Network::SendBinary(stream.str().data(), stream.str().size());

    g_recordFile.Close();
    g_recordStartTime = 0.0;

    Respond("Network: Record stop");
}

void RSProfiler::PlaybackStart(const ArgList& args)
{
    auto imageCache = &RSProfilerBase::GetImageCache();
    g_playbackFile.SetImageMapPtr(reinterpret_cast<std::map<uint64_t, ReplayImageCacheRecordFile>*>(imageCache));
    g_playbackFile.Open(RSFile::GetDefaultPath());
    if (!g_playbackFile.IsOpen()) {
        return;
    }

    g_playbackPid = args.Pid();

    auto& nodeMap = g_renderServiceContext->GetMutableNodeMap();
    nodeMap.FilterForReplay(g_playbackPid);
    nodeMap.FilterNodeReplayed();

    const NodeId processRootNodeId = Utils::GetRootNodeId(g_playbackPid);
    const auto processRootNode = nodeMap.GetRenderNode(processRootNodeId);
    auto parent = processRootNode ? processRootNode->GetParent().lock() : nullptr;
    if (parent) {
        parent->RemoveChild(processRootNode);
    }

    g_playbackParentNodeId = parent ? parent->GetId() : 0;

    for (size_t pid : g_playbackFile.GetHeaderPIDList()) {
        g_renderService->CreateMockConnection(Utils::GetMockPid(pid));
    }

    g_playbackStartTime = Utils::Now();

    const double pauseTime = args.Fp64(1);
    if (pauseTime > 0.0) {
        const uint64_t currentTime = Utils::RawNowNano();
        const uint64_t pauseTimeStart = currentTime + pauseTime * NANOSECONDS_IN_SECONDS;
        RSProfilerBase::TimePauseAt(currentTime, pauseTimeStart);
    }

    AwakeRenderServiceThread();

    g_playbackShouldBeTerminated = false;

    std::thread thread([]() {
        while (IsPlaying()) {
            const int64_t timestamp = Utils::RawNowNano();

            PlaybackUpdate();
            Network::SendTelemetry(g_playbackStartTime);

            constexpr int64_t timeoutLimit = 8000000;
            const int64_t timeout = timeoutLimit - Utils::RawNowNano() + timestamp;
            if (timeout > 0) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(timeout));
            }

            AwakeRenderServiceThread();
        }
    });
    thread.detach();

    Respond("Playback start");
}

void RSProfiler::PlaybackStop(const ArgList& args)
{
    if (g_playbackPid > 0) {
        auto& nodeMap = g_renderServiceContext->GetMutableNodeMap();
        nodeMap.FilterForReplay(g_playbackPid);
        nodeMap.FilterNodeReplayed();
    }
    g_playbackShouldBeTerminated = true;

    Respond("Playback stop");
}

void RSProfiler::PlaybackUpdate()
{
    if (!IsPlaying()) {
        return;
    }

    const double deltaTime = Utils::Now() - g_playbackStartTime;

    std::vector<uint8_t> data;
    double readTime = 0.0;
    while (!g_playbackShouldBeTerminated && g_playbackFile.ReadRSData(deltaTime, data, readTime)) {
        std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
        stream.write(reinterpret_cast<const char*>(data.data()), data.size());
        stream.seekg(0);

        pid_t pid = 0;
        stream.read(reinterpret_cast<char*>(&pid), sizeof(pid));

        RSRenderServiceConnection* connection = g_renderService->GetConnectionByPID(Utils::GetMockPid(pid));
        if (!connection) {
            const std::vector<pid_t>& pids = g_playbackFile.GetHeaderPIDList();
            if (!pids.empty()) {
                connection = g_renderService->GetConnectionByPID(Utils::GetMockPid(pids[0]));
            }
        }
        if (!connection) {
            continue;
        }

        uint32_t code = 0;
        stream.read(reinterpret_cast<char*>(&code), sizeof(code));

        int32_t dataSize = 0;
        stream.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

        auto* data = new uint8_t[dataSize];
        stream.read(reinterpret_cast<char*>(data), dataSize);

        int32_t flags = 0;
        stream.read(reinterpret_cast<char*>(&flags), sizeof(flags));

        int32_t waitTime = 0;
        stream.read(reinterpret_cast<char*>(&waitTime), sizeof(waitTime));

        AlignedMessageParcel parcel;
        parcel.parcel.WriteBuffer(data, dataSize);

        MessageOption option;
        option.SetFlags(flags);
        option.SetWaitTime(waitTime);

        MessageParcel reply;
        connection->OnRemoteRequest(code, parcel.parcel, reply, option);
    }

    if (g_playbackShouldBeTerminated || g_playbackFile.RSDataEOF()) {
        g_playbackStartTime = 0.0;
        g_playbackFile.Close();
        g_playbackPid = 0;
        RSProfilerBase::TimePauseClear();
    }
}

void RSProfiler::PlaybackPrepare(const ArgList& args)
{
    const uint32_t pid = args.Pid();
    if (!g_renderServiceContext || (pid == 0)) {
        return;
    }

    g_renderServiceContext->GetMutableNodeMap().FilterForReplay(pid);
    AwakeRenderServiceThread();
    Respond("OK");
}

void RSProfiler::PlaybackPause(const ArgList& args)
{
    if (!g_playbackFile.IsOpen()) {
        return;
    }
    if (g_playbackStartTime <= 0.0) {
        return;
    }

    const uint64_t curTimeRaw = Utils::RawNowNano();
    const double recordPlayTime = Utils::Now() - g_playbackStartTime;

    RSProfilerBase::TimePauseAt(curTimeRaw, curTimeRaw);
    Respond("OK: " + std::to_string(recordPlayTime));
}

void RSProfiler::PlaybackPauseAt(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    const double pauseTime = args.Fp64();
    const double recordPlayTime = Utils::Now() - g_playbackStartTime;
    if (recordPlayTime > pauseTime) {
        return;
    }

    const uint64_t curTimeRaw = Utils::RawNowNano();
    const uint64_t pauseTimeStart = curTimeRaw + (pauseTime - recordPlayTime) * NANOSECONDS_IN_SECONDS;

    RSProfilerBase::TimePauseAt(curTimeRaw, pauseTimeStart);

    g_renderServiceThread->ResetAnimationStamp();

    Respond("OK");
}

void RSProfiler::PlaybackPauseClear(const ArgList& args)
{
    RSProfilerBase::TimePauseClear();
    Respond("OK");
}

void RSProfiler::PlaybackResume(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    const uint64_t curTimeRaw =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();

    RSProfilerBase::TimePauseResume(curTimeRaw);

    g_renderServiceThread->ResetAnimationStamp();

    Respond("OK");
}

RSProfiler::Command RSProfiler::GetCommand(const std::string& command)
{
    static const std::map<std::string, Command> COMMANDS = {
        { "rstree_contains", DumpTree },
        { "rstree_fix", PatchNode },
        { "rstree_kill_node", KillNode },
        { "rstree_setparent", AttachChild },
        { "rstree_getroot", GetRoot },
        { "rstree_node_mod", DumpNodeModifiers },
        { "rstree_node_prop", DumpNodeProperties },
        { "rstree_pid", DumpSurfaces },
        { "rstree_kill_pid", KillPid },
        { "rstree_prepare_replay", PlaybackPrepare },
        { "rsrecord_start", RecordStart },
        { "rsrecord_stop", RecordStop },
        { "rsrecord_replay", PlaybackStart },
        { "rsrecord_replay_stop", PlaybackStop },
        { "rsrecord_pause_now", PlaybackPause },
        { "rsrecord_pause_at", PlaybackPauseAt },
        { "rsrecord_pause_resume", PlaybackResume },
        { "rsrecord_pause_clear", PlaybackPauseClear },
        { "rssurface_pid", DumpNodeSurface },
        { "rscon_print", DumpConnections },
        { "save_rdc", SaveRdc },
        { "save_skp", SaveSkp },
        { "info", GetDeviceInfo },
    };

    if (command.empty()) {
        return nullptr;
    }

    const auto delegate = COMMANDS.find(command);
    return (delegate != COMMANDS.end()) ? delegate->second : nullptr;
}

void RSProfiler::ProcessCommands()
{
    const std::lock_guard<std::mutex> guard(Network::incomingMutex_);
    auto& commandData = Network::incoming_;
    if (commandData.empty()) {
        return;
    }

    const std::string command = commandData[0];
    const ArgList args = (commandData.size() > 1) ? ArgList({ commandData.begin() + 1, commandData.end() }) : ArgList();
    commandData.clear();

    if (const Command delegate = GetCommand(command)) {
        delegate(args);
    } else if (!command.empty()) {
        Respond("Command has not been found: " + command);
    }
}

} // namespace OHOS::Rosen
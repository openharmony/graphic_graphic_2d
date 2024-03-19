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
#include "rs_profiler_capture_recorder.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_file.h"
#include "rs_profiler_network.h"
#include "rs_profiler_telemetry.h"
#include "rs_profiler_utils.h"

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

static std::string g_testDataFrame;
static std::list<RSRenderNode::SharedPtr> g_childOfDisplayNodes;

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
double RSProfiler::GetDirtyRegionRelative(RSContext& context)
{
    std::shared_ptr<RSDisplayRenderNode> displayNode = GetDisplayNode(context);
    if (!displayNode) {
        return -1;
    }

    const RectI displayResolution = displayNode->GetDirtyManager()->GetSurfaceRect();
    const double displayWidth = displayResolution.GetWidth();
    const double displayHeight = displayResolution.GetHeight();
    const uint32_t bufferAge = 3;
    // not to update the RSRenderFrame/DirtyManager and just calculate dirty region
    const bool isAlignedDirtyRegion = false;
    RSUniRenderUtil::MergeDirtyHistory(displayNode, bufferAge, isAlignedDirtyRegion);
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;
    const Occlusion::Region dirtyRegion =
        RSUniRenderUtil::MergeVisibleDirtyRegion(displayNode, hasVisibleDirtyRegionSurfaceVec, isAlignedDirtyRegion);

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
    if (!IsEnabled()) {
        return;
    }

    g_renderService = renderService;
    g_renderServiceThread = g_renderService ? g_renderService->mainThread_ : nullptr;
    g_renderServiceContext = g_renderServiceThread ? g_renderServiceThread->context_.get() : nullptr;

    static std::thread const THREAD(Network::Run);
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

void RSProfiler::OnRemoteRequest(RSRenderServiceConnection* connection, uint32_t code, MessageParcel& parcel,
    MessageParcel& /*reply*/, MessageOption& option)
{
    if (!IsEnabled()) {
        return;
    }

    if (IsRecording() && (g_recordStartTime > 0.0)) {
        const pid_t pid = GetConnectionPid(connection);
        const auto& pids = g_recordFile.GetHeaderPids();
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

    if (IsLoadSaveFirstScreenInProgress()) {
        // saving screen right now
    }
    if (IsPlaying()) {
        SetReplayTimes(g_playbackStartTime, g_playbackFile.GetWriteTime());
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

    sptr<RSIRenderServiceConnection> newConn(new RSRenderServiceConnection(pid, g_renderService, g_renderServiceThread,
        g_renderService->screenManager_, tokenObj, g_renderService->appVSyncDistributor_));

    sptr<RSIRenderServiceConnection> tmp;

    std::unique_lock<std::mutex> lock(g_renderService->mutex_);
    // if connections_ has the same token one, replace it.
    if (g_renderService->connections_.count(tokenObj) > 0) {
        tmp = g_renderService->connections_.at(tokenObj);
    }
    g_renderService->connections_[tokenObj] = newConn;
    lock.unlock();
    g_renderServiceThread->AddTransactionDataPidInfo(pid);
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

pid_t RSProfiler::GetConnectionPid(RSRenderServiceConnection* connection)
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

void RSProfiler::OnProcessCommand()
{
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
    if (IsRecording()) {
        constexpr double ratioToPercent = 100.0;
        g_dirtyRegionPercentage = GetDirtyRegionRelative(*g_renderServiceContext) * ratioToPercent;
    }
}

void RSProfiler::OnRenderEnd() {}

void RSProfiler::OnFrameBegin()
{
    if (!IsEnabled()) {
        return;
    }

    g_frameBeginTimestamp = Utils::RawNowNano();
}

void RSProfiler::OnFrameEnd()
{
    if (!IsEnabled()) {
        return;
    }

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

void RSProfiler::ResetAnimationStamp()
{
    if (g_renderServiceThread && g_renderServiceContext) {
        g_renderServiceThread->lastAnimateTimestamp_ = g_renderServiceContext->GetCurrentTimestamp();
    }
}

std::shared_ptr<RSRenderNode> RSProfiler::GetRenderNode(uint64_t id)
{
    return g_renderServiceContext ? g_renderServiceContext->GetMutableNodeMap().GetRenderNode(id) : nullptr;
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

    const auto& rootRenderNode = g_renderServiceContext->GetGlobalRootRenderNode();
    auto& children = rootRenderNode->GetChildren();
    if (children.empty()) {
        return;
    }
    if (auto& displayNode = children.front()) {
        if (auto rootNode = GetRenderNode(Utils::PatchNodeId(0))) {
            g_childOfDisplayNodes = displayNode->GetChildren();

            displayNode->ClearChildren();
            displayNode->AddChild(rootNode);
        }
    }

    g_renderServiceThread->SetDirtyFlag();

    AwakeRenderServiceThread();
}

void RSProfiler::HiddenSpaceTurnOff()
{
    const auto& rootRenderNode = g_renderServiceContext->GetGlobalRootRenderNode();
    const auto& children = rootRenderNode->GetChildren();
    if (children.empty()) {
        return;
    }
    if (auto& displayNode = children.front()) {
        displayNode->ClearChildren();
        for (const auto& child : g_childOfDisplayNodes) {
            displayNode->AddChild(child);
        }
        FilterMockNode(g_renderServiceContext->GetMutableNodeMap());
        g_childOfDisplayNodes.clear();
    }

    g_renderServiceThread->SetDirtyFlag();

    AwakeRenderServiceThread();
}

std::string RSProfiler::FirstFrameMarshalling()
{
    std::stringstream stream;
    SetMode(Mode::WRITE_EMUL);
    RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
    MarshalNodes(*g_renderServiceContext, stream);
    RSMarshallingHelper::EndNoSharedMem();
    SetMode(Mode::NONE);

    const int32_t focusPid = g_renderServiceThread->focusAppPid_;
    stream.write(reinterpret_cast<const char*>(&focusPid), sizeof(focusPid));

    const int32_t focusUid = g_renderServiceThread->focusAppUid_;
    stream.write(reinterpret_cast<const char*>(&focusUid), sizeof(focusUid));

    const uint64_t focusNodeId = g_renderServiceThread->focusNodeId_;
    stream.write(reinterpret_cast<const char*>(&focusNodeId), sizeof(focusNodeId));

    const std::string bundleName = g_renderServiceThread->focusAppBundleName_;
    int32_t size = bundleName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(bundleName.data()), size);

    const std::string abilityName = g_renderServiceThread->focusAppAbilityName_;
    size = abilityName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(abilityName.data()), size);

    return stream.str();
}

void RSProfiler::FirstFrameUnmarshalling(const std::string& data)
{
    std::stringstream stream;
    stream.str(data);

    SetMode(Mode::READ_EMUL);

    RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
    UnmarshalNodes(*g_renderServiceContext, stream);
    RSMarshallingHelper::EndNoSharedMem();

    SetMode(Mode::NONE);

    int32_t focusPid = 0;
    stream.read(reinterpret_cast<char*>(&focusPid), sizeof(focusPid));

    int32_t focusUid = 0;
    stream.read(reinterpret_cast<char*>(&focusUid), sizeof(focusUid));

    uint64_t focusNodeId = 0;
    stream.read(reinterpret_cast<char*>(&focusNodeId), sizeof(focusNodeId));

    int32_t size = 0;
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
    g_renderServiceThread->SetFocusAppInfo(focusPid, focusUid, bundleName, abilityName, focusNodeId);
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
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_COUNT, GetCommandCount());
        captureData.SetProperty(RSCaptureData::KEY_RS_PIXEL_IMAGE_ADDED, GetImageCount());
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

void RSProfiler::DumpTree(const ArgList& args)
{
    if (!g_renderServiceContext) {
        return;
    }

    const RSRenderNodeMap& nodeMap = g_renderServiceContext->GetMutableNodeMap();

    std::map<std::string, std::tuple<NodeId, std::string>> list;
    GetSurfacesTrees(nodeMap, list);

    std::string out = "Tree: count=" + std::to_string(static_cast<int>(GetRenderNodeCount(nodeMap))) +
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
    GetSurfacesTreesByPid(nodeMap, args.Pid(), surfaces);

    std::string out;
    for (const auto& item : surfaces) {
        out += "*** " + std::to_string(item.first) + " pid=" + std::to_string(ExtractPid(item.first)) +
               " lowId=" + std::to_string(Utils::ExtractNodeId(item.first)) + "\n" + item.second + "\n";
    }

    out += "TREE: count=" + std::to_string(static_cast<int32_t>(GetRenderNodeCount(nodeMap))) +
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

    const Vector4f screenRect = GetScreenRect(*g_renderServiceContext);

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
    std::shared_ptr<RSRenderNode> node = nodeMap.GetRenderNode<RSRenderNode>(GetRandomSurfaceNode(nodeMap));
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

void RSProfiler::TestSaveFrame(const ArgList& args)
{
    g_testDataFrame = FirstFrameMarshalling();
    Respond("Save Frame Size: " + std::to_string(g_testDataFrame.size()));
}

void RSProfiler::TestLoadFrame(const ArgList& args)
{
    FirstFrameUnmarshalling(g_testDataFrame);
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

    auto& imageMap = RSProfiler::GetImageCache();
    ClearImageCache();

    g_recordFile.Create(RSFile::GetDefaultPath());
    g_recordFile.SetImageCache(reinterpret_cast<std::map<uint64_t, FileImageCacheRecord>*>(&imageMap));
    g_recordFile.AddLayer(); // add 0 layer

    auto& nodeMap = g_renderServiceContext->GetMutableNodeMap();
    FilterMockNode(nodeMap);

    g_recordFile.AddHeaderFirstFrame(FirstFrameMarshalling());

    std::vector<pid_t> pids = GetConnectionsPids();
    for (pid_t pid : pids) {
        g_recordFile.AddHeaderPid(pid);
    }

    SetMode(Mode::WRITE);

    g_recordStartTime = Utils::Now();

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

    const int32_t pidCount = g_recordFile.GetHeaderPids().size();
    stream.write(reinterpret_cast<const char*>(&pidCount), sizeof(pidCount));
    for (auto item : g_recordFile.GetHeaderPids()) {
        stream.write(reinterpret_cast<const char*>(&item), sizeof(item));
    }

    int sizeFirstFrame = g_recordFile.GetHeaderFirstFrame().size();
    stream.write(reinterpret_cast<const char*>(&sizeFirstFrame), sizeof(sizeFirstFrame));
    stream.write(reinterpret_cast<const char*>(&g_recordFile.GetHeaderFirstFrame()[0]), sizeFirstFrame);

    auto& imageMap = GetImageCache();

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

    ClearImageCache();

    Respond("Network: Record stop (" + std::to_string(stream.str().size()) + ")");
}

void RSProfiler::PlaybackStart(const ArgList& args)
{
    g_playbackPid = args.Pid();
    g_playbackStartTime = 0.0;

    auto imageCache = &GetImageCache();
    ClearImageCache();

    g_playbackFile.SetImageCache(reinterpret_cast<FileImageCache*>(imageCache));
    g_playbackFile.Open(RSFile::GetDefaultPath());
    if (!g_playbackFile.IsOpen()) {
        return;
    }
    std::string dataFirstFrame = g_playbackFile.GetHeaderFirstFrame();

    // get first frame data
    FirstFrameUnmarshalling(dataFirstFrame);
    HiddenSpaceTurnOn();

    for (size_t pid : g_playbackFile.GetHeaderPids()) {
        CreateMockConnection(Utils::GetMockPid(pid));
    }

    g_playbackStartTime = Utils::Now();

    const double pauseTime = args.Fp64(1);
    if (pauseTime > 0.0) {
        const uint64_t currentTime = Utils::RawNowNano();
        const uint64_t pauseTimeStart = currentTime + pauseTime * NANOSECONDS_IN_SECONDS;
        TimePauseAt(currentTime, pauseTimeStart);
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
    HiddenSpaceTurnOff();
    FilterMockNode(g_renderServiceContext->GetMutableNodeMap());
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

        RSRenderServiceConnection* connection = GetConnection(Utils::GetMockPid(pid));
        if (!connection) {
            const std::vector<pid_t>& pids = g_playbackFile.GetHeaderPIDList();
            if (!pids.empty()) {
                connection = GetConnection(Utils::GetMockPid(pids[0]));
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
        parcel.parcel.SetMaxCapacity(dataSize + 1);
        parcel.parcel.WriteBuffer(data, dataSize);

        delete[] data;

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
        TimePauseClear();
    }
}

void RSProfiler::PlaybackPrepare(const ArgList& args)
{
    const uint32_t pid = args.Pid();
    if (!g_renderServiceContext || (pid == 0)) {
        return;
    }

    FilterForPlayback(g_renderServiceContext->GetMutableNodeMap(), pid);
    AwakeRenderServiceThread();
    Respond("OK");
}

void RSProfiler::PlaybackPause(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    const uint64_t currentTime = Utils::RawNowNano();
    const double recordPlayTime = Utils::Now() - g_playbackStartTime;
    TimePauseAt(currentTime, currentTime);
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

    TimePauseAt(curTimeRaw, pauseTimeStart);

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

    const uint64_t curTimeRaw =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();

    TimePauseResume(curTimeRaw);

    ResetAnimationStamp();

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
        { "rstree_save_frame", TestSaveFrame },
        { "rstree_load_frame", TestLoadFrame },
        { "rstree_switch", TestSwitch },
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
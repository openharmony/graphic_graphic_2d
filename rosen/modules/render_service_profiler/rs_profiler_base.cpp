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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <sys/mman.h>
#include <type_traits>
#include <utility>
#include <vector>

#include "message_parcel.h"
#include "rs_profiler.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_capture_recorder.h"
#include "rs_profiler_file.h"
#include "rs_profiler_log.h"
#include "rs_profiler_network.h"
#include "rs_profiler_utils.h"
#include "sys_binder.h"

#include "animation/rs_animation_manager.h"
#include "command/rs_base_node_command.h"
#include "command/rs_canvas_drawing_node_command.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_depth_node_command.h"
#include "command/rs_effect_node_command.h"
#include "command/rs_proxy_node_command.h"
#include "command/rs_root_node_command.h"
#include "command/rs_surface_node_command.h"
#include "feature/window_keyframe/rs_window_keyframe_node_command.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "transaction/rs_ashmem_helper.h"

#include "ge_shader_filter.h"

namespace OHOS::Rosen {
std::atomic_bool RSProfiler::recordAbortRequested_ = false;
std::atomic_uint32_t RSProfiler::mode_ = static_cast<uint32_t>(Mode::NONE);
RSProfiler::LogicalDisplayChildren RSProfiler::displayChildren_;
static thread_local uint32_t g_subMode = static_cast<uint32_t>(SubMode::NONE);
static std::atomic<uint32_t> g_commandCount = 0;        // UNMARSHALLING RSCOMMAND COUNT
static std::atomic<uint32_t> g_commandExecuteCount = 0; // EXECUTE RSCOMMAND COUNT

static std::mutex g_msgBaseMutex;
static std::queue<std::string> g_msgBaseList;

static std::mutex g_rsLogListMutex;
static std::queue<RSProfilerLogMsg> g_rsLogList;

static std::mutex g_mutexCommandOffsets;
static std::map<uint32_t, std::vector<uint32_t>> g_parcelNumber2Offset;

static uint64_t g_pauseAfterTime = 0;
static int64_t g_pauseCumulativeTime = 0;
static int64_t g_transactionTimeCorrection = 0;
static std::atomic<int64_t> g_replayStartTimeNano = 0;
static double g_replaySpeed = 1.0;

static const size_t PARCEL_MAX_CAPACITY = 234 * 1024 * 1024;

static std::unordered_map<AnimationId, std::vector<int64_t>> g_animeStartMap;

bool RSProfiler::testing_ = false;
std::vector<std::shared_ptr<RSRenderNode>> RSProfiler::testTree_ = std::vector<std::shared_ptr<RSRenderNode>>();
bool RSProfiler::enabled_ = RSSystemProperties::GetProfilerEnabled();
bool RSProfiler::hrpServiceEnabled_ = RSSystemProperties::GetProfilerEnabled();
std::atomic<bool> RSProfiler::betaRecordingEnabled_ = RSSystemProperties::GetBetaRecordingMode() != 0;
std::atomic<int8_t> RSProfiler::signalFlagChanged_ = 0;
std::atomic_bool RSProfiler::dcnRedraw_ = false;
std::atomic_bool RSProfiler::renderNodeKeepDrawCmdList_ = false;
std::unordered_map<AnimationId, int64_t> RSProfiler::animationsTimes_;

static std::atomic<TextureRecordType> g_textureRecordType = TextureRecordType::LZ4;

static std::shared_ptr<ProfilerMarshallingJob> g_marshallingJob;
static std::atomic<bool> g_marshalFirstFrameThread = false;

static std::atomic<uint64_t> g_counterParseTransactionDataStart = 0;
static std::atomic<uint64_t> g_counterParseTransactionDataEnd = 0;

constexpr size_t GetParcelMaxCapacity()
{
    return PARCEL_MAX_CAPACITY;
}

static std::vector<RSRenderNode::SharedPtr> GetChildren(const RSRenderNode& node)
{
    std::vector<RSRenderNode::SharedPtr> children;
    for (const auto& childWeak : node.GetChildrenList()) {
        if (const auto child = childWeak.lock()) {
            children.push_back(child);
        }
    }
    return children;
}

bool RSProfiler::IsEnabled()
{
    return enabled_ || testing_;
}

bool RSProfiler::IsHrpServiceEnabled()
{
    return hrpServiceEnabled_;
}

bool RSProfiler::IsBetaRecordEnabled()
{
#ifdef RS_PROFILER_BETA_ENABLED
    return betaRecordingEnabled_;
#else
    return false;
#endif
}

bool RSProfiler::IsNoneMode()
{
    return GetMode() == Mode::NONE;
}

bool RSProfiler::IsReadMode()
{
    return GetMode() == Mode::READ;
}

bool RSProfiler::IsReadEmulationMode()
{
    return GetSubMode() == SubMode::READ_EMUL;
}

bool RSProfiler::IsWriteMode()
{
    return GetMode() == Mode::WRITE;
}

bool RSProfiler::IsWriteEmulationMode()
{
    return GetSubMode() == SubMode::WRITE_EMUL;
}

bool RSProfiler::IsSavingMode()
{
    return GetMode() == Mode::SAVING;
}

void RSProfiler::AddLightBlursMetrics(uint32_t areaBlurs)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }

    GetCustomMetrics().AddInt(RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS, 1);
    GetCustomMetrics().AddInt(RSPROFILER_METRIC_BLUR_OPERATIONS, 1);
    GetCustomMetrics().AddFloat(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS, areaBlurs);
}
void RSProfiler::AddAnimationNodeMetrics(RSRenderNodeType type, int32_t size)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }

    GetCustomMetrics().AddInt(RSPROFILER_METRIC_ANIMATION_NODE, 1);
    GetCustomMetrics().AddFloat(RSPROFILER_METRIC_ANIMATION_NODE_SIZE, size);

    int profiler_node_type = -1;
    switch (type) {
        case RSRenderNodeType::SURFACE_NODE:
            profiler_node_type = RSPROFILER_METRIC_ANIMATION_NODE_TYPE_SURFACE_NODE;
            break;
        case RSRenderNodeType::PROXY_NODE:
            profiler_node_type = RSPROFILER_METRIC_ANIMATION_NODE_TYPE_PROXY_NODE;
            break;
        case RSRenderNodeType::CANVAS_NODE:
            profiler_node_type = RSPROFILER_METRIC_ANIMATION_NODE_TYPE_CANVAS_NODE;
            break;
        case RSRenderNodeType::EFFECT_NODE:
            profiler_node_type = RSPROFILER_METRIC_ANIMATION_NODE_TYPE_EFFECT_NODE;
            break;
        case RSRenderNodeType::ROOT_NODE:
            profiler_node_type = RSPROFILER_METRIC_ANIMATION_NODE_TYPE_ROOT_NODE;
            break;
        case RSRenderNodeType::CANVAS_DRAWING_NODE:
            profiler_node_type = RSPROFILER_METRIC_ANIMATION_NODE_TYPE_CANVAS_DRAWING_NODE;
            break;
        default:  // exclude RSRenderNodeType::(RS_NODE, UNKNOW, DISPLAY_NODE)
            break;
    }

    if (profiler_node_type >= 0) {
        GetCustomMetrics().AddInt(profiler_node_type, 1);
    }
}

void RSProfiler::AddAnimationStart(AnimationId id, int64_t timestamp_ns)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }
    animationsTimes_[id] = timestamp_ns;
}

void RSProfiler::AddAnimationFinish(AnimationId id, int64_t timestamp_ns)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }
    if (animationsTimes_.count(id) == 1) {
        GetCustomMetrics().AddFloat(
            RSPROFILER_METRIC_ANIMATION_DURATION, float(timestamp_ns - animationsTimes_[id]) / 1'000'000'000.f);
        animationsTimes_.erase(id);
    }
}

void RSProfiler::AddHPSBlursMetrics(uint32_t areaBlurs)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }

    GetCustomMetrics().AddInt(RSPROFILER_METRIC_HPS_BLUR_OPERATIONS, 1);
    GetCustomMetrics().AddInt(RSPROFILER_METRIC_BLUR_OPERATIONS, 1);
    GetCustomMetrics().AddFloat(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS, areaBlurs);
}

void RSProfiler::AddKawaseBlursMetrics(uint32_t areaBlurs)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }

    GetCustomMetrics().AddInt(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS, 1);
    GetCustomMetrics().AddInt(RSPROFILER_METRIC_BLUR_OPERATIONS, 1);
    GetCustomMetrics().AddFloat(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS, areaBlurs);
}

void RSProfiler::AddMESABlursMetrics(uint32_t areaBlurs)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }

    GetCustomMetrics().AddInt(RSPROFILER_METRIC_MESA_BLUR_OPERATIONS, 1);
    GetCustomMetrics().AddInt(RSPROFILER_METRIC_BLUR_OPERATIONS, 1);
    GetCustomMetrics().AddFloat(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS, areaBlurs);
}

void RSProfiler::LogShaderCall(const std::string& shaderType, const std::shared_ptr<Drawing::Image>& srcImage,
    const Drawing::Rect& dstRect, const std::shared_ptr<Drawing::Image>& outImage)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }

    if (shaderType == "KAWASE_BLUR") {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_KAWASE_BLUR_SHADER_CALLS, 1);
    } else if (shaderType == "MESA_BLUR") {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_MESA_BLUR_SHADER_CALLS, 1);
    } else if (shaderType == "AIBAR") {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_AIBAR_BLUR_SHADER_CALLS, 1);
    } else if (shaderType == "GREY") {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_GREY_BLUR_SHADER_CALLS, 1);
    } else if (shaderType == "LINEAR_GRADIENT_BLUR") {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_LINEAR_GRADIENT_BLUR_SHADER_CALLS, 1);
    } else if (shaderType == "MAGNIFIER") {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_MAGNIFIER_SHADER_CALLS, 1);
    } else if (shaderType == "WATER_RIPPLE") {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_WATER_RIPPLE_BLUR_SHADER_CALLS, 1);
    }
    GetCustomMetrics().AddInt(RSPROFILER_METRIC_BLUR_SHADER_CALLS, 1);
    GetCustomMetrics().AddFloat(RSPROFILER_METRIC_BLUR_AREA_SHADER_CALLS,
        srcImage ? srcImage->GetWidth() * srcImage->GetHeight() : 0);
}

uint32_t RSProfiler::GetCommandCount()
{
    const uint32_t count = g_commandCount;
    g_commandCount = 0;
    return count;
}

uint32_t RSProfiler::GetCommandExecuteCount()
{
    const uint32_t count = g_commandExecuteCount;
    g_commandExecuteCount = 0;
    return count;
}

void RSProfiler::EnableSharedMemory()
{
    RSMarshallingHelper::EndNoSharedMem();
}

void RSProfiler::DisableSharedMemory()
{
    RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
}

bool RSProfiler::IsSharedMemoryEnabled()
{
    return RSMarshallingHelper::GetUseSharedMem(std::this_thread::get_id());
}

bool RSProfiler::IsParcelMock(const Parcel& parcel)
{
    // gcc C++ optimization error (?): this is not working without volatile
    const volatile auto address = reinterpret_cast<uint64_t>(&parcel);
    return ((address & 1u) != 0);
}

bool RSProfiler::IsPlaybackParcel(const Parcel& parcel)
{
    return (IsReadMode() || IsReadEmulationMode())
        && IsParcelMock(parcel);
}

std::shared_ptr<MessageParcel> RSProfiler::CopyParcel(const MessageParcel& parcel)
{
    if (!IsEnabled()) {
        return std::make_shared<MessageParcel>();
    }

    if (IsParcelMock(parcel)) {
        auto* buffer = new(std::nothrow) uint8_t[sizeof(MessageParcel) + 1];
        if (!buffer) {
            return std::make_shared<MessageParcel>();
        }
        auto* mpPtr = new (buffer + 1) MessageParcel;
        return std::shared_ptr<MessageParcel>(mpPtr, [](MessageParcel* ptr) {
            ptr->~MessageParcel();
            auto* allocPtr = reinterpret_cast<uint8_t*>(ptr);
            allocPtr--;
            delete[] allocPtr;
        });
    }

    return std::make_shared<MessageParcel>();
}

NodeId RSProfiler::PatchPlainNodeId(const Parcel& parcel, NodeId id)
{
    if (!IsEnabled()) {
        return id;
    }

    if ((!IsReadMode() && !IsReadEmulationMode()) || !IsParcelMock(parcel)) {
        return id;
    }

    return Utils::PatchNodeId(id);
}

void RSProfiler::PatchTypefaceId(const Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val)
{
    if (!val || !IsEnabled()) {
        return;
    }

    if (IsReadEmulationMode()) {
        val->PatchTypefaceIds();
    } else if (IsReadMode()) {
        if (IsParcelMock(parcel)) {
            val->PatchTypefaceIds();
        }
    }
}

pid_t RSProfiler::PatchPlainPid(const Parcel& parcel, pid_t pid)
{
    if (!IsEnabled() || (!IsReadMode() && !IsReadEmulationMode()) || !IsParcelMock(parcel)) {
        return pid;
    }

    return Utils::GetMockPid(pid);
}

void RSProfiler::SetMode(Mode mode)
{
    mode_ = static_cast<uint32_t>(mode);
    if (IsNoneMode()) {
        g_pauseAfterTime = 0;
        g_pauseCumulativeTime = 0;
        g_replayStartTimeNano = 0;
    }
}

Mode RSProfiler::GetMode()
{
    return static_cast<Mode>(mode_.load());
}

void RSProfiler::SetSubMode(SubMode subMode)
{
    g_subMode = static_cast<uint32_t>(subMode);
}

SubMode RSProfiler::GetSubMode()
{
    return static_cast<SubMode>(g_subMode);
}

uint64_t RSProfiler::PatchTime(uint64_t time)
{
    if (!IsEnabled()) {
        return time;
    }
    if (!IsReadMode() && !IsReadEmulationMode()) {
        return time;
    }
    if (time == 0.0) {
        return 0.0;
    }
    if (time >= g_pauseAfterTime && g_pauseAfterTime > 0) {
        return (static_cast<int64_t>(g_pauseAfterTime) - g_pauseCumulativeTime - g_replayStartTimeNano) *
            BaseGetPlaybackSpeed() + g_replayStartTimeNano;
    }
    return (static_cast<int64_t>(time) - g_pauseCumulativeTime - g_replayStartTimeNano) *
        BaseGetPlaybackSpeed() + g_replayStartTimeNano;
}

uint64_t RSProfiler::PatchTransactionTime(const Parcel& parcel, uint64_t time)
{
    if (!IsEnabled()) {
        return time;
    }

    if (!IsReadMode()) {
        return time;
    }
    if (time == 0.0) {
        return 0.0;
    }
    if (!IsParcelMock(parcel)) {
        return time;
    }

    return PatchTime(time + g_transactionTimeCorrection);
}

void RSProfiler::TimePauseAt(uint64_t curTime, uint64_t newPauseAfterTime, bool immediate)
{
    if (g_pauseAfterTime > 0) {
        // second time pause
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += static_cast<int64_t>(curTime - g_pauseAfterTime);
        }
    }
    g_pauseAfterTime = newPauseAfterTime;
    if (immediate) {
        g_pauseCumulativeTime += static_cast<int64_t>(curTime - g_pauseAfterTime);
        g_pauseAfterTime = curTime;
    }
}

void RSProfiler::TimePauseResume(uint64_t curTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += static_cast<int64_t>(curTime - g_pauseAfterTime);
        }
    }
    g_pauseAfterTime = 0;
}

void RSProfiler::TimePauseClear()
{
    g_pauseCumulativeTime = 0;
    g_pauseAfterTime = 0;
}

uint64_t RSProfiler::TimePauseGet()
{
    return g_pauseAfterTime;
}

std::shared_ptr<RSScreenRenderNode> RSProfiler::GetScreenNode(const RSContext& context)
{
    const std::shared_ptr<RSBaseRenderNode>& root = context.GetGlobalRootRenderNode();
    // without these checks device might get stuck on startup
    if (!root || !root->GetChildrenCount()) {
        return nullptr;
    }

    const auto& children = *root->GetChildren();
    if (children.empty()) {
        return nullptr;
    }
    for (const auto& screenNode : children) {   // apply multiple screen nodes
        if (!screenNode) {
            continue;
        }
        const auto& screenNodeChildren = screenNode->GetChildren();
        if (!screenNodeChildren || screenNodeChildren->empty()) {
            continue;
        }
        return RSBaseRenderNode::ReinterpretCast<RSScreenRenderNode>(screenNode);
    }
    return nullptr;
}

Vector4f RSProfiler::GetScreenRect(const RSContext& context)
{
    std::shared_ptr<RSScreenRenderNode> node = GetScreenNode(context);
    if (!node) {
        return {};
    }

    const RectI rect = node->GetDirtyManager()->GetSurfaceRect();
    return { rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() };
}

void RSProfiler::FilterForPlayback(RSContext& context, pid_t pid)
{
    auto& map = context.GetMutableNodeMap();

    auto canBeRemoved = [](NodeId node, pid_t pid) -> bool {
        return (ExtractPid(node) == pid) && (Utils::ExtractNodeId(node) != 1);
    };

    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    auto iter = map.renderNodeMap_.find(pid);
    if (iter != map.renderNodeMap_.end()) {
        auto& subMap = iter->second;
        EraseIf(subMap, [](const auto& pair) -> bool {
            if (Utils::ExtractNodeId(pair.first) == 1) {
                return false;
            }
            // remove node from tree
            pair.second->RemoveFromTree(false);
            return true;
        });
        if (subMap.empty()) {
            map.renderNodeMap_.erase(pid);
        }
    }

    EraseIf(
        map.surfaceNodeMap_, [pid, canBeRemoved](const auto& pair) -> bool { return canBeRemoved(pair.first, pid); });

    EraseIf(map.residentSurfaceNodeMap_,
        [pid, canBeRemoved](const auto& pair) -> bool { return canBeRemoved(pair.first, pid); });

    EraseIf(
        map.screenNodeMap_, [pid, canBeRemoved](const auto& pair) -> bool { return canBeRemoved(pair.first, pid); });

    if (auto fallbackNode = map.GetAnimationFallbackNode()) {
        if (auto animationManager = fallbackNode->GetAnimationManager()) {
            animationManager->FilterAnimationByPid(pid);
        }
    }
}

void RSProfiler::FilterMockNode(RSContext& context)
{
    std::unordered_set<pid_t> pidSet;

    auto& nodeMap = context.GetMutableNodeMap();
    nodeMap.TraversalNodes([&pidSet](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr) {
            return;
        }
        if (Utils::IsNodeIdPatched(node->GetId())) {
            pidSet.insert(Utils::ExtractPid(node->GetId()));
        }
    });

    for (auto pid : pidSet) {
        nodeMap.FilterNodeByPid(pid, true);
    }

    if (auto fallbackNode = nodeMap.GetAnimationFallbackNode()) {
        // remove all fallback animations belong to given pid
        if (auto animationManager = fallbackNode->GetAnimationManager()) {
            FilterAnimationForPlayback(animationManager);
        }
    }
}

size_t RSProfiler::GetRenderNodeCount(const RSContext& context)
{
    return const_cast<RSContext&>(context).GetMutableNodeMap().GetSize();
}

NodeId RSProfiler::GetRandomSurfaceNode(const RSContext& context)
{
    const RSRenderNodeMap& map = const_cast<RSContext&>(context).GetMutableNodeMap();
    for (const auto& item : map.surfaceNodeMap_) {
        return item.first;
    }
    return 0;
}

void RSProfiler::MarshalNodes(const RSContext& context, std::stringstream& data, uint32_t fileVersion,
    const std::shared_ptr<ProfilerMarshallingJob>& job)
{
    std::unordered_set<RSRenderNode::SharedPtr> roots;
    std::vector<RSRenderNode::SharedPtr> nodes;
    context.GetNodeMap().TraversalNodes([&nodes, &roots](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node) {
            nodes.push_back(node);
            if (!node->GetParent().lock()) {
                roots.insert(node);
            }
            GetCustomMetrics().AddInt(
                node->IsOnTheTree() ? RSPROFILER_METRIC_ONTREE_NODE_COUNT : RSPROFILER_METRIC_OFFTREE_NODE_COUNT, 1);
        }
    });

    if (const auto& root = context.GetGlobalRootRenderNode(); root && !nodes.empty()) {
        roots.insert(root);
    }

    // Marshal nodes
    const auto offsetNodeCount = std::max(static_cast<std::stringstream::pos_type>(0), data.tellp());
    const auto nodeCount = static_cast<uint32_t>(nodes.size());
    data.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));
    for (const auto& node : nodes) {
        if (job && node->GetId()) {
            job->AddNode(node->GetId());
        } else {
            MarshalNode(*node, data, fileVersion);
        }
    }

    // Marshal roots
    const auto offsetRootCount = std::max(static_cast<std::stringstream::pos_type>(0), data.tellp());
    const auto rootCount = static_cast<uint32_t>(roots.size());
    data.write(reinterpret_cast<const char*>(&rootCount), sizeof(rootCount));
    for (const auto& root : roots) {
        MarshalTree(*root, data, fileVersion);
    }

    if (job) {
        job->offsetNodeCount = static_cast<size_t>(offsetNodeCount);
        job->offsetNodes = static_cast<size_t>(offsetRootCount);
    }
    g_marshallingJob = job;
}

void RSProfiler::MarshalTree(const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion, uint32_t depth)
{
    constexpr uint32_t maxDepth = 1024u;
    if (depth >= maxDepth) {
        return;
    }

    const NodeId id = node.GetId();
    data.write(reinterpret_cast<const char*>(&id), sizeof(id));

    const auto children = GetChildren(node);
    const uint32_t count = children.size();
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (const auto& child : children) {
        const NodeId id = child->GetId();
        data.write(reinterpret_cast<const char*>(&id), sizeof(id));
        MarshalTree(*child, data, fileVersion, depth + 1u);
    }
}

void RSProfiler::MarshalNode(const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion,
    bool skipDrawCmdModifiers, bool isBetaRecording)
{
    const RSRenderNodeType nodeType = node.GetType();
    data.write(reinterpret_cast<const char*>(&nodeType), sizeof(nodeType));

    const NodeId nodeId = node.GetId();
    data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));

    const bool isTextureExportNode = node.GetIsTextureExportNode();
    data.write(reinterpret_cast<const char*>(&isTextureExportNode), sizeof(isTextureExportNode));

    if (node.GetType() == RSRenderNodeType::SURFACE_NODE) {
        const auto& surfaceNode = reinterpret_cast<const RSSurfaceRenderNode&>(node);
        const std::string name = surfaceNode.GetName();
        uint32_t size = name.size();
        data.write(reinterpret_cast<const char*>(&size), sizeof(size));
        data.write(reinterpret_cast<const char*>(name.c_str()), size);

        const std::string bundleName = "";
        size = bundleName.size();
        data.write(reinterpret_cast<const char*>(&size), sizeof(size));
        data.write(reinterpret_cast<const char*>(bundleName.c_str()), size);

        const RSSurfaceNodeType type = surfaceNode.GetSurfaceNodeType();
        data.write(reinterpret_cast<const char*>(&type), sizeof(type));

        const uint8_t backgroundAlpha = surfaceNode.GetAbilityBgAlpha();
        data.write(reinterpret_cast<const char*>(&backgroundAlpha), sizeof(backgroundAlpha));

        const uint8_t globalAlpha = surfaceNode.GetGlobalAlpha();
        data.write(reinterpret_cast<const char*>(&globalAlpha), sizeof(globalAlpha));
    }

    const float positionZ = node.GetRenderProperties().GetPositionZ();
    data.write(reinterpret_cast<const char*>(&positionZ), sizeof(positionZ));

    const float pivotZ = node.GetRenderProperties().GetPivotZ();
    data.write(reinterpret_cast<const char*>(&pivotZ), sizeof(pivotZ));

    const NodePriorityType priority = NodePriorityType::MAIN_PRIORITY;
    data.write(reinterpret_cast<const char*>(&priority), sizeof(priority));

    const bool isOnTree = node.IsOnTheTree();
    data.write(reinterpret_cast<const char*>(&isOnTree), sizeof(isOnTree));

    if (fileVersion >= RSFILE_VERSION_RENDER_METRICS_ADDED) {
        const uint8_t nodeGroupType = node.nodeGroupType_;
        data.write(reinterpret_cast<const char*>(&nodeGroupType), sizeof(nodeGroupType));
    }

    if (fileVersion >= RSFILE_VERSION_ISREPAINT_BOUNDARY) {
        const bool isRepaintBoundary = node.IsRepaintBoundary();
        data.write(reinterpret_cast<const char*>(&isRepaintBoundary), sizeof(isRepaintBoundary));
    }

    MarshalNodeModifiers(node, data, fileVersion, skipDrawCmdModifiers, isBetaRecording);
}

static void MarshalRenderModifier(const ModifierNG::RSRenderModifier& modifier, std::stringstream& data)
{
    Parcel parcel;
    parcel.SetMaxCapacity(GetParcelMaxCapacity());

    // Parcel Code - can be any, in our case I selected -1 to support already captured subtrees
    parcel.WriteInt32(-1);
    // MARSHAL PARCEL VERSION
    if (!RSMarshallingHelper::MarshallingTransactionVer(parcel)) {
        return;
    }

    const_cast<ModifierNG::RSRenderModifier&>(modifier).Marshalling(parcel, false);
    const size_t dataSize = parcel.GetDataSize();
    data.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
    data.write(reinterpret_cast<const char*>(parcel.GetData()), dataSize);

    // Remove all file descriptors
    binder_size_t* object = reinterpret_cast<binder_size_t*>(parcel.GetObjectOffsets());
    size_t objectNum = parcel.GetOffsetsSize();
    uintptr_t parcelData = parcel.GetData();

    const size_t maxObjectNum = INT_MAX;
    if (!object || (objectNum > maxObjectNum)) {
        return;
    }

    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(parcelData + object[i]);
        if (!flat) {
            return;
        }
        if (flat->hdr.type == BINDER_TYPE_FD && flat->handle > 0) {
            ::close(flat->handle);
        }
    }
}

bool RSProfiler::MarshalDrawCmdModifiers(const ModifierNG::RSRenderModifier& modifier, std::stringstream& data,
    bool skipDrawCmdModifiers, bool isBetaRecording)
{
    const auto simpleCmdList = modifier.IsCustom() ? modifier.GetPropertySimpleDrawCmdList() : nullptr;
    if (simpleCmdList && skipDrawCmdModifiers) {
        return false;
    }

    if (!simpleCmdList) {
        MarshalRenderModifier(modifier, data);
        return true;
    }

    const auto cmdList = std::make_shared<Drawing::DrawCmdList>(
        simpleCmdList->GetWidth(), simpleCmdList->GetHeight(), Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    if (!cmdList) {
        return false;
    }

    for (const auto& drawOp : simpleCmdList->GetDrawOpItems()) {
        if (auto copy = drawOp) {
            cmdList->AddDrawOp(std::move(copy));
        }
    }
    cmdList->SetNoImageMarshallingFlag(isBetaRecording);
    cmdList->ProfilerMarshallingDrawOps(cmdList.get());
    cmdList->PatchTypefaceIds(cmdList);

    const auto type = ModifierNG::ModifierTypeConvertor::GetPropertyType(modifier.GetType());
    const auto oldProperty = const_cast<ModifierNG::RSRenderModifier&>(modifier).GetProperty(type);
    const auto newProperty =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(cmdList, oldProperty ? oldProperty->GetId() : 0);
    if (newProperty) {
        const_cast<ModifierNG::RSRenderModifier&>(modifier).properties_[type] = newProperty;
        MarshalRenderModifier(modifier, data);
        const_cast<ModifierNG::RSRenderModifier&>(modifier).properties_[type] = oldProperty;
        return true;
    }
    return false;
}

static std::shared_ptr<ModifierNG::RSRenderModifier> CreateSnapshotModifier(const RSRenderNode& node, uint32_t version)
{
    if (!node.IsInstanceOf<RSCanvasDrawingRenderNode>() || (version < RSFILE_VERSION_SELF_DRAWING_RESTORES)) {
        return nullptr;
    }

    const auto drawable = node.GetRenderDrawable();
    const auto image = drawable ? drawable->Snapshot() : nullptr;
    if (!image) {
        return nullptr;
    }

    const auto drawOp = std::make_shared<Drawing::DrawImageOpItem>(*image, 0, 0,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR), Drawing::Paint());
    if (!drawOp) {
        return nullptr;
    }

    const auto cmdList = std::make_shared<Drawing::DrawCmdList>(
        image->GetWidth(), image->GetHeight(), Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    if (!cmdList) {
        return nullptr;
    }
    cmdList->AddDrawOp(drawOp);
    cmdList->MarshallingDrawOps();

    if (const auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(cmdList, 0)) {
        return ModifierNG::RSRenderModifier::MakeRenderModifier(ModifierNG::RSModifierType::CONTENT_STYLE, property);
    }
    return nullptr;
}

void RSProfiler::MarshalNodeModifiers(const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion,
    bool skipDrawCmdModifiers, bool isBetaRecording)
{
    data.write(reinterpret_cast<const char*>(&node.instanceRootNodeId_), sizeof(node.instanceRootNodeId_));
    data.write(reinterpret_cast<const char*>(&node.firstLevelNodeId_), sizeof(node.firstLevelNodeId_));

    uint32_t modifierNGCount = 0;
    long long countOffset = data.tellp();
    data.write(reinterpret_cast<const char*>(&modifierNGCount), sizeof(modifierNGCount));
    if (skipDrawCmdModifiers) {
        return;
    }

    for (const auto& [_, slot] : node.GetAllModifiers()) {
        for (auto& modifierNG : slot) {
            if (!modifierNG || modifierNG->GetType() == ModifierNG::RSModifierType::PARTICLE_EFFECT) {
                continue;
            }
            modifierNGCount += MarshalDrawCmdModifiers(*modifierNG, data, skipDrawCmdModifiers, isBetaRecording);
        }
    }

    if (!isBetaRecording) {
        const auto snapshot = CreateSnapshotModifier(node, fileVersion);
        if (snapshot) {
            MarshalRenderModifier(*snapshot, data); // mustn't be called in beta-recording mode
            modifierNGCount++;
        }
    }

    data.seekp(countOffset, std::ios_base::beg);
    data.write(reinterpret_cast<const char*>(&modifierNGCount), sizeof(modifierNGCount));
    data.seekp(0, std::ios_base::end);
}

std::string RSProfiler::UnmarshalNodes(RSContext& context, std::stringstream& data, uint32_t fileVersion)
{
    constexpr uint32_t maxCount = 10'000u;
    uint32_t count = 0;
    if (!data.read(reinterpret_cast<char*>(&count), sizeof(count)) || (count > maxCount)) {
        return "UnmarshalNodes: Node count out of range";
    }

    for (uint32_t i = 0; i < count; i++) {
        auto error = UnmarshalNode(context, data, fileVersion);
        if (!error.empty()) {
            FilterMockNode(context);
            return error;
        }
    }

    if (!data.read(reinterpret_cast<char*>(&count), sizeof(count)) || (count > maxCount)) {
        FilterMockNode(context);
        return "UnmarshalNodes: Tree node count out of range";
    }

    for (uint32_t i = 0; i < count; i++) {
        auto error = UnmarshalTree(context, data, fileVersion);
        if (!error.empty()) {
            FilterMockNode(context);
            return error;
        }
    }

    MarkReplayNodesDirty(context);
    return "";
}

void RSProfiler::MarkReplayNodesDirty(RSContext& context)
{
    context.GetNodeMap().TraversalNodes([](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node && Utils::IsNodeIdPatched(node->GetId())) {
            node->SetContentDirty();
            node->SetDirty();
        }
    });
}

static std::string CreateRenderSurfaceNode(
    RSContext& context, NodeId id, bool isTextureExportNode, std::stringstream& data)
{
    constexpr uint32_t maxSize = 4096u;
    uint32_t size = 0u;
    if (!data.read(reinterpret_cast<char*>(&size), sizeof(size)) || (size > maxSize)) {
        return "CreateRenderSurfaceNode: Invalid name size";
    }

    std::string name(size, 0);
    if (!data.read(reinterpret_cast<char*>(name.data()), size)) {
        return "CreateRenderSurfaceNode: Cannot read name";
    }

    if (!data.read(reinterpret_cast<char*>(&size), sizeof(size)) || (size > maxSize)) {
        return "CreateRenderSurfaceNode: Invalid bundle name size";
    }

    std::string bundle(size, 0);
    if (!data.read(reinterpret_cast<char*>(bundle.data()), size)) {
        return "CreateRenderSurfaceNode: Cannot read bundle name";
    }

    auto type = RSSurfaceNodeType::DEFAULT;
    if (!data.read(reinterpret_cast<char*>(&type), sizeof(type))) {
        return "CreateRenderSurfaceNode: Cannot read type";
    }

    uint8_t backgroundAlpha = 0u;
    if (!data.read(reinterpret_cast<char*>(&backgroundAlpha), sizeof(backgroundAlpha))) {
        return "CreateRenderSurfaceNode: Cannot read backgroundAlpha";
    }

    uint8_t globalAlpha = 0u;
    if (!data.read(reinterpret_cast<char*>(&globalAlpha), sizeof(globalAlpha))) {
        return "CreateRenderSurfaceNode: Cannot read globalAlpha";
    }

    const RSSurfaceRenderNodeConfig config {
        .id = id,
        .name = name + "_",
        .nodeType = type,
        .additionalData = nullptr,
        .isTextureExportNode = isTextureExportNode,
        .isSync = false,
    };

    if (const auto node = SurfaceNodeCommandHelper::CreateWithConfigInRS(config, context)) {
        context.GetMutableNodeMap().RegisterRenderNode(node);
        node->SetAbilityBGAlpha(backgroundAlpha);
        node->SetGlobalAlpha(globalAlpha);
    }
    return "";
}

std::string RSProfiler::UnmarshalNode(RSContext& context, std::stringstream& data, uint32_t fileVersion)
{
    auto type = RSRenderNodeType::UNKNOW;
    if (!data.read(reinterpret_cast<char*>(&type), sizeof(type))) {
        return "UnmarshalNode: Cannot read type";
    }

    NodeId id = 0;
    if (!data.read(reinterpret_cast<char*>(&id), sizeof(id))) {
        return "UnmarshalNode: Cannot read id";
    }
    id = Utils::PatchNodeId(id);

    bool isTextureExportNode = false;
    if (!data.read(reinterpret_cast<char*>(&isTextureExportNode), sizeof(isTextureExportNode))) {
        return "UnmarshalNode: Cannot read isTextureExportNode";
    }

    if (type == RSRenderNodeType::RS_NODE) {
        RootNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::SCREEN_NODE) {
        RootNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::LOGICAL_DISPLAY_NODE) {
        RootNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::SURFACE_NODE) {
        auto error = CreateRenderSurfaceNode(context, id, isTextureExportNode, data);
        if (!error.empty()) {
            return error;
        }
    } else if (type == RSRenderNodeType::PROXY_NODE) {
        ProxyNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::CANVAS_NODE) {
        RSCanvasNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::EFFECT_NODE) {
        EffectNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::ROOT_NODE) {
        RootNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::DEPTH_NODE) {
        RSDepthNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        RSCanvasDrawingNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else if (type == RSRenderNodeType::WINDOW_KEYFRAME_NODE) {
        RSWindowKeyFrameNodeCommandHelper::Create(context, id, isTextureExportNode);
    } else {
        RootNodeCommandHelper::Create(context, id, isTextureExportNode);
    }
    
    return UnmarshalNode(context, data, id, fileVersion);
}

std::string RSProfiler::UnmarshalNode(RSContext& context, std::stringstream& data, NodeId nodeId, uint32_t fileVersion)
{
    float positionZ = 0.0f;
    if (!data.read(reinterpret_cast<char*>(&positionZ), sizeof(positionZ))) {
        return "UnmarshalNode: Cannot read positionZ";
    }

    float pivotZ = 0.0f;
    if (!data.read(reinterpret_cast<char*>(&pivotZ), sizeof(pivotZ))) {
        return "UnmarshalNode: Cannot read pivotZ";
    }

    auto priority = NodePriorityType::MAIN_PRIORITY;
    if (!data.read(reinterpret_cast<char*>(&priority), sizeof(priority))) {
        return "UnmarshalNode: Cannot read priority";
    }

    bool isOnTree = false;
    if (!data.read(reinterpret_cast<char*>(&isOnTree), sizeof(isOnTree))) {
        return "UnmarshalNode: Cannot read isOnTree";
    }

    uint8_t nodeGroupType = 0;
    if ((fileVersion >= RSFILE_VERSION_RENDER_METRICS_ADDED) &&
        !data.read(reinterpret_cast<char*>(&nodeGroupType), sizeof(nodeGroupType))) {
        return "UnmarshalNode: Cannot read nodeGroupType";
    }

    bool repaintBoundary = false;
    if ((fileVersion >= RSFILE_VERSION_ISREPAINT_BOUNDARY) &&
        !data.read(reinterpret_cast<char*>(&repaintBoundary), sizeof(repaintBoundary))) {
        return "UnmarshalNode: Cannot read isRepaintBoundary";
    }

    if (const auto node = context.GetMutableNodeMap().GetRenderNode(nodeId)) {
        node->GetMutableRenderProperties().SetPositionZ(positionZ);
        node->GetMutableRenderProperties().SetPivotZ(pivotZ);
        node->nodeGroupType_ = nodeGroupType;
#ifdef SUBTREE_PARALLEL_ENABLE
        node->MarkRepaintBoundary(repaintBoundary);
#endif
        return UnmarshalNodeModifiers(*node, data, fileVersion);
    }
    return "";
}

static std::shared_ptr<ModifierNG::RSRenderModifier> UnmarshalRenderModifier(
    std::stringstream& data, std::string& error)
{
    struct ParcelGuard final {
        Parcel& parcel;
        ~ParcelGuard()
        {
            parcel.~Parcel();
        }
    };

    constexpr size_t maxSize = 50 * 1024u * 1024u;
    size_t size = 0u;
    if (!data.read(reinterpret_cast<char*>(&size), sizeof(size)) || (size > maxSize)) {
        error = "UnmarshalRenderModifier: Invalid buffer size";
        return nullptr;
    }

    std::vector<char> buffer(size);
    if (!data.read(buffer.data(), static_cast<std::streamsize>(buffer.size()))) {
        error = "UnmarshalRenderModifier: Cannot read buffer";
        return nullptr;
    }

    uint8_t parcelMemory[sizeof(Parcel) + 1];
    auto parcel = new (parcelMemory + 1) Parcel;
    const ParcelGuard guard { *parcel };
    if ((parcel->GetMaxCapacity() < GetParcelMaxCapacity()) && !parcel->SetMaxCapacity(GetParcelMaxCapacity())) {
        error = "UnmarshalRenderModifier: Cannot set parcel capacity";
        return nullptr;
    }

    if (!parcel->WriteBuffer(buffer.data(), buffer.size())) {
        error = "UnmarshalRenderModifier: Cannot write parcel";
        return nullptr;
    }

    const auto useVersion = (parcel->ReadInt32() == -1);
    if (useVersion && !RSMarshallingHelper::UnmarshallingTransactionVer(*parcel)) {
        error = "UnmarshalRenderModifier: Cannot unmarshal transaction version";
        return nullptr;
    }

    if (!useVersion && !parcel->RewindRead(0)) {
        error = "UnmarshalRenderModifier: Cannot rewind parcel";
        return nullptr;
    }

    auto modifier = ModifierNG::RSRenderModifier::Unmarshalling(*parcel, false);
    if (!modifier) {
        constexpr size_t minSize = 2;
        if (buffer.size() >= minSize) {
            const auto type = *(reinterpret_cast<ModifierNG::RSModifierType*>(&buffer[0]));
            error = "UnmarshalRenderModifier: Type: " + ModifierNG::RSModifierTypeString::GetModifierTypeString(type);
        } else {
            error = "UnmarshalRenderModifier: Invalid data";
        }
    }
    return modifier;
}

static void SetupCanvasDrawingRenderNode(RSRenderNode& node)
{
    if (!node.IsInstanceOf<RSCanvasDrawingRenderNode>()) {
        return;
    }

    int32_t width = 0;
    int32_t height = 0;
    for (const auto& modifier : node.GetModifiersNG(ModifierNG::RSModifierType::CONTENT_STYLE)) {
        const auto cmdList = modifier ? modifier->GetPropertySimpleDrawCmdList() : nullptr;
        if (cmdList) {
            width = std::max(width, cmdList->GetWidth());
            height = std::max(height, cmdList->GetHeight());
        }
    }

    if ((width > 0) && (height > 0)) {
        static_cast<RSCanvasDrawingRenderNode&>(node).ResetSurface(width, height, 0);
    }
}

std::string RSProfiler::UnmarshalNodeModifiers(RSRenderNode& node, std::stringstream& data, uint32_t fileVersion)
{
    if (!data.read(reinterpret_cast<char*>(&node.instanceRootNodeId_), sizeof(node.instanceRootNodeId_))) {
        return "UnmarshalNodeModifiers: Cannot read instance root node id";
    }
    node.instanceRootNodeId_ = Utils::PatchNodeId(node.instanceRootNodeId_);

    if (!data.read(reinterpret_cast<char*>(&node.firstLevelNodeId_), sizeof(node.firstLevelNodeId_))) {
        return "UnmarshalNodeModifiers: Cannot read first level node id";
    }
    node.firstLevelNodeId_ = Utils::PatchNodeId(node.firstLevelNodeId_);

    const auto discard =
        (node.GetType() == RSRenderNodeType::LOGICAL_DISPLAY_NODE) || (node.GetType() == RSRenderNodeType::SCREEN_NODE);

    constexpr int32_t maxCount = 10000u;
    int32_t count = 0;
    if (!data.read(reinterpret_cast<char*>(&count), sizeof(count)) || (count < 0) || (count > maxCount)) {
        return "UnmarshalNodeModifiers: Modifier count out of range";
    }

    for (int32_t i = 0; i < count; i++) {
        std::string error;
        const auto modifier = UnmarshalRenderModifier(data, error);
        if (modifier && !discard) {
            modifier->ConvertDrawCmdListToSimple();
            node.AddModifier(modifier);
        }
        if (!modifier) {
            SendMessageBase("%s", error.data());
        }
    }

    SetupCanvasDrawingRenderNode(node);
    node.ApplyModifiers();
    return "";
}

std::string RSProfiler::UnmarshalTree(RSContext& context, std::stringstream& data, uint32_t fileVersion, uint32_t depth)
{
    constexpr uint32_t maxDepth = 1024u;
    if (depth >= maxDepth) {
        return "UnmarshalTree: Max depth exceeded";
    }

    NodeId id = 0;
    if (!data.read(reinterpret_cast<char*>(&id), sizeof(id))) {
        return "UnmarshalTree: Cannot read node id";
    }

    constexpr uint32_t maxCount = 1'000'000u;
    uint32_t count = 0u;
    if (!data.read(reinterpret_cast<char*>(&count), sizeof(count)) || (count > maxCount)) {
        return "UnmarshalTree: Node count out of range";
    }

    const auto node = context.GetNodeMap().GetRenderNode(Utils::PatchNodeId(id));
    if (!node) {
        return "UnmarshalTree: Invalid node id: " + std::to_string(id);
    }

    for (uint32_t i = 0; i < count; i++) {
        if (!data.read(reinterpret_cast<char*>(&id), sizeof(id))) {
            return "UnmarshalTree: Cannot read child id";
        }
        node->AddChild(context.GetNodeMap().GetRenderNode(Utils::PatchNodeId(id)), static_cast<int32_t>(i));
        auto error = UnmarshalTree(context, data, fileVersion, depth + 1);
        if (!error.empty()) {
            return error;
        }
    }
    return "";
}

std::string RSProfiler::DumpRenderProperties(const RSRenderNode& node)
{
    return node.renderProperties_.Dump();
}

std::string RSProfiler::DumpModifiers(const RSRenderNode& node)
{
    std::string out;
    out += "<";
    for (uint16_t type = 0; type < ModifierNG::MODIFIER_TYPE_COUNT; type++) {
        const auto& slot = node.GetModifiersNG(static_cast<ModifierNG::RSModifierType>(type));
        if (slot.empty()) {
            continue;
        }
        if (!slot[0]->IsCustom()) {
            continue;
        }
        out += "(";
        out += std::to_string(type);
        out += ", ";
        for (auto& modifier : slot) {
            out += "[";
            const auto modifierId = modifier->GetId();
            out += std::to_string(Utils::ExtractPid(modifierId));
            out += "|";
            out += std::to_string(Utils::ExtractNodeId(modifierId));
            out += " type=";
            out += std::to_string(type);
            out += " [modifier dump is not implemented yet]";
            out += "]";
        }
        out += ")";
    }
    out += ">";
    return out;
}

std::string RSProfiler::DumpSurfaceNode(const RSRenderNode& node)
{
    if (node.GetType() != RSRenderNodeType::SURFACE_NODE) {
        return "";
    }

    std::string out;
    const auto& surfaceNode = (static_cast<const RSSurfaceRenderNode&>(node));
    const auto parent = node.parent_.lock();
    out += ", Parent [" + (parent ? std::to_string(parent->GetId()) : "null") + "]";
    out += ", Name [" + surfaceNode.GetName() + "]";
    if (surfaceNode.GetRSSurfaceHandler()) {
        out += ", hasConsumer: " + std::to_string(surfaceNode.GetRSSurfaceHandler()->HasConsumer());
    }
    std::string contextAlpha = std::to_string(surfaceNode.contextAlpha_);
    std::string propertyAlpha = std::to_string(surfaceNode.GetRenderProperties().GetAlpha());
    out += ", Alpha: " + propertyAlpha + " (include ContextAlpha: " + contextAlpha + ")";
    out += ", Visible: " + std::to_string(surfaceNode.GetRenderProperties().GetVisible());
    out += ", " + surfaceNode.GetVisibleRegion().GetRegionInfo();
    out += ", OcclusionBg: " + std::to_string(surfaceNode.GetAbilityBgAlpha());
    out += ", Properties: " + node.GetRenderProperties().Dump();
    return out;
}

// RSAnimationManager
void RSProfiler::FilterAnimationForPlayback(std::shared_ptr<RSAnimationManager> manager)
{
    if (manager == nullptr) {
        return;
    }
    EraseIf(manager->animations_, [](const auto& pair) -> bool {
        if (!Utils::IsNodeIdPatched(pair.first)) {
            return false;
        }
        if (!pair.second) {
            return true;
        }
        pair.second->Finish();
        pair.second->Detach();
        return true;
    });
}

void RSProfiler::SetReplayStartTimeNano(uint64_t replayStartTimeNano)
{
    g_replayStartTimeNano = static_cast<int64_t>(replayStartTimeNano);
}

uint64_t RSProfiler::GetReplayStartTimeNano()
{
    return g_replayStartTimeNano;
}

void RSProfiler::SetTransactionTimeCorrection(double recordStartTime)
{
    g_transactionTimeCorrection = static_cast<int64_t>(g_replayStartTimeNano) -
        static_cast<int64_t>(Utils::ToNanoseconds(recordStartTime));
}

std::string RSProfiler::GetParcelCommandList()
{
    const std::lock_guard<std::mutex> guard(g_mutexCommandOffsets);
    if (g_parcelNumber2Offset.size()) {
        const auto it = g_parcelNumber2Offset.begin();
        std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
        stream.write(reinterpret_cast<const char*>(&it->first), sizeof(it->first));
        stream.write(reinterpret_cast<const char*>(it->second.data()), it->second.size() * sizeof(uint32_t));
        g_parcelNumber2Offset.erase(it);
        return stream.str();
    }
    return "";
}

void RSProfiler::TransactionUnmarshallingEnd(const Parcel& parcel, uint32_t parcelNumber)
{
    g_counterParseTransactionDataEnd++;
}

void RSProfiler::PushOffset(std::vector<uint32_t>& commandOffsets, uint32_t offset)
{
    if (!IsEnabled()) {
        return;
    }
    if (IsWriteMode()) {
        commandOffsets.push_back(offset);
    }
}

void RSProfiler::TransactionUnmarshallingStart(const Parcel& parcel, uint32_t parcelNumber)
{
    g_counterParseTransactionDataStart++;
    if (!IsEnabled()) {
        return;
    }
    if (IsWriteMode()) {
        std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
        stream.write(reinterpret_cast<const char*>(&parcelNumber), sizeof(parcelNumber));
        SendRSLogBase(RSProfilerLogType::PARCEL_UNMARSHALLING_START, stream.str());
    }
}

void RSProfiler::PushOffsets(const Parcel& parcel, uint32_t parcelNumber, std::vector<uint32_t>& commandOffsets)
{
    if (!IsEnabled()) {
        return;
    }
    if (!parcelNumber) {
        return;
    }
    if (IsWriteMode()) {
        const std::lock_guard<std::mutex> guard(g_mutexCommandOffsets);
        g_parcelNumber2Offset[parcelNumber] = commandOffsets;

        std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
        stream.write(reinterpret_cast<const char*>(&parcelNumber), sizeof(parcelNumber));
        stream.write(reinterpret_cast<const char*>(commandOffsets.data()), commandOffsets.size() * sizeof(uint32_t));
        SendRSLogBase(RSProfilerLogType::PARCEL_UNMARSHALLING_END, stream.str());
    }
}

void RSProfiler::PatchCommand(const Parcel& parcel, RSCommand* command)
{
    if (!IsEnabled()) {
        return;
    }
    if (command == nullptr) {
        return;
    }

    if (command && IsParcelMock(parcel)) {
        command->Patch(Utils::PatchNodeId);
    }
    
    if (IsWriteMode()) {
        g_commandCount++;
        MarshallingTouch(command->GetNodeId());
    }
}

void RSProfiler::MarshallingTouch(NodeId nodeId)
{
    auto job = g_marshallingJob;
    if (job && job->GetUnfinishedCount() && job->marshallingTick) {
        job->marshallingTick(nodeId, false);
    }
}

void RSProfiler::ExecuteCommand(const RSCommand* command)
{
    if (!IsEnabled()) {
        return;
    }
    if (!IsWriteMode() && !IsReadMode()) {
        return;
    }
    if (command == nullptr) {
        return;
    }

    g_commandExecuteCount++;
    if (IsWriteMode()) {
        MarshallingTouch(command->GetNodeId());
    }
}

uint32_t RSProfiler::CalcNodeCmdListCount(RSRenderNode& node)
{
    uint32_t nodeCmdListCount = 0;
    for (uint16_t type = 0; type < ModifierNG::MODIFIER_TYPE_COUNT; type++) {
        const auto& slot = node.GetModifiersNG(static_cast<ModifierNG::RSModifierType>(type));
        if (slot.empty()) {
            continue;
        }
        if (!slot[0]->IsCustom()) {
            continue;
        }
        for (auto& modifier : slot) {
            std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyPtr = nullptr;
            if (modifier != nullptr) {
                auto baseProperty = modifier->GetProperty(
                    ModifierNG::ModifierTypeConvertor::GetPropertyType(modifier->GetType()));
                propertyPtr = baseProperty ?
                    baseProperty->CastToPropertyOf<Drawing::DrawCmdListPtr>(__func__) : nullptr;
            }
            auto propertyValue = propertyPtr ? propertyPtr->Get() : nullptr;
            if (propertyValue && propertyValue->GetOpItemSize() > 0) {
                nodeCmdListCount = 1;
            }
        }
    }
    return nodeCmdListCount;
}

void RSProfiler::MarshalDrawingImage(std::shared_ptr<Drawing::Image>& image,
    std::shared_ptr<Drawing::Data>& compressData)
{
    if (IsEnabled() && !IsSharedMemoryEnabled()) {
        image = nullptr;
        compressData = nullptr;
    }
}

void RSProfiler::EnableBetaRecord()
{
    static constexpr uint32_t recordingMode = 1u;
    if (RSSystemProperties::GetBetaRecordingMode() != recordingMode) {
        RSSystemProperties::SetBetaRecordingMode(recordingMode);
    }
}

bool RSProfiler::IsBetaRecordSavingTriggered()
{
    constexpr uint32_t savingMode = 2u;
    return RSSystemProperties::GetBetaRecordingMode() == savingMode;
}

bool RSProfiler::IsBetaRecordEnabledWithMetrics()
{
    constexpr uint32_t metricsMode = 3u;
    return RSSystemProperties::GetBetaRecordingMode() == metricsMode;
}

void RSProfiler::SetDrawingCanvasNodeRedraw(bool enable)
{
    dcnRedraw_ = enable && IsEnabled();
}

void RSProfiler::DrawingNodeAddClearOp(const SimpleDrawCmdListPtr& drawCmdList)
{
    if (dcnRedraw_ || !drawCmdList) {
        return;
    }
    drawCmdList->ClearOp();
}

void RSProfiler::SetRenderNodeKeepDrawCmd(bool enable)
{
    renderNodeKeepDrawCmdList_ = enable && IsEnabled();
}

void RSProfiler::KeepDrawCmd(bool& drawCmdListNeedSync)
{
    drawCmdListNeedSync = !renderNodeKeepDrawCmdList_;
}

static uint64_t NewAshmemDataCacheId()
{
    static std::atomic_uint32_t id = 0u;
    return Utils::ComposeDataId(Utils::GetPid(), id++);
}

static void CacheAshmemData(uint64_t id, const uint8_t* data, size_t size)
{
    if (RSProfiler::IsWriteMode() && data && (size > 0)) {
        Image ashmem;
        ashmem.data.insert(ashmem.data.end(), data, data + size);
        ImageCache::Add(id, std::move(ashmem));
    }
}

static const uint8_t* GetCachedAshmemData(uint64_t id, size_t size)
{
    if (!RSProfiler::IsReadMode()) {
        return nullptr;
    }
    // ImageCache::Get returns a raw pointer into the cache map; another thread
    // mutating the cache (Reset/Deserialize) would invalidate it. Snapshot the
    // data under the cache's lock and keep it alive via a thread-local buffer
    // so the caller can use the returned pointer without races.
    thread_local std::vector<uint8_t> ashmem;
    auto copy = ImageCache::Copy(id);
    if (copy.data.size() != size) {
        return nullptr;
    }
    ashmem = std::move(copy.data);
    return ashmem.data();
}

void RSProfiler::WriteParcelData(Parcel& parcel)
{
    bool isClientEnabled = RSSystemProperties::GetProfilerEnabled();
    if (!parcel.WriteBool(isClientEnabled)) {
        HRPE("Unable to write is_client_enabled");
        return;
    }

    if (!isClientEnabled) {
        return;
    }

    if (!parcel.WriteUint64(NewAshmemDataCacheId())) {
        HRPE("Unable to write NewAshmemDataCacheId failed");
        return;
    }
}

const void* RSProfiler::ReadParcelData(Parcel& parcel, size_t size, bool& isMalloc)
{
    bool isClientEnabled = false;
    if (!parcel.ReadBool(isClientEnabled)) {
        HRPE("ReadParcelData: Cannot read isClientEnabled");
        return nullptr;
    }

    if (!isClientEnabled) {
        return RSMarshallingHelper::ReadFromAshmem(parcel, size, isMalloc);
    }

    uint64_t id = 0u;
    if (!parcel.ReadUint64(id)) {
        HRPE("ReadParcelData: Cannot read id");
        return nullptr;
    }

    if (auto data = GetCachedAshmemData(id, size)) {
        constexpr uint32_t skipBytes = 24u;
        parcel.SkipBytes(skipBytes);
        isMalloc = false;
        return data;
    }

    auto data = RSMarshallingHelper::ReadFromAshmem(parcel, size, isMalloc);
    CacheAshmemData(id, reinterpret_cast<const uint8_t*>(data), size);
    return data;
}

bool RSProfiler::SkipParcelData(Parcel& parcel, size_t size)
{
    bool isClientEnabled = false;
    if (!parcel.ReadBool(isClientEnabled)) {
        HRPE("SkipParcelData: Cannot read isClientEnabled");
        return false;
    }

    if (!isClientEnabled) {
        return false;
    }

    uint64_t id = 0u;
    if (!parcel.ReadUint64(id)) {
        HRPE("SkipParcelData: Cannot read id");
        return false;
    }

    if (IsReadMode()) {
        constexpr uint32_t skipBytes = 24u;
        parcel.SkipBytes(skipBytes);
        return true;
    }

    return false;
}

bool IsTypefaceVariation(const Drawing::SharedTypeface& typeface)
{
    // See RSClientToServiceConnection::RegisterTypeface
    return typeface.originId_ > 0;
}

void CacheSharedTypeface(uint64_t id, const Drawing::SharedTypeface& typeface)
{
    if (!RSProfiler::IsWriteMode() || (typeface.fd_ == INVALID_FD) || (typeface.size_ <= 0) ||
        IsTypefaceVariation(typeface)) {
        return;
    }

    const auto data = ::mmap(nullptr, typeface.size_, PROT_READ, MAP_SHARED, typeface.fd_, 0);
    if (data && (data != MAP_FAILED)) { // NOLINT
        CacheAshmemData(id, reinterpret_cast<const uint8_t*>(data), typeface.size_);
        ::munmap(data, typeface.size_);
    } else {
        HRPE("CacheSharedTypeface: Cannot cache typeface data");
    }
}

bool FetchSharedTypeface(uint64_t id, Drawing::SharedTypeface& typeface)
{
    constexpr int64_t maxSize = 100 * 1024 * 1024u;
    if (!RSProfiler::IsReadMode() || (typeface.size_ <= 0u) || (typeface.size_ > maxSize)) {
        return false;
    }

    const auto file = AshmemCreate("HRPSharedTypeface", typeface.size_);
    if ((file == INVALID_FD) || (AshmemSetProt(file, PROT_READ | PROT_WRITE) != EOK)) {
        ::close(file);
        HRPE("FetchSharedTypeface: Cannot create typeface file");
        return false;
    }

    if (IsTypefaceVariation(typeface)) {
        typeface.fd_ = file;
        return true;
    }

    const auto cached = GetCachedAshmemData(id, typeface.size_);
    if (!cached) {
        ::close(file);
        HRPE("FetchSharedTypeface: Cannot get cached data");
        return false;
    }

    const auto data = ::mmap(nullptr, typeface.size_, PROT_WRITE, MAP_SHARED, file, 0);
    if (data && (data != MAP_FAILED)) { // NOLINT
        if (::memcpy_s(data, typeface.size_, cached, typeface.size_) == EOK) {
            ::munmap(data, typeface.size_);
            typeface.fd_ = file;
            return true;
        }
        ::munmap(data, typeface.size_);
    }

    ::close(file);
    HRPE("FetchSharedTypeface: Copy failed");
    return false;
}

void RSProfiler::WriteSharedTypeface(Parcel& parcel, const Drawing::SharedTypeface& typeface)
{
    const auto profilerEnabled = RSSystemProperties::GetProfilerEnabled();
    if (!parcel.WriteBool(profilerEnabled)) {
        HRPE("WriteSharedTypeface: Cannot write profilerEnabled");
        return;
    }

    if (!profilerEnabled) {
        return;
    }

    if (!parcel.WriteUint64(NewAshmemDataCacheId())) {
        HRPE("WriteSharedTypeface: Cannot write cache id");
    }
}

void RSProfiler::ReadSharedTypeface(Parcel& parcel, Drawing::SharedTypeface& typeface)
{
    bool profilerEnabled = false;
    if (!parcel.ReadBool(profilerEnabled)) {
        HRPE("ReadSharedTypeface: Cannot read profilerEnabled");
        return;
    }

    if (!profilerEnabled) {
        typeface.fd_ = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
        return;
    }

    uint64_t id = 0u;
    if (!parcel.ReadUint64(id)) {
        HRPE("ReadSharedTypeface: Cannot read id");
        return;
    }

    if (FetchSharedTypeface(id, typeface)) {
        constexpr uint32_t skipBytes = 24u;
        parcel.SkipBytes(skipBytes);
        return;
    }

    typeface.fd_ = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    CacheSharedTypeface(id, typeface);
}

uint32_t RSProfiler::GetNodeDepth(const std::shared_ptr<RSRenderNode> node)
{
    uint32_t depth = 0;
    for (auto curNode = node; curNode != nullptr; depth++) {
        curNode = curNode ? curNode->GetParent().lock() : nullptr;
    }
    return depth;
}

std::string RSProfiler::ReceiveMessageBase()
{
    const std::lock_guard<std::mutex> guard(g_msgBaseMutex);
    if (g_msgBaseList.empty()) {
        return "";
    }
    std::string value = g_msgBaseList.front();
    g_msgBaseList.pop();
    return value;
}

void RSProfiler::SendMessageBase(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    const auto message = Utils::Format(format, args);
    va_end(args);

    if (!message.empty()) {
        const std::lock_guard<std::mutex> guard(g_msgBaseMutex);
        g_msgBaseList.push(message);
    }
}

std::unordered_map<AnimationId, std::vector<int64_t>>& RSProfiler::AnimeGetStartTimes()
{
    return g_animeStartMap;
}

void RSProfiler::ReplayFixTrIndex(uint64_t curIndex, uint64_t& lastIndex)
{
    if (!IsEnabled()) {
        return;
    }
    if (IsReadMode()) {
        if (lastIndex == 0) {
            lastIndex = curIndex - 1;
        }
    }
}

int64_t RSProfiler::AnimeSetStartTime(AnimationId id, int64_t nanoTime)
{
    if (!IsEnabled()) {
        return nanoTime;
    }

    if (IsReadMode()) {
        if (!g_animeStartMap.count(id)) {
            return nanoTime;
        }
        int64_t minDt = INT64_MAX;
        int64_t minTime = nanoTime - g_replayStartTimeNano;
        for (const auto recordedTime : g_animeStartMap[id]) {
            int64_t dt = abs(recordedTime - (nanoTime - g_replayStartTimeNano));
            if (dt < minDt) {
                minDt = dt;
                minTime = recordedTime;
            }
        }
        return minTime + g_replayStartTimeNano;
    } else if (IsWriteMode()) {
        if (g_animeStartMap.count(id)) {
            g_animeStartMap[Utils::PatchNodeId(id)].push_back(nanoTime);
        } else {
            std::vector<int64_t> list;
            list.push_back(nanoTime);
            g_animeStartMap.insert({ Utils::PatchNodeId(id), list });
        }
    }

    return nanoTime;
}

bool RSProfiler::ProcessAddChild(RSRenderNode* parent, RSRenderNode::SharedPtr child, int index)
{
    if (!parent || !child || !IsEnabled()) {
        return false;
    }
    if (!IsReadMode()) {
        return false;
    }

    // Disable lock screen during playback
    if (!Utils::IsNodeIdPatched(child->GetId())) {
        const auto display = displayChildren_.find(parent->ReinterpretCastTo<RSLogicalDisplayRenderNode>());
        if (display != displayChildren_.end()) {
            display->second.push_back(child);
            return true;
        }
    }
    return false;
}

void RSProfiler::RequestRecordAbort()
{
    recordAbortRequested_ = true;
}

bool RSProfiler::IsRecordAbortRequested()
{
    return recordAbortRequested_;
}

bool RSProfiler::BaseSetPlaybackSpeed(double speed)
{
    float invSpeed = 1.0f;
    if (speed <= .0f || std::isnan(speed)) {
        return false;
    } else {
        invSpeed /= speed > 0.0f ? speed : 1.0f;
    }

    if (IsReadMode()) {
        if (Utils::Now() >= g_pauseAfterTime && g_pauseAfterTime > 0) {
            // paused can change speed but need adjust start time then
            int64_t curTime = static_cast<int64_t>(g_pauseAfterTime) - g_pauseCumulativeTime - g_replayStartTimeNano;
            g_pauseCumulativeTime = static_cast<int64_t>(g_pauseAfterTime) - g_replayStartTimeNano -
                    curTime * g_replaySpeed * invSpeed;
            g_replaySpeed = speed;
            return true;
        }
        // change of speed when replay in progress is not possible
        return false;
    }
    g_replaySpeed = speed;
    return true;
}

double RSProfiler::BaseGetPlaybackSpeed()
{
    return g_replaySpeed;
}

void RSProfiler::MarshalSubTreeLo(
    RSContext& context, std::stringstream& data, const RSRenderNode& node, uint32_t fileVersion, uint32_t depth)
{
    constexpr uint32_t maxDepth = 1024u;
    if (depth >= maxDepth) {
        return;
    }

    const NodeId nodeId = node.GetId();
    data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));

    MarshalNode(node, data, fileVersion);

    const auto children = GetChildren(node);
    const uint32_t count = children.size();
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (const auto& child : children) {
        MarshalSubTreeLo(context, data, *child, fileVersion, depth + 1u);
    }
}

std::string RSProfiler::UnmarshalSubTreeLo(
    RSContext& context, std::stringstream& data, RSRenderNode& attachNode, uint32_t fileVersion, uint32_t depth)
{
    constexpr uint32_t maxDepth = 1024u;
    if (depth >= maxDepth) {
        return "UnmarshalSubTreeLo: Max depth exceeded";
    }

    NodeId id = 0;
    if (!data.read(reinterpret_cast<char*>(&id), sizeof(id))) {
        return "UnmarshalSubTreeLo: Cannot read node id";
    }

    auto error = UnmarshalNode(context, data, fileVersion);
    if (!error.empty()) {
        return error;
    }

    const auto node = context.GetMutableNodeMap().GetRenderNode(Utils::PatchNodeId(id));
    if (!node) {
        return "UnmarshalSubTreeLo: Invalid node id: " + std::to_string(id);
    }

    attachNode.AddChild(node);

    constexpr uint32_t maxCount = 1'000'000u;
    uint32_t count = 0u;
    if (!data.read(reinterpret_cast<char*>(&count), sizeof(count)) || count > maxCount) {
        return "UnmarshalSubTreeLo: Node count out of range";
    }
    for (uint32_t i = 0; i < count; i++) {
        error = UnmarshalSubTreeLo(context, data, *node, fileVersion, depth + 1u);
        if (!error.empty()) {
            return error;
        }
    }
    return error;
}

TextureRecordType RSProfiler::GetTextureRecordType()
{
    if (IsBetaRecordEnabled() || g_marshalFirstFrameThread) {
        return TextureRecordType::ONE_PIXEL;
    }
    return g_textureRecordType;
}

void RSProfiler::SetTextureRecordType(TextureRecordType type)
{
    g_textureRecordType = type;
}

bool RSProfiler::IfNeedToSkipDuringReplay(Parcel& parcel, uint32_t skipBytes)
{
    if (!IsEnabled()) {
        return false;
    }
    if (!IsParcelMock(parcel)) {
        return false;
    }
    if (IsReadEmulationMode() || IsReadMode()) {
        parcel.SkipBytes(skipBytes);
        return true;
    }
    return false;
}

bool RSProfiler::IsFirstFrameParcel(const Parcel& parcel)
{
    if (!IsEnabled()) {
        return false;
    }
    if (!IsBetaRecordEnabled()) {
        return false;
    }
    return IsWriteEmulationMode() || IsReadEmulationMode();
}

std::shared_ptr<ProfilerMarshallingJob>& RSProfiler::GetMarshallingJob()
{
    return g_marshallingJob;
}

void RSProfiler::SetMarshalFirstFrameThreadFlag(bool flag)
{
    g_marshalFirstFrameThread = flag;
}

bool RSProfiler::GetMarshalFirstFrameThreadFlag()
{
    return g_marshalFirstFrameThread;
}

uint64_t RSProfiler::GetParseTransactionDataStartCounter()
{
    return g_counterParseTransactionDataStart;
}

uint64_t RSProfiler::GetParseTransactionDataEndCounter()
{
    return g_counterParseTransactionDataEnd;
}

void RSProfiler::MetricRenderNodeInc(bool isOnTree)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }
    if (isOnTree) {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_ONTREE_NODE_COUNT, 1);
    } else {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_OFFTREE_NODE_COUNT, 1);
    }
}
 
void RSProfiler::MetricRenderNodeDec(bool isOnTree)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }
    if (isOnTree) {
        GetCustomMetrics().SubInt(RSPROFILER_METRIC_ONTREE_NODE_COUNT, 1);
    } else {
        GetCustomMetrics().SubInt(RSPROFILER_METRIC_OFFTREE_NODE_COUNT, 1);
    }
}
 
void RSProfiler::MetricRenderNodeChange(bool isOnTree)
{
    if (!IsEnabled() || !IsWriteMode()) {
        return;
    }
    if (isOnTree) {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_ONTREE_NODE_COUNT, 1);
        GetCustomMetrics().SubInt(RSPROFILER_METRIC_OFFTREE_NODE_COUNT, 1);
    } else {
        GetCustomMetrics().AddInt(RSPROFILER_METRIC_OFFTREE_NODE_COUNT, 1);
        GetCustomMetrics().SubInt(RSPROFILER_METRIC_ONTREE_NODE_COUNT, 1);
    }
}

void RSProfiler::MetricRenderNodeInit(RSContext* context)
{
    if (!context) {
        return;
    }
    GetCustomMetrics().SetZero(RSPROFILER_METRIC_ONTREE_NODE_COUNT);
    GetCustomMetrics().SetZero(RSPROFILER_METRIC_OFFTREE_NODE_COUNT);
}
void RSProfiler::RSLogOutput(RSProfilerLogType type, const char* format, va_list argptr)
{
    if (!IsEnabled() || !(IsWriteMode() || IsReadEmulationMode())) {
        return;
    }
 
    // no access to vsnprintf_s_p in inner api of hilog - have to write naive code myself
    constexpr int maxSize = 1024;
    char format2[maxSize] = {0}; // zero ending always present
    const char* ptr1 = format;
    char* ptr2 = format2;
    const auto effectiveSize = maxSize - 3; // max 3 chars can be added in one iteration
    constexpr int publicLen = 8; // publicLen = strlen("{public}");
    for (; *ptr1 && ptr2 - format2 < effectiveSize && ptr1 - format < effectiveSize - publicLen;) {
        if (*ptr1 == '%') {
            if (*(ptr1 + 1) == '%') {
                // %% translates to %%
                *ptr2++ = *ptr1++;
                *ptr2++ = *ptr1++;
            } else if (!memcmp(ptr1 + 1, "{public}", publicLen)) {
                // %{public} translates to %
                *ptr2++ = *ptr1++;
                ptr1 += publicLen;
            } else {
                // abcd%{private} translates to abcd{private} - all subsequent vars are skipped
                strcat_s(ptr2, effectiveSize - (ptr2 - format2), "{private}");
                ptr2 = format2 + strlen(format2);
                break;
            }
        } else {
            // other symbols are just copied
            *ptr2++ = *ptr1++;
        }
    }
    *ptr2++ = 0;
 
    char outStr[maxSize] = {0};
    if (vsprintf_s(outStr, sizeof(outStr), format2, argptr) > 0) {
        SendRSLogBase(type, std::string(outStr));
    }
}
 
RSProfilerLogMsg RSProfiler::ReceiveRSLogBase()
{
    const std::lock_guard<std::mutex> guard(g_rsLogListMutex);
    if (g_rsLogList.empty()) {
        return RSProfilerLogMsg();
    }
    auto value = g_rsLogList.front();
    g_rsLogList.pop();
    return value;
}
 
void RSProfiler::SendRSLogBase(RSProfilerLogType type, const std::string& msg)
{
    if (IsReadEmulationMode()) {
        if (type == RSProfilerLogType::WARNING) {
            SendMessageBase("RS_LOGW: %s", msg.data());
        } else if (type == RSProfilerLogType::ERROR) {
            SendMessageBase("RS_LOGE: %s", msg.data());
        }
    } else {
        const std::lock_guard<std::mutex> guard(g_rsLogListMutex);
        g_rsLogList.push(RSProfilerLogMsg(type, Utils::Now(), msg));
    }
}
 
void RSProfiler::ResetCustomMetrics()
{
    RSProfilerCustomMetrics& customMetrics = GetCustomMetrics();
    customMetrics.Reset();
}
 
RSProfilerCustomMetrics& RSProfiler::GetCustomMetrics()
{
    static RSProfilerCustomMetrics s_customMetrics;
    return s_customMetrics;
}
 
bool RSProfiler::IsRecordingMode()
{
    return IsEnabled() && IsWriteMode();
}
} // namespace OHOS::Rosen

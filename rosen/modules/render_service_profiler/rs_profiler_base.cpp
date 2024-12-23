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
#include <iostream>
#include <memory>
#include <string>
#include <sys/mman.h>
#include <type_traits>
#include <utility>
#include <vector>

#include "sys_binder.h"
#include "message_parcel.h"
#include "rs_profiler.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_network.h"
#include "rs_profiler_utils.h"
#include "rs_profiler_file.h"
#include "rs_profiler_log.h"

#include "animation/rs_animation_manager.h"
#include "command/rs_base_node_command.h"
#include "command/rs_canvas_drawing_node_command.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_effect_node_command.h"
#include "command/rs_proxy_node_command.h"
#include "command/rs_root_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_ashmem_helper.h"

namespace OHOS::Rosen {

static Mode g_mode = Mode::NONE;
static std::vector<pid_t> g_pids;
static pid_t g_pid = 0;
static NodeId g_parentNode = 0;
static std::atomic<uint32_t> g_commandCount = 0;        // UNMARSHALLING RSCOMMAND COUNT
static std::atomic<uint32_t> g_commandExecuteCount = 0; // EXECUTE RSCOMMAND COUNT
constexpr uint32_t COMMAND_PARSE_LIST_COUNT = 1024;
constexpr uint32_t COMMAND_PARSE_LIST_SIZE = COMMAND_PARSE_LIST_COUNT * 2 + 5;

static std::mutex g_msgBaseMutex;
static std::queue<std::string> g_msgBaseList;

#pragma pack(push, 1)
struct PacketParsedCommandList {
    double packetTime;
    uint32_t packetSize;
    uint16_t cmdCount;
    uint32_t cmdCode[COMMAND_PARSE_LIST_SIZE];
};
#pragma pack(pop)

static thread_local PacketParsedCommandList g_commandParseBuffer;
constexpr uint32_t COMMAND_LOOP_SIZE = 32;
static PacketParsedCommandList g_commandLoop[COMMAND_LOOP_SIZE];
static std::atomic<uint32_t> g_commandLoopIndexStart = 0;
static std::atomic<uint32_t> g_commandLoopIndexEnd = 0;

static uint64_t g_pauseAfterTime = 0;
static uint64_t g_pauseCumulativeTime = 0;
static int64_t g_transactionTimeCorrection = 0;
static int64_t g_replayStartTimeNano = 0.0;

static const size_t PARCEL_MAX_CAPACITY = 234 * 1024 * 1024;

static std::unordered_map<AnimationId, std::vector<int64_t>> g_animeStartMap;

bool RSProfiler::testing_ = false;
bool RSProfiler::enabled_ = RSSystemProperties::GetProfilerEnabled();
bool RSProfiler::betaRecordingEnabled_ = RSSystemProperties::GetBetaRecordingMode() != 0;
int8_t RSProfiler::signalFlagChanged_ = 0;
std::atomic_bool RSProfiler::dcnRedraw_ = false;
std::vector<RSRenderNode::WeakPtr> g_childOfDisplayNodesPostponed;

constexpr size_t GetParcelMaxCapacity()
{
    return PARCEL_MAX_CAPACITY;
}

bool RSProfiler::IsEnabled()
{
    return enabled_ || testing_;
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
    return g_mode == Mode::NONE;
}

bool RSProfiler::IsReadMode()
{
    return g_mode == Mode::READ;
}

bool RSProfiler::IsReadEmulationMode()
{
    return g_mode == Mode::READ_EMUL;
}

bool RSProfiler::IsWriteMode()
{
    return g_mode == Mode::WRITE;
}

bool RSProfiler::IsWriteEmulationMode()
{
    return g_mode == Mode::WRITE_EMUL;
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

std::shared_ptr<MessageParcel> RSProfiler::CopyParcel(const MessageParcel& parcel)
{
    if (!IsEnabled()) {
        return std::make_shared<MessageParcel>();
    }

    if (IsParcelMock(parcel)) {
        auto* buffer = new uint8_t[sizeof(MessageParcel) + 1];
        auto* mpPtr = new (buffer + 1) MessageParcel;
        return std::shared_ptr<MessageParcel>(mpPtr, [](MessageParcel* ptr) {
            ptr->~MessageParcel();
            auto* allocPtr = reinterpret_cast<uint8_t*>(ptr);
            allocPtr--;
            delete allocPtr;
        });
    }

    return std::make_shared<MessageParcel>();
}

NodeId RSProfiler::PatchPlainNodeId(const Parcel& parcel, NodeId id)
{
    if (!IsEnabled()) {
        return id;
    }

    if ((g_mode != Mode::READ && g_mode != Mode::READ_EMUL) || !IsParcelMock(parcel)) {
        return id;
    }

    return Utils::PatchNodeId(id);
}

void RSProfiler::PatchTypefaceId(const Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val)
{
    if (!val || !IsEnabled()) {
        return;
    }

    if (g_mode == Mode::READ_EMUL) {
        val->PatchTypefaceIds();
    } else if (g_mode == Mode::READ) {
        if (IsParcelMock(parcel)) {
            val->PatchTypefaceIds();
        }
    }
}

pid_t RSProfiler::PatchPlainPid(const Parcel& parcel, pid_t pid)
{
    if (!IsEnabled()) {
        return pid;
    }

    if ((g_mode != Mode::READ && g_mode != Mode::READ_EMUL) || !IsParcelMock(parcel)) {
        return pid;
    }

    return Utils::GetMockPid(pid);
}

void RSProfiler::SetMode(Mode mode)
{
    g_mode = mode;
    if (g_mode == Mode::NONE) {
        g_pauseAfterTime = 0;
        g_pauseCumulativeTime = 0;
    }
}

Mode RSProfiler::GetMode()
{
    return g_mode;
}

void RSProfiler::SetSubstitutingPid(const std::vector<pid_t>& pids, pid_t pid, NodeId parent)
{
    g_pids = pids;
    g_pid = pid;
    g_parentNode = parent;
}

NodeId RSProfiler::GetParentNode()
{
    return g_parentNode;
}

const std::vector<pid_t>& RSProfiler::GetPids()
{
    return g_pids;
}

pid_t RSProfiler::GetSubstitutingPid()
{
    return g_pid;
}

uint64_t RSProfiler::PatchTime(uint64_t time)
{
    if (!IsEnabled()) {
        return time;
    }
    if (g_mode != Mode::READ && g_mode != Mode::READ_EMUL) {
        return time;
    }
    if (time == 0.0) {
        return 0.0;
    }
    if (time >= g_pauseAfterTime && g_pauseAfterTime > 0) {
        return g_pauseAfterTime - g_pauseCumulativeTime;
    }
    return time - g_pauseCumulativeTime;
}

uint64_t RSProfiler::PatchTransactionTime(const Parcel& parcel, uint64_t time)
{
    if (!IsEnabled()) {
        return time;
    }

    if (g_mode == Mode::WRITE) {
        g_commandParseBuffer.packetTime = Utils::ToSeconds(time);
        g_commandParseBuffer.packetSize = parcel.GetDataSize();
        uint32_t index = g_commandLoopIndexEnd++;
        index %= COMMAND_LOOP_SIZE;
        g_commandLoop[index] = g_commandParseBuffer;
        g_commandParseBuffer.cmdCount = 0;
    }

    if (g_mode != Mode::READ) {
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

void RSProfiler::TimePauseAt(uint64_t curTime, uint64_t newPauseAfterTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += curTime - g_pauseAfterTime;
        }
    }
    g_pauseAfterTime = newPauseAfterTime;
}

void RSProfiler::TimePauseResume(uint64_t curTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += curTime - g_pauseAfterTime;
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

std::shared_ptr<RSDisplayRenderNode> RSProfiler::GetDisplayNode(const RSContext& context)
{
    const std::shared_ptr<RSBaseRenderNode>& root = context.GetGlobalRootRenderNode();
    // without these checks device might get stuck on startup
    if (!root || (root->GetChildrenCount() != 1)) {
        return nullptr;
    }

    return RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(root->GetSortedChildren()->front());
}

Vector4f RSProfiler::GetScreenRect(const RSContext& context)
{
    std::shared_ptr<RSDisplayRenderNode> node = GetDisplayNode(context);
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
        map.displayNodeMap_, [pid, canBeRemoved](const auto& pair) -> bool { return canBeRemoved(pair.first, pid); });

    if (auto fallbackNode = map.GetAnimationFallbackNode()) {
        fallbackNode->GetAnimationManager().FilterAnimationByPid(pid);
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
        nodeMap.FilterNodeByPid(pid);
    }

    if (auto fallbackNode = nodeMap.GetAnimationFallbackNode()) {
        // remove all fallback animations belong to given pid
        FilterAnimationForPlayback(fallbackNode->GetAnimationManager());
    }
}

void RSProfiler::GetSurfacesTrees(
    const RSContext& context, std::map<std::string, std::tuple<NodeId, std::string>>& list)
{
    constexpr uint32_t treeDumpDepth = 2;

    list.clear();

    const RSRenderNodeMap& map = const_cast<RSContext&>(context).GetMutableNodeMap();
    for (const auto& [_, subMap] : map.renderNodeMap_) {
        for (const auto& [_, node] : subMap) {
            if (node->GetType() == RSRenderNodeType::SURFACE_NODE) {
                std::string tree;
                node->DumpTree(treeDumpDepth, tree);
                const auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
                list.insert({ surfaceNode->GetName(), { surfaceNode->GetId(), tree } });
            }
        }
    }
}

void RSProfiler::GetSurfacesTrees(const RSContext& context, pid_t pid, std::map<NodeId, std::string>& list)
{
    constexpr uint32_t treeDumpDepth = 2;

    list.clear();

    const RSRenderNodeMap& map = const_cast<RSContext&>(context).GetMutableNodeMap();
    for (const auto& [_, subMap] : map.renderNodeMap_) {
        for (const auto& [_, node] : subMap) {
            if (node->GetId() == Utils::GetRootNodeId(pid)) {
                std::string tree;
                node->DumpTree(treeDumpDepth, tree);
                list.insert({ node->GetId(), tree });
            }
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

void RSProfiler::MarshalNodes(const RSContext& context, std::stringstream& data, uint32_t fileVersion)
{
    const auto& map = const_cast<RSContext&>(context).GetMutableNodeMap();
    const uint32_t count = static_cast<uint32_t>(map.GetSize());
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));
    const auto& rootRenderNode = context.GetGlobalRootRenderNode();
    if (rootRenderNode == nullptr) {
        RS_LOGE("RSProfiler::MarshalNodes rootRenderNode is nullptr");
        return;
    }

    std::vector<std::shared_ptr<RSRenderNode>> nodes;
    nodes.emplace_back(rootRenderNode);

    for (const auto& [_, subMap] : map.renderNodeMap_) {
        for (const auto& [_, node] : subMap) {
            if (node != nullptr) {
                MarshalNode(*node, data, fileVersion);
                std::shared_ptr<RSRenderNode> parent = node->GetParent().lock();
                if (!parent && (node != rootRenderNode)) {
                    nodes.emplace_back(node);
                }
            }
        }
    }

    const uint32_t nodeCount = static_cast<uint32_t>(nodes.size());
    data.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));
    for (const auto& node : nodes) { // no nullptr in nodes, omit check
        MarshalTree(*node, data, fileVersion);
    }
}

void RSProfiler::MarshalTree(const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion)
{
    const NodeId nodeId = node.GetId();
    data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));

    const uint32_t count = node.children_.size();
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& child : node.children_) {
        if (auto node = child.lock().get()) {
            const NodeId nodeId = node->GetId();
            data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));
            MarshalTree(*node, data, fileVersion);
        }
    }
}

void RSProfiler::MarshalNode(const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion)
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

    const NodePriorityType priority = const_cast<RSRenderNode&>(node).GetPriority();
    data.write(reinterpret_cast<const char*>(&priority), sizeof(priority));

    const bool isOnTree = node.IsOnTheTree();
    data.write(reinterpret_cast<const char*>(&isOnTree), sizeof(isOnTree));

    if (fileVersion >= RSFILE_VERSION_RENDER_METRICS_ADDED) {
        const uint8_t nodeGroupType = node.nodeGroupType_;
        data.write(reinterpret_cast<const char*>(&nodeGroupType), sizeof(nodeGroupType));
    }

    MarshalNodeModifiers(node, data, fileVersion);
}

static void MarshalRenderModifier(const RSRenderModifier& modifier, std::stringstream& data)
{
    Parcel parcel;
    parcel.SetMaxCapacity(GetParcelMaxCapacity());
    const_cast<RSRenderModifier&>(modifier).Marshalling(parcel);

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

void RSProfiler::MarshalNodeModifiers(const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion)
{
    data.write(reinterpret_cast<const char*>(&node.instanceRootNodeId_), sizeof(node.instanceRootNodeId_));
    data.write(reinterpret_cast<const char*>(&node.firstLevelNodeId_), sizeof(node.firstLevelNodeId_));

    const uint32_t modifierCount = node.modifiers_.size();
    data.write(reinterpret_cast<const char*>(&modifierCount), sizeof(modifierCount));

    for (const auto& [id, modifier] : node.modifiers_) {
        if (modifier) {
            MarshalRenderModifier(*modifier, data);
        }
    }

    const uint32_t drawModifierCount = node.renderContent_->drawCmdModifiers_.size();
    data.write(reinterpret_cast<const char*>(&drawModifierCount), sizeof(drawModifierCount));

    for (const auto& [type, modifiers] : node.renderContent_->drawCmdModifiers_) {
        const uint32_t modifierCount = modifiers.size();
        data.write(reinterpret_cast<const char*>(&modifierCount), sizeof(modifierCount));
        for (const auto& modifier : modifiers) {
            if (!modifier) {
                continue;
            }
            if (auto commandList = reinterpret_cast<Drawing::DrawCmdList*>(modifier->GetDrawCmdListId())) {
                std::string allocData = commandList->ProfilerPushAllocators();
                commandList->MarshallingDrawOps();
                MarshalRenderModifier(*modifier, data);
                commandList->ProfilerPopAllocators(allocData);
            } else {
                MarshalRenderModifier(*modifier, data);
            }
        }
    }
}

static std::string CreateRenderSurfaceNode(RSContext& context,
                                           NodeId id,
                                           bool isTextureExportNode,
                                           std::stringstream& data)
{
    constexpr uint32_t nameSizeMax = 4096;
    uint32_t size = 0u;
    data.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (size > nameSizeMax) {
        return "CreateRenderSurfaceNode unmarshalling failed, file is damaged";
    }

    std::string name;
    name.resize(size, ' ');
    data.read(reinterpret_cast<char*>(name.data()), size);

    data.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (size > nameSizeMax) {
        return "CreateRenderSurfaceNode unmarshalling failed, file is damaged";
    }
    std::string bundleName;
    bundleName.resize(size, ' ');
    data.read(reinterpret_cast<char*>(bundleName.data()), size);

    RSSurfaceNodeType nodeType = RSSurfaceNodeType::DEFAULT;
    data.read(reinterpret_cast<char*>(&nodeType), sizeof(nodeType));

    uint8_t backgroundAlpha = 0u;
    data.read(reinterpret_cast<char*>(&backgroundAlpha), sizeof(backgroundAlpha));

    uint8_t globalAlpha = 0u;
    data.read(reinterpret_cast<char*>(&globalAlpha), sizeof(globalAlpha));

    const RSSurfaceRenderNodeConfig config = { .id = id,
        .name = name + "_",
        .nodeType = nodeType,
        .additionalData = nullptr,
        .isTextureExportNode = isTextureExportNode,
        .isSync = false };

    if (auto node = SurfaceNodeCommandHelper::CreateWithConfigInRS(config, context)) {
        context.GetMutableNodeMap().RegisterRenderNode(node);
        node->SetAbilityBGAlpha(backgroundAlpha);
        node->SetGlobalAlpha(globalAlpha);
    }
    return "";
}

std::string RSProfiler::UnmarshalNodes(RSContext& context, std::stringstream& data, uint32_t fileVersion)
{
    std::string errReason;

    uint32_t count = 0;
    data.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (uint32_t i = 0; i < count; i++) {
        errReason = UnmarshalNode(context, data, fileVersion);
        if (errReason.size()) {
            return errReason;
        }
    }

    data.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (uint32_t i = 0; i < count; i++) {
        errReason = UnmarshalTree(context, data, fileVersion);
        if (errReason.size()) {
            return errReason;
        }
    }

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

    return "";
}

std::string RSProfiler::UnmarshalNode(RSContext& context, std::stringstream& data, uint32_t fileVersion)
{
    RSRenderNodeType nodeType = RSRenderNodeType::UNKNOW;
    data.read(reinterpret_cast<char*>(&nodeType), sizeof(nodeType));

    NodeId nodeId = 0;
    data.read(reinterpret_cast<char*>(&nodeId), sizeof(nodeId));
    nodeId = Utils::PatchNodeId(nodeId);

    bool isTextureExportNode = false;
    data.read(reinterpret_cast<char*>(&isTextureExportNode), sizeof(isTextureExportNode));

    if (data.eof()) {
        return "UnmarshalNode failed, file is damaged";
    }

    if (nodeType == RSRenderNodeType::RS_NODE) {
        RootNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::DISPLAY_NODE) {
        RootNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::SURFACE_NODE) {
        std::string errReason = CreateRenderSurfaceNode(context, nodeId, isTextureExportNode, data);
        if (errReason.size()) {
            return errReason;
        }
    } else if (nodeType == RSRenderNodeType::PROXY_NODE) {
        ProxyNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::CANVAS_NODE) {
        RSCanvasNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::EFFECT_NODE) {
        EffectNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::ROOT_NODE) {
        RootNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        RSCanvasDrawingNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else {
        RootNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    }
    
    return UnmarshalNode(context, data, nodeId, fileVersion);
}

std::string RSProfiler::UnmarshalNode(RSContext& context, std::stringstream& data, NodeId nodeId, uint32_t fileVersion)
{
    float positionZ = 0.0f;
    data.read(reinterpret_cast<char*>(&positionZ), sizeof(positionZ));
    float pivotZ = 0.0f;
    data.read(reinterpret_cast<char*>(&pivotZ), sizeof(pivotZ));
    NodePriorityType priority = NodePriorityType::MAIN_PRIORITY;
    data.read(reinterpret_cast<char*>(&priority), sizeof(priority));
    bool isOnTree = false;
    data.read(reinterpret_cast<char*>(&isOnTree), sizeof(isOnTree));

    uint8_t nodeGroupType = 0;
    if (fileVersion >= RSFILE_VERSION_RENDER_METRICS_ADDED) {
        data.read(reinterpret_cast<char*>(&nodeGroupType), sizeof(nodeGroupType));
    }

    if (auto node = context.GetMutableNodeMap().GetRenderNode(nodeId)) {
        node->GetMutableRenderProperties().SetPositionZ(positionZ);
        node->GetMutableRenderProperties().SetPivotZ(pivotZ);
        node->SetPriority(priority);
        node->RSRenderNode::SetIsOnTheTree(isOnTree);
        node->nodeGroupType_ = nodeGroupType;
        return UnmarshalNodeModifiers(*node, data, fileVersion);
    }
    return "";
}

static RSRenderModifier* UnmarshalRenderModifier(std::stringstream& data, std::string& errReason)
{
    errReason = "";

    constexpr size_t bufferSizeMax = 50'000'000;
    size_t bufferSize = 0;
    data.read(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize));
    if (bufferSize > bufferSizeMax) {
        errReason = "UnmarshalRenderModifier failed, file is damaged";
        return nullptr;
    }

    std::vector<uint8_t> buffer;
    buffer.resize(bufferSize);
    data.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    if (data.eof()) {
        errReason = "UnmarshalRenderModifier failed, file is damaged";
        return nullptr;
    }

    uint8_t parcelMemory[sizeof(Parcel) + 1];
    auto* parcel = new (parcelMemory + 1) Parcel;
    parcel->SetMaxCapacity(GetParcelMaxCapacity());
    parcel->WriteBuffer(buffer.data(), buffer.size());

    auto ptr = RSRenderModifier::Unmarshalling(*parcel);
    if (!ptr) {
        constexpr size_t minBufferSize = 2;
        if (buffer.size() >= minBufferSize) {
            const auto typeModifier = *(reinterpret_cast<RSModifierType *>(&buffer[0]));
            errReason = RSModifierTypeString().GetModifierTypeString(typeModifier);
        } else {
            errReason = "RSRenderModifier buffer too short";
        }
        errReason += ", size=" + std::to_string(buffer.size());
    }

    return ptr;
}

std::string RSProfiler::UnmarshalNodeModifiers(RSRenderNode& node, std::stringstream& data, uint32_t fileVersion)
{
    data.read(reinterpret_cast<char*>(&node.instanceRootNodeId_), sizeof(node.instanceRootNodeId_));
    node.instanceRootNodeId_ = Utils::PatchNodeId(node.instanceRootNodeId_);

    data.read(reinterpret_cast<char*>(&node.firstLevelNodeId_), sizeof(node.firstLevelNodeId_));
    node.firstLevelNodeId_ = Utils::PatchNodeId(node.firstLevelNodeId_);

    int32_t modifierCount = 0;
    data.read(reinterpret_cast<char*>(&modifierCount), sizeof(modifierCount));
    for (int32_t i = 0; i < modifierCount; i++) {
        std::string errModifierCode = "";
        auto ptr = UnmarshalRenderModifier(data, errModifierCode);
        if (!ptr) {
            return "Modifier format changed [" + errModifierCode + "]";
        }
        node.AddModifier(std::shared_ptr<RSRenderModifier>(ptr));
    }

    uint32_t drawModifierCount = 0u;
    data.read(reinterpret_cast<char*>(&drawModifierCount), sizeof(drawModifierCount));
    for (uint32_t i = 0; i < drawModifierCount; i++) {
        uint32_t modifierCount = 0u;
        data.read(reinterpret_cast<char*>(&modifierCount), sizeof(modifierCount));
        for (uint32_t j = 0; j < modifierCount; j++) {
            std::string errModifierCode = "";
            auto ptr = UnmarshalRenderModifier(data, errModifierCode);
            if (!ptr) {
                return "DrawModifier format changed [" + errModifierCode + "]";
            }
            node.AddModifier(std::shared_ptr<RSRenderModifier>(ptr));
        }
    }
    if (data.eof()) {
        return "UnmarshalNodeModifiers failed, file is damaged";
    }

    node.ApplyModifiers();
    return "";
}

std::string RSProfiler::UnmarshalTree(RSContext& context, std::stringstream& data, uint32_t fileVersion)
{
    const auto& map = context.GetMutableNodeMap();

    NodeId nodeId = 0;
    data.read(reinterpret_cast<char*>(&nodeId), sizeof(nodeId));
    nodeId = Utils::PatchNodeId(nodeId);

    uint32_t count = 0;
    data.read(reinterpret_cast<char*>(&count), sizeof(count));

    auto node = map.GetRenderNode(nodeId);
    if (!node) {
        return "Error nodeId was not found";
    }
    for (uint32_t i = 0; i < count; i++) {
        NodeId nodeId = 0;
        data.read(reinterpret_cast<char*>(&nodeId), sizeof(nodeId));
        if (node) {
            node->AddChild(map.GetRenderNode(Utils::PatchNodeId(nodeId)), i);
        }
        UnmarshalTree(context, data, fileVersion);
    }
    return "";
}

std::string RSProfiler::DumpRenderProperties(const RSRenderNode& node)
{
    if (node.renderContent_) {
        return node.renderContent_->renderProperties_.Dump();
    }
    return "";
}

std::string RSProfiler::DumpModifiers(const RSRenderNode& node)
{
    if (!node.renderContent_) {
        return "";
    }

    std::string out;
    out += "<";

    for (auto& [type, modifiers] : node.renderContent_->drawCmdModifiers_) {
        out += "(";
        out += std::to_string(static_cast<int32_t>(type));
        out += ", ";
        for (auto& item : modifiers) {
            out += "[";
            const auto propertyId = item->GetPropertyId();
            out += std::to_string(Utils::ExtractPid(propertyId));
            out += "|";
            out += std::to_string(Utils::ExtractNodeId(propertyId));
            out += " type=";
            out += std::to_string(static_cast<int32_t>(item->GetType()));
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
void RSProfiler::FilterAnimationForPlayback(RSAnimationManager& manager)
{
    EraseIf(manager.animations_, [](const auto& pair) -> bool {
        if (!Utils::IsNodeIdPatched(pair.first)) {
            return false;
        }
        pair.second->Finish();
        pair.second->Detach();
        return true;
    });
}

void RSProfiler::SetTransactionTimeCorrection(double replayStartTime, double recordStartTime)
{
    g_transactionTimeCorrection = static_cast<int64_t>((replayStartTime - recordStartTime) * NS_TO_S);
    g_replayStartTimeNano = replayStartTime * NS_TO_S;
}

std::string RSProfiler::GetCommandParcelList(double recordStartTime)
{
    if (g_commandLoopIndexStart >= g_commandLoopIndexEnd) {
        return "";
    }

    uint32_t index = g_commandLoopIndexStart;
    g_commandLoopIndexStart++;
    index %= COMMAND_LOOP_SIZE;

    std::string retStr;

    uint16_t cmdCount = g_commandLoop[index].cmdCount;
    if (cmdCount > COMMAND_PARSE_LIST_SIZE) {
        cmdCount = COMMAND_PARSE_LIST_SIZE;
    }
    if (cmdCount > 0) {
        uint16_t copyBytes = sizeof(PacketParsedCommandList) - (COMMAND_PARSE_LIST_SIZE - cmdCount) * sizeof(uint32_t);
        retStr.resize(copyBytes, ' ');
        Utils::Move(retStr.data(), copyBytes, &g_commandLoop[index], copyBytes);
        g_commandLoop[index].cmdCount = 0;
    }

    return retStr;
}

void RSProfiler::PatchCommand(const Parcel& parcel, RSCommand* command)
{
    if (!IsEnabled()) {
        return;
    }
    if (command == nullptr) {
        return;
    }

    if (g_mode == Mode::WRITE) {
        g_commandCount++;
        uint16_t cmdCount = g_commandParseBuffer.cmdCount;
        if (cmdCount < COMMAND_PARSE_LIST_COUNT) {
            constexpr uint32_t bits = 16u;
            g_commandParseBuffer.cmdCode[cmdCount] =
                command->GetType() + (static_cast<uint32_t>(command->GetSubType()) << bits);
        }
        g_commandParseBuffer.cmdCount++;
    }

    if (command && IsParcelMock(parcel)) {
        command->Patch(Utils::PatchNodeId);
    }
}

void RSProfiler::ExecuteCommand(const RSCommand* command)
{
    if (!IsEnabled()) {
        return;
    }
    if (g_mode != Mode::WRITE && g_mode != Mode::READ) {
        return;
    }
    if (command == nullptr) {
        return;
    }

    g_commandExecuteCount++;
}

uint32_t RSProfiler::PerfTreeFlatten(const std::shared_ptr<RSRenderNode> node,
    std::vector<std::pair<NodeId, uint32_t>>& nodeSet,
    std::unordered_map<NodeId, uint32_t>& mapNode2Count, int depth)
{
    if (!node) {
        return 0;
    }

    constexpr uint32_t depthToAnalyze = 10;
    uint32_t drawCmdListCount = CalcNodeCmdListCount(*node);
    uint32_t valuableChildrenCount = 0;
    if (node->GetSortedChildren()) {
        for (auto& child : *node->GetSortedChildren()) {
            if (child && child->GetType() != RSRenderNodeType::EFFECT_NODE && depth < depthToAnalyze) {
                nodeSet.emplace_back(child->id_, depth + 1);
            }
        }
        for (auto& child : *node->GetSortedChildren()) {
            if (child) {
                drawCmdListCount += PerfTreeFlatten(child, nodeSet, mapNode2Count, depth + 1);
                valuableChildrenCount++;
            }
        }
    }

    if (drawCmdListCount > 0) {
        mapNode2Count[node->id_] = drawCmdListCount;
    }
    return drawCmdListCount;
}

uint32_t RSProfiler::CalcNodeCmdListCount(RSRenderNode& node)
{
    if (!node.renderContent_) {
        return 0;
    }

    uint32_t nodeCmdListCount = 0;
    for (auto& [type, modifiers] : node.renderContent_->drawCmdModifiers_) {
        if (type >= RSModifierType::ENV_FOREGROUND_COLOR) {
            continue;
        }
        for (auto& modifier : modifiers) {
            auto propertyPtr =
                modifier ? std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier->GetProperty())
                         : nullptr;
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
    RSSystemProperties::SetBetaRecordingMode(1);
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

void RSProfiler::DrawingNodeAddClearOp(const std::shared_ptr<Drawing::DrawCmdList>& drawCmdList)
{
    if (dcnRedraw_ || !drawCmdList) {
        return;
    }
    drawCmdList->ClearOp();
}

static uint64_t NewAshmemDataCacheId()
{
    static uint32_t id = 0u;
    return Utils::ComposeDataId(Utils::GetPid(), id++);
}

static void CacheAshmemData(uint64_t id, const uint8_t* data, size_t size)
{
    if (g_mode != Mode::WRITE) {
        return;
    }

    if (data && (size > 0)) {
        Image ashmem;
        ashmem.data.insert(ashmem.data.end(), data, data + size);
        ImageCache::Add(id, std::move(ashmem));
    }
}

static const uint8_t* GetCachedAshmemData(uint64_t id)
{
    const auto ashmem = (g_mode == Mode::READ) ? ImageCache::Get(id) : nullptr;
    return ashmem ? ashmem->data.data() : nullptr;
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

    parcel.WriteUint64(NewAshmemDataCacheId());
}

const void* RSProfiler::ReadParcelData(Parcel& parcel, size_t size, bool& isMalloc)
{
    bool isClientEnabled = false;
    if (!parcel.ReadBool(isClientEnabled)) {
        HRPE("Unable to read is_client_enabled");
        return nullptr;
    }
    if (!isClientEnabled) {
        return RSMarshallingHelper::ReadFromAshmem(parcel, size, isMalloc);
    }

    const uint64_t id = parcel.ReadUint64();
    if (auto data = GetCachedAshmemData(id)) {
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
        return false;
    }
    if (!isClientEnabled) {
        return false;
    }

    [[maybe_unused]] const uint64_t id = parcel.ReadUint64();

    if (g_mode == Mode::READ) {
        constexpr uint32_t skipBytes = 24u;
        parcel.SkipBytes(skipBytes);
        return true;
    }

    return false;
}

uint32_t RSProfiler::GetNodeDepth(const std::shared_ptr<RSRenderNode> node)
{
    uint32_t depth = 0;
    for (auto curNode = node; curNode != nullptr; depth++) {
        curNode = curNode ? curNode->GetParent().lock() : nullptr;
    }
    return depth;
}

std::string RSProfiler::SendMessageBase()
{
    const std::lock_guard<std::mutex> guard(g_msgBaseMutex);
    if (g_msgBaseList.empty()) {
        return "";
    }
    std::string value = g_msgBaseList.front();
    g_msgBaseList.pop();
    return value;
}

void RSProfiler::SendMessageBase(const std::string& msg)
{
    const std::lock_guard<std::mutex> guard(g_msgBaseMutex);
    g_msgBaseList.push(msg);
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
    if (g_mode == Mode::READ) {
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

    if (g_mode == Mode::READ) {
        if (!g_animeStartMap.count(id)) {
            return nanoTime;
        }
        int64_t minDt = INT64_MAX, minTime = nanoTime - g_replayStartTimeNano;
        for (const auto recordedTime : g_animeStartMap[id]) {
            int64_t dt = abs(recordedTime - (nanoTime - g_replayStartTimeNano));
            if (dt < minDt) {
                minDt = dt;
                minTime = recordedTime;
            }
        }
        return minTime + g_replayStartTimeNano;
    } else if (g_mode == Mode::WRITE) {
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
    if (RSProfiler::GetMode() != Mode::READ) {
        return false;
    }

    if (parent->GetType() == RSRenderNodeType::DISPLAY_NODE &&
        ! (child->GetId() & Utils::ComposeNodeId(Utils::GetMockPid(0), 0))) {
        // BLOCK LOCK-SCREEN ATTACH TO DISPLAY
        g_childOfDisplayNodesPostponed.clear();
        g_childOfDisplayNodesPostponed.emplace_back(child);
        return true;
    }
    return false;
}

std::vector<RSRenderNode::WeakPtr>& RSProfiler::GetChildOfDisplayNodesPostponed()
{
    return g_childOfDisplayNodesPostponed;
}

} // namespace OHOS::Rosen
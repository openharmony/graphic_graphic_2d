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

#include "rs_profiler_base.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "message_parcel.h"
#include "rs_profiler_utils.h"

#include "command/rs_base_node_command.h"
#include "pipeline/rs_display_render_node.h"

namespace OHOS::Rosen {

static SpecParseMode g_mode;
static std::vector<pid_t> g_pidList;
static pid_t g_pidValue = 0;
static NodeId g_parentNode = 0;
static std::map<uint64_t, ImageCacheRecord> g_imageMap;
static std::atomic<uint32_t> g_parsedCmdCount = 0;
static std::atomic<uint32_t> g_lastImagemapCount = 0;

static uint64_t g_pauseAfterTime = 0;
static uint64_t g_pauseCumulativeTime = 0;

ImageCache& RSProfilerBase::GetImageCache()
{
    return g_imageMap;
}

uint32_t RSProfilerBase::ImagesAddedCountGet()
{
    const uint32_t retValue = g_imageMap.size() - g_lastImagemapCount;
    g_lastImagemapCount = g_imageMap.size();
    return retValue;
}

uint32_t RSProfilerBase::ParsedCmdCountGet()
{
    const uint32_t retValue = g_parsedCmdCount;
    g_parsedCmdCount = 0;
    return retValue;
}

void RSProfilerBase::ParsedCmdCountAdd(uint32_t addon)
{
    g_parsedCmdCount += addon;
}

bool RSProfilerBase::IsParcelMock(const Parcel& parcel)
{
    // gcc C++ optimization error (?): this is not working without volatile
    const volatile auto address = reinterpret_cast<uint64_t>(&parcel);
    return ((address & 1u) != 0);
}

std::shared_ptr<MessageParcel> RSProfilerBase::CopyParcel(const MessageParcel& parcel)
{
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

uint64_t RSProfilerBase::ReadTime(Parcel& parcel)
{
    return parcel.ReadInt64();
}

NodeId RSProfilerBase::ReadNodeId(Parcel& parcel)
{
    return PatchPlainNodeId(parcel, static_cast<NodeId>(parcel.ReadUint64()));
}

pid_t RSProfilerBase::ReadProcessId(Parcel& parcel)
{
    return PatchPlainProcessId(parcel, static_cast<pid_t>(parcel.ReadUint32()));
}

NodeId RSProfilerBase::PatchPlainNodeId(const Parcel& parcel, NodeId id)
{
    if ((g_mode != SpecParseMode::READ) || !IsParcelMock(parcel)) {
        return id;
    }

    return Utils::PatchNodeId(id);
}

pid_t RSProfilerBase::PatchPlainProcessId(const Parcel& parcel, pid_t pid)
{
    if ((g_mode != SpecParseMode::READ) || !IsParcelMock(parcel)) {
        return pid;
    }

    return Utils::GetMockPid(pid);
}

void RSProfilerBase::SpecParseModeSet(SpecParseMode mode)
{
    g_mode = mode;
    if (g_mode == SpecParseMode::NONE) {
        g_pidList.clear();
        g_imageMap.clear();
        g_pidValue = 0;
        g_parentNode = 0;
        g_parsedCmdCount = 0;
        g_lastImagemapCount = 0;
        g_pauseAfterTime = 0;
        g_pauseCumulativeTime = 0;
    }
}

SpecParseMode RSProfilerBase::SpecParseModeGet()
{
    return g_mode;
}

void RSProfilerBase::SpecParseReplacePIDSet(
    const std::vector<pid_t>& replacePidList, pid_t replacePidValue, NodeId parentNode)
{
    g_pidList = replacePidList;
    g_pidValue = replacePidValue;
    g_parentNode = parentNode;
}

NodeId RSProfilerBase::SpecParseParentNodeGet()
{
    return g_parentNode;
}

const std::vector<pid_t>& RSProfilerBase::SpecParsePidListGet()
{
    return g_pidList;
}

pid_t RSProfilerBase::SpecParsePidReplaceGet()
{
    return g_pidValue;
}

void RSProfilerBase::ReplayImageAdd(uint64_t uniqueId, void* image, uint32_t imageSize, uint32_t skipBytes)
{
    if ((g_mode != SpecParseMode::WRITE) || (g_imageMap.count(uniqueId) > 0)) {
        return;
    }

    if (image && (imageSize > 0)) {
        auto imageData = new uint8_t[imageSize];
        memmove_s(imageData, imageSize, image, imageSize);

        ImageCacheRecord record;
        record.image.reset(imageData);
        record.imageSize = imageSize;
        record.skipBytes = skipBytes;

        g_imageMap.insert({ uniqueId, record });
    }
}

ImageCacheRecord* RSProfilerBase::ReplayImageGet(uint64_t uniqueId)
{
    if (g_mode != SpecParseMode::READ) {
        return nullptr;
    }

    if (g_imageMap.count(uniqueId) == 0) {
        return nullptr;
    }

    return &g_imageMap[uniqueId];
}

std::string RSProfilerBase::ReplayImagePrintList()
{
    if (g_mode != SpecParseMode::READ) {
        return "";
    }

    std::string out;
    for (const auto& it : g_imageMap) {
        out += std::to_string(Utils::ExtractPid(it.first)) + ":" + std::to_string(Utils::ExtractNodeId(it.first)) + " ";
    }

    return out;
}

uint64_t RSProfilerBase::TimePauseApply(uint64_t curTime)
{
    if (g_mode != SpecParseMode::READ) {
        return curTime;
    }

    if (curTime >= g_pauseAfterTime && g_pauseAfterTime > 0) {
        return g_pauseAfterTime - g_pauseCumulativeTime;
    }
    return curTime - g_pauseCumulativeTime;
}

void RSProfilerBase::TimePauseAt(uint64_t curTime, uint64_t newPauseAfterTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += curTime - g_pauseAfterTime;
        }
    }
    g_pauseAfterTime = newPauseAfterTime;
}

void RSProfilerBase::TimePauseResume(uint64_t curTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += curTime - g_pauseAfterTime;
        }
    }
    g_pauseAfterTime = 0;
}

void RSProfilerBase::TimePauseClear()
{
    g_pauseCumulativeTime = 0;
    g_pauseAfterTime = 0;
}

uint32_t RSProfilerBase::GenerateUniqueImageId()
{
    static uint32_t uniqueImageId = 0u;
    return uniqueImageId++;
}

Vector4f RSProfilerBase::GetScreenRect(RSContext& context)
{
    const std::shared_ptr<RSBaseRenderNode>& rootNode = context.GetGlobalRootRenderNode();
    // without these checks device might get stuck on startup
    if (!rootNode || (rootNode->GetChildrenCount() != 1)) {
        return {};
    }

    std::shared_ptr<RSDisplayRenderNode> displayNode =
        RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(rootNode->GetSortedChildren().front());
    if (!displayNode) {
        return {};
    }

    const RectI rect = displayNode->GetDirtyManager()->GetSurfaceRect();
    return { rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() };
}

void RSProfilerBase::TransactionDataOnProcess(RSContext& context)
{
    if (SpecParseModeGet() != SpecParseMode::READ) {
        return;
    }

    auto& nodeMap = context.GetNodeMap();
    std::shared_ptr<RSRenderNode> baseNode = nullptr;
    NodeId baseNodeId = 0;
    const Vector4f screenRect = GetScreenRect(context);
    for (auto item : SpecParsePidListGet()) {
        const NodeId nodeId = Utils::PatchNodeId(Utils::GetRootNodeId(item));
        auto node = nodeMap.GetRenderNode(nodeId);
        if (node) {
            baseNode = node;
            baseNodeId = nodeId;
            RSProperties& properties = node->GetMutableRenderProperties();
            properties.SetBounds(screenRect);
            properties.SetFrame(screenRect);
            auto parentNode = nodeMap.GetRenderNode(SpecParseParentNodeGet());
            if (parentNode) {
                RSProperties& parentProperties = parentNode->GetMutableRenderProperties();
                parentProperties.SetBounds(screenRect);
                parentProperties.SetFrame(screenRect);
            }
        }
    }

    if (baseNode) {
        BaseNodeCommandHelper::ClearChildren(context, SpecParseParentNodeGet());
        BaseNodeCommandHelper::AddChild(context, SpecParseParentNodeGet(), baseNodeId, 0);
    }
}

} // namespace OHOS::Rosen
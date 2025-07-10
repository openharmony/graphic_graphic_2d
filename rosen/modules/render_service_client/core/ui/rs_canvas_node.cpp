/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "ui/rs_canvas_node.h"

#include <algorithm>
#include <string>

#include "common/rs_obj_abs_geometry.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_node_command.h"
#include "platform/common/rs_log.h"
#include "common/rs_obj_geometry.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_node_map.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_ui_context.h"
#ifdef RS_ENABLE_VK
#include "modifier_render_thread/rs_modifiers_draw.h"
#include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "media_errors.h"
#endif

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []() -> int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif
namespace OHOS {
namespace Rosen {
RSCanvasNode::SharedPtr RSCanvasNode::Create(
    bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
{
    SharedPtr node(new RSCanvasNode(isRenderServiceNode, isTextureExportNode, rsUIContext));
    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeCreate>(node->GetId(), isTextureExportNode);
    node->AddCommand(command, node->IsRenderServiceNode());
    node->SetUIContextToken();
    return node;
}

RSCanvasNode::RSCanvasNode(bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, isTextureExportNode, rsUIContext)
{
    tid_ = gettid();
}

RSCanvasNode::RSCanvasNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode,
    std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, id, isTextureExportNode, rsUIContext)
{
    tid_ = gettid();
}

RSCanvasNode::~RSCanvasNode()
{
    CheckThread();
#ifdef RS_ENABLE_VK
    if (IsHybridRenderCanvas()) {
        RSModifiersDraw::RemoveSurfaceByNodeId(GetId(), true);
    }
#endif
}

void RSCanvasNode::SetHDRPresent(bool hdrPresent)
{
    CheckThread();
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeSetHDRPresent>(GetId(), hdrPresent);
    if (AddCommand(command, true)) {
        ROSEN_LOGD("RSCanvasNode::SetHDRPresent HDRClient set hdr true");
    }
}

void RSCanvasNode::SetColorGamut(uint32_t colorGamut)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeSetColorGamut>(GetId(), colorGamut);
    AddCommand(command, true);
}

ExtendRecordingCanvas* RSCanvasNode::BeginRecording(int width, int height)
{
    CheckThread();

    if (recordingCanvas_) {
        delete recordingCanvas_;
        recordingCanvas_ = nullptr;
        RS_LOGE("RSCanvasNode::BeginRecording last beginRecording without finishRecording");
    }

    recordingCanvas_ = new ExtendRecordingCanvas(width, height);
    recordingCanvas_->SetIsCustomTextType(isCustomTextType_);
    recordingCanvas_->SetIsCustomTypeface(isCustomTypeface_);
    if (auto recording = recordingCanvas_->GetDrawCmdList()) {
        recording->SetIsNeedUnmarshalOnDestruct(!IsRenderServiceNode());
    }
    if (!recordingUpdated_) {
        return recordingCanvas_;
    }
    recordingUpdated_ = false;
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeClearRecording>(GetId());
    AddCommand(command, IsRenderServiceNode());
    return recordingCanvas_;
}

bool RSCanvasNode::IsRecording() const
{
    return recordingCanvas_ != nullptr;
}

void RSCanvasNode::RegisterNodeMap()
{
    auto rsContext = GetRSUIContext();
    if (rsContext == nullptr) {
        return;
    }
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterNode(shared_from_this());
}

void RSCanvasNode::CreateRenderNodeForTextureExportSwitch()
{
    CheckThread();

    if (IsRenderServiceNode()) {
        hasCreateRenderNodeInRS_ = true;
    } else {
        hasCreateRenderNodeInRT_ = true;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeCreate>(GetId(), isTextureExportNode_);
    AddCommand(command, IsRenderServiceNode());
}

void RSCanvasNode::FinishRecording()
{
    CheckThread();
    if (!IsRecording()) {
        return;
    }
    auto recording = recordingCanvas_->GetDrawCmdList();
    delete recordingCanvas_;
    recordingCanvas_ = nullptr;
    if (recording && recording->IsEmpty()) {
        return;
    }
    if (recording != nullptr && RSSystemProperties::GetDrawTextAsBitmap()) {
        // replace drawOpItem with cached one (generated by CPU)
        recording->GenerateCache();
    }

#if defined(MODIFIER_NG)
    auto modifierType = static_cast<uint16_t>(
        drawContentLast_ ? ModifierNG::RSModifierType::FOREGROUND_STYLE : ModifierNG::RSModifierType::CONTENT_STYLE);
#else
    auto modifierType =
        static_cast<uint16_t>(drawContentLast_ ? RSModifierType::FOREGROUND_STYLE : RSModifierType::CONTENT_STYLE);
#endif
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording, modifierType);
    AddCommand(command, IsRenderServiceNode());
    recordingUpdated_ = true;
}

void RSCanvasNode::DrawOnNode(RSModifierType type, DrawFunc func)
{
    CheckThread();
    auto recordingCanvas = std::make_shared<ExtendRecordingCanvas>(GetPaintWidth(), GetPaintHeight());
    recordingCanvas->SetIsCustomTextType(isCustomTextType_);
    recordingCanvas->SetIsCustomTypeface(isCustomTypeface_);
    func(recordingCanvas);

    auto recording = recordingCanvas->GetDrawCmdList();
    if (recording) {
        recording->SetIsNeedUnmarshalOnDestruct(!IsRenderServiceNode());
    }
    if (recording && recording->IsEmpty()) {
        return;
    }
    RSNode::SetDrawNode();
    if (recording != nullptr && RSSystemProperties::GetDrawTextAsBitmap()) {
        // replace drawOpItem with cached one (generated by CPU)
        recording->GenerateCache();
    }
#if defined(MODIFIER_NG)
    auto modifierType = static_cast<uint16_t>(ModifierTypeConvertor::ToModifierNGType(type));
#else
    auto modifierType = static_cast<uint16_t>(type);
#endif
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording, modifierType);
    if (AddCommand(command, IsRenderServiceNode())) {
        recordingUpdated_ = true;
    }
}

float RSCanvasNode::GetPaintWidth() const
{
    auto frame = GetStagingProperties().GetFrame();
    return frame.z_ <= 0.f ? GetStagingProperties().GetBounds().z_ : frame.z_;
}

float RSCanvasNode::GetPaintHeight() const
{
    auto frame = GetStagingProperties().GetFrame();
    return frame.w_ <= 0.f ? GetStagingProperties().GetBounds().w_ : frame.w_;
}

void RSCanvasNode::SetFreeze(bool isFreeze)
{
    if (!IsUniRenderEnabled()) {
        ROSEN_LOGE("SetFreeze is not supported in separate render");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSCanvasNode::SetFreeze id:%llu", GetId());
    CheckThread();
    RSNode::SetDrawNode();
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(GetId(), isFreeze);
    AddCommand(command, true);
}

void RSCanvasNode::OnBoundsSizeChanged() const
{
    auto bounds = GetStagingProperties().GetBounds();
    std::lock_guard<std::mutex> lock(mutex_);
    if (boundsChangedCallback_) {
        boundsChangedCallback_(bounds);
    }
}

void RSCanvasNode::SetBoundsChangedCallback(BoundsChangedCallback callback)
{
  std::lock_guard<std::mutex> lock(mutex_);
  boundsChangedCallback_ = callback;
}

bool RSCanvasNode::GetBitmap(Drawing::Bitmap& bitmap, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    if (!IsHybridRenderCanvas()) {
        return false;
    }
    bool ret = false;
#ifdef RS_ENABLE_VK
    RSModifiersDrawThread::Instance().PostSyncTask([this, &bitmap, &ret]() {
        auto pixelMap = RSModifiersDraw::GetPixelMapByNodeId(GetId(), false);
        if (pixelMap == nullptr) {
            RS_LOGE("RSCanvasNode::GetBitmap pixelMap is nullptr");
            return;
        }
        Drawing::ImageInfo info(
            pixelMap->GetWidth(), pixelMap->GetHeight(), Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL);
        if (!bitmap.InstallPixels(info, pixelMap->GetWritablePixels(), pixelMap->GetRowBytes())) {
            RS_LOGE("RSCanvasNode::GetBitmap get bitmap fail");
            return;
        }
        ret = true;
    });
#endif
    return ret;
}

bool RSCanvasNode::GetPixelmap(std::shared_ptr<Media::PixelMap> pixelMap,
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList, const Drawing::Rect* rect)
{
    if (!IsHybridRenderCanvas()) {
        return false;
    }
    if (pixelMap == nullptr || rect == nullptr) {
        RS_LOGE("RSCanvasNode::GetPixelmap pixelMap or rect is nullptr");
        return false;
    }
    bool ret = false;
#ifdef RS_ENABLE_VK
    RSModifiersDrawThread::Instance().PostSyncTask([this, pixelMap, rect, &ret]() {
        auto srcPixelMap = RSModifiersDraw::GetPixelMapByNodeId(GetId(), false);
        if (srcPixelMap == nullptr) {
            RS_LOGE("RSCanvasNode::GetPixelmap get source pixelMap fail");
            return;
        }
        Media::Rect srcRect = { rect->GetLeft(), rect->GetTop(), rect->GetWidth(), rect->GetHeight() };
        auto ret =
            srcPixelMap->ReadPixels(Media::RWPixelsOptions { static_cast<uint8_t*>(pixelMap->GetWritablePixels()),
                pixelMap->GetByteCount(), 0, pixelMap->GetRowStride(), srcRect, Media::PixelFormat::RGBA_8888 });
        if (ret != Media::SUCCESS) {
            RS_LOGE("RSCanvasNode::GetPixelmap get pixelMap fail");
            return;
        }
        ret = true;
    });
#endif
    return ret;
}

bool RSCanvasNode::ResetSurface(int width, int height)
{
    if (!IsHybridRenderCanvas()) {
        return false;
    }
#ifdef RS_ENABLE_VK
    return RSModifiersDraw::ResetSurfaceByNodeId(width, height, GetId(), true, true);
#endif
    return false;
}

void RSCanvasNode::CheckThread()
{
    if (tid_ != gettid()) {
        ROSEN_LOGE("RSCanvasNode::CheckThread Must be called on same thread");
    }
}

// [Attention] Only used in PC window resize scene now
void RSCanvasNode::SetLinkedRootNodeId(NodeId rootNodeId)
{
    ROSEN_LOGI("RSCanvasNode::SetLinkedRootNodeId nodeId: %{public}" PRIu64 ", rootNode: %{public}" PRIu64 "",
         GetId(), rootNodeId);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasNodeSetLinkedRootNodeId>(GetId(), rootNodeId);
    AddCommand(command, true);
    linkedRootNodeId_ = rootNodeId;
}

// [Attention] Only used in PC window resize scene now
NodeId RSCanvasNode::GetLinkedRootNodeId()
{
    return linkedRootNodeId_;
}

bool RSCanvasNode::Marshalling(Parcel& parcel) const
{
    bool success =
        parcel.WriteUint64(GetId()) && parcel.WriteBool(IsRenderServiceNode()) && parcel.WriteUint64(linkedRootNodeId_);
    if (!success) {
        ROSEN_LOGE("RSCanvasNode::Marshalling, read parcel failed");
    }
    return success;
}

RSCanvasNode::SharedPtr RSCanvasNode::Unmarshalling(Parcel& parcel)
{
    uint64_t id = UINT64_MAX;
    NodeId linkedRootNodeId = INVALID_NODEID;
    bool isRenderServiceNode = false;
    if (!(parcel.ReadUint64(id) && parcel.ReadBool(isRenderServiceNode) && parcel.ReadUint64(linkedRootNodeId))) {
        ROSEN_LOGE("RSCanvasNode::Unmarshalling, read param failed");
        return nullptr;
    }

    if (auto prevNode = RSNodeMap::Instance().GetNode(id)) {
        RS_LOGW("RSCanvasNode::Unmarshalling, the node id is already in the map");
        // if the node id is already in the map, we should not create a new node
        return prevNode->ReinterpretCastTo<RSCanvasNode>();
    }

    SharedPtr canvasNode(new RSCanvasNode(isRenderServiceNode, id));
    RSNodeMap::MutableInstance().RegisterNode(canvasNode);

    // for nodes constructed by unmarshalling, we should not destroy the corresponding render node on destruction
    canvasNode->skipDestroyCommandInDestructor_ = true;
    canvasNode->linkedRootNodeId_ = linkedRootNodeId;

    return canvasNode;
}

} // namespace Rosen
} // namespace OHOS

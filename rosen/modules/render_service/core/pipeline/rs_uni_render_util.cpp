/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_uni_render_util.h"

#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
void RSUniRenderUtil::DrawBufferOnCanvas(sptr<SurfaceBuffer> buffer, const ColorGamut& dstGamut,
    RSPaintFilterCanvas& canvas, SkRect srcRect, SkRect dstRect)
{
    SkBitmap bitmap;
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(buffer, newBuffer, dstGamut, bitmap)) {
        RS_LOGE("RSUniRenderUtil::DrawBufferOnCanvas ConvertBufferToBitmap failed");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    canvas.save();
    canvas.drawBitmapRect(bitmap, srcRect, dstRect, &paint);
    canvas.restore();
}

#ifdef RS_ENABLE_EGLIMAGE
void RSUniRenderUtil::DrawImageOnCanvas(BufferInfo& bufferInfo, RSPaintFilterCanvas& canvas, SkRect srcRect,
    SkRect dstRect)
{
    auto renderEngine = RSMainThread::Instance()->GetRenderEngine();
    auto renderContext = renderEngine->GetRenderContext();
    auto eglImageManager =  renderEngine->GetRSEglImageManager();
    sk_sp<SkImage> image;
    if (!RSBaseRenderUtil::ConvertBufferToEglImage(bufferInfo.buffer, eglImageManager, renderContext->GetGrContext(),
        bufferInfo.acquireFence, image)) {
        RS_LOGE("RSUniRenderUtil::DrawImageOnCanvas ConvertBufferToEglImage failed");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    canvas.save();
    canvas.drawImageRect(image, srcRect, dstRect, &paint);
    canvas.restore();

    auto consumerSurface = bufferInfo.consumerSurface;
    if (consumerSurface != nullptr) {
        GSError error = consumerSurface->RegisterDeleteBufferListener([eglImageManager] (int32_t bufferId) {
            eglImageManager->UnMapEglImageFromSurfaceBuffer(bufferId);
        });
        if (error != GSERROR_OK) {
            RS_LOGE("RSUniRenderVisitor::DrawImageOnCanvas: fail to register UnMapEglImage callback.");
        }
    }
}
#endif

void RSUniRenderUtil::UpdateRenderNodeDstRect(RSRenderNode& node)
{
    auto parentNode = node.GetParent().lock();
    std::shared_ptr<RSRenderNode> rsParent = nullptr;
    if (!parentNode) {
        RS_LOGE("RSUniRenderUtil::UpdateDstRect: fail to get parent dstRect.");
        return;
    }
    rsParent = parentNode->ReinterpretCastTo<RSRenderNode>();
    auto& property = node.GetMutableRenderProperties();
    property.UpdateGeometry(rsParent ? &(rsParent->GetRenderProperties()) : nullptr, true);
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (geoPtr && node.IsInstanceOf<RSSurfaceRenderNode>()) {
        std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
        auto surfaceNode = nodePtr->ReinterpretCastTo<RSSurfaceRenderNode>();
        surfaceNode->SetDstRect(geoPtr->GetAbsRect());
        auto dstRect = surfaceNode->GetDstRect();
        RS_LOGD("RSUniRenderUtil::UpdateDstRect: nodeName: %s, dstRect[%d, %d, %d, %d].",
            surfaceNode->GetName().c_str(),
            dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetWidth(), dstRect.GetHeight());
    }
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
    int32_t bufferAge)
{
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    Occlusion::Region curRegion;
    node->CollectSurface(node, curAllSurfaces, true);
    for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
        auto sufaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (sufaceNode == nullptr) {
            continue;
        }
        auto surfaceDirtyManager = sufaceNode->GetDirtyManager();
        if (!surfaceDirtyManager->SetBufferAge(bufferAge)) {
            ROSEN_LOGE("RSUniRenderUtil::MergeVisibleDirtyRegion with invalid buffer age %d", bufferAge);
        }
        surfaceDirtyManager->UpdateDirty();
        auto surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
            surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
        auto visibleRegion = sufaceNode->GetVisibleRegion();
        Occlusion::Region surfaceDirtyRegion { dirtyRect };
        Occlusion::Region uniRegion = surfaceDirtyRegion.And(visibleRegion);
        curRegion = curRegion.Or(uniRegion);
    }
    return curRegion;
}
} // namespace Rosen
} // namespace OHOS

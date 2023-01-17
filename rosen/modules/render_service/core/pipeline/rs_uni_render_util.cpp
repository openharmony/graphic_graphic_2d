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
void RSUniRenderUtil::UpdateRenderNodeDstRect(RSRenderNode& node, const SkMatrix& matrix)
{
    // [planning] use RSRenderNode::Update instead
    auto parentNode = node.GetParent().lock();
    if (!parentNode) {
        RS_LOGE("RSUniRenderUtil::UpdateDstRect: fail to get parent dstRect.");
        return;
    }
    auto rsParent = parentNode->ReinterpretCastTo<RSRenderNode>();
    auto parentProp = rsParent ? &(rsParent->GetRenderProperties()) : nullptr;
    auto& property = node.GetMutableRenderProperties();
    auto surfaceNode = node.ReinterpretCastTo<RSSurfaceRenderNode>();
    auto isSelfDrawingNode = surfaceNode &&
        (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE ||
        surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    Vector2f offset(0.f, 0.f);
    if (isSelfDrawingNode) {
        offset = { parentProp->GetFrameOffsetX(), parentProp->GetFrameOffsetY() };
    }
    property.UpdateGeometry(parentProp, true, offset);
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (geoPtr && node.IsInstanceOf<RSSurfaceRenderNode>()) {
        if (!isSelfDrawingNode) {
            geoPtr->ConcatMatrix(matrix);
        }
        RS_LOGD("RSUniRenderUtil::UpdateDstRect: nodeName: %s, dstRect[%s].",
            surfaceNode->GetName().c_str(), surfaceNode->GetDstRect().ToString().c_str());
    }
}

void RSUniRenderUtil::MergeDirtyHistory(std::shared_ptr<RSDisplayRenderNode>& node, int32_t bufferAge)
{
    // update all child surfacenode history
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (!surfaceDirtyManager->SetBufferAge(bufferAge)) {
            ROSEN_LOGE("RSUniRenderUtil::MergeVisibleDirtyRegion with invalid buffer age %d", bufferAge);
        }
        surfaceDirtyManager->IntersectDirtyRect(surfaceNode->GetOldDirtyInSurface());
        surfaceDirtyManager->UpdateDirty();
        if (surfaceNode->GetDstRect().IsInsideOf(surfaceDirtyManager->GetDirtyRegion())
            && surfaceNode->HasContainerWindow()) {
            node->GetDirtyManager()->MergeDirtyRect(surfaceNode->GetDstRect());
        }
    }
    // update display dirtymanager
    node->UpdateDisplayDirtyManager(bufferAge);
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node)
{
    Occlusion::Region allSurfaceVisibleDirtyRegion;
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        auto surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
            surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
        auto visibleRegion = surfaceNode->GetVisibleRegion();
        Occlusion::Region surfaceDirtyRegion { dirtyRect };
        Occlusion::Region surfaceVisibleDirtyRegion = surfaceDirtyRegion.And(visibleRegion);
        surfaceNode->SetVisibleDirtyRegion(surfaceVisibleDirtyRegion);
        allSurfaceVisibleDirtyRegion = allSurfaceVisibleDirtyRegion.Or(surfaceVisibleDirtyRegion);
    }
    return allSurfaceVisibleDirtyRegion;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSSurfaceRenderNode& node, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
    params.paint.setAntiAlias(true);
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);

    const RSProperties& property = node.GetRenderProperties();
    params.dstRect = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());

    RectF localBounds = {0.0f, 0.0f, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RSBaseRenderUtil::FlipMatrix(node, params);
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(node, localBounds, params);
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSDisplayRenderNode& node, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
    params.paint.setAntiAlias(true);
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    params.dstRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    return params;
}

void RSUniRenderUtil::DrawCachedSurface(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas,
    sk_sp<SkSurface> surface)
{
    if (surface == nullptr) {
        return;
    }
    canvas.save();
    canvas.scale(node.GetRenderProperties().GetBoundsWidth() / surface->width(),
        node.GetRenderProperties().GetBoundsHeight() / surface->height());
    SkPaint paint;
    surface->draw(&canvas, 0.0, 0.0, &paint);
    canvas.restore();
}

void RSUniRenderUtil::DrawCachedImage(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas, sk_sp<SkImage> image)
{
    if (image == nullptr) {
        return;
    }
    canvas.save();
    canvas.scale(node.GetRenderProperties().GetBoundsWidth() / image->width(),
        node.GetRenderProperties().GetBoundsHeight() / image->height());
    SkPaint paint;
    canvas.drawImage(image.get(), 0.0, 0.0, &paint);
    canvas.restore();
}

bool RSUniRenderUtil::ReleaseBuffer(RSSurfaceHandler& surfaceHandler)
{
    auto& consumer = surfaceHandler.GetConsumer();
    if (consumer == nullptr) {
        return false;
    }

    auto& preBuffer = surfaceHandler.GetPreBuffer();
    if (preBuffer.buffer != nullptr) {
        static bool firstEntry = true;
        static std::function<void()> firstBufferRelease = nullptr;
        if (firstEntry) {
            // In order to avoid waiting for buffers' fence, we delay the first ReleaseBuffer to alloc 3 buffers.
            // [planning] delete this function after Repaint parallelization.
            firstEntry = false;
            firstBufferRelease = [consumer, buffer = preBuffer.buffer, fence = preBuffer.releaseFence]() mutable {
                auto ret = consumer->ReleaseBuffer(buffer, fence);
                if (ret != OHOS::SURFACE_ERROR_OK) {
                    RS_LOGE("RsDebug firstBufferRelease failed(ret: %d)!", ret);
                }
            };
            preBuffer.Reset();
            return true;
        }
        if (firstBufferRelease) {
            firstBufferRelease();
            firstBufferRelease = nullptr;
        }
        auto ret = consumer->ReleaseBuffer(preBuffer.buffer, preBuffer.releaseFence);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGE("RsDebug surfaceHandler(id: %" PRIu64 ") ReleaseBuffer failed(ret: %d)!",
                surfaceHandler.GetNodeId(), ret);
            return false;
        }
        // reset prevBuffer if we release it successfully,
        // to avoid releasing the same buffer next frame in some situations.
        preBuffer.Reset();
    }

    return true;
}
} // namespace Rosen
} // namespace OHOS

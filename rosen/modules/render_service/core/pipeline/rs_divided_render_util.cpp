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
#include "rs_divided_render_util.h"

#include <parameters.h>

#include "common/rs_obj_abs_geometry.h"
#include "parameters.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
BufferDrawParam RSDividedRenderUtil::CreateBufferDrawParam(
    const RSSurfaceRenderNode& node, bool inLocalCoordinate, bool isClipHole, bool forceCPU, bool setColorFilter)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    (void)(forceCPU); // unused param.
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
    params.paint.SetAlphaF(node.GetGlobalAlpha());
    params.paint.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);
    params.setColorFilter = setColorFilter;

    const RSProperties& property = node.GetRenderProperties();
    auto backgroundColor = property.GetBackgroundColor();
    auto backgroundAlpha = backgroundColor.GetAlpha();
    int16_t finalBackgroundAlpha = static_cast<int16_t>(backgroundAlpha * node.GetGlobalAlpha());
    backgroundColor.SetAlpha(finalBackgroundAlpha);
    params.backgroundColor = static_cast<Drawing::ColorQuad>(backgroundColor.AsArgbInt());

    const RectF absBounds = {
        node.GetTotalMatrix().Get(Drawing::Matrix::TRANS_X), node.GetTotalMatrix().Get(Drawing::Matrix::TRANS_Y),
        property.GetBoundsWidth(), property.GetBoundsHeight()};
    RectF localBounds = {0.0f, 0.0f, absBounds.GetWidth(), absBounds.GetHeight()};

    // calculate clipRect and clipRRect(if has cornerRadius) for canvas.
    CalculateSurfaceNodeClipRects(node, absBounds, localBounds, inLocalCoordinate, params);

    // inLocalCoordinate: reset the translate to (0, 0).
    // else: use node's total matrix.
    if (inLocalCoordinate) {
        params.matrix = Drawing::Matrix();
    } else {
        params.matrix = node.GetTotalMatrix();
    }

    // we can use only the bound's size (ignore its offset) now,
    // (the canvas was moved to the node's left-top point correctly).
    params.dstRect = Drawing::Rect(0, 0, localBounds.GetWidth(), localBounds.GetHeight());

    const sptr<IConsumerSurface>& surface = node.GetConsumer();
    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    if (isClipHole || surface == nullptr || buffer == nullptr) {
        return params;
    }

    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(surface->GetTransform(), property.GetFrameGravity(),
        localBounds, params);
    RSBaseRenderUtil::FlipMatrix(surface->GetTransform(), params);
    return params;
}

void RSDividedRenderUtil::CalculateSurfaceNodeClipRects(
    const RSSurfaceRenderNode& node,
    const RectF& absBounds,
    const RectF& localBounds,
    bool inLocalCoordinate,
    BufferDrawParam& params)
{
    const RSProperties& property = node.GetRenderProperties();
    params.cornerRadius = property.GetCornerRadius();
    params.isNeedClip = property.GetClipToFrame();
    if (inLocalCoordinate) {
        // in canvas's local coordinate system.
        params.clipRect = Drawing::Rect(0, 0, localBounds.GetWidth(), localBounds.GetHeight());
        params.clipRRect = RRect(localBounds, params.cornerRadius);
    } else {
        // in logical screen's coordinate system.
        const auto& clipRect = node.GetDstRect();
        params.clipRect = Drawing::Rect(clipRect.GetLeft(), clipRect.GetTop(),
            clipRect.GetWidth() + clipRect.GetLeft(), clipRect.GetHeight() + clipRect.GetTop());
        params.clipRRect = RRect(absBounds, params.cornerRadius);
    }
}
} // namespace Rosen
} // namespace OHOS

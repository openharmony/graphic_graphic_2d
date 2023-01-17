/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
bool RSDividedRenderUtil::enableClient = false;

void RSDividedRenderUtil::SetNeedClient(bool flag)
{
    enableClient = flag;
}

bool RSDividedRenderUtil::IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info)
{
    if (IsForceClient()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient: client composition is force enabled.");
        return true;
    }

    if (enableClient) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client");
        return true;
    }

    const auto& property = node.GetRenderProperties();
    auto backgroundColor = static_cast<SkColor>(property.GetBackgroundColor().AsArgbInt());
    // If node's gravity is not RESIZE and backgroundColor is not transparent,
    // we check the src and dst size to decide whether to use client composition or not.
    if (property.GetFrameGravity() != Gravity::RESIZE && SkColorGetA(backgroundColor) != SK_AlphaTRANSPARENT &&
        (info.srcRect.w != info.dstRect.w || info.srcRect.h != info.dstRect.h)) {
        return true;
    }

    if (property.GetBackgroundFilter() || property.GetFilter()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need filter");
        return true;
    }
    
    if (!property.GetCornerRadius().IsZero()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need round corner");
        return true;
    }
    if (property.IsShadowValid()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need shadow");
        return true;
    }
    if (property.GetRotation() != 0 || property.GetRotationX() != 0 || property.GetRotationY() != 0 ||
        property.GetQuaternion() != Quaternion()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need rotation");
        return true;
    }
    return false;
}

bool RSDividedRenderUtil::IsForceClient()
{
    return (std::atoi((system::GetParameter("rosen.client_composition.enabled", "0")).c_str()) != 0);
}

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
    params.paint.setAlphaf(node.GetGlobalAlpha());
    params.paint.setAntiAlias(true);
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
    params.setColorFilter = setColorFilter;

    const RSProperties& property = node.GetRenderProperties();
    auto backgroundColor = property.GetBackgroundColor();
    auto backgroundAlpha = backgroundColor.GetAlpha();
    int16_t finalBackgroundAlpha = static_cast<int16_t>(backgroundAlpha * node.GetGlobalAlpha());
    backgroundColor.SetAlpha(finalBackgroundAlpha);
    params.backgroundColor = static_cast<SkColor>(backgroundColor.AsArgbInt());

    const RectF absBounds = {
        node.GetTotalMatrix().getTranslateX(), node.GetTotalMatrix().getTranslateY(),
        property.GetBoundsWidth(), property.GetBoundsHeight()};
    RectF localBounds = {0.0f, 0.0f, absBounds.GetWidth(), absBounds.GetHeight()};

    // calculate clipRect and clipRRect(if has cornerRadius) for canvas.
    CalculateSurfaceNodeClipRects(node, absBounds, localBounds, inLocalCoordinate, params);

    // inLocalCoordinate: reset the translate to (0, 0).
    // else: use node's total matrix.
    if (inLocalCoordinate) {
        params.matrix = SkMatrix::I();
    } else {
        params.matrix = node.GetTotalMatrix();
    }

    // we can use only the bound's size (ignore its offset) now,
    // (the canvas was moved to the node's left-top point correctly).
    params.dstRect = SkRect::MakeWH(localBounds.GetWidth(), localBounds.GetHeight());

    const sptr<Surface>& surface = node.GetConsumer();
    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    if (isClipHole || surface == nullptr || buffer == nullptr) {
        return params;
    }

    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());

    RSBaseRenderUtil::FlipMatrix(node, params);
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(node, localBounds, params);
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
        params.clipRect = SkRect::MakeWH(localBounds.GetWidth(), localBounds.GetHeight());
        params.clipRRect = RRect(localBounds, params.cornerRadius);
    } else {
        // in logical screen's coordinate system.
        const auto& clipRect = node.GetDstRect();
        params.clipRect = SkRect::MakeXYWH(
            clipRect.GetLeft(), clipRect.GetTop(), clipRect.GetWidth(), clipRect.GetHeight());
        params.clipRRect = RRect(absBounds, params.cornerRadius);
    }
}
} // namespace Rosen
} // namespace OHOS

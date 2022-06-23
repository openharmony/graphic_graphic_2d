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

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_base_render_util.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "property/rs_transition_properties.h"
#include "render/rs_blur_filter.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
bool RSDividedRenderUtil::enableClient = false;

bool RSDividedRenderUtil::IsNeedClient(RSSurfaceRenderNode* node)
{
    if (enableClient) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client");
        return true;
    }
    if (node == nullptr) {
        RS_LOGE("RSDividedRenderUtil::IsNeedClient node is empty");
        return false;
    }
    auto filter = std::static_pointer_cast<RSBlurFilter>(node->GetRenderProperties().GetBackgroundFilter());
    if (filter != nullptr && filter->GetBlurRadiusX() > 0 && filter->GetBlurRadiusY() > 0) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need filter");
        return true;
    }
    auto transitionProperties = node->GetAnimationManager().GetTransitionProperties();
    if (!transitionProperties) {
        return false;
    }
    SkMatrix matrix = transitionProperties->GetRotate();
    float value[9];
    matrix.get9(value);
    if (SkMatrix::kMSkewX < 0 || SkMatrix::kMSkewX >= 9 || // 9 is the upper bound
        SkMatrix::kMScaleX < 0 || SkMatrix::kMScaleX >= 9) { // 9 is the upper bound
        RS_LOGE("RSDividedRenderUtil:: The value of kMSkewX or kMScaleX is illegal");
        return false;
    } else {
        float rAngle = -round(atan2(value[SkMatrix::kMSkewX], value[SkMatrix::kMScaleX]) * (180 / PI));
        bool isNeedClient = rAngle > 0;
        if (isNeedClient) {
            RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need animation rotate");
        }
        return isNeedClient;
    }
}

void RSDividedRenderUtil::DealAnimation(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node, BufferDrawParam& params,
    const Vector2f& center)
{
    auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    RSPropertiesPainter::DrawTransitionProperties(transitionProperties, center, canvas);
    const RSProperties& property = node.GetRenderProperties();
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, params.srcRect.width(), params.srcRect.height());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, skRectPtr);
    }
}

SkMatrix RSDividedRenderUtil::GetCanvasTransform(const RSSurfaceRenderNode& node, const SkMatrix& canvasMatrix,
    ScreenRotation rotation, SkRect clipRect)
{
    SkMatrix transform = canvasMatrix;
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        return transform;
    }
    sptr<Surface> surface = node.GetConsumer();
    if (surface == nullptr) {
        return transform;
    }

    const auto geoAbsRect = RectI(clipRect.left(), clipRect.top(), clipRect.width(), clipRect.height());
    transform.preTranslate(geoAbsRect.left_, geoAbsRect.top_);
    switch (rotation) {
        case ScreenRotation::ROTATION_90: {
            switch (surface->GetTransform()) {
                case TransformType::ROTATE_90: {
                    transform.preTranslate(geoAbsRect.height_, 0);
                    transform.preRotate(-90); // rotate 90 degrees anti-clockwise at last.
                    break;
                }
                case TransformType::ROTATE_180: {
                    transform.preTranslate(geoAbsRect.height_, -geoAbsRect.width_);
                    transform.preRotate(180); // rotate 180 degrees anti-clockwise at last.
                    break;
                }
                case TransformType::ROTATE_270: {
                    transform.preTranslate(0, -geoAbsRect.width_);
                    transform.preRotate(-270); // rotate 270 degrees anti-clockwise at last.
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case ScreenRotation::ROTATION_180: {
            switch (surface->GetTransform()) {
                case TransformType::ROTATE_90: {
                    transform.preTranslate(0, -geoAbsRect.height_);
                    transform.preRotate(-90); // rotate 90 degrees anti-clockwise at last.
                    break;
                }
                case TransformType::ROTATE_180: {
                    transform.preTranslate(-geoAbsRect.width_, -geoAbsRect.height_);
                    transform.preRotate(180); // rotate 180 degrees anti-clockwise at last.
                    break;
                }
                case TransformType::ROTATE_270: {
                    transform.preTranslate(-geoAbsRect.width_, 0);
                    transform.preRotate(-270); // rotate 270 degrees anti-clockwise at last.
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case ScreenRotation::ROTATION_270: {
            switch (surface->GetTransform()) {
                case TransformType::ROTATE_90: {
                    transform.preTranslate(-geoAbsRect.height_, 0);
                    transform.preRotate(-90); // rotate 90 degrees anti-clockwise at last.
                    break;
                }
                case TransformType::ROTATE_180: {
                    transform.preTranslate(-geoAbsRect.height_, geoAbsRect.width_);
                    transform.preRotate(180); // rotate 180 degrees anti-clockwise at last.
                    break;
                }
                case TransformType::ROTATE_270: {
                    transform.preTranslate(0, geoAbsRect.width_);
                    transform.preRotate(-270); // rotate 270 degrees anti-clockwise at last.
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default: {
            transform = node.GetTotalMatrix();
            switch (surface->GetTransform()) {
                case TransformType::ROTATE_90: {
                    transform.preTranslate(0, geoAbsRect.height_);
                    transform.preRotate(-90); // rotate 90 degrees anti-clockwise at last.
                    break;
                }
                case TransformType::ROTATE_180: {
                    transform.preTranslate(geoAbsRect.width_, geoAbsRect.height_);
                    transform.preRotate(180); // rotate 180 degrees anti-clockwise at last.
                    break;
                }
                case TransformType::ROTATE_270: {
                    transform.preTranslate(geoAbsRect.width_, 0);
                    transform.preRotate(-270); // rotate 270 degrees anti-clockwise at last.
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }

    return transform;
}

BufferDrawParam RSDividedRenderUtil::CreateBufferDrawParam(RSSurfaceRenderNode& node, SkMatrix canvasMatrix,
    ScreenRotation rotation, SkPaint paint)
{
    const RSProperties& property = node.GetRenderProperties();
    SkRect dstRect = SkRect::MakeXYWH(node.GetDstRect().left_, node.GetDstRect().top_,
        node.GetDstRect().width_, node.GetDstRect().height_);
    BufferDrawParam params;
    auto buffer = node.GetBuffer();
    sptr<Surface> surface = node.GetConsumer();
    if (!buffer || !surface) {
        return params;
    }
    params.buffer = buffer;
    params.matrix = GetCanvasTransform(node, canvasMatrix, rotation, dstRect);
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = SkRect::MakeXYWH(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    const auto surfaceTransform = surface->GetTransform();
    if (surfaceTransform == TransformType::ROTATE_90 || surfaceTransform == TransformType::ROTATE_270) {
        params.dstRect = SkRect::MakeXYWH(0, 0, property.GetBoundsHeight(), property.GetBoundsWidth());
    } else {
        params.dstRect = SkRect::MakeXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
    }
    params.clipRect = dstRect;
    params.paint = paint;
    params.cornerRadius = property.GetCornerRadius();
    params.isNeedClip = property.GetClipToFrame();
    return params;
}

void SetPropertiesForCanvas(RSPaintFilterCanvas& canvas, BufferDrawParam& bufferDrawParam,
    RSDividedRenderUtil::CanvasPostProcess process)
{
    canvas.save();
    if (bufferDrawParam.isNeedClip) {
        SkRect clipRect = bufferDrawParam.clipRect;
        if (!bufferDrawParam.cornerRadius.IsZero()) {
            RectF rect(clipRect.left(), clipRect.top(), clipRect.width(), clipRect.height());
            RRect rrect = RRect(rect, bufferDrawParam.cornerRadius);
            canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(rrect), true);
        } else {
            canvas.clipRect(bufferDrawParam.clipRect);
        }
    }
    canvas.setMatrix(bufferDrawParam.matrix);
    if (process) {
        process(canvas, bufferDrawParam);
    }
}

void RSDividedRenderUtil::DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& bufferDrawParam,
    CanvasPostProcess process)
{
    SkBitmap bitmap;
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(bufferDrawParam.buffer, newBuffer, bufferDrawParam.targetColorGamut,
        bitmap)) {
        RS_LOGE("RSDividedRenderUtil::DrawBuffer: create bitmap failed.");
        return;
    }
    SetPropertiesForCanvas(canvas, bufferDrawParam, process);
    canvas.drawBitmapRect(bitmap, bufferDrawParam.srcRect, bufferDrawParam.dstRect, &(bufferDrawParam.paint));
    canvas.restore();
}

#ifdef RS_ENABLE_EGLIMAGE
void RSDividedRenderUtil::DrawImage(std::shared_ptr<RSEglImageManager> eglImageManager, GrContext* grContext,
    RSPaintFilterCanvas& canvas, BufferDrawParam& bufferDrawParam, CanvasPostProcess process)
{
    RS_TRACE_NAME("GpuClientComposition");
    RS_LOGI("RSDividedRenderUtil::DrawImage start");
    sk_sp<SkImage> image;
    if (!RSBaseRenderUtil::ConvertBufferToEglImage(bufferDrawParam.buffer, eglImageManager, grContext,
        bufferDrawParam.acquireFence, image)) {
        RS_LOGE("RSDividedRenderUtil::DrawImage ConvertBufferToEglImage failed");
        return;
    }

    SetPropertiesForCanvas(canvas, bufferDrawParam, process);
    canvas.drawImageRect(image, bufferDrawParam.srcRect, bufferDrawParam.dstRect, &(bufferDrawParam.paint));
    canvas.restore();
}
#endif // RS_ENABLE_EGLIMAGE

void RSDividedRenderUtil::InitEnableClient()
{
    if (access("/etc/enable_client", F_OK) == 0) {
        enableClient = true;
    } else {
        enableClient = false;
    }
}
} // namespace Rosen
} // namespace OHOS

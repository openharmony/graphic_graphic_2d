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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_BASE_RENDER_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_BASE_RENDER_UTIL_H

#include <vector>
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "include/core/SkColorFilter.h"

#include "screen_manager/rs_screen_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class RSTransactionData;
struct BufferDrawParam {
    sptr<OHOS::SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;

    SkMatrix matrix; // for moving canvas to layer(surface)'s leftTop point.
    SkRect srcRect; // surface's bufferSize
    SkRect dstRect; // surface's boundsSize

    Vector4f cornerRadius;
    RRect clipRRect;
    SkRect clipRect;

    bool isNeedClip = true;
    SkPaint paint;
    SkColor backgroundColor = SK_ColorTRANSPARENT;
    ColorGamut targetColorGamut = ColorGamut::COLOR_GAMUT_SRGB;

    bool useCPU = false;
    bool setColorFilter = true;
    std::vector<HDRMetaData> metaDatas = {}; // static meta datas for HDR10
    HDRMetaDataSet metaDataSet; // dynamic meta datas for HDR10+, HDR VIVID
};

enum class ColorFilterMode {
    INVERT_MODE = 0,
    PROTANOMALY_MODE,
    DEUTERANOMALY_MODE,
    TRITANOMALY_MODE,
    COLOR_FILTER_END,
};

class RSBaseRenderUtil {
public:
    static bool IsBufferValid(const sptr<SurfaceBuffer>& buffer);
    static BufferRequestConfig GetFrameBufferRequestConfig(const ScreenInfo& screenInfo, bool isPhysical = true);
    static BufferDrawParam CreateBufferDrawParam(
        const RSSurfaceRenderNode& node,
        bool inLocalCoordinate = false,
        bool isClipHole = false,
        bool forceCPU = false,
        bool setColorFilter = true);

    static SkMatrix GetSurfaceTransformMatrix(const RSSurfaceRenderNode& node, const RectF& bounds);
    static SkMatrix GetNodeGravityMatrix(
        const RSSurfaceRenderNode& node, const sptr<SurfaceBuffer>& buffer, const RectF& bounds);
    static void SetPropertiesForCanvas(RSPaintFilterCanvas& canvas, const BufferDrawParam& params);

    static void DropFrameProcess(RSSurfaceHandler& surfaceHandler);
    static bool ConsumeAndUpdateBuffer(RSSurfaceHandler& surfaceHandler);
    static bool ReleaseBuffer(RSSurfaceHandler& surfaceHandler);

    static std::unique_ptr<RSTransactionData> ParseTransactionData(MessageParcel& parcel);

    static bool ConvertBufferToBitmap(sptr<SurfaceBuffer> buffer, ColorGamut dstGamut, SkBitmap& bitmap,
        const std::vector<HDRMetaData>& metaDatas = {});
    /**
     * @brief Set the Color Filter Mode To Paint object
     *
     * @param colorFilterMode SkBlendMode applied to SKPaint
     * @param paint color matrix applied to SKPaint
     */
    static void SetColorFilterModeToPaint(ColorFilterMode colorFilterMode, SkPaint& paint);

private:
    static void CalculateSurfaceNodeClipRects(
        const RSSurfaceRenderNode& node,
        const RectF& absBounds,
        const RectF& localBounds,
        bool inLocalCoordinate,
        BufferDrawParam& params);
    static void DealWithSurfaceRotationAndGravity(
        const RSSurfaceRenderNode& node, RectF& bounds, BufferDrawParam& params);
    static bool CreateYuvToRGBABitMap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        SkBitmap& bitmap);
    static bool CreateNewColorGamutBitmap(sptr<OHOS::SurfaceBuffer> buffer, SkBitmap& bitmap,
        ColorGamut srcGamut, ColorGamut dstGamut, const std::vector<HDRMetaData>& metaDatas = {});
    static bool CreateBitmap(sptr<OHOS::SurfaceBuffer> buffer, SkBitmap& bitmap);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_BASE_RENDER_UTIL_H

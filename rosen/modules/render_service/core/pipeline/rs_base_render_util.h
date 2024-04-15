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
#include "image/bitmap.h"
#include "utils/matrix.h"
#include "utils/rect.h"
#include "draw/pen.h"

#include "screen_manager/rs_screen_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_composer_adapter.h"
#include "pixel_map.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class RSTransactionData;
#ifdef USE_VIDEO_PROCESSING_ENGINE
constexpr float DEFAULT_SCREEN_LIGHT_NITS = 500;
#endif
struct BufferDrawParam {
    sptr<OHOS::SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;

    Drawing::Matrix matrix; // for moving canvas to layer(surface)'s leftTop point.
    Drawing::Rect srcRect; // surface's bufferSize
    Drawing::Rect dstRect; // surface's boundsSize

    Vector4f cornerRadius;
    RRect clipRRect;

    Drawing::Rect clipRect;

    bool isNeedClip = true;
    Drawing::Brush paint;
    Drawing::ColorQuad backgroundColor = Drawing::Color::COLOR_TRANSPARENT;
    GraphicColorGamut targetColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;

    bool useCPU = false;
    bool isMirror = false;
    bool setColorFilter = true;
    bool useBilinearInterpolation = false;
    std::vector<GraphicHDRMetaData> metaDatas = {}; // static meta datas for HDR10
    GraphicHDRMetaDataSet metaDataSet; // dynamic meta datas for HDR10+, HDR VIVID
    uint32_t threadIndex = UNI_MAIN_THREAD_INDEX; // use to decide eglimage unmap thread index
#ifdef USE_VIDEO_PROCESSING_ENGINE
    float screenBrightnessNits = DEFAULT_SCREEN_LIGHT_NITS;
#endif
};

using WriteToPngParam = struct {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t bitDepth;
    const uint8_t *data;
};

enum class ColorFilterMode {
    INVERT_COLOR_DISABLE_MODE = 0,
    INVERT_COLOR_ENABLE_MODE = 1,
    DALTONIZATION_PROTANOMALY_MODE = 2,
    DALTONIZATION_DEUTERANOMALY_MODE = 4,
    DALTONIZATION_TRITANOMALY_MODE = 8,
    INVERT_DALTONIZATION_PROTANOMALY_MODE = 3,
    INVERT_DALTONIZATION_DEUTERANOMALY_MODE = 5,
    INVERT_DALTONIZATION_TRITANOMALY_MODE = 9,
    DALTONIZATION_NORMAL_MODE = 16,
    COLOR_FILTER_END = 32,
};

class RSBaseRenderUtil {
public:
    static bool IsNeedClient(RSRenderNode& node, const ComposeInfo& info);
    static void SetNeedClient(bool flag);
    static bool IsBufferValid(const sptr<SurfaceBuffer>& buffer);
    static BufferRequestConfig GetFrameBufferRequestConfig(const ScreenInfo& screenInfo, bool isPhysical = true,
        GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB,
        GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888);

    static Drawing::Matrix GetSurfaceTransformMatrix(GraphicTransformType rotationTransform, const RectF& bounds);
    static Drawing::Matrix GetGravityMatrix(Gravity gravity, const sptr<SurfaceBuffer>& buffer, const RectF& bounds);
    static void SetPropertiesForCanvas(RSPaintFilterCanvas& canvas, const BufferDrawParam& params);

    static GSError DropFrameProcess(RSSurfaceHandler& node);
    static Rect MergeBufferDamages(const std::vector<Rect>& damages);
    static bool ConsumeAndUpdateBuffer(RSSurfaceHandler& surfaceHandler);
    static bool ReleaseBuffer(RSSurfaceHandler& surfaceHandler);

    static std::unique_ptr<RSTransactionData> ParseTransactionData(MessageParcel& parcel);

    static bool ConvertBufferToBitmap(sptr<SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        GraphicColorGamut dstGamut, Drawing::Bitmap& bitmap, const std::vector<GraphicHDRMetaData>& metaDatas = {});
    /**
     * @brief Set the Color Filter Mode To Paint object
     *
     * @param colorFilterMode SkBlendMode applied to SKPaint
     * @param paint color matrix applied to SKPaint
     */
    static void SetColorFilterModeToPaint(ColorFilterMode colorFilterMode, Drawing::Brush& paint);
    static bool IsColorFilterModeValid(ColorFilterMode mode);

    static bool WriteSurfaceRenderNodeToPng(const RSSurfaceRenderNode& node);
    static bool WriteCacheRenderNodeToPng(const RSRenderNode& node);
    static bool WriteSurfaceBufferToPng(sptr<SurfaceBuffer>& buffer, uint64_t id = 0);

    static bool WritePixelMapToPng(Media::PixelMap& pixelMap);
    static void DealWithSurfaceRotationAndGravity(GraphicTransformType transform, Gravity gravity,
        RectF& localBounds, BufferDrawParam& params);
    static void FlipMatrix(GraphicTransformType transform, BufferDrawParam& params);

    // GraphicTransformType has two attributes: rotation and flip, it take out one of the attributes separately
    static GraphicTransformType GetRotateTransform(GraphicTransformType transform);
    static GraphicTransformType GetFlipTransform(GraphicTransformType transform);

    // GraphicTransformType from hdi layer info is clockwise, for surface and surface node is anti-clockwise
    // need conversion here
    static GraphicTransformType ClockwiseToAntiClockwiseTransform(GraphicTransformType transform);
    static int RotateEnumToInt(GraphicTransformType rotation);
    static GraphicTransformType RotateEnumToInt(int angle,
        GraphicTransformType flip = GraphicTransformType::GRAPHIC_ROTATE_NONE);
    static bool IsForceClient();
<<<<<<< HEAD
    static bool WriteCacheImageRenderNodeToPng(std::shared_ptr<Drawing::Surface> surface, std::string debugInfo);
    static bool WriteCacheImageRenderNodeToPng(std::shared_ptr<Drawing::Image> image, std::string debugInfo);
=======
    static Rect MergeBufferDamages(const std::vector<Rect>& damages);

>>>>>>> origin/master
private:
    static bool CreateYuvToRGBABitMap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        Drawing::Bitmap& bitmap);
    static bool CreateNewColorGamutBitmap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        Drawing::Bitmap& bitmap, GraphicColorGamut srcGamut, GraphicColorGamut dstGamut,
        const std::vector<GraphicHDRMetaData>& metaDatas = {});
    static bool CreateBitmap(sptr<OHOS::SurfaceBuffer> buffer, Drawing::Bitmap& bitmap);
    static bool WriteToPng(const std::string &filename, const WriteToPngParam &param);

    static bool enableClient;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_BASE_RENDER_UTIL_H

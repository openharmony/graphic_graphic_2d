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
#include <atomic>
#include "image/bitmap.h"
#include "metadata_helper.h"
#include "params/rs_surface_render_params.h"
#include "utils/matrix.h"
#include "utils/rect.h"
#include "draw/pen.h"

#include "screen_manager/rs_screen_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "pixel_map.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
struct ComposeInfo {
    GraphicIRect srcRect;
    GraphicIRect dstRect;
    GraphicIRect boundRect;
    GraphicIRect visibleRect;
    std::vector<GraphicIRect> dirtyRects;
    GraphicMatrix matrix;
    int32_t gravity { 0 };
    int32_t zOrder { 0 };
    GraphicLayerAlpha alpha;
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> fence = SyncFence::InvalidFence();
    GraphicBlendType blendType = GraphicBlendType::GRAPHIC_BLEND_NONE;
    bool needClient = false;
    float sdrNit { 0.0f };
    float displayNit { 0.0f };
    float brightnessRatio { 0.0f };
    std::vector<float> layerLinearMatrix;
};
#ifdef RS_ENABLE_GPU
class RSSurfaceRenderParams;
#endif
class RSTransactionData;
#ifdef USE_VIDEO_PROCESSING_ENGINE
constexpr float DEFAULT_SCREEN_LIGHT_NITS = 500;
constexpr float DEFAULT_BRIGHTNESS_RATIO = 1.0f;
#endif
constexpr uint64_t CONSUME_DIRECTLY = 0;
struct BufferDrawParam {
    sptr<OHOS::SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();

    Drawing::Matrix matrix; // for moving canvas to layer(surface)'s leftTop point.
    Drawing::Rect srcRect; // surface's bufferSize
    Drawing::Rect dstRect; // surface's boundsSize
    bool hasCropMetadata = false;
    HDI::Display::Graphic::Common::V1_0::BufferHandleMetaRegion metaRegion;

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
    ScreenId screenId = INVALID_SCREEN_ID;
    std::vector<GraphicHDRMetaData> metaDatas = {}; // static meta datas for HDR10
    GraphicHDRMetaDataSet metaDataSet; // dynamic meta datas for HDR10+, HDR VIVID
    uint32_t threadIndex = UNI_MAIN_THREAD_INDEX; // use to decide eglimage unmap thread index
#ifdef USE_VIDEO_PROCESSING_ENGINE
    float sdrNits = DEFAULT_SCREEN_LIGHT_NITS;
    float tmoNits = DEFAULT_SCREEN_LIGHT_NITS;
    float displayNits = DEFAULT_SCREEN_LIGHT_NITS;
    float brightnessRatio = DEFAULT_BRIGHTNESS_RATIO;
    std::vector<float> layerLinearMatrix;
    bool isHdrRedraw = false;
    bool isHdrToSdr = false;
    bool hasMetadata = false; // SDR has metadata
#endif
    bool colorFollow = false;
    bool preRotation = false;
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

constexpr int32_t ROUND_ANGLE = 360;

class RSBaseRenderUtil {
public:
    static bool IsNeedClient(RSRenderNode& node, const ComposeInfo& info);
    static void SetNeedClient(bool flag);
    static bool IsBufferValid(const sptr<SurfaceBuffer>& buffer);
    static BufferRequestConfig GetFrameBufferRequestConfig(const ScreenInfo& screenInfo, bool isProtected = false,
        GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB,
        GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888);

    static GraphicTransformType GetSurfaceBufferTransformType(
        const sptr<IConsumerSurface>& consumer, const sptr<SurfaceBuffer>& buffer);
    static Drawing::Matrix GetSurfaceTransformMatrix(GraphicTransformType rotationTransform, const RectF &bounds,
        const RectF &bufferBounds = {0.0f, 0.0f, 0.0f, 0.0f});
    static Drawing::Matrix GetSurfaceTransformMatrixForRotationFixed(GraphicTransformType rotationTransform,
        const RectF &bounds, const RectF &bufferBounds = {0.0f, 0.0f, 0.0f, 0.0f}, Gravity gravity = Gravity::RESIZE);
    static Drawing::Matrix GetGravityMatrix(Gravity gravity, const RectF& bufferSize, const RectF& bounds);

    static void SetPropertiesForCanvas(RSPaintFilterCanvas& canvas, const BufferDrawParam& params);
    static Drawing::ColorType GetColorTypeFromBufferFormat(int32_t pixelFmt);
    static Drawing::BitmapFormat GenerateDrawingBitmapFormat(const sptr<OHOS::SurfaceBuffer>& buffer);

    static GSError DropFrameProcess(RSSurfaceHandler& surfaceHandler, uint64_t presentWhen = 0,
        bool adaptiveDVSyncEnable = false);
    static bool ConsumeAndUpdateBuffer(RSSurfaceHandler& surfaceHandler,
        uint64_t presentWhen = CONSUME_DIRECTLY, bool dropFrameByPidEnable = false, bool adaptiveDVSyncEnable = false,
        bool needConsume = true, std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr);
    static bool ReleaseBuffer(RSSurfaceHandler& surfaceHandler);

    static std::unique_ptr<RSTransactionData> ParseTransactionData(MessageParcel& parcel, uint32_t parcelNumber);

    static bool ConvertBufferToBitmap(sptr<SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        GraphicColorGamut dstGamut, Drawing::Bitmap& bitmap, const std::vector<GraphicHDRMetaData>& metaDatas = {});
    /**
     * @brief Set the Color Filter Mode To Paint object
     *
     * @param colorFilterMode SkBlendMode applied to SKPaint
     * @param paint color matrix applied to SKPaint
     * @param brightnessRatio hdr brightness ratio
     */
    static void SetColorFilterModeToPaint(ColorFilterMode colorFilterMode, Drawing::Brush& paint,
        float hdrBrightnessRatio = 1.f);
    static bool IsColorFilterModeValid(ColorFilterMode mode);

    static bool WriteSurfaceRenderNodeToPng(const RSSurfaceRenderNode& node);
    static bool WriteSurfaceBufferToPng(sptr<SurfaceBuffer>& buffer, uint64_t id = 0);

    static bool WritePixelMapToPng(Media::PixelMap& pixelMap);
    static int32_t GetScreenRotationOffset(RSSurfaceRenderParams* nodeParams);
#ifdef RS_ENABLE_GPU
    static void DealWithSurfaceRotationAndGravity(GraphicTransformType transform, Gravity gravity,
        RectF& localBounds, BufferDrawParam& params, RSSurfaceRenderParams* nodeParams = nullptr);
#endif
    static void FlipMatrix(GraphicTransformType transform, BufferDrawParam& params);

    // GraphicTransformType has two attributes: rotation and flip, it take out one of the attributes separately
    static GraphicTransformType GetRotateTransform(GraphicTransformType transform);
    static GraphicTransformType GetFlipTransform(GraphicTransformType transform);

    static int RotateEnumToInt(ScreenRotation rotation);
    static int RotateEnumToInt(GraphicTransformType rotation);
    static GraphicTransformType RotateEnumToInt(int angle,
        GraphicTransformType flip = GraphicTransformType::GRAPHIC_ROTATE_NONE);
    static Rect MergeBufferDamages(const std::vector<Rect>& damages);
    static void MergeBufferDamages(Rect& surfaceDamage, const std::vector<Rect>& damages);
    static bool WriteCacheImageRenderNodeToPng(std::shared_ptr<Drawing::Surface> surface, std::string debugInfo);
    static bool WriteCacheImageRenderNodeToPng(std::shared_ptr<Drawing::Image> image, std::string debugInfo);
    static bool WriteCacheImageRenderNodeToPng(std::shared_ptr<Drawing::Bitmap> bitmap, std::string debugInfo);

    static int GetAccumulatedBufferCount();
    static void IncAcquiredBufferCount();
    static void DecAcquiredBufferCount();
    static pid_t GetLastSendingPid();
    static bool PortraitAngle(int angle);

private:
    static bool CreateYuvToRGBABitMap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        Drawing::Bitmap& bitmap);
    static bool CreateNewColorGamutBitmap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        Drawing::Bitmap& bitmap, GraphicColorGamut srcGamut, GraphicColorGamut dstGamut,
        const std::vector<GraphicHDRMetaData>& metaDatas = {});
    static bool CreateBitmap(sptr<OHOS::SurfaceBuffer> buffer, Drawing::Bitmap& bitmap);
    static bool WriteToPng(const std::string &filename, const WriteToPngParam &param);
    static ScreenId GetScreenIdFromSurfaceRenderParams(RSSurfaceRenderParams* nodeParams);

    static bool enableClient;

    static inline std::atomic<int> acquiredBufferCount_ = 0;
    static pid_t lastSendingPid_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_BASE_RENDER_UTIL_H

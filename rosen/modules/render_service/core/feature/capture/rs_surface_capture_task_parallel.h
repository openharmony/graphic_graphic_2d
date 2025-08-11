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

#ifndef RS_SURFACE_CAPTURE_TASK_PARALLEL
#define RS_SURFACE_CAPTURE_TASK_PARALLEL
#define EGL_EGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include "drawable/rs_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "pixel_map.h"
#include "system/rs_system_parameters.h"

namespace OHOS {
namespace Rosen {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
class DmaMem {
public:
    DmaMem() = default;
    ~DmaMem();
    sptr<SurfaceBuffer> DmaMemAlloc(Drawing::ImageInfo &dstInfo, const std::unique_ptr<Media::PixelMap>& pixelMap);
    std::shared_ptr<Drawing::Surface> GetSurfaceFromSurfaceBuffer(sptr<SurfaceBuffer> surfaceBuffer,
        std::shared_ptr<Drawing::GPUContext> gpuContext);
    void ReleaseDmaMemory();
private:
    OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
};
#endif

class RSSurfaceCaptureTaskParallel {
public:
    explicit RSSurfaceCaptureTaskParallel(NodeId nodeId, const RSSurfaceCaptureConfig& captureConfig)
        : nodeId_(nodeId), captureConfig_(captureConfig) {}
    ~RSSurfaceCaptureTaskParallel() = default;

    // Confirm whether the node is occlusive which should apply modifiers
    static void CheckModifiers(NodeId id, bool useCurWindow);
    // Do capture pipeline task
    static void Capture(sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureParam& captureParam);

#ifdef RS_ENABLE_UNI_RENDER
    static std::function<void()> CreateSurfaceSyncCopyTask(std::shared_ptr<Drawing::Surface> surface,
        std::unique_ptr<Media::PixelMap> pixelMap, NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        sptr<RSISurfaceCaptureCallback> callback, int32_t rotation = 0);
    static std::function<void()> CreateSurfaceSyncCopyTaskWithDoublePixelMap(
        std::shared_ptr<Drawing::Surface> surface, std::unique_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Drawing::Surface> surfaceHDR, std::unique_ptr<Media::PixelMap> pixelMapHDR, NodeId id,
        const RSSurfaceCaptureConfig& captureConfig, sptr<RSISurfaceCaptureCallback> callback, int32_t rotation);
    static bool PixelMapCopy(std::unique_ptr<Media::PixelMap>& pixelmap,
        std::shared_ptr<Drawing::ColorSpace> colorSpace, const Drawing::BackendTexture& backendTexture,
        Drawing::ColorType colorType, bool useDma, int32_t rotation);
#endif

    bool CreateResources();

    bool Run(sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureParam& captureParam);
    bool RunHDR(sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureParam& captureParam);
    bool DrawHDRSurfaceContent(
        std::shared_ptr<Drawing::Surface> surface, bool isOnHDR, const RSSurfaceCaptureParam& captureParam);
    bool UseScreenShotWithHDR() const
    {
        return useScreenShotWithHDR_;
    }

    void SetUseScreenShotWithHDR(bool enableHdrCapture)
    {
        useScreenShotWithHDR_ = enableHdrCapture && displayNodeDrawable_ && pixelMapHDR_;
    }

    static void ClearCacheImageByFreeze(NodeId id);

private:
    std::shared_ptr<Drawing::Surface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap);

    std::unique_ptr<Media::PixelMap> CreatePixelMapBySurfaceNode(std::shared_ptr<RSSurfaceRenderNode> node,
        bool isF16Capture = false);

    std::unique_ptr<Media::PixelMap> CreatePixelMapByDisplayNode(std::shared_ptr<RSLogicalDisplayRenderNode> node,
        bool isHDRCapture = false);
    
    bool CreateResourcesForClientPixelMap(const std::shared_ptr<RSRenderNode>& node);

    void AddBlur(RSPaintFilterCanvas& canvas, const std::shared_ptr<Drawing::Surface>& surface, float blurRadius);

    void SetupGpuContext();

    int32_t CalPixelMapRotation();

    void CaptureDisplayNode(DrawableV2::RSRenderNodeDrawable& displayNodeDrawable, RSPaintFilterCanvas& canvas,
        const RSSurfaceCaptureParam& captureParam, RSPaintFilterCanvas::ScreenshotType type);

    std::unique_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::unique_ptr<Media::PixelMap> pixelMapHDR_ = nullptr;
    std::shared_ptr<DrawableV2::RSRenderNodeDrawable> surfaceNodeDrawable_ = nullptr;
    std::shared_ptr<DrawableV2::RSRenderNodeDrawable> displayNodeDrawable_ = nullptr;
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode_ = nullptr;
    NodeId nodeId_;
    ScreenId screenId_ = INVALID_SCREEN_ID;
    bool useScreenShotWithHDR_ = false;
    RSSurfaceCaptureConfig captureConfig_;
    ScreenRotation screenCorrection_ = ScreenRotation::ROTATION_0;
    ScreenRotation screenRotation_ = ScreenRotation::ROTATION_0;
    float boundsX_ = 0;
    float boundsY_ = 0;
    int32_t finalRotationAngle_ = RS_ROTATION_0;
    // only used for RSUniRenderThread
    std::shared_ptr<Drawing::GPUContext> gpuContext_ = nullptr;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_SURFACE_CAPTURE_TASK_PARALLEL

/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef SKIA_SURFACE_H
#define SKIA_SURFACE_H

#include "include/core/SkSurface.h"
#include "include/core/SkImage.h"

#include "draw/surface.h"
#include "impl_interface/surface_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaSurface : public SurfaceImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaSurface();
    ~SkiaSurface() override;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    bool Bind(const Bitmap& bitmap) override;
#ifdef RS_ENABLE_GPU
    bool Bind(const Image& image) override;
    bool Bind(const FrameBuffer& frameBuffer) override;
    
#ifdef RS_ENABLE_VK
    static std::shared_ptr<Surface> MakeFromBackendRenderTarget(GPUContext* gpuContext, const TextureInfo& info,
        TextureOrigin origin, ColorType colorType, std::shared_ptr<ColorSpace> colorSpace,
        void (*deleteVkImage)(void *), void* cleanHelper);
#endif
    static std::shared_ptr<Surface> MakeFromBackendTexture(GPUContext* gpuContext, const TextureInfo& info,
        TextureOrigin origin, int sampleCnt, ColorType colorType,
        std::shared_ptr<ColorSpace> colorSpace, void (*deleteVkImage)(void *), void* cleanHelper);
    static std::shared_ptr<Surface> MakeRenderTarget(GPUContext* gpuContext, bool budgeted, const ImageInfo& imageInfo);
#endif
    static std::shared_ptr<Surface> MakeRaster(const ImageInfo& imageInfo);
    static std::shared_ptr<Surface> MakeRasterDirect(const ImageInfo& imageInfo, void* pixels, size_t rowBytes);
    static std::shared_ptr<Surface> MakeRasterN32Premul(int32_t width, int32_t height);

    std::shared_ptr<Canvas> GetCanvas() const override;
    std::shared_ptr<Image> GetImageSnapshot() const override;
    std::shared_ptr<Image> GetImageSnapshot(const RectI& bounds, bool allowRefCache = true) const override;
    std::shared_ptr<Surface> MakeSurface(int width, int height) const override;
    std::shared_ptr<Surface> MakeSurface(const ImageInfo& imageInfo) const override;
    BackendTexture GetBackendTexture(BackendAccess access) const override;
    
    void SetSkSurface(const sk_sp<SkSurface>& skSurface);
    void FlushAndSubmit(bool syncCpu) override;
    void Flush(FlushInfo *drawingflushInfo = nullptr) override;
#ifdef RS_ENABLE_GL
    void Wait(const std::vector<GrGLsync>& syncs) override;
#endif
#ifdef RS_ENABLE_VK
    void Wait(int32_t time, const VkSemaphore& semaphore) override;
    void SetDrawingArea(const std::vector<RectI>& rects) override;
    void ClearDrawingArea() override;
#endif
    void SetHeadroom(float headroom) override;
    float GetHeadroom() const override;
    sk_sp<SkSurface> GetSkSurface() const;
    int Width() const override;
    int Height() const override;
private:
    void PostSkSurfaceToTargetThread();
    sk_sp<SkSurface> skSurface_ = nullptr;
    sk_sp<SkImage> skImage_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_SURFACE_H

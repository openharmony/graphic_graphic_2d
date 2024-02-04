/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "draw/surface.h"

#include "impl_factory.h"
#include "static_factory.h"
#include "utils/log.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Surface::Surface() : impl_(ImplFactory::CreateSurfaceImpl()), cachedCanvas_(nullptr) {}

bool Surface::Bind(const Bitmap& bitmap)
{
    return impl_->Bind(bitmap);
}

#ifdef ACE_ENABLE_GPU
bool Surface::Bind(const Image& image)
{
    return impl_->Bind(image);
}

bool Surface::Bind(const FrameBuffer& frameBuffer)
{
    return impl_->Bind(frameBuffer);
}

#ifdef RS_ENABLE_VK
std::shared_ptr<Surface> Surface::MakeFromBackendRenderTarget(GPUContext* gpuContext, const TextureInfo& info,
    TextureOrigin origin, ColorType colorType, std::shared_ptr<ColorSpace> colorSpace,
    void (*deleteFunc)(void*), void* cleanupHelper)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    return StaticFactory::MakeFromBackendRenderTarget(gpuContext, info, origin,
        colorType, colorSpace, deleteFunc, cleanupHelper);
}

std::shared_ptr<Surface> Surface::MakeFromBackendTexture(GPUContext* gpuContext, const TextureInfo& info,
    TextureOrigin origin, int sampleCnt, ColorType colorType,
    std::shared_ptr<ColorSpace> colorSpace, void (*deleteVkImage)(void *), void* cleanHelper)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    return StaticFactory::MakeFromBackendTexture(gpuContext, info, origin, sampleCnt, colorType,
        colorSpace, deleteVkImage, cleanHelper);
}
#endif

std::shared_ptr<Surface> Surface::MakeRenderTarget(GPUContext* gpuContext, bool budgeted, const ImageInfo& imageInfo)
{
    return StaticFactory::MakeRenderTarget(gpuContext, budgeted, imageInfo);
}
#endif

std::shared_ptr<Surface> Surface::MakeRaster(const ImageInfo& imageInfo)
{
    return StaticFactory::MakeRaster(imageInfo);
}

std::shared_ptr<Surface> Surface::MakeRasterDirect(const ImageInfo& imageInfo, void* pixels, size_t rowBytes)
{
    return StaticFactory::MakeRasterDirect(imageInfo, pixels, rowBytes);
}

std::shared_ptr<Surface> Surface::MakeRasterN32Premul(int32_t width, int32_t height)
{
    return StaticFactory::MakeRasterN32Premul(width, height);
}

std::shared_ptr<Canvas> Surface::GetCanvas()
{
    if (cachedCanvas_ == nullptr) {
        cachedCanvas_ = impl_->GetCanvas();
    }
    return cachedCanvas_;
}

std::shared_ptr<Image> Surface::GetImageSnapshot() const
{
    return impl_->GetImageSnapshot();
}

BackendTexture Surface::GetBackendTexture(BackendAccess access) const
{
    return impl_->GetBackendTexture(access);
}

std::shared_ptr<Image> Surface::GetImageSnapshot(const RectI& bounds) const
{
    return impl_->GetImageSnapshot(bounds);
}

std::shared_ptr<Surface> Surface::MakeSurface(int width, int height) const
{
    return impl_->MakeSurface(width, height);
}

ImageInfo Surface::GetImageInfo()
{
    std::shared_ptr<Canvas> canvas = GetCanvas();
    if (!canvas) {
        LOGD("canvas nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return ImageInfo{};
    }
    return canvas->GetImageInfo();
}

void Surface::FlushAndSubmit(bool syncCpu)
{
    impl_->FlushAndSubmit(syncCpu);
}

void Surface::Flush(FlushInfo *drawingflushInfo)
{
    if (!impl_) {
        LOGD("surfaceImpl Flush failed impl nullptr");
        return;
    }
    impl_->Flush(drawingflushInfo);
}

#ifdef RS_ENABLE_VK
void Surface::Wait(int32_t time, const VkSemaphore& semaphore)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    if (!impl_) {
        return;
    }
    impl_->Wait(time, semaphore);
}

void Surface::SetDrawingArea(const std::vector<RectI>& rects)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    if (!impl_) {
        LOGD("surfaceImpl SetDrawingArea failed impl nullptr");
        return;
    }
    impl_->SetDrawingArea(rects);
}

void Surface::ClearDrawingArea()
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    if (!impl_) {
        LOGD("surfaceImpl ClearDrawingArea failed impl nullptr");
        return;
    }
    impl_->ClearDrawingArea();
}
#endif

int Surface::Width() const
{
    return impl_->Width();
}

int Surface::Height() const
{
    return impl_->Height();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

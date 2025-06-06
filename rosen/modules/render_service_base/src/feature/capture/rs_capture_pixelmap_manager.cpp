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

#include "feature/capture/rs_capture_pixelmap_manager.h"
#include "draw/color.h"
#include "image/bitmap.h"
#include "platform/common/rs_log.h"
#include <memory>
#include <mutex>
#include "rs_trace.h"
#ifdef ROSEN_OHOS
#include <sys/mman.h>
#include "parameters.h"
#endif

#if defined (RS_ENABLE_UNI_RENDER) && defined (ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "draw/surface.h"
#include "surface_buffer.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/native_buffer_utils.h"
#endif

namespace OHOS {
namespace Rosen {
CapturePixelMapMemFunHandle RSCapturePixelMapManager::funcHandle_;
CapturePixelMapCheckFunHandle RSCapturePixelMapManager::checkHandle_;

/**
 * @brief Define pixelMap Attach Mem
**/
void RSCapturePixelMapManager::LoadCheckFunc()
{
    checkHandle_[uint32UniRenderDisableType_][uint32DefaultCaptureType_] = [](const Drawing::Rect& areaRect,
        const RSSurfaceCaptureConfig& captureConfig) {
        return true;
    };

    checkHandle_[uint32UniRenderDisableType_][uint32UiCaptureType_] = [](const Drawing::Rect& areaRect,
        const RSSurfaceCaptureConfig& captureConfig) {
        return true;
    };

    checkHandle_[uint32UniRenderEnableType_][uint32DefaultCaptureType_] = [](const Drawing::Rect& areaRect,
        const RSSurfaceCaptureConfig& captureConfig) {
        if (captureConfig.scaleX > 1.f || captureConfig.scaleY > 1.f) {
            return false;
        }
        return true;
    };

    checkHandle_[uint32UniRenderEnableType_][uint32UiCaptureType_] = [](const Drawing::Rect& areaRect,
        const RSSurfaceCaptureConfig& captureConfig) {
#ifdef RS_ENABLE_VK
        int32_t width = ceil(areaRect.GetWidth() * captureConfig.scaleX);
        int32_t height = ceil(areaRect.GetHeight() * captureConfig.scaleY);
        if (width > 0 && static_cast<int32_t>(OHOS::Rosen::NativeBufferUtils::VKIMAGE_LIMIT_SIZE) / width < height) {
            RS_LOGE("RSUiCaptureTaskParallel::CreateResources:image is too large, width:%{public}d,height::%{public}d",
                width, height);
            return false;
        }
#endif
        return true;
    };
}

void RSCapturePixelMapManager::LoadSetMemFunc()
{
    funcHandle_[uint32UniRenderDisableType_][uint32DefaultCaptureType_] = AttachCommMem;
    funcHandle_[uint32UniRenderDisableType_][uint32UiCaptureType_] = AttachShareMem;

    funcHandle_[uint32UniRenderEnableType_][uint32DefaultCaptureType_] = AttachUniCommMem;
    funcHandle_[uint32UniRenderEnableType_][uint32UiCaptureType_] = AttachUniCommMem;
}

bool RSCapturePixelMapManager::SetCapturePixelMapMem(const std::unique_ptr<Media::PixelMap>& pixelmap,
    const RSSurfaceCaptureConfig& captureConfig, const UniRenderEnabledType &uniRenderEnabledType)
{
    if (pixelmap == nullptr) {
        return false;
    }
    // Init Policy Function
    static std::once_flag LoadSetMemFunFlag;
    std::call_once(LoadSetMemFunFlag, []()
    {
        RSCapturePixelMapManager::LoadSetMemFunc();
    });

    auto tempCaptureType = static_cast<uint32_t>(captureConfig.captureType);
    auto tmpUniRenderType = static_cast<uint32_t>(uniRenderEnabledType);
    auto iter = funcHandle_.find(tmpUniRenderType);
    if (iter != funcHandle_.end() && iter->second.find(tempCaptureType) != iter->second.end()) {
        return funcHandle_[tmpUniRenderType][tempCaptureType](pixelmap, captureConfig.useDma);
    }
    return false;
}

bool RSCapturePixelMapManager::CheckCaptureConfig(const Drawing::Rect& areaRect,
    const RSSurfaceCaptureConfig& captureConfig, const UniRenderEnabledType &uniRenderEnabledType)
{
    if (ROSEN_EQ(captureConfig.scaleX, 0.f) || ROSEN_EQ(captureConfig.scaleY, 0.f) ||
        captureConfig.scaleX < 0.f || captureConfig.scaleY < 0.f) {
        return false;
    }
    auto tempCaptureType = static_cast<uint32_t>(captureConfig.captureType);
    auto tmpUniRenderType = static_cast<uint32_t>(uniRenderEnabledType);
    auto iter = checkHandle_.find(tmpUniRenderType);
    if (iter != checkHandle_.end() && iter->second.find(tempCaptureType) != iter->second.end()) {
        return checkHandle_[tmpUniRenderType][tempCaptureType](areaRect, captureConfig);
    }
    return false;
}

bool RSCapturePixelMapManager::CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img,
    const std::unique_ptr<Media::PixelMap>& pixelmap, std::shared_ptr<Drawing::ColorSpace> colorSpace)
{
    if (img == nullptr || pixelmap == nullptr) {
        RS_LOGE("RSCapturePixelMapManager::CopyDataToPixelMap img or pixelmap is nullptr");
        return false;
    }

    uint8_t* pixelMapData = const_cast<uint8_t*>(pixelmap->GetPixels());
    if (pixelMapData == nullptr) {
        RS_LOGE("RSCapturePixelMapManager::CopyDataToPixelMap pixelMapData is nullptr");
        return false;
    }
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format, 0, colorSpace);
    bitmap.SetPixels(pixelMapData);
    if (!img->ReadPixels(bitmap, 0, 0)) {
        RS_LOGE("RSCapturePixelMapManager::CopyDataToPixelMap readPixels failed");
        return false;
    }

    return true;
}

bool RSCapturePixelMapManager::CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img,
    const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    if (img == nullptr || pixelmap == nullptr) {
        return false;
    }
    uint8_t* pixelMapData = const_cast<uint8_t*>(pixelmap->GetPixels());
    if (pixelMapData == nullptr) {
        RS_LOGE("RSCapturePixelMapManager::CopyDataToPixelMap pixelMapData is nullptr");
        return false;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL);
    if (!img->ReadPixels(info, pixelMapData, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSUniUICapture::CopyDataToPixelMap readPixels failed");
        return false;
    }
    return true;
}

Drawing::Rect RSCapturePixelMapManager::GetCaptureAreaRect(const Drawing::Rect& nodeAreaRect,
    const Drawing::Rect& specifiedAreaRect, const SurfaceCaptureType& captureType)
{
    if (captureType == SurfaceCaptureType::DEFAULT_CAPTURE || !specifiedAreaRect.IsValid()) {
        return nodeAreaRect;
    }
    // nodeAreaRect is node weight and height, left and rigth = 0
    if ((specifiedAreaRect.GetWidth() > nodeAreaRect.GetWidth()) ||
        (specifiedAreaRect.GetHeight() > nodeAreaRect.GetHeight()) ||
        (specifiedAreaRect.GetLeft() < 0) || (specifiedAreaRect.GetTop() < 0) ||
        (specifiedAreaRect.GetRight() > nodeAreaRect.GetWidth()) ||
        (specifiedAreaRect.GetBottom() > nodeAreaRect.GetHeight())) {
        RS_LOGE("RSCapturePixelMapManager::GetCaptureAreaRect: specifiedAreaRect is out of bounds");
        return nodeAreaRect;
    }
    return specifiedAreaRect;
}

std::unique_ptr<Media::PixelMap> RSCapturePixelMapManager::GetClientCapturePixelMap(const Drawing::Rect& nodeAreaRect,
    const RSSurfaceCaptureConfig& captureConfig, const UniRenderEnabledType &uniRenderEnabledType,
    const Drawing::Rect& specifiedAreaRect)
{
    if (uniRenderEnabledType == UniRenderEnabledType::UNI_RENDER_DISABLED) {
        return nullptr;
    }
    Drawing::Rect pixelMapArea = GetCaptureAreaRect(nodeAreaRect, specifiedAreaRect, captureConfig.captureType);

    auto pixelMap = GetCapturePixelMap(pixelMapArea, captureConfig, uniRenderEnabledType);
    if (pixelMap == nullptr) {
        return nullptr;
    }
    if (!SetCapturePixelMapMem(pixelMap, captureConfig, uniRenderEnabledType)) {
        RS_LOGE("RSCapturePixelMapManager::GetClientCapturePixelMap SetCapturePixelMapMem failed");
        return nullptr;
    }
    return pixelMap;
}

std::unique_ptr<Media::PixelMap> RSCapturePixelMapManager::GetCapturePixelMap(const Drawing::Rect& areaRect,
    const RSSurfaceCaptureConfig& captureConfig, const UniRenderEnabledType &uniRenderEnabledType)
{
    // Init Policy Function
    static std::once_flag LoadCheckFunFlag;
    std::call_once(LoadCheckFunFlag, []() {
        RSCapturePixelMapManager::LoadCheckFunc();
    });

    if (!CheckCaptureConfig(areaRect, captureConfig, uniRenderEnabledType)) {
        return nullptr;
    }
    return CreatePixelMap(areaRect, captureConfig);
}

std::unique_ptr<Media::PixelMap> RSCapturePixelMapManager::CreatePixelMap(const Drawing::Rect& areaRect,
    const RSSurfaceCaptureConfig& captureConfig)
{
    float pixelMapWidth = areaRect.GetWidth();
    float pixelMapHeight = areaRect.GetHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixelMapWidth * captureConfig.scaleX);
    opts.size.height = ceil(pixelMapHeight * captureConfig.scaleY);
    RS_LOGD("RSCapturePixelMapManager::CreatePixelMap:"
        " origin pixelmap width is [%{public}f], height is [%{public}f],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleX:[%{public}f], scaleY:[%{public}f]",
        pixelMapWidth, pixelMapHeight, opts.size.width, opts.size.height,
        captureConfig.scaleX, captureConfig.scaleY);

    return Media::PixelMap::Create(opts);
}

bool RSCapturePixelMapManager::AttachUniCommMem(const std::unique_ptr<Media::PixelMap>& pixelMap, bool isUsedDma)
{
#if defined (RS_ENABLE_UNI_RENDER) && defined (ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR && isUsedDma) {
        return RSCapturePixelMapManager::AttachDmaMem(pixelMap);
    }
#endif

#ifdef ROSEN_OHOS
    return AttachShareMem(pixelMap);
#endif
    return AttachHeapMem(pixelMap);
}

bool RSCapturePixelMapManager::AttachCommMem(const std::unique_ptr<Media::PixelMap>& pixelMap, bool isUsedDma)
{
    (void)isUsedDma;
#ifdef ROSEN_OHOS
    return AttachShareMem(pixelMap);
#endif
    return AttachHeapMem(pixelMap);
}

bool RSCapturePixelMapManager::AttachShareMem(const std::unique_ptr<Media::PixelMap>& pixelMap, bool isUsedDma)
{
    // unused
    (void)isUsedDma;
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE) && (defined ROSEN_OHOS)
    auto size = pixelMap->GetRowBytes() * pixelMap->GetHeight();
    int fd = AshmemCreate("RSSurfaceCapture Data", size);
    if (fd < 0) {
        RS_LOGE("RSCapturePixelMapManager::AttachShareMem AshmemCreate fd < 0");
        return false;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        RS_LOGE("RSCapturePixelMapManager::AttachShareMem AshmemSetProt error");
        ::close(fd);
        return false;
    }
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    auto data = static_cast<uint8_t*>(ptr);
    if (ptr == MAP_FAILED || ptr == nullptr) {
        RS_LOGE("RSCapturePixelMapManager::AttachShareMem data is nullptr");
        ::close(fd);
        return false;
    }
    void* fdPtr = new (std::nothrow) int32_t();
    if (fdPtr == nullptr) {
        ::munmap(ptr, size);
        ::close(fd);
        return false;
    }
    *static_cast<int32_t*>(fdPtr) = fd;
    pixelMap->SetPixelsAddr(data, fdPtr, size, Media::AllocatorType::SHARE_MEM_ALLOC, nullptr);
    RS_LOGD("RSCapturePixelMapManager::AttachShareMem Success");
    return true;
#else
    RS_LOGE("RSCapturePixelMapManager::AttachShareMem Unsupport AttachShareMem");
    return false;
#endif
}

bool RSCapturePixelMapManager::AttachHeapMem(const std::unique_ptr<Media::PixelMap>& pixelMap)
{
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    auto size = pixelMap->GetRowBytes() * pixelMap->GetHeight();
    auto data = (uint8_t *)malloc(size);
    if (data == nullptr) {
        RS_LOGE("RSCapturePixelMapManager::AttachHeapMem data is nullptr");
        return false;
    }
    pixelMap->SetPixelsAddr(data, nullptr, size, Media::AllocatorType::HEAP_ALLOC, nullptr);
    RS_LOGD("RSCapturePixelMapManager::AttachHeapMem Success");
    return true;
#else
    RS_LOGE("RSCapturePixelMapManager::AttachHeapMem Unsupport AttachHeapMem");
    return false;
#endif
}


bool RSCapturePixelMapManager::AttachDmaMem(const std::unique_ptr<Media::PixelMap>& pixelMap)
{
#if defined(_WIN32) || defined(_APPLE) || defined(A_PLATFORM) || defined(IOS_PLATFORM)
    RS_LOGE("RSCapturePixelMapManager::AttachDmaMem Unsupport dma mem alloc");
    return false;
#elif defined (RS_ENABLE_UNI_RENDER) && defined (ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (pixelmap == nullptr) {
        RS_LOGE("RSCapturePixelMapManager::AttachDmaMem: pixelmap is nullptr");
        return false;
    }
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    if (!surfaceBuffer) {
        RS_LOGE("RSCapturePixelMapManager::AttachDmaMem: surfaceBuffer create failed");
        return false;
    }
    BufferRequestConfig requestConfig = {
        .width = dstInfo.GetWidth(),
        .height = dstInfo.GetHeight(),
        .strideAlignment = 0x8, // set 0x8 as default value to alloc SurfaceBufferImpl
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888, // PixelFormat
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE |
            BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_MMZ_CACHE,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = surfaceBuffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSCapturePixelMapManager::AttachDmaMem: surfaceBuffer alloc failed, %{public}s",
            GSErrorStr(ret).c_str());
        return false;
    }
    void* nativeBuffer = surfaceBuffer.GetRefPtr();
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(nativeBuffer);
    ref->IncStrongRef(ref);
    int32_t bufferSize = pixelmap->GetByteCount();
    pixelmap->SetPixelsAddr(surfaceBuffer->GetVirAddr(), nativeBuffer, bufferSize,
        Media::AllocatorType::DMA_ALLOC, nullptr);
    return false;
#else
    RS_LOGE("RSCapturePixelMapManager::AttachDmaMem Unsupport AttachDmaMem");
    return false;
#endif
}

} // OHOS
} // Rosen
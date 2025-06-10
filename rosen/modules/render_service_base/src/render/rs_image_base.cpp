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

#include "render/rs_image_base.h"

#include <unistd.h>
#include "image_type.h"
#include "image/image.h"
#include "common/rs_background_thread.h"
#ifdef RS_ENABLE_PARALLEL_UPLOAD
#include "render/rs_resource_manager.h"
#endif
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/sk_resource_manager.h"
#include "property/rs_properties_painter.h"
#include "render/rs_image_cache.h"
#include "render/rs_pixel_map_util.h"
#include "memory/rs_memory_track.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
#include "rs_profiler.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "native_buffer_inner.h"
#include "native_window.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
namespace OHOS::Rosen {
namespace {
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_VK) || defined(RS_ENABLE_GL))
constexpr uint8_t ASTC_HEADER_SIZE = 16;
#endif
#ifdef ROSEN_OHOS
bool PixelMapCanBePurge(std::shared_ptr<Media::PixelMap>& pixelMap)
{
    return RSSystemProperties::GetRSImagePurgeEnabled() && pixelMap &&
        pixelMap->GetAllocatorType() == Media::AllocatorType::SHARE_MEM_ALLOC &&
        !RSPixelMapUtil::IsYUVFormat(pixelMap) &&
        !pixelMap->IsEditable() && !pixelMap->IsAstc() && !pixelMap->IsHdr();
}
#endif
}
RSImageBase::~RSImageBase()
{
    if (pixelMap_) {
#ifdef ROSEN_OHOS
        pixelMap_->DecreaseUseCount();
#endif
        pixelMap_ = nullptr;
        if (uniqueId_ > 0) {
            if (renderServiceImage_ || isDrawn_) {
                RSImageCache::Instance().CollectUniqueId(uniqueId_);
            } else {
                RSImageCache::Instance().ReleasePixelMapCache(uniqueId_);
            }
        }
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        RSTaskDispatcher::GetInstance().PostTask(tid_, [nativeWindowBuffer = nativeWindowBuffer_,
            cleanupHelper = cleanUpHelper_]() {
            if (nativeWindowBuffer != nullptr) {
                DestroyNativeWindowBuffer(nativeWindowBuffer);
            }
            if (cleanupHelper != nullptr) {
                NativeBufferUtils::DeleteVkImage(cleanupHelper);
            }
        });
    }
#endif
    } else { // if pixelMap_ not nullptr, do not release skImage cache
        if (image_) {
            image_ = nullptr;
            if (uniqueId_ > 0) {
                // if image_ is obtained by RSPixelMapUtil::ExtractSkImage, uniqueId_ here is related to pixelMap,
                // image_ is not in SkiaImageCache, but still check it here
                // in this case, the cached image_ will be removed when pixelMap cache is removed
                RSImageCache::Instance().ReleaseDrawingImageCache(uniqueId_);
            }
        }
    }
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
Drawing::ColorType GetColorTypeWithVKFormat(VkFormat vkFormat)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return Drawing::COLORTYPE_RGBA_8888;
    }
    switch (vkFormat) {
        case VK_FORMAT_R8G8B8A8_UNORM:
            return Drawing::COLORTYPE_RGBA_8888;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return Drawing::COLORTYPE_RGBA_F16;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return Drawing::COLORTYPE_RGB_565;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return Drawing::COLORTYPE_RGBA_1010102;
        default:
            return Drawing::COLORTYPE_RGBA_8888;
    }
}
#endif

void RSImageBase::DrawImage(Drawing::Canvas& canvas, const Drawing::SamplingOptions& samplingOptions,
    Drawing::SrcRectConstraint constraint)
{
#ifdef ROSEN_OHOS
    auto pixelMapUseCountGuard = PixelMapUseCountGuard(pixelMap_, IsPurgeable());
    DePurge();
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (pixelMap_ && pixelMap_->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC && !pixelMap_->IsAstc()) {
        BindPixelMapToDrawingImage(canvas);
    }
    if (pixelMap_ && pixelMap_->IsAstc()) {
        SetCompressedDataForASTC();
        UploadGpu(canvas);
    }
#endif
    if (!image_) {
        ConvertPixelMapToDrawingImage();
    }
    auto src = RSPropertiesPainter::Rect2DrawingRect(srcRect_);
    auto dst = RSPropertiesPainter::Rect2DrawingRect(dstRect_);
    if (image_ == nullptr) {
        RS_LOGE("RSImageBase::DrawImage image_ is nullptr");
        return;
    }
    canvas.DrawImageRect(*image_, src, dst, samplingOptions, constraint);
}

void RSImageBase::DrawImageNine(Drawing::Canvas& canvas, const Drawing::RectI& center, const Drawing::Rect& dst,
    Drawing::FilterMode filterMode)
{
#ifdef ROSEN_OHOS
    auto pixelMapUseCountGuard = PixelMapUseCountGuard(pixelMap_, IsPurgeable());
    DePurge();
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (pixelMap_ && pixelMap_->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        BindPixelMapToDrawingImage(canvas);
    }
#endif
    if (!image_) {
        ConvertPixelMapToDrawingImage();
    }
    if (image_ == nullptr) {
        RS_LOGE("RSImageBase::DrawImage image_ is nullptr");
        return;
    }
    canvas.DrawImageNine(image_.get(), center, dst, filterMode, nullptr);
}

void RSImageBase::DrawImageLattice(Drawing::Canvas& canvas, const Drawing::Lattice& lattice, const Drawing::Rect& dst,
    Drawing::FilterMode filterMode)
{
#ifdef ROSEN_OHOS
    auto pixelMapUseCountGuard = PixelMapUseCountGuard(pixelMap_, IsPurgeable());
    DePurge();
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (pixelMap_ && pixelMap_->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        BindPixelMapToDrawingImage(canvas);
    }
#endif
    if (!image_) {
        ConvertPixelMapToDrawingImage();
    }
    if (image_ == nullptr) {
        RS_LOGE("RSImageBase::DrawImage image_ is nullptr");
        return;
    }
    canvas.DrawImageLattice(image_.get(), lattice, dst, filterMode);
}

void RSImageBase::SetImage(const std::shared_ptr<Drawing::Image> image)
{
    isDrawn_ = false;
    image_ = image;
    if (image_) {
#ifndef ROSEN_ARKUI_X
        SKResourceManager::Instance().HoldResource(image);
#endif
        srcRect_.SetAll(0.0, 0.0, image_->GetWidth(), image_->GetHeight());
        GenUniqueId(image_->GetUniqueID());
    }
}

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
void RSImageBase::SetDmaImage(const std::shared_ptr<Drawing::Image> image)
{
    isDrawn_ = false;
    image_ = image;
}

void RSImageBase::MarkYUVImage()
{
    isDrawn_ = false;
    isYUVImage_ = true;
    canPurgeShareMemFlag_ = CanPurgeFlag::DISABLED;
}
#endif

void RSImageBase::SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap)
{
#ifdef ROSEN_OHOS
    if (pixelMap_) {
        pixelMap_->DecreaseUseCount();
    }
    if (pixelmap) {
        pixelmap->IncreaseUseCount();
    }
#endif
    pixelMap_ = pixelmap;
    if (pixelMap_) {
        srcRect_.SetAll(0.0, 0.0, pixelMap_->GetWidth(), pixelMap_->GetHeight());
        image_ = nullptr;
        GenUniqueId(pixelMap_->GetUniqueId());
    }
}

void RSImageBase::DumpPicture(DfxString& info) const
{
    if (!pixelMap_) {
        return;
    }
    info.AppendFormat("%d    [%d * %d]  %p\n", pixelMap_->GetByteCount(), pixelMap_->GetWidth(), pixelMap_->GetHeight(),
        pixelMap_.get());
}

void RSImageBase::SetSrcRect(const RectF& srcRect)
{
    srcRect_ = srcRect;
}

void RSImageBase::SetDstRect(const RectF& dstRect)
{
    if (dstRect_ != dstRect) {
        isDrawn_ = false;
    }
    dstRect_ = dstRect;
}

void RSImageBase::SetImagePixelAddr(void* addr)
{
    imagePixelAddr_ = addr;
    if (imagePixelAddr_) {
        canPurgeShareMemFlag_ = CanPurgeFlag::DISABLED;
    }
}

void RSImageBase::UpdateNodeIdToPicture(NodeId nodeId)
{
    if (!nodeId) {
        return;
    }
    if (pixelMap_) {
#ifndef ROSEN_ARKUI_X
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetClosePixelMapFdEnabled()) {
        MemoryTrack::Instance().UpdatePictureInfo(pixelMap_->GetPixels(), nodeId, ExtractPid(nodeId));
    } else {
        MemoryTrack::Instance().UpdatePictureInfo(pixelMap_->GetFd(), nodeId, ExtractPid(nodeId));
    }
#else
        MemoryTrack::Instance().UpdatePictureInfo(pixelMap_->GetPixels(), nodeId, ExtractPid(nodeId));
#endif
#endif
    }
    if (image_ || imagePixelAddr_) {
#ifndef ROSEN_ARKUI_X
        MemoryTrack::Instance().UpdatePictureInfo(imagePixelAddr_, nodeId, ExtractPid(nodeId));
#endif
    }
}

void RSImageBase::Purge()
{
#ifdef ROSEN_OHOS
    if (canPurgeShareMemFlag_ != CanPurgeFlag::ENABLED ||
        uniqueId_ <= 0 || !pixelMap_ || pixelMap_->IsUnMap()) {
        return;
    }

    constexpr int USE_COUNT_FOR_PURGE = 2; // one in this RSImage, one in RSImageCache
    auto imageUseCount = image_.use_count();
    auto pixelMapCount = pixelMap_.use_count();
    if (!(imageUseCount == USE_COUNT_FOR_PURGE && pixelMapCount == USE_COUNT_FOR_PURGE + 1) &&
        !(imageUseCount == 0 && pixelMapCount == USE_COUNT_FOR_PURGE)) {
        return;
    }
    // skip purge if multi RsImage Holds this PixelMap
    if (!RSImageCache::Instance().CheckRefCntAndReleaseImageCache(uniqueId_, pixelMap_)) {
        return;
    }
    isDrawn_ = false;
    image_ = nullptr;
    bool unmapResult = pixelMap_->UnMap();
    if (unmapResult && pixelMap_.use_count() > USE_COUNT_FOR_PURGE) {
        RS_LOGW("UNMAP_LOG Purge while use_count > USE_COUNT_FOR_PURGE");
    }
#endif
}

void RSImageBase::DePurge()
{
#ifdef ROSEN_OHOS
    if (canPurgeShareMemFlag_ != CanPurgeFlag::ENABLED ||
        uniqueId_ <= 0 || !pixelMap_ || !pixelMap_->IsUnMap()) {
        return;
    }
    if (image_ != nullptr) {
        RS_LOGW("UNMAP_LOG Image is not reset when PixelMap is unmap");
        isDrawn_ = false;
        image_ = nullptr;
    }
    pixelMap_->ReMap();
#endif
}

void RSImageBase::MarkRenderServiceImage()
{
    renderServiceImage_ = true;
}

void RSImageBase::MarkPurgeable()
{
#ifdef ROSEN_OHOS
    if (renderServiceImage_ && canPurgeShareMemFlag_ == CanPurgeFlag::UNINITED && PixelMapCanBePurge(pixelMap_)) {
        canPurgeShareMemFlag_ = CanPurgeFlag::ENABLED;
    }
#endif
}

bool RSImageBase::IsPurgeable() const
{
    return canPurgeShareMemFlag_ == CanPurgeFlag::ENABLED;
}

#ifdef ROSEN_OHOS
static bool UnmarshallingAndCacheDrawingImage(
    Parcel& parcel, std::shared_ptr<Drawing::Image>& img, uint64_t uniqueId, void*& imagepixelAddr)
{
    if (img != nullptr) {
        // match a cached SkImage
        if (!RSMarshallingHelper::SkipImage(parcel)) {
            RS_LOGE("UnmarshalAndCacheSkImage SkipSkImage fail");
            return false;
        }
    } else if (RSMarshallingHelper::Unmarshalling(parcel, img, imagepixelAddr)) {
        // unmarshalling the SkImage and cache it
        RSImageCache::Instance().CacheDrawingImage(uniqueId, img);
    } else {
        RS_LOGE("UnmarshalAndCacheSkImage fail");
        return false;
    }
    return true;
}


static bool UnmarshallingAndCachePixelMap(Parcel& parcel, std::shared_ptr<Media::PixelMap>& pixelMap,
    uint64_t uniqueId, uint32_t versionId)
{
    if (pixelMap != nullptr) {
        // match a cached pixelMap
        if (versionId == pixelMap->GetVersionId()) {
            if (!RSMarshallingHelper::SkipPixelMap(parcel)) {
                return false;
            }
        } else {
            RSImageCache::Instance().ReleasePixelMapCache(uniqueId);
            if (!RSMarshallingHelper::Unmarshalling(parcel, pixelMap)) {
                return false;
            }
            // unmarshalling the pixelmap and cache it
            RSImageCache::Instance().CachePixelMap(uniqueId, pixelMap);
        }
    } else if (RSMarshallingHelper::Unmarshalling(parcel, pixelMap)) {
        if (pixelMap) {
            // unmarshalling the pixelMap and cache it
            RSImageCache::Instance().CachePixelMap(uniqueId, pixelMap);
        }
    } else {
        return false;
    }
    return true;
}

static bool UnmarshallingIdAndRect(Parcel& parcel, uint64_t& uniqueId, RectF& srcRect, RectF& dstRect)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, uniqueId)) {
        RS_LOGE("RSImage::Unmarshalling uniqueId fail");
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, uniqueId);
    if (!RSMarshallingHelper::Unmarshalling(parcel, srcRect)) {
        RS_LOGE("RSImage::Unmarshalling srcRect fail");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, dstRect)) {
        RS_LOGE("RSImage::Unmarshalling dstRect fail");
        return false;
    }
    return true;
}

bool RSImageBase::UnmarshallingDrawingImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useSkImage,
    std::shared_ptr<Drawing::Image>& img, std::shared_ptr<Media::PixelMap>& pixelMap, void*& imagepixelAddr)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, useSkImage)) {
        return false;
    }
    uint32_t versionId = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, versionId)) {
        return false;
    }
    if (useSkImage) {
        img = RSImageCache::Instance().GetDrawingImageCache(uniqueId);
        RS_TRACE_NAME_FMT("RSImageBase::Unmarshalling Image uniqueId:%lu, size:[%d %d], cached:%d",
            uniqueId, img ? img->GetWidth() : 0, img ? img->GetHeight() : 0, img != nullptr);
        if (!UnmarshallingAndCacheDrawingImage(parcel, img, uniqueId, imagepixelAddr)) {
            RS_LOGE("RSImageBase::Unmarshalling UnmarshalAndCacheSkImage fail");
            return false;
        }
        RSMarshallingHelper::SkipPixelMap(parcel);
    } else {
        if (!RSMarshallingHelper::SkipImage(parcel)) {
            return false;
        }
        pixelMap = RSImageCache::Instance().GetPixelMapCache(uniqueId);
        RS_TRACE_NAME_FMT("RSImageBase::Unmarshalling pixelMap uniqueId:%lu, size:[%d %d], cached:%d",
            uniqueId, pixelMap ? pixelMap->GetWidth() : 0, pixelMap ? pixelMap->GetHeight() : 0, pixelMap != nullptr);
        if (!UnmarshallingAndCachePixelMap(parcel, pixelMap, uniqueId, versionId)) {
            RS_LOGE("RSImageBase::Unmarshalling UnmarshalAndCachePixelMap fail");
            return false;
        }
    }
    return true;
}

void RSImageBase::IncreaseCacheRefCount(uint64_t uniqueId, bool useSkImage, std::shared_ptr<Media::PixelMap>
    pixelMap)
{
    if (useSkImage) {
        RSImageCache::Instance().IncreaseDrawingImageCacheRefCount(uniqueId);
    } else if (pixelMap) {
        RSImageCache::Instance().IncreasePixelMapCacheRefCount(uniqueId);
    }
}

bool RSImageBase::Marshalling(Parcel& parcel) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t versionId = pixelMap_ == nullptr ? 0 : pixelMap_->GetVersionId();
    bool success = RSMarshallingHelper::Marshalling(parcel, uniqueId_) &&
                   RSMarshallingHelper::Marshalling(parcel, srcRect_) &&
                   RSMarshallingHelper::Marshalling(parcel, dstRect_) &&
                   parcel.WriteBool(pixelMap_ == nullptr) &&
                   RSMarshallingHelper::Marshalling(parcel, versionId) &&
                   RSMarshallingHelper::Marshalling(parcel, image_) &&
                   RSMarshallingHelper::Marshalling(parcel, pixelMap_);
    if (!success) {
        RS_LOGE("RSImageBase::Marshalling parcel fail");
    }
    return success;
}

RSImageBase* RSImageBase::Unmarshalling(Parcel& parcel)
{
    uint64_t uniqueId;
    RectF srcRect;
    RectF dstRect;
    if (!UnmarshallingIdAndRect(parcel, uniqueId, srcRect, dstRect)) {
        RS_LOGE("RSImage::Unmarshalling UnmarshalIdAndSize fail");
        return nullptr;
    }

    bool useSkImage;
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    std::shared_ptr<Media::PixelMap> pixelMap;
    void* imagepixelAddr = nullptr;
    if (!UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSkImage, img, pixelMap, imagepixelAddr)) {
        return nullptr;
    }

    RSImageBase* rsImage = new RSImageBase();
    rsImage->SetImage(img);
    rsImage->SetImagePixelAddr(imagepixelAddr);
    rsImage->SetPixelMap(pixelMap);
    rsImage->SetSrcRect(srcRect);
    rsImage->SetDstRect(dstRect);
    rsImage->uniqueId_ = uniqueId;
    rsImage->MarkRenderServiceImage();
    IncreaseCacheRefCount(uniqueId, useSkImage, pixelMap);
    return rsImage;
}
#endif

void RSImageBase::ConvertPixelMapToDrawingImage(bool paraUpload)
{
#if defined(ROSEN_OHOS)
    // paraUpload only enable in render_service or UnmarshalThread
    pid_t tid = paraUpload ? getpid() : gettid();
#endif
    if (!image_ && pixelMap_ && !pixelMap_->IsAstc() && !isYUVImage_) {
#if defined(ROSEN_OHOS)
            image_ = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(uniqueId_, tid);
#else
            image_ = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(uniqueId_);
#endif
        if (!image_) {
            image_ = RSPixelMapUtil::ExtractDrawingImage(pixelMap_);
#if defined(ROSEN_OHOS)
            RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(uniqueId_, image_, tid);
#else
            RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(uniqueId_, image_);
#endif
#ifdef RS_ENABLE_PARALLEL_UPLOAD
            RSResourceManager::Instance().UploadTexture(paraUpload && renderServiceImage_, image_,
                pixelMap_, uniqueId_);
#endif
        }
    }
}

uint64_t RSImageBase::GetUniqueId() const
{
    return uniqueId_;
}

void RSImageBase::GenUniqueId(uint32_t id)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsignd number shift
    uniqueId_ = shiftedPid | id;
}

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
void RSImageBase::ProcessYUVImage(std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    if (!gpuContext) {
        return;
    }
    auto cache = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(uniqueId_, gettid());
    std::lock_guard<std::mutex> lock(mutex_);
    if (cache) {
        image_ = cache;
        return;
    }
    RS_TRACE_NAME("make yuv img");
    auto image = RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap_);
    if (image) {
        image_ = image;
        SKResourceManager::Instance().HoldResource(image);
        RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(uniqueId_, image, gettid());
    } else {
        RS_LOGE("make yuv image %{public}d (%{public}d, %{public}d) failed",
            (int)uniqueId_, (int)srcRect_.width_, (int)srcRect_.height_);
    }
}
#endif

void RSImageBase::SetCompressData(const std::shared_ptr<Drawing::Data> compressData)
{
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    isDrawn_ = false;
    compressData_ = compressData;
    canPurgeShareMemFlag_ = CanPurgeFlag::DISABLED;
#endif
}

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
bool RSImageBase::SetCompressedDataForASTC()
{
    if (!pixelMap_ || !pixelMap_->GetFd() || !pixelMap_->IsAstc()) {
        RS_LOGE("%{public}s fail, pixelMap_ is invalid", __func__);
        return false;
    }
    std::shared_ptr<Drawing::Data> fileData = std::make_shared<Drawing::Data>();
#if defined(RS_ENABLE_VK)
    if (pixelMap_->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC &&
        (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR)) {
        sptr<SurfaceBuffer> surfaceBuf(reinterpret_cast<SurfaceBuffer *>(pixelMap_->GetFd()));
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuf);
        OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer_);
        if (nativeBuffer == nullptr || !fileData->BuildFromOHNativeBuffer(nativeBuffer, pixelMap_->GetCapacity())) {
            RS_LOGE("%{public}s data BuildFromOHNativeBuffer fail", __func__);
            return false;
        }
    } else {
#endif // RS_ENABLE_VK
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        const void* data = pixelMap_->GetPixels();
        if (pixelMap_->GetCapacity() > ASTC_HEADER_SIZE &&
            (data == nullptr || !fileData->BuildWithoutCopy(
                reinterpret_cast<const void *>(reinterpret_cast<const char *>(data) + ASTC_HEADER_SIZE),
                pixelMap_->GetCapacity() - ASTC_HEADER_SIZE))) {
            RS_LOGE("%{public}s data BuildWithoutCopy fail", __func__);
            return false;
        }
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#if defined(RS_ENABLE_VK)
    }
#endif // RS_ENABLE_VK
    SetCompressData(fileData);
    return true;
}
#endif // ROSEN_OHOS && (RS_ENABLE_GL || RS_ENABLE_VK)

std::shared_ptr<Media::PixelMap> RSImageBase::GetPixelMap() const
{
    return pixelMap_;
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
std::shared_ptr<Drawing::Image> RSImageBase::MakeFromTextureForVK(
    Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    if (!surfaceBuffer || !canvas.GetGPUContext()) {
        RS_LOGE("RSImageBase MakeFromTextureForVK surfaceBuffer is nullptr");
        return nullptr;
    }
    if (nativeWindowBuffer_ == nullptr) {
        sptr<SurfaceBuffer> sfBuffer(surfaceBuffer);
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&sfBuffer);
        if (!nativeWindowBuffer_) {
            RS_LOGE("RSImageBase MakeFromTextureForVK create native window buffer fail");
            return nullptr;
        }
    }
    if (!backendTexture_.IsValid()) {
        backendTexture_ = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
            nativeWindowBuffer_, surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), false);
        if (backendTexture_.IsValid()) {
            auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
            cleanUpHelper_ = new NativeBufferUtils::VulkanCleanupHelper(
                RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
        } else {
            return nullptr;
        }
        tid_ = gettid();
    }

    std::shared_ptr<Drawing::Image> dmaImage = std::make_shared<Drawing::Image>();
    auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
    Drawing::ColorType colorType = GetColorTypeWithVKFormat(vkTextureInfo->format);
    Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
    if (!dmaImage->BuildFromTexture(*canvas.GetGPUContext(), backendTexture_.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr, NativeBufferUtils::DeleteVkImage,
        cleanUpHelper_->Ref())) {
        RS_LOGE("RSImageBase MakeFromTextureForVK build image failed");
        return nullptr;
    }
    return dmaImage;
}

void RSImageBase::BindPixelMapToDrawingImage(Drawing::Canvas& canvas)
{
    if (pixelMap_ && !pixelMap_->IsAstc()) {
        std::shared_ptr<Drawing::Image> imageCache = nullptr;
        if (!pixelMap_->IsEditable()) {
            imageCache = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(uniqueId_, gettid());
        }
        if (imageCache) {
            image_ = imageCache;
        } else {
            image_ = MakeFromTextureForVK(canvas, reinterpret_cast<SurfaceBuffer*>(pixelMap_->GetFd()));
            if (image_) {
                SKResourceManager::Instance().HoldResource(image_);
                RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(uniqueId_, image_, gettid());
            }
        }
    }
}
#endif

#ifdef ROSEN_OHOS
RSImageBase::PixelMapUseCountGuard::PixelMapUseCountGuard(std::shared_ptr<Media::PixelMap> pixelMap, bool purgeable)
    : pixelMap_(pixelMap), purgeable_(purgeable)
{
    if (purgeable_) {
        pixelMap_->IncreaseUseCount();
    }
}

RSImageBase::PixelMapUseCountGuard::~PixelMapUseCountGuard()
{
    if (purgeable_) {
        pixelMap_->DecreaseUseCount();
    }
}
#endif

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
static Drawing::CompressedType PixelFormatToCompressedType(Media::PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case Media::PixelFormat::ASTC_4x4: return Drawing::CompressedType::ASTC_RGBA8_4x4;
        case Media::PixelFormat::ASTC_6x6: return Drawing::CompressedType::ASTC_RGBA8_6x6;
        case Media::PixelFormat::ASTC_8x8: return Drawing::CompressedType::ASTC_RGBA8_8x8;
        case Media::PixelFormat::UNKNOWN:
        default: return Drawing::CompressedType::NoneType;
    }
}

static std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(ColorManager::ColorSpaceName
 colorSpaceName)
{
    switch (colorSpaceName) {
        case ColorManager::ColorSpaceName::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case ColorManager::ColorSpaceName::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case ColorManager::ColorSpaceName::SRGB:
            return Drawing::ColorSpace::CreateSRGB();
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}
#endif

void RSImageBase::UploadGpu(Drawing::Canvas& canvas)
{
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (compressData_) {
        auto cache = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(uniqueId_, gettid());
        std::lock_guard<std::mutex> lock(mutex_);
        if (cache) {
            image_ = cache;
        } else {
            if (canvas.GetGPUContext() == nullptr) {
                return;
            }
            RS_TRACE_NAME("make compress img");
            Media::ImageInfo imageInfo;
            pixelMap_->GetImageInfo(imageInfo);
            Media::Size realSize;
            pixelMap_->GetAstcRealSize(realSize);
            auto image = std::make_shared<Drawing::Image>();
            std::shared_ptr<Drawing::ColorSpace> colorSpace =
                ColorSpaceToDrawingColorSpace(pixelMap_->InnerGetGrColorSpace().GetColorSpaceName());
            bool result = image->BuildFromCompressed(*canvas.GetGPUContext(), compressData_,
                static_cast<int>(realSize.width), static_cast<int>(realSize.height),
                PixelFormatToCompressedType(imageInfo.pixelFormat), colorSpace);
            if (result) {
                image_ = image;
                SKResourceManager::Instance().HoldResource(image);
                RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(uniqueId_, image, gettid());
            } else {
                RS_LOGE("make astc image %{public}d (%{public}d, %{public}d) failed",
                    (int)uniqueId_, (int)srcRect_.width_, (int)srcRect_.height_);
            }
            compressData_ = nullptr;
        }
        return;
    }
    if (isYUVImage_) {
        ProcessYUVImage(canvas.GetGPUContext());
    }
#endif
}
}

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <thread>

#include "feature/image_detail_enhancer/rs_image_detail_enhancer_thread.h"

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_pixel_map_util.h"
#include "rs_trace.h"

#ifdef ROSEN_OHOS
#include <linux/dma-buf.h>
#include <parameters.h>
#include <sys/ioctl.h>
#endif

#ifdef RES_BASE_SCHED_ENABLE
#include "qos.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#define DMA_BUF_SET_LEAK_TYPE _IOW(DMA_BUF_BASE, 5, const char *)

namespace OHOS {
namespace Rosen {
constexpr int IMAGE_DIFF_VALUE = 200; // Image size difference

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
constexpr float INFO_ALPHA = 0.5f; // Sharpness parameter for SLR
#endif

RSImageDetailEnhancerThread& RSImageDetailEnhancerThread::Instance()
{
    static RSImageDetailEnhancerThread instance;
    return instance;
}

RSImageDetailEnhancerThread::RSImageDetailEnhancerThread()
{
    isParamValidate_ = RsCommonHook::Instance().IsImageEnhanceParamsValid();
    if (!isParamValidate_) {
        return;
    }
    params_ = RsCommonHook::Instance().GetImageEnhanceParams();
    slrParams_ = RsCommonHook::Instance().GetImageEnhanceAlgoParams("SLR");
    esrParams_ = RsCommonHook::Instance().GetImageEnhanceAlgoParams("ESR");
    runner_ = AppExecFwk::EventRunner::Create("RSImageDetailEnhancerThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
#ifdef RES_BASE_SCHED_ENABLE
    PostTask([this]() {
        auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
        RS_LOGI("RSImageDetailEnhancerThread SetThreadQos retcode = %{public}d", ret);
    });
#endif
}

void RSImageDetailEnhancerThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    } else {
        RS_LOGE("RSImageDetailEnhancerThread PostTask failed, handler_ is invalid!");
    }
}

bool RSImageDetailEnhancerThread::RegisterCallback(const std::function<void(uint64_t)>& callback)
{
    if (callback == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread RegisterCallback failed, callBack is invalid!");
        return false;
    }
    callback_ = callback;
    return true;
}

void RSImageDetailEnhancerThread::MarkDirty(uint64_t nodeId)
{
    if (callback_ == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread MarkDirty failed!");
        return;
    }
    callback_(nodeId);
}

bool RSImageDetailEnhancerThread::IsSizeSupport(int srcWidth, int srcHeight, int dstWidth, int dstHeight)
{
    if (srcWidth > 0 && srcHeight > 0 && srcWidth != dstWidth && srcHeight != dstHeight && dstWidth > params_.minSize &&
        dstHeight > params_.minSize && dstWidth < params_.maxSize && dstHeight < params_.maxSize) {
        float scaleX = (dstWidth * 1.0f) / (srcWidth * 1.0f);
        float scaleY = (dstHeight * 1.0f) / (srcHeight * 1.0f);
        if ((scaleX > params_.minScaleRatio && scaleY > params_.minScaleRatio) &&
            (scaleX < params_.maxScaleRatio && scaleY < params_.maxScaleRatio)) {
            return true;
        }
    }
    RS_LOGD("RSImageDetailEnhancerThread size is not support, srcSize=(%{public}d, %{public}d), \
        dstSize=(%{public}d, %{public}d)", srcWidth, srcHeight, dstWidth, dstHeight);
    return false;
}

bool RSImageDetailEnhancerThread::GetSharpness(RSImageDetailEnhanceAlgoParams& param,
    float scaleRatio, float& sharpness)
{
    for (size_t i = 0; i < param.rangeParams.size(); i++) {
        if (scaleRatio <= param.rangeParams[i].rangeMax && scaleRatio >= param.rangeParams[i].rangeMin) {
            sharpness =  param.rangeParams[i].effectParam;
            return true;
        }
    }
    return false;
}

void RSImageDetailEnhancerThread::ResetStatus(int srcWidth, int srcHeight,
    int dstWidth, int dstHeight, uint64_t imageId)
{
    if (GetProcessStatus(imageId) && GetProcessReady(imageId) &&
        IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight)) {
        std::shared_ptr<Drawing::Image> dstImage = GetOutImage(imageId);
        if (dstImage != nullptr) {
            int dstImageWidth = dstImage->GetWidth();
            int dstImageHeight = dstImage->GetHeight();
            if (dstImageWidth != dstWidth && dstImageHeight != dstHeight &&
                abs(dstImageWidth - dstWidth) > IMAGE_DIFF_VALUE &&
                abs(dstImageHeight - dstHeight) > IMAGE_DIFF_VALUE) {
                SetProcessStatus(imageId, false);
                SetProcessReady(imageId, false);
            }
        }
    }
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
bool RSImageDetailEnhancerThread::IsTypeSupport(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap != nullptr && !pixelMap->IsEditable() && !pixelMap->IsHdr() &&
        pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        return true;
    }
    RS_LOGD("RSImageDetailEnhancerThread type is not support!");
    return false;
}

std::shared_ptr<Drawing::Image> RSImageDetailEnhancerThread::ScaleByAAE(sptr<SurfaceBuffer>& dstSurfaceBuffer,
    const std::shared_ptr<Drawing::Image>& srcImage)
{
    if (dstSurfaceBuffer == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread ScaleByAAE failed, input is invalid!");
        return nullptr;
    }
    std::shared_ptr<Drawing::Image> dstImage = DetailEnhancerUtils::Instance().MakeImageFromSurfaceBuffer(
        dstSurfaceBuffer, srcImage);
    if (srcImage && dstImage) {
        Drawing::RectI srcRect = Drawing::RectI(0, 0, srcImage->GetWidth(), srcImage->GetHeight());
        Drawing::RectI dstRect = Drawing::RectI(0, 0, dstSurfaceBuffer->GetWidth(), dstSurfaceBuffer->GetHeight());
        Drawing::ScalingOption option = {srcRect, dstRect, Drawing::ScalingType::OPTION_AAE_ARSR};
        Drawing::ScaleImageResult ret = Drawing::Image::ScaleImage(srcImage, dstImage, option);
        if (ret == Drawing::ScaleImageResult::SCALE_RESULT_SUCCESS) {
            RS_LOGD("ScaleByAAE success, srcSize=(%{public}d, %{public}d), dstSize=(%{public}d, %{public}d)",
                srcImage->GetWidth(), srcImage->GetHeight(), dstSurfaceBuffer->GetWidth(),
                dstSurfaceBuffer->GetHeight());
            return dstImage;
        } else {
            RS_LOGD("RSImageDetailEnhancerThread ScaleImageAsync Scaling Image with AAE failed!");
            return nullptr;
        }
    }
    return nullptr;
}

std::shared_ptr<Drawing::Image> RSImageDetailEnhancerThread::ScaleByHDSampler(int dstWidth, int dstHeight,
    sptr<SurfaceBuffer>& dstSurfaceBuffer, const std::shared_ptr<Drawing::Image>& srcImage)
{
    std::shared_ptr<Drawing::Surface> newSurface =
        DetailEnhancerUtils::Instance().InitSurface(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    if (newSurface == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread ScaleByHDSampler create newSurface failed!");
        return nullptr;
    }
    auto newCanvas = newSurface->GetCanvas();
    if (newCanvas == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread ScaleByHDSampler failed, newCanvas is invalid!");
        return nullptr;
    }
    int srcWidth = srcImage->GetWidth();
    int srcHeight = srcImage->GetHeight();
    auto src = Drawing::Rect(0, 0, srcWidth, srcHeight);
    auto dst = Drawing::Rect(0, 0, dstWidth, dstHeight);
    Drawing::Brush brush;
    Drawing::HDSampleInfo info;
    float scaleRatio = static_cast<float>(dstWidth) / static_cast<float>(srcWidth);
    if (srcWidth > dstWidth && srcHeight > dstHeight) {
        info.type = Drawing::HDSampleType::SLR;
        if (!GetSharpness(slrParams_, scaleRatio, info.alpha)) {
            return nullptr;
        }
        info.isUniformScale = false;
    } else if (srcWidth < dstWidth && srcHeight < dstHeight) {
        info.type = Drawing::HDSampleType::ESR;
    } else {
        RS_LOGE("RSImageDetailEnhancerThread ScaleByHDSampler failed, size is invalid!");
        return nullptr;
    }
    auto imageFilter = Drawing::ImageFilter::CreateHDSampleImageFilter(srcImage, src, dst, info);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(imageFilter);
    brush.SetFilter(filter);
    newCanvas->AttachBrush(brush);
    auto sampling = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    newCanvas->DrawImageRect(*srcImage, src, dst, sampling);
    std::shared_ptr<Drawing::Image> dstImage = newSurface->GetImageSnapshot();
    newSurface->FlushAndSubmit(true);
    newCanvas->DetachBrush();
    if (dstImage == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread ScaleByHDSampler failed, dstImage is invalid!");
        return nullptr;
    }
    RS_LOGD("ScaleByHDSampler success, srcSize=(%{public}d, %{public}d), dstSize=(%{public}d, %{public}d)",
        srcWidth, srcHeight, dstWidth, dstHeight);
    return dstImage;
}

void RSImageDetailEnhancerThread::ExecuteTaskAsync(int dstWidth, int dstHeight,
    const std::shared_ptr<Drawing::Image>& image, uint64_t nodeId, uint64_t imageId)
{
    RS_TRACE_NAME_FMT("RSImageDetailEnhancerThread::ExecuteTaskAsync");
    if (image == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread ExecuteTaskAsync image is nullptr!");
        return;
    }
    sptr<SurfaceBuffer> dstSurfaceBuffer = DetailEnhancerUtils::Instance().CreateSurfaceBuffer(dstWidth, dstHeight);
    if (dstSurfaceBuffer == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread ExecuteTaskAsync create dstSurfaceBuffer failed!");
        return;
    }
    if (!DetailEnhancerUtils::Instance().SetMemoryName(dstSurfaceBuffer)) {
        return;
    }
    std::shared_ptr<Drawing::Image> dstImage = nullptr;
    int srcWidth = image->GetWidth();
    int srcHeight = image->GetHeight();
    if (srcWidth < dstWidth && srcHeight < dstHeight) {
        dstImage = ScaleByAAE(dstSurfaceBuffer, image);
    }
    if (dstImage == nullptr) {
        dstImage = ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, image);
    }
    if (dstImage != nullptr) {
        SetOutImage(imageId, dstImage);
        MarkDirty(nodeId);
    }
}

std::shared_ptr<Drawing::Image> RSImageDetailEnhancerThread::EnhanceImageAsync(
    Drawing::Canvas& canvas, const Drawing::SamplingOptions& samplingOptions,
    const RSImageParams& RSImageParams) const
{
    bool isEnable = GetEnableStatus() && IsEnableImageDetailEnhance(RSImageParams.mNodeId);
    if (isEnable || RSImageParams.mPixelMap == nullptr) {
        return false;
    }
    ScaleImageAsync(RSImageParams.mPixelMap, RSImageParams.mDst, RSImageParams.mNodeId,
        RSImageParams.mUniqueId, RSImageParams.mImage);
    std::shared_ptr<Drawing::Image> dstImage = GetOutImage(RSImageParams.mUniqueId);
    if (dstImage == nullptr) {
        return false;
    }
    Drawing::Rect newSrcRect(0, 0, dstImage->GetWidth(), dstImage->GetHeight());
    canvas.DrawImageRect(*dstImage, newSrcRect, RSImageParams.mDst, samplingOptions,
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    if (RSImageParams.mNeedDetachPen) {
        canvas.DetachPen();
    }
    return true;
}

void RSImageDetailEnhancerThread::ScaleImageAsync(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::Rect& dst, uint64_t nodeId, uint64_t imageId, const std::shared_ptr<Drawing::Image>& image)
{
    if (image == nullptr || !IsTypeSupport(pixelMap)) {
        RS_LOGD("RSImageDetailEnhancerThread ScaleImageAsync, input is invalid!");
        return;
    }
    RS_TRACE_NAME_FMT("RSImageDetailEnhancerThread::ScaleImageAsync");
    int srcWidth = image->GetWidth();
    int srcHeight = image->GetHeight();
    int dstWidth = static_cast<int>(dst.GetWidth());
    int dstHeight = static_cast<int>(dst.GetHeight());
    ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);
    if (IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight) && !GetProcessStatus(imageId)) {
        SetProcessStatus(imageId, true);
        auto asyncEnhancerTask = [this, dstWidth, dstHeight, image, nodeId, imageId]() {
            ExecuteTaskAsync(dstWidth, dstHeight, image, nodeId, imageId);
        };
        RSImageDetailEnhancerThread::Instance().PostTask(asyncEnhancerTask);
    }
}
#endif

void RSImageDetailEnhancerThread::SetOutImage(uint64_t imageId, const std::shared_ptr<Drawing::Image>& image)
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    outImageMap_[imageId] = image;
    processStatusMap_[imageId] = true;
    processReadyMap_[imageId] = true;
}

std::shared_ptr<Drawing::Image> RSImageDetailEnhancerThread::GetOutImage(uint64_t imageId) const
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    auto it = outImageMap_.find(imageId);
    return it != outImageMap_.end() ? it->second : nullptr;
}

void RSImageDetailEnhancerThread::ReleaseOutImage(uint64_t imageId)
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    outImageMap_.erase(imageId);
}

void RSImageDetailEnhancerThread::SetProcessStatus(uint64_t imageId, bool flag)
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    processStatusMap_[imageId] = flag;
}

bool RSImageDetailEnhancerThread::GetProcessStatus(uint64_t imageId) const
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    auto it = processStatusMap_.find(imageId);
    return it != processStatusMap_.end() ? it->second : false;
}

void RSImageDetailEnhancerThread::SetProcessReady(uint64_t imageId, bool flag)
{
    processReadyMap_[imageId] = flag;
}

bool RSImageDetailEnhancerThread::GetProcessReady(uint64_t imageId) const
{
    auto it = processReadyMap_.find(imageId);
    return it != processReadyMap_.end() ? it->second : false;
}

bool RSImageDetailEnhancerThread::GetEnableStatus() const
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (std::atoi((system::GetParameter("rosen.isEnabledScaleImageAsync.enabled", "0")).c_str()) != 0 &&
        RSSystemProperties::GetMemoryWatermarkEnabled() &&
        isParamValidate_) {
        return true;
    }
    return false;
#endif
    return false;
}

bool RSImageDetailEnhancerThread::IsEnableImageDetailEnhance(uint64_t nodeId) const
{
    pid_t pid = ExtractPid(nodeId);
    std::unordered_set<pid_t> imageEnhancePidList = RsCommonHook::Instance().GetImageEnhancePidList();
    return imageEnhancePidList.find(pid) != imageEnhancePidList.end();
}

DetailEnhancerUtils& DetailEnhancerUtils::Instance()
{
    static DetailEnhancerUtils instance;
    return instance;
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
std::shared_ptr<Drawing::Surface> DetailEnhancerUtils::InitSurface(int dstWidth, int dstHeight,
    sptr<SurfaceBuffer>& dstSurfaceBuffer, const std::shared_ptr<Drawing::Image>& image)
{
    if (!dstSurfaceBuffer) {
        RS_LOGE("DetailEnhancerUtils InitSurface failed, dstSurfaceBuffer is invalid!");
        return nullptr;
    }
    if (!image) {
        RS_LOGE("DetailEnhancerUtils InitSurface failed, image is invalid!");
        return nullptr;
    }
    Drawing::ImageInfo imageInfoForRenderTarget(dstWidth, dstHeight, image->GetColorType(),
        image->GetAlphaType(), image->GetColorSpace());
    auto context = RsVulkanContext::GetSingleton().GetDrawingContext();
    std::unique_ptr<NativeBufferUtils::NativeSurfaceInfo> nativeSurfaceInfo =
        std::make_unique<NativeBufferUtils::NativeSurfaceInfo>();
    OHNativeWindowBuffer* nativeWindowBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&dstSurfaceBuffer);
    if (nativeSurfaceInfo == nullptr) {
        RS_LOGE("DetailEnhancerUtils InitSurface failed, nativeSurfaceInfo is invalid!");
        return nullptr;
    }
    if (nativeWindowBuffer == nullptr) {
        RS_LOGE("DetailEnhancerUtils InitSurface failed, nativeWindowBuffer is invalid!");
        return nullptr;
    }
    nativeSurfaceInfo->nativeWindowBuffer = nativeWindowBuffer;
    std::shared_ptr<Drawing::Surface> newSurface = NativeBufferUtils::CreateFromNativeWindowBuffer(context.get(),
        imageInfoForRenderTarget, *nativeSurfaceInfo);
    return newSurface;
}

std::shared_ptr<Drawing::Image> DetailEnhancerUtils::MakeImageFromSurfaceBuffer(
    sptr<SurfaceBuffer>& surfaceBuffer, const std::shared_ptr<Drawing::Image>& image)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        RS_LOGE("DetailEnhancerUtils MakeImageFromSurfaceBuffer failed, GpuApiType is not support!");
        return nullptr;
    }
    auto drawingContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
    std::shared_ptr<Drawing::GPUContext> gpuContext(drawingContext);
    if (!surfaceBuffer || !image || !gpuContext) {
        RS_LOGE("DetailEnhancerUtils MakeImageFromSurfaceBuffer failed, input is invalid!");
        return nullptr;
    }
    OHNativeWindowBuffer* nativeWindowBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer);
    if (!nativeWindowBuffer) {
        RS_LOGE("DetailEnhancerUtils MakeImageFromSurfaceBuffer failed, create native window buffer failed!");
        return nullptr;
    }
    Drawing::BackendTexture backendTexture = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
        nativeWindowBuffer, surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), false);
    DestroyNativeWindowBuffer(nativeWindowBuffer);
    if (!backendTexture.IsValid()) {
        RS_LOGE("DetailEnhancerUtils MakeImageFromSurfaceBuffer failed, backendTexture is invalid!");
        return nullptr;
    }
    auto vkTextureInfo = backendTexture.GetTextureInfo().GetVKTextureInfo();
    if (!vkTextureInfo) {
        RS_LOGE("DetailEnhancerUtils MakeImageFromSurfaceBuffer failed, vkTextureInfo is invalid!");
        return nullptr;
    }
    NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper = new NativeBufferUtils::VulkanCleanupHelper(
        RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    std::shared_ptr<Drawing::Image> dmaImage = std::make_shared<Drawing::Image>();
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::TOP_LEFT;
    image->GetBackendTexture(false, &origin);
    Drawing::BitmapFormat bitmapFormat = {GetColorTypeWithVKFormat(vkTextureInfo->format), image->GetAlphaType()};
    if (!dmaImage->BuildFromTexture(*gpuContext, backendTexture.GetTextureInfo(), origin,
        bitmapFormat, nullptr, NativeBufferUtils::DeleteVkImage, cleanUpHelper->Ref())) {
        RS_LOGE("DetailEnhancerUtils MakeImageFromSurfaceBuffer build image failed!");
        NativeBufferUtils::DeleteVkImage(cleanUpHelper);
        return nullptr;
    }
    NativeBufferUtils::DeleteVkImage(cleanUpHelper);
    return dmaImage;
}

Drawing::ColorType DetailEnhancerUtils::GetColorTypeWithVKFormat(VkFormat vkFormat)
{
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

bool DetailEnhancerUtils::SetMemoryName(sptr<SurfaceBuffer>& buffer)
{
    static std::string MemoryName = "asynscaling_hpae_memory";
    if (buffer == nullptr) {
        RS_LOGE("DetailEnhancerUtils SetMemoryName failed, buffer is invalid!");
        return false;
    }
    int fd = buffer->GetFileDescriptor();
    if (fd < 0) {
        return false;
    }
    int ret = -1;
#ifdef ROSEN_OHOS
    ret = TEMP_FAILURE_RETRY(ioctl(fd, DMA_BUF_SET_LEAK_TYPE, MemoryName.c_str()));
#endif
    return ret == 0;
}

sptr<SurfaceBuffer> DetailEnhancerUtils::CreateSurfaceBuffer(int width, int height)
{
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    if (surfaceBuffer == nullptr) {
        RS_LOGE("DetailEnhancerUtils CreateSurfaceBuffer failed!");
        return nullptr;
    }
    BufferRequestConfig bufConfig = {
        .width = static_cast<int32_t>(width),
        .height = static_cast<int32_t>(height),
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA |
            BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE,
        .timeout = 0,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    surfaceBuffer->Alloc(bufConfig);
    return surfaceBuffer;
}

void DetailEnhancerUtils::SavePixelmapToFile(Drawing::Bitmap& bitmap, const std::string& dst)
{
    int32_t w = bitmap.GetWidth();
    int32_t h = bitmap.GetHeight();
    int32_t rowStride = bitmap.GetRowBytes();
    int32_t totalSize = rowStride * h;
    std::string localTime = CommonTools::GetLocalTime();
    std::string fileName = dst + localTime + "_w" + std::to_string(w) + "_h" + std::to_string(h) +
        "_stride" + std::to_string(rowStride) + ".dat";
    std::ofstream outfile(fileName, std::fstream::out);
    if (!outfile.is_open()) {
        RS_LOGE("SavePixelmapToFile write error, path=%{public}s", fileName.c_str());
        return;
    }
    outfile.write(reinterpret_cast<const char*>(bitmap.GetPixels()), totalSize);
    outfile.close();
    RS_LOGI("SavePixelmapToFile write success, path=%{public}s", fileName.c_str());
}
#endif
} // OHOS
} // Rosen
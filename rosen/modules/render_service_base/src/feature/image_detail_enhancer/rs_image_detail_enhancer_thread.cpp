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

#include <chrono>
#include <fstream>
#include <thread>

#include "common/rs_common_tools.h"
#include "feature/image_detail_enhancer/rs_image_detail_enhancer_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

#ifdef ROSEN_OHOS
#include <iomanip>
#include <linux/dma-buf.h>
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
constexpr int CHANNELS_CNT = 4; // Number of channels
constexpr float MEMUNIT_RATE = 1024.0f; // Mem unit rate
constexpr float MAX_IMAGE_CACHE = 150.0f; // Max image cache
constexpr int MAX_SCALEUP_SIZE = 1920 * 1080; // Max scale up size
constexpr int RELEASE_TIME = 5000; // Release scaledImage time (ms)
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
constexpr int DEBUG_NUM_DIVISIOR = 2; // Use for dump debug info
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
    ifReleaseAllScaledImage_ = false;
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
        RS_LOGE("RSImageDetailEnhancerThread RegisterCallback failed, callback is invalid!");
        return false;
    }
    callback_ = callback;
    return true;
}

void RSImageDetailEnhancerThread::MarkScaledImageDirty(uint64_t nodeId)
{
    if (callback_ == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread MarkScaledImageDirty failed!");
        return;
    }
    callback_(nodeId);
}

bool RSImageDetailEnhancerThread::IsSizeSupported(int srcWidth, int srcHeight, int dstWidth, int dstHeight)
{
    // Return if target size is invalid or unchanged
    if (dstWidth <= 0 || dstHeight <= 0 || srcWidth == dstWidth || srcHeight == dstHeight) {
        return false;
    }

    // Limit upscale target to 1080P
    if (srcWidth < dstWidth && srcHeight < dstHeight && srcWidth * srcHeight > MAX_SCALEUP_SIZE) {
        return false;
    }

    // Check if source and target sizes fall within supported config range
    if (dstWidth > params_.minSize && dstHeight > params_.minSize &&
        srcWidth > params_.minSize && srcHeight > params_.minSize && dstWidth < params_.maxSize &&
        dstHeight < params_.maxSize && srcWidth < params_.maxSize && srcHeight < params_.maxSize) {
        float scaleX = static_cast<float>(dstWidth) / static_cast<float>(srcWidth);
        float scaleY = static_cast<float>(dstHeight) / static_cast<float>(srcHeight);
        if ((scaleX > params_.minScaleRatio && scaleY > params_.minScaleRatio) &&
            (scaleX < params_.maxScaleRatio && scaleY < params_.maxScaleRatio)) {
            RS_LOGD("RSImageDetailEnhancerThread size is supported, srcSize=(%{public}d, %{public}d), \
                dstSize=(%{public}d, %{public}d)", srcWidth, srcHeight, dstWidth, dstHeight);
            return true;
        }
    }
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
        IsSizeSupported(srcWidth, srcHeight, dstWidth, dstHeight)) {
        std::shared_ptr<Drawing::Image> dstImage = GetScaledImage(imageId);
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
        RS_LOGD("RSImageDetailEnhancerThread type is supported!");
        return true;
    }
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
    float scaleRatio = static_cast<float>(dstHeight) / static_cast<float>(srcHeight);
    int dstImageSize = dstWidth * dstHeight;
    if (scaleRatio >= slrParams_.rangeParams.front().rangeMin && dstImageSize <= slrParams_.maxSize &&
        dstImageSize >= slrParams_.minSize && scaleRatio <= slrParams_.rangeParams.back().rangeMax) {
        info.type = Drawing::HDSampleType::SLR;
        info.isUniformScale = false;
        if (!GetSharpness(slrParams_, scaleRatio, info.sharpness)) {
            return nullptr;
        }
    } else if (scaleRatio >= esrParams_.rangeParams.front().rangeMin && dstImageSize <= esrParams_.maxSize &&
        dstImageSize >= esrParams_.minSize && scaleRatio <= esrParams_.rangeParams.back().rangeMax) {
        info.type = Drawing::HDSampleType::ESR;
        if (!GetSharpness(esrParams_, scaleRatio, info.sharpness)) {
            return nullptr;
        }
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
    return dstImage;
}

void RSImageDetailEnhancerThread::ExecuteTaskAsync(int dstWidth, int dstHeight,
    const std::shared_ptr<Drawing::Image>& image, uint64_t nodeId, uint64_t imageId)
{
    RS_TRACE_NAME("RSImageDetailEnhancerThread::ExecuteTaskAsync");
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
    float scaleRatio = static_cast<float>(dstWidth) / static_cast<float>(srcWidth);
    if (scaleRatio > slrParams_.rangeParams.back().rangeMax) {
        dstImage = ScaleByAAE(dstSurfaceBuffer, image);
    }
    if (dstImage == nullptr) {
        dstImage = ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, image);
    }
    if (dstImage != nullptr) {
        RS_LOGD("RSImageDetailEnhancerThread SetOutImage, imageId=%{public}" PRIu64 ", srcSize=(%{public}d,%{public}d),"
            "dstSize=(%{public}d,%{public}d)", imageId, srcWidth, srcHeight, dstWidth, dstHeight);
        SetScaledImage(imageId, dstImage);
        DumpImage(dstImage, imageId);
        MarkScaledImageDirty(nodeId);
    }
}

// used for ScaleImageAsync
std::shared_ptr<Drawing::Image> RSImageDetailEnhancerThread::EnhanceImageAsync(bool& isScaledImageAsync,
    const RSImageParams& RSImageParams)
{
    if (!GetEnabled() || RSImageParams.mPixelMap == nullptr || RSImageParams.mImage == nullptr) {
        return nullptr;
    }
    if (!IsPidEnabled(RSImageParams.mNodeId)) {
        return nullptr;
    }
    ScaleImageAsync(RSImageParams.mPixelMap, RSImageParams.mDst,
        RSImageParams.mNodeId, RSImageParams.mUniqueId, RSImageParams.mImage);
    std::shared_ptr<Drawing::Image> dstImage = GetScaledImage(RSImageParams.mUniqueId);
    if (dstImage == nullptr) {
        isScaledImageAsync = false;
        return nullptr;
    }
    if (dstImage->GetWidth() == 0 || RSImageParams.mImage->GetWidth() == 0) {
        return nullptr;
    }
    float realDstWidth = RSImageParams.mDst.GetWidth() * RSImageParams.mMatrixScaleX;
    float newScaleRatio = realDstWidth / static_cast<float>(dstImage->GetWidth());
    float originScaleRatio = realDstWidth / static_cast<float>(RSImageParams.mImage->GetWidth());
    if (abs(originScaleRatio - 1.0f) < abs(newScaleRatio - 1.0f)) {
        isScaledImageAsync = false;
        return nullptr;
    }
    isScaledImageAsync = true;
    return dstImage;
}

void RSImageDetailEnhancerThread::ScaleImageAsync(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::Rect& dst, uint64_t nodeId, uint64_t imageId, const std::shared_ptr<Drawing::Image>& image)
{
    if (image == nullptr || !IsTypeSupport(pixelMap)) {
        return;
    }
    RS_TRACE_NAME("RSImageDetailEnhancerThread::ScaleImageAsync");
    int srcWidth = image->GetWidth();
    int srcHeight = image->GetHeight();
    int dstWidth = static_cast<int>(dst.GetWidth());
    int dstHeight = static_cast<int>(dst.GetHeight());
    ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);
    if (IsSizeSupported(srcWidth, srcHeight, dstWidth, dstHeight) && !GetProcessStatus(imageId)) {
        SetProcessStatus(imageId, true);
        auto asyncEnhancerTask = [this, dstWidth, dstHeight, image, nodeId, imageId]() {
            ExecuteTaskAsync(dstWidth, dstHeight, image, nodeId, imageId);
        };
        RSImageDetailEnhancerThread::Instance().PostTask(asyncEnhancerTask);
    }
}

void RSImageDetailEnhancerThread::DumpImage(const std::shared_ptr<Drawing::Image>& image, uint64_t imageId)
{
    if (!RSSystemProperties::GetDumpUICaptureEnabled() || image == nullptr) {
        return;
    }
    RS_TRACE_NAME("RSImageDetailEnhancerThread::DumpImage");
    int width = image->GetWidth();
    int height = image->GetHeight();
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(width, height, format);
    image->ReadPixels(bitmap, 0, 0);
    DetailEnhancerUtils::Instance().SavePixelmapToFile(bitmap, "/data/scaledPixelmap_");
}

void RSImageDetailEnhancerThread::ImageSamplingDump(uint64_t imageId)
{
    RS_TRACE_NAME("RSImageDetailEnhancerThread::ImageSamplingDump");
    std::shared_ptr<Drawing::Image> image = GetScaledImage(imageId);
    if (image == nullptr) {
        return;
    }
    int width = image->GetWidth();
    int height = image->GetHeight();
    auto asyncEnhancerTask = [this, width, height, image, imageId]() {
        RS_TRACE_NAME("RSImageDetailEnhancerThread::ImageSamplingDumpAsync");
        Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888,
            Drawing::AlphaType::ALPHATYPE_PREMUL };
        Drawing::Bitmap bitmap;
        bitmap.Build(width, height, format);
        image->ReadPixels(bitmap, 0, 0);
        std::stringstream oss;
        int32_t w = bitmap.GetWidth();
        int32_t h = bitmap.GetHeight();
        oss << "[ imageId:" << imageId << " Width:" << w << " Height:" << h;
        oss << " pixels:" << std::hex << std::uppercase;
        int32_t widthStep = std::max((w / DEBUG_NUM_DIVISIOR) - 1, 1);
        int32_t heightStep = std::max((h / DEBUG_NUM_DIVISIOR) - 1, 1);
        for (int32_t i = 1; i < w; i += widthStep) {
            for (int32_t j = 1; j < h; j += heightStep) {
                uint32_t pixel = static_cast<uint32_t>(bitmap.GetColor(i, j));
                oss << " ARGB-0x" << pixel;
            }
        }
        oss << ']';
        RS_LOGI("ScaledImage SamplingDump: %{public}s", oss.str().c_str());
    };
    RSImageDetailEnhancerThread::Instance().PostTask(asyncEnhancerTask);
}
#endif

void RSImageDetailEnhancerThread::SetScaledImage(uint64_t imageId, const std::shared_ptr<Drawing::Image>& image)
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    if (image == nullptr) {
        RS_LOGE("RSImageDetailEnhancerThread image is nullptr!");
        return;
    }
    if (keyMap_.find(imageId) != keyMap_.end()) {
        std::shared_ptr<Drawing::Image> cachedImage = keyMap_[imageId]->second;
        curCache_ -= DetailEnhancerUtils::Instance().GetImageSize(cachedImage);
        imageList_.erase(keyMap_[imageId]);
    }
    PushImageList(imageId, image);
    RS_LOGD("RSImageDetailEnhancerThreadTest SetOutImage2,imageId=%{public}" PRIu64
        ",mapSize=%{public}zu, curCacheSize2=%{public}f M ", imageId, imageList_.size(), curCache_);
}

void RSImageDetailEnhancerThread::PushImageList(uint64_t imageId, const std::shared_ptr<Drawing::Image>& image)
{
    processStatusMap_[imageId] = true;
    processReadyMap_[imageId] = true;
    curCache_ += DetailEnhancerUtils::Instance().GetImageSize(image);
    while (static_cast<int>(curCache_) >= static_cast<int>(MAX_IMAGE_CACHE) && imageList_.size() > 0) {
        std::shared_ptr<Drawing::Image> cachedImage = imageList_.back().second;
        if (cachedImage) {
            curCache_ -= DetailEnhancerUtils::Instance().GetImageSize(cachedImage);
        }
        keyMap_.erase(imageList_.back().first);
        imageList_.pop_back();
    }
    imageList_.emplace_front(imageId, image);
    keyMap_[imageId] = imageList_.begin();
}

std::shared_ptr<Drawing::Image> RSImageDetailEnhancerThread::GetScaledImage(uint64_t imageId)
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    if (keyMap_.find(imageId) == keyMap_.end()) {
        return nullptr;
    }
    imageList_.splice(imageList_.begin(), imageList_, keyMap_[imageId]);
    return keyMap_[imageId]->second;
}

void RSImageDetailEnhancerThread::ReleaseScaledImage(uint64_t imageId)
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    if (keyMap_.find(imageId) != keyMap_.end()) {
        std::shared_ptr<Drawing::Image> cachedImage = keyMap_[imageId]->second;
        curCache_ -= DetailEnhancerUtils::Instance().GetImageSize(cachedImage);
        imageList_.erase(keyMap_[imageId]);
        keyMap_.erase(imageId);
        processStatusMap_.erase(imageId);
        processReadyMap_.erase(imageId);
        RS_LOGD("RSImageDetailEnhancerThreadTest ReleaseScaledImage, imageId=%{public}" PRIu64
            ",mapSize=%{public}zu, curCacheSize=%{public}f M", imageId, imageList_.size(), curCache_);
    }
}

void RSImageDetailEnhancerThread::ReleaseAllScaledImage()
{
    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    if (imageList_.size() == 0) {
        return;
    }
    if (releaseTime_ == 0) {
        releaseTime_ = DetailEnhancerUtils::Instance().GetCurTime();
    }
    if ((DetailEnhancerUtils::Instance().GetCurTime() - releaseTime_) > RELEASE_TIME) {
        curCache_ = 0;
        imageList_.clear();
        keyMap_.clear();
        processStatusMap_.clear();
        processReadyMap_.clear();
        releaseTime_ = 0;
        RS_LOGE("RSImageDetailEnhancerThreadTest ReleaseAllScaledImage");
    }
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

bool RSImageDetailEnhancerThread::GetEnabled()
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR &&
        RSSystemProperties::GetScaleImageAsyncEnabled() &&
        RSSystemProperties::GetMemoryWatermarkEnabled() &&
        isParamValidate_) {
        ifReleaseAllScaledImage_ = true;
        return true;
    }
    if (ifReleaseAllScaledImage_) {
        ReleaseAllScaledImage();
    }
    ifReleaseAllScaledImage_ = false;
    return false;
#endif
    return false;
}

bool RSImageDetailEnhancerThread::IsPidEnabled(uint64_t nodeId)
{
    pid_t pid = ExtractPid(nodeId);
    const std::unordered_set<pid_t>& imageEnhancePidList = RsCommonHook::Instance().GetImageEnhancePidList();
    if (imageEnhancePidList.empty()) {
        ReleaseAllScaledImage();
    }
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
    std::shared_ptr<Drawing::Surface> newSurface = NativeBufferUtils::CreateFromNativeWindowBufferImpl(context.get(),
        imageInfoForRenderTarget, *nativeSurfaceInfo, image->GetColorSpace());
    return newSurface;
}

std::shared_ptr<Drawing::Image> DetailEnhancerUtils::MakeImageFromSurfaceBuffer(sptr<SurfaceBuffer>& surfaceBuffer,
    const std::shared_ptr<Drawing::Image>& image)
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
        bitmapFormat, image->GetColorSpace(), NativeBufferUtils::DeleteVkImage, cleanUpHelper->Ref())) {
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

float DetailEnhancerUtils::GetImageSize(const std::shared_ptr<Drawing::Image>& image) const
{
    if (image == nullptr) {
        return 0.0f;
    }
    return static_cast<float>(image->GetWidth() * image->GetHeight()
        * CHANNELS_CNT / MEMUNIT_RATE / MEMUNIT_RATE);
}

long long DetailEnhancerUtils::GetCurTime() const
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return ms.time_since_epoch().count();
}
} // OHOS
} // Rosen
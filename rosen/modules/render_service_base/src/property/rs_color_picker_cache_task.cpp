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

#include "property/rs_color_picker_cache_task.h"

#include <atomic>

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
#define CHECK_CACHE_PROCESS_STATUS                                       \
    do {                                                                 \
        if (cacheProcessStatus_.load() == CacheProcessStatus::WAITING) { \
            return false;                                                \
        }                                                                \
    } while (false)

const bool RSColorPickerCacheTask::ColorPickerPartialEnabled =
    RSSystemProperties::GetColorPickerPartialEnabled() && RSUniRenderJudgement::IsUniRender();

bool RSColorPickerCacheTask::InitSurface(Drawing::GPUContext* gpuContext)
{
    RS_TRACE_NAME("RSColorPickerCacheTask InitSurface");
    if (cacheSurface_ != nullptr) {
        return true;
    }
#ifdef IS_OHOS
    auto runner = AppExecFwk::EventRunner::Current();
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
#endif
    Drawing::ImageInfo info = Drawing::ImageInfo::MakeN32Premul(surfaceSize_.GetWidth(), surfaceSize_.GetHeight());
    cacheSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext, true, info);

    return cacheSurface_ != nullptr;
}

bool RSColorPickerCacheTask::InitTask(const std::shared_ptr<Drawing::Image> imageSnapshot)
{
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSColorPickerCacheTask imageSnapshot is null");
        return false;
    }
    if (imageSnapshotCache_) {
        cacheBackendTexture_ = imageSnapshotCache_->GetBackendTexture(false, nullptr);
        return true;
    }
    ROSEN_LOGD("RSColorPickerCacheTask InitTask:%{public}p", this);
#ifdef IS_OHOS
    auto runner = AppExecFwk::EventRunner::Current();
    initHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
#endif
    imageSnapshotCache_ = imageSnapshot;
    surfaceSize_.SetRight(imageSnapshotCache_->GetWidth());
    surfaceSize_.SetBottom(imageSnapshotCache_->GetHeight());
    return false;
}

void RSColorPickerCacheTask::Reset()
{
    ROSEN_LOGD("RSColorPickerCacheTask::Reset:%{public}p", this);
    {
        std::unique_lock<std::mutex> lock(*grBackendTextureMutex_);
        if (!imageSnapshotCache_) {
            return;
        }
        imageSnapshotCache_.reset();
        waitRelease_->store(false);
    }
}

bool RSColorPickerCacheTask::GpuScaleImage(std::shared_ptr<RSPaintFilterCanvas> cacheCanvas,
    const std::shared_ptr<Drawing::Image> threadImage, std::shared_ptr<Drawing::Pixmap>& dst)
{
    if (cacheCanvas == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSColorPickerCacheTask cacheCanvas is null");
        return false;
    }
    std::string shaderString(R"(
        uniform shader imageInput;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            return half4(c.rgb, 1.0);
        }
    )");

    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(shaderString);
    if (!effect) {
        ROSEN_LOGE("RSColorPickerCacheTask effect is null");
        SetStatus(CacheProcessStatus::WAITING);
        return false;
    }

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> effectBulider =
        std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    Drawing::ImageInfo pcInfo;
    Drawing::Matrix matrix;
    matrix.SetScale(1.0, 1.0);
    if (threadImage->GetWidth() * threadImage->GetHeight() < 10000) { // 10000 = 100 * 100 pixels
        pcInfo = Drawing::ImageInfo::MakeN32Premul(10, 10); // 10 * 10 pixels
        matrix.SetScale(10.0 / threadImage->GetWidth(), 10.0 / threadImage->GetHeight()); // 10.0 pixels
    } else {
        pcInfo = Drawing::ImageInfo::MakeN32Premul(100, 100); // 100 * 100 pixels
        matrix.SetScale(100.0 / threadImage->GetWidth(), 100.0 / threadImage->GetHeight());  // 100.0 pixels
    }
    effectBulider->SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(
        *threadImage, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, matrix));
    std::shared_ptr<Drawing::Image> tmpColorImg = effectBulider->MakeImage(
        cacheCanvas->GetGPUContext().get(), nullptr, pcInfo, false);

    const int buffLen = tmpColorImg->GetWidth() * tmpColorImg->GetHeight();
    if (pixelPtr_ != nullptr) {
        delete [] pixelPtr_;
        pixelPtr_ = nullptr;
    }
    pixelPtr_ = new uint32_t[buffLen];

    auto info = tmpColorImg->GetImageInfo();
    dst = std::make_shared<Drawing::Pixmap>(info, pixelPtr_, info.GetWidth() * info.GetBytesPerPixel());
    bool flag = tmpColorImg->ReadPixels(*dst, 0, 0);

    return flag;
}

bool RSColorPickerCacheTask::Render()
{
    RS_TRACE_NAME_FMT("RSColorPickerCacheTask::Render:%p", this);
    ROSEN_LOGD("RSColorPickerCacheTask::Render:%{public}p", this);
    if (cacheSurface_ == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSColorPickerCacheTask cacheSurface is null");
        return false;
    }
    CHECK_CACHE_PROCESS_STATUS;
    std::shared_ptr<RSPaintFilterCanvas> cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface_.get());
    CHECK_CACHE_PROCESS_STATUS;
    auto threadImage = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat info = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888,
        Drawing::ALPHATYPE_PREMUL };
    {
        std::unique_lock<std::mutex> lock(*grBackendTextureMutex_);
        if (cacheCanvas == nullptr || !cacheBackendTexture_.IsValid()) {
            SetStatus(CacheProcessStatus::WAITING);
            ROSEN_LOGE("RSColorPickerCacheTask cacheCanvas is null or cacheBackendTexture not valid");
            return false;
        }
        if (!threadImage->BuildFromTexture(*cacheCanvas->GetGPUContext(), cacheBackendTexture_.GetTextureInfo(),
            Drawing::TextureOrigin::BOTTOM_LEFT, info, nullptr)) {
            SetStatus(CacheProcessStatus::WAITING);
            ROSEN_LOGE("RSColorPickerCacheTask::Render BuildFromTexture failed");
            return false;
        }
    }
    CHECK_CACHE_PROCESS_STATUS;
    Drawing::ColorQuad color;
    std::shared_ptr<Drawing::Pixmap> dst;
    if (GpuScaleImage(cacheCanvas, threadImage, dst)) {
        uint32_t errorCode = 0;
        CHECK_CACHE_PROCESS_STATUS;
        std::shared_ptr<RSColorPicker> colorPicker = RSColorPicker::CreateColorPicker(dst, errorCode);
        if (errorCode == 0) {
            if (isShadow_ && shadowColorStrategy_ == SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN) {
                colorPicker->GetLargestProportionColor(color);
            } else {
                colorPicker->GetAverageColor(color);
            }
            std::unique_lock<std::mutex> lock(colorMutex_);
            color_ = RSColor(Drawing::Color::ColorQuadGetR(color), Drawing::Color::ColorQuadGetG(color),
                Drawing::Color::ColorQuadGetB(color), Drawing::Color::ColorQuadGetA(color));
            firstGetColorFinished_ = true;
            valid_ = true;
        } else {
            valid_ = false;
        }
    } else {
        valid_ = false;
    }
    if (pixelPtr_ != nullptr) {
        delete [] pixelPtr_;
        pixelPtr_ = nullptr;
    }

    CHECK_CACHE_PROCESS_STATUS;
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    cacheProcessStatus_.store(CacheProcessStatus::DONE);
    Notify();
    return true;
}
bool RSColorPickerCacheTask::GetColor(RSColor& color)
{
    std::unique_lock<std::mutex> lock(colorMutex_);
    color = color_;
    return valid_;
}

void RSColorPickerCacheTask::CalculateColorAverage(RSColor& colorCur)
{
    // black color defination
    RSColor black = RSColor(0, 0, 0, 255);
    int colorArrayLen = colorArray_.size();
    int colorArraySize = 20;
    int continueBlackColorNum = 5;
    if (colorArrayLen >= colorArraySize) {
        colorArray_.pop_back();
    }
    colorArray_.emplace(colorArray_.begin(), colorCur);
    int validColorNum = 0;
    int R = 0;
    int G = 0;
    int B = 0;
    int mark = 0;

    for (int i = 0; i < static_cast<int>(colorArray_.size()); i++) {
        if (colorArray_[i].GetRed() == 0 && colorArray_[i].GetGreen() == 0 && colorArray_[i].GetBlue() == 0) {
            ++mark;
        } else {
            if (mark > continueBlackColorNum) {
                R += black.GetRed() * mark;
                G += black.GetGreen() * mark;
                B += black.GetBlue() * mark;
                validColorNum += mark;
            }
            R += colorArray_[i].GetRed();
            G += colorArray_[i].GetGreen();
            B += colorArray_[i].GetBlue();
            validColorNum++;
            mark = 0;
        }
    }

    if (mark > continueBlackColorNum) {
        R += black.GetRed() * mark;
        G += black.GetGreen() * mark;
        B += black.GetBlue() * mark;
        validColorNum += mark;
    }
    
    if (validColorNum != 0) {
        R = R / validColorNum;
        G = G / validColorNum;
        B = B / validColorNum;
    } else {
        colorAverage_ = colorCur;
    }

    colorAverage_ = RSColor(R, G, B, colorCur.GetAlpha());
}

void RSColorPickerCacheTask::GetColorAverage(RSColor& color)
{
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    CalculateColorAverage(color_);
    color = colorAverage_;
}

bool RSColorPickerCacheTask::GetFirstGetColorFinished()
{
    return firstGetColorFinished_;
}

std::function<void(std::weak_ptr<RSColorPickerCacheTask>)> RSColorPickerCacheTask::postColorPickerTask = nullptr;
#ifdef IS_OHOS
    std::function<void(std::shared_ptr<Drawing::Image> &&,
        std::shared_ptr<Drawing::Surface> &&,
        std::shared_ptr<OHOS::AppExecFwk::EventHandler>,
        std::weak_ptr<std::atomic<bool>>,
        std::weak_ptr<std::mutex>)> RSColorPickerCacheTask::saveImgAndSurToRelease = nullptr;
#endif

bool RSColorPickerCacheTask::PostPartialColorPickerTask(std::shared_ptr<RSColorPickerCacheTask> colorPickerTask,
    std::shared_ptr<Drawing::Image> imageSnapshot)
{
    ROSEN_LOGD("RSColorPickerCacheTask::PostPartialColorPickerTask:%{public}p", colorPickerTask.get());
    if (RSColorPickerCacheTask::postColorPickerTask == nullptr) {
        ROSEN_LOGD("PostPartialColorPickerTask::postColorPickerTask is null\n");
        return false;
    }

    if (colorPickerTask == nullptr) {
        ROSEN_LOGE("PostPartialColorPickerTask::colorPickerTask is null\n");
        return false;
    }

    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("PostPartialColorPickerTask::imageSnapshot is null\n");
        return false;
    }

    if (colorPickerTask->GetStatus() == CacheProcessStatus::WAITING && !colorPickerTask->GetWaitRelease()) {
        if (colorPickerTask->InitTask(imageSnapshot)) {
            ROSEN_LOGD("PostPartialColorPickerTask, init task");
            colorPickerTask->SetStatus(CacheProcessStatus::DOING);
            RSColorPickerCacheTask::postColorPickerTask(colorPickerTask);
        }
        return false;
    } else if (colorPickerTask->GetStatus() == CacheProcessStatus::DONE) {
        ROSEN_LOGD("PostPartialColorPickerTask, DONE");
        #ifdef IS_OHOS
            colorPickerTask->SetWaitRelease(true);
            auto initHandler = colorPickerTask->GetInitHandler();
            if (initHandler != nullptr) {
                auto task = colorPickerTask;
                ROSEN_LOGD("CacheProcessStatus::DONE, Reset():%{public}p", task.get());
                initHandler->PostTask(
                    [task]() { task->Reset(); }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
            }
        #endif
        return true;
    } else {
        ROSEN_LOGD("PostPartialColorPickerTask, doing or wait release");
        return false;
    }
}

void RSColorPickerCacheTask::SetDeviceSize(int& deviceWidth, int& deviceHeight)
{
    deviceWidth_ = deviceWidth;
    deviceHeight_ = deviceHeight;
}

void RSColorPickerCacheTask::SetIsShadow(bool isShadow)
{
    isShadow_ = isShadow;
}

void RSColorPickerCacheTask::SetShadowColorStrategy(int shadowColorStrategy)
{
    shadowColorStrategy_ = shadowColorStrategy;
}

void RSColorPickerCacheTask::SetWaitRelease(bool waitRelease)
{
    waitRelease_->store(waitRelease);
}

bool RSColorPickerCacheTask::GetDeviceSize(int& deviceWidth, int& deviceHeight) const
{
    if (deviceWidth_.has_value() && deviceHeight_.has_value()) {
        deviceWidth = deviceWidth_.value();
        deviceHeight = deviceHeight_.value();
        return true;
    }
    return false;
}

bool RSColorPickerCacheTask::GetWaitRelease() const
{
    return waitRelease_->load();
}

void RSColorPickerCacheTask::ReleaseColorPicker()
{
#ifdef IS_OHOS
    waitRelease_->store(true);
    cacheProcessStatus_.store(CacheProcessStatus::WAITING);
    if (imageSnapshotCache_ || cacheSurface_ || initHandler_ || handler_) {
        ROSEN_LOGD("RSColorPickerCacheTask::ReleaseColorPicker:%{public}p", this);
        RSColorPickerCacheTask::saveImgAndSurToRelease(std::move(imageSnapshotCache_), std::move(cacheSurface_),
            initHandler_, waitRelease_, grBackendTextureMutex_);
    }
#endif
}

} // namespace Rosen
} // namespace OHOS


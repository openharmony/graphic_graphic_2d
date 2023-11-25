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

#include "include/gpu/GrBackendSurface.h"
#include "src/image/SkImage_Base.h"

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

#ifndef USE_ROSEN_DRAWING
bool RSColorPickerCacheTask::InitSurface(GrRecordingContext* grContext)
{
    RS_TRACE_NAME("RSColorPickerCacheTask InitSurface");
    if (cacheSurface_ != nullptr) {
        ROSEN_LOGE("RSColorPickerCacheTask cacheSurface is null");
        return true;
    }
#ifdef IS_OHOS
    auto runner = AppExecFwk::EventRunner::Current();
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
#endif
    SkImageInfo info = SkImageInfo::MakeN32Premul(surfaceSize_.width(), surfaceSize_.height());
    cacheSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kYes, info);

    return cacheSurface_ != nullptr;
}
#endif

bool RSColorPickerCacheTask::InitTask(const sk_sp<SkImage> imageSnapshot)
{
    RS_TRACE_NAME("RSColorPickerCacheTask InitTask");
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSColorPickerCacheTask imageSnapshot is null");
        return false;
    }
    if (imageSnapshotCache_) {
        cacheBackendTexture_ = imageSnapshotCache_->getBackendTexture(false);
        return true;
    }
    imageSnapshotCache_ = imageSnapshot;
    surfaceSize_.set(imageSnapshotCache_->width(), imageSnapshotCache_->height());
    return false;
}

bool RSColorPickerCacheTask::GpuScaleImage(std::shared_ptr<RSPaintFilterCanvas> cacheCanvas,
    const sk_sp<SkImage> threadImage, std::shared_ptr<SkPixmap>& dst)
{
    if (cacheCanvas == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSColorPickerCacheTask cacheCanvas is null");
        return false;
    }
    SkString shaderString(R"(
        uniform shader imageInput;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            return half4(c.rgb, 1.0);
        }
    )");

    auto [effect, error] = SkRuntimeEffect::MakeForShader(shaderString);
    if (!effect) {
        ROSEN_LOGE("RSColorPickerCacheTask effect is null");
        SetStatus(CacheProcessStatus::WAITING);
        return false;
    }

    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    SkRuntimeShaderBuilder effectBulider(effect);
    SkImageInfo pcInfo;
    auto matrix = SkMatrix::Scale(1.0, 1.0);
    if (threadImage->width() * threadImage->height() < 10000) { // 10000 = 100 * 100 pixels
        pcInfo = SkImageInfo::MakeN32Premul(10, 10); // 10 * 10 pixels
        matrix = SkMatrix::Scale(10.0 / threadImage->width(), 10.0 / threadImage->height()); // 10.0 pixels
    } else {
        pcInfo = SkImageInfo::MakeN32Premul(100, 100); // 100 * 100 pixels
        matrix = SkMatrix::Scale(100.0 / threadImage->width(), 100.0 / threadImage->height());  // 100.0 pixels
    }
    effectBulider.child("imageInput") = threadImage->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, matrix);
    sk_sp<SkImage> tmpColorImg = effectBulider.makeImage(cacheCanvas->recordingContext(), nullptr, pcInfo, false);

    const int buffLen = tmpColorImg->width() * tmpColorImg->height();
    if (pixelPtr_ != nullptr) {
        delete [] pixelPtr_;
        pixelPtr_ = nullptr;
    }
    pixelPtr_ = new uint32_t[buffLen];

    auto info = tmpColorImg->imageInfo();
    dst = std::make_shared<SkPixmap>(info, pixelPtr_, info.width() * info.bytesPerPixel());
    bool flag = tmpColorImg->readPixels(*dst, 0, 0);

    return flag;
}

bool RSColorPickerCacheTask::Render()
{
    RS_TRACE_NAME_FMT("RSColorPickerCacheTask::Render:%p", this);
    if (cacheSurface_ == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSColorPickerCacheTask cacheSurface is null");
        return false;
    }
    CHECK_CACHE_PROCESS_STATUS;
    std::shared_ptr<RSPaintFilterCanvas> cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface_.get());
    if (cacheCanvas == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSColorPickerCacheTask cacheCanvas is null");
        return false;
    }

    auto threadImage = SkImage::MakeFromTexture(cacheCanvas->recordingContext(), cacheBackendTexture_,
        kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);

    SkColor color;
    std::shared_ptr<SkPixmap> dst;
    if (GpuScaleImage(cacheCanvas, threadImage, dst)) {
        uint32_t errorCode = 0;
        std::shared_ptr<RSColorPicker> colorPicker = RSColorPicker::CreateColorPicker(dst, errorCode);
        if (errorCode == 0) {
            if (isShadow_ && shadowColorStrategy_ == SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN) {
                colorPicker->GetLargestProportionColor(color);
            } else {
                colorPicker->GetAverageColor(color);
            }
            std::unique_lock<std::mutex> lock(colorMutex_);
            color_ = RSColor(SkColorGetR(color), SkColorGetG(color), SkColorGetB(color), SkColorGetA(color));
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
    int colorArraySize = 21;
    int continueBlackColorNum = 20;
    if (colorArrayLen >= colorArraySize) {
        colorArray_.pop_back();
    }
    colorArray_.emplace(colorArray_.begin(), colorCur);
    int validColorNum = 0;
    int R = 0;
    int G = 0;
    int B = 0;
    int mark = 0;

    for (int i = 0; i < colorArrayLen; i++) {
        if (colorArray_[i] == black) {
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


void RSColorPickerCacheTask::ResetGrContext()
{
    if (cacheSurface_ != nullptr) {
        auto recordingContext = cacheSurface_->recordingContext();
        if (recordingContext != nullptr) {
            GrDirectContext* grContext = recordingContext->asDirectContext();
            if (grContext != nullptr) {
                cacheSurface_ = nullptr;
                grContext->freeGpuResources();
            }
        }
    }
}

std::function<void(std::weak_ptr<RSColorPickerCacheTask>)> RSColorPickerCacheTask::postColorPickerTask = nullptr;

bool RSColorPickerCacheTask::PostPartialColorPickerTask(std::shared_ptr<RSColorPickerCacheTask> colorPickerTask,
    sk_sp<SkImage> imageSnapshot)
{
    if (RSColorPickerCacheTask::postColorPickerTask == nullptr) {
        ROSEN_LOGE("PostPartialColorPickerTask::postColorPickerTask is null\n");
        return false;
    }

    if (colorPickerTask == nullptr) {
        ROSEN_LOGE("PostPartialColorPickerTask::colorPickerTask is null\n");
        return false;
    }

    if (colorPickerTask->GetStatus() == CacheProcessStatus::WAITING) {
        if (colorPickerTask->InitTask(imageSnapshot)) {
            ROSEN_LOGD("PostPartialColorPickerTask, init task");
            colorPickerTask->SetStatus(CacheProcessStatus::DOING);
            RSColorPickerCacheTask::postColorPickerTask(colorPickerTask);
        }
        return false;
    } else if (colorPickerTask->GetStatus() == CacheProcessStatus::DONE) {
        ROSEN_LOGD("PostPartialColorPickerTask, done");
        colorPickerTask->Reset();
        return true;
    } else {
        ROSEN_LOGD("PostPartialColorPickerTask, doing");
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

bool RSColorPickerCacheTask::GetDeviceSize(int& deviceWidth, int& deviceHeight) const
{
    if (deviceWidth_.has_value() && deviceHeight_.has_value()) {
        deviceWidth = deviceWidth_.value();
        deviceHeight = deviceHeight_.value();
        return true;
    }
    return false;
}


} // namespace Rosen
} // namespace OHOS


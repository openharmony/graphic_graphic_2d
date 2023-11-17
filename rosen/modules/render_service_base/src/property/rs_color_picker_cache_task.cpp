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

RSColorPickerCacheTask::~RSColorPickerCacheTask()
{
    ReleaseCacheOffTree();
}

#ifndef USE_ROSEN_DRAWING
bool RSColorPickerCacheTask::InitSurface(GrRecordingContext* grContext)
{
    RS_TRACE_NAME("RSColorPickerCacheTask InitSurface");
    if (cacheSurface_ != nullptr) {
        ROSEN_LOGE("RSColorPickerCacheTask cacheSurface is null");
        return true;
    }
    auto runner = AppExecFwk::EventRunner::Current();
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    SkImageInfo info = SkImageInfo::MakeN32Premul(imageSnapshotCache_->width(), imageSnapshotCache_->height());
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
    return false;
}

bool RSColorPickerCacheTask::GpuScaleImage(const sk_sp<SkImage> threadImage, std::shared_ptr<SkPixmap>& dst)
{
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface_.get());
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
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface_.get());
    if (cacheCanvas == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSColorPickerCacheTask cacheCanvas is null");
        return false;
    }

    auto threadImage = SkImage::MakeFromTexture(cacheCanvas->recordingContext(), cacheBackendTexture_,
        kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);

    SkColor color;
    std::shared_ptr<SkPixmap> dst;
    if (GpuScaleImage(threadImage, dst)) {
        uint32_t errorCode = 0;
        std::shared_ptr<RSColorPicker> colorPicker = RSColorPicker::CreateColorPicker(dst, errorCode);
        if (errorCode == 0) {
            colorPicker->GetAverageColor(color);
            std::unique_lock<std::mutex> lock(parallelRenderMutex_);
            color_ = RSColor(SkColorGetR(color), SkColorGetG(color), SkColorGetB(color), SkColorGetA(color));
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
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    color = color_;
    return valid_;
}

void RSColorPickerCacheTask::ResetGrContext()
{
    if (cacheSurface_ != nullptr) {
        GrDirectContext* grContext_ = cacheSurface_->recordingContext()->asDirectContext();
        cacheSurface_ = nullptr;
        grContext_->freeGpuResources();
    }
}

void RSColorPickerCacheTask::ReleaseCacheOffTree()
{
    SetStatus(CacheProcessStatus::WAITING);
    Reset();
    if (handler_ != nullptr) {
        handler_->PostTask([this]() {ResetGrContext();}, AppExecFwk::EventQueue::Priority::IMMEDIATE);
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

} // namespace Rosen
} // namespace OHOS


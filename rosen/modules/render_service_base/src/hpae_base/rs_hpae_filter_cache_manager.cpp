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

#include "hpae_base/rs_hpae_filter_cache_manager.h"

#if defined(ASYNC_BUILD_TASK) && defined(ROSEN_OHOS)
#include "ffrt_inner.h"
#endif

#include "ge_render.h"
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_fusion_operator.h"
#include "hpae_base/rs_hpae_hianimation.h"
#include "hpae_base/rs_hpae_log.h"
#include "hpae_base/rs_hpae_scheduler.h"
#include "unistd.h"

#include "common/rs_background_thread.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

static constexpr float EPSILON_F = 0.001f;
static const int HPAE_CACHE_SIZE = 3; // for Set() before Get(), N-2 require a queue size of 3

static bool FloatEqual(float a, float b)
{
    return std::fabs(a - b) < EPSILON_F;
}

int RSHpaeFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSFilter>& rsFilter,
    bool shouldClearFilteredCache, const std::optional<Drawing::RectI>& srcRect,
    const std::optional<Drawing::RectI>& dstRect)
{
    inputBufferInfo_ = RSHpaeBaseData::GetInstance().RequestHpaeInputBuffer();
    outputBufferInfo_ = RSHpaeBaseData::GetInstance().RequestHpaeOutputBuffer();
    if (inputBufferInfo_.canvas == nullptr || outputBufferInfo_.canvas == nullptr) {
        HPAE_LOGE("invalid canvas");
        ReleaseHpaeBufferInfo();
        return -1;
    }

    Drawing::RectI clipBounds = canvas.GetDeviceClipBounds();
    Drawing::RectI src = canvas.GetRoundInDeviceClipBounds();
    if (clipBounds.IsEmpty() || src.IsEmpty()) {
        HPAE_LOGE("canvas rect is empty");
        ReleaseHpaeBufferInfo();
        return -1; // -1 or 0?
    }

    RSHpaeScheduler::GetInstance().Reset();

    if (RSHpaeBaseData::GetInstance().GetNeedReset()) {
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        RS_OPTIONAL_TRACE_NAME("Clear for first frame");
        hpaeBlurOutputQueue_.clear();
        backgroundRadius_ = 0;
        prevBlurImage_ = nullptr;
        RSHpaeBaseData::GetInstance().SetResetDone();
    }

    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);

    int32_t radiusI = (int)RSHpaeBaseData::GetInstance().GetBlurRadius();
    curRadius_ = radiusI; // use int radius

    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    }

    drawUsingGpu_ = CheckIfUsingGpu();

    int ret = -1;
    if (DrawFilterImpl(filter, clipBounds, src, shouldClearFilteredCache) == 0) {
        ret = DrawBackgroundToCanvas(canvas);
    }

    ReleaseHpaeBufferInfo();

    return ret;
};

bool RSHpaeFilterCacheManager::CheckIfUsingGpu()
{
    bool usingGpu = false;
    {
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        if (hpaeBlurOutputQueue_.empty()) {
            usingGpu = true;
        }
    }

    // wait here, before using previous blur result
    if (usingGpu == false && !HianimationManager::GetInstance().WaitPreviousTask()) {
        HPAE_LOGW("Hianimation resource not enough");
        usingGpu = true;
    }

    return usingGpu;
}

int RSHpaeFilterCacheManager::DrawFilterImpl(const std::shared_ptr<RSDrawingFilter>& filter,
    const Drawing::RectI& clipBounds, const Drawing::RectI& src, bool shouldClearFilteredCache)
{
    if (cachedFilteredSnapshot_ != nullptr) { // notify to using cache
        return BlurUsingFilteredSnapshot();
    }

    if (!cachedSnapshot_) {
        HPAE_LOGE("clear background snapshot is nullptr");
        return -1;
    }

    return BlurWithoutSnapshot(clipBounds, cachedSnapshot_->cachedImage_, filter, src, shouldClearFilteredCache);
}

bool RSHpaeFilterCacheManager::IsCacheValid()
{
    return cachedSnapshot_ != nullptr || cachedFilteredSnapshot_ != nullptr;
}

void RSHpaeFilterCacheManager::InvalidateFilterCache(FilterCacheType clearType)
{
    if (clearType == FilterCacheType::BOTH) {
        cachedSnapshot_.reset();
        cachedFilteredSnapshot_.reset();
        return;
    }
    if (clearType == FilterCacheType::SNAPSHOT) {
        cachedSnapshot_.reset();
        return;
    }
    if (clearType == FilterCacheType::FILTERED_SNAPSHOT) {
        cachedFilteredSnapshot_.reset();
    }
}

void RSHpaeFilterCacheManager::ReleaseHpaeBufferInfo()
{
    inputBufferInfo_ = HpaeBufferInfo();
    outputBufferInfo_ = HpaeBufferInfo();
}

void RSHpaeFilterCacheManager::ResetFilterCache(std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedSnapshot,
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedFilteredSnapshot, RectI snapshotRegion)
{
    // Update cachedSnapshot and cachedFilteredSnapshot for RSFilterCacheManager
    // Use the cache even if it's null because this function is called before DrawFilter()
    if (cachedSnapshot) {
        cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(
            cachedSnapshot->cachedImage_, cachedSnapshot->cachedRect_);
    } else {
        cachedSnapshot_.reset();
    }
    if (cachedFilteredSnapshot) {
        cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData> (
            cachedFilteredSnapshot->cachedImage_, cachedFilteredSnapshot->cachedRect_);
    } else {
        cachedFilteredSnapshot_.reset();
    }
    snapshotRegion_ = snapshotRegion;
}

void RSHpaeFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& srcRect)
{
    auto drawingSurface = canvas.GetSurface();
    if (drawingSurface == nullptr) {
        return;
    }

    RS_OPTIONAL_TRACE_NAME("RSHpaeFilterCacheManager::TakeSnapshot");
    Drawing::RectI snapshotIBounds = srcRect;
    // Take a screenshot
    std::shared_ptr<Drawing::Image> snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds, false);
    filter->PreProcess(snapshot);

    // Update the cache state.
    snapshotRegion_ = RectI(srcRect.GetLeft(), srcRect.GetTop(), srcRect.GetWidth(), srcRect.GetHeight());
    cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), snapshotIBounds);
}

int RSHpaeFilterCacheManager::BlurWithoutSnapshot(const Drawing::RectI& clipBounds,
    const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<RSDrawingFilter>& filter,
    const Drawing::RectI& src, bool shouldClearFilteredCache)
{
    RS_OPTIONAL_TRACE_NAME_FMT("HPAE::BlurWithoutSnapshot: %d", shouldClearFilteredCache);
    if (shouldClearFilteredCache) {
        cachedFilteredSnapshot_.reset();
    } else {
        cachedFilteredSnapshot_ =
            std::make_shared<RSPaintFilterCanvas::CachedEffectData>(); // fill data after SetBlurOutput
    }

    int result = ProcessHpaeBlur(clipBounds, image, filter, src);
    if (result != 0) {
        cachedFilteredSnapshot_.reset(); // invalidate cache if blur failed
        HpaeBackgroundCacheItem emptyItem;
        SetBlurOutput(emptyItem); // notify task done to avoid output being empty
    } else {
        RSHpaeBaseData::GetInstance().NotifyBufferUsed(true);
    }
    return result;
}

int RSHpaeFilterCacheManager::BlurUsingFilteredSnapshot()
{
    if (cachedFilteredSnapshot_ == nullptr) {
        HPAE_LOGW("cachedFilteredSnapshot_ is nullptr");
        return -1;
    }

    if (cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        RS_OPTIONAL_TRACE_NAME_FMT("HPAE::BlurUsingPrevResult");
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        if (hpaeBlurOutputQueue_.empty()) {
            HPAE_LOGW("blur output queue is empty");
            return -1;
        }
        HpaeBackgroundCacheItem blurItem;
        // using previous item; don't set anything but useCache_
        blurItem.useCache_ = true;
        hpaeBlurOutputQueue_.push_back(blurItem);
        return 0;
    }

    RS_OPTIONAL_TRACE_NAME("HPAE::BlurUsingFilteredSnapshot");
    {
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        HpaeBackgroundCacheItem blurItem;
        if (hpaeBlurOutputQueue_.empty()) {
            blurItem.blurImage_ = cachedFilteredSnapshot_->cachedImage_;
            blurItem.radius_ = curRadius_;
        } else {
            // using previous item; don't set anything but useCache_
            blurItem.useCache_ = true;
        }

        hpaeBlurOutputQueue_.push_back(blurItem);
    }
    return 0;
}

int RSHpaeFilterCacheManager::ProcessHianimationBlur(const std::shared_ptr<RSDrawingFilter>& filter, float radius)
{
    HpaeBackgroundCacheItem blurItem;
    blurItem.hpaeTask_ = GenerateHianimationTask(inputBufferInfo_, outputBufferInfo_, radius, filter);

    if (blurItem.hpaeTask_.taskPtr == nullptr) {
        return -1;
    }

    blurItem.surface_ = outputBufferInfo_.surface;
    auto curFrameId = RSHpaeFfrtPatternManager::Instance().MHCGetVsyncId();
    HPAE_LOGD("mhc_so MHCRequestEGraph. frameId:%{public}" PRIu64 " ", curFrameId);
    RS_OPTIONAL_TRACE_NAME_FMT("mhc_so: MHCRequestEGraph curFrameId=%u", curFrameId);
    if (!RSHpaeFfrtPatternManager::Instance().MHCRequestEGraph(curFrameId)) {
        HPAE_LOGE("mhc_so MHCRequestEGraph failed. frameId:%{public}" PRIu64 " ", curFrameId);
        RSHpaeFfrtPatternManager::Instance().MHCSetCurFrameId(0);
        HianimationManager::GetInstance().HianimationDestroyTask(blurItem.hpaeTask_.taskId);
        return -1;
    }
    blurItem.gpFrameId_ = curFrameId;
    RSHpaeFfrtPatternManager::Instance().MHCSetCurFrameId(curFrameId);
    HPAE_LOGD("HianimationBlur: taskId: %{public}d, radius=%{public}f",
        blurItem.hpaeTask_.taskId, radius);
    RSHpaeFfrtPatternManager::Instance().SetUpdatedFlag();
    RSHpaeScheduler::GetInstance().CacheHpaeItem(blurItem); // cache and notify after flush
#if defined(ASYNC_BUILD_TASK) && defined(ROSEN_OHOS)
    {
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        hpaeBlurOutputQueue_.pop_back(); // remove previous empty item
    }
#endif
    SetBlurOutput(blurItem);

    return 0;
}

int RSHpaeFilterCacheManager::ProcessHpaeBlur(const Drawing::RectI& clipBounds,
    const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<RSDrawingFilter>& filter,
    const Drawing::RectI& src)
{
    RS_TRACE_NAME_FMT("ProcessHpaeBlur. radius=%f", curRadius_);

    HpaeBackgroundCacheItem blurItem;
    float radius = curRadius_ / 2.0f; // HpaeBlur using half image size
    if (radius < 1.0f) {
        // directly process to outputbuffer if radius == 0
        HPAE_LOGD("SmallRadius: %f", radius);
        if (outputBufferInfo_.canvas) {
            Drawing::Rect dst = Drawing::Rect(0, 0, outputBufferInfo_.canvas->GetWidth(),
                outputBufferInfo_.canvas->GetHeight());
            filter->ApplyColorFilter(*outputBufferInfo_.canvas, image, src, dst, 1.0f);
            filter->PostProcess(*outputBufferInfo_.canvas);
        }
        blurItem.surface_ = outputBufferInfo_.surface;
        SetBlurOutput(blurItem);
        return 0;
    }

    if (0 != RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, inputBufferInfo_, filter, src)) {
        HPAE_LOGW("ProcessGreyAndStretch failed");
        return -1;
    }

    int result = -1;
#if defined(ASYNC_BUILD_TASK) && defined(ROSEN_OHOS)
    if (!drawUsingGpu_) {
        SetBlurOutput(blurItem); // empty item to motivate the hpaeBlurOutputQueue_.remember to pop it
        RSHpaeScheduler::GetInstance().SetToWaitBuildTask();
        void *hianimationTask = ffrt::submit_h([=]() {
            RS_OPTIONAL_TRACE_NAME("Async BuildTask");
            if (this->ProcessHianimationBlur(filter, radius) != 0) {
                std::unique_lock<std::mutex> lock(blurOutMutex_);
                hpaeBlurOutputQueue_.pop_back(); // remove previous empty item
            }
            RSHpaeScheduler::GetInstance().NotifyBuildTaskDone();
        }, {}, {}, ffrt::task_attr().qos(5));
        result = 0;
    }
#else
    if (!drawUsingGpu_) {
        result = ProcessHianimationBlur(filter, radius);
    }
#endif

    if (result != 0) {
        // using GPU for exception
        result = ProcessGpuBlur(inputBufferInfo_, outputBufferInfo_, filter, radius);
        if (result == 0) {
            // set buffer to Output if blur successed
            blurItem.surface_ = outputBufferInfo_.surface;
            SetBlurOutput(blurItem);
        }
    }

    return result;
}

int RSHpaeFilterCacheManager::ProcessGpuBlur(const HpaeBufferInfo &inputBuffer,
    const HpaeBufferInfo &outputBuffer, const std::shared_ptr<RSDrawingFilter>& filter, float radius)
{
    if (inputBuffer.canvas == nullptr || outputBuffer.canvas == nullptr) {
        HPAE_LOGE("canvas is nullptr");
        return -1;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("ProcessGpuBlur: radius: %f, type: %d", radius, filter->GetFilterType());
    auto snapshotIBounds = Drawing::RectI(0, 0, inputBuffer.canvas->GetWidth(), inputBuffer.canvas->GetHeight());
    auto image = inputBuffer.canvas->GetSurface()->GetImageSnapshot(snapshotIBounds, false);
    auto outCanvas = outputBuffer.canvas;

    Drawing::Rect srcRect = Drawing::Rect(0, 0, inputBuffer.canvas->GetWidth(), inputBuffer.canvas->GetHeight());
    Drawing::Rect dstRect = Drawing::Rect(0, 0, outputBuffer.canvas->GetWidth(), outputBuffer.canvas->GetHeight());

    HPAE_LOGD("ProcessGpuBlur: radius: %f, type: %{public}d", radius, filter->GetFilterType());
    auto brush = filter->GetBrush(1.0f);
    float saturation = RSHpaeBaseData::GetInstance().GetSaturation();
    float brightness = RSHpaeBaseData::GetInstance().GetBrightness();
    auto hpsParam = Drawing::HpsBlurParameter(srcRect, dstRect, radius, saturation, brightness);
    if (RSSystemProperties::GetHpsBlurEnabled() && filter->GetFilterType() == RSFilter::MATERIAL) {
        RS_TRACE_NAME_FMT("HPAE:ApplyHPSBlur::radius %f", radius);
        if (HpsBlurFilter::GetHpsBlurFilter().ApplyHpsBlur(*outCanvas, image, hpsParam,
            brush.GetColor().GetAlphaF(), brush.GetFilter().GetColorFilter())) {
            filter->ForcePostProcess(*outCanvas);
            return 0;
        }
    }
    {
        RS_TRACE_NAME("HPAE:ApplyKawaseBlur " + std::to_string(radius));
        HPAE_LOGD("HPAE:ApplyKawaseBlur %f", radius);
        auto effectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
        auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(radius);
        kawaseBlurFilter->GenerateGEVisualEffect(effectContainer);
        auto blurImage = geRender->ApplyImageEffect(
            *outCanvas, *effectContainer, image, srcRect, srcRect, Drawing::SamplingOptions());
        if (blurImage == nullptr) {
            ROSEN_LOGE("ProcessGpuBlur::DrawImageRect blurImage is null");
            return -1;
        }
        outCanvas->AttachBrush(brush);
        outCanvas->DrawImageRect(*blurImage, dstRect, Drawing::SamplingOptions());
        outCanvas->DetachBrush();
        filter->ForcePostProcess(*outCanvas);
        return 0;
    }
    return -1;
}

HpaeTask RSHpaeFilterCacheManager::GenerateHianimationTask(const HpaeBufferInfo &inputBuffer,
    const HpaeBufferInfo &outputBuffer, float radius, const std::shared_ptr<RSDrawingFilter>& filter)
{
    HpaeTask resultTask;
    resultTask.taskPtr = nullptr;

#if defined(ROSEN_OHOS)
    if (!RSSystemProperties::GetHpaeBlurUsingAAE()) {
        return resultTask;
    }
#endif

    if (inputBuffer.canvas == nullptr || outputBuffer.canvas == nullptr ||
        inputBuffer.bufferHandle == nullptr || outputBuffer.bufferHandle == nullptr) {
        HPAE_LOGE("input is null");
        return resultTask;
    }

    outputBuffer.canvas->Clear(Drawing::Color::COLOR_TRANSPARENT); // notify GetImageSnapshot()

    int32_t srcWidth = inputBuffer.canvas->GetWidth();
    int32_t srcHeight = inputBuffer.canvas->GetHeight();

    BlurImgParam imgInfo = {srcWidth, srcHeight, radius};
    HaeNoiseValue noisePara = {0.25f, 1.75f, 1.75f, -2.0f}; // from experience
    if (!HianimationManager::GetInstance().HianimationInputCheck(&imgInfo, &noisePara)) {
        HPAE_LOGE("HianimationInputCheck failed");
        return resultTask;
    }

    struct HaeImage srcLayer;
    struct HaeImage dstLayer;
    srcLayer.handle = inputBuffer.bufferHandle;
    srcLayer.rect = HaeRect(0, 0, srcWidth, srcHeight);
    dstLayer.handle = outputBuffer.bufferHandle;
    dstLayer.rect = HaeRect(0, 0, outputBuffer.canvas->GetWidth(), outputBuffer.canvas->GetHeight());

    HaeBlurBasicAttr basicInfo;
    basicInfo.srcLayer = &srcLayer;
    basicInfo.dstLayer = &dstLayer;
    basicInfo.perfLevel = 4; // 4 for the highest level
    basicInfo.expectRunTime = 2000; // us
    basicInfo.timeoutMs = 0;
    basicInfo.sigmaNum = radius;
    basicInfo.enablePremult = false;

    HaeBlurEffectAttr effectInfo;
    effectInfo.noisePara = noisePara;
    effectInfo.effectCaps = BLPP_COLOR_MASK_EN | BLPP_COLOR_MATRIX_EN | BLPP_NOISE_EN;
    effectInfo.alphaReplaceVal = 0;
    effectInfo.colorMaskPara = RSHpaeFusionOperator::GetHaePixel(filter);
    RSHpaeFusionOperator::GetColorMatrixCoef(filter, effectInfo.colorMatrixCoef);

    HianimationManager::GetInstance().HianimationBuildTask(
        &basicInfo, &effectInfo, &resultTask.taskId, &resultTask.taskPtr);
        RS_OPTIONAL_TRACE_NAME_FMT("Hianimation: taskId: %d, hpae radius=%f",
            resultTask.taskId, radius);

    return resultTask;
}

void RSHpaeFilterCacheManager::SetBlurOutput(HpaeBackgroundCacheItem& hpaeOutputItem)
{
    std::unique_lock<std::mutex> lock(blurOutMutex_);
    if (hpaeOutputItem.surface_.lock()) {
        hpaeOutputItem.radius_ = curRadius_;
    }
    RS_OPTIONAL_TRACE_NAME("HAPE::SetBlurOutput");
    hpaeBlurOutputQueue_.push_back(hpaeOutputItem);
}

bool RSHpaeFilterCacheManager::UseCacheImage() const
{
    return useCacheImage_;
}

HpaeBackgroundCacheItem RSHpaeFilterCacheManager::GetBlurOutput()
{
    HpaeBackgroundCacheItem result;
    {
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        HpaeBackgroundCacheItem prevItem;
        while (hpaeBlurOutputQueue_.size() > HPAE_CACHE_SIZE) {
            prevItem = hpaeBlurOutputQueue_.front();
            hpaeBlurOutputQueue_.pop_front();
        }
        useCacheImage_ = false;
        if (!hpaeBlurOutputQueue_.empty()) {
            auto headIter = hpaeBlurOutputQueue_.begin();
            if (headIter->useCache_) {
                headIter->blurImage_ = prevItem.blurImage_;
                headIter->radius_ = prevItem.radius_;
                RS_OPTIONAL_TRACE_NAME("UsePrevItem");
            }
            auto headSurface = headIter->surface_.lock();
            if (headIter->blurImage_ == nullptr && headSurface) {
                auto snapshotIBounds = Drawing::RectI(0, 0, headSurface->Width(), headSurface->Height());
                headIter->blurImage_ = headSurface->GetImageSnapshot(snapshotIBounds, false);
                RS_OPTIONAL_TRACE_NAME("UseSnapshot");
            } else {
                headIter->gpFrameId_ = 0; // avoid submit with FFTS
                useCacheImage_ = true;
            }
            result = hpaeBlurOutputQueue_.front();
            if (useCacheImage_ && hpaeBlurOutputQueue_.size() == 1) {
                HPAE_LOGD("Clear cache for first frame");
                hpaeBlurOutputQueue_.clear();
            }
        }
    }

    RS_OPTIONAL_TRACE_NAME_FMT("HPAE::GetBlurOutput: %d, %" PRIu64 "", hpaeBlurOutputQueue_.size(), result.gpFrameId_);

    RSHpaeScheduler::GetInstance().SetHpaeFrameId(result.gpFrameId_);
    return result;
}

static bool CanDiscardCanvas(RSPaintFilterCanvas& canvas, const Drawing::RectI& dstRect)
{
    /* Check that drawing will be in full canvas and no issues with clip */
    return (RSSystemProperties::GetDiscardCanvasBeforeFilterEnabled() && canvas.IsClipRect() &&
        canvas.GetDeviceClipBounds() == dstRect && canvas.GetWidth() == dstRect.GetWidth() &&
        canvas.GetHeight() == dstRect.GetHeight() && dstRect.GetLeft() == 0 && dstRect.GetTop() == 0 &&
        canvas.GetAlpha() == 1.0 && !canvas.HasOffscreenLayer());
}

static void ClipVisibleRect(RSPaintFilterCanvas& canvas)
{
    auto visibleRectF = canvas.GetVisibleRect();
    visibleRectF.Round();
    Drawing::RectI visibleIRect = {(int)visibleRectF.GetLeft(), (int)visibleRectF.GetTop(),
        (int)visibleRectF.GetRight(), (int)visibleRectF.GetBottom()};
    auto deviceClipRect = canvas.GetDeviceClipBounds();
    if (!visibleIRect.IsEmpty() && deviceClipRect.Intersect(visibleIRect)) {
        canvas.ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
}

int RSHpaeFilterCacheManager::DrawBackgroundToCanvas(RSPaintFilterCanvas& canvas)
{
    auto hpaeBlurItem = GetBlurOutput();
    auto hpaeOutSnapshot = hpaeBlurItem.blurImage_;

    if (hpaeOutSnapshot == nullptr) {
        HPAE_LOGE("get shared image failed");
        return -1;
    }

    if (cachedFilteredSnapshot_) {
        cachedFilteredSnapshot_->cachedImage_ = hpaeOutSnapshot;
        cachedFilteredSnapshot_->cachedRect_ =
            Drawing::RectI(0, 0, hpaeOutSnapshot->GetWidth(), hpaeOutSnapshot->GetHeight());
    }

    backgroundRadius_ = hpaeBlurItem.radius_;
    if (FloatEqual(backgroundRadius_, curRadius_)) {
        blurContentChanged_ = false;
    } else {
        blurContentChanged_ = true;
    }

    prevBlurImage_ = hpaeOutSnapshot;

    RS_OPTIONAL_TRACE_NAME_FMT("DrawBackgroundToCanvas: radius[%f, %f], src[%dx%d], dst:[%dx%d]",
        backgroundRadius_, curRadius_,
        hpaeOutSnapshot->GetWidth(),
        hpaeOutSnapshot->GetHeight(),
        canvas.GetWidth(),
        canvas.GetHeight());

    auto src = Drawing::Rect(0, 0, hpaeOutSnapshot->GetWidth(), hpaeOutSnapshot->GetHeight());
    auto dst = Drawing::Rect(0, 0, canvas.GetWidth(), canvas.GetHeight());

    // Draw in device coordinates.
    bool discardCanvas = CanDiscardCanvas(canvas, Drawing::RectI(0, 0, canvas.GetWidth(), canvas.GetHeight()));
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ResetMatrix();

    // Only draw within the visible rect.
    ClipVisibleRect(canvas);

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    if (discardCanvas) {
        canvas.Discard();
    }
    canvas.DrawImageRect(*hpaeOutSnapshot, src, dst, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();

    return 0;
}

} // Rosen
} // OHOS
#include "hpae_base/rs_hpae_filter_cache_manager.h"
#include "hpae_base/rs_hpae_hianimation.h"
#include "render/rs_kawase_blur_shader_filter.h"
#include "render/rs_mesa_blur_shader_filter.h"
#include "render/rs_grey_shader_filter.h"
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_log.h"
#include "hpae_base/rs_hpae_scheduler.h"
#include "hpae_base/rs_hpae_fusion_operator.h"
#include "cpp/ffrt_dynamic_graph.h"
#include "ge_render.h"
#include "platform/common/rs_log.h"
#include "unistd.h"
#include "common/rs_background_thread.h"
#include "platform/common/rs_system_properties.h"

// #define ASYNC_BUILD_TASK 1

namespace OHOS {
namesapce Rosen {

static constexpr float epsilon = 0.999f;
static const int HPAE_CACHE_SIZE = 3; // for Set() before Get(), N-2 require a queue size of 3

// Reference: RSFilterCacheManager::DrawFilter
int RSHpaeFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSFilter>& rsFilter,
    bool shouldClearFilteredCache, const std::optional<Drawing::RectI>& srcRect, const std::optional<Drawing::RectI>& dstRect)
{
    RSHpaeScheduler::GetInstance().Reset();

    inputBufferInfo_ = RSHpaeBaseData::GetInstance().RequestHpaeInputBuffer();
    outputBufferInfo_ = RSHpaeBaseData::GetInstance().RequestHpaeOutputBuffer();

    if (inputBufferInfo_.canvas == nullptr || outputBufferInfo_canvas == nullptr) {
        HPAE_LOGE("invalid canvas");
        return -1;
    }
    // inputBufferInfo_.canvas->CopyHDRConfiguration(canvas);
    // inputBufferInfo_.canvas->CopyConfigurationToOffscreenCanvas(canvas);
    // outputBufferInfo_.canvas->CopyHDRConfiguration(canvas);
    // outputBufferInfo_.canvas->CopyConfigurationToOffscreenCanvas(canvas);

    Drawing::RectI clipBounds = canvas.GetDeviceClipBounds();
    Drawing::RectI src = canvas.GetRoundInDeviceClipBounds();
    if (clipBounds.IsEmpity() || src.IsEmpty()) {
        HPAE_LOGE("canvas rect is empty");
        return -1; // -1 or 0?
    }

    if (RSHpaeBaseData::GetInstance().GetNeedReset()) {
        std::unique_lock(std::mutex) lock(blurOutMutex_);
        HPAE_TRACE_NAME("Clear for first frame");
        hpaeBlurOutputQueue_.clear();
        backgroundRadius_ = 0;
        preBlurImage_ = nullptr;
        RSHpaeBaseData::GetInstance().SetResetDone();
        // InvalidateFilterCache(FilterCacheType::FILTERED_SNAPSHOT);
    }

    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);

    curRadius_ = RSHpaeBaseData::GetInstance().GetBlurRadius();

    if (!IsCacheValid()) {
        TaskSnapshot(canvas, filter, src);
    }

    drawUsingGpu_ = false;
    {
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        if (hpaeBlurOutputQueue_.empty()) {
            drawUsingGpu_ = true;
        }
    }

    // wait here, before using previous blur result
    if (drawUsingGpu_ == false && !HianimatiionManager::GetInstance().WaitPreviousTask()) {
        HPAE_LOGW("Hianimation resource not enough");
        drawUsingGpu_ = true;
    }

    if (0 == DrawFilterImpl(filter, clipBounds, src, shouldClearFilteredCache)) {
        return DrawBackgroundToCanvas(canvas);
    }

    return -1;
};

int RSHpaeFilterCacheManager::DrawFilterImpl(const std::shared_ptr<RSDrawingFilter>& filter,
    const Drawing::RectI& clipBounds, const Drawing::RectI& src, bool shouldClearFilteredCache)
{
    if (cachedFilterSnapshot_ != nullptr) { // notify to using cache
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
        HPAE_TRACE_NAME_FMT("%p InvalidateCache BOTH", this);
        return;
    }
    if (clearType == FilterCacheType::SNAPSHOT) {
        cachedSnapshot_.reset();
        HPAE_TRACE_NAME_FMT("%p InvalidateCache SNAPSHOT", this);
        return;
    }
    if (clearType == FilterCacheType::FILTERED_SNAPSHOT) {
        HPAE_TRACE_NAME_FMT("%p InvalidateCache FILTERED_SNAPSHOT", this);
        cachedFilteredSnapshot_.reset();
    }
}

void RSHpaeFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& srcRect)
{
    // TODO
    // 通过GetImageSnapshot获取清晰背景
    // 注意缓存的是EffectData而非只有Drawing::Image
    auto drawingSurface = canvas.GetSurface();
    if (drawongSurface == nullptr) {
        return;
    }

    HPAE_TRACE_NAME("RSHpaeFilterCacheManager::TaskSnapshot");
    Drawing::RectI snapshotIBounds = srcRect;
    // Task a screenshot
    std::shared_ptr<Drawing::Image> snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds, false);
    filter->PrePorcess(snapshot);

    // Update the cache state.
    snapshotRegion_ = RectI(srcRect.GetLeft(), srcRect.GetTop(), srcRect.GetWidth(), srcRect.GetHeight());
    cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), snapshotIBounds);
}

int RSHpaeFilterCacheManager::BlurWithoutSnapshot(const Drawing::Rect& clipBounds,
    const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<RSDrawingFilter>& filter,
    const Drawing::RectI &src, bool shouldClearFilteredCache)
{
    HPAE_TRACE_NAME("HPAE::BlurWithoutSnapshot");
    if (shouldClearFilteredCache) {
        cacheFilteredSnapshot_.reset();
    } else {
        cachedFilteredSnapshot_ = 
            std::make_shared<RSPaintFilterCanvas::CachedEffectData>(); // fill data after SetBlurOutput
    }

    // cachedFilterHash_ = filter->Hash();

    int result = ProcessHpaeBlur(clipBounds, image, filter, src);
    if (result != 0) {
        cachedFilteredSnapshot_.reset(); // invalidate cache if blur failed
        HpaeBackgroundCachItem emptyItem;
        SetBlurOutput(emptyItem); // notify task done avoid output being empty
    } else {
        RSHpaeBaseData::GetInstance().NotifyBufferUsed(true);
    }
    return result;
}

int RSHpaeFilterCacheManager::BlurUsingFilteredSnapshot()
{
    if (cachedFilteredSnapshot_ == nullptr) {
        return -1;
    };

    if (cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        HPAE_TRACE_NAME_FMT("HPAE::BlurUsingPrevResult");
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        HpaeBackgroundCacheItem blurItem;
        // using previous item; don't set anything but useCache_
        blurItem.useCache_ = true;
        hpaeBlirOutputQueue_.push_back(blurItem);
        return 0;
    }

    HPAE_TRACE_NAME_FMT("HPAE::BlurUsingFilteredSnapshoy: %p, %p", cachedFilteredSnapshot_->cachedImage_.get(), this);
    {
        std::unique_lock<std::mutex> lock(blurOutMutex_);
        HpaeBackgroundCacheItem blurItem;
        if (hpaeBlirOutputQueue_.empty()) {
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
    blurItem.hpaeTask_ = GenerateHianimationTask(inputBufferInfo_, outputBufferInfo, radius, filter);

    if (blurItem.hpaeTask_.taskPtr == nullptr) {
        return -1;
    }

    blurItem.surface_ = outputBufferInfo_.surface;
    auto curFrameId = RSHpaeFfrtPatternManager::Instance().MHCGetVsyncId();
    HPAE_LOGW("mhc_so MHCRequestEGraph. frameId:%{public}" PRIu64 " ", curFrameId);
    HPAE_TRACE_NAME_FMT("mhc_so: MHCRequestEGraph curFrameId=%u", curFramId);
    if (!RSHpaeFfrtPatternManager::Instance().MHCRequestEGgraph(curFrameId)) {
        HPAE_LOGE("mhc_so MHCRequestEGraph failed. frameId:%{public}" PRIu64 " ", curFrameId);
        RSHpaeFfrtPatternManager::Instance().MHCSetCurFrameId(0);
        HianimationManager::GetInstance().HianimationDestroyTask(blurItem.hpaeTask_.taskId);
        return -1;
    }
    blurItem.gpFrameId_ = curFrameId;
    RSHpaeFfrtPatternManager::Instance().MHCSetCurFrameId(curFrameId);
    HPAE_LOGW("HianimationBlur: taskId: %{public}d, taskPtr: %{public}p, radius=%{public}f",
        blurItem.hpaeTask_.taskId, blurItem.hpaeTask_.taskPtr, radius);
    RSHpaeFfrtPatternManager::Instance().SetUpdatedFlag();
    RSHpaeScheduler::GetInstance().CacheHpaeItem(blurItem); // cache and notify after flush
#ifdef ASYNC_BUILD_TASK
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
    RS_TRACE_NAME_FMT("ProcessHpaeBlur. radius=%f, %p", curRadius_, this);

    HpaeBackgroundCacheItem blurItem;
    float radius = curRadius_ / 2.0f; // HpaeBlur using half image size
    // float radius = curRadius_; // HpaeBlur using whole image size
    // if (ROSEN_LE(radius, epsilon)) {
    if (radius < epsilon) {
        // directly process to outputbuffer if radius == 0
        RS_TRACE_NAME_FMT("SmallRadius: %f", radius);
        // if (0 != ProcessGreyAndStretch(clipBounds, image, outputBufferInfo_, filter, src)) {
        //     return -1;
        // }
        // TODO: ApplyColorFilter(canvas, outImage, attr.src, attr.dst, attr.brushAlpha);
        if (outputBufferInfo_.canvas) {
            Drawing::Rect dst = Drawing::Rect(0, 0, outputBufferInfo_.canvas->GetWidth(), outputBufferInfo_.canvas->GetHeight());
            filter->ApplyColorFilter(*outputBufferInfo_.canvas, image, src, dst, 1.0f);
            filter->PostProcess(*outputBufferInfo_.canvas);
        }
        blurItem.surface_ = outputBufferInfo_.surface;
        SetBlurOutput(blurItem);
        return 0;
    }

    if (0 != RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, inputBufferInfo_, filter, src)) {
        return -1;
    }

    int result = -1;
#ifdef ASYNC_BUILD_TASK
    if (!drawUsingGpu_) {
        SetBlurOutput(blurItem); // empty item to motivate the hpaeBlurOutputQueue_.rememner to pop it
        RSHpaeScheduler::GetInstance().SetToWaitBuildTask();
        void *hianimationTask = ffrt::submit_h([=]() {
            HPAE_TRACE_NAME("Async BuildTask");
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

// 使用1/4 ion buffer进行的kawasBlur兜底
// HpaeBufferInfo中提供了GPU绘制需要的surface/canvas/context
// inputBuffer: 提供了灰阶和边缘像素后的结果
int RSHpaeFilterCacheManager::ProcessGpuBlur(const HpaeBufferInfo &inputBuffer,
    const HpaeBufferInfo &outputBuffer, const std::shared_ptr<RSDrawingFilter>& filter, float radius)
{
    if (inputBuffer.canvas == nullptr || outputBuffer.canvas == nullptr) {
        HPAE_LOGE("canvas is nullptr");
        return -1;
    }

    HPAE_TRACE_NAME_FMT("ProcessGpuBlur: radius: %f, type: %d", radius, filter->GetFilterType());
    auto snapshotIBounds = Drawing::RectI(0, 0, inputBuffer.canvas->GetWidth(), inputBuffer.canvas->GetHeight());
    auto image = inputBuffer.canvas->GetSurface()->GetImageSnapshot(snapshotIBounds, false);
    auto outCanvas = outputBuffer.canvas;

    Drawing::Rect srcRect = Drawing::Rect(0, 0, inputBuffer.canvas->GetWidth(), inputBuffer.canvas->GetHeight());
    Drawing::Rect dstRect = Drawing::Rect(0, 0, inputBuffer.canvas->GetWidth(), outputBuffer.canvas->GetHeight());

    if (radius < epsilon) {
        filter->ApplyColorFilter(*outCanvas, image, srcRect, dstRect, 1.0f);
        filter->PostProcess(*outCanvas);
        return 0;
    }
    HPAE_LOGW("ProcessGpuBlur: radius: %f, type: %{public}d", radius, filter->GetFilterType());
    auto brush = filter->GetBrush(1.0f); // todo: get true alpha
    float saturation = RSHpaeBaseData::GetInstance().GetSaturation();
    float brightness = RSHpaeBaseData::GetInstance().GetBrightness();
    auto hpsParam = Drawing::HpsBlurParameter(srcRect, dstRect, radius, saturation, brightness);
    if (RSSystemProperties::GetHpaBlurEnableed() && filter->GetFilterType() == RSFilter::MATERIAL) {
        RS_TRACE_NAME_FMT("HPAE:ApplyHPSBlur::radius %f", radius);
        if (HpsBlurFilter::GetHpsBlurFilter().ApplyHpsBlur(*outCanvas, image, hasParam,
            brush.GetColor().GetAlphaF(), brush.GetFilter().GetColorFilter())) {
                filter->PostProcess(*outCanvas);
                return 0;
        }
    }
    {
        RS_REACE_NAME("HPAE:ApplyKawaseBlur " + std::to_string(radius));
        HPAE_LOGW("HPAE:ApplyKawaseBlur %f", radius);
        auto effectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
        auto geRender = std::make_shared<GraphicEffectEngine::GERender>();
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShadeFilter>(radius);
        kawaseBlurFilter->GenerateGEVisualEffect(effectContainer);
        auto blurImage = geRender->ApplyImageEffect(
            *outCanvas, *effectContainer, image, srcRect, srcRect, Drawing::SamplingOptions());
        if (blurImage == nullptr) {
            ROSEN_LOGE("ProcessGpuBlur::DrawingImageRect blurImage is null");
            return -1;
        }
        outCanvas->AttachBrush(brush);
        outCanvas->DrawImageRect(*blurImage, dstRect, Drawing::SamplingOptions());
        outCanvas->DetachBrush();
        filter->PostProcess(*outCanvas);
        return 0;
    }
    return -1;
}

// 注意该函数不要产生对canvas的GPU操作
HpaeTask RSHpaeFilterCacheManager::GenerateHianimationTask(const HpaeBufferInfo &inputBuffer,
    const HpaeBufferInfo &outputBuffer, float radius, const std::shared_ptr<RSDrawingFilter>& filter)
{
    HpaeTask resultTask;
    resultTask.taskPtr = nullptr;

    if (!RSSystemProperties::GetHpaeBlurUsingAAE()) {
        return resultTask;
    }

    if (inputBuffer.canvas == nullptr || outputBuffer.canvas == nullptr) {
        HPAE_LOGE("canvas is null");
        return resultTask;
    }

    if (inputBuffer.bufferHandle == nullptr || outputBuffer.bufferHandle == nullptr) {
        HPAE_LOGE("bufferHandle is null");
        return resultTask;
    }

    int32_t srcWidth = inputBuffer.canvas->GetWidth();
    int32_t srcHeight = inputBuffer.canvas->GetHeight();
    int32_t dstWidth = outputBuffer.canvas->GetWidth();
    int32_t dstHeight = ourtputBuffer.canvas->GetHeight();
    uint32_t format = inputBuffer.bufferHandle->format;

    // TODO: 此处无需每次初始化
    HianimationManager::GetInstance().OpenDevice();
    BlurImgParam imgInfo = {srcWidth, srcHeight, radius};
    HaeNoiseValue noisePara = {0.25f, 1.75f, 1.75f, -2.0f}; // from experience
    if (!HianimationManager::GetInstance().HianimationInputCheck(&imgInfo, &noisePara)) {
        HPAE_LOGE("HianimationInputCheck failed");
        return resultTask;
    }

    // TODO: keep srcLayer and dstLayer since its pointer is used in libhianimation
    struct HaeImage srcLayer;
    struct HaeImage dstLayer;
    srcLayer.handle = inputBuffer.bufferHandle;
    srcLayer.rect = HaeRect(0, 0, srcWidth, srcHeight);

    dstLayer.handle = outputBuffer.bufferHandle;
    dstLayer.rect = HaeRect(0, 0, dstWidth, dstHeight);
    // dstLayer.rect = HaeRect(0, 0, dst.GetWidth() / 2, dst.GetHeight() / 2);

    HaeBlurBasicAttr basicInfo;
    basicInfo.srcLayer = &srcLayer;
    basicInfo.dstLayer = &dstLayer;
    basicInfo.perfLevel = 4; // 4 for the hightest level
    basicInfo.expectRunTime = 2000; // us
    basicInfo.timeoutMs = 0;
    basicInfo.sigmaNum = radius;
    basicInfo.enablePremult = false; // TODO

    HaeBlurBasicAttr effectInfo;
    effectInfo.noisePara = noisePara;
    effectInfo.effectCaps = BLPP_COLOR_MASK_EN | BLPP_COLOR_MATRIX_EN | BLPP_NOISE_EN;
    effectInfo.alphaReplaceVal = 0;
    effectInfo.colorMaskPara = RSHpaeFusionOperator::GetHaePixel(filter);
    RSHpaeFusionOperator::GetColorMatrixCoef(filter, effectInfo.colorMatrixCoef);

    // std::string maskStr = "[";
    // maskStr += std::to_string(effectInfo.colorMaskPara.a);
    // maskStr += ", ";
    // maskStr += std::to_string(effectInfo.colorMaskPara.r);
    // maskStr += ", ";
    // maskStr += std::to_string(effectInfo.colorMaskPara.g);
    // maskStr += ", ";
    // maskStr += std::to_string(effectInfo.colorMaskPara.b);
    // maskStr += ", ";

    // std::string matrixStr = "["
    // for (int i = 0; i < HAE_COLOR_MATRIX_COEF_COUNT; ++i) {
    //     matrixStr += std::to_string(effectInfo.colorMatrixCof[i]);
    //     matrixStr += ", ";
    // }
    // matrixStr += "] ";

    // HPAE_LOGW("HianimationParams: Radius: %f, size[%d x %d], ColorMask: %s, ColorMatrix: %s, effectCaps: %d",
    //     raduis, srcWidth, srcHeight, maskStr.c_str(), matrixStr.c_str(), effectInfo.effectCaps);

    uint32_t taskId = 0; // for destroy task
    void *taskPtr = nullptr; // task function
    int32_t ret = HianimationManager::GetInstance().HianimationBuildTask(&basicInfo, &effectInfo, &taskId, &taskPtr);
    {
        HPAE_TRACE_NAME_FMT("Hianimation: taskId: %d, taskPtr: %p, buildTask ret: %d, hpae radius=%f",
            taskId, taskPtr, ret, radius);
    }

    resultTask.taskId = taskId;
    resultTask.taskPtr = taskPtr;
    return resultTask;
}

// 将模糊输出的1/4 ion buffer存入队列
void RSHpaeFilterCacheManager::SetBlurOutput(HpaeBackgroundCacheItem& hpaeOutputItem)
{
    std::unique_lock<std::mutex> lock(blurOutMutex_);
    if (hpaeOutputItem.surface_) {
        auto outBounds = Drawing::RectI(0, 0, hpaeOutputItem.surface_->Width(), hpaeOutputItem.surface_->Heoght());
        hpaeOutputItem.radius_ = curRadius_; // 后续需移出到绘制函数中

        if (cachedFilteredSnapshot_) {
            // cachedFilteredSnapshot_->cachedImage_ = hpaeOutputItem.blurImage_;
            cachedFilteredSnapshot_->cachedRect_ = outBounds;
        }
    }
    HPAE_TRACE_NAME_FMT("HAPE::SetBlurOutput: surface[%p]", hpaeOutputItem.surface_.get());
    hpaeBlurOutputQueue_.push_back(hpaeOutputItem);
}

// 前景绘制通过该函数取背景图层的模糊结果，可以使用之前帧的缓存
HpaeBackgroundCacheItem RSHpaeFilterCacheManager::GetBluroutput()
{
    // TODO
    // 考虑使用N-1/N-2模糊输出的不同策略
    // 考虑首帧等待和非首帧处理差异
    // 考虑拿到模糊输出buffer后需要等待的ffrt_task问题
    HpaeBackgroundCacheItem result;
    {
        std::unqiue_lock<std::mutex> lock(blurOutMutex_);
        HpaeBackgroundCacheItem prevItem;
        while (hpaeBlurOutputQueue_.size() > HPAE_CACHE_SIZE) {
            prevItem = hpaeBlurOutputQueue_.front();
            hpaeBlurOutputQueue_.pop_front();
            HPAE_LOGI("HPAE::pop front: %p", prevItem.blurImage_.get());
        }
        if (!hpaeBlurOutputQueue_.empty()) {
            auto headIter = hpaeBlurOutputQueue_.begin();
            if (headIter->useCache_) {
                headIter->blurImage_ = prevItem.blurImage_;
                headIter->radius_ = prevItem.radius_;
                HPAE_TRACE_NAME_FMT("UsePrevItem: %p", headIter->blurImage_.get());
            }
            if (headIter->blurImage_ == nullptr && headIter->surface_) {
                auto snapshotIBounds = Drawing::RectI(0, 0, headIter->surface_->Width(), headIter->surface_->Height());
                headIter->blurImage_ = headIter->surface_->GetImageSnapshot(snapshotIBounds, false);
                HPAE_TRACE_NAME_FMT("UseSnapshot: surface:%p, image: %p", headIter->surface_.get(), headIter->blurImage_.get());
            } else {
                headIter->gpFrameId_ = 0; // avoid submit with FFTS
            }
            result = hpaeBlurOutputQueue_.front();
        }
    }

    HPAE_TRACE_NAME_FMT("HPAE::GetBlurOutput: %d, surface: %p, image: %p, %d", hpaeBlurOutputQueue_.size(),
        result.surface_.get(), result.blurImage_.get(), result.gpFrameId_);

    RSHpaeScheduler::GetInstance().SetHpaeFrameId(result.gpFrameId_);
    return result;
}

int RSHpaeFilterCacheManager::DrawBackgroundToCanvas(RSPaintFilterCanvas& canvas)
{
    // TODO
    // 实现将1/4 buffer采样到前景canvas，考虑跨Context/线程的资源访问问题
    // 这里交给GPU绘制，需要保证ion buffer在绘制完后才释放，通过FFTS给GPU任务添加aaeTask依赖完成
    auto hpaeBlurItem = GetBlurOutput();
    auto hpaeOutSnapshot = hpaeBlurItem.blurImage_;

    if (hpaeOutSnapshot == nullptr) {
        HPAE_TRACE_NAME("get shared image failed");
        return -1; // todo
    }

    if (cachedFilteredSnapshot_) {
        if (hpaeBlurItem.radius_ == curRadius_) {
            // cache filtered snapshot only when blur radius is the same
            cachedFilteredSnapshot_->cachedImage_ = hpaeOutSnapshot;
        } else {
            cachedFilteredSnapshot_->cachedImage_.reset();
        }
    }

    backgroundRadius_ = hpaeBlurItem.radius_;
    if (backgroundRadius_ == curRadius_ && hpaeBlurItem.useCache_) {
        blurContentChanged_ = false;
    } else {
        blurContentChanged_ = true;
    }

    prevBlurImage_ = hpaeOutSnapshot;

    HPAE_TRACE_NAME_FMT("DrawBackgroundToCanvas: blur out ino buffer: radius[%f, %f], %p, src[%dx%d], dst:[%dx%d]",
        backgroundRadius_, curRadius_,
        hpaeOutSnapshot.get(),
        hpaeOutSnapshot->GetWidth(),
        hpaeOutSnapshot->GetHeight(),
        canvas.GetWidth(),
        canvas.GetHeight());

    auto src = Drawing::Rect(0, 0, hpaeOutSnapshot->GetWidth(), hpaeOutSnapshot->GetHeight());
    auto dst = Drawing::Rect(0, 0, canvas.GetWidth(), canvas.GetHeight());

    const auto scalMatrix = RSHpaeFusionOperator::GetShaderTransform(
        dst, dst.GetWidth() / src.GetWidth(), dst.GetHeight() / src.GetHeight());

    const auto scaleShader = Drawing::ShadeEffect::CreateImageShader(*hpaeOutSnapshot,
        Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP,
        Drawing::SamplingOption(Drawing::FilterMode::LINEAR),
        scaleMatrix);
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ResetMatrix();

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetShaderEffect(scaleShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();

    return 0;
}

int RSHpaeFilterCacheManager::wzTest(int num)
{
    if (num < 0) {
        return -1;
    } else if (num < 100) {
        return 0;
    } else if (num < 500) {
        return 1;
    } else {
        return 2;
    }
}

} // Rosen
} // OHOS
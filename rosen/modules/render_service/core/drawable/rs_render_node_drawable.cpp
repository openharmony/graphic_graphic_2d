/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_render_node_drawable.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_thread.h"
// #include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::DrawableV2 {
#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
RSRenderNodeDrawable::Registrar RSRenderNodeDrawable::instance_;

namespace {
    constexpr int32_t DRAWING_CACHE_MAX_UPDATE_TIME = 3;
}
RSRenderNodeDrawable::RSRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawableAdapter(std::move(node))
{}

RSRenderNodeDrawable::~RSRenderNodeDrawable()
{
    ClearCachedSurface();
}

RSRenderNodeDrawable::Ptr RSRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSRenderNodeDrawable(std::move(node));
}

void RSRenderNodeDrawable::Draw(Drawing::Canvas& canvas)
{
    if (UNLIKELY(RSUniRenderThread::GetCaptureParam().isInCaptureFlag_)) {
        OnCapture(canvas);
    } else {
        OnDraw(canvas);
    }
}

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    auto& renderParams = renderNode_->GetRenderParams();
    Drawing::Rect bounds = renderParams ? renderParams->GetFrameRect() : Drawing::Rect(0, 0, 0, 0);

    DrawAll(canvas, bounds);
}

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    RSRenderNodeDrawable::OnDraw(canvas);
}

void RSRenderNodeDrawable::GenerateCacheIfNeed(Drawing::Canvas& canvas, RSRenderParams& params)
{
    // check if drawing cache enabled
    if (!isDrawingCacheEnabled_) {
        return;
    }
    if (params.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSCanvasRenderNodeDrawable::OnDraw id:%llu cacheType:%d cacheChanged:%d" \
            " size:[%.2f, %.2f] ChildHasVisibleFilter:%d ChildHasVisibleEffect:%d" \
            " shadowRect:[%.2f, %.2f, %.2f, %.2f] HasFilterOrEffect:%d",
            params.GetId(), params.GetDrawingCacheType(), params.GetDrawingCacheChanged(),
            params.GetCacheSize().x_, params.GetCacheSize().y_,
            params.ChildHasVisibleFilter(), params.ChildHasVisibleEffect(),
            params.GetShadowRect().GetLeft(), params.GetShadowRect().GetTop(),
            params.GetShadowRect().GetWidth(), params.GetShadowRect().GetHeight(),
            HasFilterOrEffect());
    }

    // check drawing cache type (disabled: clear cache)
    if (params.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE) {
        SetCacheType(DrawableCacheType::NONE);
        ClearCachedSurface();
        {
            std::lock_guard<std::mutex> lock(drawingCacheMapMutex_);
            drawingCacheUpdateTimeMap_.erase(renderNode_->GetId());
        }
        return;
    }

    // generate(first time)/update cache(cache changed) [TARGET -> DISABLED if >= MAX UPDATE TIME]
    bool needUpdateCache = CheckIfNeedUpdateCache(params);
    bool hasFilter = params.ChildHasVisibleFilter() || params.ChildHasVisibleEffect();
    if (params.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE ||
        (!needUpdateCache && !hasFilter)) {
        return;
    }

    // in case of no filter
    if (needUpdateCache && !hasFilter) {
        RS_TRACE_NAME_FMT("UpdateCacheSurface id:%llu", renderNode_->GetId());
        UpdateCacheSurface(canvas, params);
        return;
    }

    // in case of with filter
    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (needUpdateCache) {
        // 1. update cache without filer/shadow/effect & clip hole
        auto canvasType = curCanvas->GetCacheType();
        // set canvas type as OFFSCREEN to not draw filter/shadow/filter
        curCanvas->SetCacheType(RSPaintFilterCanvas::CacheType::OFFSCREEN);
        RS_TRACE_NAME_FMT("UpdateCacheSurface with filter id:%llu", renderNode_->GetId());
        UpdateCacheSurface(canvas, params);
        curCanvas->SetCacheType(canvasType);
    }

    // 2. traverse children to draw filter/shadow/effect
    Drawing::AutoCanvasRestore arc(*curCanvas, true);
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;
    drawBlurForCache_ = true;
    auto drawableCacheType = GetCacheType();
    SetCacheType(DrawableCacheType::NONE);
    RS_TRACE_NAME_FMT("DrawBlurForCache id:%llu", renderNode_->GetId());

    RSRenderNodeDrawable::OnDraw(canvas);
    SetCacheType(drawableCacheType);
    isOpDropped_ = isOpDropped;
    drawBlurForCache_ = false;
}

void RSRenderNodeDrawable::CheckCacheTypeAndDraw(Drawing::Canvas& canvas, const RSRenderParams& params)
{
    if (!isDrawingCacheEnabled_){
        RSRenderNodeDrawable::OnDraw(canvas);
        return;
    }

    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (drawBlurForCache_ && !params.ChildHasVisibleFilter() && !params.ChildHasVisibleEffect() &&
        !curCanvas->GetIsParallelCanvas()) {
        return;
    }
    
    if (drawBlurForCache_ && !params.ChildHasVisibleFilter() && !params.ChildHasVisibleEffect()) {
        RS_OPTIONAL_TRACE_NAME_FMT("CheckCacheTypeAndDraw id:%llu child without filter, skip", renderNode_->GetId());
        Drawing::AutoCanvasRestore arc(canvas, true);
        DrawBackground(canvas, params.GetBounds());
        return;
    }

    // RSPaintFilterCanvas::CacheType::OFFSCREEN case
    if (curCanvas->GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        if (HasFilterOrEffect()) {
            // clip hole for filter/shadow
            DrawBackgroundWithoutFilterAndEffect(canvas, params);
            DrawContent(canvas, params.GetFrameRect());
            DrawChildren(canvas, params.GetBounds());
            DrawForeground(canvas, params.GetBounds());
            return;
        }
    }

    switch (GetCacheType()) {
        case DrawableCacheType::NONE: {
            RSRenderNodeDrawable::OnDraw(canvas);
            break;
        }
        case DrawableCacheType::CONTENT: {
            RS_OPTIONAL_TRACE_NAME_FMT("DrawCachedSurface id:%llu", renderNode_->GetId());
             DrawBackground(canvas, params.GetBounds());
             DrawCachedSurface(*curCanvas, params.GetCacheSize(), gettid());
             DrawForeground(canvas, params.GetBounds());
             DrawDfxForCache(canvas, params.GetBounds());
            break;
        }
        default:
            break;
    }
}

void RSRenderNodeDrawable::DrawDfxForCache(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    if (!isDrawingCacheDfxEnabled_) {
        return;
    }
    Drawing::Rect dst;
    canvas.GetTotalMatrix().MapRect(dst, rect);
    RectI dfxRect(static_cast<int>(dst.GetLeft()), static_cast<int>(dst.GetTop()),
        static_cast<int>(dst.GetWidth()),static_cast<int>(dst.GetHeight()));
    drawingCacheRects_.emplace_back(dfxRect);
}

void RSRenderNodeDrawable::SetCacheType(DrawableCacheType cacheType)
{
    cacheType_ = cacheType;
}

DrawableCacheType RSRenderNodeDrawable::GetCacheType() const
{
    return cacheType_;
}



std::shared_ptr<Drawing::Surface> RSRenderNodeDrawable::GetCachedSurface(pid_t threadId) const
{
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
    return threadId == cacheThreadId_ ? cachedSurface_ : nullptr;
}

void RSRenderNodeDrawable::InitCachedSurface(Drawing::GPUContext* gpuContext, const Vector2f& cacheSize,
    pid_t threadId)
{
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (gpuContext == nullptr) {
        return;
    }
    ClearCachedSurface();
    cacheThreadId_ = threadId;
    auto width = static_cast<int32_t>(cacheSize.x_);
    auto height = static_cast<int32_t>(cacheSize.y_);

#ifdef RS_ENABLE_GL
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        Drawing::ImageInfo info = Drawing::ImageInfo::MakeN32Premul(width, height);
        std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
        cachedSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext, true, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
        cachedBackendTexture_ = RSUniRenderUtil::MakeBackendTexture(width, height);
        auto vkTextureInfo = cachedBackendTexture_.GetTextureInfo().GetVKTextureInfo();
        if (!cachedBackendTexture_.IsValid() || !vkTextureInfo) {
            return;
        }
        vulkanCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
        cachedSurface_ = Drawing::Surface::MakeFromBackendTexture(gpuContext, cachedBackendTexture_.GetTextureInfo(),
            Drawing::TextureOrigin::BOTTOM_LEFT, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, nullptr,
            NativeBufferUtils::DeleteVkImage, vulkanCleanupHelper_);
    }
#endif
#else
    cachedSurface_ = Drawing::Surface::MakeRasterN32Premul(static_cast<int32_t>(cacheSize.x_),
        static_cast<int32_t>(cacheSize.y_));
#endif
}

bool RSRenderNodeDrawable::NeedInitCachedSurface(const Vector2f& newSize)
{
    auto width = static_cast<int32_t>(newSize.x_);
    auto height = static_cast<int32_t>(newSize.y_);
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
    if (cachedSurface_ == nullptr) {
        return true;
    }
    auto cacheCanvas = cachedSurface_->GetCanvas();
    if (cacheCanvas == nullptr) {
        return true;
    }
    return cacheCanvas->GetWidth() != width || cacheCanvas->GetHeight() != height;
}

std::shared_ptr<Drawing::Image> RSRenderNodeDrawable::GetCachedImage(RSPaintFilterCanvas& canvas, pid_t threadId)
{
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
    if (!cachedSurface_) {
        RS_LOGE("RSRenderNodeDrawabl::GetCachedImage invalid cachedSurface_");
        return nullptr;
    }
    auto image = cachedSurface_->GetImageSnapshot();
    if (!image) {
        RS_LOGE("RSRenderNodeDrawable::GetCachedImage GetImageSnapshot failed");
        return nullptr;
    }
    if (threadId == cacheThreadId_) {
        return image;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = image->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("RSRenderNodeDrawable::GetCachedImage get backendTexture failed");
        return nullptr;
    }
    auto cacheImage = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat info = Drawing::BitmapFormat{ image->GetColorType(), image->GetAlphaType() };
    bool ret = cacheImage->BuildFromTexture(*canvas.GetGPUContext(), backendTexture.GetTextureInfo(),
        origin, info, nullptr);
    if (!ret) {
        RS_LOGE("RSRenderNodeDrawable::GetCachedImage image BuildFromTexture failed");
        return nullptr;
    }
    return cacheImage;
#else
    return image;
#endif
}

void RSRenderNodeDrawable::DrawCachedSurface(RSPaintFilterCanvas& canvas, const Vector2f& boundSize, pid_t threadId)
{
    auto cacheImage = GetCachedImage(canvas, threadId);
    if (cacheImage == nullptr) {
        return;
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (cacheImage->IsTextureBacked()) {
            RS_LOGI("RSRenderNodeDrawable::DrawCachedSurface convert cacheImage from texture to raster image");
            cacheImage = cacheImage->MakeRasterImage();
        }
    }
    float scaleX = boundSize.x_ / static_cast<float>(cacheImage->GetWidth());
    float scaleY = boundSize.y_ / static_cast<float>(cacheImage->GetHeight());

    Drawing::AutoCanvasRestore arc(canvas, true);
    canvas.Scale(scaleX, scaleY);
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    if (canvas.GetTotalMatrix().HasPerspective()) {
        // In case of perspective transformation, make dstRect 1px outset to anti-alias
        Drawing::Rect dst(0, 0,
            static_cast<float>(cacheImage->GetWidth()), static_cast<float>(cacheImage->GetHeight()));
        dst.MakeOutset(1, 1);
        canvas.DrawImageRect(*cacheImage, dst, samplingOptions);
    } else {
        canvas.DrawImage(*cacheImage, 0.0, 0.0, samplingOptions);
    }
    canvas.DetachBrush();
}

void RSRenderNodeDrawable::ClearCachedSurface()
{
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
    if (cachedSurface_ == nullptr) {
        return;
    }

    auto clearTask = [surface = cachedSurface_]() mutable {
        surface = nullptr;
    };
    cachedSurface_ = nullptr;
    RSTaskDispatcher::GetInstance().PostTask(cacheThreadId_.load(), clearTask);

#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        vulkanCleanupHelper_ = nullptr;
    }
#endif
}

bool RSRenderNodeDrawable::CheckIfNeedUpdateCache(RSRenderParams& params)
{
    int32_t updateTimes = 0;
    {
        std::lock_guard<std::mutex> lock(drawingCacheMapMutex_);
        if (drawingCacheUpdateTimeMap_.count(renderNode_->GetId()) > 0) {
            updateTimes = drawingCacheUpdateTimeMap_.at(renderNode_->GetId());
        } else {
            drawingCacheUpdateTimeMap_.emplace(renderNode_->GetId(), 0);
        }
    }

    RS_OPTIONAL_TRACE_NAME_FMT("CheckUpdateCache id:%llu updateTimes:%d type:%d cacheChanged:%d size:[%.2f, %.2f]",
        renderNode_->GetId(), updateTimes, params.GetDrawingCacheType(), params.GetDrawingCacheChanged(),
        params.GetCacheSize().x_, params.GetCacheSize().y_);

    if (params.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE &&
        updateTimes >= DRAWING_CACHE_MAX_UPDATE_TIME) {
        SetCacheType(DrawableCacheType::NONE);
        params.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        ClearCachedSurface();
        return false;
    }
    SetCacheType(DrawableCacheType::CONTENT);

    if (NeedInitCachedSurface(params.GetCacheSize())) {
        ClearCachedSurface();
        return true;
    }

    if (updateTimes == 0 || params.GetDrawingCacheChanged()) {
        params.SetDrawingCacheChanged(false, true);
        return true;
    }
    return false;
}

void RSRenderNodeDrawable::UpdateCacheSurface(Drawing::Canvas& canvas, const RSRenderParams& params)
{
    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    pid_t threadId = gettid();
    if (GetCachedSurface(threadId) == nullptr) {
        RS_TRACE_NAME_FMT("InitCachedSurface size:[%.2f, %.2f]", params.GetCacheSize().x_, params.GetCacheSize().y_);
        InitCachedSurface(curCanvas->GetGPUContext().get(), params.GetCacheSize(), threadId);
    }

    auto surface = GetCachedSurface(threadId);
    if (!surface) {
        RS_LOGE("RSRenderNodeDrawable::UpdateCacheSurface GetCachedSurface failed");
        return;
    }

    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(surface.get());
    if (!cacheCanvas) {
        return;
    }

    // copy current canvas properties into cacheCanvas
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine) {
        cacheCanvas->SetHighContrast(renderEngine->IsHighContrastEnabled());
    }
    cacheCanvas->CopyConfiguration(*curCanvas);
    // Using filter cache in multi-thread environment may cause GPU memory leak or invalid textures
    // [PLANNNING] disable it in sub-thread.
    // cacheCanvas->SetDisableFilterCache(isSubThread_);

    auto cacheCanvasPtr = cacheCanvas.get();
    // When drawing CacheSurface, all child node should be drawn.
    // So set isOpDropped_ = false here.
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;
    cacheCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);

    std::swap(cacheCanvasPtr, curCanvas);
    // draw content + children
    auto bounds = params.GetBounds();
    DrawContent(*curCanvas, params.GetFrameRect());
    DrawChildren(*curCanvas, bounds);

    std::swap(cacheCanvasPtr, curCanvas);
    isOpDropped_ = isOpDropped;

    // update cache updateTimes
    {
        std::lock_guard<std::mutex> lock(drawingCacheMapMutex_);
        drawingCacheUpdateTimeMap_[renderNode_->GetId()]++;
    }
}

int RSRenderNodeDrawable::GetProcessedNodeCount()
{
    return processedNodeCount_;
}

void RSRenderNodeDrawable::ProcessedNodeCountInc()
{
    ++processedNodeCount_;
}

void RSRenderNodeDrawable::ClearProcessedNodeCount()
{
    processedNodeCount_ = 0;
}
} // namespace OHOS::Rosen::DrawableV2

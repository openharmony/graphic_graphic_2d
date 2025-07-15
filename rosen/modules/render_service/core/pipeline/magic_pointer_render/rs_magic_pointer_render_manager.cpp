/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "common/rs_background_thread.h"
#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/sk_resource_manager.h"
#include "rs_magic_pointer_render_manager.h"
#include "rs_trace.h"
#ifdef USE_M133_SKIA
#include "src/gpu/ganesh/gl/GrGLDefines.h"
#else
#include "src/gpu/gl/GrGLDefines.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
static const std::string DISPLAY = "DisplayNode";
static const std::string POINTER = "pointer";
static const float RGB = 255.0f;
static const float HALF = 0.5f;
static const float MIN_BUFFER_NUMBER = 0.0f;
static const float MAX_BUFFER_NUMBER = 1.0f;
static const int64_t MAX_INTERVAL_TIME = 1000;
static const int32_t MAX_RANGE_SIZE = 100;
static const int32_t CALCULATE_MIDDLE = 2;
} // namespace
static std::unique_ptr<RSMagicPointerRenderManager> g_pointerRenderManagerInstance =
    std::make_unique<RSMagicPointerRenderManager>();

RSMagicPointerRenderManager& RSMagicPointerRenderManager::GetInstance()
{
    return *g_pointerRenderManagerInstance;
}

#if defined (RS_ENABLE_GL) && defined (RS_ENABLE_EGLIMAGE) || defined (RS_ENABLE_VK)
void RSMagicPointerRenderManager::InitInstance(const std::shared_ptr<RSImageManager>& imageManager)
{
    g_pointerRenderManagerInstance->imageManager_ = imageManager;
}
#endif

int64_t RSMagicPointerRenderManager::GetCurrentTime()
{
    auto timeNow = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    return std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch()).count();
}

void RSMagicPointerRenderManager::SetPointerColorInversionConfig(float darkBuffer, float brightBuffer,
    int64_t interval, int32_t rangeSize)
{
    std::lock_guard<std::mutex> locker(mtx_);
    if (darkBuffer > MIN_BUFFER_NUMBER && darkBuffer < MAX_BUFFER_NUMBER) {
        darkBuffer_ = darkBuffer;
    }
    if (brightBuffer > MIN_BUFFER_NUMBER && brightBuffer < MAX_BUFFER_NUMBER) {
        brightBuffer_ = brightBuffer;
    }
    if (interval > 0 && interval < MAX_INTERVAL_TIME) {
        colorSamplingInterval_ = interval;
    }
    if (rangeSize > 0 && rangeSize < MAX_RANGE_SIZE) {
        rangeSize_ = rangeSize;
    }
}

void RSMagicPointerRenderManager::SetPointerColorInversionEnabled(bool enable)
{
    std::lock_guard<std::mutex> locker(mtx_);
    isEnableCursorInversion_ = enable;
}

bool RSMagicPointerRenderManager::GetPointerColorInversionEnabled()
{
    std::lock_guard<std::mutex> locker(mtx_);
    return isEnableCursorInversion_;
}

void RSMagicPointerRenderManager::RegisterPointerLuminanceChangeCallback(pid_t pid,
    sptr<RSIPointerLuminanceChangeCallback> callback)
{
    std::lock_guard<std::mutex> locker(mtx_);
    colorChangeListeners_[pid] = callback;
}

void RSMagicPointerRenderManager::UnRegisterPointerLuminanceChangeCallback(pid_t pid)
{
    std::lock_guard<std::mutex> locker(mtx_);
    colorChangeListeners_.erase(pid);
}

void RSMagicPointerRenderManager::ExecutePointerLuminanceChangeCallback(int32_t brightness)
{
    for (auto it = colorChangeListeners_.begin(); it != colorChangeListeners_.end(); it++) {
        if (it->second) {
            it->second->OnPointerLuminanceChanged(brightness);
        }
    }
}

void RSMagicPointerRenderManager::CallPointerLuminanceChange(int32_t brightness)
{
    RS_LOGD("RSMagicPointerRenderManager::CallPointerLuminanceChange brightness:%{public}d.", brightness);
    lastColorPickerTime_ = RSMagicPointerRenderManager::GetCurrentTime();
    if (brightnessMode_ == CursorBrightness::NONE) {
        brightnessMode_ = brightness < static_cast<int32_t>(RGB * HALF) ?
            CursorBrightness::DARK : CursorBrightness::BRIGHT;
        ExecutePointerLuminanceChangeCallback(brightness);
    } else if (brightnessMode_ == CursorBrightness::DARK) {
        // Dark cursor to light cursor buffer
        if (brightness > static_cast<int32_t>(RGB * darkBuffer_)) {
            brightnessMode_ = CursorBrightness::BRIGHT;
            ExecutePointerLuminanceChangeCallback(brightness);
        }
    } else {
        // light cursor to Dark cursor buffer
        if (brightness < static_cast<int32_t>(RGB * brightBuffer_)) {
            brightnessMode_ = CursorBrightness::DARK;
            ExecutePointerLuminanceChangeCallback(brightness);
        }
    }
}

bool RSMagicPointerRenderManager::CheckColorPickerEnabled()
{
    if (!isEnableCursorInversion_ || taskDoing_) {
        return false;
    }

    auto time = RSMagicPointerRenderManager::GetCurrentTime() - lastColorPickerTime_;
    if (time < colorSamplingInterval_) {
        return false;
    }

    bool exists = false;
    auto& threadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (threadParams == nullptr) {
        ROSEN_LOGE("RSMagicPointerRenderManager::CheckColorPickerEnabled threadParams == nullptr");
        return false;
    }
    const auto& hardwareDrawables = threadParams->GetHardwareEnabledTypeDrawables();

    for (const auto& [_, __, drawable] : hardwareDrawables) {
        if (drawable == nullptr) {
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        if (surfaceDrawable != nullptr && surfaceDrawable->IsHardwareEnabledTopSurface()) {
            exists = true;
            break;
        }
    }

    return exists;
}

void RSMagicPointerRenderManager::ProcessColorPicker(std::shared_ptr<RSProcessor> processor,
    std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    if (!CheckColorPickerEnabled()) {
        ROSEN_LOGD("RSMagicPointerRenderManager::CheckColorPickerEnabled is false!");
        return;
    }

    RS_TRACE_BEGIN("RSMagicPointerRenderManager ProcessColorPicker");
    std::lock_guard<std::mutex> locker(mtx_);

    if (cacheImgForPointer_) {
        if (!GetIntersectImageBySubset(gpuContext)) {
            ROSEN_LOGE("RSMagicPointerRenderManager::GetIntersectImageBySubset is false!");
            return;
        }
    } else {
        if (!CalculateTargetLayer(processor)) { // get the layer intersect with pointer and calculate the rect
            ROSEN_LOGD("RSMagicPointerRenderManager::CalculateTargetLayer is false!");
            return;
        }
    }

    // post color picker task to background thread
    RunColorPickerTask();
    RS_TRACE_END();
}

bool RSMagicPointerRenderManager::GetIntersectImageBySubset(std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    if (gpuContext == nullptr) {
        return false;
    }
    const auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& [_, __, drawable] : hardwareDrawables) {
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        if (surfaceDrawable == nullptr || !surfaceDrawable->IsHardwareEnabledTopSurface()) {
            continue;
        }
        auto& surfaceParams = surfaceDrawable->GetRenderParams();
        if (!surfaceParams) {
            continue;
        }
        image_ = std::make_shared<Drawing::Image>();
        RectI pointerRect = surfaceParams->GetAbsDrawRect();
        CalculateColorRange(pointerRect);
        Drawing::RectI drawingPointerRect = Drawing::RectI(pointerRect.GetLeft(), pointerRect.GetTop(),
            pointerRect.GetRight(), pointerRect.GetBottom());
        image_->BuildSubset(cacheImgForPointer_, drawingPointerRect, *gpuContext);

        Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
        backendTexture_ = image_->GetBackendTexture(false, &origin);
        bitmapFormat_ = Drawing::BitmapFormat{ image_->GetColorType(), image_->GetAlphaType() };
        return true;
    }
    return false;
}

bool RSMagicPointerRenderManager::CalculateTargetLayer(std::shared_ptr<RSProcessor> processor)
{
#ifdef RS_ENABLE_GPU
    auto uniRenderProcessor = std::static_pointer_cast<RSUniRenderProcessor>(processor);
    if (uniRenderProcessor == nullptr) {
        ROSEN_LOGE("RSMagicPointerRenderManager::CalculateTargetLayer uniRenderProcessor is null!");
        return false;
    }
    std::vector<LayerInfoPtr> layers = uniRenderProcessor->GetLayers();
    forceCPU_ = RSBaseRenderEngine::NeedForceCPU(layers);

    std::sort(layers.begin(), layers.end(), [](LayerInfoPtr a, LayerInfoPtr b) {
        return a->GetZorder() < b->GetZorder();
    });
    // get pointer and display node layer
    bool find = false;
    RectI pRect;
    int displayNodeIndex = INT_MAX;
    for (int i = 0; i < static_cast<int>(layers.size()); ++i) {
        std::string name = layers[i]->GetSurface()->GetName();
        if (name.find(DISPLAY) != std::string::npos) {
            displayNodeIndex = i;
            continue;
        }
        if (name.find(POINTER) != std::string::npos) {
            GraphicIRect rect = layers[i]->GetLayerSize();
            pRect.SetAll(rect.x, rect.y, rect.w, rect.h);
            find = true;
            continue;
        }
    }

    if (!find || displayNodeIndex == INT_MAX) {
        ROSEN_LOGD("RSMagicPointerRenderManager::CalculateTargetLayer cannot find pointer or display node.");
        return false;
    }
    CalculateColorRange(pRect);
    // calculate the max intersection layer and rect
    GetRectAndTargetLayer(layers, pRect, displayNodeIndex);
#endif
    return true;
}

void RSMagicPointerRenderManager::CalculateColorRange(RectI& pRect)
{
    if (rangeSize_ > 0 && rangeSize_ < pRect.GetWidth() && rangeSize_ < pRect.GetHeight()) {
        int left = pRect.GetLeft() + (pRect.GetWidth() - rangeSize_) / CALCULATE_MIDDLE;
        int top = pRect.GetTop() + (pRect.GetHeight() - rangeSize_) / CALCULATE_MIDDLE;
        pRect.SetAll(left, top, rangeSize_, rangeSize_);
    }
}

void RSMagicPointerRenderManager::GetRectAndTargetLayer(std::vector<LayerInfoPtr>& layers, RectI& pRect,
    int displayNodeIndex)
{
    target_ = nullptr;
    rect_.Clear();

    for (int i = std::max(0, displayNodeIndex - 1); i >= 0; --i) {
        if (layers[i]->GetSurface()->GetName().find(POINTER) != std::string::npos) {
            continue;
        }
        GraphicIRect layerSize = layers[i]->GetLayerSize();
        RectI curRect = RectI(layerSize.x, layerSize.y, layerSize.w, layerSize.h);
        if (!curRect.Intersect(pRect)) {
            continue;
        }
        RectI dstRect = curRect.IntersectRect(pRect);
        if (dstRect.width_ * dstRect.height_ >= rect_.width_ * rect_.height_) {
            rect_ = dstRect;
            target_ = layers[i];
        }
    }

    // if not intersect with other layers, calculate the displayNode intersection rect
    if (target_ == nullptr) {
        target_ = layers[displayNodeIndex];
        GraphicIRect layerSize = target_->GetLayerSize();
        RectI curRect = RectI(layerSize.x, layerSize.y, layerSize.w, layerSize.h);
        RectI dstRect = curRect.IntersectRect(pRect);
    }
}

int16_t RSMagicPointerRenderManager::CalcAverageLuminance(std::shared_ptr<Drawing::Image> image)
{
    // create a 1x1 SkPixmap
    uint32_t pixel[1] = { 0 };
    Drawing::ImageInfo single_pixel_info(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL);
    Drawing::Bitmap single_pixel;
    single_pixel.Build(single_pixel_info, single_pixel_info.GetBytesPerPixel());
    single_pixel.SetPixels(pixel);

    // resize image to 1x1 to calculate average color
    // kMedium_SkFilterQuality will do bilerp + mipmaps for down-scaling, we can easily get average color
    image->ScalePixels(single_pixel,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR));
    // convert color format and return average luminance
    auto color = SkColor4f::FromBytes_RGBA(pixel[0]).toSkColor();
    return Drawing::Color::ColorQuadGetR(color) * 0.2126f + Drawing::Color::ColorQuadGetG(color) * 0.7152f +
        Drawing::Color::ColorQuadGetB(color) * 0.0722f;
}

void RSMagicPointerRenderManager::RunColorPickerTaskBackground(BufferDrawParam& param)
{
#ifdef RS_ENABLE_UNI_RENDER
    std::lock_guard<std::mutex> locker(mtx_);
    std::shared_ptr<Drawing::Image> image;
    auto context = RSBackgroundThread::Instance().GetShareGPUContext().get();
    if (backendTexturePre_.IsValid()) {
        image = std::make_shared<Drawing::Image>();
        SharedTextureContext* sharedContext = new SharedTextureContext(imagePre_);
        bool ret = image->BuildFromTexture(*context, backendTexturePre_.GetTextureInfo(),
            Drawing::TextureOrigin::BOTTOM_LEFT, bitmapFormat_, nullptr,
            SKResourceManager::DeleteSharedTextureContext, sharedContext);
        if (!ret) {
            RS_LOGE("RSMagicPointerRenderManager::RunColorPickerTaskBackground BuildFromTexture failed.");
            return;
        }
    } else {
        image = GetIntersectImageByLayer(param);
    }
    if (image == nullptr || !image->IsValid(context)) {
        RS_LOGE("RSMagicPointerRenderManager::RunColorPickerTaskBackground image not valid.");
        return;
    }

    luminance_ = CalcAverageLuminance(image);

    CallPointerLuminanceChange(luminance_);
#endif
}

void RSMagicPointerRenderManager::RunColorPickerTask()
{
    if (!imagePre_ && (target_ == nullptr || rect_.IsEmpty())) {
        ROSEN_LOGE("RSMagicPointerRenderManager::RunColorPickerTask failed for null target or rect is empty!");
        return;
    }

    BufferDrawParam param;
    if (!imagePre_) {
        param = RSUniRenderUtil::CreateLayerBufferDrawParam(target_, forceCPU_);
    }
    RSBackgroundThread::Instance().PostTask([this, param] () mutable {
        taskDoing_ = true;
        this->RunColorPickerTaskBackground(param);
        backendTexturePre_ = backendTexture_;
        backendTexture_ = Drawing::BackendTexture(false);
        imagePre_ = image_;
        image_ = nullptr;
        taskDoing_ = false;
    });
}

std::shared_ptr<Drawing::Image> RSMagicPointerRenderManager::GetIntersectImageByLayer(BufferDrawParam& param)
{
#ifdef RS_ENABLE_UNI_RENDER
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSPointerRenderManager::GetIntersectImageByLayer screenManager is null.");
        return nullptr;
    }
    auto screenInfo = screenManager->QueryScreenInfo(param.screenId);
    param.matrix.PostScale(screenInfo.GetRogWidthRatio(), screenInfo.GetRogHeightRatio());

    Drawing::Matrix mat;
    bool flag = param.matrix.Invert(mat);
    if (!flag) {
        RS_LOGE("RSPointerRenderManager::GetIntersectImageByLayer param.matrix not invert.");
        return nullptr;
    }
    Drawing::Rect rectOrigin = Drawing::Rect(rect_.GetLeft(), rect_.GetTop(), rect_.GetRight(), rect_.GetBottom());
    Drawing::Rect imgCutRectF;
    mat.MapRect(imgCutRectF, rectOrigin);
    Drawing::RectI imgCutRect = Drawing::RectI(imgCutRectF.GetLeft(), imgCutRectF.GetTop(), imgCutRectF.GetRight(),
        imgCutRectF.GetBottom());
    const auto context = RSBackgroundThread::Instance().GetShareGPUContext();
    if (context == nullptr) {
        RS_LOGE("RSMagicPointerRenderManager::GetIntersectImageByLayer context is nullptr.");
        return nullptr;
    }
    if (param.buffer == nullptr) {
        ROSEN_LOGE("RSMagicPointerRenderManager::GetIntersectImageByLayer param buffer is nullptr");
        return nullptr;
    }
#if (defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)) || defined(RS_ENABLE_VK)
    return imageManager_->GetIntersectImage(imgCutRect, context,
        param.buffer, param.acquireFence, param.threadIndex);
#endif
#endif
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS
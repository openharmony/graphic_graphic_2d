/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
#include "src/gpu/gl/GrGLDefines.h"
#include "rs_trace.h"
#include "pipeline/rs_uni_render_processor.h"
#include "pipeline/rs_uni_render_util.h"
#include "rs_pointer_render_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
static const std::string DISPLAY = "DisplayNode";
static const std::string POINTER = "pointer";
static const float RGB = 255.0f;
static const float HALF = 0.5f;
} // namespace
static std::unique_ptr<RSPointerRenderManager> g_pointerRenderManagerInstance =
    std::make_unique<RSPointerRenderManager>();


RSPointerRenderManager::RSPointerRenderManager()
{
    colorPickerTask_ = std::make_shared<RSColorPickerCacheTask>();
}

RSPointerRenderManager& RSPointerRenderManager::GetInstance()
{
    return *g_pointerRenderManagerInstance;
}

#if defined (RS_ENABLE_VK)
void RSPointerRenderManager::InitInstance(const std::shared_ptr<RSVkImageManager>& vkImageManager)
{
    g_pointerRenderManagerInstance->vkImageManager_ = vkImageManager;
}
#endif

#if defined (RS_ENABLE_GL) && defined (RS_ENABLE_EGLIMAGE)
void RSPointerRenderManager::InitInstance(const std::shared_ptr<RSEglImageManager>& eglImageManager)
{
    g_pointerRenderManagerInstance->eglImageManager_ = eglImageManager;
}
#endif

int64_t RSPointerRenderManager::GetCurrentTime()
{
    auto timeNow = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    return std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch()).count();
}

void RSPointerRenderManager::SetPointerColorInversionConfig(float darkBuffer, float brightBuffer, int64_t interval)
{
    std::lock_guard<std::mutex> lock(cursorInvertMutex_);
    darkBuffer_ = darkBuffer;
    brightBuffer_ = brightBuffer;
    colorSamplingInterval_ = interval;
}
 
void RSPointerRenderManager::SetPointerColorInversionEnabled(bool enable)
{
    std::lock_guard<std::mutex> lock(cursorInvertMutex_);
    isEnableCursorInversion_ = enable;
}
 
void RSPointerRenderManager::RegisterPointerLuminanceChangeCallback(pid_t pid,
    sptr<RSIPointerLuminanceChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(cursorInvertMutex_);
    colorChangeListeners_[pid] = callback;
}
 
void RSPointerRenderManager::UnRegisterPointerLuminanceChangeCallback(pid_t pid)
{
    std::lock_guard<std::mutex> lock(cursorInvertMutex_);
    colorChangeListeners_.erase(pid);
}
 
void RSPointerRenderManager::ExecutePointerLuminanceChangeCallback(int32_t brightness)
{
    std::lock_guard<std::mutex> lock(cursorInvertMutex_);
    lastColorPickerTime_ = RSPointerRenderManager::GetCurrentTime();
    for (auto it = colorChangeListeners_.begin(); it != colorChangeListeners_.end(); it++) {
        if (it->second) {
            it->second->OnPointerLuminanceChanged(brightness);
        }
    }
}

void RSPointerRenderManager::CallPointerLuminanceChange(int32_t brightness)
{
    RS_LOGD("RSPointerRenderManager::CallPointerLuminanceChange luminance_:%{public}d.", luminance_);
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

bool RSPointerRenderManager::CheckColorPickerEnabled()
{
    if (!isEnableCursorInversion_ || taskDoing_) {
        return false;
    }

    auto time = RSPointerRenderManager::GetCurrentTime() - lastColorPickerTime_;
    if (time < colorSamplingInterval_) {
        return false;
    }

    bool exists = false;
    auto& threadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (threadParams == nullptr) {
        ROSEN_LOGE("RSPointerRenderManager::CheckColorPickerEnabled threadParams == nullptr");
        return false;
    }
    auto& hardwareDrawables = threadParams->GetHardwareEnabledTypeDrawables();

    for (const auto& drawable : hardwareDrawables) {
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        if (surfaceDrawable != nullptr && surfaceDrawable->IsHardwareEnabledTopSurface()) {
            exists = true;
            break;
        }
    }

    return exists;
}

void RSPointerRenderManager::ProcessColorPicker(std::shared_ptr<RSProcessor> processor,
    std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    if (!CheckColorPickerEnabled()) {
        ROSEN_LOGD("RSPointerRenderManager::CheckColorPickerEnabled is false!");
        return;
    }

    RS_TRACE_BEGIN("RSPointerRenderManager ProcessColorPicker");
    std::lock_guard<std::mutex> locker(mtx_);
    image_ = nullptr;
    if (cacheImgForPointer_) {
        if (!GetIntersectImageBySubset(gpuContext)) {
            ROSEN_LOGE("RSPointerRenderManager::GetIntersectImageBySubset is false!");
            return;
        }
    } else {
        if (!CalculateTargetLayer(processor)) { // get the layer intersect with pointer and calculate the rect
            ROSEN_LOGD("RSPointerRenderManager::CalculateTargetLayer is false!");
            return;
        }
    }

    // post color picker task to background thread
    RunColorPickerTask();
    RS_TRACE_END();
}

bool RSPointerRenderManager::GetIntersectImageBySubset(std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& drawable : hardwareDrawables) {
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        if (surfaceDrawable == nullptr || !surfaceDrawable->IsHardwareEnabledTopSurface() ||
            !surfaceDrawable->GetRenderParams()) {
            continue;
        }
        image_ = std::make_shared<Drawing::Image>();
        RectI pointerRect = surfaceDrawable->GetRenderParams()->GetAbsDrawRect();
        Drawing::RectI drawingPointerRect = Drawing::RectI(pointerRect.GetLeft(), pointerRect.GetTop(),
            pointerRect.GetRight(), pointerRect.GetBottom());
        image_->BuildSubset(cacheImgForPointer_, drawingPointerRect, *gpuContext);
        return true;
    }
    return false;
}

bool RSPointerRenderManager::CalculateTargetLayer(std::shared_ptr<RSProcessor> processor)
{
    auto uniRenderProcessor = std::static_pointer_cast<RSUniRenderProcessor>(processor);
    if (uniRenderProcessor == nullptr) {
        ROSEN_LOGE("RSPointerRenderManager::CalculateTargetLayer uniRenderProcessor is null!");
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
    for (int i = 0; i < layers.size(); ++i) {
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
        ROSEN_LOGD("RSPointerRenderManager::CalculateTargetLayer cannot find pointer or display node.");
        return false;
    }

    // calculate the max intersection layer and rect
    GetRectAndTargetLayer(layers, pRect, displayNodeIndex);

    return true;
}

void RSPointerRenderManager::GetRectAndTargetLayer(std::vector<LayerInfoPtr>& layers, RectI& pRect,
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
            dstRect = dstRect.Offset(-curRect.GetLeft(), -curRect.GetTop());
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
        rect_ = dstRect.Offset(-curRect.GetLeft(), -curRect.GetTop());
    }
}

void RSPointerRenderManager::RunColorPickerTaskBackground(BufferDrawParam& param)
{
#ifdef RS_ENABLE_UNI_RENDER
    std::lock_guard<std::mutex> locker(mtx_);
    taskDoing_ = true;
    std::shared_ptr<Drawing::Image> image;
    auto context = RSBackgroundThread::Instance().GetShareGPUContext().get();
    if (image_) {
        image = image_;
    } else {
        image = GetIntersectImageByLayer(param);
        if (image == nullptr || !image->IsValid(context)) {
            RS_LOGE("RSPointerRenderManager::RunColorPickerTaskBackground image not valid.");
            taskDoing_ = false;
            return;
        }
    }
    if (!colorPickerTask_->InitTask(image)) {
        bool initStatus = colorPickerTask_->InitTask(image);
        if (!initStatus) {
            RS_LOGE("RSPointerRenderManager::RunColorPickerTaskBackground InitTask failed.");
            taskDoing_ = false;
            return;
        }
    }
    colorPickerTask_->SetStatus(CacheProcessStatus::DOING);
    if (!colorPickerTask_->InitSurface(context)) {
        RS_LOGE("RSPointerRenderManager::RunColorPickerTaskBackground InitSurface failed.");
        colorPickerTask_->SetStatus(CacheProcessStatus::WAITING);
        taskDoing_ = false;
        return;
    }
    if (!colorPickerTask_->Render()) {
        RS_LOGE("RSPointerRenderManager::RunColorPickerTaskBackground Render failed.");
        taskDoing_ = false;
        return;
    }
    colorPickerTask_->Reset();
    RSColor color;
    colorPickerTask_->GetColor(color);

    luminance_ = color.GetRed() * 0.2126f + color.GetGreen() * 0.7152f + color.GetBlue() * 0.0722f;
    taskDoing_ = false;
    CallPointerLuminanceChange(luminance_);
#endif
}

void RSPointerRenderManager::RunColorPickerTask()
{
    if (!image_ && (target_ == nullptr || rect_.IsEmpty())) {
        ROSEN_LOGE("RSPointerRenderManager::RunColorPickerTask failed for null target or rect is empty!");
        return;
    }

    BufferDrawParam param;
    if (!image_) {
        param = RSUniRenderUtil::CreateLayerBufferDrawParam(target_, forceCPU_);
    }
    RSBackgroundThread::Instance().PostTask([this, param] () {
        this->RunColorPickerTaskBackground(param);
    });
}

std::shared_ptr<Drawing::Image> RSPointerRenderManager::GetIntersectImageByLayer(const BufferDrawParam& param)
{
#ifdef RS_ENABLE_UNI_RENDER
    Drawing::RectI imgCutRect = Drawing::RectI(rect_.GetLeft(), rect_.GetTop(), rect_.GetRight(), rect_.GetBottom());
    auto context = RSBackgroundThread::Instance().GetShareGPUContext();
    if (context == nullptr) {
        RS_LOGE("RSPointerRenderManager::GetIntersectImageByLayer context is nullptr.");
        return nullptr;
    }
    if (param.buffer == nullptr) {
        ROSEN_LOGE("RSPointerRenderManager::GetIntersectImageByLayer param buffer is nullptr");
        return nullptr;
    }
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN
        || RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return GetIntersectImageFromVK(imgCutRect, context, param);
        }
#endif
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        return GetIntersectImageFromGL(imgCutRect, context, param);
    }
#endif
#endif
    return nullptr;
}

#ifdef RS_ENABLE_VK
std::shared_ptr<Drawing::Image> RSPointerRenderManager::GetIntersectImageFromVK(Drawing::RectI& imgCutRect,
    std::shared_ptr<Drawing::GPUContext>& context, const BufferDrawParam& param)
{
    if (vkImageManager_ == nullptr) {
        RS_LOGE("RSPointerRenderManager::GetIntersectImageFromVK vkImageManager_ == nullptr");
        return nullptr;
    }
    auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(param.buffer, param.acquireFence,
        param.threadIndex);
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSPointerRenderManager::GetIntersectImageFromVK imageCache == nullptr!");
        return nullptr;
    }
    auto& backendTexture = imageCache->GetBackendTexture();
    Drawing::BitmapFormat bitmapFormat = RSBaseRenderUtil::GenerateDrawingBitmapFormat(param.buffer);

    std::shared_ptr<Drawing::Image> layerImage = std::make_shared<Drawing::Image>();
    if (!layerImage->BuildFromTexture(*context, backendTexture.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr,
        NativeBufferUtils::DeleteVkImage, imageCache->RefCleanupHelper())) {
        ROSEN_LOGE("RSPointerRenderManager::GetIntersectImageFromVK image BuildFromTexture failed.");
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> cutDownImage = std::make_shared<Drawing::Image>();
    cutDownImage->BuildSubset(layerImage, imgCutRect, *context);
    return cutDownImage;
}
#endif

#if defined (RS_ENABLE_GL) && defined (RS_ENABLE_EGLIMAGE)
std::shared_ptr<Drawing::Image> RSPointerRenderManager::GetIntersectImageFromGL(Drawing::RectI& imgCutRect,
    std::shared_ptr<Drawing::GPUContext>& context, const BufferDrawParam& param)
{
    if (eglImageManager_ == nullptr) {
        RS_LOGE("RSPointerRenderManager::GetIntersectImageFromGL eglImageManager_ == nullptr");
        return nullptr;
    }
    auto eglTextureId = eglImageManager_->MapEglImageFromSurfaceBuffer(param.buffer,
        param.acquireFence, param.threadIndex);
    if (eglTextureId == 0) {
        RS_LOGE("RSPointerRenderManager::GetIntersectImageFromGL invalid texture ID");
        return nullptr;
    }

    Drawing::BitmapFormat bitmapFormat = RSBaseRenderUtil::GenerateDrawingBitmapFormat(param.buffer);
    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(param.buffer->GetSurfaceBufferWidth());
    externalTextureInfo.SetHeight(param.buffer->GetSurfaceBufferHeight());
    externalTextureInfo.SetIsMipMapped(false);
    externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
    externalTextureInfo.SetID(eglTextureId);
    auto glType = GR_GL_RGBA8;
    auto pixelFmt = param.buffer->GetFormat();
    if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
        glType = GR_GL_BGRA8;
    } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        glType = GL_RGB10_A2;
    }
    externalTextureInfo.SetFormat(glType);

    std::shared_ptr<Drawing::Image> layerImage = std::make_shared<Drawing::Image>();
    if (!layerImage->BuildFromTexture(*context, externalTextureInfo,
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
        RS_LOGE("RSPointerRenderManager::GetIntersectImageFromGL image BuildFromTexture failed");
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> cutDownImage = std::make_shared<Drawing::Image>();
    cutDownImage->BuildSubset(layerImage, imgCutRect, *context);
    Drawing::ImageInfo info = Drawing::ImageInfo(imgCutRect.GetWidth(), imgCutRect.GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL);

    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRenderTarget(context.get(), false, info);
    if (surface == nullptr) {
        RS_LOGE("RSPointerRenderManager::GetIntersectImageFromGL MakeRenderTarget failed.");
        return nullptr;
    }
    auto drawCanvas = std::make_shared<RSPaintFilterCanvas>(surface.get());
    drawCanvas->DrawImage(*cutDownImage, 0.f, 0.f, Drawing::SamplingOptions());
    surface->FlushAndSubmit(true);
    return surface.get()->GetImageSnapshot();
}
#endif
} // namespace Rosen
} // namespace OHOS
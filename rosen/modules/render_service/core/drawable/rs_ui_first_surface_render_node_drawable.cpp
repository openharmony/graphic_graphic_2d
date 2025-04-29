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

#include <memory>

#include "impl_interface/region_impl.h"
#include "rs_trace.h"

#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "draw/brush.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "rs_profiler.h"
#include "rs_frame_report.h"
#include "utils/rect.h"
#include "utils/region.h"
#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

namespace {
static const OHOS::Rosen::Drawing::Matrix IDENTITY_MATRIX = []() {
    OHOS::Rosen::Drawing::Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 1.0f);
    return matrix;
}();

constexpr float SCALE_DIFF = 0.01f;
}

namespace OHOS::Rosen::DrawableV2 {
CacheProcessStatus RSSurfaceRenderNodeDrawable::GetCacheSurfaceProcessedStatus() const
{
    return uiFirstParams.cacheProcessStatus_.load();
}

void RSSurfaceRenderNodeDrawable::SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus)
{
    if (cacheProcessStatus == CacheProcessStatus::DONE || cacheProcessStatus == CacheProcessStatus::SKIPPED) {
        RSUiFirstProcessStateCheckerHelper::NotifyAll([this, cacheProcessStatus] {
            uiFirstParams.cacheProcessStatus_.store(cacheProcessStatus);
        });
    } else {
        uiFirstParams.cacheProcessStatus_.store(cacheProcessStatus);
    }
}

std::shared_ptr<Drawing::Surface> RSSurfaceRenderNodeDrawable::GetCacheSurface(uint32_t threadIndex,
    bool needCheckThread, bool releaseAfterGet)
{
    if (releaseAfterGet) {
        return std::move(cacheSurface_);
    }
    if (!needCheckThread || cacheSurfaceThreadIndex_ == threadIndex || !cacheSurface_) {
        return cacheSurface_;
    }

    // freeze cache scene
    ClearCacheSurfaceInThread();
    return nullptr;
}

void RSSurfaceRenderNodeDrawable::ClearCacheSurfaceInThread()
{
    std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
    if (clearCacheSurfaceFunc_) {
        clearCacheSurfaceFunc_(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
            cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
    }
    ClearCacheSurface();
}

void RSSurfaceRenderNodeDrawable::ClearCacheSurfaceOnly()
{
    RS_TRACE_NAME_FMT("ClearCacheSurfaceOnly id:%" PRIu64, nodeId_);
    if (cacheSurface_ == nullptr) {
        return;
    }
    if (clearCacheSurfaceFunc_) {
        clearCacheSurfaceFunc_(
            std::move(cacheSurface_), nullptr, cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
    }
    ClearCacheSurface(false);
    cacheSurface_.reset();
}

Drawing::Matrix RSSurfaceRenderNodeDrawable::GetGravityMatrix(float imgWidth, float imgHeight)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetGravityTranslate surfaceParams is nullptr");
        return Drawing::Matrix();
    }
    auto gravity = surfaceParams->GetUIFirstFrameGravity();
    float boundsWidth = surfaceParams->GetCacheSize().x_;
    float boundsHeight = surfaceParams->GetCacheSize().y_;
    Drawing::Matrix gravityMatrix;
    RSPropertiesPainter::GetGravityMatrix(gravity, RectF {0.0f, 0.0f, boundsWidth, boundsHeight},
        imgWidth, imgHeight, gravityMatrix);
    return gravityMatrix;
}

std::shared_ptr<Drawing::Image> RSSurfaceRenderNodeDrawable::GetCompletedImage(
    RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst)
{
    auto gpuContext = canvas.GetGPUContext();
    if (!gpuContext) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage GetGPUContext nullptr");
        return nullptr;
    }
    if (isUIFirst) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
        if (!cacheCompletedBackendTexture_.IsValid()) {
            RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage invalid grBackendTexture_");
            return nullptr;
        }
        auto colorType = Drawing::COLORTYPE_RGBA_8888;
#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            if (!cacheCompletedSurface_ || !cacheCompletedCleanupHelper_) {
                RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage %{public}s is nullptr",
                    cacheCompletedSurface_ == nullptr ? "surface" : "cleanupHelper");
                return nullptr;
            }
        }
        auto vkTexture = cacheCompletedBackendTexture_.GetTextureInfo().GetVKTextureInfo();
        auto colorSpace = Drawing::ColorSpace::CreateSRGB();
        if (vkTexture != nullptr && vkTexture->format == VK_FORMAT_R16G16B16A16_SFLOAT) {
            colorType = Drawing::ColorType::COLORTYPE_RGBA_F16;
        } else if (targetColorGamut_ != GRAPHIC_COLOR_GAMUT_SRGB) {
            colorSpace =
                Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        }
#endif
        auto image = std::make_shared<Drawing::Image>();
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
        Drawing::BitmapFormat info = Drawing::BitmapFormat{ colorType,
            Drawing::ALPHATYPE_PREMUL };
#ifdef RS_ENABLE_GL
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
            image->BuildFromTexture(*gpuContext, cacheCompletedBackendTexture_.GetTextureInfo(),
                origin, info, nullptr);
        }
#endif

#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            image->BuildFromTexture(*gpuContext, cacheCompletedBackendTexture_.GetTextureInfo(),
                origin, info, colorSpace, NativeBufferUtils::DeleteVkImage, cacheCompletedCleanupHelper_->Ref());
        }
#endif
        return image;
#endif
    }

    if (!cacheCompletedSurface_) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage invalid cacheCompletedSurface");
        return nullptr;
    }
    auto completeImage = cacheCompletedSurface_->GetImageSnapshot();
    if (!completeImage) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage Get complete image failed");
        return nullptr;
    }
    if (threadIndex == completedSurfaceThreadIndex_) {
        return completeImage;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = completeImage->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage get backendTexture failed");
        return nullptr;
    }
    SharedTextureContext* sharedContext = new SharedTextureContext(completeImage);
    auto cacheImage = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat info =
        Drawing::BitmapFormat{ completeImage->GetColorType(), completeImage->GetAlphaType() };
    bool ret = cacheImage->BuildFromTexture(*gpuContext, backendTexture.GetTextureInfo(),
        origin, info, nullptr, SKResourceManager::DeleteSharedTextureContext, sharedContext);
    if (!ret) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage image BuildFromTexture failed");
        return nullptr;
    }
    return cacheImage;
#else
    return completeImage;
#endif
}

bool RSSurfaceRenderNodeDrawable::DrawCacheSurface(RSPaintFilterCanvas& canvas, const Vector2f& boundSize,
    uint32_t threadIndex, bool isUIFirst)
{
    if (ROSEN_EQ(boundsWidth_, 0.f) || ROSEN_EQ(boundsHeight_, 0.f)) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::DrawCacheSurface return %d", __LINE__);
        return false;
    }

    auto cacheImage = GetCompletedImage(canvas, threadIndex, isUIFirst);
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheImage, "cacheImage");
    if (cacheImage == nullptr || ROSEN_EQ(cacheImage->GetWidth(), 0) ||
        ROSEN_EQ(cacheImage->GetHeight(), 0)) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::DrawCacheSurface return %d", __LINE__);
        return false;
    }
    canvas.Save();
    const auto& gravityMatrix = GetGravityMatrix(cacheImage->GetWidth(), cacheImage->GetHeight());
    float scaleX = boundSize.x_ / static_cast<float>(cacheImage->GetWidth());
    float scaleY = boundSize.y_ / static_cast<float>(cacheImage->GetHeight());
    if (ROSEN_EQ(scaleY, scaleX, SCALE_DIFF)) {
        canvas.Scale(scaleX, scaleY);
    } else {
        canvas.Scale(gravityMatrix.Get(Drawing::Matrix::SCALE_X), gravityMatrix.Get(Drawing::Matrix::SCALE_Y));
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (cacheImage->IsTextureBacked()) {
            RS_LOGI("RSSurfaceRenderNodeDrawable::DrawCacheSurface convert cacheImage from texture to raster image");
            cacheImage = cacheImage->MakeRasterImage();
        }
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto translateX = gravityMatrix.Get(Drawing::Matrix::TRANS_X);
    auto translateY = gravityMatrix.Get(Drawing::Matrix::TRANS_Y);
    canvas.DrawImage(*cacheImage, translateX, translateY, samplingOptions);
    canvas.DetachBrush();
    canvas.Restore();
    return true;
}

void RSSurfaceRenderNodeDrawable::InitCacheSurface(Drawing::GPUContext* gpuContext, ClearCacheSurfaceFunc func,
    uint32_t threadIndex, bool isNeedFP16)
{
    if (func) {
        cacheSurfaceThreadIndex_ = threadIndex;
        if (!clearCacheSurfaceFunc_) {
            clearCacheSurfaceFunc_ = func;
        }
        if (cacheSurface_) {
            func(std::move(cacheSurface_), nullptr,
                cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
            cacheSurface_ = nullptr;
        }
    } else {
        cacheSurface_ = nullptr;
    }

    float width = 0.0f;
    float height = 0.0f;
    if (const auto& params = GetRenderParams()) {
        auto size = params->GetCacheSize();
        boundsWidth_ = size.x_;
        boundsHeight_ = size.y_;
    } else {
        RS_LOGE("uifirst cannot get cachesize");
    }

    width = boundsWidth_;
    height = boundsHeight_;

#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (gpuContext == nullptr) {
        if (func) {
            std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
            func(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
                cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
            ClearCacheSurface();
        }
        RS_LOGE("RSSurfaceRenderNodeDrawable::InitCacheSurface gpuContext == nullptr");
        return;
    }
#ifdef RS_ENABLE_GL
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        Drawing::ImageInfo info = Drawing::ImageInfo::MakeN32Premul(width, height);
        cacheSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext, true, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        auto colorType = Drawing::ColorType::COLORTYPE_RGBA_8888;
        auto colorSpace = Drawing::ColorSpace::CreateSRGB();
        if (isNeedFP16) {
            format = VK_FORMAT_R16G16B16A16_SFLOAT;
            colorType = Drawing::ColorType::COLORTYPE_RGBA_F16;
        } else if (targetColorGamut_ != GRAPHIC_COLOR_GAMUT_SRGB) {
            colorSpace =
                Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        }
        cacheBackendTexture_ = RSUniRenderUtil::MakeBackendTexture(width, height, format);
        auto vkTextureInfo = cacheBackendTexture_.GetTextureInfo().GetVKTextureInfo();
        if (!cacheBackendTexture_.IsValid() || !vkTextureInfo) {
            if (func) {
                std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
                func(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
                    cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
                ClearCacheSurface();
            }
            RS_LOGE("RSSurfaceRenderNodeDrawable::InitCacheSurface !cacheBackendTexture_.IsValid() || !vkTextureInfo");
            return;
        }
        cacheCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
        cacheSurface_ = Drawing::Surface::MakeFromBackendTexture(
            gpuContext, cacheBackendTexture_.GetTextureInfo(), Drawing::TextureOrigin::BOTTOM_LEFT,
            1, colorType, colorSpace, NativeBufferUtils::DeleteVkImage, cacheCleanupHelper_);
    }
#endif
#else
    cacheSurface_ = Drawing::Surface::MakeRasterN32Premul(width, height);
#endif
}
bool RSSurfaceRenderNodeDrawable::HasCachedTexture() const
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    return isTextureValid_.load();
#else
    return true;
#endif
}

bool RSSurfaceRenderNodeDrawable::NeedInitCacheSurface()
{
    int width = 0;
    int height = 0;

    if (const auto& params = GetRenderParams()) {
        auto size = params->GetCacheSize();
        width =  size.x_;
        height = size.y_;
    }

    if (cacheSurface_ == nullptr) {
        return true;
    }
    auto cacheCanvas = cacheSurface_->GetCanvas();
    if (cacheCanvas == nullptr) {
        return true;
    }
    return cacheCanvas->GetWidth() != width || cacheCanvas->GetHeight() != height;
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
void RSSurfaceRenderNodeDrawable::UpdateBackendTexture()
{
    RS_TRACE_NAME_FMT("UpdateBackendTexture id:%" PRIu64, nodeId_);
    if (cacheSurface_ == nullptr) {
        RS_LOGE("UpdateBackendTexture cacheSurface is nullptr");
        return;
    }
    cacheBackendTexture_ = cacheSurface_->GetBackendTexture();
}
#endif

void RSSurfaceRenderNodeDrawable::UpdateCompletedCacheSurface()
{
    RS_TRACE_NAME_FMT("UpdateCompletedCacheSurface id:%" PRIu64, nodeId_);
    // renderthread not use, subthread done not use
    std::swap(cacheSurface_, cacheCompletedSurface_);
    std::swap(cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
    std::swap(cacheSurfaceInfo_, cacheCompletedSurfaceInfo_);
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::swap(cacheBackendTexture_, cacheCompletedBackendTexture_);
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        std::swap(cacheCleanupHelper_, cacheCompletedCleanupHelper_);
    }
#endif
    SetTextureValidFlag(true);
    SetCacheSurfaceNeedUpdated(false);
#endif
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheSurface_, "cacheSurface_");
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheCompletedSurface_, "cacheCompletedSurface_");
}

void RSSurfaceRenderNodeDrawable::SetTextureValidFlag(bool isValid)
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    isTextureValid_.store(isValid);
#endif
}

void RSSurfaceRenderNodeDrawable::ClearCacheSurface(bool isClearCompletedCacheSurface)
{
    cacheSurface_ = nullptr;
    cacheSurfaceInfo_ = { -1, -1, -1.f };
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        cacheCleanupHelper_ = nullptr;
    }
#endif
    if (isClearCompletedCacheSurface) {
        std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
        cacheCompletedSurface_ = nullptr;
        cacheCompletedSurfaceInfo_ = { -1, -1, -1.f };
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            cacheCompletedCleanupHelper_ = nullptr;
        }
#endif
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
        isTextureValid_.store(false);
#endif
    }
}

bool RSSurfaceRenderNodeDrawable::IsCurFrameStatic()
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::IsCurFrameStatic params is nullptr");
        return false;
    }
    return surfaceParams->GetSurfaceCacheContentStatic();
}

void RSSurfaceRenderNodeDrawable::SetTaskFrameCount(uint64_t frameCount)
{
    frameCount_ = frameCount;
}

uint64_t RSSurfaceRenderNodeDrawable::GetTaskFrameCount() const
{
    return frameCount_;
}

void RSSurfaceRenderNodeDrawable::SubDraw(Drawing::Canvas& canvas)
{
    const auto& uifirstParams = GetUifirstRenderParams();
    auto debugSize = uifirstParams ? uifirstParams->GetCacheSize() : Vector2f(0.f, 0.f);
    RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::SubDraw[%s] w%.1f h%.1f",
        name_.c_str(), debugSize.x_, debugSize.y_);

    auto rscanvas = reinterpret_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::SubDraw, rscanvas is nullptr");
        return;
    }
    Drawing::Rect bounds = uifirstParams ? uifirstParams->GetBounds() : Drawing::Rect(0, 0, 0, 0);

    auto parentSurfaceMatrix = RSRenderParams::GetParentSurfaceMatrix();
    RSRenderParams::SetParentSurfaceMatrix(IDENTITY_MATRIX);

    ClearTotalProcessedSurfaceCount();
    RSRenderNodeDrawable::ClearProcessedNodeCount();
    RSRenderNodeDrawable::DrawUifirstContentChildren(*rscanvas, bounds);
    int totalNodes = GetProcessedNodeCount();
    int totalSurfaces = GetTotalProcessedSurfaceCount();
    const auto& params = GetRenderParams();
    float globalAlpha = params? params->GetGlobalAlpha() : -1.f;
    RS_TRACE_NAME_FMT("SubDraw totalSurfaces:%d totalNodes:%d alpha:%f", totalSurfaces, totalNodes, globalAlpha);
    if (totalSurfaces <= 0 || totalNodes <= 0) {
        RS_LOGI("uifirst subDraw id:%{public}" PRIu64 ",name:%{public}s,totalSurfaces:%{public}d,totalNodes:%{public}d"
            ",alpha:%{public}f", nodeId_, name_.c_str(), totalSurfaces, totalNodes, globalAlpha);
    } else {
        RS_LOGD("uifirst subDraw id:%{public}" PRIu64 ",name:%{public}s,totalSurfaces:%{public}d,totalNodes:%{public}d"
            ",alpha:%{public}f", nodeId_, name_.c_str(), totalSurfaces, totalNodes, globalAlpha);
    }
    RSRenderParams::SetParentSurfaceMatrix(parentSurfaceMatrix);
}

bool RSSurfaceRenderNodeDrawable::DrawUIFirstCache(RSPaintFilterCanvas& rscanvas, bool canSkipWait)
{
    RS_TRACE_NAME_FMT("DrawUIFirstCache_NOSTARTING");
    const auto& params = GetRenderParams();
    if (!params) {
        RS_LOGE("RSUniRenderUtil::HandleSubThreadNodeDrawable params is nullptr");
        return false;
    }

    static constexpr int REQUEST_SET_FRAME_LOAD_ID = 100006;
    static constexpr int REQUEST_FRAME_AWARE_LOAD = 90;
    static constexpr int REQUEST_FRAME_STANDARD_LOAD = 50;
    if (!HasCachedTexture()) {
        RS_TRACE_NAME_FMT("HandleSubThreadNode wait %d %" PRIu64 "", canSkipWait, nodeId_);
        if (canSkipWait) {
            RS_LOGI("uifirst skip wait id:%{public}" PRIu64 ",name:%{public}s", nodeId_, name_.c_str());
            return false; // draw nothing
        }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        bool frameParamEnable = RsFrameReport::GetInstance().GetEnable();
        if (frameParamEnable) {
            RsFrameReport::GetInstance().SetFrameParam(
                REQUEST_SET_FRAME_LOAD_ID, REQUEST_FRAME_AWARE_LOAD, 0, GetLastFrameUsedThreadIndex());
        }
        RSSubThreadManager::Instance()->WaitNodeTask(nodeId_);
        if (frameParamEnable) {
            RsFrameReport::GetInstance().SetFrameParam(
                REQUEST_SET_FRAME_LOAD_ID, REQUEST_FRAME_STANDARD_LOAD, 0, GetLastFrameUsedThreadIndex());
        }
        UpdateCompletedCacheSurface();
#endif
    }
    return DrawCacheSurface(rscanvas, params->GetCacheSize(), UNI_MAIN_THREAD_INDEX, true);
}

bool RSSurfaceRenderNodeDrawable::DrawUIFirstCacheWithStarting(RSPaintFilterCanvas& rscanvas, NodeId id)
{
    RS_TRACE_NAME_FMT("DrawUIFirstCacheWithStarting %d, nodeID:%" PRIu64 "", HasCachedTexture(), id);
    bool ret = true;
    auto drawable = RSRenderNodeDrawableAdapter::GetDrawableById(id);
    if (drawable) {
        const auto& startingParams = drawable->GetRenderParams();
        if (!HasCachedTexture() && startingParams && !ROSEN_EQ(startingParams->GetAlpha(), 1.0f)) {
            ret = DrawUIFirstCache(rscanvas, false);
            RS_TRACE_NAME_FMT("wait and drawStarting, GetAlpha:%f, GetGlobalAlpha:%f",
                startingParams->GetAlpha(), startingParams->GetGlobalAlpha());
            drawable->Draw(rscanvas);
            return ret;
        }
    }
    const auto& params = GetRenderParams();
    if (!params) {
        RS_LOGE("RSUniRenderUtil::HandleSubThreadNodeDrawable params is nullptr");
        return false;
    }
    // draw surface content&&childrensss
    if (HasCachedTexture()) {
        ret = DrawCacheSurface(rscanvas, params->GetCacheSize(), UNI_MAIN_THREAD_INDEX, true);
    }
    // draw starting window
    if (drawable) {
        RS_TRACE_NAME_FMT("drawStarting");
        drawable->Draw(rscanvas);
    }
    return ret;
}

void RSSurfaceRenderNodeDrawable::SetSubThreadSkip(bool isSubThreadSkip)
{
    isSubThreadSkip_ = isSubThreadSkip;
}

int RSSurfaceRenderNodeDrawable::GetTotalProcessedSurfaceCount() const
{
    return totalProcessedSurfaceCount_;
}

void RSSurfaceRenderNodeDrawable::TotalProcessedSurfaceCountInc(RSPaintFilterCanvas& canvas)
{
    if (canvas.GetIsParallelCanvas()) {
        ++totalProcessedSurfaceCount_;
    }
}

void RSSurfaceRenderNodeDrawable::ClearTotalProcessedSurfaceCount()
{
    totalProcessedSurfaceCount_ = 0;
}

uint32_t RSSurfaceRenderNodeDrawable::GetUifirstPostOrder() const
{
    return uifirstPostOrder_;
}

void RSSurfaceRenderNodeDrawable::SetUifirstPostOrder(uint32_t order)
{
    uifirstPostOrder_ = order;
}

void RSSurfaceRenderNodeDrawable::UpdateCacheSurfaceInfo()
{
    const auto& params = GetRenderParams();
    if (params) {
        cacheSurfaceInfo_.processedSurfaceCount = GetTotalProcessedSurfaceCount();
        cacheSurfaceInfo_.processedNodeCount = GetProcessedNodeCount();
        cacheSurfaceInfo_.alpha = params->GetGlobalAlpha();
    }
}
} // namespace OHOS::Rosen

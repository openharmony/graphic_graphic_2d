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

#include "rs_sub_thread_cache.h"

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
#include "render/rs_drawing_filter.h"
#include "rs_profiler.h"
#include "utils/graphic_coretrace.h"
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
RsSubThreadCache::RsSubThreadCache()
    : syncUifirstDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{}

CacheProcessStatus RsSubThreadCache::GetCacheSurfaceProcessedStatus() const
{
    return uiFirstParams.cacheProcessStatus_.load();
}

void RsSubThreadCache::SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus)
{
    if (cacheProcessStatus == CacheProcessStatus::DONE || cacheProcessStatus == CacheProcessStatus::SKIPPED) {
        RSUiFirstProcessStateCheckerHelper::NotifyAll([this, cacheProcessStatus] {
            uiFirstParams.cacheProcessStatus_.store(cacheProcessStatus);
        });
    } else {
        uiFirstParams.cacheProcessStatus_.store(cacheProcessStatus);
    }
}

std::shared_ptr<Drawing::Surface> RsSubThreadCache::GetCacheSurface(uint32_t threadIndex)
{
    if (cacheSurfaceThreadIndex_ == threadIndex) {
        return cacheSurface_;
    }
    return nullptr;
}

void RsSubThreadCache::ClearCacheSurfaceInThread()
{
    RS_TRACE_NAME_FMT("ClearCacheSurfaceInThread id:%" PRIu64, GetNodeId());
    RS_LOGI("ClearCacheSurfaceInThread id:%{public}" PRIu64, GetNodeId());
    std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
    if (clearCacheSurfaceFunc_) {
        clearCacheSurfaceFunc_(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
            cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
    }
    ClearCacheSurface();
}

void RsSubThreadCache::ClearCacheSurfaceOnly()
{
    RS_TRACE_NAME_FMT("ClearCacheSurfaceOnly id:%" PRIu64, GetNodeId());
    RS_LOGI("ClearCacheSurfaceOnly id:%{public}" PRIu64, GetNodeId());
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

std::shared_ptr<Drawing::Image> RsSubThreadCache::GetCompletedImage(
    RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_GETCOMPLETEDIMAGE);
    auto gpuContext = canvas.GetGPUContext();
    if (!gpuContext) {
        RS_LOGE("RsSubThreadCache::GetCompletedImage GetGPUContext nullptr");
        return nullptr;
    }
    if (isUIFirst) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
        if (!cacheCompletedBackendTexture_.IsValid()) {
            RS_LOGE("RsSubThreadCache::GetCompletedImage invalid grBackendTexture_");
            return nullptr;
        }
        auto colorType = Drawing::COLORTYPE_RGBA_8888;
#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            if (!cacheCompletedSurface_ || !cacheCompletedCleanupHelper_) {
                RS_LOGE("RsSubThreadCache::GetCompletedImage %{public}s is nullptr",
                    cacheCompletedSurface_ == nullptr ? "surface" : "cleanupHelper");
                return nullptr;
            }
        }
        auto vkTexture = cacheCompletedBackendTexture_.GetTextureInfo().GetVKTextureInfo();
        // When the colorType is FP16, the colorspace of the uifirst buffer must be sRGB
        // In other cases, ensure the image's color space matches the target surface's color profile.
        auto colorSpace = Drawing::ColorSpace::CreateSRGB();
        if (vkTexture != nullptr && vkTexture->format == VK_FORMAT_R16G16B16A16_SFLOAT) {
            colorType = Drawing::ColorType::COLORTYPE_RGBA_F16;
        } else if (cacheCompletedSurface_) {
            colorSpace = cacheCompletedSurface_->GetImageInfo().GetColorSpace();
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
        RS_LOGE("RsSubThreadCache::GetCompletedImage invalid cacheCompletedSurface");
        return nullptr;
    }
    auto completeImage = cacheCompletedSurface_->GetImageSnapshot();
    if (!completeImage) {
        RS_LOGE("RsSubThreadCache::GetCompletedImage Get complete image failed");
        return nullptr;
    }
    if (threadIndex == completedSurfaceThreadIndex_) {
        return completeImage;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = completeImage->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("RsSubThreadCache::GetCompletedImage get backendTexture failed");
        return nullptr;
    }
    SharedTextureContext* sharedContext = new SharedTextureContext(completeImage);
    auto cacheImage = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat info =
        Drawing::BitmapFormat{ completeImage->GetColorType(), completeImage->GetAlphaType() };
    bool ret = cacheImage->BuildFromTexture(*gpuContext, backendTexture.GetTextureInfo(),
        origin, info, nullptr, SKResourceManager::DeleteSharedTextureContext, sharedContext);
    if (!ret) {
        RS_LOGE("RsSubThreadCache::GetCompletedImage image BuildFromTexture failed");
        return nullptr;
    }
    return cacheImage;
#else
    return completeImage;
#endif
}

bool RsSubThreadCache::DrawCacheSurface(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    RSPaintFilterCanvas& canvas, const Vector2f& boundSize, uint32_t threadIndex, bool isUIFirst)
{
    if (!surfaceDrawable) {
        RS_LOGE("DrawCacheSurface surfaceDrawable is nullptr");
        return false;
    }
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_DRAWCACHESURFACE);
    if (ROSEN_EQ(surfaceDrawable->boundsWidth_, 0.f) || ROSEN_EQ(surfaceDrawable->boundsHeight_, 0.f)) {
        RS_LOGE("RsSubThreadCache::DrawCacheSurface return %d", __LINE__);
        return false;
    }

    auto cacheImage = GetCompletedImage(canvas, threadIndex, isUIFirst);
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheImage, "cacheImage");
    if (cacheImage == nullptr || ROSEN_EQ(cacheImage->GetWidth(), 0) ||
        ROSEN_EQ(cacheImage->GetHeight(), 0)) {
        RS_LOGE("RsSubThreadCache::DrawCacheSurface return %d", __LINE__);
        return false;
    }
    canvas.Save();
    const auto& gravityMatrix = surfaceDrawable->GetGravityMatrix(cacheImage->GetWidth(), cacheImage->GetHeight());
    float scaleX = boundSize.x_ / static_cast<float>(cacheImage->GetWidth());
    float scaleY = boundSize.y_ / static_cast<float>(cacheImage->GetHeight());
    if (ROSEN_EQ(scaleY, scaleX, SCALE_DIFF)) {
        canvas.Scale(scaleX, scaleY);
    } else {
        canvas.Scale(gravityMatrix.Get(Drawing::Matrix::SCALE_X), gravityMatrix.Get(Drawing::Matrix::SCALE_Y));
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (cacheImage->IsTextureBacked()) {
            RS_LOGI("RsSubThreadCache::DrawCacheSurface convert cacheImage from texture to raster image");
            cacheImage = cacheImage->MakeRasterImage();
            if (!cacheImage) {
                RS_LOGE("RsSubThreadCache::DrawCacheSurface: MakeRasterImage failed");
                canvas.Restore();
                return false;
            }
        }
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto translateX = gravityMatrix.Get(Drawing::Matrix::TRANS_X);
    auto translateY = gravityMatrix.Get(Drawing::Matrix::TRANS_Y);
    DrawBehindWindowBeforeCache(canvas, translateX, translateY);
    auto stencilVal = canvas.GetStencilVal();
    if (stencilVal > Drawing::Canvas::INVALID_STENCIL_VAL && stencilVal < canvas.GetMaxStencilVal()) {
        RS_OPTIONAL_TRACE_NAME_FMT("DrawImageWithStencil, stencilVal: %" PRId64 "", stencilVal);
        RS_LOGD("DrawImageWithStencil, stencilVal: %{public}" PRId64 "", stencilVal);
        canvas.DrawImageWithStencil(*cacheImage, translateX, translateY, samplingOptions,
            static_cast<uint32_t>(stencilVal));
    } else {
        canvas.DrawImage(*cacheImage, translateX, translateY, samplingOptions);
    }
    canvas.DetachBrush();
    canvas.Restore();
    return true;
}

void RsSubThreadCache::InitCacheSurface(Drawing::GPUContext* gpuContext,
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable,
    ClearCacheSurfaceFunc func, uint32_t threadIndex, bool isNeedFP16)
{
    if (!nodeDrawable) {
        RS_LOGE("RsSubThreadCache::InitCacheSurface nodeDrawable is nullptr");
        return;
    }
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_INITCACHESURFACE);
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
    if (const auto& params = nodeDrawable->GetRenderParams()) {
        auto size = params->GetCacheSize();
        nodeDrawable->boundsWidth_ = size.x_;
        nodeDrawable->boundsHeight_ = size.y_;
    } else {
        RS_LOGE("uifirst cannot get cachesize");
    }

    width = nodeDrawable->boundsWidth_;
    height = nodeDrawable->boundsHeight_;

#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (gpuContext == nullptr) {
        if (func) {
            std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
            func(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
                cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
            ClearCacheSurface();
        }
        RS_LOGE("RsSubThreadCache::InitCacheSurface gpuContext == nullptr");
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
        // When the colorType is FP16, the colorspace of the uifirst buffer must be sRGB
        // In other cases, the colorspace follows the targetColorGamut_
        auto colorSpace = Drawing::ColorSpace::CreateSRGB();
        RS_LOGD("RsSubThreadCache::InitCacheSurface sub thread cache's targetColorGamut_ is [%{public}d]",
            targetColorGamut_);
        if (isNeedFP16) {
            format = VK_FORMAT_R16G16B16A16_SFLOAT;
            colorType = Drawing::ColorType::COLORTYPE_RGBA_F16;
            RS_LOGD("RsSubThreadCache::InitCacheSurface colorType is FP16, take colorspace to sRGB");
        } else if (targetColorGamut_ != GRAPHIC_COLOR_GAMUT_SRGB) {
            colorSpace =
                Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        }
        cacheBackendTexture_ = NativeBufferUtils::MakeBackendTexture(
            width, height, ExtractPid(nodeDrawable->nodeId_), format);
        auto vkTextureInfo = cacheBackendTexture_.GetTextureInfo().GetVKTextureInfo();
        if (!cacheBackendTexture_.IsValid() || !vkTextureInfo) {
            if (func) {
                std::scoped_lock<std::recursive_mutex> lock(completeResourceMutex_);
                func(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
                    cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
                ClearCacheSurface();
            }
            RS_LOGE("RsSubThreadCache::InitCacheSurface !cacheBackendTexture_.IsValid() || !vkTextureInfo");
            return;
        }
        cacheCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory, vkTextureInfo->vkAlloc.statName);
        cacheSurface_ = Drawing::Surface::MakeFromBackendTexture(
            gpuContext, cacheBackendTexture_.GetTextureInfo(), Drawing::TextureOrigin::BOTTOM_LEFT,
            1, colorType, colorSpace, NativeBufferUtils::DeleteVkImage, cacheCleanupHelper_);
    }
#endif
#else
    cacheSurface_ = Drawing::Surface::MakeRasterN32Premul(width, height);
#endif
}

void RsSubThreadCache::ResetUifirst(bool isOnlyClearCache)
{
    RS_LOGI("ResetUifirst id:%{public}" PRIu64 ", isOnlyClearCache:%{public}d", GetNodeId(), isOnlyClearCache);
    if (isOnlyClearCache) {
        ClearCacheSurfaceOnly();
    } else {
        ClearCacheSurfaceInThread();
    }
}

bool RsSubThreadCache::HasCachedTexture() const
{
    return isCacheCompletedValid_;
}

bool RsSubThreadCache::IsCacheValid() const
{
    return isCacheValid_;
}

bool RsSubThreadCache::NeedInitCacheSurface(RSSurfaceRenderParams* surfaceParams)
{
    int width = 0;
    int height = 0;

    if (surfaceParams) {
        auto size = surfaceParams->GetCacheSize();
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
void RsSubThreadCache::UpdateBackendTexture()
{
    RS_TRACE_NAME_FMT("UpdateBackendTexture");
    if (cacheSurface_ == nullptr) {
        RS_LOGE("UpdateBackendTexture cacheSurface is nullptr");
        return;
    }
    cacheBackendTexture_ = cacheSurface_->GetBackendTexture();
    isCacheValid_ = true;
}
#endif

void RsSubThreadCache::UpdateCompletedCacheSurface()
{
    RS_TRACE_NAME_FMT("UpdateCompletedCacheSurface");
    if (cacheSurface_ == nullptr || !IsCacheValid()) {
        RS_LOGE("cacheSurface is nullptr, cache and completeCache swap failed");
        isCacheValid_ = false;
        return;
    }
    // renderthread not use, subthread done not use
    std::swap(cacheSurface_, cacheCompletedSurface_);
    std::swap(cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
    std::swap(cacheSurfaceInfo_, cacheCompletedSurfaceInfo_);
    std::swap(cacheBehindWindowData_, cacheCompletedBehindWindowData_);
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::swap(cacheBackendTexture_, cacheCompletedBackendTexture_);
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        std::swap(cacheCleanupHelper_, cacheCompletedCleanupHelper_);
    }
#endif
    std::swap(isCacheValid_, isCacheCompletedValid_);
    isTextureValid_.store(true);
    SetCacheSurfaceNeedUpdated(false);
#endif
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheSurface_, "cacheSurface_");
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheCompletedSurface_, "cacheCompletedSurface_");
}

void RsSubThreadCache::ClearCacheSurface(bool isClearCompletedCacheSurface)
{
    cacheSurface_ = nullptr;
    cacheSurfaceInfo_ = { -1, -1, -1.f };
    isCacheValid_ = false;
    ResetCacheBehindWindowData();
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
        isCacheCompletedValid_ = false;
        ResetCacheCompletedBehindWindowData();
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

void RsSubThreadCache::SetTaskFrameCount(uint64_t frameCount)
{
    frameCount_ = frameCount;
}

uint64_t RsSubThreadCache::GetTaskFrameCount() const
{
    return frameCount_;
}

std::shared_ptr<RSDirtyRegionManager> RsSubThreadCache::GetSyncUifirstDirtyManager() const
{
    return syncUifirstDirtyManager_;
}

bool RsSubThreadCache::UpdateCacheSurfaceDirtyManager(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    bool hasCompleteCache, bool isLastFrameSkip)
{
    if (!surfaceDrawable) {
        RS_LOGE("UpdateCacheSurfaceDirtyManager surfaceDrawable is nullptr");
        return false;
    }
    if (!syncUifirstDirtyManager_ || !surfaceDrawable->syncDirtyManager_) {
        RS_LOGE("UpdateCacheSurfaceDirtyManager dirty manager is nullptr");
        return false;
    }
    syncUifirstDirtyManager_->Clear();
    auto curDirtyRegion = surfaceDrawable->syncDirtyManager_->GetDirtyRegion();
    auto& curFrameDirtyRegion = surfaceDrawable->syncDirtyManager_->GetUifirstFrameDirtyRegion();
    curDirtyRegion = curDirtyRegion.JoinRect(curFrameDirtyRegion);
    if (isLastFrameSkip) {
        curDirtyRegion = curDirtyRegion.JoinRect(syncUifirstDirtyManager_->GetUiLatestHistoryDirtyRegions(1));
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("UpdateCacheSurfaceDirtyManager surfaceParams is nullptr");
        return false;
    }
    if (!hasCompleteCache) {
        curDirtyRegion = surfaceParams->GetAbsDrawRect();
    }
    RS_TRACE_NAME_FMT("UpdateCacheSurfaceDirtyManager[%s] %" PRIu64", curDirtyRegion[%d %d %d %d], hasCache:%d",
        surfaceDrawable->GetName().c_str(), surfaceDrawable->GetId(), curDirtyRegion.GetLeft(),
        curDirtyRegion.GetTop(), curDirtyRegion.GetWidth(), curDirtyRegion.GetHeight(), hasCompleteCache);
    syncUifirstDirtyManager_->MergeDirtyRect(curDirtyRegion);
    // set history dirty count
    syncUifirstDirtyManager_->SetBufferAge(1); // 1 means buffer age
    // update history dirty count
    syncUifirstDirtyManager_->UpdateDirty(false);
    return true;
}

void RsSubThreadCache::UpdateUifirstDirtyManager(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable)
{
    if (!surfaceDrawable) {
        RS_LOGE("UpdateUifirstDirtyManager surfaceDrawable is nullptr");
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("UpdateUifirstDirtyManager params is nullptr");
        UpdateDirtyRecordCompletatedState(false);
        return;
    }
    bool isCacheValid = IsCacheValid();
    bool isLastFrameSkip = GetSurfaceSkipCount() > 0;
    auto isRecordCompletate = UpdateCacheSurfaceDirtyManager(surfaceDrawable, isCacheValid, isLastFrameSkip);
    // nested surfacenode uifirstDirtyManager update is required
    for (const auto& nestedDrawable : surfaceDrawable->
        GetDrawableVectorById(surfaceParams->GetAllSubSurfaceNodeIds())) {
        auto surfaceNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(nestedDrawable);
        if (surfaceNodeDrawable) {
            isRecordCompletate = isRecordCompletate && surfaceNodeDrawable->GetRsSubThreadCache()
                .UpdateCacheSurfaceDirtyManager(surfaceNodeDrawable.get(), isCacheValid, isLastFrameSkip);
        }
    }
    UpdateDirtyRecordCompletatedState(isRecordCompletate);
}

bool RsSubThreadCache::IsDirtyRecordCompletated()
{
    bool isDirtyRecordCompletated = isDirtyRecordCompletated_;
    isDirtyRecordCompletated_ = false;
    return isDirtyRecordCompletated;
}

void RsSubThreadCache::UpdateDirtyRecordCompletatedState(bool isCompletate)
{
    isDirtyRecordCompletated_ = isCompletate;
}

void RsSubThreadCache::SetUifirstDirtyRegion(Drawing::Region dirtyRegion)
{
    uifirstDirtyRegion_ = dirtyRegion;
}

Drawing::Region RsSubThreadCache::GetUifirstDirtyRegion() const
{
    return uifirstDirtyRegion_;
}

void RsSubThreadCache::SetUifrstDirtyEnableFlag(bool dirtyEnableFlag)
{
    uifrstDirtyEnableFlag_ = dirtyEnableFlag;
}

bool RsSubThreadCache::GetUifrstDirtyEnableFlag() const
{
    return uifrstDirtyEnableFlag_;
}

bool RsSubThreadCache::GetCurDirtyRegionWithMatrix(const Drawing::Matrix& matrix,
    Drawing::RectF& latestDirtyRect, Drawing::RectF& absDrawRect)
{
    Drawing::Matrix inverseMatrix;
    if (!matrix.Invert(inverseMatrix)) {
        return false;
    }
    Drawing::RectF latestDirtyRectTemp = {0, 0, 0, 0};
    Drawing::RectF absDrawRectTemp = {0, 0, 0, 0};
    std::swap(latestDirtyRectTemp, latestDirtyRect);
    std::swap(absDrawRectTemp, absDrawRect);
    inverseMatrix.MapRect(latestDirtyRect, latestDirtyRectTemp);
    inverseMatrix.MapRect(absDrawRect, absDrawRectTemp);
    return true;
}

bool RsSubThreadCache::CalculateUifirstDirtyRegion(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    Drawing::RectI& dirtyRect)
{
    if (!surfaceDrawable) {
        RS_LOGE("CalculateUifirstDirtyRegion surfaceDrawable is nullptr");
        return false;
    }
    auto uifirstDirtyManager = GetSyncUifirstDirtyManager();
    if (!uifirstDirtyManager) {
        RS_LOGE("CalculateUifirstDirtyRegion uifirstDirtyManager is nullptr");
        return false;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("CalculateUifirstDirtyRegion surfaceParams is nullptr");
        return false;
    }
    RectI latestDirtyRect = uifirstDirtyManager->GetUiLatestHistoryDirtyRegions();
    if (latestDirtyRect.IsEmpty()) {
        dirtyRect = {};
        return true;
    }
    auto absDrawRect = surfaceParams->GetAbsDrawRect();
    if (absDrawRect.GetWidth() == 0 || absDrawRect.GetHeight() == 0 ||
        !absDrawRect.IsInsideOf(surfaceParams->GetScreenRect())) {
        RS_LOGD("absRect params is err or out of dispaly");
        return false;
    }
    Drawing::RectF curDrityRegion = Drawing::RectF(latestDirtyRect.GetLeft(), latestDirtyRect.GetTop(),
        latestDirtyRect.GetRight(), latestDirtyRect.GetBottom());
    Drawing::RectF curAbsDrawRect = Drawing::RectF(absDrawRect.GetLeft(), absDrawRect.GetTop(),
        absDrawRect.GetRight(), absDrawRect.GetBottom());
    if (!GetCurDirtyRegionWithMatrix(surfaceParams->GetDirtyRegionMatrix(), curDrityRegion, curAbsDrawRect)) {
        return false;
    }
    auto surfaceBounds = surfaceParams->GetBounds();
    float widthScale = surfaceBounds.GetWidth() / curAbsDrawRect.GetWidth();
    float heightScale = surfaceBounds.GetHeight() / curAbsDrawRect.GetHeight();
    float left = (curDrityRegion.GetLeft() - curAbsDrawRect.GetLeft()) * widthScale;
    float top = (curDrityRegion.GetTop() - curAbsDrawRect.GetTop()) * heightScale;
    float width = curDrityRegion.GetWidth() * widthScale;
    float height = curDrityRegion.GetHeight() * heightScale;
    Drawing::RectF tempRect = Drawing::RectF(left, top, left + width, top + height);
    dirtyRect = tempRect.RoundOut();
    RS_TRACE_NAME_FMT("lR[%.1f %.1f %.1f %.1f], absR[%.1f %.1f %.1f %.1f], resultR[%d %d %d %d]",
        curDrityRegion.GetLeft(), curDrityRegion.GetTop(), curDrityRegion.GetWidth(), curDrityRegion.GetHeight(),
        curAbsDrawRect.GetLeft(), curAbsDrawRect.GetTop(), curAbsDrawRect.GetWidth(), curAbsDrawRect.GetHeight(),
        dirtyRect.GetLeft(), dirtyRect.GetTop(), dirtyRect.GetWidth(), dirtyRect.GetHeight());
    return true;
}

bool RsSubThreadCache::MergeUifirstAllSurfaceDirtyRegion(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    Drawing::RectI& dirtyRects)
{
    if (!surfaceDrawable) {
        RS_LOGE("MergeUifirstAllSurfaceDirtyRegion surfaceDrawable is nullptr");
        return false;
    }
    uifirstMergedDirtyRegion_ = {};
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("CalculateUifirstDirtyRegion params is nullptr");
        return false;
    }
    if (!surfaceParams->IsLeashWindow() || !IsDirtyRecordCompletated()) {
        RS_LOGD("MergeUifirstAllSurfaceDirtyRegion not support");
        return false;
    }
    Drawing::RectI tempRect = {};
    bool isCalculateSucc = CalculateUifirstDirtyRegion(surfaceDrawable, tempRect);
    uifirstMergedDirtyRegion_.SetRect(tempRect);
    dirtyRects.Join(tempRect);
    for (const auto& nestedDrawable : surfaceDrawable->
        GetDrawableVectorById(surfaceParams->GetAllSubSurfaceNodeIds())) {
        auto surfaceNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(nestedDrawable);
        if (surfaceNodeDrawable) {
            tempRect = {};
            isCalculateSucc = isCalculateSucc && surfaceNodeDrawable->GetRsSubThreadCache()
                .CalculateUifirstDirtyRegion(surfaceNodeDrawable.get(), tempRect);
            Drawing::Region resultRegion;
            resultRegion.SetRect(tempRect);
            uifirstMergedDirtyRegion_.Op(resultRegion, Drawing::RegionOp::UNION);
            dirtyRects.Join(tempRect);
        }
    }
    return isCalculateSucc;
}

void RsSubThreadCache::UpadteAllSurfaceUifirstDirtyEnableState(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    bool isEnableDirtyRegion)
{
    if (!surfaceDrawable) {
        RS_LOGE("UpadteAllSurfaceUifirstDirtyEnableState surfaceDrawable is nullptr");
        return;
    }
    SetUifirstDirtyRegion(uifirstMergedDirtyRegion_);
    SetUifrstDirtyEnableFlag(isEnableDirtyRegion);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("UpadteAllSurfaceUifirstDirtyState params is nullptr");
        return;
    }
    for (const auto& nestedDrawable : surfaceDrawable->
        GetDrawableVectorById(surfaceParams->GetAllSubSurfaceNodeIds())) {
        auto surfaceNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(nestedDrawable);
        if (surfaceNodeDrawable) {
            surfaceNodeDrawable->GetRsSubThreadCache().SetUifirstDirtyRegion(uifirstMergedDirtyRegion_);
            surfaceNodeDrawable->GetRsSubThreadCache().SetUifrstDirtyEnableFlag(isEnableDirtyRegion);
        }
    }
}

void RsSubThreadCache::SubDraw(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable, Drawing::Canvas& canvas)
{
    if (!surfaceDrawable) {
        RS_LOGE("SubDraw surfaceDrawable is nullptr");
        return;
    }
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_SUBDRAW);
    const auto& uifirstParams = surfaceDrawable->GetUifirstRenderParams();
    auto debugSize = uifirstParams ? uifirstParams->GetCacheSize() : Vector2f(0.f, 0.f);
    RS_TRACE_NAME_FMT("RsSubThreadCache::SubDraw[%s] %" PRIu64 ", w%.1f h%.1f, isHigh:%d",
        surfaceDrawable->GetName().c_str(), surfaceDrawable->GetId(), debugSize.x_, debugSize.y_,
        IsHighPostPriority());

    auto rscanvas = reinterpret_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RsSubThreadCache::SubDraw, rscanvas is nullptr");
        return;
    }
    Drawing::Rect bounds = uifirstParams ? uifirstParams->GetBounds() : Drawing::Rect(0, 0, 0, 0);

    auto parentSurfaceMatrix = RSRenderParams::GetParentSurfaceMatrix();
    RSRenderParams::SetParentSurfaceMatrix(IDENTITY_MATRIX);

    // merge uifirst dirty region
    Drawing::RectI uifirstSurfaceDrawRects = {};
    auto dirtyEnableFlag = MergeUifirstAllSurfaceDirtyRegion(surfaceDrawable, uifirstSurfaceDrawRects) &&
        RSSystemProperties::GetUIFirstDirtyEnabled() &&
        RSUifirstManager::Instance().GetUiFirstType() == UiFirstCcmType::MULTI;
    UpadteAllSurfaceUifirstDirtyEnableState(surfaceDrawable, dirtyEnableFlag);
    if (!dirtyEnableFlag) {
        rscanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    } else {
        rscanvas->ClipRegion(uifirstMergedDirtyRegion_);
        rscanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
    {
        RS_TRACE_NAME_FMT("uifirstDirtyMerged[%d %d %d %d], dirtyEnableFlag:%d",
            uifirstSurfaceDrawRects.GetLeft(), uifirstSurfaceDrawRects.GetTop(),
            uifirstSurfaceDrawRects.GetWidth(), uifirstSurfaceDrawRects.GetHeight(), dirtyEnableFlag);
    }

    ClearTotalProcessedSurfaceCount();
    RSRenderNodeDrawable::ClearProcessedNodeCount();
    surfaceDrawable->DrawUifirstContentChildren(*rscanvas, bounds);
    int totalNodes = RSRenderNodeDrawable::GetProcessedNodeCount();
    int totalSurfaces = GetTotalProcessedSurfaceCount();
    const auto& params = surfaceDrawable->GetRenderParams();
    float globalAlpha = params? params->GetGlobalAlpha() : -1.f;
    RS_TRACE_NAME_FMT("SubDraw totalSurfaces:%d totalNodes:%d alpha:%f", totalSurfaces, totalNodes, globalAlpha);
    if (totalSurfaces <= 0 || totalNodes <= 0) {
        RS_LOGI("uifirst subDraw id:%{public}" PRIu64 ",name:%{public}s,totalSurfaces:%{public}d,totalNodes:%{public}d"
            ",alpha:%{public}f", surfaceDrawable->GetId(), surfaceDrawable->name_.c_str(), totalSurfaces, totalNodes,
            globalAlpha);
    } else {
        RS_LOGD("uifirst subDraw id:%{public}" PRIu64 ",name:%{public}s,totalSurfaces:%{public}d,totalNodes:%{public}d"
            ",alpha:%{public}f", surfaceDrawable->GetId(), surfaceDrawable->name_.c_str(), totalSurfaces, totalNodes,
            globalAlpha);
    }
    RSRenderParams::SetParentSurfaceMatrix(parentSurfaceMatrix);
    // uifirst dirty dfx
    UifirstDirtyRegionDfx(*rscanvas, uifirstSurfaceDrawRects);
}

void RsSubThreadCache::PushDirtyRegionToStack(RSPaintFilterCanvas& canvas, Drawing::Region& resultRegion)
{
    if (canvas.GetIsParallelCanvas()) {
        if (GetUifrstDirtyEnableFlag()) {
            auto uifirstDirtyRegion = GetUifirstDirtyRegion();
            canvas.PushDirtyRegion(uifirstDirtyRegion);
        }
    } else {
        canvas.PushDirtyRegion(resultRegion);
    }
}

void RsSubThreadCache::UifirstDirtyRegionDfx(Drawing::Canvas& canvas, Drawing::RectI& surfaceDrawRect)
{
    if (!RSSystemProperties::GetUIFirstDirtyDebugEnabled()) {
        return;
    }
    const int defaultTextOffsetX = 6; // text position is 6 pixelSize right side of the Rect
    const int defaultTextOffsetY = 30; // text position has 30 pixelSize under the Rect
    Drawing::Brush rectBrush;
    rectBrush.SetColor(Drawing::Color(0x80FFB6C1));
    rectBrush.SetAntiAlias(true);
    rectBrush.SetAlphaF(0.4f); // alpha 0.4 by default
    std::shared_ptr<Drawing::Typeface> typeFace = nullptr;
    std::string position = "pos:[" + surfaceDrawRect.ToString() + "]";
    // font size: 24
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromString(position.c_str(), Drawing::Font(typeFace, 24.0f, 0.6f, 0.0f));
    canvas.AttachBrush(rectBrush);
    canvas.DrawRect(surfaceDrawRect);
    canvas.DetachBrush();
    canvas.AttachBrush(Drawing::Brush());
    canvas.DrawTextBlob(textBlob.get(),
        surfaceDrawRect.GetLeft() + defaultTextOffsetX, surfaceDrawRect.GetTop() + defaultTextOffsetY);
    canvas.DetachBrush();
}

bool RsSubThreadCache::DrawUIFirstCache(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    RSPaintFilterCanvas& rscanvas, bool canSkipWait)
{
    if (!surfaceDrawable) {
        RS_LOGE("DrawUIFirstCache surfaceDrawable is nullptr");
        return false;
    }
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_DRAWUIFIRSTCACHE);
    RS_TRACE_NAME_FMT("DrawUIFirstCache_NOSTARTING");
    const auto& params = surfaceDrawable->GetRenderParams();
    if (!params) {
        RS_LOGE("RsSubThreadCache::DrawUIFirstCache params is nullptr");
        return false;
    }

    static constexpr int REQUEST_SET_FRAME_LOAD_ID = 100006;
    static constexpr int REQUEST_FRAME_AWARE_LOAD = 90;
    static constexpr int REQUEST_FRAME_STANDARD_LOAD = 50;
    if (!HasCachedTexture()) {
        RS_TRACE_NAME_FMT("HandleSubThreadNode wait %d %" PRIu64 "", canSkipWait, surfaceDrawable->nodeId_);
        if (canSkipWait) {
            RS_LOGI("uifirst skip wait id:%{public}" PRIu64 ",name:%{public}s", surfaceDrawable->nodeId_,
                surfaceDrawable->name_.c_str());
            return false; // draw nothing
        }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        bool frameParamEnable = RsFrameReport::GetInstance().GetEnable();
        if (frameParamEnable) {
            RsFrameReport::GetInstance().SetFrameParam(
                REQUEST_SET_FRAME_LOAD_ID, REQUEST_FRAME_AWARE_LOAD, 0, GetLastFrameUsedThreadIndex());
        }
        RSSubThreadManager::Instance()->WaitNodeTask(surfaceDrawable->nodeId_);
        if (frameParamEnable) {
            RsFrameReport::GetInstance().SetFrameParam(
                REQUEST_SET_FRAME_LOAD_ID, REQUEST_FRAME_STANDARD_LOAD, 0, GetLastFrameUsedThreadIndex());
        }
        UpdateCompletedCacheSurface();
#endif
    }
    return DrawCacheSurface(surfaceDrawable, rscanvas, params->GetCacheSize(), UNI_MAIN_THREAD_INDEX, true);
}

bool RsSubThreadCache::DrawUIFirstCacheWithStarting(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    RSPaintFilterCanvas& rscanvas, NodeId id)
{
    if (!surfaceDrawable) {
        RS_LOGE("DrawUIFirstCacheWithStarting surfaceDrawable is nullptr");
        return false;
    }
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_DRAWUIFIRSTCACHEWITHSTARTING);
    RS_TRACE_NAME_FMT("DrawUIFirstCacheWithStarting %d, nodeID:%" PRIu64 "", HasCachedTexture(), id);
    bool ret = true;
    auto drawable = RSRenderNodeDrawableAdapter::GetDrawableById(id);
    if (drawable) {
        const auto& startingParams = drawable->GetRenderParams();
        if (!HasCachedTexture() && startingParams && !ROSEN_EQ(startingParams->GetAlpha(), 1.0f)) {
            ret = DrawUIFirstCache(surfaceDrawable, rscanvas, false);
            RS_TRACE_NAME_FMT("wait and drawStarting, GetAlpha:%f, GetGlobalAlpha:%f",
                startingParams->GetAlpha(), startingParams->GetGlobalAlpha());
            drawable->Draw(rscanvas);
            return ret;
        }
    }
    const auto& params = surfaceDrawable->GetRenderParams();
    if (!params) {
        RS_LOGE("RSUniRenderUtil::HandleSubThreadNodeDrawable params is nullptr");
        return false;
    }
    // draw surface content&&childrensss
    if (HasCachedTexture()) {
        ret = DrawCacheSurface(surfaceDrawable, rscanvas, params->GetCacheSize(), UNI_MAIN_THREAD_INDEX, true);
    }
    // draw starting window
    if (drawable) {
        RS_TRACE_NAME_FMT("drawStarting");
        drawable->Draw(rscanvas);
    }
    return ret;
}

void RsSubThreadCache::SetSubThreadSkip(bool isSubThreadSkip)
{
    isSubThreadSkip_ = isSubThreadSkip;
}

int RsSubThreadCache::GetTotalProcessedSurfaceCount() const
{
    return totalProcessedSurfaceCount_;
}

void RsSubThreadCache::TotalProcessedSurfaceCountInc(RSPaintFilterCanvas& canvas)
{
    if (canvas.GetIsParallelCanvas()) {
        ++totalProcessedSurfaceCount_;
    }
}

void RsSubThreadCache::ClearTotalProcessedSurfaceCount()
{
    totalProcessedSurfaceCount_ = 0;
}

void RsSubThreadCache::ProcessSurfaceSkipCount()
{
    isSurfaceSkipCount_++;
}

void RsSubThreadCache::ResetSurfaceSkipCount()
{
    isSurfaceSkipCount_ = 0;
    isSurfaceSkipPriority_ = 0;
}

int32_t RsSubThreadCache::GetSurfaceSkipCount() const
{
    return isSurfaceSkipCount_;
}

int32_t RsSubThreadCache::GetSurfaceSkipPriority()
{
    return ++isSurfaceSkipPriority_;
}

uint32_t RsSubThreadCache::GetUifirstPostOrder() const
{
    return uifirstPostOrder_;
}

void RsSubThreadCache::SetUifirstPostOrder(uint32_t order)
{
    uifirstPostOrder_ = order;
}

bool RsSubThreadCache::IsHighPostPriority()
{
    return isHighPostPriority_;
}

void RsSubThreadCache::SetHighPostPriority(bool postPriority)
{
    isHighPostPriority_ = postPriority;
}

void RsSubThreadCache::UpdateCacheSurfaceInfo(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable)
{
    if (!nodeDrawable) {
        RS_LOGE("UpdateCacheSurfaceInfo nodeDrawable is nullptr");
        return;
    }
    const auto& params = nodeDrawable->GetRenderParams();
    if (params) {
        cacheSurfaceInfo_.processedSurfaceCount = GetTotalProcessedSurfaceCount();
        cacheSurfaceInfo_.processedNodeCount = RSRenderNodeDrawable::GetProcessedNodeCount();
        cacheSurfaceInfo_.alpha = params->GetGlobalAlpha();
    }
}

bool RsSubThreadCache::DealWithUIFirstCache(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams, RSRenderThreadParams& uniParams)
{
    if (!surfaceDrawable) {
        RS_LOGE("DealWithUIFirstCache surfaceDrawable is nullptr");
        return false;
    }
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_DEALWITHUIFIRSTCACHE);
    if (drawWindowCache_.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, uniParams)) {
        return true;
    }
    auto enableType = surfaceParams.GetUifirstNodeEnableParam();
    auto cacheState = GetCacheSurfaceProcessedStatus();
    if ((!RSUniRenderThread::GetCaptureParam().isSnapshot_ && enableType == MultiThreadCacheType::NONE &&
        // WAITING may change to DOING in subThread at any time
        cacheState != CacheProcessStatus::WAITING && cacheState != CacheProcessStatus::DOING) ||
        (RSUniRenderThread::GetCaptureParam().isSnapshot_ && !HasCachedTexture())) {
        return false;
    }
    RS_TRACE_NAME_FMT("DrawUIFirstCache [%s] %" PRIu64 ", type %d, cacheState:%d",
        surfaceParams.GetName().c_str(), surfaceParams.GetId(), enableType, cacheState);
    Drawing::Rect bounds = surfaceParams.GetBounds();
    RSAutoCanvasRestore acr(&canvas);
    // Alpha and matrix have been applied in func CaptureSurface
    if (!RSUniRenderThread::GetCaptureParam().isSnapshot_ && !RSUniRenderThread::GetCaptureParam().isMirror_) {
        canvas.MultiplyAlpha(surfaceParams.GetAlpha());
        canvas.ConcatMatrix(surfaceParams.GetMatrix());
    }
    // This branch is entered only when the conditions for executing the DrawUIFirstCache function are met.
    if (surfaceParams.GetGlobalPositionEnabled() &&
        surfaceParams.GetUifirstUseStarting() == INVALID_NODEID) {
        auto matrix = surfaceParams.GetMatrix();
        Drawing::Matrix inverseMatrix;
        if (!matrix.Invert(inverseMatrix)) {
            RS_LOGW("RsSubThreadCache::%{public}s name: %{public}s matrix invert inverseMatrix Failed", __func__, surfaceParams.GetName().c_str());
        }
        canvas.ConcatMatrix(inverseMatrix);
        canvas.Translate(-surfaceDrawable->offsetX_, -surfaceDrawable->offsetY_);
        canvas.ConcatMatrix(matrix);
        RS_LOGD("RsSubThreadCache::DealWithUIFirstCache Translate screenId=[%{public}" PRIu64 "] "
            "offsetX=%{public}d offsetY=%{public}d", surfaceDrawable->curDisplayScreenId_, surfaceDrawable->offsetX_,
            surfaceDrawable->offsetY_);
    }

    auto stencilVal = surfaceParams.GetStencilVal();
    if (surfaceParams.IsLeashWindow()) {
        surfaceDrawable->DrawLeashWindowBackground(canvas, bounds,
            uniParams.IsStencilPixelOcclusionCullingEnabled(), stencilVal);
    } else {
        surfaceDrawable->DrawBackground(canvas, bounds);
    }
    canvas.SetStencilVal(stencilVal);
    bool drawCacheSuccess = DrawUIFirstCache(surfaceDrawable, canvas, false);
    canvas.SetStencilVal(Drawing::Canvas::INVALID_STENCIL_VAL);
    if (!drawCacheSuccess) {
        surfaceDrawable->SetDrawSkipType(DrawSkipType::UI_FIRST_CACHE_FAIL);
        RS_TRACE_NAME_FMT("[%s] reuse failed!", surfaceParams.GetName().c_str());
        RS_LOGI("uifirst %{public}s drawcache failed! id:%{public}" PRIu64, surfaceDrawable->name_.c_str(),
            surfaceDrawable->nodeId_);
    }
    surfaceDrawable->DrawForeground(canvas, bounds);
    surfaceDrawable->DrawWatermark(canvas, surfaceParams);
    if (uniParams.GetUIFirstDebugEnabled()) {
        DrawUIFirstDfx(canvas, enableType, surfaceParams, drawCacheSuccess);
    }
    return true;
}

void RsSubThreadCache::DrawUIFirstDfx(RSPaintFilterCanvas& canvas, MultiThreadCacheType enableType,
    RSSurfaceRenderParams& surfaceParams, bool drawCacheSuccess)
{
    auto sizeDebug = surfaceParams.GetCacheSize();
    Drawing::Brush rectBrush;
    if (drawCacheSuccess) {
        if (enableType == MultiThreadCacheType::ARKTS_CARD) {
            // rgba: Alpha 128, blue 128
            rectBrush.SetColor(Drawing::Color(0, 0, 128, 128));
        } else {
            // rgba: Alpha 128, green 128, blue 128
            rectBrush.SetColor(Drawing::Color(0, 128, 128, 128));
        }
    } else {
        // rgba: Alpha 128, red 128
        rectBrush.SetColor(Drawing::Color(128, 0, 0, 128));
    }
    canvas.AttachBrush(rectBrush);
    canvas.DrawRect(Drawing::Rect(0, 0, sizeDebug.x_, sizeDebug.y_));
    canvas.DetachBrush();
}

bool RsSubThreadCache::BufferFormatNeedUpdate(std::shared_ptr<Drawing::Surface> cacheSurface,
    bool isNeedFP16)
{
    bool bufferFormatNeedUpdate = cacheSurface ? isNeedFP16 &&
        cacheSurface->GetImageInfo().GetColorType() != Drawing::ColorType::COLORTYPE_RGBA_F16 : false;
    RS_LOGD("RsSubThreadCache::BufferFormatNeedUpdate: %{public}d", bufferFormatNeedUpdate);
    return bufferFormatNeedUpdate;
}

void RsSubThreadCache::SetCacheBehindWindowData(const std::shared_ptr<RSPaintFilterCanvas::CacheBehindWindowData>& data)
{
    cacheBehindWindowData_ = data;
}

void RsSubThreadCache::SetCacheCompletedBehindWindowData(
    const std::shared_ptr<RSPaintFilterCanvas::CacheBehindWindowData>& data)
{
    cacheCompletedBehindWindowData_ = data;
}

void RsSubThreadCache::ResetCacheBehindWindowData()
{
    cacheBehindWindowData_.reset();
}

void RsSubThreadCache::ResetCacheCompletedBehindWindowData()
{
    cacheCompletedBehindWindowData_.reset();
}

void RsSubThreadCache::DrawBehindWindowBeforeCache(RSPaintFilterCanvas& canvas,
    const Drawing::scalar px, const Drawing::scalar py)
{
    if (!cacheCompletedBehindWindowData_) {
        RS_LOGD("RsSubThreadCache::DrawBehindWindowBeforeCache no need to draw");
        return;
    }
    if (!cacheCompletedBehindWindowData_->filter_ || !cacheCompletedBehindWindowData_->rect_.IsValid()) {
        RS_LOGE("RsSubThreadCache::DrawBehindWindowBeforeCache data is not valid");
        return;
    }
    auto surface = canvas.GetSurface();
    if (!surface) {
        RS_LOGE("RsSubThreadCache::DrawBehindWindowBeforeCache surface is nullptr");
        return;
    }
    RSAutoCanvasRestore acr(&canvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    canvas.Translate(px, py);
    Drawing::Rect absRect;
    canvas.GetTotalMatrix().MapRect(absRect, cacheCompletedBehindWindowData_->rect_);
    Drawing::RectI imageRect(std::ceil(absRect.GetLeft()), std::ceil(absRect.GetTop()), std::ceil(absRect.GetRight()),
        std::ceil(absRect.GetBottom()));
    auto deviceRect = Drawing::RectI(0, 0, surface->Width(), surface->Height());
    imageRect.Intersect(deviceRect);
    auto filter = std::static_pointer_cast<RSDrawingFilter>(cacheCompletedBehindWindowData_->filter_);
    auto imageSnapshot = surface->GetImageSnapshot(imageRect);
    if (!imageSnapshot) {
        RS_LOGE("RsSubThreadCache::DrawBehindWindowBeforeCache imageSnapshot is nullptr");
        return;
    }
    filter->PreProcess(imageSnapshot);
    Drawing::Rect srcRect = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    Drawing::Rect dstRect = imageRect;
    canvas.ResetMatrix();
    filter->DrawImageRect(canvas, imageSnapshot, srcRect, dstRect);
    filter->PostProcess(canvas);
    RS_TRACE_NAME_FMT("RsSubThreadCache::DrawBehindWindowBeforeCache imageRect:%s", imageRect.ToString().c_str());
}
} // namespace OHOS::Rosen

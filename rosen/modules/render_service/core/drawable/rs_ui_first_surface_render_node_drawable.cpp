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

#include "drawable/rs_surface_render_node_drawable.h"

#include <memory>
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "impl_interface/region_impl.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"

#ifdef RS_PARALLEL
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "draw/brush.h"

#include "platform/common/rs_log.h"
#include "utils/rect.h"
#include "utils/region.h"

#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#ifdef RS_ENABLE_VK
namespace {
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton();
    VkPhysicalDevice physicalDevice = vkContext.GetPhysicalDevice();

    VkPhysicalDeviceMemoryProperties memProperties;
    vkContext.vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return UINT32_MAX;
}

void SetVkImageInfo(std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo,
    const VkImageCreateInfo& imageInfo)
{
    vkImageInfo->imageTiling = imageInfo.tiling;
    vkImageInfo->imageLayout = imageInfo.initialLayout;
    vkImageInfo->format = imageInfo.format;
    vkImageInfo->imageUsageFlags = imageInfo.usage;
    vkImageInfo->levelCount = imageInfo.mipLevels;
    vkImageInfo->currentQueueFamily = VK_QUEUE_FAMILY_EXTERNAL;
    vkImageInfo->ycbcrConversionInfo = {};
    vkImageInfo->sharingMode = imageInfo.sharingMode;
}

OHOS::Rosen::Drawing::BackendTexture MakeBackendTexture(uint32_t width, uint32_t height,
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM)
{
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkImageCreateInfo imageInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {width, height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton();
    VkDevice device = vkContext.GetDevice();
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    if (vkContext.vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        return {};
    }

    VkMemoryRequirements memRequirements;
    vkContext.vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (allocInfo.memoryTypeIndex == UINT32_MAX) {
        return {};
    }

    if (vkContext.vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        return {};
    }

    vkContext.vkBindImageMemory(device, image, memory, 0);

    OHOS::Rosen::Drawing::BackendTexture backendTexture(true);
    OHOS::Rosen::Drawing::TextureInfo textureInfo;
    textureInfo.SetWidth(width);
    textureInfo.SetHeight(height);

    std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo =
        std::make_shared<OHOS::Rosen::Drawing::VKTextureInfo>();
    vkImageInfo->vkImage = image;
    vkImageInfo->vkAlloc.memory = memory;
    vkImageInfo->vkAlloc.size = memRequirements.size;

    SetVkImageInfo(vkImageInfo, imageInfo);
    textureInfo.SetVKTextureInfo(vkImageInfo);
    backendTexture.SetTextureInfo(textureInfo);
    return backendTexture;
}
} // un-named
#endif //RS_ENABLE_VK
#endif //RS_PARALLEL
namespace OHOS::Rosen::DrawableV2 {
#ifdef RS_PARALLEL
CacheProcessStatus RSSurfaceRenderNodeDrawable::GetCacheSurfaceProcessedStatus() const
{
    return uiFirstParams.cacheProcessStatus_.load();
}

void RSSurfaceRenderNodeDrawable::SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus)
{
    uiFirstParams.cacheProcessStatus_.store(cacheProcessStatus);
}

std::shared_ptr<Drawing::Surface> RSSurfaceRenderNodeDrawable::GetCacheSurface(uint32_t threadIndex, bool needCheckThread,
    bool releaseAfterGet)
{
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (releaseAfterGet) {
            return std::move(cacheSurface_);
        }
        if (!needCheckThread || cacheSurfaceThreadIndex_ == threadIndex || !cacheSurface_) {
            return cacheSurface_;
        }
    }

    // freeze cache scene
    ClearCacheSurfaceInThread();
    return nullptr;
}

std::shared_ptr<Drawing::Surface> RSSurfaceRenderNodeDrawable::GetCompletedCacheSurface(uint32_t threadIndex, bool needCheckThread,
    bool releaseAfterGet)
{
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (releaseAfterGet) {
            return std::move(cacheCompletedSurface_);
        }
        if (!needCheckThread || completedSurfaceThreadIndex_ == threadIndex || !cacheCompletedSurface_) {
            return cacheCompletedSurface_;
        }
    }

    // freeze cache scene
    ClearCacheSurfaceInThread();
    return nullptr;
}


void RSSurfaceRenderNodeDrawable::ClearCacheSurfaceInThread()
{
    if (clearCacheSurfaceFunc_) {
        clearCacheSurfaceFunc_(std::move(cacheSurface_), std::move(cacheCompletedSurface_), cacheSurfaceThreadIndex_,
            completedSurfaceThreadIndex_);
    }
    ClearCacheSurface();
}

bool RSSurfaceRenderNodeDrawable::NeedInitCacheCompletedSurface()
{
    int width = 0;
    int height = 0;
    if (auto& params = GetRenderNode()->GetRenderParams()) {
        auto size = params->GetCacheSize();
        width =  size.x_;
        height = size.y_;
    }
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheCompletedSurface_ == nullptr) {
        return true;
    }
    auto cacheCanvas = cacheCompletedSurface_->GetCanvas();
    if (cacheCanvas == nullptr) {
        return true;
    }
    return cacheCanvas->GetWidth() != width || cacheCanvas->GetHeight() != height;
}

std::shared_ptr<Drawing::Image> RSSurfaceRenderNodeDrawable::GetCompletedImage(
    RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst)
{
    if (isUIFirst) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (!cacheCompletedBackendTexture_.IsValid()) {
            RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage invalid grBackendTexture_");
            return nullptr;
        }
#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            if (!cacheCompletedSurface_ || !cacheCompletedCleanupHelper_) {
                RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage !cacheCompletedSurface_ %p || !cacheCompletedCleanupHelper_ %p",cacheCompletedSurface_.get(), cacheCompletedSurface_.get());
                return nullptr;
            }
        }
#endif
        auto image = std::make_shared<Drawing::Image>();
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
        Drawing::BitmapFormat info = Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888,
            Drawing::ALPHATYPE_PREMUL };
#ifdef RS_ENABLE_GL
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
            image->BuildFromTexture(*canvas.GetGPUContext(), cacheCompletedBackendTexture_.GetTextureInfo(),
                origin, info, nullptr);
        }
#endif

#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            //image = cacheSurface_->GetImageSnapshot();
            image->BuildFromTexture(*canvas.GetGPUContext(), cacheCompletedBackendTexture_.GetTextureInfo(),
                origin, info, nullptr,
                NativeBufferUtils::DeleteVkImage, cacheCompletedCleanupHelper_->Ref());
        }
#endif
        return image;
#endif
    }

    if (!cacheCompletedSurface_) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage DrawCacheSurface invalid cacheCompletedSurface");
        return nullptr;
    }
    auto completeImage = cacheCompletedSurface_->GetImageSnapshot();
    if (!completeImage) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage DrawCacheSurface Get complete image failed");
        return nullptr;
    }
    if (threadIndex == completedSurfaceThreadIndex_) {
        return completeImage;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = completeImage->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetCompletedImage DrawCacheSurface get backendTexture failed");
        return nullptr;
    }
    auto cacheImage = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat info =
        Drawing::BitmapFormat{ completeImage->GetColorType(), completeImage->GetAlphaType() };
    bool ret = cacheImage->BuildFromTexture(*canvas.GetGPUContext(), backendTexture.GetTextureInfo(),
        origin, info, nullptr);
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
    RS_TRACE_NAME("DrawCacheSurface");
    if (ROSEN_EQ(boundsWidth_, 0.f) || ROSEN_EQ(boundsHeight_, 0.f)) {
        RS_TRACE_NAME_FMT("return %d", __LINE__);
        RS_LOGE("RSSurfaceRenderNodeDrawable::DrawCacheSurface return %d", __LINE__);
        return false;
    }

    auto cacheImage = GetCompletedImage(canvas, threadIndex, isUIFirst);
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheImage, "cacheImage");
    if (cacheImage == nullptr) {
        RS_TRACE_NAME_FMT("return %d", __LINE__);
        RS_LOGE("RSSurfaceRenderNodeDrawable::DrawCacheSurface return %d", __LINE__);
        return false;
    }
    float scaleX = boundSize.x_ / static_cast<float>(cacheImage->GetWidth());
    float scaleY = boundSize.y_ / static_cast<float>(cacheImage->GetHeight());
    canvas.Save();
    canvas.Scale(scaleX, scaleY);
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (cacheImage->IsTextureBacked()) {
            RS_LOGI("RSSurfaceRenderNodeDrawable::DrawCacheSurface convert cacheImage from texture to raster image");
            cacheImage = cacheImage->MakeRasterImage();
        }
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    canvas.DrawImage(*cacheImage, 0.0, 0.0, samplingOptions);
    canvas.DetachBrush();
    canvas.Restore();
    return true;
}

void RSSurfaceRenderNodeDrawable::InitCacheSurface(Drawing::GPUContext* gpuContext, ClearCacheSurfaceFunc func, uint32_t threadIndex)
{
    if (func) {
        cacheSurfaceThreadIndex_ = threadIndex;
        if (!clearCacheSurfaceFunc_) {
            clearCacheSurfaceFunc_ = func;
        }
        if (cacheSurface_) {
            func(std::move(cacheSurface_), nullptr,
                cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
            std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
            cacheSurface_ = nullptr;
        }
    } else {
        cacheSurface_ = nullptr;
    }

    float width = 0.0f, height = 0.0f;
    if (auto& params = GetRenderNode()->GetRenderParams()) {
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
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        cacheSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext, true, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        cacheBackendTexture_ = MakeBackendTexture(width, height);
        auto vkTextureInfo = cacheBackendTexture_.GetTextureInfo().GetVKTextureInfo();
        if (!cacheBackendTexture_.IsValid() || !vkTextureInfo) {
            if (func) {
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
            1, Drawing::ColorType::COLORTYPE_RGBA_8888, nullptr,
            NativeBufferUtils::DeleteVkImage, cacheCleanupHelper_);
    }
#endif
#else
    cacheSurface_ = Drawing::Surface::MakeRasterN32Premul(width, height);
#endif
}
bool RSSurfaceRenderNodeDrawable::HasCachedTexture() const
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    return isTextureValid_;
#else
    return true;
#endif
}

bool RSSurfaceRenderNodeDrawable::NeedInitCacheSurface()
{
    int width = 0;
    int height = 0;

    if (auto& params = GetRenderNode()->GetRenderParams()) {
        auto size = params->GetCacheSize();
        width =  size.x_;
        height = size.y_;
    }

    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
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
    RS_TRACE_NAME("RSRenderNodeDrawable::UpdateBackendTexture()");
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheSurface_ == nullptr) {
        return;
    }
    cacheBackendTexture_ = cacheSurface_->GetBackendTexture();
}
#endif

bool RSSurfaceRenderNodeDrawable::IsCacheSurfaceValid() const
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    return  (cacheCompletedSurface_ != nullptr);
}

void RSSurfaceRenderNodeDrawable::UpdateCompletedCacheSurface()
{
    RS_TRACE_NAME("RSRenderNodeDrawable::UpdateCompletedCacheSurface()");
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    std::swap(cacheSurface_, cacheCompletedSurface_);
    std::swap(cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
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
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    isTextureValid_ = isValid;
#endif
}
void RSSurfaceRenderNodeDrawable::ClearCacheSurface(bool isClearCompletedCacheSurface)
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    cacheSurface_ = nullptr;
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        cacheCleanupHelper_ = nullptr;
    }
#endif
    if (isClearCompletedCacheSurface) {
        cacheCompletedSurface_ = nullptr;
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            cacheCompletedCleanupHelper_ = nullptr;
        }
#endif
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
        isTextureValid_ = false;
#endif
    }
}

void RSSurfaceRenderNodeDrawable::SubDraw(Drawing::Canvas& canvas)
{
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto surfaceNode = std::static_pointer_cast<RSSurfaceRenderNode>(nodeSp);
    RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::SubDraw [%s]", surfaceNode->GetName().c_str());

    auto rscanvas = reinterpret_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::SubDraw, rscanvas us nullptr");
        return;
    }
    auto& renderParams = renderNode_->GetUifirstRenderParams();
    Drawing::Rect bounds = renderParams ? renderParams->GetBounds() : Drawing::Rect(0, 0, 0, 0);
    RSRenderNodeDrawable::DrawUifirstContentChildren(*rscanvas, bounds);
}

bool RSSurfaceRenderNodeDrawable::DrawUIFirstCache(RSPaintFilterCanvas& rscanvas)
{
    auto& params = GetRenderNode()->GetRenderParams();
    if (!params) {
        RS_LOGE("RSUniRenderUtil::HandleSubThreadNodeDrawable params is nullptr");
        return false;
    }

    if (!HasCachedTexture()) {
        RS_TRACE_NAME_FMT("HandleSubThreadNode wait %" PRIu64 "", params->GetId());
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        RSSubThreadManager::Instance()->WaitNodeTask(params->GetId());
        UpdateCompletedCacheSurface();
#endif
    }
    return DrawCacheSurface(rscanvas, params->GetCacheSize(), UNI_MAIN_THREAD_INDEX, true);
}
#endif //RS_PARALLEL
} // namespace OHOS::Rosen

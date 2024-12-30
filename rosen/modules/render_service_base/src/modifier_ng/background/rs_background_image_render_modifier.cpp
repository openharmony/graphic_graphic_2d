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

#include "modifier_ng/background/rs_background_image_render_modifier.h"

#ifdef ROSEN_OHOS
#include "common/rs_common_tools.h"
#endif
#include "drawable/rs_property_drawable_utils.h"
#ifdef ROSEN_OHOS
#include "native_buffer_inner.h"
#include "native_window.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#endif

#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_task_dispatcher.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "include/gpu/GrBackendSemaphore.h"
#endif

namespace OHOS::Rosen::ModifierNG {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
constexpr uint8_t ASTC_HEADER_SIZE = 16;
#endif

const RSBackgroundImageRenderModifier::LegacyPropertyApplierMap
    RSBackgroundImageRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::BG_IMAGE,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSImage>, &RSProperties::SetBgImage> },
        { RSPropertyType::BG_IMAGE_INNER_RECT,
            RSRenderModifier::PropertyApplyHelperAdd<Vector4f, &RSProperties::SetBgImageInnerRect,
                &RSProperties::GetBgImageInnerRect> },
        { RSPropertyType::BG_IMAGE_RECT, RSRenderModifier::PropertyApplyHelperAdd<Vector4f,
                                             &RSProperties::SetBgImageDstRect, &RSProperties::GetBgImageDstRect> },
    };

void RSBackgroundImageRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetBgImage({});
    properties.SetBgImageDstRect({});
    properties.SetBgImageDstRect({});
}

RSBackgroundImageRenderModifier::~RSBackgroundImageRenderModifier()
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    ReleaseNativeWindowBuffer();
#endif
}

bool RSBackgroundImageRenderModifier::OnApply(RSModifierContext& context)
{
    if (!HasProperty(RSPropertyType::BG_IMAGE)) {
        return false;
    }
    stagingBgImage_ = Getter<std::shared_ptr<RSImage>>(RSPropertyType::BG_IMAGE);
    if (!stagingBgImage_) {
        return false;
    }

    stagingBoundsRect_ = RSPropertyDrawableUtils::Rect2DrawingRect(context.GetBoundsRect());
    auto innerRect = Getter<Vector4f>(RSPropertyType::BG_IMAGE_INNER_RECT, Vector4f(0.f));
    auto dstRect = Getter<Vector4f>(RSPropertyType::BG_IMAGE_RECT, Vector4f(0.f));
    stagingBgImage_->SetDstRect(RectF(dstRect));
    stagingBgImage_->SetInnerRect(std::make_optional<Drawing::RectI>(
        innerRect.x_, innerRect.y_, innerRect.x_ + innerRect.z_, innerRect.y_ + innerRect.w_));
    return true;
}

void RSBackgroundImageRenderModifier::OnSync()
{
    renderBgImage_ = std::move(stagingBgImage_);
    renderBoundsRect_ = stagingBoundsRect_;
}

void RSBackgroundImageRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (!renderBgImage_) {
        return;
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (renderBgImage_->GetPixelMap() && !renderBgImage_->GetPixelMap()->IsAstc() &&
        renderBgImage_->GetPixelMap()->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        if (!renderBgImage_->GetPixelMap()->GetFd()) {
            return;
        }
        auto dmaImage = MakeFromTextureForVK(static_cast<Drawing::Canvas&>(canvas),
            reinterpret_cast<SurfaceBuffer*>(renderBgImage_->GetPixelMap()->GetFd()));
        renderBgImage_->SetDmaImage(dmaImage);
    }
    if (renderBgImage_->GetPixelMap() && renderBgImage_->GetPixelMap()->IsAstc()) {
        SetCompressedDataForASTC();
    }
#endif
    renderBgImage_->CanvasDrawImage(
        static_cast<Drawing::Canvas&>(canvas), renderBoundsRect_, Drawing::SamplingOptions(), true);
    canvas.DetachBrush();
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
Drawing::ColorType RSBackgroundImageRenderModifier::GetColorTypeFromVKFormat(VkFormat vkFormat)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return Drawing::COLORTYPE_RGBA_8888;
    }
    switch (vkFormat) {
        case VK_FORMAT_R8G8B8A8_UNORM:
            return Drawing::COLORTYPE_RGBA_8888;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return Drawing::COLORTYPE_RGBA_F16;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return Drawing::COLORTYPE_RGB_565;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return Drawing::COLORTYPE_RGBA_1010102;
        default:
            return Drawing::COLORTYPE_RGBA_8888;
    }
}

void RSBackgroundImageRenderModifier::ReleaseNativeWindowBuffer()
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        if (nativeWindowBuffer_ == nullptr && cleanUpHelper_ == nullptr) {
            return;
        }
        RSTaskDispatcher::GetInstance().PostTask(
            tid_, [nativeWindowBuffer = nativeWindowBuffer_, cleanUpHelper = cleanUpHelper_]() {
                if (nativeWindowBuffer != nullptr) {
                    DestroyNativeWindowBuffer(nativeWindowBuffer);
                }
                if (cleanUpHelper != nullptr) {
                    NativeBufferUtils::DeleteVkImage(cleanUpHelper);
                }
            });
        nativeWindowBuffer_ = nullptr;
        cleanUpHelper_ = nullptr;
    }
}

std::shared_ptr<Drawing::Image> RSBackgroundImageRenderModifier::MakeFromTextureForVK(
    Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    if (surfaceBuffer == nullptr || surfaceBuffer->GetBufferHandle() == nullptr) {
        RS_LOGE("MakeFromTextureForVK surfaceBuffer is nullptr or buffer handle is nullptr");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = renderBgImage_->GetPixelMap();
    if (pixelMapId_ != pixelMap->GetUniqueId() || !backendTexture_.IsValid()) {
        backendTexture_ = {};
        ReleaseNativeWindowBuffer();
        sptr<SurfaceBuffer> sfBuffer(surfaceBuffer);
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&sfBuffer);
        if (!nativeWindowBuffer_) {
            RS_LOGE("MakeFromTextureForVK create native window buffer fail");
            return nullptr;
        }
        backendTexture_ = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
            nativeWindowBuffer_, surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), false);
        if (backendTexture_.IsValid()) {
            auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
            cleanUpHelper_ = new NativeBufferUtils::VulkanCleanupHelper(
                RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
        } else {
            return nullptr;
        }
        pixelMapId_ = pixelMap->GetUniqueId();
        tid_ = gettid();
    }

    if (canvas.GetGPUContext() == nullptr) {
        RS_LOGE("RSBackgroundImageRenderModifier::MakeFromTextureForVK canvas.GetGPUContext is nullptr");
        ReleaseNativeWindowBuffer();
        return nullptr;
    }
    std::shared_ptr<Drawing::Image> dmaImage = std::make_shared<Drawing::Image>();
    auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
    Drawing::ColorType colorType = GetColorTypeFromVKFormat(vkTextureInfo->format);
    Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
    if (!dmaImage->BuildFromTexture(*canvas.GetGPUContext(), backendTexture_.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr, NativeBufferUtils::DeleteVkImage,
        cleanUpHelper_->Ref())) {
        RS_LOGE("RSBackgroundImageRenderModifier build image failed");
        return nullptr;
    }
    return dmaImage;
}

void RSBackgroundImageRenderModifier::SetCompressedDataForASTC()
{
    std::shared_ptr<Media::PixelMap> pixelMap = renderBgImage_->GetPixelMap();
    if (!pixelMap || !pixelMap->GetFd()) {
        RS_LOGE("SetCompressedDataForASTC fail, data is null");
        return;
    }
    std::shared_ptr<Drawing::Data> fileData = std::make_shared<Drawing::Data>();
    // After RS is switched to Vulkan, the judgment of GpuApiType can be deleted.
    if (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC &&
        (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR)) {
        if (pixelMapId_ != pixelMap->GetUniqueId()) {
            if (nativeWindowBuffer_) {
                DestroyNativeWindowBuffer(nativeWindowBuffer_);
            }
            sptr<SurfaceBuffer> surfaceBuf(reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd()));
            nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuf);
            pixelMapId_ = pixelMap->GetUniqueId();
        }
        OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer_);
        if (nativeBuffer == nullptr || !fileData->BuildFromOHNativeBuffer(nativeBuffer, pixelMap->GetCapacity())) {
            RS_LOGE("SetCompressedDataForASTC data BuildFromOHNativeBuffer fail");
            return;
        }
    } else {
        const void* data = pixelMap->GetPixels();
        if (pixelMap->GetCapacity() > ASTC_HEADER_SIZE &&
            (data == nullptr || !fileData->BuildWithoutCopy(reinterpret_cast<const void*>(
                                                                reinterpret_cast<const char*>(data) + ASTC_HEADER_SIZE),
                                    pixelMap->GetCapacity() - ASTC_HEADER_SIZE))) {
            RS_LOGE("SetCompressedDataForASTC data BuildWithoutCopy fail");
            return;
        }
    }
    renderBgImage_->SetCompressData(fileData);
}
#endif
} // namespace OHOS::Rosen::ModifierNG

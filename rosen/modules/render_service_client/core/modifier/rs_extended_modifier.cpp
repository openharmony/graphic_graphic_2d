/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "modifier/rs_extended_modifier.h"

#include <memory>

#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_log.h"
#if defined(RS_ENABLE_VK)
#include "common/rs_optional_trace.h"
#include "image/image.h"
#include "native_window.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "recording/cmd_list_helper.h"
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Rosen {

#if defined(RS_ENABLE_VK)
static sptr<SurfaceBuffer> DmaMemAlloc(const int32_t& width, const int32_t& height,
    const std::unique_ptr<Media::PixelMap>& pixelMap)
{
#if defined(_WIN32) || defined(_APPLE) || defined(A_PLATFORM) || defined(IOS_PLATFORM)
    RS_LOGE("DmaMemAlloc::Unsupport dma mem alloc");
    return nullptr;
#else
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8, // set 0x8 as default value to alloc SurfaceBufferImpl
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888, // PixelFormat
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = surfaceBuffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        RS_LOGE("DmaMemAlloc::SurfaceBuffer Alloc failed, %{public}s", GSErrorStr(ret).c_str());
        return nullptr;
    }
    void* nativeBuffer = surfaceBuffer.GetRefPtr();
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(nativeBuffer);
    ref->IncStrongRef(ref);
    int32_t bufferSize = pixelMap->GetByteCount();
    pixelMap->SetPixelsAddr(surfaceBuffer->GetVirAddr(), nativeBuffer, bufferSize,
        Media::AllocatorType::DMA_ALLOC, nullptr);
    return surfaceBuffer;
#endif
}

static std::shared_ptr<Drawing::Surface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelMap, int32_t& width,
    int32_t& height)
{
    sptr<SurfaceBuffer> surfaceBufferTmp = DmaMemAlloc(width, height, pixelMap);
    if (!surfaceBufferTmp) {
        RS_LOGE("CreateSurface::DmaMemAlloc fail.");
        return nullptr;
    }

    OHNativeWindowBuffer *nativeWindowBufferTmp = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBufferTmp);
    if (!nativeWindowBufferTmp) {
        RS_LOGE("CreateSurface::CreateNativeWindowBufferFromSurfaceBuffer fail");
        return nullptr;
    }

    Drawing::BackendTexture backendTextureTmp = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
        nativeWindowBufferTmp, surfaceBufferTmp->GetWidth(), surfaceBufferTmp->GetHeight());
    if (!backendTextureTmp.IsValid()) {
        DestroyNativeWindowBuffer(nativeWindowBufferTmp);
        RS_LOGE("CreateSurface::MakeBackendTextureFromNativeBuffer fail");
        return nullptr;
    }

    auto vkTextureInfo = backendTextureTmp.GetTextureInfo().GetVKTextureInfo();
    vkTextureInfo->imageUsageFlags = vkTextureInfo->imageUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    auto cleanUpHelper = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
        vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    if (!cleanUpHelper) {
        DestroyNativeWindowBuffer(nativeWindowBufferTmp);
        RS_LOGE("CreateSurface::construct VulkanCleanupHelper fail.");
        return nullptr;
    }
    auto gpuContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
    if (!gpuContext) {
        RS_LOGE("CreateSurface::CreateDrawingContext fail.");
        delete cleanUpHelper;
        cleanUpHelper = nullptr;
        DestroyNativeWindowBuffer(nativeWindowBufferTmp);
        return nullptr;
    }
    std::shared_ptr<Drawing::Surface> drawingSurface = Drawing::Surface::MakeFromBackendTexture(gpuContext.get(),
        backendTextureTmp.GetTextureInfo(), Drawing::TextureOrigin::TOP_LEFT, 1,
        Drawing::ColorType::COLORTYPE_RGBA_8888, nullptr, RSExtendedModifierHelper::DeleteVkImage, cleanUpHelper);
    if (!drawingSurface) {
        delete cleanUpHelper;
        cleanUpHelper = nullptr;
        RS_LOGE("CreateSurface::MakeFromBackendTexture fail.");
    }
    DestroyNativeWindowBuffer(nativeWindowBufferTmp);
    return drawingSurface;
}

static std::shared_ptr<Drawing::DrawCmdList> MakePiexlMapDrawCmdList(std::shared_ptr<Drawing::DrawCmdList>& recording,
    RSDrawingContext& ctx)
{
    static constexpr size_t LONGTEXT_WATER_LINE = 20;
    static constexpr float OFFSET_COORDANATE = 0.5f;
    std::shared_ptr<Drawing::DrawCmdList> pixelMapDrawCmdList = nullptr;
    if (recording->GetOpItemSize() > LONGTEXT_WATER_LINE && recording->CountTextBlobNum() > LONGTEXT_WATER_LINE) {
        RS_TRACE_NAME("enter long text GPU render.");
        int32_t width = static_cast<int32_t>(ctx.width + OFFSET_COORDANATE);
        int32_t height = static_cast<int32_t>(ctx.height + OFFSET_COORDANATE);
        Media::InitializationOptions options;
        options.size.width = width;
        options.size.height = height;
        options.srcPixelFormat = Media::PixelFormat::RGBA_8888;
        options.pixelFormat = Media::PixelFormat::RGBA_8888;
        auto pixelMap = Media::PixelMap::Create(options);
        if (!pixelMap) {
            RS_LOGE("MakePiexlMapDrawCmdList::PixelMap Create fail.");
            return nullptr;
        }
        auto drawingSurface = CreateSurface(pixelMap, width, height);
        if (!drawingSurface) {
            RS_LOGE("MakePiexlMapDrawCmdList::drawingSurface Create fail.");
            return nullptr;
        }
        auto tmpCanvas = drawingSurface->GetCanvas();
        recording->Playback(*tmpCanvas, nullptr);
        drawingSurface->FlushAndSubmit();
        Drawing::SamplingOptions sampling(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
        pixelMapDrawCmdList =
            std::make_shared<Drawing::DrawCmdList>(width, height, Drawing::DrawCmdList::UnmarshalMode::IMMEDIATE);
        Drawing::AdaptiveImageInfo adaptiveImageInfo = { 0, 0, {}, 1.0, 0, width, height };
        auto object = std::make_shared<RSExtendImageObject>(std::move(pixelMap), adaptiveImageInfo);
        auto objectHandle = Drawing::CmdListHelper::AddImageObjectToCmdList(*pixelMapDrawCmdList, object);
        Drawing::PaintHandle paintHandle;
        paintHandle.isAntiAlias = true;
        paintHandle.style = Drawing::Paint::PaintStyle::PAINT_FILL;
        pixelMapDrawCmdList->AddDrawOp<Drawing::DrawPixelMapWithParmOpItem::ConstructorHandle>(objectHandle, sampling,
            paintHandle);
    }
    return pixelMapDrawCmdList;
}

void RSExtendedModifierHelper::DeleteVkImage(void* context)
{
    NativeBufferUtils::VulkanCleanupHelper *cleanUpHelper =
        static_cast<NativeBufferUtils::VulkanCleanupHelper*>(context);
    if (cleanUpHelper != nullptr) {
        cleanUpHelper->UnRef();
    }
}
#endif

RSDrawingContext RSExtendedModifierHelper::CreateDrawingContext(NodeId nodeId)
{
    auto node = RSNodeMap::Instance().GetNode<RSCanvasNode>(nodeId);
    if (!node) {
        return { nullptr };
    }
    auto recordingCanvas = new ExtendRecordingCanvas(node->GetPaintWidth(), node->GetPaintHeight());
    recordingCanvas->SetIsCustomTextType(node->GetIsCustomTextType());
    recordingCanvas->SetIsCustomTypeface(node->GetIsCustomTypeface());
    return { recordingCanvas, node->GetPaintWidth(), node->GetPaintHeight() };
}

std::shared_ptr<RSRenderModifier> RSExtendedModifierHelper::CreateRenderModifier(
    RSDrawingContext& ctx, PropertyId id, RSModifierType type)
{
    auto renderProperty = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(
        RSExtendedModifierHelper::FinishDrawing(ctx), id);
    auto renderModifier =  std::make_shared<RSDrawCmdListRenderModifier>(renderProperty);
    renderModifier->SetType(type);
    return renderModifier;
}

std::shared_ptr<Drawing::DrawCmdList> RSExtendedModifierHelper::FinishDrawing(RSDrawingContext& ctx)
{
    auto recordingCanvas = static_cast<ExtendRecordingCanvas*>(ctx.canvas);
    if (!recordingCanvas) {
        RS_LOGW("RSExtendedModifierHelper::FinishDrawing recordingCanvas is nullptr");
        return nullptr;
    }
    auto recording = recordingCanvas->GetDrawCmdList();
    if (!recording) {
        RS_LOGW("RSExtendedModifierHelper::FinishDrawing recording is nullptr");
        delete ctx.canvas;
        ctx.canvas = nullptr;
        return nullptr;
    }
#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN && RSSystemProperties::GetTextBlobAsPixelMap()) {
        auto pixelMapDrawCmdList = MakePiexlMapDrawCmdList(recording, ctx);
        if (pixelMapDrawCmdList) {
            delete ctx.canvas;
            ctx.canvas = nullptr;
            return pixelMapDrawCmdList;
        }
    }
#endif
    if (RSSystemProperties::GetDrawTextAsBitmap()) {
        // replace drawOpItem with cached one (generated by CPU)
        recording->GenerateCache();
    }
    delete ctx.canvas;
    ctx.canvas = nullptr;
    return recording;
}
} // namespace Rosen
} // namespace OHOS

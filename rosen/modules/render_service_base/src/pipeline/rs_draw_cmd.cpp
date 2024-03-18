/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "common/rs_common_tools.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_log.h"
#include "render/rs_pixel_map_util.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "utils/system_properties.h"
#include "pipeline/rs_task_dispatcher.h"
#include "platform/common/rs_system_properties.h"
#ifdef ROSEN_OHOS
#include "native_window.h"
#endif
#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSemaphore.h"
#include "native_buffer_inner.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#include "include/gpu/GrDirectContext.h"

namespace OHOS {
namespace Rosen {
constexpr int32_t CORNER_SIZE = 4;

#ifdef RS_ENABLE_VK
Drawing::ColorType GetColorTypeFromVKFormat(VkFormat vkFormat)
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
        default:
            return Drawing::COLORTYPE_RGBA_8888;
    }
}
#endif

RSExtendImageObject::RSExtendImageObject(const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Drawing::Data>& data, const Drawing::AdaptiveImageInfo& imageInfo)
{
    rsImage_ = std::make_shared<RSImage>();
    rsImage_->SetImage(image);
    rsImage_->SetCompressData(data, imageInfo.uniqueId, imageInfo.width, imageInfo.height);
    rsImage_->SetImageFit(imageInfo.fitNum);
    rsImage_->SetImageRepeat(imageInfo.repeatNum);
    std::vector<Drawing::Point> radiusValue(imageInfo.radius, imageInfo.radius + CORNER_SIZE);
    rsImage_->SetRadius(radiusValue);
    rsImage_->SetScale(imageInfo.scale);
}

RSExtendImageObject::RSExtendImageObject(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::AdaptiveImageInfo& imageInfo)
{
    if (pixelMap) {
        if (RSSystemProperties::GetDumpUIPixelmapEnabled()) {
            CommonTools::SavePixelmapToFile(pixelMap, "/data/storage/el1/base/imageObject_");
        }
        rsImage_ = std::make_shared<RSImage>();
        rsImage_->SetPixelMap(pixelMap);
        rsImage_->SetImageFit(imageInfo.fitNum);
        rsImage_->SetImageRepeat(imageInfo.repeatNum);
        std::vector<Drawing::Point> radiusValue(imageInfo.radius, imageInfo.radius + CORNER_SIZE);
        rsImage_->SetRadius(radiusValue);
        rsImage_->SetScale(imageInfo.scale);
    }
}

void RSExtendImageObject::SetNodeId(NodeId id)
{
    if (rsImage_) {
        rsImage_->UpdateNodeIdToPicture(id);
    }
}

void RSExtendImageObject::Playback(Drawing::Canvas& canvas, const Drawing::Rect& rect,
    const Drawing::SamplingOptions& sampling, bool isBackground)
{
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::shared_ptr<Media::PixelMap> pixelmap = rsImage_->GetPixelMap();
    if (canvas.GetRecordingCanvas()) {
        image_ = RSPixelMapUtil::ExtractDrawingImage(pixelmap);
        if (image_) {
            rsImage_->SetDmaImage(image_);
        }
        rsImage_->CanvasDrawImage(canvas, rect, sampling, isBackground);
        return;
    }
    if (pixelmap != nullptr && pixelmap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
#if defined(RS_ENABLE_GL)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
            if (GetDrawingImageFromSurfaceBuffer(canvas, reinterpret_cast<SurfaceBuffer*>(pixelmap->GetFd()))) {
                rsImage_->SetDmaImage(image_);
            }
        }
#endif
#if defined(RS_ENABLE_VK)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            if (MakeFromTextureForVK(canvas, reinterpret_cast<SurfaceBuffer*>(pixelmap->GetFd()))) {
                rsImage_->SetDmaImage(image_);
            }
        }
#endif
    } else {
        if (pixelmap && pixelmap->IsAstc()) {
            const void* data = pixelmap->GetWritablePixels();
            std::shared_ptr<Drawing::Data> fileData = std::make_shared<Drawing::Data>();
            const int seekSize = 16;
            if (pixelmap->GetCapacity() > seekSize) {
                fileData->BuildWithoutCopy((void*)((char*) data + seekSize), pixelmap->GetCapacity() - seekSize);
            }
            rsImage_->SetCompressData(fileData);
        }
    }
#endif
    rsImage_->CanvasDrawImage(canvas, rect, sampling, isBackground);
}

bool RSExtendImageObject::Marshalling(Parcel &parcel) const
{
    bool ret = RSMarshallingHelper::Marshalling(parcel, rsImage_);
    return ret;
}

RSExtendImageObject *RSExtendImageObject::Unmarshalling(Parcel &parcel)
{
    auto object = new RSExtendImageObject();
    bool ret = RSMarshallingHelper::Unmarshalling(parcel, object->rsImage_);
    if (!ret) {
        return nullptr;
    }
    return object;
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
bool RSExtendImageObject::GetDrawingImageFromSurfaceBuffer(Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return false;
    }
    if (surfaceBuffer == nullptr) {
        RS_LOGE("GetDrawingImageFromSurfaceBuffer surfaceBuffer is nullptr");
        return false;
    }
    if (nativeWindowBuffer_ == nullptr) {
        sptr<SurfaceBuffer> sfBuffer(surfaceBuffer);
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&sfBuffer);
        if (!nativeWindowBuffer_) {
            RS_LOGE("GetDrawingImageFromSurfaceBuffer create native window buffer fail");
            return false;
        }
    }
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED,
        EGL_TRUE,
        EGL_NONE,
    };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
        if (eglImage_ == EGL_NO_IMAGE_KHR) {
            RS_LOGE("%{public}s create egl image fail %{public}d", __func__, eglGetError());
            return false;
        }
        tid_ = gettid();
    }

    // Create texture object
    if (texId_ == 0U) {
        glGenTextures(1, &texId_);
        glBindTexture(GL_TEXTURE_2D, texId_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, static_cast<GLeglImageOES>(eglImage_));
    }

    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(surfaceBuffer->GetWidth());
    externalTextureInfo.SetHeight(surfaceBuffer->GetHeight());
    externalTextureInfo.SetIsMipMapped(false);
    externalTextureInfo.SetTarget(GL_TEXTURE_2D);
    externalTextureInfo.SetID(texId_);
    externalTextureInfo.SetFormat(GL_RGBA8_OES);

    Drawing::BitmapFormat bitmapFormat = {
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    if (!canvas.GetGPUContext()) {
        RS_LOGE("GetDrawingImageFromSurfaceBuffer gpu context is nullptr");
        return false;
    }
    if (!image_) {
        image_ = std::make_shared<Drawing::Image>();
    }
#ifndef ROSEN_EMULATOR
    auto surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
#else
    auto surfaceOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
#endif
    if (!image_->BuildFromTexture(*(canvas.GetGPUContext()), externalTextureInfo,
        surfaceOrigin, bitmapFormat,
        std::make_shared<Drawing::ColorSpace>(Drawing::ColorSpace::ColorSpaceType::SRGB))) {
        RS_LOGE("BuildFromTexture failed");
        return false;
    }
    return true;
}
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
bool RSExtendImageObject::MakeFromTextureForVK(Drawing::Canvas& canvas, SurfaceBuffer *surfaceBuffer)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return false;
    }
    if (surfaceBuffer == nullptr) {
        RS_LOGE("MakeFromTextureForVK surfaceBuffer is nullptr");
        return false;
    }
    if (nativeWindowBuffer_ == nullptr) {
        sptr<SurfaceBuffer> sfBuffer(surfaceBuffer);
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&sfBuffer);
        if (!nativeWindowBuffer_) {
            RS_LOGE("MakeFromTextureForVK create native window buffer fail");
            return false;
        }
    }
    if (!backendTexture_.IsValid()) {
        backendTexture_ = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nativeWindowBuffer_,
            surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight());
        if (backendTexture_.IsValid()) {
            auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
            cleanUpHelper_ = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
                vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
        } else {
            return false;
        }
        tid_ = gettid();
    }
    if (!image_) {
        image_ = std::make_shared<Drawing::Image>();
    }
    auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
    Drawing::ColorType colorType = GetColorTypeFromVKFormat(vkTextureInfo->format);
    Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
    if (!image_->BuildFromTexture(*canvas.GetGPUContext(), backendTexture_.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr,
        NativeBufferUtils::DeleteVkImage,
        cleanUpHelper_->Ref())) {
        RS_LOGE("MakeFromTextureForVK build image failed");
        return false;
    }
    return true;
}
#endif

RSExtendImageObject::~RSExtendImageObject()
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        RSTaskDispatcher::GetInstance().PostTask(tid_, [texId = texId_,
                                                        nativeWindowBuffer = nativeWindowBuffer_,
                                                        eglImage = eglImage_]() {
            if (texId != 0U) {
                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1, &texId);
            }
            if (nativeWindowBuffer != nullptr) {
                DestroyNativeWindowBuffer(nativeWindowBuffer);
            }
            if (eglImage != EGL_NO_IMAGE_KHR) {
                auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                eglDestroyImageKHR(disp, eglImage);
            }
        });
    }
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        RSTaskDispatcher::GetInstance().PostTask(tid_, [nativeWindowBuffer = nativeWindowBuffer_,
            cleanupHelper = cleanUpHelper_]() {
            if (nativeWindowBuffer != nullptr) {
                DestroyNativeWindowBuffer(nativeWindowBuffer);
            }
            if (cleanupHelper != nullptr) {
                NativeBufferUtils::DeleteVkImage(cleanupHelper);
            }
        });
    }
#endif
}

RSExtendImageBaseObj::RSExtendImageBaseObj(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& src,
    const Drawing::Rect& dst)
{
    if (pixelMap) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetPixelMap(pixelMap);
        rsImage_->SetSrcRect(RectF(src.GetLeft(), src.GetTop(), src.GetWidth(), src.GetHeight()));
        rsImage_->SetDstRect(RectF(dst.GetLeft(), dst.GetTop(), dst.GetWidth(), dst.GetHeight()));
    }
}

void RSExtendImageBaseObj::Playback(Drawing::Canvas& canvas, const Drawing::Rect& rect,
    const Drawing::SamplingOptions& sampling)
{
    if (rsImage_) {
        rsImage_->DrawImage(canvas, sampling);
    }
}

void RSExtendImageBaseObj::SetNodeId(NodeId id)
{
    if (rsImage_) {
        rsImage_->UpdateNodeIdToPicture(id);
    }
}

bool RSExtendImageBaseObj::Marshalling(Parcel &parcel) const
{
    bool ret = RSMarshallingHelper::Marshalling(parcel, rsImage_);
    return ret;
}

RSExtendImageBaseObj *RSExtendImageBaseObj::Unmarshalling(Parcel &parcel)
{
    auto object = new RSExtendImageBaseObj();
    bool ret = RSMarshallingHelper::Unmarshalling(parcel, object->rsImage_);
    if (!ret) {
        return nullptr;
    }
    return object;
}

void RSExtendDrawFuncObj::Playback(Drawing::Canvas* canvas, const Drawing::Rect* rect)
{
    if (drawFunc_) {
        drawFunc_(canvas, rect);
    }
}

bool RSExtendDrawFuncObj::Marshalling(Parcel &parcel) const
{
    return false;
}

RSExtendDrawFuncObj *RSExtendDrawFuncObj::Unmarshalling(Parcel &parcel)
{
    return nullptr;
}

namespace Drawing {
/* DrawImageWithParmOpItem */
REGISTER_UNMARSHALLING_FUNC(
    DrawImageWithParm, DrawOpItem::IMAGE_WITH_PARM_OPITEM, DrawImageWithParmOpItem::Unmarshalling);

DrawImageWithParmOpItem::DrawImageWithParmOpItem(
    const DrawCmdList& cmdList, DrawImageWithParmOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, IMAGE_WITH_PARM_OPITEM), sampling_(handle->sampling)
{
    objectHandle_ = CmdListHelper::GetImageObjectFromCmdList(cmdList, handle->objectHandle);
}

DrawImageWithParmOpItem::DrawImageWithParmOpItem(const std::shared_ptr<Image>& image, const std::shared_ptr<Data>& data,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling, const Paint& paint)
    : DrawWithPaintOpItem(paint, IMAGE_WITH_PARM_OPITEM), sampling_(sampling)
{
    objectHandle_ = std::make_shared<RSExtendImageObject>(image, data, rsImageInfo);
}

std::shared_ptr<DrawOpItem> DrawImageWithParmOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageWithParmOpItem>(
        cmdList, static_cast<DrawImageWithParmOpItem::ConstructorHandle*>(handle));
}

void DrawImageWithParmOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto objectHandle = CmdListHelper::AddImageObjectToCmdList(cmdList, objectHandle_);
    cmdList.AddOp<ConstructorHandle>(objectHandle, sampling_, paintHandle);
}

void DrawImageWithParmOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawImageWithParmOpItem objectHandle is nullptr!");
        return;
    }
    canvas->AttachPaint(paint_);
    objectHandle_->Playback(*canvas, *rect, sampling_, false);
}

void DrawImageWithParmOpItem::SetNodeId(NodeId id)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawImageWithParmOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->SetNodeId(id);
}

/* DrawPixelMapWithParmOpItem */
REGISTER_UNMARSHALLING_FUNC(
    DrawPixelMapWithParm, DrawOpItem::PIXELMAP_WITH_PARM_OPITEM, DrawPixelMapWithParmOpItem::Unmarshalling);

DrawPixelMapWithParmOpItem::DrawPixelMapWithParmOpItem(
    const DrawCmdList& cmdList, DrawPixelMapWithParmOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PIXELMAP_WITH_PARM_OPITEM), sampling_(handle->sampling)
{
    objectHandle_ = CmdListHelper::GetImageObjectFromCmdList(cmdList, handle->objectHandle);
}

DrawPixelMapWithParmOpItem::DrawPixelMapWithParmOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling, const Paint& paint)
    : DrawWithPaintOpItem(paint, PIXELMAP_WITH_PARM_OPITEM), sampling_(sampling)
{
    objectHandle_ = std::make_shared<RSExtendImageObject>(pixelMap, rsImageInfo);
}

std::shared_ptr<DrawOpItem> DrawPixelMapWithParmOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPixelMapWithParmOpItem>(
        cmdList, static_cast<DrawPixelMapWithParmOpItem::ConstructorHandle*>(handle));
}

void DrawPixelMapWithParmOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto objectHandle = CmdListHelper::AddImageObjectToCmdList(cmdList, objectHandle_);
    cmdList.AddOp<ConstructorHandle>(objectHandle, sampling_, paintHandle);
}

void DrawPixelMapWithParmOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapWithParmOpItem objectHandle is nullptr!");
        return;
    }
    canvas->AttachPaint(paint_);
    objectHandle_->Playback(*canvas, *rect, sampling_, false);
}

void DrawPixelMapWithParmOpItem::SetNodeId(NodeId id)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapWithParmOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->SetNodeId(id);
}

/* DrawPixelMapRectOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawPixelMapRect, DrawOpItem::PIXELMAP_RECT_OPITEM, DrawPixelMapRectOpItem::Unmarshalling);

DrawPixelMapRectOpItem::DrawPixelMapRectOpItem(
    const DrawCmdList& cmdList, DrawPixelMapRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PIXELMAP_RECT_OPITEM), sampling_(handle->sampling)
{
    objectHandle_ = CmdListHelper::GetImageBaseObjFromCmdList(cmdList, handle->objectHandle);
}

DrawPixelMapRectOpItem::DrawPixelMapRectOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap, const Rect& src,
    const Rect& dst, const SamplingOptions& sampling, const Paint& paint)
    : DrawWithPaintOpItem(paint, PIXELMAP_RECT_OPITEM), sampling_(sampling)
{
    objectHandle_ = std::make_shared<RSExtendImageBaseObj>(pixelMap, src, dst);
}

std::shared_ptr<DrawOpItem> DrawPixelMapRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPixelMapRectOpItem>(
        cmdList, static_cast<DrawPixelMapRectOpItem::ConstructorHandle*>(handle));
}

void DrawPixelMapRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto objectHandle = CmdListHelper::AddImageBaseObjToCmdList(cmdList, objectHandle_);
    cmdList.AddOp<ConstructorHandle>(objectHandle, sampling_, paintHandle);
}

void DrawPixelMapRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapRectOpItem objectHandle is nullptr!");
        return;
    }
    canvas->AttachPaint(paint_);
    objectHandle_->Playback(*canvas, *rect, sampling_);
}

void DrawPixelMapRectOpItem::SetNodeId(NodeId id)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapRectOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->SetNodeId(id);
}

/* DrawFuncOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawFunc, DrawOpItem::DRAW_FUNC_OPITEM, DrawFuncOpItem::Unmarshalling);

DrawFuncOpItem::DrawFuncOpItem(const DrawCmdList& cmdList, DrawFuncOpItem::ConstructorHandle* handle)
    : DrawOpItem(DRAW_FUNC_OPITEM)
{
    objectHandle_ = CmdListHelper::GetDrawFuncObjFromCmdList(cmdList, handle->funcObjectId);
}

DrawFuncOpItem::DrawFuncOpItem(RecordingCanvas::DrawFunc&& drawFunc) : DrawOpItem(DRAW_FUNC_OPITEM)
{
    objectHandle_ = std::make_shared<RSExtendDrawFuncObj>(std::move(drawFunc));
}

std::shared_ptr<DrawOpItem> DrawFuncOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawFuncOpItem>(cmdList, static_cast<DrawFuncOpItem::ConstructorHandle*>(handle));
}

void DrawFuncOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto objectHandle = CmdListHelper::AddDrawFuncObjToCmdList(cmdList, objectHandle_);
    cmdList.AddOp<ConstructorHandle>(objectHandle);
}

void DrawFuncOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_) {
        objectHandle_->Playback(canvas, rect);
    }
}

#ifdef ROSEN_OHOS
/* DrawSurfaceBufferOpItem */
REGISTER_UNMARSHALLING_FUNC(
    DrawSurfaceBuffer, DrawOpItem::SURFACEBUFFER_OPITEM, DrawSurfaceBufferOpItem::Unmarshalling);

DrawSurfaceBufferOpItem::DrawSurfaceBufferOpItem(const DrawCmdList& cmdList,
    DrawSurfaceBufferOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, SURFACEBUFFER_OPITEM),
      surfaceBufferInfo_(nullptr, handle->surfaceBufferInfo.offSetX_, handle->surfaceBufferInfo.offSetY_,
                         handle->surfaceBufferInfo.width_, handle->surfaceBufferInfo.height_)
{
    surfaceBufferInfo_.surfaceBuffer_ = CmdListHelper::GetSurfaceBufferFromCmdList(cmdList, handle->surfaceBufferId);
}

DrawSurfaceBufferOpItem::~DrawSurfaceBufferOpItem()
{
    Clear();
}

std::shared_ptr<DrawOpItem> DrawSurfaceBufferOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawSurfaceBufferOpItem>(cmdList,
        static_cast<DrawSurfaceBufferOpItem::ConstructorHandle*>(handle));
}

void DrawSurfaceBufferOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(
        CmdListHelper::AddSurfaceBufferToCmdList(cmdList, surfaceBufferInfo_.surfaceBuffer_),
        surfaceBufferInfo_.offSetX_, surfaceBufferInfo_.offSetY_,
        surfaceBufferInfo_.width_, surfaceBufferInfo_.height_, paintHandle);
}

void DrawSurfaceBufferOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (auto recordingCanvas = static_cast<ExtendRecordingCanvas*>(canvas->GetRecordingCanvas())) {
        recordingCanvas->DrawSurfaceBuffer(surfaceBufferInfo_);
        return;
    }
    Clear();
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (surfaceBufferInfo_.surfaceBuffer_ == nullptr) {
        LOGE("SurfaceBufferOpItem::Draw surfaceBuffer_ is nullptr");
        return;
    }
    nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&(surfaceBufferInfo_.surfaceBuffer_));
    if (!nativeWindowBuffer_) {
        LOGE("create nativeWindowBuffer_ fail.");
        return;
    }
#endif
    canvas->AttachPaint(paint_);
    Draw(canvas);
}

void DrawSurfaceBufferOpItem::Clear()
{
#ifdef RS_ENABLE_GL
    if (SystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (texId_ != 0U) {
            glDeleteTextures(1, &texId_);
        }
        if (eglImage_ != EGL_NO_IMAGE_KHR) {
            auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglDestroyImageKHR(disp, eglImage_);
        }
    }
#endif
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
#endif
}

void DrawSurfaceBufferOpItem::Draw(Canvas* canvas)
{
#ifdef RS_ENABLE_VK
    if (SystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        DrawWithVulkan(canvas);
        return;
    }
#endif
#ifdef RS_ENABLE_GL
    if (SystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        DrawWithGles(canvas);
        return;
    }
#endif
}

void DrawSurfaceBufferOpItem::DrawWithVulkan(Canvas* canvas)
{
#ifdef RS_ENABLE_VK
    auto backendTexture = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nativeWindowBuffer_,
        surfaceBufferInfo_.surfaceBuffer_->GetWidth(), surfaceBufferInfo_.surfaceBuffer_->GetHeight());
    if (!backendTexture.IsValid()) {
        LOGE("DrawSurfaceBufferOpItem::Draw backendTexture is not valid");
        return;
    }
    if (!canvas->GetGPUContext()) {
        LOGE("DrawSurfaceBufferOpItem::Draw gpu context is nullptr");
        return;
    }
    Drawing::BitmapFormat bitmapFormat = { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto image = std::make_shared<Drawing::Image>();
    auto vkTextureInfo = backendTexture.GetTextureInfo().GetVKTextureInfo();
    if (!vkTextureInfo || !image->BuildFromTexture(*canvas->GetGPUContext(), backendTexture.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr, NativeBufferUtils::DeleteVkImage,
        new NativeBufferUtils::VulkanCleanupHelper(
            RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory))) {
        LOGE("DrawSurfaceBufferOpItem::Draw image BuildFromTexture failed");
        return;
    }
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    canvas->DrawImageRect(*image, Rect{
        surfaceBufferInfo_.offSetX_, surfaceBufferInfo_.offSetY_,
        surfaceBufferInfo_.offSetX_ + surfaceBufferInfo_.width_,
        surfaceBufferInfo_.offSetY_ + surfaceBufferInfo_.height_},
        samplingOptions);
#endif
}

void DrawSurfaceBufferOpItem::DrawWithGles(Canvas* canvas)
{
#ifdef RS_ENABLE_GL
    if (!CreateEglTextureId()) {
        return;
    }
    GrGLTextureInfo textureInfo = { GL_TEXTURE_EXTERNAL_OES, texId_, GL_RGBA8_OES };

    GrBackendTexture backendTexture(
        surfaceBufferInfo_.width_, surfaceBufferInfo_.height_, GrMipMapped::kNo, textureInfo);

    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(surfaceBufferInfo_.width_);
    externalTextureInfo.SetHeight(surfaceBufferInfo_.height_);
    externalTextureInfo.SetIsMipMapped(false);
    externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
    externalTextureInfo.SetID(texId_);
    externalTextureInfo.SetFormat(GL_RGBA8_OES);
    Drawing::BitmapFormat bitmapFormat = { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    if (!canvas->GetGPUContext()) {
        LOGE("DrawSurfaceBufferOpItem::Draw: gpu context is nullptr");
        return;
    }
#ifndef ROSEN_EMULATOR
    auto surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
#else
    auto surfaceOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
#endif
    auto newImage = std::make_shared<Drawing::Image>();
    if (!newImage->BuildFromTexture(*canvas->GetGPUContext(), externalTextureInfo,
        surfaceOrigin, bitmapFormat, nullptr)) {
        LOGE("DrawSurfaceBufferOpItem::Draw: image BuildFromTexture failed");
        return;
    }
    canvas->DrawImage(*newImage, surfaceBufferInfo_.offSetX_, surfaceBufferInfo_.offSetY_, Drawing::SamplingOptions());
#endif // RS_ENABLE_GL
}

bool DrawSurfaceBufferOpItem::CreateEglTextureId()
{
    EGLint attrs[] = { EGL_IMAGE_PRESERVED, EGL_TRUE, EGL_NONE };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        LOGE("%{public}s create egl image fail %{public}d", __func__, eglGetError());
        return false;
    }

    // save
    GLuint originTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint *>(&originTexture));
    GLint minFilter;
    glGetTexParameteriv(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, &minFilter);
    GLint magFilter;
    glGetTexParameteriv(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, &magFilter);
    GLint wrapS;
    glGetTexParameteriv(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, &wrapS);
    GLint wrapT;
    glGetTexParameteriv(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, &wrapT);

    // Create texture object
    texId_ = 0;
    glGenTextures(1, &texId_);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texId_);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, static_cast<GLeglImageOES>(eglImage_));

    // restore
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, originTexture);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, wrapT);

    return true;
}
#endif
}
} // namespace Rosen
} // namespace OHOS

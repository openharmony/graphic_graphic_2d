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

#include <sstream>

#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_log.h"
#include "render/rs_pixel_map_util.h"
#include "render/rs_image_cache.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "rs_trace.h"
#include "utils/system_properties.h"
#include "pipeline/rs_task_dispatcher.h"
#include "platform/common/rs_system_properties.h"
#include "pipeline/sk_resource_manager.h"
#ifdef ROSEN_OHOS
#include "common/rs_common_tools.h"
#include "native_buffer_inner.h"
#include "native_window.h"
#endif
#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrBackendSemaphore.h"
#else
#include "include/gpu/GrBackendSemaphore.h"
#endif
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrDirectContext.h"
#else
#include "include/gpu/GrDirectContext.h"
#endif

namespace OHOS {
namespace Rosen {
constexpr int32_t CORNER_SIZE = 4;
#if (defined(ROSEN_OHOS) && defined(RS_ENABLE_GPU))
constexpr uint32_t FENCE_WAIT_TIME = 3000; // ms
#endif
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
constexpr uint8_t ASTC_HEADER_SIZE = 16;
#endif

#ifdef RS_ENABLE_VK
Drawing::ColorType GetColorTypeFromVKFormat(VkFormat vkFormat)
{
    if (!RSSystemProperties::IsUseVulkan()) {
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

bool WaitFence(const sptr<SyncFence>& fence)
{
    if (fence == nullptr) {
        return false;
    }
    int ret = fence->Wait(FENCE_WAIT_TIME);
    auto fenceFd = fence->Get();
    if (ret < 0 && fenceFd != -1) {
        return false;
    }
    return true;
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
    rsImage_->SetImageRotateDegree(imageInfo.rotateDegree);
    std::vector<Drawing::Point> radiusValue(imageInfo.radius, imageInfo.radius + CORNER_SIZE);
    rsImage_->SetRadius(radiusValue);
    rsImage_->SetScale(imageInfo.scale);
    rsImage_->SetDynamicRangeMode(imageInfo.dynamicRangeMode);
    rsImage_->SetFitMatrix(imageInfo.fitMatrix);
    rsImage_->SetOrientationFit(imageInfo.orientationNum);
    imageInfo_ = imageInfo;
}

RSExtendImageObject::RSExtendImageObject(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::AdaptiveImageInfo& imageInfo)
{
    if (pixelMap) {
#ifdef ROSEN_OHOS
        if (RSSystemProperties::GetDumpUIPixelmapEnabled()) {
            CommonTools::SavePixelmapToFile(pixelMap, "/data/storage/el1/base/imageObject_");
        }
#endif
        rsImage_ = std::make_shared<RSImage>();
        rsImage_->SetPixelMap(pixelMap);
        rsImage_->SetImageFit(imageInfo.fitNum);
        rsImage_->SetImageRotateDegree(imageInfo.rotateDegree);
        rsImage_->SetImageRepeat(imageInfo.repeatNum);
        std::vector<Drawing::Point> radiusValue(imageInfo.radius, imageInfo.radius + CORNER_SIZE);
        rsImage_->SetRadius(radiusValue);
        rsImage_->SetScale(imageInfo.scale);
        rsImage_->SetDynamicRangeMode(imageInfo.dynamicRangeMode);
        RectF frameRect(imageInfo.frameRect.GetLeft(),
                        imageInfo.frameRect.GetTop(),
                        imageInfo.frameRect.GetRight(),
                        imageInfo.frameRect.GetBottom());
        rsImage_->SetFrameRect(frameRect);
        rsImage_->SetFitMatrix(imageInfo.fitMatrix);
        rsImage_->SetOrientationFit(imageInfo.orientationNum);
    }
}

void RSExtendImageObject::SetNodeId(NodeId id)
{
    if (rsImage_) {
        rsImage_->UpdateNodeIdToPicture(id);
    }
    nodeId_ = id;
}

NodeId RSExtendImageObject::GetNodeId() const
{
    return nodeId_;
}

void RSExtendImageObject::SetPaint(Drawing::Paint paint)
{
    if (rsImage_) {
        rsImage_->SetPaint(paint);
    }
}

void RSExtendImageObject::Purge()
{
    if (rsImage_) {
        rsImage_->Purge();
    }
}

bool RSExtendImageObject::IsValid()
{
    return rsImage_ != nullptr && rsImage_->GetPixelMap() != nullptr;
}

void RSExtendImageObject::Dump(std::string& dump)
{
    if (rsImage_ == nullptr) {
        dump += " rsImage is nullptr";
    } else {
        std::string rsImageDump;
        rsImage_->Dump(rsImageDump, 0);
        rsImageDump.erase(
            std::remove_if(rsImageDump.begin(),
            rsImageDump.end(),
            [](auto c) { return c == '\t' || c == '\n'; }),
            rsImageDump.end()
        );
        dump += rsImageDump;
    }
}

void RSExtendImageObject::Playback(Drawing::Canvas& canvas, const Drawing::Rect& rect,
    const Drawing::SamplingOptions& sampling, bool isBackground)
{
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!rsImage_) {
        return;
    }
    std::shared_ptr<Media::PixelMap> pixelmap = rsImage_->GetPixelMap();
    bool isPurgeable = rsImage_->IsPurgeable();
    RSImageBase::PixelMapUseCountGuard guard = {pixelmap, isPurgeable};
    if (isPurgeable) {
        rsImage_->DePurge();
    }
    if (pixelmap && pixelmap->IsAstc()) {
        if (auto recordingCanvas = static_cast<ExtendRecordingCanvas*>(canvas.GetRecordingCanvas())) {
            Drawing::AdaptiveImageInfo imageInfo = rsImage_->GetAdaptiveImageInfoWithCustomizedFrameRect(rect);
            if (isPurgeable) {
                pixelmap->IncreaseUseCount();
            }
            recordingCanvas->DrawPixelMapWithParm(pixelmap, imageInfo, sampling);
            return;
        }
    }
    if (canvas.GetRecordingCanvas()) {
        image_ = RSPixelMapUtil::ExtractDrawingImage(pixelmap);
        if (image_) {
#ifndef ROSEN_ARKUI_X
            SKResourceManager::Instance().HoldResource(image_);
#endif
            rsImage_->SetDmaImage(image_);
        }
        rsImage_->CanvasDrawImage(canvas, rect, sampling, isBackground);
        return;
    }
    PreProcessPixelMap(canvas, pixelmap, sampling);
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
        delete object;
        return nullptr;
    }
    if (object->rsImage_) {
        object->rsImage_->MarkPurgeable();
    }
    return object;
}

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
void RSExtendImageObject::PreProcessPixelMap(Drawing::Canvas& canvas, const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::SamplingOptions& sampling)
{
    if (!pixelMap || !rsImage_) {
        return;
    }
    auto colorSpace = RSPixelMapUtil::GetPixelmapColorSpace(pixelMap);
#ifdef USE_VIDEO_PROCESSING_ENGINE
    if (pixelMap->IsHdr()) {
        colorSpace = Drawing::ColorSpace::CreateSRGB();
    }
#endif
    if (!pixelMap->IsAstc() && RSPixelMapUtil::IsSupportZeroCopy(pixelMap, sampling)) {
#if defined(RS_ENABLE_GL)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL &&
            GetDrawingImageFromSurfaceBuffer(canvas, reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd()))) {
            rsImage_->SetDmaImage(image_);
        }
#endif
#if defined(RS_ENABLE_VK)
        if (RSSystemProperties::IsUseVukan() &&
            GetRsImageCache(canvas, pixelMap, reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd()), colorSpace)) {
            rsImage_->SetDmaImage(image_);
        }
#endif
        return;
    }
    if (pixelMap->IsAstc()) {
        std::shared_ptr<Drawing::Data> fileData = std::make_shared<Drawing::Data>();
        // After RS is switched to Vulkan, the judgment of GpuApiType can be deleted.
        if (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC &&
            (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR)) {
            if (!nativeWindowBuffer_) {
                sptr<SurfaceBuffer> sfBuffer(reinterpret_cast<SurfaceBuffer *>(pixelMap->GetFd()));
                nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&sfBuffer);
            }
            OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer_);
            if (nativeBuffer == nullptr || !fileData->BuildFromOHNativeBuffer(nativeBuffer, pixelMap->GetCapacity())) {
                LOGE("PreProcessPixelMap data BuildFromOHNativeBuffer fail");
                return;
            }
        } else {
            const void* data = pixelMap->GetPixels();
            if (pixelMap->GetCapacity() > ASTC_HEADER_SIZE &&
                (data == nullptr || !fileData->BuildWithoutCopy((void*)((char*) data + ASTC_HEADER_SIZE),
                pixelMap->GetCapacity() - ASTC_HEADER_SIZE))) {
                LOGE("PreProcessPixelMap data BuildWithoutCopy fail");
                return;
            }
        }
        rsImage_->SetCompressData(fileData);
        return;
    }
    if (RSPixelMapUtil::IsYUVFormat(pixelMap)) {
        rsImage_->MarkYUVImage();
    }
}
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
bool RSExtendImageObject::GetRsImageCache(Drawing::Canvas& canvas, const std::shared_ptr<Media::PixelMap>& pixelMap,
    SurfaceBuffer *surfaceBuffer, const std::shared_ptr<Drawing::ColorSpace>& colorSpace)
{
    if (pixelMap == nullptr) {
        return false;
    }
    if (rsImage_ == nullptr) {
        return false;
    }
    std::shared_ptr<Drawing::Image> imageCache = nullptr;
    if (!pixelMap->IsEditable()) {
        imageCache = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(
            rsImage_->GetUniqueId(), gettid());
    }
    if (imageCache) {
        this->image_ = imageCache;
    } else {
        bool ret = MakeFromTextureForVK(
            canvas, reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd()), colorSpace);
        if (ret && image_ && !pixelMap->IsEditable()) {
            SKResourceManager::Instance().HoldResource(image_);
            RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(
                rsImage_->GetUniqueId(), image_, gettid());
        }
        return ret;
    }
    return true;
}
#endif

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

    image_ = std::make_shared<Drawing::Image>();
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
bool RSExtendImageObject::MakeFromTextureForVK(Drawing::Canvas& canvas, SurfaceBuffer *surfaceBuffer,
    const std::shared_ptr<Drawing::ColorSpace>& colorSpace)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        return false;
    }
    if (surfaceBuffer == nullptr || surfaceBuffer->GetBufferHandle() == nullptr) {
        RS_LOGE("MakeFromTextureForVK surfaceBuffer is nullptr or buffer handle is nullptr");
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
            surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), false);
        if (backendTexture_.IsValid()) {
            auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
            if (!vkTextureInfo) {
                return false;
            }
            cleanUpHelper_ = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
                vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
        } else {
            return false;
        }
        tid_ = gettid();
    }

    auto context = canvas.GetGPUContext();
    if (!context) {
        RS_LOGE("MakeFromTextureForVK gpu context is nullptr");
        return false;
    }
    image_ = std::make_shared<Drawing::Image>();
    auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
    if (!vkTextureInfo || !cleanUpHelper_) {
        return false;
    }
    Drawing::ColorType colorType = GetColorTypeFromVKFormat(vkTextureInfo->format);
    Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
    if (!image_->BuildFromTexture(*context, backendTexture_.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, colorSpace,
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
    if (RSSystemProperties::IsUseVulkan()) {
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
    const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint)
{
    if (rsImage_) {
        rsImage_->DrawImage(canvas, sampling, constraint);
    }
}

void RSExtendImageBaseObj::SetNodeId(NodeId id)
{
    if (rsImage_) {
        rsImage_->UpdateNodeIdToPicture(id);
    }
}

void RSExtendImageBaseObj::Purge()
{
    if (rsImage_) {
        rsImage_->Purge();
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
        delete object;
        return nullptr;
    }
    if (object->rsImage_) {
        object->rsImage_->MarkPurgeable();
    }
    return object;
}

RSExtendImageNineObject::RSExtendImageNineObject(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetPixelMap(pixelMap);
    }
}

void RSExtendImageNineObject::Playback(Drawing::Canvas& canvas, const Drawing::RectI& center,
    const Drawing::Rect& dst, Drawing::FilterMode filterMode)
{
    if (rsImage_) {
        rsImage_->DrawImageNine(canvas, center, dst, filterMode);
    }
}

void RSExtendImageNineObject::SetNodeId(NodeId id)
{
    if (rsImage_) {
        rsImage_->UpdateNodeIdToPicture(id);
    }
}

void RSExtendImageNineObject::Purge()
{
    if (rsImage_) {
        rsImage_->Purge();
    }
}

bool RSExtendImageNineObject::Marshalling(Parcel &parcel) const
{
    bool ret = RSMarshallingHelper::Marshalling(parcel, rsImage_);
    return ret;
}

RSExtendImageNineObject *RSExtendImageNineObject::Unmarshalling(Parcel &parcel)
{
    auto object = new RSExtendImageNineObject();
    bool ret = RSMarshallingHelper::Unmarshalling(parcel, object->rsImage_);
    if (!ret) {
        delete object;
        return nullptr;
    }
    return object;
}

RSExtendImageLatticeObject::RSExtendImageLatticeObject(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetPixelMap(pixelMap);
    }
}

void RSExtendImageLatticeObject::Playback(Drawing::Canvas& canvas, const Drawing::Lattice& lattice,
    const Drawing::Rect& dst, Drawing::FilterMode filterMode)
{
    if (rsImage_) {
        rsImage_->DrawImageLattice(canvas, lattice, dst, filterMode);
    }
}

void RSExtendImageLatticeObject::SetNodeId(NodeId id)
{
    if (rsImage_) {
        rsImage_->UpdateNodeIdToPicture(id);
    }
}

void RSExtendImageLatticeObject::Purge()
{
    if (rsImage_) {
        rsImage_->Purge();
    }
}

bool RSExtendImageLatticeObject::Marshalling(Parcel &parcel) const
{
    bool ret = RSMarshallingHelper::Marshalling(parcel, rsImage_);
    return ret;
}

RSExtendImageLatticeObject *RSExtendImageLatticeObject::Unmarshalling(Parcel &parcel)
{
    auto object = new RSExtendImageLatticeObject();
    bool ret = RSMarshallingHelper::Unmarshalling(parcel, object->rsImage_);
    if (!ret) {
        delete object;
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
UNMARSHALLING_REGISTER(DrawImageWithParm, DrawOpItem::IMAGE_WITH_PARM_OPITEM,
    DrawImageWithParmOpItem::Unmarshalling, sizeof(DrawImageWithParmOpItem::ConstructorHandle));

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

void DrawImageWithParmOpItem::Dump(std::string& out) const
{
    out += "[sampling:";
    sampling_.Dump(out);
    out += " objectHandle:";
    
    std::stringstream stream;
    stream << std::hex << objectHandle_.get() << "]";
    out += std::string(stream.str());
}

/* DrawPixelMapWithParmOpItem */
UNMARSHALLING_REGISTER(DrawPixelMapWithParm, DrawOpItem::PIXELMAP_WITH_PARM_OPITEM,
    DrawPixelMapWithParmOpItem::Unmarshalling, sizeof(DrawPixelMapWithParmOpItem::ConstructorHandle));

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
    objectHandle_->SetPaint(paint_);
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

void DrawPixelMapWithParmOpItem::DumpItems(std::string& out) const
{
    out += " sampling";
    sampling_.Dump(out);

    if (objectHandle_ == nullptr) {
        out += " objectHandle_ is nullptr";
    } else {
        objectHandle_->Dump(out);
    }
}

#ifdef RS_ENABLE_VK
/* DrawHybridPixelMapOpItem */
UNMARSHALLING_REGISTER(DrawHybridPixelMapOpItem, DrawOpItem::HYBRID_RENDER_PIXELMAP_OPITEM,
    DrawHybridPixelMapOpItem::Unmarshalling, sizeof(DrawHybridPixelMapOpItem::ConstructorHandle));
 
DrawHybridPixelMapOpItem::DrawHybridPixelMapOpItem(
    const DrawCmdList& cmdList, DrawHybridPixelMapOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, HYBRID_RENDER_PIXELMAP_OPITEM), sampling_(handle->sampling),
    isRenderForeground_(handle->isRenderForeground)
{
    objectHandle_ = CmdListHelper::GetImageObjectFromCmdList(cmdList, handle->objectHandle);
    auto entry = CmdListHelper::GetSurfaceBufferEntryFromCmdList(cmdList, handle->entryId);
    if (entry != nullptr) {
        fence_ = entry->acquireFence_;
    }
}

DrawHybridPixelMapOpItem::DrawHybridPixelMapOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling, const Paint& paint)
    : DrawWithPaintOpItem(paint, HYBRID_RENDER_PIXELMAP_OPITEM), sampling_(sampling)
{
    objectHandle_ = std::make_shared<RSExtendImageObject>(pixelMap, rsImageInfo);
}
 
void DrawHybridPixelMapOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto objectHandle = CmdListHelper::AddImageObjectToCmdList(cmdList, objectHandle_);
    uint32_t entryId = CmdListHelper::AddSurfaceBufferEntryToCmdList(cmdList,
        std::make_shared<SurfaceBufferEntry>(nullptr, fence_));
    cmdList.AddOp<ConstructorHandle>(objectHandle, sampling_, paintHandle, entryId, isRenderForeground_);
}
 
std::shared_ptr<DrawOpItem> DrawHybridPixelMapOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawHybridPixelMapOpItem>(
        cmdList, static_cast<DrawHybridPixelMapOpItem::ConstructorHandle*>(handle));
}
 
void DrawHybridPixelMapOpItem::SetNodeId(NodeId id)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawHybridPixelMapOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->SetNodeId(id);
}
 
void DrawHybridPixelMapOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawHybridPixelMapOpItem objectHandle is nullptr!");
        return;
    }

    OHOS::Rosen::RSPaintFilterCanvas* paintCanvas = static_cast<OHOS::Rosen::RSPaintFilterCanvas*>(canvas);
    Drawing::Filter filter;

    filter.SetColorFilter(ColorFilter::CreateBlendModeColorFilter(paintCanvas->GetEnvForegroundColor(),
        BlendMode::SRC_ATOP));
    paint_.SetFilter(filter);
    if (isRenderForeground_) {
        objectHandle_->SetPaint(paint_);
        paintCanvas->AttachPaintWithColor(paint_);
    } else {
        paintCanvas->AttachPaint(paint_);
    }
    if (objectHandle_->IsValid() && !WaitFence(fence_)) {
        if (auto rsExtendImageObject = static_cast<RSExtendImageObject*>(objectHandle_.get())) {
            RS_TRACE_NAME_FMT("DrawHybridPixelMap waitfence error, nodeId: %llu", rsExtendImageObject->GetNodeId());
        }
    }
    objectHandle_->Playback(*paintCanvas, *rect, sampling_, false);
}
 
void DrawHybridPixelMapOpItem::DumpItems(std::string& out) const
{
    out += " sampling";
    sampling_.Dump(out);
    out += " foreground:" + std::to_string(isRenderForeground_);
    if (fence_ != nullptr) {
        out += " fenceFd:" + std::to_string(fence_->Get());
    }
}
#endif

/* DrawPixelMapRectOpItem */
UNMARSHALLING_REGISTER(DrawPixelMapRect, DrawOpItem::PIXELMAP_RECT_OPITEM,
    DrawPixelMapRectOpItem::Unmarshalling, sizeof(DrawPixelMapRectOpItem::ConstructorHandle));

DrawPixelMapRectOpItem::DrawPixelMapRectOpItem(
    const DrawCmdList& cmdList, DrawPixelMapRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PIXELMAP_RECT_OPITEM), sampling_(handle->sampling),
      constraint_(handle->constraint)
{
    objectHandle_ = CmdListHelper::GetImageBaseObjFromCmdList(cmdList, handle->objectHandle);
}

DrawPixelMapRectOpItem::DrawPixelMapRectOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap, const Rect& src,
    const Rect& dst, const SamplingOptions& sampling, SrcRectConstraint constraint, const Paint& paint)
    : DrawWithPaintOpItem(paint, PIXELMAP_RECT_OPITEM), sampling_(sampling), constraint_(constraint)
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
    cmdList.AddOp<ConstructorHandle>(objectHandle, sampling_, constraint_, paintHandle);
}

void DrawPixelMapRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapRectOpItem objectHandle is nullptr!");
        return;
    }
    canvas->AttachPaint(paint_);
    objectHandle_->Playback(*canvas, *rect, sampling_, constraint_);
}

void DrawPixelMapRectOpItem::SetNodeId(NodeId id)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapRectOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->SetNodeId(id);
}

void DrawPixelMapRectOpItem::DumpItems(std::string& out) const
{
    out += " sampling";
    sampling_.Dump(out);
}

/* DrawPixelMapNineOpItem */
UNMARSHALLING_REGISTER(DrawPixelMapNine, DrawOpItem::PIXELMAP_NINE_OPITEM,
    DrawPixelMapNineOpItem::Unmarshalling, sizeof(DrawPixelMapNineOpItem::ConstructorHandle));

DrawPixelMapNineOpItem::DrawPixelMapNineOpItem(
    const DrawCmdList& cmdList, DrawPixelMapNineOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PIXELMAP_NINE_OPITEM), center_(handle->center),
      dst_(handle->dst), filterMode_(handle->filterMode)
{
    objectHandle_ = CmdListHelper::GetImageNineObjecFromCmdList(cmdList, handle->objectHandle);
}

DrawPixelMapNineOpItem::DrawPixelMapNineOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::RectI& center, const Drawing::Rect& dst, Drawing::FilterMode filterMode, const Paint& paint)
    : DrawWithPaintOpItem(paint, PIXELMAP_NINE_OPITEM), center_(center), dst_(dst), filterMode_(filterMode)
{
    objectHandle_ = std::make_shared<RSExtendImageNineObject>(pixelMap);
}

std::shared_ptr<DrawOpItem> DrawPixelMapNineOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPixelMapNineOpItem>(
        cmdList, static_cast<DrawPixelMapNineOpItem::ConstructorHandle*>(handle));
}

void DrawPixelMapNineOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto objectHandle = CmdListHelper::AddImageNineObjecToCmdList(cmdList, objectHandle_);
    cmdList.AddOp<ConstructorHandle>(objectHandle, center_, dst_, filterMode_, paintHandle);
}

void DrawPixelMapNineOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapNineOpItem objectHandle is nullptr!");
        return;
    }
    canvas->AttachPaint(paint_);
    objectHandle_->Playback(*canvas, center_, dst_, filterMode_);
}

void DrawPixelMapNineOpItem::SetNodeId(NodeId id)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapNineOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->SetNodeId(id);
}

void DrawPixelMapNineOpItem::DumpItems(std::string& out) const
{
    out += " DrawPixelMapNineOpItem";
}

/* DrawPixelMapLatticeOpItem */
UNMARSHALLING_REGISTER(DrawPixelMapLattice, DrawOpItem::PIXELMAP_LATTICE_OPITEM,
    DrawPixelMapLatticeOpItem::Unmarshalling, sizeof(DrawPixelMapLatticeOpItem::ConstructorHandle));

DrawPixelMapLatticeOpItem::DrawPixelMapLatticeOpItem(
    const DrawCmdList& cmdList, DrawPixelMapLatticeOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PIXELMAP_LATTICE_OPITEM),
      dst_(handle->dst), filterMode_(handle->filterMode)
{
    objectHandle_ = CmdListHelper::GetImageLatticeObjecFromCmdList(cmdList, handle->objectHandle);
    lattice_ = CmdListHelper::GetLatticeFromCmdList(cmdList, handle->latticeHandle);
}

DrawPixelMapLatticeOpItem::DrawPixelMapLatticeOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::Lattice& lattice, const Drawing::Rect& dst, Drawing::FilterMode filterMode, const Paint& paint)
    : DrawWithPaintOpItem(paint, PIXELMAP_LATTICE_OPITEM), lattice_(lattice), dst_(dst), filterMode_(filterMode)
{
    objectHandle_ = std::make_shared<RSExtendImageLatticeObject>(pixelMap);
}

std::shared_ptr<DrawOpItem> DrawPixelMapLatticeOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPixelMapLatticeOpItem>(
        cmdList, static_cast<DrawPixelMapLatticeOpItem::ConstructorHandle*>(handle));
}

void DrawPixelMapLatticeOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto objectHandle = CmdListHelper::AddImageLatticeObjecToCmdList(cmdList, objectHandle_);
    auto latticeHandle = CmdListHelper::AddLatticeToCmdList(cmdList, lattice_);
    cmdList.AddOp<ConstructorHandle>(objectHandle, latticeHandle, dst_, filterMode_, paintHandle);
}

void DrawPixelMapLatticeOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapLatticeOpItem objectHandle is nullptr!");
        return;
    }
    canvas->AttachPaint(paint_);
    objectHandle_->Playback(*canvas, lattice_, dst_, filterMode_);
}

void DrawPixelMapLatticeOpItem::SetNodeId(NodeId id)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapLatticeOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->SetNodeId(id);
}

void DrawPixelMapLatticeOpItem::DumpItems(std::string& out) const
{
    out += " DrawPixelMapLatticeOpItem";
}

/* DrawFuncOpItem */
UNMARSHALLING_REGISTER(DrawFunc, DrawOpItem::DRAW_FUNC_OPITEM,
    DrawFuncOpItem::Unmarshalling, sizeof(DrawFuncOpItem::ConstructorHandle));

DrawFuncOpItem::DrawFuncOpItem(const DrawCmdList& cmdList, DrawFuncOpItem::ConstructorHandle* handle)
    : DrawOpItem(DRAW_FUNC_OPITEM)
{
    if (handle == nullptr) {
        LOGE("DrawFuncOpItem handle is nullptr!");
        return;
    }
    objectHandle_ = CmdListHelper::GetDrawFuncObjFromCmdList(cmdList, handle->funcObjectId);
}

DrawFuncOpItem::DrawFuncOpItem(RecordingCanvas::DrawFunc&& drawFunc) : DrawOpItem(DRAW_FUNC_OPITEM)
{
    objectHandle_ = std::make_shared<RSExtendDrawFuncObj>(std::move(drawFunc));
}

std::shared_ptr<DrawOpItem> DrawFuncOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    if (handle == nullptr) {
        LOGE("Unmarshalling handle is nullptr!");
        return nullptr;
    }
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
UNMARSHALLING_REGISTER(DrawSurfaceBuffer, DrawOpItem::SURFACEBUFFER_OPITEM,
    DrawSurfaceBufferOpItem::Unmarshalling, sizeof(DrawSurfaceBufferOpItem::ConstructorHandle));

DrawSurfaceBufferOpItem::DrawSurfaceBufferOpItem(
    const DrawCmdList& cmdList, DrawSurfaceBufferOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, SURFACEBUFFER_OPITEM),
      surfaceBufferInfo_(nullptr, handle->surfaceBufferInfo.dstRect_.GetLeft(),
          handle->surfaceBufferInfo.dstRect_.GetTop(), handle->surfaceBufferInfo.dstRect_.GetWidth(),
          handle->surfaceBufferInfo.dstRect_.GetHeight(), handle->surfaceBufferInfo.pid_,
          handle->surfaceBufferInfo.uid_, nullptr, handle->surfaceBufferInfo.transform_,
          handle->surfaceBufferInfo.srcRect_),
      isNeedDrawDirectly_(!IsValidRemoteAddress(handle->surfaceBufferInfo.pid_,
          handle->surfaceBufferInfo.uid_))
{
    auto surfaceBufferEntry = CmdListHelper::GetSurfaceBufferEntryFromCmdList(cmdList, handle->surfaceBufferId);
    if (surfaceBufferEntry) {
        surfaceBufferInfo_.surfaceBuffer_ = surfaceBufferEntry->surfaceBuffer_;
        surfaceBufferInfo_.acquireFence_ = surfaceBufferEntry->acquireFence_;
    }
}

DrawSurfaceBufferOpItem::~DrawSurfaceBufferOpItem()
{
    OnDestruct();
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
    std::shared_ptr<SurfaceBufferEntry> surfaceBufferEntry =
        std::make_shared<SurfaceBufferEntry>(surfaceBufferInfo_.surfaceBuffer_, surfaceBufferInfo_.acquireFence_);
    cmdList.AddOp<ConstructorHandle>(CmdListHelper::AddSurfaceBufferEntryToCmdList(cmdList, surfaceBufferEntry),
        surfaceBufferInfo_.dstRect_.GetLeft(), surfaceBufferInfo_.dstRect_.GetTop(),
        surfaceBufferInfo_.dstRect_.GetWidth(), surfaceBufferInfo_.dstRect_.GetHeight(), surfaceBufferInfo_.pid_,
        surfaceBufferInfo_.uid_, surfaceBufferInfo_.transform_, surfaceBufferInfo_.srcRect_, paintHandle);
}

namespace {
    std::function<void(const DrawSurfaceBufferFinishCbData&)> surfaceBufferFinishCb;
    std::function<void(const DrawSurfaceBufferAfterAcquireCbData&)> surfaceBufferAfterAcquireCb;
    std::function<NodeId()> getRootNodeIdForRT;
    bool contextIsUniRender = true;
}

void DrawSurfaceBufferOpItem::OnDestruct()
{
    ReleaseBuffer();
}

void DrawSurfaceBufferOpItem::SetIsUniRender(bool isUniRender)
{
    contextIsUniRender = isUniRender;
}

void DrawSurfaceBufferOpItem::OnBeforeDraw()
{
    switch (MapGraphicTransformType(surfaceBufferInfo_.transform_)) {
        case GraphicTransformType::GRAPHIC_ROTATE_90 :
            [[fallthrough]];
        case GraphicTransformType::GRAPHIC_ROTATE_270 : {
            isNeedRotateDstRect_ = true;
            break;
        }
        default:
            break;
    }
}

void DrawSurfaceBufferOpItem::OnAfterDraw()
{
    isRendered_ = true;
    if (contextIsUniRender || IsNeedDrawDirectly()) {
        return;
    }
    rootNodeId_ = getRootNodeIdForRT ? std::invoke(getRootNodeIdForRT) : INVALID_NODEID;
#ifdef RS_ENABLE_GL
    if (SystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        auto sync = eglCreateSyncKHR(disp, EGL_SYNC_NATIVE_FENCE_ANDROID, nullptr);
        if (sync == EGL_NO_SYNC_KHR) {
            RS_LOGE("DrawSurfaceBufferOpItem::OnAfterDraw Error on eglCreateSyncKHR %{public}d",
                static_cast<int>(eglGetError()));
            return;
        }
        auto fd = eglDupNativeFenceFDANDROID(disp, sync);
        eglDestroySyncKHR(disp, sync);
        if (fd == EGL_NO_NATIVE_FENCE_FD_ANDROID) {
            RS_LOGE("DrawSurfaceBufferOpItem::OnAfterDraw: Error on eglDupNativeFenceFD");
            return;
        }
        releaseFence_ = new (std::nothrow) SyncFence(fd);
        if (!releaseFence_) {
            releaseFence_ = SyncFence::INVALID_FENCE;
        }
    }
#endif
}

void DrawSurfaceBufferOpItem::ReleaseBuffer()
{
    if (IsNeedDrawDirectly()) {
        return;
    }
    if (surfaceBufferFinishCb && surfaceBufferInfo_.surfaceBuffer_) {
        RS_TRACE_NAME_FMT("DrawSurfaceBufferOpItem::ReleaseBuffer %s Release, isNeedTriggerCbDirectly = %d",
            std::to_string(surfaceBufferInfo_.surfaceBuffer_->GetSeqNum()).c_str(),
            releaseFence_ && releaseFence_->IsValid());
        std::invoke(surfaceBufferFinishCb, DrawSurfaceBufferFinishCbData {
            .uid = surfaceBufferInfo_.uid_,
            .pid = surfaceBufferInfo_.pid_,
            .surfaceBufferId = surfaceBufferInfo_.surfaceBuffer_->GetSeqNum(),
            .rootNodeId = rootNodeId_,
            .releaseFence = releaseFence_,
            .isRendered = isRendered_,
            .isNeedTriggerCbDirectly = releaseFence_ && releaseFence_->IsValid(),
        });
    }
}

void DrawSurfaceBufferOpItem::OnAfterAcquireBuffer()
{
    if (IsNeedDrawDirectly()) {
        return;
    }
    if (surfaceBufferAfterAcquireCb && surfaceBufferInfo_.surfaceBuffer_) {
        std::invoke(surfaceBufferAfterAcquireCb, DrawSurfaceBufferAfterAcquireCbData {
            .uid = surfaceBufferInfo_.uid_,
            .pid = surfaceBufferInfo_.pid_,
        });
    }
}

void DrawSurfaceBufferOpItem::RegisterSurfaceBufferCallback(
    DrawSurfaceBufferOpItemCb callbacks)
{
    if (std::exchange(surfaceBufferFinishCb, callbacks.OnFinish)) {
        RS_LOGE("DrawSurfaceBufferOpItem::RegisterSurfaceBufferCallback"
            " registered OnFinish twice incorrectly.");
    }
    if (std::exchange(surfaceBufferAfterAcquireCb, callbacks.OnAfterAcquireBuffer)) {
        RS_LOGE("DrawSurfaceBufferOpItem::RegisterSurfaceBufferCallback"
            " registered OnAfterAcquireBuffer twice incorrectly.");
    }
}

void DrawSurfaceBufferOpItem::RegisterGetRootNodeIdFuncForRT(std::function<NodeId()> func)
{
    if (std::exchange(getRootNodeIdForRT, func)) {
        RS_LOGE("DrawSurfaceBufferOpItem::RegisterGetRootNodeIdFuncForRT"
            " registered OnAfterAcquireBuffer twice incorrectly.");
    }
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
    DealWithRotate(canvas);
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

void DrawSurfaceBufferOpItem::DealWithRotate(Canvas* canvas)
{
    // The first step rotates the center of the buffer by the corresponding
    // number of degrees, and in the second step we translate according to
    // the relative coordinates.
    // The purpose of our translation is to align the 'upper left corner'
    // of the relative coordinates to ensure that after rotating the buffer,
    // the buffer can be drawn in the correct viewport.
    auto halfW = surfaceBufferInfo_.dstRect_.GetWidth() / 2.f;
    auto halfH = surfaceBufferInfo_.dstRect_.GetHeight() / 2.f;
    auto halfWHDiff = halfW - halfH;
    switch (MapGraphicTransformType(surfaceBufferInfo_.transform_)) {
        case GraphicTransformType::GRAPHIC_ROTATE_90 : {
            canvas->Rotate(-90,
                canvas->GetTotalMatrix().Get(Drawing::Matrix::PERSP_0) + halfW,
                canvas->GetTotalMatrix().Get(Drawing::Matrix::PERSP_1) + halfH);
            canvas->Translate(halfWHDiff, -halfWHDiff);
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180 : {
            canvas->Rotate(-180,
                canvas->GetTotalMatrix().Get(Drawing::Matrix::PERSP_0) + halfW,
                canvas->GetTotalMatrix().Get(Drawing::Matrix::PERSP_1) + halfH);
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270 : {
            canvas->Rotate(-270,
                canvas->GetTotalMatrix().Get(Drawing::Matrix::PERSP_0) + halfW,
                canvas->GetTotalMatrix().Get(Drawing::Matrix::PERSP_1) + halfH);
            canvas->Translate(halfWHDiff, -halfWHDiff);
            break;
        }
        default:
            break;
    }
}

void DrawSurfaceBufferOpItem::Draw(Canvas* canvas)
{
    OnBeforeDraw();
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        DrawWithVulkan(canvas);
    }
#endif
#ifdef RS_ENABLE_GL
    if (SystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        DrawWithGles(canvas);
    }
#endif
    OnAfterDraw();
}

Drawing::BitmapFormat DrawSurfaceBufferOpItem::CreateBitmapFormat(int32_t bufferFormat)
{
    switch (bufferFormat) {
        case OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888 : {
            return {
                .colorType = Drawing::ColorType::COLORTYPE_RGBA_8888,
                .alphaType = Drawing::AlphaType::ALPHATYPE_PREMUL,
            };
        }
        case OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBX_8888 : {
            return {
                .colorType = Drawing::ColorType::COLORTYPE_RGB_888X,
                .alphaType = Drawing::AlphaType::ALPHATYPE_OPAQUE,
            };
        }
        case OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_BGRA_8888 : {
            return {
                .colorType = Drawing::ColorType::COLORTYPE_BGRA_8888,
                .alphaType = Drawing::AlphaType::ALPHATYPE_PREMUL,
            };
        }
        case OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGB_565 : {
            return {
                .colorType = Drawing::ColorType::COLORTYPE_RGB_565,
                .alphaType = Drawing::AlphaType::ALPHATYPE_OPAQUE,
            };
        }
        case OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_1010102 : {
            return {
                .colorType = Drawing::ColorType::COLORTYPE_RGBA_1010102,
                .alphaType = Drawing::AlphaType::ALPHATYPE_PREMUL,
            };
        }
        default : {
            return {
                .colorType = Drawing::ColorType::COLORTYPE_RGBA_8888,
                .alphaType = Drawing::AlphaType::ALPHATYPE_PREMUL,
            };
        }
    }
}

#ifdef RS_ENABLE_GL
GLenum DrawSurfaceBufferOpItem::GetGLTextureFormatByBitmapFormat(Drawing::ColorType colorType)
{
    switch (colorType) {
        case Drawing::ColorType::COLORTYPE_ALPHA_8 : {
            return GL_ALPHA8_OES;
        }
        case Drawing::ColorType::COLORTYPE_RGB_888X : {
            return GL_RGBA8_OES;
        }
        case Drawing::ColorType::COLORTYPE_RGB_565 : {
            return GL_RGB565_OES;
        }
        case Drawing::ColorType::COLORTYPE_RGBA_1010102 : {
            return GL_RGB10_A2_EXT;
        }
        case Drawing::ColorType::COLORTYPE_BGRA_8888 : {
            return GL_BGRA8_EXT;
        }
        case Drawing::ColorType::COLORTYPE_RGBA_8888 : {
            return GL_RGBA8_OES;
        }
        case Drawing::ColorType::COLORTYPE_RGBA_F16 : {
            return GL_RGBA16F_EXT;
        }
        case Drawing::ColorType::COLORTYPE_GRAY_8 :
            [[fallthrough]];
        case Drawing::ColorType::COLORTYPE_ARGB_4444 :
            [[fallthrough]];
        case Drawing::ColorType::COLORTYPE_N32 :
            [[fallthrough]];
        default : {
            return GL_RGBA8_OES;
        }
    }
}
#endif

void DrawSurfaceBufferOpItem::DrawWithVulkan(Canvas* canvas)
{
#ifdef RS_ENABLE_VK
    if (surfaceBufferInfo_.acquireFence_) {
        RS_TRACE_NAME_FMT("DrawSurfaceBufferOpItem::DrawWithVulkan waitfence");
        int res = surfaceBufferInfo_.acquireFence_->Wait(FENCE_WAIT_TIME);
        if (res < 0) {
            LOGW("DrawSurfaceBufferOpItem::DrawWithVulkan waitfence timeout");
        } else {
            OnAfterAcquireBuffer();
        }
    }
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
    Drawing::BitmapFormat bitmapFormat = CreateBitmapFormat(surfaceBufferInfo_.surfaceBuffer_->GetFormat());
    auto image = std::make_shared<Drawing::Image>();
    auto vkTextureInfo = backendTexture.GetTextureInfo().GetVKTextureInfo();
    if (!vkTextureInfo || !image->BuildFromTexture(*canvas->GetGPUContext(), backendTexture.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr, NativeBufferUtils::DeleteVkImage,
        new NativeBufferUtils::VulkanCleanupHelper(
            RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory))) {
        LOGE("DrawSurfaceBufferOpItem::Draw image BuildFromTexture failed");
        return;
    }
    auto rotatedRect = surfaceBufferInfo_.dstRect_;
    if (isNeedRotateDstRect_) {
        auto width = rotatedRect.GetWidth();
        auto height = rotatedRect.GetHeight();
        std::swap(width, height);
        rotatedRect.SetRight(rotatedRect.GetLeft() + width);
        rotatedRect.SetBottom(rotatedRect.GetTop() + height);
    }
    canvas->DrawImageRect(*image, surfaceBufferInfo_.srcRect_, rotatedRect,
        CreateSamplingOptions(canvas->GetTotalMatrix()));
#endif
}

void DrawSurfaceBufferOpItem::DrawWithGles(Canvas* canvas)
{
#ifdef RS_ENABLE_GL
    if (surfaceBufferInfo_.acquireFence_) {
        RS_TRACE_NAME_FMT("DrawSurfaceBufferOpItem::DrawWithGles waitfence");
        int res = surfaceBufferInfo_.acquireFence_->Wait(FENCE_WAIT_TIME);
        if (res < 0) {
            LOGW("DrawSurfaceBufferOpItem::DrawWithGles waitfence timeout");
        } else {
            OnAfterAcquireBuffer();
        }
    }
    if (!CreateEglTextureId()) {
        return;
    }
    auto rotatedRect = surfaceBufferInfo_.dstRect_;
    if (isNeedRotateDstRect_) {
        auto width = rotatedRect.GetWidth();
        auto height = rotatedRect.GetHeight();
        std::swap(width, height);
        rotatedRect.SetRight(rotatedRect.GetLeft() + width);
        rotatedRect.SetBottom(rotatedRect.GetTop() + height);
    }

    Drawing::BitmapFormat bitmapFormat = CreateBitmapFormat(surfaceBufferInfo_.surfaceBuffer_->GetFormat());
    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(rotatedRect.GetWidth());
    externalTextureInfo.SetHeight(rotatedRect.GetHeight());
    externalTextureInfo.SetIsMipMapped(false);
    externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
    externalTextureInfo.SetID(texId_);
    externalTextureInfo.SetFormat(GetGLTextureFormatByBitmapFormat(bitmapFormat.colorType));
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
    canvas->DrawImage(*newImage, rotatedRect.GetLeft(), rotatedRect.GetTop(),
        CreateSamplingOptions(canvas->GetTotalMatrix()));
#endif // RS_ENABLE_GL
}

Drawing::SamplingOptions DrawSurfaceBufferOpItem::CreateSamplingOptions(const Drawing::Matrix& matrix)
{
    auto scaleX = matrix.Get(Drawing::Matrix::SCALE_X);
    auto scaleY = matrix.Get(Drawing::Matrix::SCALE_Y);
    auto skewX = matrix.Get(Drawing::Matrix::SKEW_X);
    auto skewY = matrix.Get(Drawing::Matrix::SKEW_Y);
    if (ROSEN_EQ(skewX, 0.0f) && ROSEN_EQ(skewY, 0.0f)) {
        if (!ROSEN_EQ(std::abs(scaleX), 1.0f) || !ROSEN_EQ(std::abs(scaleY), 1.0f)) {
            // has scale
            return Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
        }
    } else if (ROSEN_EQ(scaleX, 0.0f) && ROSEN_EQ(scaleY, 0.0f)) {
        if (!ROSEN_EQ(std::abs(skewX), 1.0f) || !ROSEN_EQ(std::abs(skewY), 1.0f)) {
            // has scale
            return Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
        }
    } else {
        // skew and/or non 90 degrees rotation
        return Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    }
    return Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
}

GraphicTransformType DrawSurfaceBufferOpItem::MapGraphicTransformType(GraphicTransformType origin)
{
    GraphicTransformType rotation;
    switch (origin) {
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT90:
            [[fallthrough]];
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT90:
            rotation = GraphicTransformType::GRAPHIC_ROTATE_90;
            break;
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT180:
            [[fallthrough]];
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT180:
            rotation = GraphicTransformType::GRAPHIC_ROTATE_180;
            break;
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT270:
            [[fallthrough]];
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT270:
            rotation = GraphicTransformType::GRAPHIC_ROTATE_270;
            break;
        default:
            rotation = origin;
            break;
    }
    return rotation;
}

bool DrawSurfaceBufferOpItem::IsNeedDrawDirectly() const
{
    return isNeedDrawDirectly_;
}

bool DrawSurfaceBufferOpItem::IsValidRemoteAddress(pid_t pid, uint64_t uid)
{
    return pid != 0 && uid != 0ull;
}

bool DrawSurfaceBufferOpItem::CreateEglTextureId()
{
#ifdef RS_ENABLE_GL
    EGLint attrs[] = { EGL_IMAGE_PRESERVED, EGL_TRUE, EGL_NONE };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
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
#endif
    return true;
}

void DrawSurfaceBufferOpItem::DumpItems(std::string& out) const
{
    out += " surfaceBufferInfo[width:" + std::to_string(surfaceBufferInfo_.dstRect_.GetWidth());
    out += " height:" + std::to_string(surfaceBufferInfo_.dstRect_.GetHeight());
    out += " offSetX:" + std::to_string(surfaceBufferInfo_.dstRect_.GetLeft());
    out += " offSetY:" + std::to_string(surfaceBufferInfo_.dstRect_.GetTop());
    out += " transform: " + std::to_string(surfaceBufferInfo_.transform_);
    out += "]";
#ifdef RS_ENABLE_GL
    out += " texId:" + std::to_string(texId_);
#endif
}
#endif
}
} // namespace Rosen
} // namespace OHOS

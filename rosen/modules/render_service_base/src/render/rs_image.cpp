/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "render/rs_image.h"

#include "common/rs_common_tools.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_image_cache.h"
#include "render/rs_pixel_map_util.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
#include "rs_profiler.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t CORNER_SIZE = 4;
}

RSImage::~RSImage()
{}

bool RSImage::IsEqual(const RSImage& other) const
{
    bool radiusEq = true;
    for (auto i = 0; i < CORNER_SIZE; i++) {
        radiusEq &= (radius_[i] == other.radius_[i]);
    }
    return (image_ == other.image_) && (pixelMap_ == other.pixelMap_) &&
           (imageFit_ == other.imageFit_) && (imageRepeat_ == other.imageRepeat_) &&
           (scale_ == other.scale_) && radiusEq && (compressData_ == other.compressData_);
}

void RSImage::CanvasDrawImage(Drawing::Canvas& canvas, const Drawing::Rect& rect,
    const Drawing::SamplingOptions& samplingOptions, bool isBackground)
{
    if (canvas.GetRecordingState() && RSSystemProperties::GetDumpUICaptureEnabled() && pixelMap_) {
        CommonTools::SavePixelmapToFile(pixelMap_, "/data/rsImage_");
    }
    if (!isDrawn_ || rect != lastRect_ || RSPixelMapUtil::IsYUVFormat(pixelMap_)) {
        UpdateNodeIdToPicture(nodeId_);
        Drawing::AutoCanvasRestore acr(canvas, HasRadius());
        frameRect_.SetAll(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
        if (!isBackground) {
            ApplyImageFit();
            ApplyCanvasClip(canvas);
        }
        DrawImageRepeatRect(samplingOptions, canvas);
    } else {
        Drawing::AutoCanvasRestore acr(canvas, HasRadius());
        if (pixelMap_ != nullptr && pixelMap_->IsAstc()) {
            canvas.Save();
            RSPixelMapUtil::TransformDataSetForAstc(pixelMap_, src_, dst_, canvas);
        }
        if (image_) {
            if (!isBackground) {
                ApplyCanvasClip(canvas);
            }
            if (innerRect_.has_value()) {
                canvas.DrawImageNine(image_.get(), innerRect_.value(), dst_, Drawing::FilterMode::LINEAR);
            } else {
                canvas.DrawImageRect(*image_, src_, dst_, samplingOptions,
                    Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
            }
        }
        if (pixelMap_ != nullptr && pixelMap_->IsAstc()) {
            canvas.Restore();
        }
    }
    lastRect_ = rect;
}

struct ImageParameter
{
    float ratio;
    float srcW;
    float srcH;
    float frameW;
    float frameH;
    float dstW;
    float dstH;
};

RectF ApplyImageFitSwitch(ImageParameter &imageParameter, ImageFit imageFit_, RectF tempRectF)
{
    switch (imageFit_) {
        case ImageFit::TOP_LEFT:
            tempRectF.SetAll(0.f, 0.f, imageParameter.srcW, imageParameter.srcH);
            return tempRectF;
        case ImageFit::FILL:
            break;
        case ImageFit::NONE:
            imageParameter.dstW = imageParameter.srcW;
            imageParameter.dstH = imageParameter.srcH;
            break;
        case ImageFit::COVER:
            imageParameter.dstW = std::max(imageParameter.frameW, imageParameter.frameH * imageParameter.ratio);
            imageParameter.dstH = std::max(imageParameter.frameH, imageParameter.frameW / imageParameter.ratio);
            break;
        case ImageFit::FIT_WIDTH:
            imageParameter.dstH = imageParameter.frameW / imageParameter.ratio;
            break;
        case ImageFit::FIT_HEIGHT:
            imageParameter.dstW = imageParameter.frameH * imageParameter.ratio;
            break;
        case ImageFit::SCALE_DOWN:
            if (imageParameter.srcW < imageParameter.frameW && imageParameter.srcH < imageParameter.frameH) {
                imageParameter.dstW = imageParameter.srcW;
                imageParameter.dstH = imageParameter.srcH;
            } else {
                imageParameter.dstW = std::min(imageParameter.frameW, imageParameter.frameH * imageParameter.ratio);
                imageParameter.dstH = std::min(imageParameter.frameH, imageParameter.frameW / imageParameter.ratio);
            }
            break;
        case ImageFit::COVER_TOP_LEFT:
            imageParameter.dstW = std::max(imageParameter.frameW, imageParameter.frameH * imageParameter.ratio);
            imageParameter.dstH = std::max(imageParameter.frameH, imageParameter.frameW / imageParameter.ratio);
            tempRectF.SetAll(0, 0, std::ceil(imageParameter.dstW), std::ceil(imageParameter.dstH));
            return tempRectF;
        case ImageFit::CONTAIN:
        default:
            imageParameter.dstW = std::min(imageParameter.frameW, imageParameter.frameH * imageParameter.ratio);
            imageParameter.dstH = std::min(imageParameter.frameH, imageParameter.frameW / imageParameter.ratio);
            break;
    }
    constexpr float horizontalAlignmentFactor = 2.f;
    constexpr float verticalAlignmentFactor = 2.f;
    tempRectF.SetAll(std::floor((imageParameter.frameW - imageParameter.dstW) / horizontalAlignmentFactor),
                     std::floor((imageParameter.frameH - imageParameter.dstH) / verticalAlignmentFactor),
                     std::ceil(imageParameter.dstW),
                     std::ceil(imageParameter.dstH));
    return tempRectF;
}

void RSImage::ApplyImageFit()
{
    if (scale_ == 0) {
        RS_LOGE("RSImage::ApplyImageFit failed, scale_ is zero ");
        return;
    }
    const float srcW = srcRect_.width_ / scale_;
    const float srcH = srcRect_.height_ / scale_;
    const float frameW = frameRect_.width_;
    const float frameH = frameRect_.height_;
    float dstW = frameW;
    float dstH = frameH;
    if (srcH == 0) {
        RS_LOGE("RSImage::ApplyImageFit failed, srcH is zero ");
        return;
    }
    float ratio = srcW / srcH;
    if (ratio == 0) {
        RS_LOGE("RSImage::ApplyImageFit failed, ratio is zero ");
        return;
    }
    ImageParameter imageParameter;
    imageParameter.ratio = ratio;
    imageParameter.srcW = srcW;
    imageParameter.srcH = srcH;
    imageParameter.frameW = frameW;
    imageParameter.frameH = frameH;
    imageParameter.dstW = dstW;
    imageParameter.dstH = dstH;
    RectF tempRectF = dstRect_;
    dstRect_ = ApplyImageFitSwitch(imageParameter, imageFit_, tempRectF);
}

bool RSImage::HasRadius() const
{
    return hasRadius_;
}

void RSImage::ApplyCanvasClip(Drawing::Canvas& canvas)
{
    if (!HasRadius()) {
        return;
    }
    auto rect = (imageRepeat_ == ImageRepeat::NO_REPEAT) ? dstRect_.IntersectRect(frameRect_) : frameRect_;
    Drawing::RoundRect rrect(RSPropertiesPainter::Rect2DrawingRect(rect), radius_);
    canvas.ClipRoundRect(rrect, Drawing::ClipOp::INTERSECT, true);
}

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
static Drawing::CompressedType PixelFormatToCompressedType(Media::PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case Media::PixelFormat::ASTC_4x4: return Drawing::CompressedType::ASTC_RGBA8_4x4;
        case Media::PixelFormat::ASTC_6x6: return Drawing::CompressedType::ASTC_RGBA8_6x6;
        case Media::PixelFormat::ASTC_8x8: return Drawing::CompressedType::ASTC_RGBA8_8x8;
        case Media::PixelFormat::UNKNOWN:
        default: return Drawing::CompressedType::NoneType;
    }
}
#endif

void RSImage::UploadGpu(Drawing::Canvas& canvas)
{
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (compressData_) {
        auto cache = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(uniqueId_, gettid());
        std::lock_guard<std::mutex> lock(mutex_);
        if (cache) {
            image_ = cache;
        } else {
            if (canvas.GetGPUContext() == nullptr) {
                return;
            }
            Media::ImageInfo imageInfo;
            pixelMap_->GetImageInfo(imageInfo);
            Media::Size realSize;
            pixelMap_->GetAstcRealSize(realSize);
            auto image = std::make_shared<Drawing::Image>();
            bool result = image->BuildFromCompressed(*canvas.GetGPUContext(), compressData_,
                static_cast<int>(realSize.width), static_cast<int>(realSize.height),
                PixelFormatToCompressedType(imageInfo.pixelFormat));
            if (result) {
                image_ = image;
                SKResourceManager::Instance().HoldResource(image);
                RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(uniqueId_, image, gettid());
            } else {
                RS_LOGE("make astc image %{public}d (%{public}d, %{public}d) failed",
                    (int)uniqueId_, (int)srcRect_.width_, (int)srcRect_.height_);
            }
            compressData_ = nullptr;
        }
    }
    if (RSPixelMapUtil::IsYUVFormat(pixelMap_)) {
        ProcessYUVImage(canvas.GetGPUContext());
    }
#endif
}

void RSImage::DrawImageRepeatRect(const Drawing::SamplingOptions& samplingOptions, Drawing::Canvas& canvas)
{
    int minX = 0;
    int minY = 0;
    int maxX = 0;
    int maxY = 0;
    float left = frameRect_.left_;
    float right = frameRect_.GetRight();
    float top = frameRect_.top_;
    float bottom = frameRect_.GetBottom();
    // calculate REPEAT_XY
    float eps = 0.01; // set epsilon
    if (ImageRepeat::REPEAT_X == imageRepeat_ || ImageRepeat::REPEAT == imageRepeat_) {
        while (dstRect_.left_ + minX * dstRect_.width_ > left + eps) {
            --minX;
        }
        while (dstRect_.left_ + maxX * dstRect_.width_ < right - eps) {
            ++maxX;
        }
    }
    if (ImageRepeat::REPEAT_Y == imageRepeat_ || ImageRepeat::REPEAT == imageRepeat_) {
        while (dstRect_.top_ + minY * dstRect_.height_ > top + eps) {
            --minY;
        }
        while (dstRect_.top_ + maxY * dstRect_.height_ < bottom - eps) {
            ++maxY;
        }
    }

    // draw repeat rect
    ConvertPixelMapToDrawingImage();
    UploadGpu(canvas);
    src_ = RSPropertiesPainter::Rect2DrawingRect(srcRect_);
    bool isAstc = pixelMap_ != nullptr && pixelMap_->IsAstc();
    for (int i = minX; i <= maxX; ++i) {
        for (int j = minY; j <= maxY; ++j) {
            dst_ = Drawing::Rect(dstRect_.left_ + i * dstRect_.width_, dstRect_.top_ + j * dstRect_.height_,
                dstRect_.left_ + (i + 1) * dstRect_.width_, dstRect_.top_ + (j + 1) * dstRect_.height_);
            if (isAstc) {
                canvas.Save();
                RSPixelMapUtil::TransformDataSetForAstc(pixelMap_, src_, dst_, canvas);
            }
            if (image_) {
                if (canvas.GetTotalMatrix().HasPerspective()) {
                    // In case of perspective transformation, make dstRect 1px outset to anti-alias
                    dst_.MakeOutset(1, 1);
                }
                if (innerRect_.has_value()) {
                    canvas.DrawImageNine(image_.get(), innerRect_.value(), dst_, Drawing::FilterMode::LINEAR);
                } else {
                    canvas.DrawImageRect(*image_, src_, dst_, samplingOptions,
                        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
                }
            }
            if (isAstc) {
                canvas.Restore();
            }
        }
    }
    if (imageRepeat_ == ImageRepeat::NO_REPEAT) {
        isDrawn_ = true;
    }
}

void RSImage::SetCompressData(
    const std::shared_ptr<Drawing::Data> data, const uint32_t id, const int width, const int height)
{
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    compressData_ = data;
    if (compressData_) {
        srcRect_.SetAll(0.0, 0.0, width, height);
        GenUniqueId(image_ ? image_->GetUniqueID() : id);
        image_ = nullptr;
    }
#endif
}

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined (RS_ENABLE_VK))
void RSImage::SetCompressData(const std::shared_ptr<Drawing::Data> compressData)
{
    isDrawn_ = false;
    compressData_ = compressData;
}
#endif

void RSImage::SetImageFit(int fitNum)
{
    imageFit_ = static_cast<ImageFit>(fitNum);
}

void RSImage::SetImageRepeat(int repeatNum)
{
    imageRepeat_ = static_cast<ImageRepeat>(repeatNum);
}

void RSImage::SetRadius(const std::vector<Drawing::Point>& radius)
{
    hasRadius_ = false;
    for (auto i = 0; i < CORNER_SIZE; i++) {
        radius_[i] = radius[i];
        hasRadius_ = hasRadius_ || !radius_[i].IsZero();
    }
}

void RSImage::SetScale(double scale)
{
    if (scale > 0.0) {
        scale_ = scale;
    }
}

void RSImage::SetNodeId(NodeId nodeId)
{
    nodeId_ = nodeId;
}

void RSImage::SetPaint(Drawing::Paint paint)
{
    paint_ = paint;
}
void RSImage::SetDyamicRangeMode(uint32_t dynamicRangeMode)
{
    dynamicRangeMode_ = dynamicRangeMode;
}

#ifdef ROSEN_OHOS
static bool UnmarshallingIdAndSize(Parcel& parcel, uint64_t& uniqueId, int& width, int& height)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, uniqueId)) {
        RS_LOGE("RSImage::Unmarshalling uniqueId fail");
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, uniqueId);
    if (!RSMarshallingHelper::Unmarshalling(parcel, width)) {
        RS_LOGE("RSImage::Unmarshalling width fail");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, height)) {
        RS_LOGE("RSImage::Unmarshalling height fail");
        return false;
    }
    return true;
}

static bool UnmarshallingCompressData(Parcel& parcel, bool skipData, std::shared_ptr<Drawing::Data>& compressData)
{
    if (skipData) {
        if (!RSMarshallingHelper::SkipData(parcel)) {
            RS_LOGE("RSImage::Unmarshalling SkipData fail");
            return false;
        }
    } else {
        if (!RSMarshallingHelper::UnmarshallingWithCopy(parcel, compressData)) {
            RS_LOGE("RSImage::Unmarshalling UnmarshallingWithCopy Data fail");
            return false;
        }
    }
    return true;
}

bool RSImage::Marshalling(Parcel& parcel) const
{
    int imageFit = static_cast<int>(imageFit_);
    int imageRepeat = static_cast<int>(imageRepeat_);

    std::lock_guard<std::mutex> lock(mutex_);
    auto image = image_;
    if (image && image->IsTextureBacked()) {
        image = nullptr;
        ROSEN_LOGE("RSImage::Marshalling skip texture image");
    }
    bool success = RSMarshallingHelper::Marshalling(parcel, uniqueId_) &&
                   RSMarshallingHelper::Marshalling(parcel, static_cast<int>(srcRect_.width_)) &&
                   RSMarshallingHelper::Marshalling(parcel, static_cast<int>(srcRect_.height_)) &&
                   RSMarshallingHelper::Marshalling(parcel, nodeId_) &&
                   parcel.WriteBool(pixelMap_ == nullptr) &&
                   RSMarshallingHelper::Marshalling(parcel, image) &&
                   RSMarshallingHelper::Marshalling(parcel, pixelMap_) &&
                   RSMarshallingHelper::Marshalling(parcel, compressData_) &&
                   RSMarshallingHelper::Marshalling(parcel, imageFit) &&
                   RSMarshallingHelper::Marshalling(parcel, imageRepeat) &&
                   RSMarshallingHelper::Marshalling(parcel, radius_) &&
                   RSMarshallingHelper::Marshalling(parcel, scale_);
    return success;
}

RSImage* RSImage::Unmarshalling(Parcel& parcel)
{
    uint64_t uniqueId;
    int width;
    int height;
    if (!UnmarshallingIdAndSize(parcel, uniqueId, width, height)) {
        RS_LOGE("RSImage::Unmarshalling UnmarshallingIdAndSize fail");
        return nullptr;
    }
    NodeId nodeId;
    if (!RSMarshallingHelper::Unmarshalling(parcel, nodeId)) {
        RS_LOGE("RSImage::Unmarshalling nodeId fail");
        return nullptr;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, nodeId);

    bool useSkImage;
    std::shared_ptr<Drawing::Image> img;
    std::shared_ptr<Media::PixelMap> pixelMap;
    void* imagepixelAddr = nullptr;
    if (!UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSkImage, img, pixelMap, imagepixelAddr)) {
        return nullptr;
    }
    std::shared_ptr<Drawing::Data> compressData;
    bool skipData = img != nullptr || !useSkImage;
    if (!UnmarshallingCompressData(parcel, skipData, compressData)) {
        return nullptr;
    }

    int fitNum;
    if (!RSMarshallingHelper::Unmarshalling(parcel, fitNum)) {
        RS_LOGE("RSImage::Unmarshalling fitNum fail");
        return nullptr;
    }

    int repeatNum;
    if (!RSMarshallingHelper::Unmarshalling(parcel, repeatNum)) {
        RS_LOGE("RSImage::Unmarshalling repeatNum fail");
        return nullptr;
    }

    std::vector<Drawing::Point> radius(CORNER_SIZE);
    if (!RSMarshallingHelper::Unmarshalling(parcel, radius)) {
        RS_LOGE("RSImage::Unmarshalling radius fail");
        return nullptr;
    }

    double scale;
    if (!RSMarshallingHelper::Unmarshalling(parcel, scale)) {
        RS_LOGE("RSImage::Unmarshalling scale fail");
        return nullptr;
    }

    RSImage* rsImage = new RSImage();
    rsImage->SetImage(img);
    rsImage->SetImagePixelAddr(imagepixelAddr);
    rsImage->SetCompressData(compressData, uniqueId, width, height);
    rsImage->SetPixelMap(pixelMap);
    rsImage->SetImageFit(fitNum);
    rsImage->SetImageRepeat(repeatNum);
    rsImage->SetRadius(radius);
    rsImage->SetScale(scale);
    rsImage->SetNodeId(nodeId);
    rsImage->uniqueId_ = uniqueId;
    rsImage->MarkRenderServiceImage();
    RSImageBase::IncreaseCacheRefCount(uniqueId, useSkImage, pixelMap);
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
#if defined(RS_ENABLE_UNI_RENDER)
        if (pixelMap != nullptr && pixelMap->GetAllocatorType() != Media::AllocatorType::DMA_ALLOC) {
            rsImage->ConvertPixelMapToDrawingImage(true);
        }
#endif
    }
#endif
    return rsImage;
}
#endif
} // namespace Rosen
} // namespace OHOS

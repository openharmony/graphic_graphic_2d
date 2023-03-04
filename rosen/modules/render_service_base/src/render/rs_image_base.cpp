/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "render/rs_image_base.h"

#include "include/core/SkImage.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_image_cache.h"
#include "render/rs_pixel_map_util.h"
#include "rs_trace.h"
#include "sandbox_utils.h"

namespace OHOS::Rosen {
RSImageBase::~RSImageBase()
{
    if (image_) {
        image_ = nullptr;
        if (uniqueId_ > 0) {
            // if image_ is obtained by RSPixelMapUtil::ExtractSkImage, uniqueId_ here is related to pixelMap,
            // image_ is not in SkiaImageCache, but still check it here
            // in this case, the cached image_ will be removed when pixelMap cache is removed
            RSImageCache::Instance().ReleaseSkiaImageCache(uniqueId_);
        }
    }
    if (pixelMap_) {
        pixelMap_ = nullptr;
        if (uniqueId_ > 0) {
            RSImageCache::Instance().ReleasePixelMapCache(uniqueId_);
        }
    }
}

void RSImageBase::DrawImage(SkCanvas& canvas, const SkPaint& paint)
{
    ConvertPixelMapToSkImage();
    auto src = RSPropertiesPainter::Rect2SkRect(srcRect_);
    auto dst = RSPropertiesPainter::Rect2SkRect(dstRect_);
    canvas.drawImageRect(image_, src, dst, &paint);
}

void RSImageBase::SetImage(const sk_sp<SkImage> image)
{
    image_ = image;
    if (image_) {
        srcRect_.SetAll(0.0, 0.0, image_->width(), image_->height());
        GenUniqueId(image_->uniqueID());
    }
}

void RSImageBase::SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap)
{
    pixelMap_ = pixelmap;
    if (pixelMap_) {
        srcRect_.SetAll(0.0, 0.0, pixelMap_->GetWidth(), pixelMap_->GetHeight());
        image_ = nullptr;
        GenUniqueId(pixelMap_->GetUniqueId());
    }
}

void RSImageBase::SetSrcRect(const RectF& srcRect)
{
    srcRect_ = srcRect;
}

void RSImageBase::SetDstRect(const RectF& dstRect)
{
    dstRect_ = dstRect;
}

#ifdef ROSEN_OHOS
static bool UnmarshallingAndCacheSkImage(Parcel& parcel, sk_sp<SkImage>& img, uint64_t uniqueId)
{
    if (img != nullptr) {
        // match a cached SkImage
        if (!RSMarshallingHelper::SkipSkImage(parcel)) {
            RS_LOGE("UnmarshalAndCacheSkImage SkipSkImage fail");
            return false;
        }
    } else if (RSMarshallingHelper::Unmarshalling(parcel, img)) {
        // unmarshalling the SkImage and cache it
        RSImageCache::Instance().CacheSkiaImage(uniqueId, img);
    } else {
        RS_LOGE("UnmarshalAndCacheSkImage fail");
        return false;
    }
    return true;
}

static bool UnmarshallingAndCachePixelMap(Parcel& parcel, std::shared_ptr<Media::PixelMap>& pixelMap, uint64_t uniqueId)
{
    if (pixelMap != nullptr) {
        // match a cached pixelMap
        if (!RSMarshallingHelper::SkipPixelMap(parcel)) {
            return false;
        }
    } else if (RSMarshallingHelper::Unmarshalling(parcel, pixelMap)) {
        if (pixelMap && !pixelMap->IsEditable()) {
            // unmarshalling the pixelMap and cache it
            RSImageCache::Instance().CachePixelMap(uniqueId, pixelMap);
        }
    } else {
        return false;
    }
    return true;
}

static bool UnmarshallingIdAndRect(Parcel& parcel, uint64_t& uniqueId, RectF& srcRect, RectF& dstRect)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, uniqueId)) {
        RS_LOGE("RSImage::Unmarshalling uniqueId fail");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, srcRect)) {
        RS_LOGE("RSImage::Unmarshalling srcRect fail");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, dstRect)) {
        RS_LOGE("RSImage::Unmarshalling dstRect fail");
        return false;
    }
    return true;
}

bool RSImageBase::UnmarshallingSkImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useSkImage,
    sk_sp<SkImage>& img, std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, useSkImage)) {
        return false;
    }
    if (useSkImage) {
        img = RSImageCache::Instance().GetSkiaImageCache(uniqueId);
        RS_TRACE_NAME_FMT("RSImageBase::Unmarshalling skImage uniqueId:%lu, size:[%d %d], cached:%d",
            uniqueId, img ? img->width() : 0, img ? img->height() : 0, img != nullptr);
        if (!UnmarshallingAndCacheSkImage(parcel, img, uniqueId)) {
            RS_LOGE("RSImageBase::Unmarshalling UnmarshalAndCacheSkImage fail");
            return false;
        }
        RSMarshallingHelper::SkipPixelMap(parcel);
    } else {
        if (!RSMarshallingHelper::SkipSkImage(parcel)) {
            return false;
        }
        pixelMap = RSImageCache::Instance().GetPixelMapCache(uniqueId);
        RS_TRACE_NAME_FMT("RSImageBase::Unmarshalling pixelMap uniqueId:%lu, size:[%d %d], cached:%d",
            uniqueId, pixelMap ? pixelMap->GetWidth() : 0, pixelMap ? pixelMap->GetHeight() : 0, pixelMap != nullptr);
        if (!UnmarshallingAndCachePixelMap(parcel, pixelMap, uniqueId)) {
            RS_LOGE("RSImageBase::Unmarshalling UnmarshalAndCachePixelMap fail");
            return false;
        }
    }
    return true;
}

void RSImageBase::IncreaseCacheRefCount(uint64_t uniqueId, bool useSkImage, std::shared_ptr<Media::PixelMap>
    pixelMap)
{
    if (useSkImage) {
        RSImageCache::Instance().IncreaseSkiaImageCacheRefCount(uniqueId);
    } else if (pixelMap && !pixelMap->IsEditable()) {
        RSImageCache::Instance().IncreasePixelMapCacheRefCount(uniqueId);
    }
}

bool RSImageBase::Marshalling(Parcel& parcel) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = RSMarshallingHelper::Marshalling(parcel, uniqueId_) &&
                   RSMarshallingHelper::Marshalling(parcel, srcRect_) &&
                   RSMarshallingHelper::Marshalling(parcel, dstRect_) &&
                   parcel.WriteBool(pixelMap_ == nullptr) &&
                   RSMarshallingHelper::Marshalling(parcel, image_) &&
                   RSMarshallingHelper::Marshalling(parcel, pixelMap_);
    return success;
}

RSImageBase* RSImageBase::Unmarshalling(Parcel& parcel)
{
    uint64_t uniqueId;
    RectF srcRect;
    RectF dstRect;
    if (!UnmarshallingIdAndRect(parcel, uniqueId, srcRect, dstRect)) {
        RS_LOGE("RSImage::Unmarshalling UnmarshalIdAndSize fail");
        return nullptr;
    }

    bool useSkImage;
    sk_sp<SkImage> img;
    std::shared_ptr<Media::PixelMap> pixelMap;
    if (!UnmarshallingSkImageAndPixelMap(parcel, uniqueId, useSkImage, img, pixelMap)) {
        return nullptr;
    }

    RSImageBase* rsImage = new RSImageBase();
    rsImage->SetImage(img);
    rsImage->SetPixelMap(pixelMap);
    rsImage->SetSrcRect(srcRect);
    rsImage->SetDstRect(dstRect);
    rsImage->uniqueId_ = uniqueId;

    IncreaseCacheRefCount(uniqueId, useSkImage, pixelMap);
    return rsImage;
}
#endif

void RSImageBase::ConvertPixelMapToSkImage()
{
    if (!image_ && pixelMap_) {
        if (!pixelMap_->IsEditable()) {
            image_ = RSImageCache::Instance().GetRenderSkiaImageCacheByPixelMapId(uniqueId_);
        }
        if (!image_) {
            image_ = RSPixelMapUtil::ExtractSkImage(pixelMap_);
            if (!pixelMap_->IsEditable()) {
                RSImageCache::Instance().CacheRenderSkiaImageByPixelMapId(uniqueId_, image_);
            }
        }
    }
}

void RSImageBase::GenUniqueId(uint32_t id)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsignd number shift
    uniqueId_ = shiftedPid | id;
}
}
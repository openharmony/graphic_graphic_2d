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
bool RSImageBase::Marshalling(Parcel& parcel) const
{
    return false;
}

RSImageBase* RSImageBase::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}
#endif

void RSImageBase::ConvertPixelMapToSkImage()
{
    if (!image_ && pixelMap_) {
        if (!pixelMap_->IsEditable()) {
            image_ = RSImageCache::Instance().GetSkiaImageCacheByPixelMapId(uniqueId_);
        }
        if (!image_) {
            image_ = RSPixelMapUtil::ExtractSkImage(pixelMap_);
            if (!pixelMap_->IsEditable()) {
                RSImageCache::Instance().CacheSkiaImageByPixelMapId(uniqueId_, image_);
            }
        }
    }
}

void RSImageBase::GenUniqueId(uint32_t id)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsignd number shift
    uniqueId_ = shiftedPid | id;
}


void RSImageNine::DrawImage(SkCanvas& canvas, const SkPaint& paint)
{
    auto dst = RSPropertiesPainter::Rect2SkRect(dstRect_);
    canvas.drawImageNine(image_, center_, dst, &paint);
}

void RSImageNine::SetCenter(SkIRect center)
{
    center_ = center;
}

#ifdef ROSEN_OHOS
bool RSImageNine::Marshalling(Parcel& parcel) const
{
    bool success = RSImageBase::Marshalling(parcel) && RSMarshallingHelper::Marshalling(parcel, center_);
    if (!success) {
        ROSEN_LOGE("RSImageNine::Marshalling failed!");
    }
    return success;
}

RSImageNine* RSImageNine::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}
#endif
}
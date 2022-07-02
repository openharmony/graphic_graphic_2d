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

#include "sk_image_chain.h"

namespace OHOS {
namespace Rosen {
SKImageChain::SKImageChain(SkCanvas* canvas, sk_sp<SkImage> image)
{
    if (canvas != nullptr && image != nullptr) {
        canvas_ = canvas;
        image_ =  image;
    }
}
SKImageChain::SKImageChain(std::shared_ptr<Media::PixelMap> srcPixelMap) :srcPixelMap_(srcPixelMap)
{
    initWithoutCanvas_ = true;
    InitWithoutCanvas();
}

SKImageChain::~SKImageChain()
{
    if (initWithoutCanvas_) {
        delete canvas_;
        delete dstPixmap_;
    }
}

void SKImageChain::InitWithoutCanvas()
{
    if (srcPixelMap_ != nullptr) {
        auto rowBytes = srcPixelMap_->GetRowBytes();
        auto width = srcPixelMap_->GetWidth();
        auto height = srcPixelMap_->GetHeight();
        SkImageInfo imageInfo = SkImageInfo::Make(width, height,
        PixelFormatConvert(srcPixelMap_->GetPixelFormat()), static_cast<SkAlphaType>(srcPixelMap_->GetAlphaType()));
        SkPixmap srcPixmap(imageInfo, srcPixelMap_->GetPixels(), rowBytes);
        SkBitmap srcBitmap;
        srcBitmap.installPixels(srcPixmap);
        image_ = SkImage::MakeFromBitmap(srcBitmap);
        Media::InitializationOptions opts;
        opts.size.width = width;
        opts.size.height = height;
        opts.editable = true;
        auto dstPixelMap = Media::PixelMap::Create(opts);
        if (dstPixelMap != nullptr) {
            dstPixmap_ = new SkPixmap(imageInfo, dstPixelMap->GetPixels(), rowBytes);
            SkBitmap dstBitmap;
            dstBitmap.installPixels(*dstPixmap_);
            canvas_ = new SkCanvas(dstBitmap);
            dstPixelMap_ = std::shared_ptr<Media::PixelMap>(dstPixelMap.release());
        }
    }
}

void SKImageChain::SetFilters(sk_sp<SkImageFilter> filter)
{
    if (filters_ == nullptr) {
        filters_ = filter;
    } else {
        filters_ = SkImageFilters::Compose(filter, filters_);
    }
}

void SKImageChain::SetClipRect(SkRect* rect)
{
    rect_ = rect;
}

void SKImageChain::SetClipPath(SkPath* path)
{
    path_ = path;
}

void SKImageChain::SetClipRRect(SkRRect* rRect)
{
    rRect_ = rRect;
}

std::shared_ptr<Media::PixelMap> SKImageChain::GetPixelMap()
{
    return dstPixelMap_;
}

void SKImageChain::Draw()
{
    if (canvas_ == nullptr) {
        return;
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setBlendMode(SkBlendMode::kSrc);
    paint.setImageFilter(filters_);
    if (rect_ != nullptr) {
        canvas_->clipRect((*rect_), true);
    } else if (path_ != nullptr) {
        canvas_->clipPath(*path_, true);
    } else if (rRect_ != nullptr) {
        canvas_->clipRRect(*rRect_, true);
    }
    canvas_->save();
    canvas_->resetMatrix();
    canvas_->drawImage(image_.get(), 0, 0, &paint);
    canvas_->restore();
}

SkColorType SKImageChain::PixelFormatConvert(const Media::PixelFormat& pixelFormat)
{
    SkColorType colorType;
    switch (pixelFormat) {
        case Media::PixelFormat::BGRA_8888:
            colorType = SkColorType::kBGRA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGBA_8888:
            colorType = SkColorType::kRGBA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGB_565:
            colorType = SkColorType::kRGB_565_SkColorType;
            break;
        case Media::PixelFormat::ALPHA_8:
            colorType = SkColorType::kAlpha_8_SkColorType;
            break;
        default:
            colorType = SkColorType::kUnknown_SkColorType;
            break;
    }
    return colorType;
}
} // namespcae Rosen
} // namespace OHOS
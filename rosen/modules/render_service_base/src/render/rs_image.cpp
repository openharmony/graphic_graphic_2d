/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "include/core/SkPaint.h"
#include "include/core/SkRRect.h"

namespace OHOS {
namespace Rosen {
SkRect Rect2SkRect(const RectF& r);
SkRRect RRect2SkRRect(const RRect& rr);

void RSImage::CanvasDrawImage(SkCanvas& canvas, const SkRect& rect, const SkPaint& paint, bool isBackground)
{
    canvas.save();
    frameRect_.SetAll(rect.left(), rect.top(), rect.width(), rect.height());
    if (!isBackground) {
        ApplyImageFit();
        ApplyCanvasClip(canvas);
    }
    DrawImageRepeatRect(paint, canvas);
    canvas.restore();
}

void RSImage::ApplyImageFit()
{
    const float srcW = srcRect_.width_ / scale_;
    const float srcH = srcRect_.height_ / scale_;
    const float frameW = frameRect_.width_;
    const float frameH = frameRect_.height_;
    float dstW = frameW;
    float dstH = frameH;
    float ratio = srcW / srcH;
    switch (imageFit_) {
        case ImageFit::FILL:
            break;
        case ImageFit::NONE:
            dstW = srcW;
            dstH = srcH;
            break;
        case ImageFit::COVER:
            dstW = std::max(frameW, frameH * ratio);
            dstH = std::max(frameH, frameW / ratio);
            break;
        case ImageFit::FIT_WIDTH:
            dstH = frameW / ratio;
            break;
        case ImageFit::FIT_HEIGHT:
            dstW = frameH * ratio;
            break;
        case ImageFit::SCALE_DOWN:
            if (srcW < frameW && srcH < frameH) {
                dstW = srcW;
                dstH = srcH;
            } else {
                dstW = std::min(frameW, frameH * ratio);
                dstH = std::min(frameH, frameW / ratio);
            }
            break;
        case ImageFit::CONTAIN:
        default:
            dstW = std::min(frameW, frameH * ratio);
            dstH = std::min(frameH, frameW / ratio);
            break;
    }
    dstRect_.SetAll((frameW - dstW) / 2, (frameH - dstH) / 2, dstW, dstH);
}

void RSImage::ApplyCanvasClip(SkCanvas& canvas)
{
    if (imageRepeat_ == ImageRepeat::NO_REPEAT) {
        // clip dst & frame intersect rect
        RRect rrect = RRect(dstRect_.IntersectRect(frameRect_), cornerRadius_, cornerRadius_);
        canvas.clipRRect(RRect2SkRRect(rrect), true);
    } else {
        // clip frame rect
        RRect rrect = RRect(frameRect_, cornerRadius_, cornerRadius_);
        canvas.clipRRect(RRect2SkRRect(rrect), true);
    }
}

void RSImage::DrawImageRepeatRect(const SkPaint& paint, SkCanvas& canvas)
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
    auto src = Rect2SkRect(srcRect_);
    for (int i = minX; i <= maxX; ++i) {
        for (int j = minY; j <= maxY; ++j) {
            auto dst = SkRect::MakeXYWH(dstRect_.left_ + i * dstRect_.width_, dstRect_.top_ + j * dstRect_.height_,
                dstRect_.width_, dstRect_.height_);
            canvas.drawImageRect(image_, src, dst, &paint, SkCanvas::kFast_SrcRectConstraint);
        }
    }
}

void RSImage::SetImage(const sk_sp<SkImage> image)
{
    image_ = image;
    if (image_) {
        srcRect_.SetAll(0.0, 0.0, image_->width(), image_->height());
    }
}

void RSImage::SetDstRect(const RectF& dstRect)
{
    dstRect_ = dstRect;
}

void RSImage::SetImageFit(int fitNum)
{
    imageFit_ = static_cast<ImageFit>(fitNum);
}

void RSImage::SetImageRepeat(int repeatNum)
{
    imageRepeat_ = static_cast<ImageRepeat>(repeatNum);
}

void RSImage::SetRadius(float radius)
{
    cornerRadius_ = radius;
}

void RSImage::SetScale(double scale)
{
    if (scale > 0.0) {
        scale_ = scale;
    }
}

#ifdef ROSEN_OHOS
bool RSImage::Marshalling(Parcel& parcel) const
{
    bool success = true;
    int imageFit = static_cast<int>(imageFit_);
    int imageRepeat = static_cast<int>(imageRepeat_);
    success &= RSMarshallingHelper::Marshalling(parcel, image_);
    success &= RSMarshallingHelper::Marshalling(parcel, imageFit);
    success &= RSMarshallingHelper::Marshalling(parcel, imageRepeat);
    success &= RSMarshallingHelper::Marshalling(parcel, cornerRadius_);
    return success;
}
RSImage* RSImage::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkImage> img;
    int fitNum;
    int repeatNum;
    float radius;
    if (!RSMarshallingHelper::Unmarshalling(parcel, img)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, fitNum)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, repeatNum)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, radius)) {
        return nullptr;
    }

    RSImage* rsImage = new RSImage();
    rsImage->SetImage(img);
    rsImage->SetImageFit(fitNum);
    rsImage->SetImageRepeat(repeatNum);
    rsImage->SetRadius(radius);

    return rsImage;
}
#endif
} // namespace Rosen
} // namespace OHOS

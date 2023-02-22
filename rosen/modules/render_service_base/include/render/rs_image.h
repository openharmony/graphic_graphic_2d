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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_H

#include <cstdint>
#include <mutex>
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkImage.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RsImageInfo final {
public:
    RsImageInfo(int fitNum, int repeatNum, const SkVector* radius, double scale, uint32_t id, int w, int h)
        : fitNum_(fitNum), repeatNum_(repeatNum), radius_(radius), scale_(scale),
          uniqueId_(id), width_(w), height_(h) {};
    ~RsImageInfo() {}
    int fitNum_ = 0;
    int repeatNum_ = 0;
    const SkVector* radius_;
    double scale_ = 0.0;
    uint32_t uniqueId_ = 0;
    int width_ = 0;
    int height_ = 0;
};

enum class ImageRepeat {
    NO_REPEAT = 0,
    REPEAT_X,
    REPEAT_Y,
    REPEAT,
};

enum class ImageFit {
    FILL,
    CONTAIN,
    COVER,
    FIT_WIDTH,
    FIT_HEIGHT,
    NONE,
    SCALE_DOWN,
    TOP_LEFT,
};

class RSB_EXPORT RSImage {
public:
    RSImage() = default;
    ~RSImage();

    bool IsEqual(const RSImage& other) const;
    void CanvasDrawImage(SkCanvas& canvas, const SkRect& rect, const SkPaint& paint, bool isBackground = false);
    void SetImage(const sk_sp<SkImage> image);
    void SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap);
    void SetDstRect(const RectF& dstRect);
    void SetImageFit(int fitNum);
    void SetImageRepeat(int repeatNum);
    void SetRadius(const SkVector radius[]);
    void SetScale(double scale);
    void SetCompressData(const sk_sp<SkData> data, uint32_t id, int width, int height);
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const;
    static RSImage* Unmarshalling(Parcel& parcel);
#endif

private:
    void ApplyImageFit();
    void ApplyCanvasClip(SkCanvas& canvas);
    void DrawImageRepeatRect(const SkPaint& paint, SkCanvas& canvas);
    void UploadGpu(SkCanvas& canvas);

    mutable std::mutex mutex_;
    sk_sp<SkImage> image_;
    sk_sp<SkData> compressData_;
    std::shared_ptr<Media::PixelMap> pixelmap_;

    ImageFit imageFit_ = ImageFit::COVER;
    ImageRepeat imageRepeat_ = ImageRepeat::NO_REPEAT;
    SkVector radius_[4];
    RectF srcRect_;
    RectF dstRect_;
    RectF frameRect_;
    double scale_ = 1.0;
    uint64_t uniqueId_;
};

template<>
inline bool ROSEN_EQ(const std::shared_ptr<RSImage>& x, const std::shared_ptr<RSImage>& y)
{
    if (x == y) {
        return true;
    }
    return (x && y) ? x->IsEqual(*y) : false;
}
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_H

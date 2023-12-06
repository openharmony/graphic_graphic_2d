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

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkColorFilter.h"
#include "include/core/SkImage.h"
#else
#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "image/image.h"
#endif
#include "render/rs_image_base.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RsImageInfo final {
public:
#ifndef USE_ROSEN_DRAWING
    RsImageInfo(int fitNum, int repeatNum, const SkVector* radius, double scale, uint32_t id, int w, int h)
        : fitNum_(fitNum), repeatNum_(repeatNum), radius_(radius), scale_(scale),
          uniqueId_(id), width_(w), height_(h) {};
#else
    RsImageInfo(int fitNum, int repeatNum, const Drawing::Point* radius, double scale, uint32_t id, int w, int h)
        : fitNum_(fitNum), repeatNum_(repeatNum), radius_(radius), scale_(scale),
          uniqueId_(id), width_(w), height_(h) {};
#endif
    ~RsImageInfo() {}
    int fitNum_ = 0;
    int repeatNum_ = 0;
#ifndef USE_ROSEN_DRAWING
    const SkVector* radius_;
#else
    const Drawing::Point* radius_;
#endif
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

class RSB_EXPORT RSImage : public RSImageBase {
public:
    RSImage() = default;
    ~RSImage();

    bool IsEqual(const RSImage& other) const;
#ifndef USE_ROSEN_DRAWING
    void CanvasDrawImage(RSPaintFilterCanvas& canvas, const SkRect& rect, const SkSamplingOptions& samplingOptions,
        const SkPaint& paint, bool isBackground = false);
#else
    void CanvasDrawImage(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        const Drawing::SamplingOptions& samplingOptions, bool isBackground = false);
#endif
    void SetImageFit(int fitNum);
    void SetImageRepeat(int repeatNum);
#ifndef USE_ROSEN_DRAWING
    void SetRadius(const SkVector radius[]);
#else
    void SetRadius(const std::vector<Drawing::Point>& radius);
#endif
    void SetScale(double scale);

#ifndef USE_ROSEN_DRAWING
    void SetCompressData(const sk_sp<SkData> data, uint32_t id, int width, int height);
#else
    void SetCompressData(const std::shared_ptr<Drawing::Data> data, uint32_t id, int width, int height);
#endif
#ifndef USE_ROSEN_DRAWING
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    void SetCompressData(const sk_sp<SkData> compressData);
#endif
#else
    void SetCompressData(const std::shared_ptr<Drawing::Data> compressData);
#endif

    void SetNodeId(NodeId nodeId);
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSImage* Unmarshalling(Parcel& parcel);
#endif
    void dump(std::string &desc, int depth) const
    {
        std::string split(depth, '\t');
        desc += split + "RSImage:{";
        desc += split + "\timageFit_: " + std::to_string(static_cast<int>(imageFit_)) + "\n";
        desc += split + "\timageRepeat_: " + std::to_string(static_cast<int>(imageRepeat_)) + "\n";
        int radiusSize = 4;
        for (int i = 0; i < radiusSize; i++) {
#ifndef USE_ROSEN_DRAWING
            radius_[i].dump(desc, depth + 1);
#else
            desc += split + "\tPointF:{ \n";
            desc += split + "\t\t x_: " + std::to_string(radius_[i].GetX()) + "\n";
            desc += split + "\t\t y_: " + std::to_string(radius_[i].GetY()) + "\n";
            desc += split + "\t}\n";
#endif
        }
        desc += split + frameRect_.ToString();
        desc += split + "\tscale_: " + std::to_string(scale_) + "\n";
        desc += split + "}\n";
    }

private:
    bool HasRadius() const;
    void ApplyImageFit();
#ifndef USE_ROSEN_DRAWING
    void ApplyCanvasClip(RSPaintFilterCanvas& canvas);
    void UploadGpu(RSPaintFilterCanvas& canvas);
    void DrawImageRepeatRect(const SkSamplingOptions& samplingOptions, const SkPaint& paint,
        RSPaintFilterCanvas& canvas);
#else
    void ApplyCanvasClip(Drawing::Canvas& canvas);
    void UploadGpu(Drawing::Canvas& canvas);
    void DrawImageRepeatRect(const Drawing::SamplingOptions& samplingOptions, Drawing::Canvas& canvas);
#endif

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkData> compressData_;
#else
    std::shared_ptr<Drawing::Data> compressData_;
#endif
    ImageFit imageFit_ = ImageFit::COVER;
    ImageRepeat imageRepeat_ = ImageRepeat::NO_REPEAT;
#ifndef USE_ROSEN_DRAWING
    SkVector radius_[4];
#else
    std::vector<Drawing::Point> radius_ = std::vector<Drawing::Point>(4);
#endif
    bool hasRadius_ = false;
    RectF frameRect_;
    double scale_ = 1.0;
    NodeId nodeId_ = 0;
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

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

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "image/image.h"
#include "render/rs_image_base.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RsImageInfo final {
public:
    RsImageInfo(int fitNum, int repeatNum, const Drawing::Point* radius, double scale, uint32_t id, int w, int h)
        : fitNum_(fitNum), repeatNum_(repeatNum), radius_(radius), scale_(scale),
          uniqueId_(id), width_(w), height_(h) {};
    ~RsImageInfo() {}
    int fitNum_ = 0;
    int repeatNum_ = 0;
    const Drawing::Point* radius_;
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
    COVER_TOP_LEFT,
};

class RSB_EXPORT RSImage : public RSImageBase {
public:
    RSImage() = default;
    ~RSImage();

    bool IsEqual(const RSImage& other) const;
    void CanvasDrawImage(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        const Drawing::SamplingOptions& samplingOptions, bool isBackground = false);
    void SetImageFit(int fitNum);
    void SetImageRepeat(int repeatNum);
    void SetRadius(const std::vector<Drawing::Point>& radius);
    void SetScale(double scale);
    void SetInnerRect(const std::optional<Drawing::RectI>& innerRect) { innerRect_ = innerRect;}

    void SetCompressData(const std::shared_ptr<Drawing::Data> data, uint32_t id, int width, int height);
    void SetCompressData(const std::shared_ptr<Drawing::Data> compressData);

    bool HDRConvert(const Drawing::SamplingOptions& sampling, Drawing::Canvas& canvas);
    void SetPaint(Drawing::Paint paint);
    void SetDyamicRangeMode(uint32_t dynamicRangeMode);

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
            desc += split + "\tPointF:{ \n";
            desc += split + "\t\t x_: " + std::to_string(radius_[i].GetX()) + "\n";
            desc += split + "\t\t y_: " + std::to_string(radius_[i].GetY()) + "\n";
            desc += split + "\t}\n";
        }
        desc += split + frameRect_.ToString();
        desc += split + "\tscale_: " + std::to_string(scale_) + "\n";
        desc += split + "}\n";
    }

private:
    bool HasRadius() const;
    void ApplyImageFit();
    void ApplyCanvasClip(Drawing::Canvas& canvas);
    void UploadGpu(Drawing::Canvas& canvas);
    void DrawImageRepeatRect(const Drawing::SamplingOptions& samplingOptions, Drawing::Canvas& canvas);
    void CalcRepeatBounds(int& minX, int& maxX, int& minY, int& maxY);
    void DrawImageOnCanvas(
        const Drawing::SamplingOptions& samplingOptions, Drawing::Canvas& canvas, const bool hdrImageDraw);
#ifdef ROSEN_OHOS
    static bool UnmarshalIdSizeAndNodeId(Parcel& parcel, uint64_t& uniqueId, int& width, int& height, NodeId& nodeId);
    static bool UnmarshalImageProperties(
        Parcel& parcel, int& fitNum, int& repeatNum, std::vector<Drawing::Point>& radius, double& scale);
    static void ProcessImageAfterCreation(RSImage* rsImage, const uint64_t uniqueId, const bool useSkImage,
        const std::shared_ptr<Media::PixelMap>& pixelMap);
#endif
    std::shared_ptr<Drawing::Data> compressData_;
    ImageFit imageFit_ = ImageFit::COVER;
    ImageRepeat imageRepeat_ = ImageRepeat::NO_REPEAT;
    std::vector<Drawing::Point> radius_ = std::vector<Drawing::Point>(4);
    std::optional<Drawing::RectI> innerRect_ = std::nullopt;
    bool hasRadius_ = false;
    RectF frameRect_;
    double scale_ = 1.0;
    NodeId nodeId_ = 0;
    Drawing::Paint paint_;
    uint32_t dynamicRangeMode_ = 0;
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

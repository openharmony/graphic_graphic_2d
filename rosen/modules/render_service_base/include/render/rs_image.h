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

#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_IMAGE_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_IMAGE_H

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "image/image.h"
#include "render/rs_image_base.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
namespace Drawing {
struct AdaptiveImageInfo {
    int32_t fitNum = 0;
    int32_t repeatNum = 0;
    Point radius[4];
    double scale = 0.0;
    uint32_t uniqueId = 0;
    int32_t width = 0;
    int32_t height = 0;
    uint32_t dynamicRangeMode = 0;
    int32_t rotateDegree = 0;
    Rect frameRect = Rect();
    Drawing::Matrix fitMatrix = Drawing::Matrix();
    int32_t orientationNum = 0;
};
}

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
    TOP,
    TOP_RIGHT,
    LEFT,
    CENTER,
    RIGHT,
    BOTTOM_LEFT,
    BOTTOM,
    BOTTOM_RIGHT,
    COVER_TOP_LEFT,
    MATRIX,
};

enum class OrientationFit : int {
    NONE,
    VERTICAL_FLIP,
    HORIZONTAL_FLIP,
};

class RSB_EXPORT RSImage : public RSImageBase {
public:
    RSImage() = default;
    ~RSImage();

    using RSImageBase::SetCompressData;

    bool IsEqual(const RSImage& other) const;
    void CanvasDrawImage(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        const Drawing::SamplingOptions& samplingOptions, bool isBackground = false);
    void SetImageFit(int fitNum);
    void SetImageRepeat(int repeatNum);
    void SetImageRotateDegree(int32_t degree);
    void SetRadius(const std::vector<Drawing::Point>& radius);
    void SetScale(double scale);
    void SetInnerRect(const std::optional<Drawing::RectI>& innerRect) { innerRect_ = innerRect;}

    void SetCompressData(const std::shared_ptr<Drawing::Data> data, uint32_t id, int width, int height);

    bool HDRConvert(const Drawing::SamplingOptions& sampling, Drawing::Canvas& canvas);
    void SetPaint(Drawing::Paint paint);
    void SetDynamicRangeMode(uint32_t dynamicRangeMode);

    void SetNodeId(NodeId nodeId);

    void ApplyImageFit();
    ImageFit GetImageFit();
    Drawing::AdaptiveImageInfo GetAdaptiveImageInfoWithCustomizedFrameRect(const Drawing::Rect& frameRect) const;
    RectF GetDstRect();
    void SetFrameRect(RectF frameRect);
    void SetFitMatrix(const Drawing::Matrix& matrix);
    Drawing::Matrix GetFitMatrix() const;
    std::shared_ptr<Drawing::Image> GetImage() const;
    void SetOrientationFit(int orientationFitNum);
    OrientationFit GetOrientationFit() const;
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSImage* Unmarshalling(Parcel& parcel);
#endif
    std::string PixelSamplingDump() const;
    void Dump(std::string &desc, uint8_t depth) const
    {
        std::string split(depth, '\t');
        desc += split + "RSImage:{ ";
        desc += split + "\timageFit_: " + std::to_string(static_cast<int>(imageFit_)) + " \n";
        desc += split + "\timageRepeat_: " + std::to_string(static_cast<int>(imageRepeat_)) + " \n";
        desc += split + "\torientationFit_: " + std::to_string(static_cast<int>(orientationFit_)) + " \n";

        int radiusSize = 4;
        for (int i = 0; i < radiusSize; i++) {
            desc += split + "\tPointF:{ \n";
            desc += split + "\t\t x_: " + std::to_string(radius_[i].GetX()) + " \n";
            desc += split + "\t\t y_: " + std::to_string(radius_[i].GetY()) + " \n";
            desc += split + "\t} \n";
        }
        desc += split + frameRect_.ToString();
        desc += split + "\tscale_: " + std::to_string(scale_) + " \n";
        desc += split + "\tsrc_: " + src_.ToString() + " \n";
        desc += split + "\tdst_: " + dst_.ToString() + " \n";
        desc += split + "\tpixel sampling: " + PixelSamplingDump() + " \n";
        desc += split + "} \n";
    }

private:
    bool HasRadius() const;
    void ApplyCanvasClip(Drawing::Canvas& canvas);
    std::pair<float, float> CalculateByDegree(const Drawing::Rect& rect);
    void DrawImageRect(
            Drawing::Canvas& canvas, const Drawing::Rect& rect, const Drawing::SamplingOptions& samplingOptions);
    void DrawImageRepeatRect(const Drawing::SamplingOptions& samplingOptions, Drawing::Canvas& canvas);
    void CalcRepeatBounds(int& minX, int& maxX, int& minY, int& maxY);
    void DrawImageOnCanvas(
        const Drawing::SamplingOptions& samplingOptions, Drawing::Canvas& canvas, const bool hdrImageDraw);
    bool CanDrawRectWithImageShader(const Drawing::Canvas& canvas) const;
    std::shared_ptr<Drawing::ShaderEffect> GenerateImageShaderForDrawRect(
        const Drawing::Canvas& canvas, const Drawing::SamplingOptions& sampling) const;
    void DrawImageShaderRectOnCanvas(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::ShaderEffect>& imageShader) const;
    void DrawImageWithFirMatrixRotateOnCanvas(
        const Drawing::SamplingOptions& samplingOptions, Drawing::Canvas& canvas) const;
    void ApplyImageOrientation(Drawing::Canvas& canvas);
#ifdef ROSEN_OHOS
    static bool UnmarshalIdSizeAndNodeId(Parcel& parcel, uint64_t& uniqueId, int& width, int& height, NodeId& nodeId);
    static bool UnmarshalImageProperties(
        Parcel& parcel, int& fitNum, int& repeatNum, std::vector<Drawing::Point>& radius, double& scale,
        bool& hasFitMatrix, Drawing::Matrix& fitMatrix, uint32_t& dynamicRangeMode, int32_t& degree,
        int& orientationFitNum);
    static void ProcessImageAfterCreation(RSImage* rsImage, const uint64_t uniqueId, const bool useSkImage,
        const std::shared_ptr<Media::PixelMap>& pixelMap);
#endif
    ImageFit imageFit_ = ImageFit::COVER;
    ImageRepeat imageRepeat_ = ImageRepeat::NO_REPEAT;
    std::vector<Drawing::Point> radius_ = std::vector<Drawing::Point>(4);
    std::optional<Drawing::RectI> innerRect_ = std::nullopt;
    bool hasRadius_ = false;
    RectF frameRect_;
    double scale_ = 1.0;
    NodeId nodeId_ = 0;
    int32_t rotateDegree_;
    Drawing::Paint paint_;
    uint32_t dynamicRangeMode_ = 0;
    std::optional<Drawing::Matrix> fitMatrix_ = std::nullopt;
    bool isFitMatrixValid_ = false;
    OrientationFit orientationFit_ = OrientationFit::NONE;
    bool isOrientationValid_ = false;
    Drawing::Rect rectForDrawShader_;
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
#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_IMAGE_H

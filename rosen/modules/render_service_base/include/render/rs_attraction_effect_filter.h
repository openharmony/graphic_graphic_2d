/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_ATTRACTION_EFFECT_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_ATTRACTION_EFFECT_FILTER_H

#include <memory>

#include "common/rs_rect.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSAttractionEffectFilter : public RSDrawingFilterOriginal {
public:
    RSAttractionEffectFilter(float attractionFraction);
    RSAttractionEffectFilter(const RSAttractionEffectFilter&) = delete;
    RSAttractionEffectFilter operator=(const RSAttractionEffectFilter&) = delete;
    ~RSAttractionEffectFilter() override;

    bool IsValid() const override;
    std::string GetDescription() override;
    Drawing::Brush GetBrush(const std::shared_ptr<Drawing::Image>& image) const;
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
    void PostProcess(Drawing::Canvas& canvas) override {};
    float GetAttractionFraction() const;
    void UpdateDirtyRegion(float leftPoint, float topPonit);
    RectI GetAttractionDirtyRegion() const;

    std::shared_ptr<RSDrawingFilterOriginal> Compose(
        const std::shared_ptr<RSDrawingFilterOriginal>& other) const override
    {
        return nullptr;
    }

    void CalculateWindowStatus(float canvasWidth, float canvasHeight, Vector2f destinationPoint);
    void GetWindowInitCtrlPoints(Drawing::Point windowCtrlPoints[], float canvasWidth, float canvasHeight, int size);
    void CalculateDeltaXAndDeltaY(const Drawing::Point windowCtrlPoints[], const Drawing::Point &pointDst,
        float &deltaX, float &deltaY, int location);
    void CalculateBezierVelList(const std::vector<Drawing::Point> &velocityList,
        std::vector<Drawing::Point> &velocityCtrl, float location);
    float CalculateCubic(float p1, float p2, float t);
    bool IsWithinThreshold(const float left, const float right, const float threshold);
    float BinarySearch(float targetX, const Drawing::Point &p1, const Drawing::Point &p2);

    std::vector<Drawing::Point> CalculateCubicsCtrlPointOffset(const std::vector<Drawing::Point> controlPointOfVertex);
    std::vector<int> CreateIndexSequence(float location);
    std::vector<Drawing::Point> CalculateCubicsCtrlPoint(std::vector<Drawing::Point> controlPointOfVertex,
        const Drawing::Point points[], float location, bool isFirstCtrl);

    Drawing::Point LerpPoint(const Drawing::Point& firstPoint, const Drawing::Point& secondPoint,
        float firstFactor, float secondFactor);
    Drawing::Point CubicBezier(const Drawing::Point &p0, const Drawing::Point &p1, const Drawing::Point &p2,
        const Drawing::Point &p3, float t);

    std::vector<Drawing::Point> CalculateVelocityCtrlPointUpper();
    std::vector<Drawing::Point> CalculateVelocityCtrlPointLower();
    std::vector<Drawing::Point> CalculateUpperCtrlPointOfVertex(float deltaX, float deltaY, float width, float height,
        int location);
    std::vector<Drawing::Point> CalculateLowerCtrlPointOfVertex(float deltaX, float deltaY, float width, float height,
        int location);

private:
    float attractionFraction_ = 0.0f;
    int location_ = 0;
    Drawing::Point windowStatusPoints_[12] = {
        {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
        {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}
    };;
    float canvasWidth_ = 0.0f;
    float canvasHeight_ = 0.0f;
    RectI attractionDirtyRegion_ = {0, 0, 0, 0};
    bool isBelowTarget_ = false;

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_ATTRACTION_EFFECT_FILTER_H
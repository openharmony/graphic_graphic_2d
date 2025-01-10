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

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "render/rs_attraction_effect_filter.h"
#include "src/core/SkOpts.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
RSAttractionEffectFilter::RSAttractionEffectFilter(float attractionFraction)
    : RSDrawingFilterOriginal(nullptr), attractionFraction_(attractionFraction)
{
    type_ = FilterType::ATTRACTION_EFFECT;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&attractionFraction_, sizeof(attractionFraction_), hash_);
}

RSAttractionEffectFilter::~RSAttractionEffectFilter() = default;

std::string RSAttractionEffectFilter::GetDescription()
{
    return "RSAttractionEffectFilter " +  std::to_string(attractionFraction_);
}

bool RSAttractionEffectFilter::IsValid() const
{
    constexpr float epsilon = 0.001f;
    return attractionFraction_ > epsilon;
}

float RSAttractionEffectFilter::GetAttractionFraction() const
{
    return attractionFraction_;
}

RectI RSAttractionEffectFilter::GetAttractionDirtyRegion() const
{
    return attractionDirtyRegion_;
}

bool RSAttractionEffectFilter::IsWithinThreshold(const float left, const float right, const float threshold)
{
    return (std::abs(left - right) <= threshold);
}

Drawing::Point RSAttractionEffectFilter::LerpPoint(const Drawing::Point &firstPoint, const Drawing::Point &secondPoint,
    float firstFactor, float secondFactor)
{
    Drawing::Point p = { 0.0f, 0.0f };
    p = firstFactor * firstPoint + secondFactor * secondPoint;
    return p;
}

Drawing::Point RSAttractionEffectFilter::CubicBezier(const Drawing::Point &p0, const Drawing::Point &p1,
    const Drawing::Point &p2, const Drawing::Point &p3, float t)
{
    // p(t) = (1 - t)^3 * p0 + 3 * t * (1 - t)^2 * p1 + 3 * t^2 * (1-t) * p2 + t^3 * p3
    float tSquared = t * t;
    float tCubed = t * t * t;
    float u = 1 - t;
    float uSquared = u * u;
    float uCubed = u * u * u;
    
    Drawing::Point p = { 0.0f, 0.0f };
    float besselCoefficient = 3.0f;
    p = p0 * uCubed + p1 * besselCoefficient * uSquared * t + p2 * besselCoefficient * u * tSquared + p3 * tCubed;
    return p;
}

float RSAttractionEffectFilter::CalculateCubic(float p1, float p2, float t)
{
    // p(t) = 3 * t * (1 - t)^2 * p1 + 3 * t^2 * (1-t) * p2 + t^3
    return 3.0f * p1 * (1.0f - t) * (1.0f - t) * t + 3.0f * p2 * (1.0f - t) * t * t + t * t * t;
}

float RSAttractionEffectFilter::BinarySearch(float targetX, const Drawing::Point &p1, const Drawing::Point &p2)
{
    float start = 0.0f;
    float end = 1.0f;
    float midPoint = 0.0f;
    int maxIterCount = 100;
    int currentIter = 0;
    while (currentIter < maxIterCount) {
        midPoint = (start + end) / 2.0f;
        float estimate = CalculateCubic(p1.GetX(), p2.GetX(), midPoint);
        if (IsWithinThreshold(targetX, estimate, 0.001f)) {
            return CalculateCubic(p1.GetY(), p2.GetY(), midPoint);
        }
        if (estimate < targetX) {
            start = midPoint;
        } else {
            end = midPoint;
        }
        currentIter++;
    }
    return CalculateCubic(p1.GetY(), p2.GetY(), midPoint);
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateCubicsCtrlPointOffset(
    const std::vector<Drawing::Point> controlPointOfVertex)
{
    std::vector<Drawing::Point> pathList;
    Drawing::Point topLeft = controlPointOfVertex[0];
    Drawing::Point topRight = controlPointOfVertex[1];
    Drawing::Point bottomLeft = controlPointOfVertex[2];
    Drawing::Point bottomRight = controlPointOfVertex[3];

    // The two numbers represent the weights of the two points
    const float firstPointWeight = 0.33f;
    const float secondPointWeight = 0.67f;
    pathList.push_back(topLeft);                                                                 // 0:top left
    pathList.push_back(LerpPoint(topLeft, topRight, secondPointWeight, firstPointWeight));       // 1:top one third
    pathList.push_back(LerpPoint(topLeft, topRight, firstPointWeight, secondPointWeight));       // 2:top two thirds
    pathList.push_back(topRight);                                                                // 3:top right
    pathList.push_back(LerpPoint(topRight, bottomRight, secondPointWeight, firstPointWeight));   // 4:right one third
    pathList.push_back(LerpPoint(topRight, bottomRight, firstPointWeight, secondPointWeight));   // 5:right two thirds
    pathList.push_back(bottomRight);                                                             // 6:bottom right
    pathList.push_back(LerpPoint(bottomLeft, bottomRight, firstPointWeight, secondPointWeight)); // 7:bottom two thirds
    pathList.push_back(LerpPoint(bottomLeft, bottomRight, secondPointWeight, firstPointWeight)); // 8:bottom one third
    pathList.push_back(bottomLeft);                                                              // 9:bottom left
    pathList.push_back(LerpPoint(topLeft, bottomLeft, firstPointWeight, secondPointWeight));     // 10:left two thirds
    pathList.push_back(LerpPoint(topLeft, bottomLeft, secondPointWeight, firstPointWeight));     // 11:left one third
    return pathList;
}

std::vector<int> RSAttractionEffectFilter::CreateIndexSequence(int direction)
{
    // Select the parameter index of the window control point according to the window position.
    // 0 to 11 indicate the index of the window control point, which is rotated clockwise.
    if (!isBelowTarget_) {
        return direction == 1 ?
            std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } :
            std::vector<int>{ 3, 2, 1, 0, 11, 10, 9, 8, 7, 6, 5, 4 };
    } else {
        return direction == 1 ?
            std::vector<int>{ 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8 } :
            std::vector<int>{ 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    }
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateCubicsCtrlPoint(
    std::vector<Drawing::Point> controlPointOfVertex,
    const Drawing::Point points[],          // Array instead of vector
    int direction,                         // Determine whether the flag on the left or right is used.
    bool isFirstCtrl)
{
    std::vector<Drawing::Point> pathList = CalculateCubicsCtrlPointOffset(controlPointOfVertex);

    std::vector<int> indice = CreateIndexSequence(direction);
    std::vector<Drawing::Point> pathCtrlPointList(POINT_NUM, Drawing::Point(0.0f, 0.0f));

    for (int i = 0; i < POINT_NUM; i++) {
        int index = indice[i];
        if (!isBelowTarget_) {
            pathCtrlPointList[i] = pathList[index] +  (isFirstCtrl ? points[i] : points[0]);
            continue;
        }

        float px = isFirstCtrl ? points[i].GetX() : points[0].GetX();
        float py = isFirstCtrl ? points[i].GetY() : points[0].GetY();

        if (direction == 1) {
            px = -px;
            pathCtrlPointList[i] = { -(pathList[index].GetY() +  px), pathList[index].GetX() +  py };
        } else {
            py = -py;
            pathCtrlPointList[i] = { pathList[index].GetY() +  px, -(pathList[index].GetX() +  py) };
        }
    }
    return pathCtrlPointList;
}

void RSAttractionEffectFilter::CalculateBezierVelList(const std::vector<Drawing::Point> &velocityList,
    std::vector<Drawing::Point> &velocityCtrl, int direction)
{
    std::vector<Drawing::Point> curveVelList;
    Drawing::Point topVelocity = velocityList[0];
    Drawing::Point bottomVelocity = velocityList[1];

    // The two numbers represent the weights of the two points
    // Weights are obtained through regression of a large amount of data.
    const float lerpPointWeight1 = 0.17f;
    const float lerpPointWeight2 = 0.33f;
    const float lerpPointWeight3 = 0.5f;
    const float lerpPointWeight4 = 0.67f;
    const float lerpPointWeight5 = 0.83f;
    const float lerpPointWeight6 = 0.57f;

    curveVelList.push_back(LerpPoint(topVelocity, bottomVelocity, lerpPointWeight3, lerpPointWeight3));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight2, lerpPointWeight4));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight1, lerpPointWeight5));
    curveVelList.push_back(topVelocity);
    // The window is located below the dock bar and uses additional parameters.
    if (isBelowTarget_) {
        curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight1, lerpPointWeight4));
        curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight2, lerpPointWeight6));
    } else {
        curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight1, lerpPointWeight5));
        curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight2, lerpPointWeight4));
    }
    curveVelList.push_back(LerpPoint(topVelocity, bottomVelocity, lerpPointWeight3, lerpPointWeight3));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight4, lerpPointWeight2));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight5, lerpPointWeight1));
    curveVelList.push_back(bottomVelocity);
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight5, lerpPointWeight1));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, lerpPointWeight4, lerpPointWeight2));
    std::vector<int> indice = CreateIndexSequence(direction);

    for (int i = 0; i < POINT_NUM; i++) {
        int index = indice[i];
        velocityCtrl[i] = curveVelList[index];
    }
}

void RSAttractionEffectFilter::InitWindowInitCtrlPoints()
{
    // Find the third division point
    float third = 1.0f / 3.0f;
    float twoThirds = 2.0f / 3.0f;
    float widthOneThird = canvasWidth_ * third;
    float widthTwoThirds = canvasWidth_ * twoThirds;
    float heightOneThird = canvasHeight_ * third;
    float heightTwoThirds = canvasHeight_ * twoThirds;

    windowCtrlPoints_[TOP_LEFT_INDEX] = { 0.0f, 0.0f };
    windowCtrlPoints_[TOP_ONE_THIRD] = { widthOneThird, 0.0f };
    windowCtrlPoints_[TOP_TWO_THIRDS] = { widthTwoThirds, 0.0f };
    windowCtrlPoints_[TOP_RIGHT_INDEX] = { canvasWidth_, 0.0f };
    windowCtrlPoints_[RIGHT_ONE_THIRD] = { canvasWidth_, heightOneThird };
    windowCtrlPoints_[RIGHT_TWO_THIRDS] = { canvasWidth_, heightTwoThirds };
    windowCtrlPoints_[BOTTOM_RIGHT_INDEX] = { canvasWidth_, canvasHeight_ };
    windowCtrlPoints_[BOTTOM_TWO_THIRDS] = { widthTwoThirds, canvasHeight_ };
    windowCtrlPoints_[BOTTOM_ONE_THIRD] = { widthOneThird, canvasHeight_ };
    windowCtrlPoints_[BOTTOM_LEFT_INDEX] = { 0.0f, canvasHeight_ };
    windowCtrlPoints_[LEFT_TWO_THIRDS] = { 0.0f, heightTwoThirds };
    windowCtrlPoints_[LEFT_ONE_THIRD] = { 0.0f, heightOneThird };
}

void RSAttractionEffectFilter::CalculateDeltaXAndDeltaY(const Drawing::Point &pointDst, float &deltaX, float &deltaY,
    int direction)
{
    Drawing::Point windowCenter = { 0.5 * canvasWidth_, canvasHeight_ };
    if (windowCenter.GetY() > pointDst.GetY()) {
        if (direction == 1) {
            deltaX = std::abs((windowCtrlPoints_[TOP_LEFT_INDEX].GetY() + windowCtrlPoints_[BOTTOM_LEFT_INDEX].GetY()) /
                2.0f - pointDst.GetY());
            deltaY = std::abs(windowCtrlPoints_[TOP_LEFT_INDEX].GetX() - pointDst.GetX());
        } else {
            deltaX = std::abs(
                (windowCtrlPoints_[TOP_RIGHT_INDEX].GetY() + windowCtrlPoints_[BOTTOM_RIGHT_INDEX].GetY()) / 2.0f -
                pointDst.GetY());
            deltaY = std::abs(windowCtrlPoints_[TOP_RIGHT_INDEX].GetX() - pointDst.GetX());
        }
    } else {
        deltaX = std::abs(windowCenter.GetX() - pointDst.GetX());
        deltaY = std::abs(windowCenter.GetY() - pointDst.GetY());
    }
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateUpperCtrlPointOfVertex(float deltaX, float deltaY,
    float width, float height, int direction)
{
    // Different regression parameters are used for different scenarios
    // The window is located below the dock bar. Parameters with greater distortion are used.
    // Coordinates of the upper control point of the curve:(k1 * width + k2 * deltaX, k3 * height + k4 * deltaY)
    Drawing::Point topLeft = isBelowTarget_ ?
        Drawing::Point((0.016f * width - 0.08f * deltaX) * direction, 0.464f * height + 0.40f * deltaY) :
        Drawing::Point((-0.100f * width - 0.008f * deltaX) * direction, 0.008f * height + 0.085f * deltaY);
    Drawing::Point bottomLeft = isBelowTarget_ ?
        Drawing::Point((-0.15f * width - 0.075f * deltaX) * direction, 0.0f * height + 0.2f * deltaY) :
        Drawing::Point((-0.008f * width - 0.008f * deltaX) * direction, 0.0f * height - 0.008f * deltaY);
    Drawing::Point topRight = { (-1.147f * width - 0.016f * deltaX) * direction, -0.187f * height + 0.30f * deltaY };
    Drawing::Point bottomRight = { (-0.84f * width - 0.2f * deltaX) * direction, -0.859f * height - 0.2f * deltaY };
    std::vector<Drawing::Point> upperControlPoint = { topLeft, topRight, bottomLeft, bottomRight };
    return upperControlPoint;
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateLowerCtrlPointOfVertex(float deltaX, float deltaY,
    float width, float height, int direction)
{
    float inverseWidth = (width >= 1.0f) ? (1.0f / width) : 1.0f;
    // Different regression parameters are used for different scenarios
    // The window is located below the dock bar. Parameters with greater distortion are used.
    // Coordinates of the lower control point of the curve:(m1*(deltaX * height/width - width)), m2 * deltaY)
    Drawing::Point topLeft = isBelowTarget_ ?
        Drawing::Point((0.300f * (deltaX * height * inverseWidth - width)) * direction, -0.20f * deltaY) :
        Drawing::Point((0.131f * (deltaX * height * inverseWidth - width)) * direction, -0.20f * deltaY);
    Drawing::Point topRight = isBelowTarget_ ?
        Drawing::Point((0.450f * (deltaX * height * inverseWidth - width)) * direction, -0.30f * deltaY) :
        Drawing::Point((0.147f * (deltaX * height * inverseWidth - width)) * direction, -0.30f * deltaY);
    Drawing::Point bottomLeft = isBelowTarget_ ?
        Drawing::Point((0.150f * (deltaX * height * inverseWidth - width)) * direction, -0.20f * deltaY) :
        Drawing::Point((0.085f * (deltaX * height * inverseWidth - width)) * direction, 0.008f * deltaY);
    Drawing::Point bottomRight = isBelowTarget_ ?
        Drawing::Point((0.300f * (deltaX * height * inverseWidth - width)) * direction, -0.112f * deltaY) :
        Drawing::Point((0.147f * (deltaX * height * inverseWidth - width)) * direction, -0.069f * deltaY);
    std::vector<Drawing::Point> lowerControlPoint = { topLeft, topRight, bottomLeft, bottomRight };
    return lowerControlPoint;
}

void RSAttractionEffectFilter::CalculateVelocityCtrlPointUpper(std::vector<Drawing::Point>& velocityCtrlPointUpper)
{
    // Cubic Bezier curve with two control points
    Drawing::Point topVelFirst = { 0.50f, 0.0f };
    // Use faster speed when the lower edge of the window is above the dock bar
    Drawing::Point bottomVelFirst = isBelowTarget_ ? Drawing::Point(0.2f, 0.0f) : Drawing::Point(0.0f, 0.0f);
    velocityCtrlPointUpper.push_back(topVelFirst);
    velocityCtrlPointUpper.push_back(bottomVelFirst);
}

void RSAttractionEffectFilter::CalculateVelocityCtrlPointLower(std::vector<Drawing::Point>& velocityCtrlPointLower)
{
    // Cubic Bezier curve with two control points
    Drawing::Point topVelSecond = { 0.50f, 1.0f };
    // Use faster speed when the lower edge of the window is above the dock bar
    Drawing::Point bottomVelSecond = isBelowTarget_ ? Drawing::Point(0.2f, 1.0f) : Drawing::Point(0.0f, 1.0f);
    velocityCtrlPointLower.push_back(topVelSecond);
    velocityCtrlPointLower.push_back(bottomVelSecond);
}

void RSAttractionEffectFilter::UpdateDirtyRegion(float leftPoint, float topPonit)
{
    float dirtyRegionMinX = windowStatusPoints_[0].GetX();
    float dirtyRegionMaxX = windowStatusPoints_[0].GetX();
    float dirtyRegionMinY = windowStatusPoints_[0].GetY();
    float dirtyRegionMaxY = windowStatusPoints_[0].GetY();

    ROSEN_LOGI("RSAttractionEffectFilter::UpdateDirtyRegion:%f", dirtyRegionMinX);
    for (int i = 1; i < POINT_NUM; ++i) {
        float x = windowStatusPoints_[i].GetX();
        float y = windowStatusPoints_[i].GetY();
        dirtyRegionMinX = std::min(dirtyRegionMinX, x);
        dirtyRegionMaxX = std::max(dirtyRegionMaxX, x);
        dirtyRegionMinY = std::min(dirtyRegionMinY, y);
        dirtyRegionMaxY = std::max(dirtyRegionMaxY, y);
    }

    int dirtyRegionLeftCurrent = static_cast<int>(dirtyRegionMinX + leftPoint);
    int dirtyRegionTopCurrent = static_cast<int>(dirtyRegionMinY + topPonit);
    int dirtyRegionRightCurrent = static_cast<int>(dirtyRegionMaxX + leftPoint);
    int dirtyRegionBottomCurrent = static_cast<int>(dirtyRegionMaxY + topPonit);

    attractionDirtyRegion_.left_ = dirtyRegionLeftCurrent;
    attractionDirtyRegion_.top_ = dirtyRegionTopCurrent;
    attractionDirtyRegion_.width_ = dirtyRegionRightCurrent - dirtyRegionLeftCurrent;
    attractionDirtyRegion_.height_ = dirtyRegionBottomCurrent - dirtyRegionTopCurrent;
}

void RSAttractionEffectFilter::CalculateWindowStatus(float canvasWidth, float canvasHeight, Vector2f& destinationPoint)
{
    canvasWidth_ = canvasWidth;
    canvasHeight_ = canvasHeight;
    InitWindowInitCtrlPoints();

    Drawing::Point pointDst[1] = { { destinationPoint.x_, destinationPoint.y_ } };
    Drawing::Point windowBottomCenter = { 0.5 * canvasWidth_, 0.5 * canvasHeight_ };

    // 1 indicates that the window is to the right of the target point,
    // and - 1 indicates that the window is to the left.
    int direction = (windowBottomCenter.GetX() > pointDst[0].GetX()) ? 1 : -1;
    // true indicates that the lower edge of the window is below the dock bar,
    // and false indicates that the lower edge of the window is above the dock bar.
    isBelowTarget_ = (windowBottomCenter.GetY() > pointDst[0].GetY()) ? true : false;

    float width = isBelowTarget_ ? canvasHeight_ : canvasWidth_;
    float height = isBelowTarget_ ? canvasWidth_ : canvasHeight_;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
    CalculateDeltaXAndDeltaY(pointDst[0], deltaX, deltaY, direction);

    std::vector<Drawing::Point> upperControlPointOfVertex =
        CalculateUpperCtrlPointOfVertex(deltaX, deltaY, width, height, direction);
    std::vector<Drawing::Point> lowerControlPointOfVertex =
        CalculateLowerCtrlPointOfVertex(deltaX, deltaY, width, height, direction);
    std::vector<Drawing::Point> velocityCtrlPointUpper;
    CalculateVelocityCtrlPointUpper(velocityCtrlPointUpper);
    std::vector<Drawing::Point> velocityCtrlPointLower;
    CalculateVelocityCtrlPointLower(velocityCtrlPointLower);

    std::vector<Drawing::Point> controlPointListFirst =
        CalculateCubicsCtrlPoint(upperControlPointOfVertex, windowCtrlPoints_, direction, true);
    std::vector<Drawing::Point> controlPointListSecond =
        CalculateCubicsCtrlPoint(lowerControlPointOfVertex, pointDst, direction, false);

    std::vector<Drawing::Point> speedListsFirst(POINT_NUM, Drawing::Point(0.0f, 0.0f));
    std::vector<Drawing::Point> speedListsSecond(POINT_NUM, Drawing::Point(0.0f, 0.0f));
    CalculateBezierVelList(velocityCtrlPointUpper, speedListsFirst, direction);
    CalculateBezierVelList(velocityCtrlPointLower, speedListsSecond, direction);

    for (int i = 0; i < POINT_NUM; ++i) {
        float speed = BinarySearch(attractionFraction_, speedListsFirst[i], speedListsSecond[i]);
        windowStatusPoints_[i] = CubicBezier(windowCtrlPoints_[i], controlPointListFirst[i], controlPointListSecond[i],
            pointDst[0], speed);
    }
}

Drawing::Brush RSAttractionEffectFilter::GetBrush(const std::shared_ptr<Drawing::Image>& image) const
{
    Drawing::Brush brush;
    brush.SetBlendMode(Drawing::BlendMode::SRC_OVER);
    Drawing::SamplingOptions samplingOptions;
    Drawing::Matrix scaleMat;
    brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, scaleMat));
    return brush;
}

void RSAttractionEffectFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (!IsValid() || !image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        ROSEN_LOGE("RSAttractionEffectFilter::shader error");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("DrawAttraction:%f", attractionFraction_);
    int width = image->GetWidth();
    int height = image->GetHeight();

    auto brush = GetBrush(image);
    canvas.AttachBrush(brush);

    // 4 coordinates of image texture
    const Drawing::Point texCoords[4] = { { 0.0f, 0.0f }, { width, 0.0f }, { width, height }, { 0.0f, height } };

    canvas.DrawPatch(windowStatusPoints_, nullptr, texCoords, Drawing::BlendMode::SRC_OVER);
    canvas.DetachBrush();
}
} // namespace Rosen
} // namespace OHOS
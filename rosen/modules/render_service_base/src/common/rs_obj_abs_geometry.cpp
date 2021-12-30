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

#include "common/rs_obj_abs_geometry.h"

#include "include/utils/SkCamera.h"
#include "include/core/SkMatrix44.h"
namespace OHOS {
namespace Rosen {
constexpr unsigned RECT_POINT_NUM = 4;
constexpr unsigned LEFT_TOP_POINT = 0;
constexpr unsigned RIGHT_TOP_POINT = 1;
constexpr unsigned RIGHT_BOTTOM_POINT = 2;
constexpr unsigned LEFT_BOTTOM_POINT = 3;
RSObjAbsGeometry::RSObjAbsGeometry() : RSObjGeometry()
{
    vertices_[LEFT_TOP_POINT].set(0, 0);
    vertices_[RIGHT_TOP_POINT].set(0, 0);
    vertices_[RIGHT_BOTTOM_POINT].set(0, 0);
    vertices_[LEFT_BOTTOM_POINT].set(0, 0);
}

RSObjAbsGeometry::~RSObjAbsGeometry() {}

void RSObjAbsGeometry::UpdateMatrix(const Matrix3f& matrix)
{
    SkMatrix skMatrix;
    skMatrix.set9(matrix.Transpose().GetData());
    matrix_.preConcat(skMatrix);
    absMatrix_.preConcat(matrix_);
    SetAbsRect();
}

void RSObjAbsGeometry::UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent, float offsetX, float offsetY)
{
    if (parent == nullptr) {
        absMatrix_.reset();
    } else {
        absMatrix_ = parent->absMatrix_;
        absMatrix_.preTranslate(offsetX, offsetY);
    }
    matrix_.reset();
    if (!trans_ || (ROSEN_EQ(z_ + trans_->translateZ_, 0.f) && ROSEN_EQ(trans_->rotationX_, 0.f) &&
        ROSEN_EQ(trans_->rotationY_, 0.f) && trans_->quaternion_.IsIdentity())) {
        UpdateAbsMatrix2D();
    } else {
        UpdateAbsMatrix3D();
    }
    absMatrix_.preConcat(matrix_);
    SetAbsRect();
}

void RSObjAbsGeometry::UpdateAbsMatrix2D()
{
    if (!trans_) {
        matrix_.preTranslate(x_, y_);
    } else {
        // Translate
        if ((x_ + trans_->translateX_ != 0) || (y_ + trans_->translateY_ != 0)) {
            matrix_.preTranslate(x_ + trans_->translateX_, y_ + trans_->translateY_);
        }
        // rotation
        if (!ROSEN_EQ(trans_->rotation_, 0.f)) {
            matrix_.preRotate(trans_->rotation_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
        }
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix_.preScale(trans_->scaleX_, trans_->scaleY_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
        }
    }
}

void RSObjAbsGeometry::UpdateAbsMatrix3D()
{
    if (!trans_->quaternion_.IsIdentity()) {
        SkMatrix44 matrix3D;
        // Translate
        matrix3D.setTranslate(trans_->pivotX_ * width_ + x_ + trans_->translateX_,
            trans_->pivotY_ * height_ + y_ + trans_->translateY_, z_ + trans_->translateZ_);
        // Rotate
        SkMatrix44 matrix4;
        float x = trans_->quaternion_[0];
        float y = trans_->quaternion_[1];
        float z = trans_->quaternion_[2];
        float w = trans_->quaternion_[3];
        matrix4.set3x3(1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w), 2.f * (x * y - z * w),
            1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w), 2.f * (x * z + y * w), 2.f * (y * z - x * w),
            1.f - 2.f * (x * x + y * y));
        matrix3D = matrix3D * matrix4;

        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix3D.preScale(trans_->scaleX_, trans_->scaleY_, 1.f);
        }
        // Translate
        matrix3D.preTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_, 0);
        matrix_.preConcat(SkMatrix(matrix3D));
    } else {
        SkMatrix matrix3D;
        Sk3DView camera;
        // Z Position
        camera.translate(0, 0, z_ + trans_->translateZ_);
        // Rotate
        camera.rotateX(trans_->rotationX_);
        camera.rotateY(trans_->rotationY_);
        camera.rotateZ(trans_->rotation_);
        camera.getMatrix(&matrix3D);
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix3D.preScale(trans_->scaleX_, trans_->scaleY_);
        }
        // Translate
        matrix3D.preTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_);
        matrix3D.postTranslate(
            trans_->pivotX_ * width_ + x_ + trans_->translateX_, trans_->pivotY_ * height_ + y_ + trans_->translateY_);
        matrix_.preConcat(matrix3D);
    }
}

void RSObjAbsGeometry::SetAbsRect()
{
    absRect_ = MapAbsRect(RectF(0.f, 0.f, width_, height_));
}

RectI RSObjAbsGeometry::MapAbsRect(const RectF& rect) const
{
    RectI absRect;
    if (!ROSEN_EQ(absMatrix_.getSkewX(), 0.f) || (absMatrix_.getScaleX() < 0) ||
        !ROSEN_EQ(absMatrix_.getSkewY(), 0.f) || (absMatrix_.getScaleY() < 0)) {
        SkPoint p[RECT_POINT_NUM];
        p[LEFT_TOP_POINT].set(rect.left_, rect.top_);
        p[RIGHT_TOP_POINT].set(rect.left_ + rect.width_, rect.top_);
        p[RIGHT_BOTTOM_POINT].set(rect.left_ + rect.width_, rect.top_ + rect.height_);
        p[LEFT_BOTTOM_POINT].set(rect.left_, rect.top_ + rect.height_);
        absMatrix_.mapPoints(p, RECT_POINT_NUM);

        Vector2f xRange = GetDataRange(p[LEFT_TOP_POINT].x(), p[RIGHT_TOP_POINT].x(),
            p[RIGHT_BOTTOM_POINT].x(), p[LEFT_BOTTOM_POINT].x());
        Vector2f yRange = GetDataRange(p[LEFT_TOP_POINT].y(), p[RIGHT_TOP_POINT].y(),
            p[RIGHT_BOTTOM_POINT].y(), p[LEFT_BOTTOM_POINT].y());
        absRect.left_ = static_cast<int>(xRange[0]);
        absRect.top_ = static_cast<int>(yRange[0]);
        absRect.width_ = static_cast<int>(std::ceil(xRange[1] - absRect.left_));
        absRect.height_ = static_cast<int>(std::ceil(yRange[1] - absRect.top_));
    } else {
        absRect.left_ = static_cast<int>(rect.left_ + absMatrix_.getTranslateX());
        absRect.top_ = static_cast<int>(rect.top_ + absMatrix_.getTranslateY());
        float right = rect.left_ + absMatrix_.getTranslateX() + rect.width_ * absMatrix_.getScaleX();
        float bottom = rect.top_ + absMatrix_.getTranslateY() + rect.height_ * absMatrix_.getScaleY();
        absRect.width_ = static_cast<int>(std::ceil(right - absRect.left_));
        absRect.height_ = static_cast<int>(std::ceil(bottom - absRect.top_));
    }
    return absRect;
}
Vector2f RSObjAbsGeometry::GetDataRange(float d0, float d1, float d2, float d3) const
{
    float min = d0;
    float max = d0;
    if (d0 > d1) {
        min = d1;
    } else {
        max = d1;
    }
    if (d2 > d3) {
        if (min > d3) {
            min = d3;
        }
        if (max < d2) {
            max = d2;
        }
    } else {
        if (min > d2) {
            min = d2;
        }
        if (max < d3) {
            max = d3;
        }
    }
    return Vector2f(min, max);
}

/**
 * calculate | p1 p2 | X | p1 p |
 * @param p1 a point on a line
 * @param p2 a point on a line
 * @param p a point on a line
 * @return the result of two cross multiplications
 */
float RSObjAbsGeometry::GetCross(const SkPoint& p1, const SkPoint& p2, const SkPoint& p) const
{
    return (p2.x() - p1.x()) * (p.y() - p1.y()) - (p.x() - p1.x()) * (p2.y() - p1.y());
}

/**
 * Determine whether a point is within a rectangle.
 *
 * Determine whether a point is between two line segments by the directivity of cross multiplication.For example
 * (AB X AE ) * (CDX CE)  >= 0 This indicates that E is between AD and BC.
 * Two judgments can prove whether the point is in the rectangle.
 *
 * @param p Point to be judged
 * @return If true indicates that the point is in the rectangle, if false indicates that the point is not in.
 */
bool RSObjAbsGeometry::IsPointInHotZone(const float x, const float y) const
{
    SkPoint p = SkPoint::Make(x, y);
    float crossResult1 = GetCross(vertices_[LEFT_TOP_POINT], vertices_[RIGHT_TOP_POINT], p);
    float crossResult2 = GetCross(vertices_[RIGHT_BOTTOM_POINT], vertices_[LEFT_BOTTOM_POINT], p);
    float crossResult3 = GetCross(vertices_[RIGHT_TOP_POINT], vertices_[RIGHT_BOTTOM_POINT], p);
    float crossResult4 = GetCross(vertices_[LEFT_BOTTOM_POINT], vertices_[LEFT_TOP_POINT], p);
    return IsPointInLine(vertices_[LEFT_TOP_POINT], vertices_[RIGHT_TOP_POINT], p, crossResult1) ||
           IsPointInLine(vertices_[RIGHT_BOTTOM_POINT], vertices_[LEFT_BOTTOM_POINT], p, crossResult2) ||
           IsPointInLine(vertices_[RIGHT_TOP_POINT], vertices_[RIGHT_BOTTOM_POINT], p, crossResult3) ||
           IsPointInLine(vertices_[LEFT_BOTTOM_POINT], vertices_[LEFT_TOP_POINT], p, crossResult4) ||
           (crossResult1 * crossResult2 > 0 && crossResult3 * crossResult4 > 0);
}

bool RSObjAbsGeometry::IsPointInLine(const SkPoint& p1, const SkPoint& p2, const SkPoint& p, const float crossRes) const
{
    return ROSEN_EQ(crossRes, 0.0f) && std::min(p1.x(), p2.x()) <= p.x() && p.x() <= std::max(p1.x(), p2.x()) &&
           std::min(p1.y(), p2.y()) <= p.y() && p.y() <= std::max(p1.y(), p2.y());
}
} // namespace Rosen
} // namespace OHOS

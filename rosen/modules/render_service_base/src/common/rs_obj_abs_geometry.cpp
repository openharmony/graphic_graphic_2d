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
constexpr float INCH_TO_PIXEL = 72;
constexpr float EPSILON = 1e-4f;

RSObjAbsGeometry::RSObjAbsGeometry() : RSObjGeometry()
{
    vertices_[LEFT_TOP_POINT].set(0, 0);
    vertices_[RIGHT_TOP_POINT].set(0, 0);
    vertices_[RIGHT_BOTTOM_POINT].set(0, 0);
    vertices_[LEFT_BOTTOM_POINT].set(0, 0);
}

RSObjAbsGeometry::~RSObjAbsGeometry() {}

void RSObjAbsGeometry::ConcatMatrix(const SkMatrix& matrix)
{
    if (matrix.isIdentity()) {
        return;
    }
    matrix_.preConcat(matrix);
    if (absMatrix_.has_value()) {
        absMatrix_->preConcat(matrix);
    }
    SetAbsRect();
}

/**
 * @brief Updates the matrix of the view with respect to its parent view.
 *
 * @param parent The parent view of the current view.
 * @param offset The offset of the current view with respect to its parent.
 * @param clipRect The optional clipping rectangle of the current view.
 */
void RSObjAbsGeometry::UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent,
    const std::optional<SkPoint>& offset, const std::optional<SkRect>& clipRect)
{
    // Initialize the absolute matrix with the absolute matrix of the parent view if the parent view exists
    if (parent == nullptr) {
        absMatrix_.reset();
    } else {
        absMatrix_ = parent->GetAbsMatrix();
    }
    if (absMatrix_.has_value() && offset.has_value() && !offset.value().isZero()) {
        absMatrix_->preTranslate(offset->x(), offset->y());
    }
    // Reset the matrix of the current view
    matrix_.reset();
    // filter invalid width and height
    if (IsEmpty()) {
        return;
    }
    // If the view has no transformations or only 2D transformations, update the absolute matrix with 2D
    // transformations
    if (!trans_ || (ROSEN_EQ(trans_->translateZ_, 0.f) && ROSEN_EQ(trans_->rotationX_, 0.f) &&
        ROSEN_EQ(trans_->rotationY_, 0.f) && trans_->quaternion_.IsIdentity())) {
        UpdateAbsMatrix2D();
    } else {
        // Otherwise, update the absolute matrix with 3D transformations
        UpdateAbsMatrix3D();
    }
    // If the absolute matrix of the current view exists, update it with the context matrix and the current matrix
    if (absMatrix_.has_value()) {
        if (contextMatrix_.has_value()) {
            absMatrix_->preConcat(*contextMatrix_);
        }
        absMatrix_->preConcat(matrix_);
    }
    // if clipRect is valid, update rect with clipRect
    if (clipRect.has_value() && !clipRect.value().isEmpty()) {
        auto mappedClipRect = clipRect.value();
        if (!matrix_.isIdentity()) {
            SkMatrix invertMatrix;
            if (matrix_.invert(&invertMatrix)) {
                mappedClipRect = invertMatrix.mapRect(mappedClipRect);
            }
            // matrix_ already includes bounds offset, we need to revert it
            mappedClipRect.offset(GetX(), GetY());
        }

        if (!mappedClipRect.intersect(x_, y_, x_ + width_, y_ + height_)) {
            // No visible area
            x_ = y_ = width_ = height_ = 0.0f;
            return;
        }
        x_ = mappedClipRect.left();
        y_ = mappedClipRect.top();
        width_ = mappedClipRect.width();
        height_ = mappedClipRect.height();
    }
    // If the context matrix of the current view exists, update the current matrix with it
    if (contextMatrix_.has_value()) {
        matrix_.preConcat(*contextMatrix_);
    }
    // Update the absolute rectangle of the current view
    SetAbsRect();
}

/**
 * @brief Updates the matrix of the view without its parent view.
 */
void RSObjAbsGeometry::UpdateByMatrixFromSelf()
{
    absMatrix_.reset();
    matrix_.reset();

    // If the view has no transformations or only 2D transformations, update the absolute matrix with 2D transformations
    if (!trans_ || (ROSEN_EQ(trans_->translateZ_, 0.f) && ROSEN_EQ(trans_->rotationX_, 0.f) &&
        ROSEN_EQ(trans_->rotationY_, 0.f) && trans_->quaternion_.IsIdentity())) {
        UpdateAbsMatrix2D();
    } else {
        // Otherwise, update the absolute matrix with 3D transformations
        UpdateAbsMatrix3D();
    }

    // If the context matrix of the view exists, update the current matrix with it
    if (contextMatrix_.has_value()) {
        matrix_.preConcat(*contextMatrix_);
    }

    // Update the absolute rectangle of the view
    SetAbsRect();
}

bool RSObjAbsGeometry::IsNeedClientCompose() const
{
    if (!trans_) {
        return false;
    }
    // return false if rotation degree is times of 90
    return !ROSEN_EQ(std::remainder(trans_->rotation_, 90.f), 0.f, EPSILON);
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
        if (!ROSEN_EQ(trans_->rotation_, 0.f, EPSILON)) {
            matrix_.preRotate(trans_->rotation_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
        }
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix_.preScale(trans_->scaleX_, trans_->scaleY_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
        }
    }
}

/**
 * Update the absolute matrix in 3D space
 */
void RSObjAbsGeometry::UpdateAbsMatrix3D()
{
    // If the view has a non-identity quaternion, apply 3D transformations
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

        // Concatenate the 3D matrix with the 2D matrix
        matrix_.preConcat(SkMatrix(matrix3D));
    } else {
        SkMatrix matrix3D;
        Sk3DView camera;

        // Z Position
        camera.translate(0, 0, z_ + trans_->translateZ_);

        // Set camera distance
        if (trans_->cameraDistance_ == 0) {
            float zOffSet = sqrt(width_ * width_ + height_ * height_) / 2;
            camera.setCameraLocation(0, 0, camera.getCameraLocationZ() - zOffSet / INCH_TO_PIXEL);
        } else {
            camera.setCameraLocation(0, 0, trans_->cameraDistance_);
        }

        // Rotate
        camera.rotateX(-trans_->rotationX_);
        camera.rotateY(-trans_->rotationY_);
        camera.rotateZ(-trans_->rotation_);
        camera.getMatrix(&matrix3D);

        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix3D.preScale(trans_->scaleX_, trans_->scaleY_);
        }

        // Translate
        matrix3D.preTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_);

        // Concatenate the 3D matrix with the 2D matrix
        matrix3D.postTranslate(
            trans_->pivotX_ * width_ + x_ + trans_->translateX_, trans_->pivotY_ * height_ + y_ + trans_->translateY_);
        matrix_.preConcat(matrix3D);
    }
}

void RSObjAbsGeometry::SetAbsRect()
{
    absRect_ = MapAbsRect(RectF(0.f, 0.f, width_, height_));
}

/**
 * Map the absolute rectangle
 * @param rect the rectangle to map
 * @return the mapped absolute rectangle
 */
RectI RSObjAbsGeometry::MapAbsRect(const RectF& rect) const
{
    RectI absRect;
    auto& matrix = GetAbsMatrix();
    // Check if the matrix has skew or negative scaling
    if (!ROSEN_EQ(matrix.getSkewX(), 0.f) || (matrix.getScaleX() < 0) ||
        !ROSEN_EQ(matrix.getSkewY(), 0.f) || (matrix.getScaleY() < 0)) {
        // Map the rectangle's points to the absolute matrix
        SkPoint p[RECT_POINT_NUM];
        p[LEFT_TOP_POINT].set(rect.left_, rect.top_);
        p[RIGHT_TOP_POINT].set(rect.left_ + rect.width_, rect.top_);
        p[RIGHT_BOTTOM_POINT].set(rect.left_ + rect.width_, rect.top_ + rect.height_);
        p[LEFT_BOTTOM_POINT].set(rect.left_, rect.top_ + rect.height_);
        matrix.mapPoints(p, RECT_POINT_NUM);

        // Get the data range of the mapped points
        Vector2f xRange = GetDataRange(p[LEFT_TOP_POINT].x(), p[RIGHT_TOP_POINT].x(),
            p[RIGHT_BOTTOM_POINT].x(), p[LEFT_BOTTOM_POINT].x());
        Vector2f yRange = GetDataRange(p[LEFT_TOP_POINT].y(), p[RIGHT_TOP_POINT].y(),
            p[RIGHT_BOTTOM_POINT].y(), p[LEFT_BOTTOM_POINT].y());

        // Set the absolute rectangle's properties
        absRect.left_ = static_cast<int>(xRange[0]);
        absRect.top_ = static_cast<int>(yRange[0]);
        absRect.width_ = static_cast<int>(std::ceil(xRange[1] - absRect.left_));
        absRect.height_ = static_cast<int>(std::ceil(yRange[1] - absRect.top_));
    } else {
        // Calculate the absolute rectangle based on the matrix's translation and scaling
        absRect.left_ = static_cast<int>(rect.left_ + matrix.getTranslateX());
        absRect.top_ = static_cast<int>(rect.top_ + matrix.getTranslateY());
        float right = rect.left_ + matrix.getTranslateX() + rect.width_ * matrix.getScaleX();
        float bottom = rect.top_ + matrix.getTranslateY() + rect.height_ * matrix.getScaleY();
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
 * Determine whether a point is between two line segments by the sign of cross multiplication.
 * For example (AB X AE ) * (CD X CE)  >= 0 This indicates that E is between AD and BC.
 * Two judgments can prove whether the point is in the rectangle.
 *
 * @param x x-coordinate of the point to be judged
 * @param y y-coordinate of the point to be judged
 * @return true if the point is in the rectangle, false otherwise
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

/**
 * Determine whether a point is on a line.
 *
 * @param p1 a point on the line
 * @param p2 a point on the line
 * @param p the point to be judged
 * @param crossRes the result of two cross multiplications
 * @return true if the point is on the line, false otherwise
 */
bool RSObjAbsGeometry::IsPointInLine(const SkPoint& p1, const SkPoint& p2, const SkPoint& p, const float crossRes) const
{
    return ROSEN_EQ(crossRes, 0.0f) && std::min(p1.x(), p2.x()) <= p.x() && p.x() <= std::max(p1.x(), p2.x()) &&
           std::min(p1.y(), p2.y()) <= p.y() && p.y() <= std::max(p1.y(), p2.y());
}

void RSObjAbsGeometry::SetContextMatrix(const std::optional<SkMatrix>& matrix)
{
    contextMatrix_ = matrix;
}

const SkMatrix& RSObjAbsGeometry::GetMatrix() const
{
    return matrix_;
}

const SkMatrix& RSObjAbsGeometry::GetAbsMatrix() const
{
    // if absMatrix_ is empty, return matrix_ instead
    return absMatrix_ ? *absMatrix_ : matrix_;
}
} // namespace Rosen
} // namespace OHOS

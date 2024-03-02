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

#include "utils/log.h"
#include "common/rs_obj_abs_geometry.h"

#include "utils/camera3d.h"

namespace OHOS {
namespace Rosen {
constexpr unsigned RECT_POINT_NUM = 4;
constexpr unsigned LEFT_TOP_POINT = 0;
constexpr unsigned RIGHT_TOP_POINT = 1;
constexpr unsigned RIGHT_BOTTOM_POINT = 2;
constexpr unsigned LEFT_BOTTOM_POINT = 3;
constexpr float INCH_TO_PIXEL = 72;
constexpr float EPSILON = 1e-4f;
constexpr float DEGREE_TO_RADIAN = M_PI / 180;

RSObjAbsGeometry::RSObjAbsGeometry() = default;
RSObjAbsGeometry::~RSObjAbsGeometry() = default;

void RSObjAbsGeometry::ConcatMatrix(const Drawing::Matrix& matrix)
{
    if (matrix.IsIdentity()) {
        return;
    }
    matrix_.PreConcat(matrix);
    if (absMatrix_.has_value()) {
        absMatrix_->PreConcat(matrix);
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
    const std::optional<Drawing::Point>& offset, const std::optional<Drawing::Rect>& clipRect)
{
    // Initialize the absolute matrix with the absolute matrix of the parent view if the parent view exists
    if (parent == nullptr) {
        absMatrix_.reset();
    } else {
        absMatrix_ = parent->GetAbsMatrix();
    }
    if (absMatrix_.has_value() && offset.has_value() && !offset.value().IsZero()) {
        absMatrix_->PreTranslate(offset->GetX(), offset->GetY());
    }
    // Reset the matrix of the current view
    matrix_.Reset();
    // filter invalid width, height, x, y
    if (IsEmpty() && !IsValidOffset()) {
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
            absMatrix_->PreConcat(*contextMatrix_);
        }
        absMatrix_->PreConcat(matrix_);
    }
    // if clipRect is valid, update rect with clipRect
    if (clipRect.has_value() && !clipRect.value().IsEmpty()) {
        auto mappedClipRect = clipRect.value();
        if (!matrix_.IsIdentity()) {
            Drawing::Matrix invertMatrix;
            if (matrix_.Invert(invertMatrix)) {
                invertMatrix.MapRect(mappedClipRect, mappedClipRect);
            }
            // matrix_ already includes bounds offset, we need to revert it
            mappedClipRect.Offset(GetX(), GetY());
        }

        if (!mappedClipRect.Intersect({ x_, y_, x_ + width_, y_ + height_ })) {
            // No visible area
            x_ = y_ = width_ = height_ = 0.0f;
            return;
        }
        x_ = mappedClipRect.GetLeft();
        y_ = mappedClipRect.GetTop();
        width_ = mappedClipRect.GetWidth();
        height_ = mappedClipRect.GetHeight();
    }
    // If the context matrix of the current view exists, update the current matrix with it
    if (contextMatrix_.has_value()) {
        matrix_.PreConcat(*contextMatrix_);
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
    matrix_.Reset();

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
        matrix_.PreConcat(*contextMatrix_);
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

void RSObjAbsGeometry::ApplySkewToMatrix(Drawing::Matrix& m, bool preConcat)
{
    if (!ROSEN_EQ(trans_->skewX_, 0.f, EPSILON) || !ROSEN_EQ(trans_->skewY_, 0.f, EPSILON)) {
        if (preConcat) {
            m.PreSkew(trans_->skewX_, trans_->skewY_);
        } else {
            m.PostSkew(trans_->skewX_, trans_->skewY_);
        }
    }
}

void RSObjAbsGeometry::ApplySkewToMatrix44(Drawing::Matrix44& m44, bool preConcat)
{
    if (!ROSEN_EQ(trans_->skewX_, 0.f, EPSILON) || !ROSEN_EQ(trans_->skewY_, 0.f, EPSILON)) {
        Drawing::Matrix44 skewM44 {};
        skewM44.SetMatrix44RowMajor({1.f, trans_->skewX_, 0.f, 0.f,
            trans_->skewY_, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f});
        if (preConcat) {
            m44 =  skewM44 * m44;
        } else {
            m44 =  m44 *skewM44;
        }
    }
}

void RSObjAbsGeometry::UpdateAbsMatrix2D()
{
    if (!trans_) {
        matrix_.PreTranslate(x_, y_);
    } else {
        // Translate
        if ((x_ + trans_->translateX_ != 0) || (y_ + trans_->translateY_ != 0)) {
            matrix_.PreTranslate(x_ + trans_->translateX_, y_ + trans_->translateY_);
        }
        // rotation
        if (!ROSEN_EQ(trans_->rotation_, 0.f, EPSILON)) {
            matrix_.PreRotate(trans_->rotation_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
        }
        // Skew
        ApplySkewToMatrix(matrix_);
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix_.PreScale(trans_->scaleX_, trans_->scaleY_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
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
        Drawing::Matrix44 matrix3D;
        // Translate
        matrix3D.Translate(trans_->pivotX_ * width_ + x_ + trans_->translateX_,
            trans_->pivotY_ * height_ + y_ + trans_->translateY_, z_ + trans_->translateZ_);
        // Rotate
        float x = trans_->quaternion_[0];
        float y = trans_->quaternion_[1];
        float z = trans_->quaternion_[2];
        float w = trans_->quaternion_[3];
        Drawing::Matrix44::Buffer buffer = {
            1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w), 0,   // m00 ~ m30
            2.f * (x * y - z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w), 0,   // m01 ~ m31
            2.f * (x * z + y * w), 2.f * (y * z - x * w), 1.f - 2.f * (x * x + y * y), 0,   // m02 ~ m32
            0, 0, 0, 1 };                                                                   // m03 ~ m33
        Drawing::Matrix44 matrix4;
        matrix4.SetMatrix44ColMajor(buffer);
        matrix3D = matrix3D * matrix4;
        // Skew
        ApplySkewToMatrix44(matrix3D, false);
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix3D.PreScale(trans_->scaleX_, trans_->scaleY_, 1.f);
        }
        // Translate
        matrix3D.PreTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_, 0);

        // Concatenate the 3D matrix with the 2D matrix
        matrix_.PreConcat(matrix3D);
    } else {
        Drawing::Matrix matrix3D;
        Drawing::Camera3D camera;
        // Z Position
        camera.Translate(0, 0, z_ + trans_->translateZ_);

        // Set camera distance
        if (trans_->cameraDistance_ == 0) {
            float zOffSet = sqrt(width_ * width_ + height_ * height_) / 2;
            camera.SetCameraPos(0, 0, camera.GetCameraPosZ() - zOffSet / INCH_TO_PIXEL);
        } else {
            camera.SetCameraPos(0, 0, trans_->cameraDistance_);
        }
        // Rotate
        if (trans_->pivotZ_ != 0.f) {
            camera.Translate(-sin(trans_->rotationY_ * DEGREE_TO_RADIAN) * trans_->pivotZ_,
                -sin(trans_->rotationX_ * DEGREE_TO_RADIAN) * trans_->pivotZ_,
                (1 - cos(trans_->rotationX_ * DEGREE_TO_RADIAN) * cos(trans_->rotationY_ * DEGREE_TO_RADIAN)) *
                    trans_->pivotZ_);
        }
        camera.RotateXDegrees(-trans_->rotationX_);
        camera.RotateYDegrees(-trans_->rotationY_);
        camera.RotateZDegrees(-trans_->rotation_);
        camera.ApplyToMatrix(matrix3D);
        // Skew
        ApplySkewToMatrix(matrix3D, false);
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix3D.PreScale(trans_->scaleX_, trans_->scaleY_);
        }
        // Translate
        matrix3D.PreTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_);

        // Concatenate the 3D matrix with the 2D matrix
        matrix3D.PostTranslate(
            trans_->pivotX_ * width_ + x_ + trans_->translateX_, trans_->pivotY_ * height_ + y_ + trans_->translateY_);
        matrix_.PreConcat(matrix3D);
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
    if (!ROSEN_EQ(matrix.Get(Drawing::Matrix::SKEW_X), 0.f) || (matrix.Get(Drawing::Matrix::SCALE_X) < 0) ||
        !ROSEN_EQ(matrix.Get(Drawing::Matrix::SKEW_Y), 0.f) || (matrix.Get(Drawing::Matrix::SCALE_Y) < 0)) {
        // Map the rectangle's points to the absolute matrix
        std::vector<Drawing::Point> p(RECT_POINT_NUM);
        p[LEFT_TOP_POINT] = {rect.left_, rect.top_};
        p[RIGHT_TOP_POINT] = {rect.left_ + rect.width_, rect.top_};
        p[RIGHT_BOTTOM_POINT] = {rect.left_ + rect.width_, rect.top_ + rect.height_};
        p[LEFT_BOTTOM_POINT] = {rect.left_, rect.top_ + rect.height_};
        matrix.MapPoints(p, p, RECT_POINT_NUM);

        Vector2f xRange = GetDataRange(p[LEFT_TOP_POINT].GetX(), p[RIGHT_TOP_POINT].GetX(),
            p[RIGHT_BOTTOM_POINT].GetX(), p[LEFT_BOTTOM_POINT].GetX());
        Vector2f yRange = GetDataRange(p[LEFT_TOP_POINT].GetY(), p[RIGHT_TOP_POINT].GetY(),
            p[RIGHT_BOTTOM_POINT].GetY(), p[LEFT_BOTTOM_POINT].GetY());

        // Set the absolute rectangle's properties
        absRect.left_ = static_cast<int>(xRange[0]);
        absRect.top_ = static_cast<int>(yRange[0]);
        absRect.width_ = static_cast<int>(std::ceil(xRange[1] - absRect.left_));
        absRect.height_ = static_cast<int>(std::ceil(yRange[1] - absRect.top_));
    } else {
        // Calculate the absolute rectangle based on the matrix's translation and scaling
        Drawing::scalar transX = matrix.Get(Drawing::Matrix::TRANS_X);
        Drawing::scalar transY = matrix.Get(Drawing::Matrix::TRANS_Y);
        Drawing::scalar scaleX = matrix.Get(Drawing::Matrix::SCALE_X);
        Drawing::scalar scaleY = matrix.Get(Drawing::Matrix::SCALE_Y);
        absRect.left_ = static_cast<int>(rect.left_ * scaleX + transX);
        absRect.top_ = static_cast<int>(rect.top_ * scaleY + transY);
        float right = (rect.left_ + rect.width_) * scaleX + transX;
        float bottom = (rect.top_ + rect.height_) * scaleY + transY;
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

void RSObjAbsGeometry::SetContextMatrix(const std::optional<Drawing::Matrix>& matrix)
{
    contextMatrix_ = matrix;
}

const Drawing::Matrix& RSObjAbsGeometry::GetMatrix() const
{
    return matrix_;
}

const Drawing::Matrix& RSObjAbsGeometry::GetAbsMatrix() const
{
    // if absMatrix_ is empty, return matrix_ instead
    return absMatrix_ ? *absMatrix_ : matrix_;
}
} // namespace Rosen
} // namespace OHOS

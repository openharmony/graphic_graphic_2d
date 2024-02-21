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

#ifndef USE_ROSEN_DRAWING
#include "include/utils/SkCamera.h"
#else // USE_ROSEN_DRAWING
#include "utils/camera3d.h"
#endif

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

#ifndef USE_ROSEN_DRAWING
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
#else
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
#endif

/**
 * @brief Updates the matrix of the view with respect to its parent view.
 *
 * @param parent The parent view of the current view.
 * @param offset The offset of the current view with respect to its parent.
 * @param clipRect The optional clipping rectangle of the current view.
 */
#ifndef USE_ROSEN_DRAWING
void RSObjAbsGeometry::UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent,
    const std::optional<SkPoint>& offset, const std::optional<SkRect>& clipRect)
#else
void RSObjAbsGeometry::UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent,
    const std::optional<Drawing::Point>& offset, const std::optional<Drawing::Rect>& clipRect)
#endif
{
    // Initialize the absolute matrix with the absolute matrix of the parent view if the parent view exists
    if (parent == nullptr) {
        absMatrix_.reset();
    } else {
        absMatrix_ = parent->GetAbsMatrix();
    }
#ifndef USE_ROSEN_DRAWING
    if (absMatrix_.has_value() && offset.has_value() && !offset.value().isZero()) {
        absMatrix_->preTranslate(offset->x(), offset->y());
    }
#else
    if (absMatrix_.has_value() && offset.has_value() && !offset.value().IsZero()) {
        absMatrix_->PreTranslate(offset->GetX(), offset->GetY());
    }
#endif
    // Reset the matrix of the current view
#ifndef USE_ROSEN_DRAWING
    matrix_.reset();
#else
    matrix_.Reset();
#endif
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
#ifndef USE_ROSEN_DRAWING
        if (contextMatrix_.has_value()) {
            absMatrix_->preConcat(*contextMatrix_);
        }
        absMatrix_->preConcat(matrix_);
#else
        if (contextMatrix_.has_value()) {
            absMatrix_->PreConcat(*contextMatrix_);
        }
        absMatrix_->PreConcat(matrix_);
#endif
    }
    // if clipRect is valid, update rect with clipRect
#ifndef USE_ROSEN_DRAWING
    if (clipRect.has_value() && !clipRect.value().isEmpty()) {
#else
    if (clipRect.has_value() && !clipRect.value().IsEmpty()) {
#endif
        auto mappedClipRect = clipRect.value();
#ifndef USE_ROSEN_DRAWING
        if (!matrix_.isIdentity()) {
            SkMatrix invertMatrix;
            if (matrix_.invert(&invertMatrix)) {
                mappedClipRect = invertMatrix.mapRect(mappedClipRect);
            }
            // matrix_ already includes bounds offset, we need to revert it
            mappedClipRect.offset(GetX(), GetY());
        }

#ifdef NEW_SKIA
        if (!mappedClipRect.intersect({x_, y_, x_ + width_, y_ + height_})) {
#else
        if (!mappedClipRect.intersect(x_, y_, x_ + width_, y_ + height_)) {
#endif
            // No visible area
            x_ = y_ = width_ = height_ = 0.0f;
            return;
        }
        x_ = mappedClipRect.left();
        y_ = mappedClipRect.top();
        width_ = mappedClipRect.width();
        height_ = mappedClipRect.height();
#else
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
#endif
    }
    // If the context matrix of the current view exists, update the current matrix with it
    if (contextMatrix_.has_value()) {
#ifndef USE_ROSEN_DRAWING
        matrix_.preConcat(*contextMatrix_);
#else
        matrix_.PreConcat(*contextMatrix_);
#endif
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
#ifndef USE_ROSEN_DRAWING
    matrix_.reset();
#else
    matrix_.Reset();
#endif

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
#ifndef USE_ROSEN_DRAWING
        matrix_.preConcat(*contextMatrix_);
#else
        matrix_.PreConcat(*contextMatrix_);
#endif
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

#ifndef USE_ROSEN_DRAWING
void RSObjAbsGeometry::ApplySkewToMatrix(SkMatrix& m, bool preConcat)
{
    if (!ROSEN_EQ(trans_->skewX_, 0.f, EPSILON) || !ROSEN_EQ(trans_->skewY_, 0.f, EPSILON)) {
        if (preConcat) {
            m.preSkew(trans_->skewX_, trans_->skewY_);
        } else {
            m.postSkew(trans_->skewX_, trans_->skewY_);
        }
    }
}
#else
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
#endif

#ifndef USE_ROSEN_DRAWING
void RSObjAbsGeometry::ApplySkewToMatrix44(SkM44& m44, bool preConcat)
#else
void RSObjAbsGeometry::ApplySkewToMatrix44(Drawing::Matrix44& m44, bool preConcat)
#endif
{
    if (!ROSEN_EQ(trans_->skewX_, 0.f, EPSILON) || !ROSEN_EQ(trans_->skewY_, 0.f, EPSILON)) {
#ifndef USE_ROSEN_DRAWING
        const SkScalar skewScalrs[] = {1.f, trans_->skewX_, 0.f, 0.f,
            trans_->skewY_, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f};
        SkM44 skewM44 = SkM44::RowMajor(skewScalrs);
#else
        Drawing::Matrix44 skewM44 {};
        skewM44.SetMatrix44RowMajor({1.f, trans_->skewX_, 0.f, 0.f,
            trans_->skewY_, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f});
#endif
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
#ifndef USE_ROSEN_DRAWING
        matrix_.preTranslate(x_, y_);
#else
        matrix_.PreTranslate(x_, y_);
#endif
    } else {
        // Translate
        if ((x_ + trans_->translateX_ != 0) || (y_ + trans_->translateY_ != 0)) {
#ifndef USE_ROSEN_DRAWING
            matrix_.preTranslate(x_ + trans_->translateX_, y_ + trans_->translateY_);
#else
            matrix_.PreTranslate(x_ + trans_->translateX_, y_ + trans_->translateY_);
#endif
        }
        // rotation
        if (!ROSEN_EQ(trans_->rotation_, 0.f, EPSILON)) {
#ifndef USE_ROSEN_DRAWING
            matrix_.preRotate(trans_->rotation_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
#else
            matrix_.PreRotate(trans_->rotation_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
#endif
        }
        // Skew
        ApplySkewToMatrix(matrix_);
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
#ifndef USE_ROSEN_DRAWING
            matrix_.preScale(trans_->scaleX_, trans_->scaleY_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
#else
            matrix_.PreScale(trans_->scaleX_, trans_->scaleY_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
#endif
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
#ifndef USE_ROSEN_DRAWING
        SkM44 matrix3D;
        // Translate
        matrix3D.setTranslate(trans_->pivotX_ * width_ + x_ + trans_->translateX_,
            trans_->pivotY_ * height_ + y_ + trans_->translateY_, z_ + trans_->translateZ_);
#else // USE_ROSEN_DRAWING
        Drawing::Matrix44 matrix3D;
        // Translate
        matrix3D.Translate(trans_->pivotX_ * width_ + x_ + trans_->translateX_,
            trans_->pivotY_ * height_ + y_ + trans_->translateY_, z_ + trans_->translateZ_);
#endif
        // Rotate
        float x = trans_->quaternion_[0];
        float y = trans_->quaternion_[1];
        float z = trans_->quaternion_[2];
        float w = trans_->quaternion_[3];
#ifndef USE_ROSEN_DRAWING
        SkScalar r[16] = {
            1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w), 0,
            2.f * (x * y - z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w), 0,
            2.f * (x * z + y * w), 2.f * (y * z - x * w), 1.f - 2.f * (x * x + y * y), 0,
            0, 0, 0, 1
        };
        SkM44 matrix4 = SkM44::ColMajor(r);
#else // USE_ROSEN_DRAWING
        Drawing::Matrix44::Buffer buffer = {
            1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w), 0,   // m00 ~ m30
            2.f * (x * y - z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w), 0,   // m01 ~ m31
            2.f * (x * z + y * w), 2.f * (y * z - x * w), 1.f - 2.f * (x * x + y * y), 0,   // m02 ~ m32
            0, 0, 0, 1 };                                                                   // m03 ~ m33
        Drawing::Matrix44 matrix4;
        matrix4.SetMatrix44ColMajor(buffer);
#endif
        matrix3D = matrix3D * matrix4;
        // Skew
        ApplySkewToMatrix44(matrix3D, false);
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
#ifndef USE_ROSEN_DRAWING
            matrix3D.preScale(trans_->scaleX_, trans_->scaleY_, 1.f);
#else
            matrix3D.PreScale(trans_->scaleX_, trans_->scaleY_, 1.f);
#endif
        }
        // Translate
#ifndef USE_ROSEN_DRAWING
        matrix3D.preTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_, 0);

        // Concatenate the 3D matrix with the 2D matrix
        matrix_.preConcat(matrix3D.asM33());
#else // USE_ROSEN_DRAWING
        matrix3D.PreTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_, 0);

        // Concatenate the 3D matrix with the 2D matrix
        matrix_.PreConcat(matrix3D);
#endif
    } else {
#ifndef USE_ROSEN_DRAWING
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
        if (trans_->pivotZ_ != 0.f) {
            camera.translate(-sin(trans_->rotationY_ * DEGREE_TO_RADIAN) * trans_->pivotZ_,
                -sin(trans_->rotationX_ * DEGREE_TO_RADIAN) * trans_->pivotZ_,
                (1 - cos(trans_->rotationX_ * DEGREE_TO_RADIAN) * cos(trans_->rotationY_ * DEGREE_TO_RADIAN)) *
                    trans_->pivotZ_);
        }
        camera.rotateX(-trans_->rotationX_);
        camera.rotateY(-trans_->rotationY_);
        camera.rotateZ(-trans_->rotation_);
        camera.getMatrix(&matrix3D);
#else
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
#endif
        // Skew
        ApplySkewToMatrix(matrix3D, false);
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
#ifndef USE_ROSEN_DRAWING
            matrix3D.preScale(trans_->scaleX_, trans_->scaleY_);
#else
            matrix3D.PreScale(trans_->scaleX_, trans_->scaleY_);
#endif
        }
        // Translate
#ifndef USE_ROSEN_DRAWING
        matrix3D.preTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_);

        // Concatenate the 3D matrix with the 2D matrix
        matrix3D.postTranslate(
            trans_->pivotX_ * width_ + x_ + trans_->translateX_, trans_->pivotY_ * height_ + y_ + trans_->translateY_);
        matrix_.preConcat(matrix3D);
#else
        matrix3D.PreTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_);

        // Concatenate the 3D matrix with the 2D matrix
        matrix3D.PostTranslate(
            trans_->pivotX_ * width_ + x_ + trans_->translateX_, trans_->pivotY_ * height_ + y_ + trans_->translateY_);
        matrix_.PreConcat(matrix3D);
#endif
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
#ifndef USE_ROSEN_DRAWING
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
#else
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
#endif

        // Set the absolute rectangle's properties
        absRect.left_ = static_cast<int>(xRange[0]);
        absRect.top_ = static_cast<int>(yRange[0]);
        absRect.width_ = static_cast<int>(std::ceil(xRange[1] - absRect.left_));
        absRect.height_ = static_cast<int>(std::ceil(yRange[1] - absRect.top_));
    } else {
        // Calculate the absolute rectangle based on the matrix's translation and scaling
#ifndef USE_ROSEN_DRAWING
        absRect.left_ = static_cast<int>(rect.left_ * matrix.getScaleX() + matrix.getTranslateX());
        absRect.top_ = static_cast<int>(rect.top_ * matrix.getScaleY() + matrix.getTranslateY());
        float right = (rect.left_ + rect.width_) * matrix.getScaleX() + matrix.getTranslateX();
        float bottom = (rect.top_ + rect.height_) * matrix.getScaleY() + matrix.getTranslateY();
        absRect.width_ = static_cast<int>(std::ceil(right - absRect.left_));
        absRect.height_ = static_cast<int>(std::ceil(bottom - absRect.top_));
#else
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
#endif
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

#ifndef USE_ROSEN_DRAWING
void RSObjAbsGeometry::SetContextMatrix(const std::optional<SkMatrix>& matrix)
#else
void RSObjAbsGeometry::SetContextMatrix(const std::optional<Drawing::Matrix>& matrix)
#endif
{
    contextMatrix_ = matrix;
}

#ifndef USE_ROSEN_DRAWING
const SkMatrix& RSObjAbsGeometry::GetMatrix() const
#else
const Drawing::Matrix& RSObjAbsGeometry::GetMatrix() const
#endif
{
    return matrix_;
}

#ifndef USE_ROSEN_DRAWING
const SkMatrix& RSObjAbsGeometry::GetAbsMatrix() const
#else
const Drawing::Matrix& RSObjAbsGeometry::GetAbsMatrix() const
#endif
{
    // if absMatrix_ is empty, return matrix_ instead
    return absMatrix_ ? *absMatrix_ : matrix_;
}
} // namespace Rosen
} // namespace OHOS

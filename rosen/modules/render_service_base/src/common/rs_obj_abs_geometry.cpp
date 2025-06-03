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
#include "utils/camera3d.h"

namespace OHOS {
namespace Rosen {
constexpr unsigned RECT_POINT_NUM = 4;
constexpr unsigned LEFT_TOP_POINT = 0;
constexpr unsigned RIGHT_TOP_POINT = 1;
constexpr unsigned RIGHT_BOTTOM_POINT = 2;
constexpr unsigned LEFT_BOTTOM_POINT = 3;
constexpr float INCH_TO_PIXEL = 72;
constexpr float EPSILON = 1e-6f;
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
void RSObjAbsGeometry::UpdateMatrix(const Drawing::Matrix* parentMatrix, const std::optional<Drawing::Point>& offset)
{
    // Initialize the absolute matrix with the absolute matrix of the parent view if the parent view exists
    if (parentMatrix == nullptr) {
        absMatrix_.reset();
    } else {
        absMatrix_ = *parentMatrix;
        if (offset.has_value()) {
            absMatrix_->PreTranslate(offset->GetX(), offset->GetY());
        }
    }
    // Reset the matrix of the current view
    matrix_.Reset();
    // filter invalid width and height
    if (IsEmpty()) {
        return;
    }
    // If the view has no transformations or only 2D transformations, update the absolute matrix with 2D
    // transformations
    if (!trans_ || (ROSEN_EQ(trans_->translateZ_, 0.f) && ROSEN_EQ(trans_->rotationX_, 0.f) &&
        ROSEN_EQ(trans_->rotationY_, 0.f) && trans_->quaternion_.IsIdentity() && ROSEN_EQ(trans_->scaleZ_, 1.f))) {
        UpdateAbsMatrix2D();
    } else {
        // Otherwise, update the absolute matrix with 3D transformations
        UpdateAbsMatrix3D();
    }
    // If the context matrix of the current view exists, update the current matrix with it
    if (contextMatrix_.has_value()) {
        matrix_.PostConcat(*contextMatrix_);
    }
    // If the absolute matrix of the current view exists, update it with the context matrix and the current matrix
    if (absMatrix_.has_value()) {
        absMatrix_->PreConcat(matrix_);
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
        ROSEN_EQ(trans_->rotationY_, 0.f) && trans_->quaternion_.IsIdentity() && ROSEN_EQ(trans_->scaleZ_, 1.f))) {
        UpdateAbsMatrix2D();
    } else {
        // Otherwise, update the absolute matrix with 3D transformations
        UpdateAbsMatrix3D();
    }

    // If the context matrix of the view exists, update the current matrix with it
    if (contextMatrix_.has_value()) {
        matrix_.PostConcat(*contextMatrix_);
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

namespace {
    void ApplySkewToMatrix44(const RSTransform& trans, Drawing::Matrix44& m44, bool preConcat)
    {
        if (!ROSEN_EQ(trans.skewX_, 0.f, EPSILON) || !ROSEN_EQ(trans.skewY_, 0.f, EPSILON)) {
            Drawing::Matrix44 skewM44 {};
            skewM44.SetMatrix44RowMajor({1.f, trans.skewX_, 0.f, 0.f,
                trans.skewY_, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                0.f, 0.f, 0.f, 1.f});
            if (preConcat) {
                m44 = m44 * skewM44;
            } else {
                m44 = skewM44 * m44;
            }
        }
    }

    void ApplyPerspToMatrix(const RSTransform& trans, Drawing::Matrix& m, bool preConcat)
    {
        if (!ROSEN_EQ(trans.perspX_, 0.f, EPSILON)
            || !ROSEN_EQ(trans.perspY_, 0.f, EPSILON)
            || !ROSEN_EQ(trans.perspW_, 1.f, EPSILON)) {
            Drawing::Matrix perspM {};
            perspM.SetMatrix(1.f, 0.f, 0.f,
                0.f, 1.f, 0.f,
                trans.perspX_, trans.perspY_, trans.perspW_);
            if (preConcat) {
                m = m * perspM;
            } else {
                m = perspM * m;
            }
        }
    }

    void ApplyPerspToMatrix44(const RSTransform& trans, Drawing::Matrix44& m44, bool preConcat)
    {
        if (!ROSEN_EQ(trans.perspX_, 0.f, EPSILON)
            || !ROSEN_EQ(trans.perspY_, 0.f, EPSILON)
            || !ROSEN_EQ(trans.perspZ_, 0.f, EPSILON)
            || !ROSEN_EQ(trans.perspW_, 1.f, EPSILON)) {
            Drawing::Matrix44 perspM44 {};
            perspM44.SetMatrix44RowMajor({1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                trans.perspX_, trans.perspY_, trans.perspZ_, trans.perspW_});
            if (preConcat) {
                m44 = m44 * perspM44;
            } else {
                m44 = perspM44 * m44;
            }
        }
    }
}

void RSObjAbsGeometry::UpdateAbsMatrix2D()
{
    if (!trans_) {
        matrix_.PreTranslate(x_, y_);
    } else {
        // Persp
        if (!ROSEN_EQ(trans_->perspX_, 0.f, EPSILON)
            || !ROSEN_EQ(trans_->perspY_, 0.f, EPSILON)
            || !ROSEN_EQ(trans_->perspW_, 1.f, EPSILON)) {
            matrix_.PreTranslate(trans_->pivotX_ * width_, trans_->pivotY_ * height_);
            ApplyPerspToMatrix(trans_.value(), matrix_, true);
            matrix_.PreTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_);
        }

        // Local Translate
        if (!ROSEN_EQ(trans_->translateX_, 0.f, EPSILON) || !ROSEN_EQ(trans_->translateY_, 0.f, EPSILON)) {
            matrix_.PreTranslate(trans_->translateX_, trans_->translateY_);
        }

        // rotation
        if (!ROSEN_EQ(trans_->rotation_, 0.f, EPSILON)) {
            matrix_.PreRotate(trans_->rotation_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
        }

        // Skew
        if (!ROSEN_EQ(trans_->skewX_, 0.f, EPSILON) || !ROSEN_EQ(trans_->skewY_, 0.f, EPSILON)) {
            matrix_.PreSkew(trans_->skewX_, trans_->skewY_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
        }

        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix_.PreScale(trans_->scaleX_, trans_->scaleY_, trans_->pivotX_ * width_, trans_->pivotY_ * height_);
        }

        // Global Translate
        if (!ROSEN_EQ(x_, 0.f, EPSILON) || !ROSEN_EQ(y_, 0.f, EPSILON)) {
            matrix_.PostTranslate(x_, y_);
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
        // Pivot
        matrix3D.Translate(trans_->pivotX_ * width_, trans_->pivotY_ * height_, 0);

        // Persp
        ApplyPerspToMatrix44(trans_.value(), matrix3D, true);

        // Translate
        matrix3D.PreTranslate(trans_->translateX_, trans_->translateY_, trans_->translateZ_);

        // Rotate
        float x = trans_->quaternion_[0];
        float y = trans_->quaternion_[1];
        float z = trans_->quaternion_[2];
        float w = trans_->quaternion_[3];
        float magnitudeRecp = 1.0f / sqrt((x * x) + (y * y) + (z * z) + (w * w));
        x *= magnitudeRecp;
        y *= magnitudeRecp;
        z *= magnitudeRecp;
        w *= magnitudeRecp;
        Drawing::Matrix44::Buffer buffer = {
            1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w), 0,   // m00 ~ m30
            2.f * (x * y - z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w), 0,   // m01 ~ m31
            2.f * (x * z + y * w), 2.f * (y * z - x * w), 1.f - 2.f * (x * x + y * y), 0,   // m02 ~ m32
            0, 0, 0, 1 };                                                                   // m03 ~ m33
        Drawing::Matrix44 matrix4;
        matrix4.SetMatrix44ColMajor(buffer);
        matrix3D = matrix3D * matrix4;

        // Skew
        ApplySkewToMatrix44(trans_.value(), matrix3D, true);

        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f) || !ROSEN_EQ(trans_->scaleZ_, 1.f)) {
            matrix3D.PreScale(trans_->scaleX_, trans_->scaleY_, trans_->scaleZ_);
        }

        // Translate
        matrix3D.PreTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_, 0);

        matrix3D.PostTranslate(x_, y_);

        // Concatenate the 3D matrix with the 2D matrix
        matrix_.PreConcat(matrix3D);
    } else {
        Drawing::Matrix matrix3D;
        Drawing::Camera3D camera;
        // Z Position
        if (isZApplicableCamera3D_) {
            camera.Translate(0, 0, z_ + trans_->translateZ_);
        } else {
            camera.Translate(0, 0, trans_->translateZ_);
        }

        // Set camera distance
        if (trans_->cameraDistance_ == 0) {
            float zOffSet = sqrt(width_ * width_ + height_ * height_) / 2;
            camera.SetCameraPos(0, 0, camera.GetCameraPosZ() - zOffSet / INCH_TO_PIXEL);
        } else {
            camera.SetCameraPos(0, 0, trans_->cameraDistance_);
        }
        // Rotate
        if (!ROSEN_EQ(trans_->pivotZ_, 0.f, EPSILON)) {
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
        if (!ROSEN_EQ(trans_->skewX_, 0.f, EPSILON) || !ROSEN_EQ(trans_->skewY_, 0.f, EPSILON)) {
            matrix3D.PreSkew(trans_->skewX_, trans_->skewY_);
        }
        // Scale
        if (!ROSEN_EQ(trans_->scaleX_, 1.f) || !ROSEN_EQ(trans_->scaleY_, 1.f)) {
            matrix3D.PreScale(trans_->scaleX_, trans_->scaleY_);
        }
        // Pivot
        matrix3D.PreTranslate(-trans_->pivotX_ * width_, -trans_->pivotY_ * height_);

        // Translate
        matrix3D.PostTranslate(x_ + trans_->translateX_, y_ + trans_->translateY_);
        // PostPersp
        ApplyPerspToMatrix(trans_.value(), matrix3D, false);
        // Pivot
        matrix3D.PostTranslate(trans_->pivotX_ * width_, trans_->pivotY_ * height_);

        // Concatenate the 3D matrix with the 2D matrix
        matrix_.PreConcat(matrix3D);
    }
}

void RSObjAbsGeometry::SetAbsRect()
{
    absRect_ = MapAbsRect(RectF(0.f, 0.f, width_, height_));
}

RectI RSObjAbsGeometry::DeflateToRectI(const RectF& rect)
{
    RectI dstRect;
    dstRect.left_ = static_cast<int>(std::ceil(rect.GetLeft()));
    dstRect.top_ = static_cast<int>(std::ceil(rect.GetTop()));
    dstRect.width_ = static_cast<int>(std::floor(rect.GetRight() - dstRect.left_));
    dstRect.height_ = static_cast<int>(std::floor(rect.GetBottom() - dstRect.top_));
    return dstRect;
}

RectI RSObjAbsGeometry::InflateToRectI(const RectF& rect)
{
    RectI dstRect;
    dstRect.left_ = static_cast<int>(std::floor(rect.GetLeft()));
    dstRect.top_ = static_cast<int>(std::floor(rect.GetTop()));
    dstRect.width_ = static_cast<int>(std::ceil(rect.GetRight() - dstRect.left_));
    dstRect.height_ = static_cast<int>(std::ceil(rect.GetBottom() - dstRect.top_));
    return dstRect;
}

RectF RSObjAbsGeometry::MapRectWithoutRounding(const RectF& rect, const Drawing::Matrix& matrix)
{
    RectF absRect;
    // Check if the matrix has skew or negative scaling
    if (!ROSEN_EQ(matrix.Get(Drawing::Matrix::PERSP_0), 0.f, EPSILON) ||
        !ROSEN_EQ(matrix.Get(Drawing::Matrix::PERSP_1), 0.f, EPSILON) ||
        !ROSEN_EQ(matrix.Get(Drawing::Matrix::PERSP_2), 0.f, EPSILON)) {
        Drawing::RectF src(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        Drawing::RectF dst;
        matrix.MapRect(dst, src);
        absRect.left_ = dst.GetLeft();
        absRect.top_ = dst.GetTop();
        absRect.width_ = dst.GetRight() - absRect.left_;
        absRect.height_ = dst.GetBottom() - absRect.top_;
    } else if (!ROSEN_EQ(matrix.Get(Drawing::Matrix::SKEW_X), 0.f) || (matrix.Get(Drawing::Matrix::SCALE_X) < 0) ||
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
        absRect.left_ = xRange[0];
        absRect.top_ = yRange[0];
        absRect.width_ = xRange[1] - absRect.left_;
        absRect.height_ = yRange[1] - absRect.top_;
    } else {
        // Calculate the absolute rectangle based on the matrix's translation and scaling
        Drawing::scalar transX = matrix.Get(Drawing::Matrix::TRANS_X);
        Drawing::scalar transY = matrix.Get(Drawing::Matrix::TRANS_Y);
        Drawing::scalar scaleX = matrix.Get(Drawing::Matrix::SCALE_X);
        Drawing::scalar scaleY = matrix.Get(Drawing::Matrix::SCALE_Y);
        absRect.left_ = rect.left_ * scaleX + transX;
        absRect.top_ = rect.top_ * scaleY + transY;
        float right = (rect.left_ + rect.width_) * scaleX + transX;
        float bottom = (rect.top_ + rect.height_) * scaleY + transY;
        absRect.width_ = right - absRect.left_;
        absRect.height_ = bottom - absRect.top_;
    }
    return absRect;
}

/**
 * Map the rectangle with specific matrix
 * [planning] replaced by Drawing::MapRect
 * @param rect the rectangle to map
 * @param matrix the specific to map
 * @return the mapped absolute rectangle
 */
RectI RSObjAbsGeometry::MapRect(const RectF& rect, const Drawing::Matrix& matrix)
{
    return InflateToRectI(MapRectWithoutRounding(rect, matrix));
}

/**
 * Map a set of rectangles (i.e. region) with specific matrix.
 * @param region: the region to map
 * @return the mapped region
 */
Occlusion::Region RSObjAbsGeometry::MapRegion(const Occlusion::Region& region, const Drawing::Matrix& matrix)
{
    std::vector<RectI> regionRects = region.GetRegionRectIs();
    Occlusion::Region mappedRegion;
    for (const auto& rect : regionRects) {
        mappedRegion.OrSelf(Occlusion::Region{ Occlusion::Rect { MapRect(rect.ConvertTo<float>(), matrix) } });
    }
    return mappedRegion;
}

/**
 * Map the absolute rectangle
 * @param rect the rectangle to map
 * @return the mapped absolute rectangle
 */
RectI RSObjAbsGeometry::MapAbsRect(const RectF& rect) const
{
    return MapRect(rect, GetAbsMatrix());
}

/**
 * Map a set of rectangles (i.e. region) to the absolute coordinates.
 * @param region: the region to map
 * @return the mapped absolute region
 */
Occlusion::Region RSObjAbsGeometry::MapAbsRegion(const Occlusion::Region& region) const
{
    return MapRegion(region, GetAbsMatrix());
}

Vector2f RSObjAbsGeometry::GetDataRange(float d0, float d1, float d2, float d3)
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
    return {min, max};
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

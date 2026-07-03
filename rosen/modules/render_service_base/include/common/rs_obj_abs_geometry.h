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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H

#include <memory>
#include <optional>

#include "utils/matrix.h"
#include "utils/matrix44.h"
#include "utils/point.h"

#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_obj_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSObjAbsGeometry : public RSObjGeometry {
public:
    RSObjAbsGeometry();
    ~RSObjAbsGeometry() override;
    void ConcatMatrix(const Drawing::Matrix& matrix);

    void UpdateMatrix(const Drawing::Matrix* parentMatrix, const std::optional<Drawing::Point>& offset);

    void ReplaceMatrix(const Drawing::Matrix& matrix, const Drawing::Matrix& absMatrix)
    {
        if (!matrix.IsIdentity()) {
            matrix_ = matrix;
        }
        if (!absMatrix.IsIdentity()) {
            absMatrix_ = absMatrix;
        }
        SetAbsRect();
    }

    // Using by RenderService
    void UpdateByMatrixFromSelf();

    void SetPivotX(float x)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->pivotX_ = x;
    }
    void SetPivotY(float y)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->pivotY_ = y;
    }
    void SetPivotZ(float z)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->pivotZ_ = z;
    }
    void SetPivot(float x, float y)
    {
        SetPivotX(x);
        SetPivotY(y);
    }
    void SetScaleX(float x)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->scaleX_ = x;
    }
    void SetScaleY(float y)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->scaleY_ = y;
    }
    void SetScaleZ(float z)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->scaleZ_ = z;
    }
    void SetScale(float x, float y)
    {
        SetScaleX(x);
        SetScaleY(y);
    }
    void SetScale(float x, float y, float z)
    {
        SetScaleX(x);
        SetScaleY(y);
        SetScaleZ(z);
    }
    void SetSkewX(float x)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->skewX_ = x;
    }
    void SetSkewY(float y)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->skewY_ = y;
    }
    void SetSkewZ(float z)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->skewZ_ = z;
    }
    void SetSkew(float x, float y)
    {
        SetSkewX(x);
        SetSkewY(y);
    }
    void SetSkew(float x, float y, float z)
    {
        SetSkewX(x);
        SetSkewY(y);
        SetSkewZ(z);
    }
    void SetPerspX(float x)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->perspX_ = x;
    }
    void SetPerspY(float y)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->perspY_ = y;
    }
    void SetPerspZ(float z)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->perspZ_ = z;
    }
    void SetPerspW(float w)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->perspW_ = w;
    }
    void SetPersp(float x, float y)
    {
        SetPerspX(x);
        SetPerspY(y);
    }
    void SetPersp(float x, float y, float z, float w)
    {
        SetPerspX(x);
        SetPerspY(y);
        SetPerspZ(z);
        SetPerspW(w);
    }
    void SetRotation(float rotation)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->rotation_ = rotation;
    }
    void SetRotationX(float rotationX)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->rotationX_ = rotationX;
    }
    void SetRotationY(float rotationY)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->rotationY_ = rotationY;
    }
    void SetTranslateX(float translateX)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->translateX_ = translateX;
    }
    void SetTranslateY(float translateY)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->translateY_ = translateY;
    }
    void SetTranslateZ(float translateZ)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        if (!ROSEN_EQ(trans_->translateZ_, translateZ)) {
            trans_->translateZ_ = translateZ;
        }
    }
    void SetCameraDistance(float cameraDistance)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->cameraDistance_ = cameraDistance;
    }
    void SetQuaternion(const Quaternion& quaternion)
    {
        if (!trans_) {
            trans_ = std::make_optional<RSTransform>();
        }
        trans_->quaternion_ = quaternion;
    }

    float GetPivotX() const
    {
        return trans_ ? trans_->pivotX_ : 0.5f;
    }
    float GetPivotY() const
    {
        return trans_ ? trans_->pivotY_ : 0.5f;
    }
    float GetPivotZ() const
    {
        return trans_ ? trans_->pivotZ_ : 0.f;
    }
    float GetScaleX() const
    {
        return trans_ ? trans_->scaleX_ : 1.f;
    }
    float GetScaleY() const
    {
        return trans_ ? trans_->scaleY_ : 1.f;
    }
    float GetScaleZ() const
    {
        return trans_ ? trans_->scaleZ_ : 1.f;
    }
    float GetSkewX() const
    {
        return trans_ ? trans_->skewX_ : 0.f;
    }
    float GetSkewY() const
    {
        return trans_ ? trans_->skewY_ : 0.f;
    }
    float GetSkewZ() const
    {
        return trans_ ? trans_->skewZ_ : 0.f;
    }
    float GetPerspX() const
    {
        return trans_ ? trans_->perspX_ : 0.f;
    }
    float GetPerspY() const
    {
        return trans_ ? trans_->perspY_ : 0.f;
    }
    float GetPerspZ() const
    {
        return trans_ ? trans_->perspZ_ : 0.f;
    }
    float GetPerspW() const
    {
        return trans_ ? trans_->perspW_ : 1.f;
    }
    float GetRotation() const
    {
        return trans_ ? trans_->rotation_ : 0.f;
    }
    float GetRotationX() const
    {
        return trans_ ? trans_->rotationX_ : 0.f;
    }
    float GetRotationY() const
    {
        return trans_ ? trans_->rotationY_ : 0.f;
    }
    float GetTranslateX() const
    {
        return trans_ ? trans_->translateX_ : 0.f;
    }
    float GetTranslateY() const
    {
        return trans_ ? trans_->translateY_ : 0.f;
    }
    float GetTranslateZ() const
    {
        return trans_ ? trans_->translateZ_ : 0.f;
    }
    float GetCameraDistance() const
    {
        return trans_ ? trans_->cameraDistance_ : 0.f;
    }
    Quaternion GetQuaternion() const
    {
        return trans_ ? trans_->quaternion_ : Quaternion();
    }

    const RectI& GetAbsRect() const
    {
        return absRect_;
    }
    RectI MapAbsRectWithMatrix(const RectF& rect, const Drawing::Matrix& matrix) const;
    RectI MapAbsRect(const RectF& rect) const;
    Occlusion::Region MapAbsRegion(const Occlusion::Region& region) const;

    // Converts RectF to RectI by inward rounding (ceil for left/top, floor for right/bottom)
    // to ensure the resulting integer rect is fully contained within the original floating-point rect.
    // attention: used in render node's opaque area calculations
    static RectI DeflateToRectI(const RectF& rect);

    // Converts a RectF to RectI by outward rounding (floor for left/top, ceil for right/bottom)
    // to ensure the original floating-point rect is fully contained within the resulting integer rect.
    // attention: used in render node's draw area calculations
    static RectI InflateToRectI(const RectF& rect);

    static RectF MapRectWithoutRounding(const RectF& rect, const Drawing::Matrix& matrix);
    static RectI MapRect(const RectF& rect, const Drawing::Matrix& matrix);
    static Occlusion::Region MapRegion(const Occlusion::Region& region, const Drawing::Matrix& matrix);

    // return transform matrix (context + self)
    // warning: If the parent node does not have the SandBox attribute, this interface does
    // NOT cause problems. Otherwise, you need to use the GetAbsMatrix interface to multiply
    // the AbsMatrix of the parent node by the left to obtain the matrix of the parent node.
    const Drawing::Matrix& GetMatrix() const;
    // return transform matrix (parent + context + self)
    const Drawing::Matrix& GetAbsMatrix() const;

    bool IsNeedClientCompose() const;

    void SetContextMatrix(const std::optional<Drawing::Matrix>& matrix);

    // used in: subTree skiped but need to calculate drawBehindWindow region
    void SetAbsMatrix(const std::optional<Drawing::Matrix>& matrix);

private:
    void UpdateAbsMatrix2D();
    void UpdateAbsMatrix3D();
    void SetAbsRect();

    static Vector2f GetDataRange(float d0, float d1, float d2, float d3);

    RectI absRect_;
    Drawing::Matrix matrix_;
    std::optional<Drawing::Matrix> absMatrix_;
    std::optional<Drawing::Matrix> contextMatrix_;
    std::optional<RSTransform> trans_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H

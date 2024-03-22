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
#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSObjAbsGeometry : public RSObjGeometry {
public:
    RSObjAbsGeometry();
    ~RSObjAbsGeometry() override;
    void ConcatMatrix(const Drawing::Matrix& matrix);
    void UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent, const std::optional<Drawing::Point>& offset,
        const std::optional<Drawing::Rect>& clipRect);

    // Using by RenderService
    void UpdateByMatrixFromSelf();

    const RectI& GetAbsRect() const
    {
        return absRect_;
    }
    RectI MapAbsRect(const RectF& rect) const;
    RectI MapRect(const RectF& rect, const Drawing::Matrix& matrix) const;

    // return transform matrix (context + self)
    const Drawing::Matrix& GetMatrix() const;
    // return transform matrix (parent + context + self)
    const Drawing::Matrix& GetAbsMatrix() const;

    bool IsNeedClientCompose() const;

    void SetContextMatrix(const std::optional<Drawing::Matrix>& matrix);

    void Reset() override
    {
        RSObjGeometry::Reset();
        absMatrix_.reset();
        contextMatrix_.reset();
    }

private:
<<<<<<< HEAD
=======
    void ApplySkewToMatrix(Drawing::Matrix& m, bool preConcat = true);
    void ApplySkewToMatrix44(Drawing::Matrix44& m44, bool preConcat = true);
>>>>>>> zhangpeng/master
    void UpdateAbsMatrix2D();
    void UpdateAbsMatrix3D();
    void SetAbsRect();

    Vector2f GetDataRange(float d0, float d1, float d2, float d3) const;

    RectI absRect_;
    Drawing::Matrix matrix_;
    std::optional<Drawing::Matrix> absMatrix_;
    std::optional<Drawing::Matrix> contextMatrix_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H

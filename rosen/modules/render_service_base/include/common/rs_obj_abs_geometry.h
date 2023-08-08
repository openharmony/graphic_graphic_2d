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

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkMatrix.h"
#include "include/core/SkPoint.h"
#else
#include "utils/matrix.h"
#include "utils/point.h"
#endif

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
#ifndef USE_ROSEN_DRAWING
    void ConcatMatrix(const SkMatrix& matrix);
    void UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent, const std::optional<SkPoint>& offset,
        const std::optional<SkRect>& clipRect);
#else
    void ConcatMatrix(const Drawing::Matrix& matrix);
    void UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent, const std::optional<Drawing::Point>& offset,
        const std::optional<Drawing::Rect>& clipRect);
#endif

    // Using by RenderService
    void UpdateByMatrixFromSelf();

    const RectI& GetAbsRect() const
    {
        return absRect_;
    }
    RectI MapAbsRect(const RectF& rect) const;

#ifndef USE_ROSEN_DRAWING
    // return transform matrix (context + self)
    const SkMatrix& GetMatrix() const;
    // return transform matrix (parent + context + self)
    const SkMatrix& GetAbsMatrix() const;
#else
    // return transform matrix (context + self)
    const Drawing::Matrix& GetMatrix() const;
    // return transform matrix (parent + context + self)
    const Drawing::Matrix& GetAbsMatrix() const;
#endif

    bool IsNeedClientCompose() const;

#ifndef USE_ROSEN_DRAWING
    void SetContextMatrix(const std::optional<SkMatrix>& matrix);
#else
    void SetContextMatrix(const std::optional<Drawing::Matrix>& matrix);
#endif

    void Reset() override
    {
        RSObjGeometry::Reset();
        absMatrix_.reset();
        contextMatrix_.reset();
    }

private:
    void UpdateAbsMatrix2D();
    void UpdateAbsMatrix3D();
    void SetAbsRect();

    Vector2f GetDataRange(float d0, float d1, float d2, float d3) const;

    RectI absRect_;
#ifndef USE_ROSEN_DRAWING
    SkMatrix matrix_;
    std::optional<SkMatrix> absMatrix_;
    std::optional<SkMatrix> contextMatrix_;
#else
    Drawing::Matrix matrix_;
    std::optional<Drawing::Matrix> absMatrix_;
    std::optional<Drawing::Matrix> contextMatrix_;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H

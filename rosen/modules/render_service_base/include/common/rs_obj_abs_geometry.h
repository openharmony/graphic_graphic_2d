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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H

#include "include/core/SkMatrix.h"
#include "include/core/SkPoint.h"
#include <memory>

#include "common/rs_matrix3.h"
#include "common/rs_obj_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {
class RSObjAbsGeometry : public RSObjGeometry {
public:
    RSObjAbsGeometry();
    ~RSObjAbsGeometry() override;
    void UpdateMatrix(const Matrix3f& matrix);
    void UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent, float offsetX, float offsetY);
    const RectI& GetAbsRect() const
    {
        return absRect_;
    }
    RectI MapAbsRect(const RectF& rect) const;
    const SkMatrix& GetMatrix() const
    {
        return matrix_;
    }
    bool IsPointInHotZone(const float x, const float y) const;

private:
    void UpdateAbsMatrix2D();
    void UpdateAbsMatrix3D();
    void SetAbsRect();
    float GetCross(const SkPoint& p1, const SkPoint& p2, const SkPoint& p) const;
    Vector2f GetDataRange(float d0, float d1, float d2, float d3) const;
    bool IsPointInLine(const SkPoint& p1, const SkPoint& p2, const SkPoint& p, const float crossRes) const;
    RectI absRect_;
    SkMatrix matrix_;
    SkMatrix absMatrix_;
    SkPoint vertices_[4];
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H

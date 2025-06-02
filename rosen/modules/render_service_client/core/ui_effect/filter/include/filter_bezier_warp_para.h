/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_FILTER_BEZIER_WARP_PARA_H
#define UIEFFECT_FILTER_BEZIER_WARP_PARA_H
#include <iostream>
#include "common/rs_vector2.h"
#include "filter_para.h"
constexpr size_t BEZIER_WARP_POINT_NUM = 12; // 12 anchor points of a patch

namespace OHOS {
namespace Rosen {
class BezierWarpPara : public FilterPara {
public:
    BezierWarpPara()
    {
        this->type_ = FilterPara::ParaType::BEZIER_WARP;
    }
    ~BezierWarpPara() override = default;

    void SetBezierControlPoints(const std::array<Vector2f, BEZIER_WARP_POINT_NUM>& bezierCtrlPoints)
    {
        bezierCtrlPoints_ = bezierCtrlPoints;
    }

    const std::array<Vector2f, BEZIER_WARP_POINT_NUM>& GetBezierControlPoints() const
    {
        return bezierCtrlPoints_;
    }

private:
    std::array<Vector2f, BEZIER_WARP_POINT_NUM> bezierCtrlPoints_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_BEZIER_WARP_PARA_H

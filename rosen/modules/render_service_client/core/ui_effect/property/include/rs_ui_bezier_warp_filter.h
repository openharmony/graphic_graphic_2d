/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_BEZIER_WARP_FILTER_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_BEZIER_WARP_FILTER_H
#include "ui_effect/property/include/rs_ui_filter_para_base.h"
#include "ui_effect/filter/include/filter_bezier_warp_para.h"
#include "render/rs_render_bezier_warp_filter.h"

#include "common/rs_vector2.h"
#include "modifier/rs_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSUIBezierWarpFilterPara : public RSUIFilterParaBase {
public:
    RSUIBezierWarpFilterPara() : RSUIFilterParaBase(RSUIFilterType::BEZIER_WARP) {}
    virtual ~RSUIBezierWarpFilterPara() = default;

    bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    virtual void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void SetBezierWarp(const std::shared_ptr<BezierWarpPara>& bezierWarp);

    virtual std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter() override;

    virtual std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties() override;

private:
    bool CreateRenderFilterCtrlPoints(const std::shared_ptr<RSRenderBezierWarpFilterPara>& frProperty,
        const RSUIFilterType type);

    std::array<RSUIFilterType, BEZIER_WARP_POINT_NUM> bezierCtrlPointsType_ = {
        RSUIFilterType::BEZIER_CONTROL_POINT0,
        RSUIFilterType::BEZIER_CONTROL_POINT1,
        RSUIFilterType::BEZIER_CONTROL_POINT2,
        RSUIFilterType::BEZIER_CONTROL_POINT3,
        RSUIFilterType::BEZIER_CONTROL_POINT4,
        RSUIFilterType::BEZIER_CONTROL_POINT5,
        RSUIFilterType::BEZIER_CONTROL_POINT6,
        RSUIFilterType::BEZIER_CONTROL_POINT7,
        RSUIFilterType::BEZIER_CONTROL_POINT8,
        RSUIFilterType::BEZIER_CONTROL_POINT9,
        RSUIFilterType::BEZIER_CONTROL_POINT10,
        RSUIFilterType::BEZIER_CONTROL_POINT11
    };
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_BEZIER_WARP_FILTER_H


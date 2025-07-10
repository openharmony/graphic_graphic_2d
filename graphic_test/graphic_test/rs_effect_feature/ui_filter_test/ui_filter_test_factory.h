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

#ifndef UI_FILTER_TEST_FACTORY_H
#define UI_FILTER_TEST_FACTORY_H

#include <memory>
#include <variant>
#include <vector>
#include <array>

#include "render/rs_render_pixel_map_mask.h"
#include "ui_effect/filter/include/filter_bezier_warp_para.h"
#include "ui_effect/filter/include/filter_color_gradient_para.h"
#include "ui_effect/filter/include/filter_content_light_para.h"
#include "ui_effect/filter/include/filter_dispersion_para.h"
#include "ui_effect/filter/include/filter_displacement_distort_para.h"
#include "ui_effect/filter/include/filter_edge_light_para.h"
#include "ui_effect/filter/include/filter_para.h"
#include "ui_effect/mask/include/pixel_map_mask_para.h"
#include "ui_effect/mask/include/radial_gradient_mask_para.h"
#include "ui_effect/mask/include/ripple_mask_para.h"
#include "ui_effect/property/include/rs_ui_filter.h"
#include "ui_effect/property/include/rs_ui_mask_para.h"

namespace OHOS {
namespace Rosen {
struct BlurParams { float radius; }; // range: > 0.9
struct PixelStretchParams {
    Vector4f stretchPercent; // range: [-1, 1]
    Drawing::TileMode stretchTileMode;
};
struct WaterRippleParams {
    float progress; // range: [0, 1]
    uint32_t waveCount; // range: [1, 3]
    float rippleCenterX; // range: [0, 1]
    float rippleCenterY; // range: [0, 1]
    uint32_t mode; // range: [0, 3]
};
struct FlyOutParams {
    float degree; // range: [0, 1]
    uint32_t flyMode; // range: 0, 1
};
struct DistortParams { float distortionK; }; // range: [-1, 1]
struct RadiusGradientBlurParams {
    float blurRadius; // range: [0, 128]
    std::vector<std::pair<float, float>> fractionStops; // range: [0, 1]
    GradientDirection direction;
};
struct DisplacementDistortParams {
    Vector2f factor; // range: [0.0, 10.0]
    std::shared_ptr<MaskPara> maskPara;
};
struct ColorGradientParams {
    std::vector<float> colors;  // len：[0, 12], range: [0, 1]
    std::vector<float> positions; // len：[0, 12], range: [0, 1]
    std::vector<float> strengths; // len：[0, 12], range: >= 0
    std::shared_ptr<MaskPara> maskPara;
};
struct EdgeLightParams {
    float alpha; // range: [0, 1]
    Vector4f color;
    bool bloom;
    std::shared_ptr<MaskPara> maskPara;
};
struct BezierWarpParams {
    static constexpr size_t bezierControlPointsCount = 12;
    std::array<Vector2f, bezierControlPointsCount> points; // range: [-1, 1]
};
struct DispersionParams {
    float opacity;
    Vector2f redOffset;
    Vector2f greenOffset;
    Vector2f blueOffset;
    std::shared_ptr<MaskPara> maskPara;
};
struct HdrParams { float brightness; }; // range: [1.0, 设备当前支持最大提亮倍数]
struct ContentLightParams {
    Vector3f lightPosition; // x分量取值范围[-10, 10]，y分量取值范围[-10, 10]，z分量取值范围[0, 10]
    Vector4f lightColor; // [0, 1]
    float lightIntensity; // [0, 1]
};

using FilterParams = std::variant<
    BlurParams,
    PixelStretchParams,
    WaterRippleParams,
    FlyOutParams,
    DistortParams,
    RadiusGradientBlurParams,
    DisplacementDistortParams,
    ColorGradientParams,
    EdgeLightParams,
    BezierWarpParams,
    DispersionParams,
    HdrParams,
    ContentLightParams
>;

struct FilterTestData {
    FilterPara::ParaType type;
    FilterParams params;
};

class FilterTestFactory {
public:
    static std::unique_ptr<Filter> CreateFilter(FilterTestData& data);
    static void AddFilterPara(std::unique_ptr<Filter>& filter, FilterTestData& data);
};

} // namespace Rosen
} // namespace OHOS
#endif // UI_FILTER_TEST_FACTORY_H

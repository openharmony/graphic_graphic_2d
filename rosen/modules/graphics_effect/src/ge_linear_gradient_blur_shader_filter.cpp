/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "ge_linear_gradient_blur_shader_filter.h"

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {

namespace {
static bool GetMaskLinearBlurEnabled()
{
    // Determine whether the mask LinearBlur render should be enabled. The default value is 0,
    // which means that it is unenabled.
    static bool enabled =
        std::atoi((system::GetParameter("persist.sys.graphic.maskLinearBlurEnabled", "1")).c_str()) != 0;
    return enabled;
}
} // namespace

GELinearGradientBlurShaderFilter::GELinearGradientBlurShaderFilter(
    const Drawing::GELinearGradientBlurShaderFilterParams& params)
{
    geoWidth_ = params.geoWidth;
    geoHeight_ = params.geoHeight;
    auto maskLinearBlur = GetMaskLinearBlurEnabled();
    linearGradientBlurPara_ = std::make_shared<GELinearGradientBlurPara>(
        params.blurRadius, params.fractionStops, static_cast<GEGradientDirection>(params.direction), maskLinearBlur);
    mat_ = params.mat;
    tranX_ = params.surfaceWidth;
    tranY_ = params.surfaceHeight;
}

GELinearGradientBlurShaderFilter::~GELinearGradientBlurShaderFilter() = default;

std::shared_ptr<Drawing::Image> GELinearGradientBlurShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    (void)canvas;
    (void)src;
    (void)dst;
    return image;
}

} // namespace Rosen
} // namespace OHOS

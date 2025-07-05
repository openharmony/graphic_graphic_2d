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

#include "ui_filter_test_factory.h"

namespace OHOS {
namespace Rosen {

namespace {
void AddBlurPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<FilterBlurPara>();
    para->SetRadius(std::get<BlurParams>(params).radius);
    filter->AddPara(para);
}

void AddPixelStretchPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<PixelStretchPara>();
    auto& pixelParams = std::get<PixelStretchParams>(params);
    para->SetStretchPercent(pixelParams.stretchPercent);
    para->SetTileMode(pixelParams.stretchTileMode);
    filter->AddPara(para);
}

void AddWaterRipplePara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<WaterRipplePara>();
    auto& rippleParams = std::get<WaterRippleParams>(params);
    para->SetProgress(rippleParams.progress);
    para->SetWaveCount(rippleParams.waveCount);
    para->SetRippleCenterX(rippleParams.rippleCenterX);
    para->SetRippleCenterY(rippleParams.rippleCenterY);
    para->SetRippleMode(rippleParams.mode);
    filter->AddPara(para);
}

void AddFlyOutPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<FlyOutPara>();
    auto& flyParams = std::get<FlyOutParams>(params);
    para->SetDegree(flyParams.degree);
    para->SetFlyMode(flyParams.flyMode);
    filter->AddPara(para);
}

void AddDistortPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<DistortPara>();
    para->SetDistortionK(std::get<DistortParams>(params).distortionK);
    filter->AddPara(para);
}

void AddRadiusGradientBlurPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<RadiusGradientBlurPara>();
    auto& blurParams = std::get<RadiusGradientBlurParams>(params);
    para->SetBlurRadius(blurParams.blurRadius);
    para->SetFractionStops(blurParams.fractionStops);
    para->SetDirection(blurParams.direction);
    filter->AddPara(para);
}

void AddDisplacementDistortPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<DisplacementDistortPara>();
    auto& distortParams = std::get<DisplacementDistortParams>(params);
    para->SetFactor(distortParams.factor);
    para->SetMask(distortParams.maskPara);
    filter->AddPara(para);
}

void AddColorGradientPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<ColorGradientPara>();
    auto& gradientParams = std::get<ColorGradientParams>(params);
    para->SetColors(gradientParams.colors);
    para->SetPositions(gradientParams.positions);
    para->SetStrengths(gradientParams.strengths);
    para->SetMask(gradientParams.maskPara);
    filter->AddPara(para);
}

void AddEdgeLightPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<EdgeLightPara>();
    auto& edgeParams = std::get<EdgeLightParams>(params);
    para->SetAlpha(edgeParams.alpha);
    para->SetColor(edgeParams.color);
    para->SetBloom(edgeParams.bloom);
    para->SetMask(edgeParams.maskPara);
    filter->AddPara(para);
}

void AddBezierWarpPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<BezierWarpPara>();
    para->SetBezierControlPoints(std::get<BezierWarpParams>(params).points);
    filter->AddPara(para);
}

void AddDispersionPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<DispersionPara>();
    auto& dispersionParams = std::get<DispersionParams>(params);
    para->SetOpacity(dispersionParams.opacity);
    para->SetRedOffset(dispersionParams.redOffset);
    para->SetGreenOffset(dispersionParams.greenOffset);
    para->SetBlueOffset(dispersionParams.blueOffset);
    para->SetMask(dispersionParams.maskPara);
    filter->AddPara(para);
}

void AddHdrBrightnessRatioPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<HDRBrightnessRatioPara>();
    para->SetBrightnessRatio(std::get<HdrParams>(params).brightness);
    filter->AddPara(para);
}

void AddContentLightPara(std::unique_ptr<Filter>& filter, FilterParams& params)
{
    auto para = std::make_shared<ContentLightPara>();
    auto& lightParams = std::get<ContentLightParams>(params);
    para->SetLightColor(lightParams.lightColor);
    para->SetLightPosition(lightParams.lightPosition);
    para->SetLightIntensity(lightParams.lightIntensity);
    filter->AddPara(para);
}
} // namespace

std::unique_ptr<Filter> FilterTestFactory::CreateFilter(FilterTestData& data)
{
    auto filter = std::make_unique<Filter>();
    AddFilterPara(filter, data);
    return filter;
}

void FilterTestFactory::AddFilterPara(std::unique_ptr<Filter>& filter, FilterTestData& data)
{
    switch (data.type) {
        case FilterPara::ParaType::BLUR:
            AddBlurPara(filter, data.params);
            break;
        case FilterPara::ParaType::PIXEL_STRETCH:
            AddPixelStretchPara(filter, data.params);
            break;
        case FilterPara::ParaType::WATER_RIPPLE:
            AddWaterRipplePara(filter, data.params);
            break;
        case FilterPara::ParaType::FLY_OUT:
            AddFlyOutPara(filter, data.params);
            break;
        case FilterPara::ParaType::DISTORT:
            AddDistortPara(filter, data.params);
            break;
        case FilterPara::ParaType::RADIUS_GRADIENT_BLUR:
            AddRadiusGradientBlurPara(filter, data.params);
            break;
        case FilterPara::ParaType::DISPLACEMENT_DISTORT:
            AddDisplacementDistortPara(filter, data.params);
            break;
        case FilterPara::ParaType::COLOR_GRADIENT:
            AddColorGradientPara(filter, data.params);
            break;
        case FilterPara::ParaType::SOUND_WAVE:
            filter->AddPara(nullptr);
            break;
        case FilterPara::ParaType::EDGE_LIGHT:
            AddEdgeLightPara(filter, data.params);
            break;
        case FilterPara::ParaType::BEZIER_WARP:
            AddBezierWarpPara(filter, data.params);
            break;
        case FilterPara::ParaType::DISPERSION:
            AddDispersionPara(filter, data.params);
            break;
        case FilterPara::ParaType::HDR_BRIGHTNESS_RATIO:
            AddHdrBrightnessRatioPara(filter, data.params);
            break;
        case FilterPara::ParaType::CONTENT_LIGHT:
            AddContentLightPara(filter, data.params);
            break;
        default:
            break;
    }
}

} // namespace Rosen
} // namespace OHOS

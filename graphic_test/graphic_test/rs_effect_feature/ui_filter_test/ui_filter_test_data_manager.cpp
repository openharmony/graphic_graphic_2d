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

#include "ui_filter_test_data_manager.h"

namespace OHOS {
namespace Rosen {

namespace {

struct SharedMaskParams {
    std::shared_ptr<MaskPara> baseMaskPara;
    std::shared_ptr<RippleMaskPara> rippleMaskPara;
    std::shared_ptr<RadialGradientMaskPara> radialGradientMaskPara;
    std::shared_ptr<PixelMapMaskPara> pixelMapMaskPara;
};

SharedMaskParams InitializeSharedMaskParameters()
{
    SharedMaskParams params;
    
    params.baseMaskPara = std::make_shared<MaskPara>();
    
    params.rippleMaskPara = std::make_shared<RippleMaskPara>();
    Vector2f rippleCenter {0.5f, 0.5f};
    params.rippleMaskPara->SetCenter(rippleCenter);
    params.rippleMaskPara->SetRadius(0.5f);
    params.rippleMaskPara->SetWidth(0.5f);
    params.rippleMaskPara->SetWidthCenterOffset(0.0f);

    params.radialGradientMaskPara = std::make_shared<RadialGradientMaskPara>();
    Vector2f gradientCenter {0.5f, 0.5f};
    params.radialGradientMaskPara->SetCenter(gradientCenter);
    params.radialGradientMaskPara->SetRadiusX(0.5f);
    params.radialGradientMaskPara->SetRadiusY(0.5f);
    std::vector<float> colors = {0.5f, 0.5f, 0.5f, 0.5f};
    params.radialGradientMaskPara->SetColors(colors);
    std::vector<float> positions = {0.5f, 0.5f};
    params.radialGradientMaskPara->SetPositions(positions);
    
    auto testPixelMap = UIFilterTestDataManager::CreatePixelMap(100, 100);
    params.pixelMapMaskPara = std::make_shared<PixelMapMaskPara>();
    params.pixelMapMaskPara->SetPixelMap(testPixelMap);
    params.pixelMapMaskPara->SetSrc({0.1f, 0.2f, 0.6f, 0.9f});
    params.pixelMapMaskPara->SetDst({0.4f, 0.3f, 0.7f, 0.5f});
    params.pixelMapMaskPara->SetFillColor({0.3f, 0.6f, 0.1f, 0.7f});

    return params;
}

std::vector<FilterTestData> CreateTestDataInvalidMin()
{
    return {
        { FilterPara::ParaType::BLUR, BlurParams { -1.0f } },
        { FilterPara::ParaType::PIXEL_STRETCH,
            PixelStretchParams { { -1.5f, -1.5f, -1.5f, -1.5f }, Drawing::TileMode::CLAMP } },
        { FilterPara::ParaType::WATER_RIPPLE,
            WaterRippleParams { -1, 0, -1, -1, -1} },
        { FilterPara::ParaType::FLY_OUT, FlyOutParams { -1, -1 } },
        { FilterPara::ParaType::DISTORT, DistortParams { -2 } },
        { FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
            RadiusGradientBlurParams { -1, { { -1, -1 } }, GradientDirection::NONE } },
        { FilterPara::ParaType::DISPLACEMENT_DISTORT, DisplacementDistortParams { { -1, -1 }, nullptr } },
        { FilterPara::ParaType::COLOR_GRADIENT,
            ColorGradientParams { { -1, -1, -1, -1 }, { -1, -1 }, { -1 }, nullptr } },
        { FilterPara::ParaType::EDGE_LIGHT, EdgeLightParams { -1, { -1, -1, -1, -1 }, false, nullptr } },
        { FilterPara::ParaType::BEZIER_WARP,
            BezierWarpParams { { { { 0.5, 0.5 }, { 0.5, 0.5 }, { 0.5, 0.5 }, { 0.5, 0.5 },
                { 0.5, 0.5 }, { 0.5, 0.5 }, { 0.5, 0.5 }, { 0.5, 0.5 },
                { 0.5, 0.5 }, { 0.5, 0.5 }, { 0.5, 0.5 }, { 0.5, 0.5 } } } } },
        { FilterPara::ParaType::DISPERSION, DispersionParams { -1, { -1, -1 }, { -1, -1 }, { -1, -1 } } },
        { FilterPara::ParaType::HDR_BRIGHTNESS_RATIO, HdrParams { -1 } },
        { FilterPara::ParaType::CONTENT_LIGHT,
            ContentLightParams { { -20, -20, -1 }, { -1, -1, -1, -1 }, -1 } }
    };
}

std::vector<FilterTestData> CreateTestDataValidData1(const SharedMaskParams& maskParams)
{
    return {
        { FilterPara::ParaType::BLUR, BlurParams { 1.0f } },
        { FilterPara::ParaType::PIXEL_STRETCH,
            PixelStretchParams { { 0.1f, 0.1f, 0.1f, 0.1f }, Drawing::TileMode::CLAMP } },
        { FilterPara::ParaType::WATER_RIPPLE, WaterRippleParams { 0.1f, 1, 0.2f, 0.2f, 0 } },
        { FilterPara::ParaType::FLY_OUT, FlyOutParams { 0.1f, 0 } },
        { FilterPara::ParaType::DISTORT, DistortParams { -0.2f } },
        { FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
            RadiusGradientBlurParams { 10.0f, { { 0.1f, 0.1f } }, GradientDirection::LEFT } },
        { FilterPara::ParaType::DISPLACEMENT_DISTORT,
            DisplacementDistortParams { { 1.0f, 1.0f }, maskParams.baseMaskPara } },
        { FilterPara::ParaType::COLOR_GRADIENT,
            ColorGradientParams { { 0.1f }, { 0.1f }, { 1.0f }, maskParams.baseMaskPara } },
        { FilterPara::ParaType::EDGE_LIGHT,
            EdgeLightParams { 0.1f, { 0.1f, 0.1f, 0.1f, 0.1f }, false, maskParams.baseMaskPara } },
        { FilterPara::ParaType::BEZIER_WARP,
            BezierWarpParams { { { { 1.0f, 1.0f }, { 0.66f, 1.0f }, { 0.33f, 1.0f }, { 0, 1.0f },
                { 0, 0.66f }, { 0, 0.33f }, { 0, 0 }, { 0.33f, 0 },
                { 0.66f, 0 }, { 1.0f, 0 }, { 1.0f, 0.33f }, { 1.0f, 0.66f } } } } },
        { FilterPara::ParaType::DISPERSION,
            DispersionParams { 0.1f, { 0.1f, 0.1f }, { 0.1f, 0.1f }, { 0.1f, 0.1f }, maskParams.baseMaskPara } },
        { FilterPara::ParaType::HDR_BRIGHTNESS_RATIO, HdrParams { 1.1f } },
        { FilterPara::ParaType::CONTENT_LIGHT,
            ContentLightParams { { 1.0f, 1.0f, 1.0f }, { 0.1f, 0.1f, 0.1f, 0.1f }, 0.1f } }
    };
}

std::vector<FilterTestData> CreateTestDataValidData2(const SharedMaskParams& maskParams)
{
    return {
        { FilterPara::ParaType::BLUR, BlurParams { 2.0f } },
        { FilterPara::ParaType::PIXEL_STRETCH,
            PixelStretchParams { { 0.3f, 0.3f, 0.3f, 0.3f }, Drawing::TileMode::MIRROR } },
        { FilterPara::ParaType::WATER_RIPPLE, WaterRippleParams { 0.4f, 2, 0.5f, 0.5f, 1 } },
        { FilterPara::ParaType::FLY_OUT, FlyOutParams { 0.4f, 0 } },
        { FilterPara::ParaType::DISTORT, DistortParams { 0.0f } },
        { FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
            RadiusGradientBlurParams { 45.0f, { { 0.3f, 0.3f } }, GradientDirection::TOP } },
        { FilterPara::ParaType::DISPLACEMENT_DISTORT,
            DisplacementDistortParams { { 3.0f, 3.0f }, maskParams.rippleMaskPara } },
        { FilterPara::ParaType::COLOR_GRADIENT,
            ColorGradientParams { { 0.3f, 0.3f }, { 0.3f, 0.3f }, { 1.5f }, maskParams.rippleMaskPara } },
        { FilterPara::ParaType::EDGE_LIGHT,
            EdgeLightParams { 0.4f, { 0.3f, 0.3f, 0.3f, 0.3f }, true, maskParams.rippleMaskPara } },
        { FilterPara::ParaType::BEZIER_WARP,
            BezierWarpParams { { { { 0.3f, 0.3f }, { 0.4f, 0.2f }, { 0.6f, 0.2f }, { 0.7f, 0.3f },
                { 0.8f, 0.4f }, { 0.8f, 0.6f }, { 0.7f, 0.7f }, { 0.6f, 0.8f },
                { 0.4f, 0.8f }, { 0.3f, 0.7f }, { 0.2f, 0.6f }, { 0.2f, 0.4f } } } } },
        { FilterPara::ParaType::DISPERSION,
            DispersionParams { 0.4f, { 0.3f, 0.3f }, { 0.3f, 0.3f }, { 0.3f, 0.3f }, maskParams.rippleMaskPara } },
        { FilterPara::ParaType::HDR_BRIGHTNESS_RATIO, HdrParams { 1.6f } },
        { FilterPara::ParaType::CONTENT_LIGHT,
            ContentLightParams { { 3.0f, 3.0f, 3.0f }, { 0.3f, 0.3f, 0.3f, 0.3f }, 0.4f } }
    };
}

std::vector<FilterTestData> CreateTestDataValidData3(const SharedMaskParams& maskParams)
{
    return {
        { FilterPara::ParaType::BLUR, BlurParams { 3.0f } },
        { FilterPara::ParaType::PIXEL_STRETCH,
            PixelStretchParams { { 0.6f, 0.6f, 0.6f, 0.6f }, Drawing::TileMode::REPEAT } },
        { FilterPara::ParaType::WATER_RIPPLE, WaterRippleParams { 0.7f, 2, 0.7f, 0.7f, 2 } },
        { FilterPara::ParaType::FLY_OUT, FlyOutParams { 0.7f, 1 } },
        { FilterPara::ParaType::DISTORT, DistortParams { 0.3f } },
        { FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
            RadiusGradientBlurParams { 80.0f, { { 0.6f, 0.6f } }, GradientDirection::RIGHT } },
        { FilterPara::ParaType::DISPLACEMENT_DISTORT,
            DisplacementDistortParams { { 6.0f, 6.0f }, maskParams.radialGradientMaskPara } },
        { FilterPara::ParaType::COLOR_GRADIENT,
            ColorGradientParams { { 0.6f, 0.6f, 0.6f }, { 0.6f, 0.6f, 0.6f }, { 2.0f },
            maskParams.radialGradientMaskPara } },
        { FilterPara::ParaType::EDGE_LIGHT,
            EdgeLightParams { 0.7f, { 0.6f, 0.6f, 0.6f, 0.6f }, true, maskParams.radialGradientMaskPara } },
        { FilterPara::ParaType::BEZIER_WARP,
            BezierWarpParams { { { { 0, 0 }, { 0.33f, 0 }, { 0.66f, 0 }, { 1.0f, 0 },
                { 1.0f, 0.33f }, { 1.0f, 0.66f }, { 1.0f, 1.0f }, { 0.66f, 1.0f },
                { 0.33f, 1.0f }, { 0, 1.0f }, { 0, 0.66f }, { 0, 0.33f } } } } },
        { FilterPara::ParaType::DISPERSION,
            DispersionParams { 0.7f, { 0.6f, 0.6f }, { 0.6f, 0.6f }, { 0.6f, 0.6f },
            maskParams.radialGradientMaskPara } },
        { FilterPara::ParaType::HDR_BRIGHTNESS_RATIO, HdrParams { 2.1f } },
        { FilterPara::ParaType::CONTENT_LIGHT,
            ContentLightParams { { 6.0f, 6.0f, 6.0f }, { 0.6f, 0.6f, 0.6f, 0.6f }, 0.7f } }
    };
}

std::vector<FilterTestData> CreateTestDataValidData4(const SharedMaskParams& maskParams)
{
    return {
        { FilterPara::ParaType::BLUR, BlurParams { 4.0f } },
        { FilterPara::ParaType::PIXEL_STRETCH,
            PixelStretchParams { { 0.9f, 0.9f, 0.9f, 0.9f }, Drawing::TileMode::CLAMP } },
        { FilterPara::ParaType::WATER_RIPPLE, WaterRippleParams { 1.0f, 3, 0.9f, 0.9f, 3 } },
        { FilterPara::ParaType::FLY_OUT, FlyOutParams { 1.0f, 1 } },
        { FilterPara::ParaType::DISTORT, DistortParams { 0.9f } },
        { FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
            RadiusGradientBlurParams { 128.0f, { { 0.9f, 0.9f } }, GradientDirection::BOTTOM } },
        { FilterPara::ParaType::DISPLACEMENT_DISTORT,
            DisplacementDistortParams { { 10.0f, 10.0f }, maskParams.pixelMapMaskPara } },
        { FilterPara::ParaType::COLOR_GRADIENT,
            ColorGradientParams { { 0.9f, 0.9f, 0.9f }, { 0.9f, 0.9f, 0.9f }, { 3.0f }, maskParams.pixelMapMaskPara } },
        { FilterPara::ParaType::EDGE_LIGHT,
            EdgeLightParams { 1.0f, { 0.9f, 0.9f, 0.9f, 0.9f }, true, maskParams.pixelMapMaskPara } },
        { FilterPara::ParaType::BEZIER_WARP,
            BezierWarpParams { { { { 1.2f, 0.2f }, { 0.33f, -0.3f }, { 0.66f, -0.3f }, { 1.2f, -0.2f },
                { 1.3f, 0.33f }, { 1.3f, 0.66f }, { 0.2f, 1.2f }, { 0.66f, 1.3f },
                { 0.33f, 1.3f }, { 0.2f, 1.2f }, { -1.3f, 0.66f }, { -1.3f, 0.33f } } } } },
        { FilterPara::ParaType::DISPERSION,
            DispersionParams { 1.0f, { 0.9f, 0.9f }, { 0.9f, 0.9f }, { 0.9f, 0.9f }, maskParams.pixelMapMaskPara } },
        { FilterPara::ParaType::HDR_BRIGHTNESS_RATIO, HdrParams { 2.5f } },
        { FilterPara::ParaType::CONTENT_LIGHT,
            ContentLightParams { { 10.0f, 10.0f, 10.0f }, { 0.9f, 0.9f, 0.9f, 0.9f }, 1.0f } }
    };
}

std::vector<FilterTestData> CreateTestDataInvalidMax()
{
    return {
        { FilterPara::ParaType::BLUR, BlurParams { 999.0f } },
        { FilterPara::ParaType::PIXEL_STRETCH,
            PixelStretchParams { { 10.0f, -10.0f, 10.0f, -10.0f }, Drawing::TileMode::CLAMP } },
        { FilterPara::ParaType::WATER_RIPPLE, WaterRippleParams { 999.0f, 999, 999.0f, 999.0f, 999 } },
        { FilterPara::ParaType::FLY_OUT, FlyOutParams { 999.0f, 999 } },
        { FilterPara::ParaType::DISTORT, DistortParams { 999.0f } },
        { FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
            RadiusGradientBlurParams { 9999.0f, { { 999.0f, 999.0f } }, GradientDirection::NONE } },
        { FilterPara::ParaType::DISPLACEMENT_DISTORT,
            DisplacementDistortParams { { 999.0f, 999.0f }, nullptr } },
        { FilterPara::ParaType::COLOR_GRADIENT,
            ColorGradientParams { { 999.0f, 999.0f, 999.0f }, { 999.0f, 999.0f }, { 999.0f }, nullptr } },
        { FilterPara::ParaType::EDGE_LIGHT,
            EdgeLightParams { 999.0f, { 999.0f, 999.0f, 999.0f, 999.0f }, true, nullptr } },
        { FilterPara::ParaType::BEZIER_WARP,
            BezierWarpParams { { { { -999.0f, -999.0f }, { 0.33f, -999.0f }, { 0.66f, -999.0f },
                { 999.0f, -999.0f }, { 999.0f, 0.33f }, { 999.0f, 0.66f },
                { 999.0f, 999.0f }, { 0.66f, 999.0f }, { 0.33f, 999.0f },
                { -999.0f, 999.0f }, { -999.0f, 0.66f }, { -999.0f, 0.33f } } } } },
        { FilterPara::ParaType::DISPERSION,
            DispersionParams { 999.0f, { 999.0f, 999.0f }, { 999.0f, 999.0f }, { 999.0f, 999.0f }, nullptr } },
        { FilterPara::ParaType::HDR_BRIGHTNESS_RATIO, HdrParams { 999.0f } },
        { FilterPara::ParaType::CONTENT_LIGHT,
            ContentLightParams { { 999.0f, 999.0f, 999.0f }, { 999.0f, 999.0f, 999.0f, 999.0f }, 999.0f } }
    };
}

} // namespace

void UIFilterTestDataManager::Initialize()
{
    CreateFilterGroups();
}

Filter* UIFilterTestDataManager::GetFilter(FilterPara::ParaType type, TestDataGroupType groupIdx)
{
    if (auto it = filterGroups_.find(groupIdx); it != filterGroups_.end()) {
        return it->second[type].get();
    }
    return nullptr;
}

Filter* UIFilterTestDataManager::GetCombinedFilter(std::vector<FilterPara::ParaType>& types, TestDataGroupType groupIdx)
{
    auto filter = std::make_unique<Filter>();
    if (auto it = filterGroups_.find(groupIdx); it != filterGroups_.end()) {
        for (auto type : types) {
            if (auto testDataIt = std::find_if(
                GetTestDatas(groupIdx).begin(),
                GetTestDatas(groupIdx).end(),
                [type](FilterTestData& testData) {
                    return testData.type == type;
                });
                testDataIt != GetTestDatas(groupIdx).end()) {
                FilterTestFactory::AddFilterPara(filter, *testDataIt);
            }
        }
    }
    static std::vector<std::unique_ptr<Filter>> persistentFilters;
    persistentFilters.push_back(std::move(filter));
    return persistentFilters.back().get();
}

std::vector<FilterTestData>& UIFilterTestDataManager::GetTestDatas(TestDataGroupType groupIdx)
{
    static auto maskParams = InitializeSharedMaskParameters();
    static std::vector<std::vector<FilterTestData>> allTestDatas = {
        CreateTestDataInvalidMin(),
        CreateTestDataValidData1(maskParams),
        CreateTestDataValidData2(maskParams),
        CreateTestDataValidData3(maskParams),
        CreateTestDataValidData4(maskParams),
        CreateTestDataInvalidMax()
    };
    return allTestDatas.at(static_cast<size_t>(groupIdx));
}

void UIFilterTestDataManager::CreateFilterGroups()
{
    for (auto i = static_cast<int>(TestDataGroupType::INVALID_DATA_MIN);
        i <= static_cast<int>(TestDataGroupType::INVALID_DATA_MAX); ++i) {
        auto& testDatas = GetTestDatas(static_cast<TestDataGroupType>(i));
        std::unordered_map<FilterPara::ParaType, std::unique_ptr<Filter>> group;
        for (auto& testData : testDatas) {
            group[testData.type] = FilterTestFactory::CreateFilter(testData);
        }
        filterGroups_[static_cast<TestDataGroupType>(i)] = std::move(group);
    }
}

std::shared_ptr<Media::PixelMap> UIFilterTestDataManager::CreatePixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelMap = Media::PixelMap::Create(opts);
    if (pixelMap == nullptr) {
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelMap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(pixelMap->GetWidth(), pixelMap->GetHeight(), format);
    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(bitmap);
    auto canvas = surface->GetCanvas();
    if (canvas == nullptr) {
        return nullptr;
    }
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    return pixelMap;
}

} // namespace Rosen
} // namespace OHOS

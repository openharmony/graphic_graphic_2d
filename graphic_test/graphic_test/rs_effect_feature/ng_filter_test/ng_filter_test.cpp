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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

using MaskCreator = std::function<std::shared_ptr<RSNGMaskBase>()>;
using MaskConvertor = std::function<std::shared_ptr<RSNGMaskBase>(std::shared_ptr<MaskPara>)>;
using FilterCreator = std::function<std::shared_ptr<RSNGFilterBase>()>;
using FilterConvertor = std::function<std::shared_ptr<RSNGFilterBase>(std::shared_ptr<FilterPara>)>;

static std::unordered_map<RSNGEffectType, MaskCreator> creatorMask = {
    {RSNGEffectType::DOUBLE_RIPPLE_MASK,
     [] {
         return std::make_shared<RSNGDoubleRippleMask>();
     }},
};

static std::unordered_map<RSNGEffectType, FilterCreator> creatorFilter = {
    {RSNGEffectType::DISPLACEMENT_DISTORT,
     [] {
         return std::make_shared<RSNGDispDistortFilter>();
     }},
    {RSNGEffectType::COLOR_GRADIENT,
     [] {
         return std::make_shared<RSNGColorGradientFilter>();
     }},
    {RSNGEffectType::FROSTED_GLASS,
     [] {
         return std::make_shared<RSNGFrostedGlassFilter>();
     }},
    {RSNGEffectType::GRID_WARP,
     [] {
         return std::make_shared<RSNGGridWarpFilter>();
     }},
};

// Default values
const Vector2f defaultBlurParams = Vector2f(48.0f, 4.0f);
const Vector2f defaultWeightsEmboss = Vector2f(1.0f, 1.0f); // (envLight, sd)
const Vector2f defaultWeightsEdl = Vector2f(1.0f, 1.0f); // (envLight, sd)
const Vector3f defaultRefractParams = Vector3f(0.010834f, 0.007349f, 1.2f);
const Vector2f defaultBgRates = Vector2f(-0.00003f, 1.2f);
const Vector3f defaultBgKBS = Vector3f(0.010834f, 0.007349f, 1.2f);
const Vector3f defaultBgPos = Vector3f(0.3f, 0.5f, 1.0f);
const Vector3f defaultBgNeg = Vector3f(0.5f, 0.5f, 1.0f);
const Vector3f defaultSdParams = Vector3f(0.0f, 2.0f, 0.0f);
const Vector2f defaultSdRates = Vector2f(0.0f, 0.0f);
const Vector3f defaultSdKBS = Vector3f(-0.02f, 2.0f, 4.62f);
const Vector3f defaultSdPos = Vector3f(1.0f, 1.5f, 2.0f);
const Vector3f defaultSdNeg = Vector3f(1.7f, 3.0f, 1.0f);
const Vector3f defaultEnvLightParams = Vector3f(0.8f, 0.2745f, 2.0f);
const Vector2f defaultEnvLightRates = Vector2f(0.0f, 0.0f);
const Vector3f defaultEnvLightKBS = Vector3f(0.8f, 0.2745f, 2.0f);
const Vector3f defaultEnvLightPos = Vector3f(1.0f, 1.5f, 2.0f);
const Vector3f defaultEnvLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
const Vector2f defaultEdLightParams = Vector2f(2.0f, -1.0f);
const Vector2f defaultEdLightAngles = Vector2f(30.0f, 30.0f);
const Vector2f defaultEdLightDir = Vector2f(-1.0f, 1.0f);
const Vector2f defaultEdLightRates = Vector2f(0.0f, 0.0f);
const Vector3f defaultEdLightKBS = Vector3f(0.6027f, 0.64f, 2.0f);
const Vector3f defaultEdLightPos = Vector3f(1.0f, 1.5f, 2.0f);
const Vector3f defaultEdLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
const Vector2f defaultBorderSize = Vector2f(200.0f, 100.0f);
const float DEFAULT_CORNER_RADIUS = 100.0f;
 
// Param arrays
std::vector<Vector2f> blurparamsParams = {
    Vector2f{2.0f, -1.0f},
    Vector2f{0.0f, -1.0f},
    Vector2f{-2.0f, -1.0f},
    Vector2f{2.0f, 0.0f},
    Vector2f{2.0f, 1.0f},
    Vector2f{std::numeric_limits<float>::infinity(), -1.0f}
};

std::vector<Vector2f> weightsEmbossParams = {
    Vector2f{2.0f, -1.0f},
    Vector2f{0.0f, -1.0f},
    Vector2f{-2.0f, -1.0f},
    Vector2f{2.0f, 0.0f},
    Vector2f{2.0f, 1.0f},
    Vector2f{std::numeric_limits<float>::infinity(), -1.0f}
};

std::vector<Vector2f> weightsEdlParams = {
    Vector2f{2.0f, -1.0f},
    Vector2f{0.0f, -1.0f},
    Vector2f{-2.0f, -1.0f},
    Vector2f{2.0f, 0.0f},
    Vector2f{2.0f, 1.0f},
    Vector2f{std::numeric_limits<float>::infinity(), -1.0f}
};

std::vector<Vector2f> bgRatesParams = {
    Vector2f{2.0f, -1.0f},
    Vector2f{0.0f, -1.0f},
    Vector2f{-2.0f, -1.0f},
    Vector2f{2.0f, 0.0f},
    Vector2f{2.0f, 1.0f},
    Vector2f{std::numeric_limits<float>::infinity(), -1.0f}
};

std::vector<Vector3f> bgKBSParams = {
    Vector3f{0.8f, 70.0f, 2.0f},
    Vector3f{0.0f, 70.0f, 2.0f},
    Vector3f{1.0f, 70.0f, 2.0f},
    Vector3f{0.8f, 0.0f, 2.0f},
    Vector3f{0.8f, 180.0f, 2.0f},
    Vector3f{0.8f, 70.0f, 0.0f},
    Vector3f{std::numeric_limits<float>::infinity(), 70.0f, 2.0f}
};

std::shared_ptr<RSNGMaskBase> CreateMask(RSNGEffectType type)
{
    auto it = creatorMask.find(type);
    return it != creatorMask.end() ? it->second() : nullptr;
}

std::shared_ptr<RSNGFilterBase> CreateFilter(RSNGEffectType type)
{
    auto it = creatorFilter.find(type);
    return it != creatorFilter.end() ? it->second() : nullptr;
}

constexpr int DOUBLE_RIPPLE_MASK_PARAMS_COUNT = 7;
std::vector<std::array<float, DOUBLE_RIPPLE_MASK_PARAMS_COUNT>> doubleRippleMaskParams = {
    {-20.0f, -20.0f, -20.0f, -20.0f, -1.0f, -1.0f, -1.0f},
    {0.15f, 0.15f, 0.15f, 0.15f, 0.4f, 0.35f, 0.75f},
    {0.25f, 0.25f, 0.75f, 0.75f, 0.5f, 0.5f, 0.5f},
    {0.15f, 0.15f, 0.65f, 0.65f, 0.8f, 0.15f, 0.35f},
    {10.0f, 10.0f, 0.75f, 0.75f, 0.5f, 0.5f, 12.0f},
    {20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f},
};

constexpr int DISPLACEMENT_DISTORT_PARAMS_COUNT = 2;
std::vector<std::array<float, DISPLACEMENT_DISTORT_PARAMS_COUNT>> displacementDistortParams = {
    {-1, -1}, {6.0f, 6.0f}, {6.0f, 6.0f}, {10.0f, 10.0f}, {3.0f, 7.0f}, {999.0f, 999.0f}
};

constexpr int COLOR_GRADIENT_PARAMS_COUNT = 7;
std::vector<std::array<float, COLOR_GRADIENT_PARAMS_COUNT>> colorGradientParams = {
    {-1, -1, -1, -1, -1, -1, -1},
    {0.5f, 0.6f, 0.9f, 0.9f, 0.9f, 0.9f, 3.0f},
    {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 1.5f},
    {0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 3.0f},
    {0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f},
    {999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f},
};

constexpr int GRID_WARP_POINT_PARAMS_COUNT = 9;
std::vector<std::array<Vector2f, GRID_WARP_POINT_PARAMS_COUNT>> gridWarpPointParams = {
    {
        Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f },
        Vector2f{ 0.0f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f },
        Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.5f, 0.0f }, Vector2f{ 1.0f, 0.0f },
        Vector2f{ 0.0f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 1.0f, 0.5f },
        Vector2f{ 0.0f, 1.0f }, Vector2f{ 0.5f, 1.0f }, Vector2f{ 1.0f, 1.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.5f, -0.1f }, Vector2f{ 1.0f, 0.0f },
        Vector2f{ -0.1f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 1.1f, 0.5f },
        Vector2f{ 0.0f, 1.0f }, Vector2f{ 0.5f, 1.1f }, Vector2f{ 1.0f, 1.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.5f, 0.1f }, Vector2f{ 1.0f, 0.0f },
        Vector2f{ 0.1f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.9f, 0.5f },
        Vector2f{ 0.0f, 1.0f }, Vector2f{ 0.5f, 0.9f }, Vector2f{ 1.0f, 1.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 1.0f, 0.5f },
        Vector2f{ 0.0f, 1.0f }, Vector2f{ 0.5f, 1.0f }, Vector2f{ 1.0f, 1.0f }
    },
    {
        Vector2f{ -999.0f, 0.0f }, Vector2f{ 0.5f, 0.0f }, Vector2f{ 999.0f, 0.0f },
        Vector2f{ -999.0f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 999.0f, 0.5f },
        Vector2f{ -999.0f, 1.0f }, Vector2f{ 0.5f, 1.0f }, Vector2f{ 999.0f, 999.0f }
    },
};

constexpr int GRID_WARP_ANGLE_PARAMS_COUNT = 9;
std::vector<std::array<Vector2f, GRID_WARP_ANGLE_PARAMS_COUNT>> gridWarpAngleParams = {
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ -999.0f, -999.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }
    },
    {
        Vector2f{ 15.0f, 15.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 15.0f, 15.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 15.0f, 15.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 15.0f, 15.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 15.0f, 15.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 15.0f, 15.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 15.0f, 15.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 15.0f, 15.0f }, Vector2f{ 0.0f, 0.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 999.0f, 999.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 999.0f, 999.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 999.0f, 999.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 999.0f, 999.0f }, Vector2f{ 0.0f, 0.0f }
    },
};

std::vector<Vector2f> blurparamsParamsForMaterialFilter = {
    Vector2f{48.0f, 4.0f},
    Vector2f{-48.0f, 4.0f},
    Vector2f{std::numeric_limits<float>::max(), 4.0f},
    Vector2f{std::numeric_limits<float>::min(), 4.0f},
    Vector2f{std::numeric_limits<float>::infinity(), 4.0f}
};

enum class TestDataGroupParamsType {
    INVALID_DATA_MIN,
    VALID_DATA1,
    VALID_DATA2,
    VALID_DATA3,
    INVALID_AND_VALID_DATA,
    INVALID_DATA_MAX,
    COUNT
};

class NGFilterTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    const int filterParaTypeCount = static_cast<int>(FilterPara::ParaType::CONTENT_LIGHT);
};

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Displacement_Distort_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
            Vector2f{ doubleRippleMaskParams[i][0], doubleRippleMaskParams[i][1]});
        doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
            Vector2f{ doubleRippleMaskParams[i][2], doubleRippleMaskParams[i][3]});
        doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[i][4]);
        doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[i][5]);
        doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[i][6]);
        // Create displacement distort filter
        auto filter = CreateFilter(RSNGEffectType::DISPLACEMENT_DISTORT);
        auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
        dispDistortFilter->Setter<DispDistortFactorTag>(
            Vector2f{ displacementDistortParams[i][0], displacementDistortParams[i][1] });
        dispDistortFilter->Setter<DispDistortMaskTag>(mask);
        
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(dispDistortFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Color_Gradient_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
            Vector2f{doubleRippleMaskParams[i][0], doubleRippleMaskParams[i][1]});
        doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
            Vector2f{doubleRippleMaskParams[i][2], doubleRippleMaskParams[i][3]});
        doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[i][4]);
        doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[i][5]);
        doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[i][6]);
        // Create color gradient filter
        auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
        auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
        std::vector<float> colors = {colorGradientParams[i][0], colorGradientParams[i][1],
                                     colorGradientParams[i][2], colorGradientParams[i][3]};
        std::vector<float> positions = {colorGradientParams[i][4], colorGradientParams[i][5]};
        std::vector<float> strengths = {colorGradientParams[i][6]};
        colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
        colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
        colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
        colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_Test)
{
    int columnCount = 1;
    int rowCount = 1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(defaultWeightsEmboss);
        frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(defaultWeightsEdl);

        frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(defaultBgRates);
        frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(defaultBgKBS);
        frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
        frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);

        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
        frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
        frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
        frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);

        frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
        frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
        frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
    
        frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
        frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
        frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
        frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
        frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
        frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
 
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BlurParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(blurparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(blurparamsParams[i]);
        frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(defaultWeightsEmboss);
        frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(defaultWeightsEdl);

        frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(defaultBgRates);
        frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(defaultBgKBS);
        frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
        frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);

        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
        frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
        frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
        frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);

        frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
        frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
        frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
    
        frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
        frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
        frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
        frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
        frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
        frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_WeightsEmbossTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(weightsEmbossParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(weightsEmbossParams[i]);
        frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(defaultWeightsEdl);

        frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(defaultBgRates);
        frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(defaultBgKBS);
        frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
        frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);

        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
        frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
        frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
        frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);

        frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
        frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
        frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
    
        frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
        frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
        frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
        frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
        frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
        frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_WeightsEdlTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(weightsEdlParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(defaultWeightsEmboss);
        frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(weightsEdlParams[i]);
        frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(defaultBgRates);
        frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(defaultBgKBS);
        frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
        frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
        frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
        frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
        frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);
        frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
        frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
        frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
        frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
        frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
        frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
        frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
        frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
        frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);
        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BgRatesTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(bgRatesParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(defaultWeightsEmboss);
        frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(defaultWeightsEdl);
        frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(bgRatesParams[i]);
        frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(defaultBgKBS);
        frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
        frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
        frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
        frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
        frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);
        frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
        frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
        frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
        frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
        frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
        frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
        frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
        frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
        frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);
        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BgKBSTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(bgKBSParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(defaultWeightsEmboss);
        frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(defaultWeightsEdl);
        frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(defaultBgRates);
        frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(bgKBSParams[i]);
        frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
        frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
        frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
        frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
        frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);
        frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
        frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
        frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
        frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
        frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
        frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
        frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
        frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
        frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
        frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);
        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_Frosted_Glass_Material_Filter_Test)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(blurparamsParamsForMaterialFilter.size() + 1);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);
        if (i == blurparamsParamsForMaterialFilter.size()) {
            filter = nullptr;
        } else {
            frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(blurparamsParamsForMaterialFilter[i]);
            frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(defaultWeightsEmboss);
            frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(defaultWeightsEdl);
            frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(defaultBgRates);
            frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(defaultBgKBS);
            frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
            frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
            frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);
            frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
            frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
            frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
            frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
            frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);
            frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
            frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
            frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
            frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
            frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
            frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
            frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
            frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
            frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
            frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
            frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
            frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);
            frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
            frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        }

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        float shrinkX = 25.f;
        float shrinkY = 25.f;
        Rosen::Vector4f materialNodeBounds(x + shrinkX, y + shrinkY, sizeX - 2 * shrinkX, sizeY - 2 * shrinkY);
        auto materialNode = Rosen::RSCanvasNode::Create();
        materialNode->SetBounds(materialNodeBounds);
        materialNode->SetFrame(materialNodeBounds);
        materialNode->SetMaterialNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(materialNode);
        RegisterNode(materialNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Grid_Warp_PointParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create grid warp filter
        auto filter = CreateFilter(RSNGEffectType::GRID_WARP);
        auto gridWarpFilter = std::static_pointer_cast<RSNGGridWarpFilter>(filter);
        gridWarpFilter->Setter<GridWarpGridPoint0Tag>(gridWarpPointParams[i][0]);
        gridWarpFilter->Setter<GridWarpGridPoint1Tag>(gridWarpPointParams[i][1]);
        gridWarpFilter->Setter<GridWarpGridPoint2Tag>(gridWarpPointParams[i][2]);
        gridWarpFilter->Setter<GridWarpGridPoint3Tag>(gridWarpPointParams[i][3]);
        gridWarpFilter->Setter<GridWarpGridPoint4Tag>(gridWarpPointParams[i][4]);
        gridWarpFilter->Setter<GridWarpGridPoint5Tag>(gridWarpPointParams[i][5]);
        gridWarpFilter->Setter<GridWarpGridPoint6Tag>(gridWarpPointParams[i][6]);
        gridWarpFilter->Setter<GridWarpGridPoint7Tag>(gridWarpPointParams[i][7]);
        gridWarpFilter->Setter<GridWarpGridPoint8Tag>(gridWarpPointParams[i][8]);
        gridWarpFilter->Setter<GridWarpRotationAngle0Tag>(gridWarpAngleParams[1][0]);
        gridWarpFilter->Setter<GridWarpRotationAngle1Tag>(gridWarpAngleParams[1][1]);
        gridWarpFilter->Setter<GridWarpRotationAngle2Tag>(gridWarpAngleParams[1][2]);
        gridWarpFilter->Setter<GridWarpRotationAngle3Tag>(gridWarpAngleParams[1][3]);
        gridWarpFilter->Setter<GridWarpRotationAngle4Tag>(gridWarpAngleParams[1][4]);
        gridWarpFilter->Setter<GridWarpRotationAngle5Tag>(gridWarpAngleParams[1][5]);
        gridWarpFilter->Setter<GridWarpRotationAngle6Tag>(gridWarpAngleParams[1][6]);
        gridWarpFilter->Setter<GridWarpRotationAngle7Tag>(gridWarpAngleParams[1][7]);
        gridWarpFilter->Setter<GridWarpRotationAngle8Tag>(gridWarpAngleParams[1][8]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(gridWarpFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Grid_Warp_AngleParamsTest) {
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create grid warp filter
        auto filter = CreateFilter(RSNGEffectType::GRID_WARP);
        auto gridWarpFilter = std::static_pointer_cast<RSNGGridWarpFilter>(filter);
        gridWarpFilter->Setter<GridWarpGridPoint0Tag>(gridWarpPointParams[1][0]);
        gridWarpFilter->Setter<GridWarpGridPoint1Tag>(gridWarpPointParams[1][1]);
        gridWarpFilter->Setter<GridWarpGridPoint2Tag>(gridWarpPointParams[1][2]);
        gridWarpFilter->Setter<GridWarpGridPoint3Tag>(gridWarpPointParams[1][3]);
        gridWarpFilter->Setter<GridWarpGridPoint4Tag>(gridWarpPointParams[1][4]);
        gridWarpFilter->Setter<GridWarpGridPoint5Tag>(gridWarpPointParams[1][5]);
        gridWarpFilter->Setter<GridWarpGridPoint6Tag>(gridWarpPointParams[1][6]);
        gridWarpFilter->Setter<GridWarpGridPoint7Tag>(gridWarpPointParams[1][7]);
        gridWarpFilter->Setter<GridWarpGridPoint8Tag>(gridWarpPointParams[1][8]);
        gridWarpFilter->Setter<GridWarpRotationAngle0Tag>(gridWarpAngleParams[i][0]);
        gridWarpFilter->Setter<GridWarpRotationAngle1Tag>(gridWarpAngleParams[i][1]);
        gridWarpFilter->Setter<GridWarpRotationAngle2Tag>(gridWarpAngleParams[i][2]);
        gridWarpFilter->Setter<GridWarpRotationAngle3Tag>(gridWarpAngleParams[i][3]);
        gridWarpFilter->Setter<GridWarpRotationAngle4Tag>(gridWarpAngleParams[i][4]);
        gridWarpFilter->Setter<GridWarpRotationAngle5Tag>(gridWarpAngleParams[i][5]);
        gridWarpFilter->Setter<GridWarpRotationAngle6Tag>(gridWarpAngleParams[i][6]);
        gridWarpFilter->Setter<GridWarpRotationAngle7Tag>(gridWarpAngleParams[i][7]);
        gridWarpFilter->Setter<GridWarpRotationAngle8Tag>(gridWarpAngleParams[i][8]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(gridWarpFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Grid_Warp_Displacement_Distort_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create grid warp filter
        auto filter0 = CreateFilter(RSNGEffectType::GRID_WARP);
        auto gridWarpFilter = std::static_pointer_cast<RSNGGridWarpFilter>(filter0);
        gridWarpFilter->Setter<GridWarpGridPoint0Tag>(gridWarpPointParams[i][0]);
        gridWarpFilter->Setter<GridWarpGridPoint1Tag>(gridWarpPointParams[i][1]);
        gridWarpFilter->Setter<GridWarpGridPoint2Tag>(gridWarpPointParams[i][2]);
        gridWarpFilter->Setter<GridWarpGridPoint3Tag>(gridWarpPointParams[i][3]);
        gridWarpFilter->Setter<GridWarpGridPoint4Tag>(gridWarpPointParams[i][4]);
        gridWarpFilter->Setter<GridWarpGridPoint5Tag>(gridWarpPointParams[i][5]);
        gridWarpFilter->Setter<GridWarpGridPoint6Tag>(gridWarpPointParams[i][6]);
        gridWarpFilter->Setter<GridWarpGridPoint7Tag>(gridWarpPointParams[i][7]);
        gridWarpFilter->Setter<GridWarpGridPoint8Tag>(gridWarpPointParams[i][8]);
        gridWarpFilter->Setter<GridWarpRotationAngle0Tag>(gridWarpAngleParams[i][0]);
        gridWarpFilter->Setter<GridWarpRotationAngle1Tag>(gridWarpAngleParams[i][1]);
        gridWarpFilter->Setter<GridWarpRotationAngle2Tag>(gridWarpAngleParams[i][2]);
        gridWarpFilter->Setter<GridWarpRotationAngle3Tag>(gridWarpAngleParams[i][3]);
        gridWarpFilter->Setter<GridWarpRotationAngle4Tag>(gridWarpAngleParams[i][4]);
        gridWarpFilter->Setter<GridWarpRotationAngle5Tag>(gridWarpAngleParams[i][5]);
        gridWarpFilter->Setter<GridWarpRotationAngle6Tag>(gridWarpAngleParams[i][6]);
        gridWarpFilter->Setter<GridWarpRotationAngle7Tag>(gridWarpAngleParams[i][7]);
        gridWarpFilter->Setter<GridWarpRotationAngle8Tag>(gridWarpAngleParams[i][8]);
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
            Vector2f{ doubleRippleMaskParams[i][0], doubleRippleMaskParams[i][1]});
        doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
            Vector2f{ doubleRippleMaskParams[i][2], doubleRippleMaskParams[i][3]});
        doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[i][4]);
        doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[i][5]);
        doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[i][6]);
        // Create displacement distort filter
        auto filter1 = CreateFilter(RSNGEffectType::DISPLACEMENT_DISTORT);
        auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter1);
        dispDistortFilter->Setter<DispDistortFactorTag>(
            Vector2f{ displacementDistortParams[i][0], displacementDistortParams[i][1] });
        dispDistortFilter->Setter<DispDistortMaskTag>(mask);
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg",
            {(i % columnCount) * sizeX, (i / columnCount) * sizeY, sizeX, sizeY});
        gridWarpFilter->Append(dispDistortFilter);
        backgroundTestNode->SetBackgroundNGFilter(gridWarpFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

}  // namespace OHOS::Rosen

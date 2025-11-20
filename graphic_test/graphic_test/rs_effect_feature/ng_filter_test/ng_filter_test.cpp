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
};

// Default values
const Vector2f defaultBorderSize = Vector2f{200.0f, 100.0f};
const Vector2f defaultBlurParams = Vector2f{48.0f, 4.0f};
const float DEFAULT_CORNER_RADIUS = 100.0f;
const float DEFAULT_BORDER_WIDTH = 2.9f;
const float DEFAULT_OFFSET = 1.88f;
const float DEFAULT_DOWNSAMPLING_FACTOR = 1.0f;
const float DEFAULT_BG_FACTOR = 0.9f;
const Vector3f defaultInnerShadowParams = Vector3f{-0.02f, 2.0f, 4.62f};
const Vector3f defaultSdParams = Vector3f{0.9f, 0.0f, 1.0f};
const float DEFAULT_REFRACT_OUTPX = 20.0f;
const Vector3f defaultEnvParams = Vector3f{0.8f, 70.0f, 2.0f};
const Vector3f defaultEdgeLightAngleParams = Vector3f{30.0f, 30.0f, 3.0f};
const Vector2f defaultEdgeLightBlurParams = Vector2f{2.0f, -1.0f};
const Vector2f defaultEdgeLightDirParams = Vector2f{-1.0f, 1.0f};
const Vector3f defaultHlParams = Vector3f{0.6027f, 160.0f, 2.0f};

// Param arrays
std::vector<Vector2f> bordersizeParams = {
    Vector2f{200.0f, 0.0f},
    Vector2f{0.0f, 100.0f},
    Vector2f{200.0f, 100.0f},
    Vector2f{1200.0f, 2000.0f},
    Vector2f{5000.0f, 100.0f},
    Vector2f{200.0f, 5000.0f},
    Vector2f{200.0f, -200.0f},
    Vector2f{-200.0f, -100.0f},
};

std::vector<Vector2f> blurparamsParams = {
    Vector2f{48.0f, 4.0f},
    Vector2f{-48.0f, 4.0f},
    Vector2f{std::numeric_limits<float>::infinity(), 4.0f},
    Vector2f{48.0f, 100.0f},
    Vector2f{48.0f, 0.0f},
    Vector2f{48.0f, -1.0f},
    Vector2f{48.0f, -4.0f},
    Vector2f{4.0f, 4.0f},
    Vector2f{48.0f, 48.0f},
};

std::vector<float> cornerradiusParams = {
    0.0f,
    100.0f,
    250.0f,
    -100.0f,
    std::numeric_limits<float>::infinity(),
    10000000000.0f,
    1e-10f,
};

std::vector<float> borderwidthParams = {
    0.0f,
    2.9f,
    -2.9f,
    100.0f,
    200.0f,
    -1.0f,
    std::numeric_limits<float>::infinity(),
    100000.0f,
};

std::vector<float> offsetParams = {
    0.0f,
    1.88f,
    200.0f,
    -1.0f,
    std::numeric_limits<float>::infinity(),
};

std::vector<float> downsamplefactorParams = {
    1.0f,
    0.0f,
    -1.0f,
    2.0f,
    std::numeric_limits<float>::infinity(),
};

std::vector<float> bgfactorParams = {
    0.0f,
    0.9f,
    1.0f,
    -1.0f,
    std::numeric_limits<float>::infinity(),
};

std::vector<Vector3f> innershadowparamsParams = {
    Vector3f{1.0f, 2.0f, 4.62f},
    Vector3f{-101.0f, 2.0f, 4.62f},
    Vector3f{0.02f, 2.0f, 4.62f},
    Vector3f{-0.02f, 0.0f, 4.62f},
    Vector3f{-0.02f, 2.0f, 0.0f},
    Vector3f{-0.02f, 2.0f, -4.62f},
    Vector3f{-0.02f, -2.0f, 4.62f},
    Vector3f{-0.02f, 2.0f, std::numeric_limits<float>::infinity()},
    Vector3f{std::numeric_limits<float>::infinity(), 2.0f, 4.62f},
};

std::vector<Vector3f> sdparamsParams = {
    Vector3f{0.9f, 0.0f, 1.0f},
    Vector3f{0.0f, 0.0f, 1.0f},
    Vector3f{-0.9f, 0.0f, 1.0f},
    Vector3f{0.9f, 1.0f, 1.0f},
    Vector3f{0.9f, 0.0f, 0.0f},
    Vector3f{0.9f, 0.0f, -1.0f},
    Vector3f{std::numeric_limits<float>::infinity(), 0.0f, 1.0f},
};

std::vector<float> refractoutpxParams = {
    20.0f,
    0.0f,
    -20.0f,
    std::numeric_limits<float>::infinity(),
};

std::vector<Vector3f> envparamsParams = {
    Vector3f{0.8f, 70.0f, 2.0f},
    Vector3f{0.0f, 70.0f, 2.0f},
    Vector3f{1.0f, 70.0f, 2.0f},
    Vector3f{0.8f, 0.0f, 2.0f},
    Vector3f{0.8f, 180.0f, 2.0f},
    Vector3f{0.8f, 70.0f, 0.0f},
    Vector3f{std::numeric_limits<float>::infinity(), 70.0f, 2.0f},
};

std::vector<Vector3f> edgelightangleparamsParams = {
    Vector3f{30.0f, 30.0f, 3.0f},
    Vector3f{0.0f, 30.0f, 3.0f},
    Vector3f{180.0f, 30.0f, 3.0f},
    Vector3f{-30.0f, 30.0f, 3.0f},
    Vector3f{30.0f, 0.0f, 3.0f},
    Vector3f{30.0f, 180.0f, 3.0f},
    Vector3f{30.0f, -30.0f, 3.0f},
    Vector3f{30.0f, 30.0f, 0.0f},
    Vector3f{30.0f, 30.0f, 6.0f},
    Vector3f{1e-06f, 30.0f, 3.0f},
    Vector3f{360.0f, 30.0f, 3.0f},
};

std::vector<Vector2f> edgelightblurparamsParams = {
    Vector2f{2.0f, -1.0f},
    Vector2f{0.0f, -1.0f},
    Vector2f{-2.0f, -1.0f},
    Vector2f{2.0f, 0.0f},
    Vector2f{2.0f, 1.0f},
    Vector2f{std::numeric_limits<float>::infinity(), -1.0f},
};

std::vector<Vector2f> edgelightdirparamsParams = {
    Vector2f{-1.0f, 1.0f},
    Vector2f{0.0f, 0.0f},
    Vector2f{1.0f, -1.0f},
    Vector2f{-1.0f, 0.0f},
    Vector2f{0.0f, 1.0f},
};

std::vector<Vector3f> hlparamsParams = {
    Vector3f{0.6027f, 160.0f, 2.0f},
    Vector3f{0.0f, 160.0f, 2.0f},
    Vector3f{-0.6027f, 160.0f, 2.0f},
    Vector3f{0.6027f, 0.0f, 2.0f},
    Vector3f{0.6027f, 180.0f, 2.0f},
    Vector3f{0.6027f, 160.0f, 0.0f},
    Vector3f{0.6027f, 160.0f, -2.0f},
    Vector3f{std::numeric_limits<float>::infinity(), 160.0f, 2.0f},
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

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BorderSizeTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(bordersizeParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(bordersizeParams[i]);

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

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(blurparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_CornerRadiusTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(cornerradiusParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(cornerradiusParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BorderWidthTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(borderwidthParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(borderwidthParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_OffsetTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(offsetParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(offsetParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_DownSampleFactorTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(downsamplefactorParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(downsamplefactorParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BgFactorTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(bgfactorParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(bgfactorParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_InnerShadowParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(innershadowparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(innershadowparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_SdParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(sdparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_RefractOutPxTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(refractoutpxParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(refractoutpxParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EnvParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(envparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(envparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EdgeLightAngleParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(edgelightangleparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(edgelightangleparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EdgeLightBlurParamsTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(edgelightblurparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(edgelightblurparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EdgeLightDirParamsTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(edgelightdirparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(defaultHlParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(edgelightdirparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_HlParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(hlparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);

        frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
        frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
        frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(DEFAULT_BORDER_WIDTH);
        frostedGlassFilter->Setter<FrostedGlassOffsetTag>(DEFAULT_OFFSET);
        frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(DEFAULT_DOWNSAMPLING_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(DEFAULT_BG_FACTOR);
        frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(defaultInnerShadowParams);
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
        frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(DEFAULT_REFRACT_OUTPX);
        frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(defaultEnvParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(defaultEdgeLightAngleParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(defaultEdgeLightBlurParams);
        frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(defaultEdgeLightDirParams);
        frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(hlparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(frostedGlassFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

}  // namespace OHOS::Rosen

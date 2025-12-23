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
#include "ng_sdf_test_utils.h"
#include "ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
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

static void InitFrostedGlassFilter(std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
{
    if (frostedGlassFilter == nullptr) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    }

    frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(DEFAULT_BLUR_PARAMS);
    frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(DEFAULT_WEIGHTS_EMBOSS);
    frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(DEFAULT_WEIGHTS_EDL);

    frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(DEFAULT_BG_RATES);
    frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(DEFAULT_BG_KBS);
    frostedGlassFilter->Setter<FrostedGlassBgPosTag>(DEFAULT_BG_POS);
    frostedGlassFilter->Setter<FrostedGlassBgNegTag>(DEFAULT_BG_NEG);
    frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(DEFAULT_REFRACT_PARAMS);

    frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(DEFAULT_SD_PARAMS);
    frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(DEFAULT_SD_RATES);
    frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(DEFAULT_SD_KBS);
    frostedGlassFilter->Setter<FrostedGlassSdPosTag>(DEFAULT_SD_POS);
    frostedGlassFilter->Setter<FrostedGlassSdNegTag>(DEFAULT_SD_NEG);

    frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(DEFAULT_ENV_LIGHT_PARAMS);
    frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(DEFAULT_ENV_LIGHT_RATES);
    frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(DEFAULT_ENV_LIGHT_KBS);
    frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(DEFAULT_ENV_LIGHT_POS);
    frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(DEFAULT_ENV_LIGHT_NEG);

    frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(DEFAULT_ED_LIGHT_PARAMS);
    frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(DEFAULT_ED_LIGHT_ANGLES);
    frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(DEFAULT_ED_LIGHT_DIR);
    frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(DEFAULT_ED_LIGHT_RATES);
    frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(DEFAULT_ED_LIGHT_KBS);
    frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(DEFAULT_ED_LIGHT_POS);
    frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(DEFAULT_ED_LIGHT_NEG);

    frostedGlassFilter->Setter<FrostedGlassBaseVibrancyEnabledTag>(DEFAULT_BASE_VIBRANCY_ENABLED);
    frostedGlassFilter->Setter<FrostedGlassSamplingScaleTag>(DEFAULT_SAMPLING_SCALE);
}

static std::shared_ptr<OHOS::Rosen::RSCanvasNode> CreateSdfChildNode(
    const int x, const int y, const int sizeX, const int sizeY,
    std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
{
    auto childTestNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
    childTestNode->SetBounds(bounds);
    childTestNode->SetFrame(bounds);
    childTestNode->SetMaterialNGFilter(frostedGlassFilter);

    const RRect defaultRectParam = {RectT<float>{sizeX/4, sizeY/4, sizeX/2, sizeY/2}, sizeX/16, sizeX/16};
    std::shared_ptr<RSNGShapeBase> sdfShape;
    InitSmoothUnionShapes(sdfShape, defaultRectParam, defaultRectParam, 0.0);
    childTestNode->SetSDFShape(sdfShape);
    return childTestNode;
}

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
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
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
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_DefaultTest)
{
    int columnCount = 1;
    int rowCount = 1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
 
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BlurParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(blurparamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(blurparamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_WeightsEmbossTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(weightsEmbossParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(weightsEmbossParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_WeightsEdlTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(weightsEdlParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(weightsEdlParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BgRatesTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(bgRatesParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(bgRatesParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BgKBSTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(bgKBSParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(bgKBSParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BgPosTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(bgPosParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassBgPosTag>(bgPosParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BgNegTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(bgNegParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassBgNegTag>(bgNegParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_RefractParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(refractParamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(refractParamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_SdParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdParamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(sdParamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_SdRatesTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdRatesParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(sdRatesParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_SdKBSTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdKBSParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(sdKBSParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_SdPosTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdPosParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassSdPosTag>(sdPosParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_SdNegTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdNegParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassSdNegTag>(sdNegParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EnvLightParamsTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(envLightParamsParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(envLightParamsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EnvLightRatesTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(envLightRatesParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto filter = CreateFilter(RSNGEffectType::FROSTED_GLASS);
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(envLightRatesParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EnvLightKBSTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(envLightKBSParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(envLightKBSParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EnvLightPosTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(envLightPosParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(envLightPosParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EnvLightNegTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(envLightNegParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(envLightNegParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EdLightRatesTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(edLightRatesParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(edLightRatesParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EdLightKBSTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(edLightKBSParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(edLightKBSParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EdLightPosTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(edLightPosParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(edLightPosParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EdLightNegTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(edLightNegParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(edLightNegParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_BaseVibrancyEnabledTest)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(baseVibrancyEnabledParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        bool baseVibrancyEnabled = baseVibrancyEnabledParams[i];
        frostedGlassFilter->Setter<FrostedGlassBaseVibrancyEnabledTag>(baseVibrancyEnabled);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_SamplingScaleTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(samplingScaleParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassSamplingScaleTag>(samplingScaleParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto childTestNode = CreateSdfChildNode(x, y, sizeX, sizeY, frostedGlassFilter);
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_Frosted_Glass_Material_Filter_Test)
{
    int columnCount = 1;
    int rowCount = static_cast<int>(blurparamsParamsForMaterialFilter.size() + 1);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        if (i == blurparamsParamsForMaterialFilter.size()) {
            frostedGlassFilter = nullptr;
        } else {
            InitFrostedGlassFilter(frostedGlassFilter);
            frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(blurparamsParamsForMaterialFilter[i]);
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

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_EdgeLight_Test_1)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[0][0], doubleRippleMaskParams[0][1] });
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[0][2], doubleRippleMaskParams[0][3] });
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[0][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[0][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[0][6]);
    
    // Create edge light filter
    auto filter = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(filter);
    edgeLightFilter->Setter<EdgeLightAlphaTag>(1.0f);
    edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    edgeLightFilter->Setter<EdgeLightMaskTag>(mask);
    edgeLightFilter->Setter<EdgeLightBloomTag>(false);
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(false);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(edgeLightFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_EdgeLight_Test_2)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[1][0], doubleRippleMaskParams[1][1] });
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[1][2], doubleRippleMaskParams[1][3] });
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[1][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[1][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[1][6]);
    
    // Create edge light filter
    auto filter = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(filter);
    edgeLightFilter->Setter<EdgeLightAlphaTag>(0.5f);
    edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    edgeLightFilter->Setter<EdgeLightMaskTag>(mask);
    edgeLightFilter->Setter<EdgeLightBloomTag>(false);
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(false);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(edgeLightFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_EdgeLight_Test_3)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[2][0], doubleRippleMaskParams[2][1] });
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[2][2], doubleRippleMaskParams[2][3] });
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[2][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[2][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[2][6]);
    
    // Create edge light filter
    auto filter = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(filter);
    edgeLightFilter->Setter<EdgeLightAlphaTag>(1.0f);
    edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f)); // 红色
    edgeLightFilter->Setter<EdgeLightMaskTag>(mask);
    edgeLightFilter->Setter<EdgeLightBloomTag>(false);
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(false);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(edgeLightFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_EdgeLight_Test_4)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[3][0], doubleRippleMaskParams[3][1] });
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[3][2], doubleRippleMaskParams[3][3] });
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[3][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[3][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[3][6]);
    
    // Create edge light filter
    auto filter = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(filter);
    edgeLightFilter->Setter<EdgeLightAlphaTag>(1.0f);
    edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f)); // 绿色
    edgeLightFilter->Setter<EdgeLightMaskTag>(mask);
    edgeLightFilter->Setter<EdgeLightBloomTag>(true);
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(false);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(edgeLightFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_EdgeLight_Test_5)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[4][0], doubleRippleMaskParams[4][1] });
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[4][2], doubleRippleMaskParams[4][3] });
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[4][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[4][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[4][6]);
    
    // Create edge light filter
    auto filter = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(filter);
    edgeLightFilter->Setter<EdgeLightAlphaTag>(1.0f);
    edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f(0.0f, 0.0f, 1.0f, 1.0f)); // 蓝色
    edgeLightFilter->Setter<EdgeLightMaskTag>(mask);
    edgeLightFilter->Setter<EdgeLightBloomTag>(false);
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(true);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(edgeLightFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Dispersion_Filter_Basic_Offset)
{
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(Vector2f{0.3f, 0.3f});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(Vector2f{0.7f, 0.7f});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(0.4f);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(0.1f);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(0.05f);

    auto filter = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(filter);
    dispersionFilter->Setter<DispersionMaskTag>(mask);
    dispersionFilter->Setter<DispersionOpacityTag>(1.0f);
    dispersionFilter->Setter<DispersionRedOffsetTag>(Vector2f{5.0f, 0.0f});
    dispersionFilter->Setter<DispersionGreenOffsetTag>(Vector2f{0.0f, 5.0f});
    dispersionFilter->Setter<DispersionBlueOffsetTag>(Vector2f{-5.0f, 0.0f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(dispersionFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Dispersion_Filter_High_Transparency)
{
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(Vector2f{0.5f, 0.5f});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(Vector2f{0.6f, 0.6f});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(0.3f);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(0.08f);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(0.03f);

    auto filter = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(filter);
    dispersionFilter->Setter<DispersionMaskTag>(mask);
    dispersionFilter->Setter<DispersionOpacityTag>(0.3f);
    dispersionFilter->Setter<DispersionRedOffsetTag>(Vector2f{10.0f, 0.0f});
    dispersionFilter->Setter<DispersionGreenOffsetTag>(Vector2f{0.0f, 10.0f});
    dispersionFilter->Setter<DispersionBlueOffsetTag>(Vector2f{-10.0f, 0.0f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(dispersionFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Dispersion_Filter_Subtle_Offset)
{
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(Vector2f{0.2f, 0.8f});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(Vector2f{0.8f, 0.2f});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(0.2f);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(0.05f);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(0.01f);

    auto filter = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(filter);
    dispersionFilter->Setter<DispersionMaskTag>(mask);
    dispersionFilter->Setter<DispersionOpacityTag>(0.8f);
    dispersionFilter->Setter<DispersionRedOffsetTag>(Vector2f{2.0f, 0.0f});
    dispersionFilter->Setter<DispersionGreenOffsetTag>(Vector2f{0.0f, 2.0f});
    dispersionFilter->Setter<DispersionBlueOffsetTag>(Vector2f{-2.0f, 0.0f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(dispersionFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Dispersion_Filter_Strong_Diagonal_Offset)
{
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(Vector2f{0.1f, 0.9f});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(Vector2f{0.9f, 0.1f});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(0.5f);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(0.15f);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(0.07f);

    auto filter = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(filter);
    dispersionFilter->Setter<DispersionMaskTag>(mask);
    dispersionFilter->Setter<DispersionOpacityTag>(1.0f);
    dispersionFilter->Setter<DispersionRedOffsetTag>(Vector2f{15.0f, 15.0f});
    dispersionFilter->Setter<DispersionGreenOffsetTag>(Vector2f{-15.0f, -15.0f});
    dispersionFilter->Setter<DispersionBlueOffsetTag>(Vector2f{0.0f, 0.0f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(dispersionFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Dispersion_Filter_Extreme_Offset_Transparent)
{
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(Vector2f{0.0f, 1.0f});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(Vector2f{1.0f, 0.0f});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(0.6f);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(0.2f);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(0.1f);

    auto filter = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(filter);
    dispersionFilter->Setter<DispersionMaskTag>(mask);
    dispersionFilter->Setter<DispersionOpacityTag>(0.1f);
    dispersionFilter->Setter<DispersionRedOffsetTag>(Vector2f{30.0f, 0.0f});
    dispersionFilter->Setter<DispersionGreenOffsetTag>(Vector2f{0.0f, 30.0f});
    dispersionFilter->Setter<DispersionBlueOffsetTag>(Vector2f{-30.0f, 0.0f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(dispersionFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_DirectionLight_Test_Negative_Boundary)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[0][0], doubleRippleMaskParams[0][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[0][2], doubleRippleMaskParams[0][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[0][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[0][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[0][6]);

    // Create DirectionLight filter
    auto filter = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(filter);
    directionLightFilter->Setter<DirectionLightMaskTag>(mask);
    directionLightFilter->Setter<DirectionLightFactorTag>(-1.0f);
    directionLightFilter->Setter<DirectionLightDirectionTag>(Vector3f{-1.0f, -1.0f, -1.0f});
    directionLightFilter->Setter<DirectionLightColorTag>(Vector4f{-1.0f, -1.0f, -1.0f, -1.0f});
    directionLightFilter->Setter<DirectionLightIntensityTag>(-1.0f);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(filter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_DirectionLight_Test_Warm_Color_Normal)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[1][0], doubleRippleMaskParams[1][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[1][2], doubleRippleMaskParams[1][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[1][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[1][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[1][6]);

    // Create DirectionLight filter
    auto filter = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(filter);
    directionLightFilter->Setter<DirectionLightMaskTag>(mask);
    directionLightFilter->Setter<DirectionLightFactorTag>(0.5f);
    directionLightFilter->Setter<DirectionLightDirectionTag>(Vector3f{0.5f, -0.6f, -0.9f});
    directionLightFilter->Setter<DirectionLightColorTag>(Vector4f{0.9f, 0.9f, 0.8f, 1.0f});
    directionLightFilter->Setter<DirectionLightIntensityTag>(3.0f);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(filter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_DirectionLight_Test_Low_Factor_Intensity)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[2][0], doubleRippleMaskParams[2][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[2][2], doubleRippleMaskParams[2][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[2][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[2][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[2][6]);

    // Create DirectionLight filter
    auto filter = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(filter);
    directionLightFilter->Setter<DirectionLightMaskTag>(mask);
    directionLightFilter->Setter<DirectionLightFactorTag>(0.3f);
    directionLightFilter->Setter<DirectionLightDirectionTag>(Vector3f{0.3f, 0.3f, 0.3f});
    directionLightFilter->Setter<DirectionLightColorTag>(Vector4f{0.3f, 0.3f, 0.3f, 1.0f});
    directionLightFilter->Setter<DirectionLightIntensityTag>(1.5f);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(filter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_DirectionLight_Test_High_White_Light)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[3][0], doubleRippleMaskParams[3][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[3][2], doubleRippleMaskParams[3][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[3][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[3][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[3][6]);

    // Create DirectionLight filter
    auto filter = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(filter);
    directionLightFilter->Setter<DirectionLightMaskTag>(mask);
    directionLightFilter->Setter<DirectionLightFactorTag>(0.9f);
    directionLightFilter->Setter<DirectionLightDirectionTag>(Vector3f{0.9f, 0.9f, -0.9f});
    directionLightFilter->Setter<DirectionLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    directionLightFilter->Setter<DirectionLightIntensityTag>(3.0f);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(filter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_DirectionLight_Test_Mid_Neutral_Color)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[4][0], doubleRippleMaskParams[4][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[4][2], doubleRippleMaskParams[4][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[4][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[4][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[4][6]);

    // Create DirectionLight filter
    auto filter = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(filter);
    directionLightFilter->Setter<DirectionLightMaskTag>(mask);
    directionLightFilter->Setter<DirectionLightFactorTag>(0.6f);
    directionLightFilter->Setter<DirectionLightDirectionTag>(Vector3f{0.6f, 0.6f, 0.6f});
    directionLightFilter->Setter<DirectionLightColorTag>(Vector4f{0.6f, 0.6f, 0.6f, 1.0f});
    directionLightFilter->Setter<DirectionLightIntensityTag>(0.6f);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(filter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_DirectionLight_Test_Extreme_Large_Params)
{
    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[5][0], doubleRippleMaskParams[5][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[5][2], doubleRippleMaskParams[5][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[5][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[5][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[5][6]);

    // Create DirectionLight filter
    auto filter = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(filter);
    directionLightFilter->Setter<DirectionLightMaskTag>(mask);
    directionLightFilter->Setter<DirectionLightFactorTag>(999.0f);
    directionLightFilter->Setter<DirectionLightDirectionTag>(Vector3f{999.0f, 999.0f, 999.0f});
    directionLightFilter->Setter<DirectionLightColorTag>(Vector4f{999.0f, 999.0f, 999.0f, 999.0f});
    directionLightFilter->Setter<DirectionLightIntensityTag>(999.0f);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(filter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_BasicTest)
{
    auto filter = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(filter);
    
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f{ 0.5f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f{ 1.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f{ 0.0f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f{ 0.5f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f{ 1.0f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f{ 0.0f, 1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f{ 0.5f, 1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f{ 1.0f, 1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f{ 0.25f, 0.25f });
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f{ 0.75f, 0.25f });
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f{ 0.25f, 0.75f });
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_BoundaryTest)
{
    auto filter = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(filter);
    
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f{ -1.0f, -1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f{ 0.5f, -1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f{ 2.0f, -1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f{ -1.0f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f{ 0.5f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f{ 2.0f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f{ -1.0f, 2.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f{ 0.5f, 2.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f{ 2.0f, 2.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f{ 1.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f{ 0.0f, 1.0f });
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_CenterDeformTest)
{
    auto filter = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(filter);
    
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f{ 0.5f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f{ 1.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f{ 0.0f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f{ 0.4f, 0.4f });
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f{ 1.0f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f{ 0.0f, 1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f{ 0.5f, 1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f{ 1.0f, 1.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f{ 0.3f, 0.3f });
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f{ 0.7f, 0.3f });
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f{ 0.3f, 0.7f });
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_TwistTest)
{
    auto filter = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(filter);
    
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f{ 0.1f, 0.1f });
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f{ 0.6f, -0.1f });
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f{ 0.9f, 0.1f });
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f{ -0.1f, 0.6f });
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f{ 0.5f, 0.5f });
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f{ 1.1f, 0.6f });
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f{ 0.1f, 0.9f });
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f{ 0.4f, 1.1f });
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f{ 0.9f, 0.9f });
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f{ 0.2f, 0.2f });
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f{ 0.8f, 0.2f });
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f{ 0.2f, 0.8f });
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_WaveTest)
{
    auto filter = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(filter);
    
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f{ 0.0f, 0.1f });
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f{ 0.25f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f{ 0.5f, 0.1f });
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f{ 0.75f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f{ 1.0f, 0.1f });
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f{ 0.0f, 0.33f });
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f{ 0.25f, 0.4f });
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f{ 0.5f, 0.33f });
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f{ 0.75f, 0.4f });
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f{ 1.0f, 0.33f });
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f{ 0.0f, 0.67f });
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f{ 0.25f, 0.6f });
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_ZeroTest)
{
    auto filter = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(filter);
    
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f{ 0.0f, 0.0f });
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f{ 0.0f, 0.0f });
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_BasicTest)
{
    auto filter = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(filter);
    
    soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f{ 1.0f, 0.0f, 0.0f, 1.0f });
    soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f{ 0.0f, 1.0f, 0.0f, 1.0f });
    soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f{ 0.0f, 0.0f, 1.0f, 1.0f });
    soundWaveFilter->Setter<SoundWaveColorProgressTag>(0.5f);
    soundWaveFilter->Setter<SoundWaveIntensityTag>(0.8f);
    soundWaveFilter->Setter<SoundWaveAlphaATag>(0.7f);
    soundWaveFilter->Setter<SoundWaveAlphaBTag>(0.9f);
    soundWaveFilter->Setter<SoundWaveProgressATag>(0.3f);
    soundWaveFilter->Setter<SoundWaveProgressBTag>(0.7f);
    soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(0.85f);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(soundWaveFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_BoundaryTest)
{
    auto filter = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(filter);
    
    soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f });
    soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f{ 1.0f, 1.0f, 1.0f, 1.0f });
    soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f{ 0.5f, 0.5f, 0.5f, 0.5f });
    soundWaveFilter->Setter<SoundWaveColorProgressTag>(0.0f);
    soundWaveFilter->Setter<SoundWaveIntensityTag>(1.0f);
    soundWaveFilter->Setter<SoundWaveAlphaATag>(0.0f);
    soundWaveFilter->Setter<SoundWaveAlphaBTag>(1.0f);
    soundWaveFilter->Setter<SoundWaveProgressATag>(0.0f);
    soundWaveFilter->Setter<SoundWaveProgressBTag>(1.0f);
    soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(1.0f);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(soundWaveFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_HighSaturationTest)
{
    auto filter = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(filter);
    
    soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f{ 1.0f, 0.0f, 1.0f, 1.0f });
    soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f{ 0.0f, 1.0f, 1.0f, 1.0f });
    soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f{ 1.0f, 1.0f, 0.0f, 1.0f });
    soundWaveFilter->Setter<SoundWaveColorProgressTag>(1.0f);
    soundWaveFilter->Setter<SoundWaveIntensityTag>(2.0f);
    soundWaveFilter->Setter<SoundWaveAlphaATag>(0.5f);
    soundWaveFilter->Setter<SoundWaveAlphaBTag>(0.5f);
    soundWaveFilter->Setter<SoundWaveProgressATag>(0.5f);
    soundWaveFilter->Setter<SoundWaveProgressBTag>(0.5f);
    soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(0.6f);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(soundWaveFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_LowAlphaTest)
{
    auto filter = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(filter);
    
    soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f{ 0.2f, 0.3f, 0.4f, 0.1f });
    soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f{ 0.4f, 0.5f, 0.6f, 0.2f });
    soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f{ 0.6f, 0.7f, 0.8f, 0.3f });
    soundWaveFilter->Setter<SoundWaveColorProgressTag>(0.25f);
    soundWaveFilter->Setter<SoundWaveIntensityTag>(0.3f);
    soundWaveFilter->Setter<SoundWaveAlphaATag>(0.1f);
    soundWaveFilter->Setter<SoundWaveAlphaBTag>(0.2f);
    soundWaveFilter->Setter<SoundWaveProgressATag>(0.1f);
    soundWaveFilter->Setter<SoundWaveProgressBTag>(0.9f);
    soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(0.15f);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(soundWaveFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_MediumAlphaTest)
{
    auto filter = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(filter);
    
    soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f{ 0.8f, 0.2f, 0.6f, 0.7f });
    soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f{ 0.2f, 0.8f, 0.4f, 0.6f });
    soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f{ 0.4f, 0.6f, 0.8f, 0.5f });
    soundWaveFilter->Setter<SoundWaveColorProgressTag>(0.6f);
    soundWaveFilter->Setter<SoundWaveIntensityTag>(0.6f);
    soundWaveFilter->Setter<SoundWaveAlphaATag>(0.4f);
    soundWaveFilter->Setter<SoundWaveAlphaBTag>(0.6f);
    soundWaveFilter->Setter<SoundWaveProgressATag>(0.2f);
    soundWaveFilter->Setter<SoundWaveProgressBTag>(0.8f);
    soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(0.5f);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(soundWaveFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_ZeroTest)
{
    auto filter = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(filter);
    
    soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f });
    soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f });
    soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f });
    soundWaveFilter->Setter<SoundWaveColorProgressTag>(0.0f);
    soundWaveFilter->Setter<SoundWaveIntensityTag>(0.0f);
    soundWaveFilter->Setter<SoundWaveAlphaATag>(0.0f);
    soundWaveFilter->Setter<SoundWaveAlphaBTag>(0.0f);
    soundWaveFilter->Setter<SoundWaveProgressATag>(0.0f);
    soundWaveFilter->Setter<SoundWaveProgressBTag>(0.0f);
    soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(0.0f);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(soundWaveFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Color_Gradient_Test_Case1)
{
    const int caseIndex = 0;

    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    // Create color gradient filter
    auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
    auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
    std::vector<float> colors = {colorGradientParams[caseIndex][0], colorGradientParams[caseIndex][1],
                                 colorGradientParams[caseIndex][2], colorGradientParams[caseIndex][3]};
    std::vector<float> positions = {colorGradientParams[caseIndex][4], colorGradientParams[caseIndex][5]};
    std::vector<float> strengths = {colorGradientParams[caseIndex][6]};
    colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
    colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
    colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
    colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Color_Gradient_Test_Case2)
{
    const int caseIndex = 1;

    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    // Create color gradient filter
    auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
    auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
    std::vector<float> colors = {colorGradientParams[caseIndex][0], colorGradientParams[caseIndex][1],
                                 colorGradientParams[caseIndex][2], colorGradientParams[caseIndex][3]};
    std::vector<float> positions = {colorGradientParams[caseIndex][4], colorGradientParams[caseIndex][5]};
    std::vector<float> strengths = {colorGradientParams[caseIndex][6]};
    colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
    colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
    colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
    colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Color_Gradient_Test_Case3)
{
    const int caseIndex = 2;

    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    // Create color gradient filter
    auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
    auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
    std::vector<float> colors = {colorGradientParams[caseIndex][0], colorGradientParams[caseIndex][1],
                                 colorGradientParams[caseIndex][2], colorGradientParams[caseIndex][3]};
    std::vector<float> positions = {colorGradientParams[caseIndex][4], colorGradientParams[caseIndex][5]};
    std::vector<float> strengths = {colorGradientParams[caseIndex][6]};
    colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
    colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
    colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
    colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Color_Gradient_Test_Case4)
{
    const int caseIndex = 3;

    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    // Create color gradient filter
    auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
    auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
    std::vector<float> colors = {colorGradientParams[caseIndex][0], colorGradientParams[caseIndex][1],
                                 colorGradientParams[caseIndex][2], colorGradientParams[caseIndex][3]};
    std::vector<float> positions = {colorGradientParams[caseIndex][4], colorGradientParams[caseIndex][5]};
    std::vector<float> strengths = {colorGradientParams[caseIndex][6]};
    colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
    colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
    colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
    colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Color_Gradient_Test_Case5)
{
    const int caseIndex = 4;

    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    // Create color gradient filter
    auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
    auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
    std::vector<float> colors = {colorGradientParams[caseIndex][0], colorGradientParams[caseIndex][1],
                                 colorGradientParams[caseIndex][2], colorGradientParams[caseIndex][3]};
    std::vector<float> positions = {colorGradientParams[caseIndex][4], colorGradientParams[caseIndex][5]};
    std::vector<float> strengths = {colorGradientParams[caseIndex][6]};
    colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
    colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
    colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
    colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Color_Gradient_Test_Case6)
{
    const int caseIndex = 5;

    // Create double ripple mask
    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    // Create color gradient filter
    auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
    auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
    std::vector<float> colors = {colorGradientParams[caseIndex][0], colorGradientParams[caseIndex][1],
                                 colorGradientParams[caseIndex][2], colorGradientParams[caseIndex][3]};
    std::vector<float> positions = {colorGradientParams[caseIndex][4], colorGradientParams[caseIndex][5]};
    std::vector<float> strengths = {colorGradientParams[caseIndex][6]};
    colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
    colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
    colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
    colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, screenWidth, screenHeight});
    backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Displacement_Distort_Test_Case1)
{
    const int caseIndex = 0;

    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    auto filter = CreateFilter(RSNGEffectType::DISPLACEMENT_DISTORT);
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    dispDistortFilter->Setter<DispDistortFactorTag>(
        Vector2f{ displacementDistortParams[caseIndex][0], displacementDistortParams[caseIndex][1] });
    dispDistortFilter->Setter<DispDistortMaskTag>(mask);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(dispDistortFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Displacement_Distort_Test_Case2)
{
    const int caseIndex = 1;

    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    auto filter = CreateFilter(RSNGEffectType::DISPLACEMENT_DISTORT);
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    dispDistortFilter->Setter<DispDistortFactorTag>(
        Vector2f{ displacementDistortParams[caseIndex][0], displacementDistortParams[caseIndex][1] });
    dispDistortFilter->Setter<DispDistortMaskTag>(mask);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(dispDistortFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Displacement_Distort_Test_Case3)
{
    const int caseIndex = 2;

    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    auto filter = CreateFilter(RSNGEffectType::DISPLACEMENT_DISTORT);
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    dispDistortFilter->Setter<DispDistortFactorTag>(
        Vector2f{ displacementDistortParams[caseIndex][0], displacementDistortParams[caseIndex][1] });
    dispDistortFilter->Setter<DispDistortMaskTag>(mask);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(dispDistortFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Displacement_Distort_Test_Case4)
{
    const int caseIndex = 3;

    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    auto filter = CreateFilter(RSNGEffectType::DISPLACEMENT_DISTORT);
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    dispDistortFilter->Setter<DispDistortFactorTag>(
        Vector2f{ displacementDistortParams[caseIndex][0], displacementDistortParams[caseIndex][1] });
    dispDistortFilter->Setter<DispDistortMaskTag>(mask);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(dispDistortFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Displacement_Distort_Test_Case5)
{
    const int caseIndex = 4;

    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    auto filter = CreateFilter(RSNGEffectType::DISPLACEMENT_DISTORT);
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    dispDistortFilter->Setter<DispDistortFactorTag>(
        Vector2f{ displacementDistortParams[caseIndex][0], displacementDistortParams[caseIndex][1] });
    dispDistortFilter->Setter<DispDistortMaskTag>(mask);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(dispDistortFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Displacement_Distort_Test_Case6)
{
    const int caseIndex = 5;

    auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
    doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][0], doubleRippleMaskParams[caseIndex][1]});
    doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[caseIndex][2], doubleRippleMaskParams[caseIndex][3]});
    doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[caseIndex][4]);
    doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[caseIndex][5]);
    doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[caseIndex][6]);

    auto filter = CreateFilter(RSNGEffectType::DISPLACEMENT_DISTORT);
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    dispDistortFilter->Setter<DispDistortFactorTag>(
        Vector2f{ displacementDistortParams[caseIndex][0], displacementDistortParams[caseIndex][1] });
    dispDistortFilter->Setter<DispDistortMaskTag>(mask);
    
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, screenWidth, screenHeight });
    backgroundTestNode->SetBackgroundNGFilter(dispDistortFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}
}  // namespace OHOS::Rosen
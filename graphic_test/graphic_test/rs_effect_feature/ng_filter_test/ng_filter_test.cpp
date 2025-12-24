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

    void SetBgAndSdfChildNodes(const int i, const int columnCount, const int sizeX, const int sizeY,
        std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
    {
        int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        // set background node
        auto backgroundTestNode =
            SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        // set child node for applying frostedGlassFilter
        auto childTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        childTestNode->SetBounds(bounds);
        childTestNode->SetFrame(bounds);
        childTestNode->SetMaterialNGFilter(frostedGlassFilter);

        //  apply sdf on child node
        const RRect defaultRectParam = {RectT<float>{sizeX/4, sizeY/4, sizeX/2, sizeY/2}, sizeX/16, sizeX/16};
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, defaultRectParam, defaultRectParam, 0.0);
        childTestNode->SetSDFShape(sdfShape);

        // add background node's child(childnode) and register childnode
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    const int filterParaTypeCount = static_cast<int>(FilterPara::ParaType::CONTENT_LIGHT);
};

static void InitFrostedGlassFilter(std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
{
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(sdParamsParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Frosted_Glass_EnvLightRatesTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(envLightRatesParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(envLightRatesParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
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

GRAPHIC_TEST(NGFilterTest, EFFECT_TEST, Set_NG_Filter_Grid_Warp_AngleParamsTest)
{
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
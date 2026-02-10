/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
namespace {
constexpr size_t NUM_2 = 2;
constexpr size_t NUM_3 = 3;
constexpr size_t NUM_4 = 4;
constexpr size_t NUM_5 = 5;
const size_t screenWidth = 1200;
const size_t screenHeight = 2000;
constexpr uint32_t COLOR_BLACK = 0xff000000;
constexpr uint32_t COLOR_BLUE = 0xff0000ff;
constexpr uint32_t COLOR_GREEN = 0xff00ff00;
constexpr uint32_t COLOR_RED = 0xffff0000;
constexpr uint32_t RECT_WIDTH = 200;
constexpr uint32_t RECT_HEIGHT = 450;
constexpr uint32_t CIRCLE_CENTER_X = 160;
constexpr uint32_t CIRCLE_CENTER_Y = 260;
constexpr uint32_t CIRCLE_CENTER_RADIUS1 = 245;
constexpr uint32_t CIRCLE_CENTER_RADIUS2 = 145;
constexpr uint32_t TRI_PNT1_X = 50;
constexpr uint32_t TRI_PNT1_Y = 50;
constexpr uint32_t TRI_PNT2_X = 250;
constexpr uint32_t TRI_PNT2_Y = 50;
constexpr uint32_t TRI_PNT3_X = 150;
constexpr uint32_t TRI_PNT3_Y = 250;
constexpr uint32_t STAR_PNT1_X = 50;
constexpr uint32_t STAR_PNT1_Y = 15;
constexpr uint32_t STAR_PNT2_X = 61;
constexpr uint32_t STAR_PNT2_Y = 39;
constexpr uint32_t STAR_PNT3_X = 88;
constexpr uint32_t STAR_PNT3_Y = 39;
constexpr uint32_t STAR_PNT4_X = 66;
constexpr uint32_t STAR_PNT4_Y = 57;
constexpr uint32_t STAR_PNT5_X = 74;
constexpr uint32_t STAR_PNT5_Y = 84;
constexpr uint32_t STAR_PNT6_X = 50;
constexpr uint32_t STAR_PNT6_Y = 69;
constexpr uint32_t STAR_PNT7_X = 26;
constexpr uint32_t STAR_PNT7_Y = 84;
constexpr uint32_t STAR_PNT8_X = 34;
constexpr uint32_t STAR_PNT8_Y = 57;
constexpr uint32_t STAR_PNT9_X = 12;
constexpr uint32_t STAR_PNT9_Y = 39;
constexpr uint32_t STAR_PNT10_X = 39;
constexpr uint32_t STAR_PNT10_Y = 39;
constexpr uint32_t SHADOW_RADIUS = 50;
constexpr uint32_t SHADOW_OFFSET_X = 50;
constexpr uint32_t SHADOW_OFFSET_Y = 50;
constexpr uint32_t colorList[NUM_4] = { COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_BLUE };
constexpr float DEFAULT_BG_ALPHA = 1.0f;
}
class NGFilterFrostedGlassTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        // add 4 Path objects
        drawingPath_.assign(4, Drawing::Path());
        // add rect as 1st object
        drawingPath_[0].AddRect(0, 0, RECT_WIDTH, RECT_HEIGHT, Drawing::PathDirection::CW_DIRECTION);
         // add circle as 2st object
        drawingPath_[1].AddCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, CIRCLE_CENTER_RADIUS1);
        const std::vector<Drawing::Point> triangle = {
            {TRI_PNT1_X, TRI_PNT1_Y}, {TRI_PNT2_X, TRI_PNT2_Y}, {TRI_PNT3_X, TRI_PNT3_Y}
        };
         // add tirangle as 3st object
        drawingPath_[NUM_2].AddPoly(triangle, triangle.size(), true);
        const std::vector<Drawing::Point> star = {
            {STAR_PNT1_X, STAR_PNT1_Y}, {STAR_PNT2_X, STAR_PNT2_Y}, {STAR_PNT3_X, STAR_PNT3_Y},
            {STAR_PNT4_X, STAR_PNT4_Y}, {STAR_PNT5_X, STAR_PNT5_Y}, {STAR_PNT6_X, STAR_PNT6_Y},
            {STAR_PNT7_X, STAR_PNT7_Y}, {STAR_PNT8_X, STAR_PNT8_Y}, {STAR_PNT9_X, STAR_PNT9_Y},
            {STAR_PNT10_X, STAR_PNT10_Y}
        };
         // add star and small circle as 4st object
        drawingPath_[NUM_3].AddPoly(star, star.size(), true);
        drawingPath_[NUM_3].AddCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, CIRCLE_CENTER_RADIUS2);
    }

    void SetSingleBgNode()
    {
        auto backgroundTestNode = SetUpNodeBgImage(FG_TEST_JPG_PATH, {0, 0, screenWidth, screenHeight});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }

    void SetBgAndSdfChildNodes(const size_t i, const size_t columnCount, const size_t sizeX,
        const size_t sizeY, std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
    {
        const size_t x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        const size_t y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

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
        const RRect defaultRectParam = {
            RectT<float>{sizeX / 4, sizeY / 4, sizeX / 2, sizeY / 2}, sizeX / 16, sizeX / 16
        };
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, defaultRectParam, defaultRectParam, 0.0);
        childTestNode->SetSDFShape(sdfShape);

        // add background node's child(childnode) and register childnode
        backgroundTestNode->AddChild(childTestNode);
        RegisterNode(childTestNode);
    }

private:
    std::vector<Drawing::Path> drawingPath_;
};

static std::shared_ptr<RSNGFrostedGlassFilter> CreateDefaultFrostedGlassFilter()
{
    auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();

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
    frostedGlassFilter->Setter<FrostedGlassMaterialColorTag>(DEFAULT_MATERIAL_COLOR);
    frostedGlassFilter->Setter<FrostedGlassBaseVibrancyEnabledTag>(DEFAULT_BASE_VIBRANCY_ENABLED);
    frostedGlassFilter->Setter<FrostedGlassSamplingScaleTag>(DEFAULT_SAMPLING_SCALE);
    frostedGlassFilter->Setter<FrostedGlassBgAlphaTag>(DEFAULT_BG_ALPHA);

    return frostedGlassFilter;
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_Default_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = 1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_BlurParams_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = blurparamsParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(blurparamsParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_WeightsEmboss_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = weightsEmbossParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(weightsEmbossParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_WeightsEdl_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = weightsEdlParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(weightsEdlParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_BgRates_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = bgRatesParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(bgRatesParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_BgKBS_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = bgKBSParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(bgKBSParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_BgPos_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = bgPosParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassBgPosTag>(bgPosParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_BgNeg_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = bgNegParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassBgNegTag>(bgNegParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_RefractParams_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = refractParamsParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(refractParamsParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_SdParams_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = sdParamsParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(sdParamsParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_SdRates_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = sdRatesParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(sdRatesParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_SdKBS_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = sdKBSParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(sdKBSParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_SdPos_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = sdPosParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassSdPosTag>(sdPosParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_SdNeg_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = sdNegParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassSdNegTag>(sdNegParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EnvLightParams_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = envLightParamsParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(envLightParamsParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EnvLightRates_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = envLightRatesParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(envLightRatesParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EnvLightKBS_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = envLightKBSParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(envLightKBSParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EnvLightPos_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = envLightPosParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(envLightPosParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EnvLightNeg_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = envLightNegParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(envLightNegParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EdLightParams_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = edLightParamsParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(edLightParamsParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EdLightAngles_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = edLightAnglesParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(edLightAnglesParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EdLightDir_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = edLightDirParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(edLightDirParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EdLightRates_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = edLightRatesParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(edLightRatesParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EdLightKBS_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = edLightKBSParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(edLightKBSParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EdLightPos_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = edLightPosParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(edLightPosParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_EdLightNeg_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = edLightNegParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(edLightNegParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_BaseVibrancyEnabled_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = baseVibrancyEnabledParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassBaseVibrancyEnabledTag>(bool(baseVibrancyEnabledParams[i]));
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_SamplingScale_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = samplingScaleParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassSamplingScaleTag>(samplingScaleParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, EFFECT_TEST, Set_MaterialColor_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = materialColorParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        frostedGlassFilter->Setter<FrostedGlassMaterialColorTag>(materialColorParams[i]);
        SetBgAndSdfChildNodes(i, columnCount, sizeX, sizeY, frostedGlassFilter);
    }
}

// frosted glass + outer shadow
GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Color_Test_1)
{
    const size_t nodePos = 500;
    const size_t nodeOffset = 100;
    const size_t nodeSize = 400;

    SetSingleBgNode();

    for (size_t i = 0; i < NUM_4; i++) {
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNodeColor = RSCanvasNode::Create();
        testNodeColor->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeColor->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeColor->SetShadowColor(colorList[i]);
        testNodeColor->SetShadowRadius(SHADOW_RADIUS);

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeColor->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeColor);
        RegisterNode(testNodeColor);
    }
}

// shadow offset
GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Offset_Test_1)
{
    const size_t nodePos = 500;
    const size_t nodeOffset = 100;
    const size_t nodeSize = 400;

    SetSingleBgNode();

    for (size_t i = 0; i < NUM_4; i++) {
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNodeOffset = RSCanvasNode::Create();
        testNodeOffset->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeOffset->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeOffset->SetShadowColor(COLOR_BLACK);
        switch (i) {
            case 0: {
                testNodeOffset->SetShadowOffset(-50, -50);
                break;
            }
            case 1: {
                testNodeOffset->SetShadowOffset(50, -50);
                break;
            }
            case 2: {
                testNodeOffset->SetShadowOffset(-50, 50);
                break;
            }
            case 3: {
                testNodeOffset->SetShadowOffset(50, 50);
                break;
            }
            default:
                break;
        }
        testNodeOffset->SetShadowRadius(SHADOW_RADIUS);

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeOffset->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeOffset);
        RegisterNode(testNodeOffset);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Offset_Test_2)
{
    const size_t nodePos = 500;
    const size_t nodeOffset = 100;
    const size_t nodeSize = 400;

    SetSingleBgNode();

    for (size_t i = 0; i < NUM_5; i++) {
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNodeOffset = RSCanvasNode::Create();
        testNodeOffset->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeOffset->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeOffset->SetShadowColor(COLOR_BLACK);
        testNodeOffset->SetShadowRadius(50);

        switch (i) {
            case 0: {
                testNodeOffset->SetShadowOffset(-250, -250);
                break;
            }
            case 1: {
                testNodeOffset->SetShadowOffset(250, 0);
                break;
            }
            case 2: {
                testNodeOffset->SetShadowOffset(0, 250);
                break;
            }
            case 3: {
                testNodeOffset->SetShadowOffset(250, 250);
                break;
            }
            case 4: {
                testNodeOffset->SetTranslate(0, nodeOffset, 0);
                testNodeOffset->SetShadowOffset(nodeSize, nodeSize);
                break;
            }
            default:
                break;
        }

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeOffset->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeOffset);
        RegisterNode(testNodeOffset);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Offset_Test_3)
{
    const size_t nodePos = 500;
    const size_t nodeOffset = 100;
    const size_t nodeSize = 400;

    const std::vector<Vector4f> radiusList = {{ 200, 200, 200, 200 }, { 100, 100, 100, 100 },
        { 100, 100, 150, 150 }, { 50, 50, 50, 50 }, { 200, 1000, 250, 50 }
    };

    SetSingleBgNode();

    for (size_t i = 0; i < radiusList.size(); i++) {
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNodeOffset = RSCanvasNode::Create();
        testNodeOffset->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeOffset->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeOffset->SetCornerRadius(radiusList[i]);
        testNodeOffset->SetShadowColor(COLOR_BLACK);
        testNodeOffset->SetShadowRadius(50);

        switch (i) {
            case 0: {
                testNodeOffset->SetShadowOffset(-100, -100);
                break;
            }
            case 1: {
                testNodeOffset->SetShadowOffset(100, 0);
                break;
            }
            case 2: {
                testNodeOffset->SetShadowOffset(0, 100);
                break;
            }
            case 3: {
                testNodeOffset->SetShadowOffset(100, 100);
                break;
            }
            case 4: {
                testNodeOffset->SetTranslate(0, nodeOffset, 0);
                testNodeOffset->SetShadowOffset(nodeSize, nodeSize);
                break;
            }
            default:
                break;
        }

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeOffset->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeOffset);
        RegisterNode(testNodeOffset);
    }
}

// shadow alpha
GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Alpha_Test_1)
{
    const size_t nodePos = 500;
    const size_t nodeOffset = 100;
    const size_t nodeSize = 400;

    const std::vector<float> alphaList = {0.0, 0.2, 0.8, 1.0};

    SetSingleBgNode();

    for (size_t i = 0; i < alphaList.size(); i++) {
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNodeAlpha = RSCanvasNode::Create();
        testNodeAlpha->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeAlpha->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeAlpha->SetShadowColor(COLOR_BLACK);
        testNodeAlpha->SetShadowAlpha(alphaList[i]);
        testNodeAlpha->SetShadowRadius(SHADOW_RADIUS);
        
        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeAlpha->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeAlpha);
        RegisterNode(testNodeAlpha);
    }
}

// shadow radius
GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Radius_Test_1)
{
    const size_t nodePos = 500;
    const size_t nodeOffset = 100;
    const size_t nodeSize = 400;

    const std::vector<float> radiusList = {0.0, 50.0, 250.0};

    SetSingleBgNode();

    for (size_t i = 0; i < radiusList.size(); i++) {
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNodeRadius = RSCanvasNode::Create();
        testNodeRadius->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeRadius->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeRadius->SetShadowColor(COLOR_BLACK);
        testNodeRadius->SetShadowRadius(radiusList[i]);

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeRadius->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeRadius);
        RegisterNode(testNodeRadius);
    }

    // ClipToBounds
    const size_t yPos = 1250;
    for (size_t i = 0; i < NUM_2; i++) {
        auto testNodeParent = RSCanvasNode::Create();
        testNodeParent->SetBounds({ i * nodePos, yPos, nodeSize, nodeSize });
        testNodeParent->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeParent->SetClipToBounds(i);
        GetRootNode()->AddChild(testNodeParent);
        RegisterNode(testNodeParent);

        auto testNodeChild = RSCanvasNode::Create();
        testNodeChild->SetBounds({ 0, 0, nodeSize, nodeSize });
        testNodeChild->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeChild->SetShadowColor(COLOR_BLACK);
        testNodeChild->SetShadowRadius(SHADOW_RADIUS);

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeChild->SetMaterialNGFilter(frostedGlassFilter);

        testNodeParent->AddChild(testNodeChild);
        RegisterNode(testNodeChild);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Radius_Test_2)
{
    const size_t nodePos = 500;
    const size_t nodeOffset = 100;
    const size_t nodeSize = 400;

    const std::vector<float> radiusList = {10.0, 50.0, 100.0, 200.0};

    SetSingleBgNode();

    // First column draw normal shadow radius
    for (size_t i = 0; i < radiusList.size(); i++) {
        auto testNodeRadius = RSCanvasNode::Create();
        testNodeRadius->SetBounds({ 0, i * nodePos, nodeSize, nodeSize });
        testNodeRadius->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeRadius->SetShadowColor(COLOR_BLACK);
        testNodeRadius->SetShadowRadius(radiusList[i]);

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeRadius->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeRadius);
        RegisterNode(testNodeRadius);
    }

    // Second column draw shadow radius with elevation
    for (size_t i = 0; i < radiusList.size(); i++) {
        auto testNodeRadius = RSCanvasNode::Create();
        testNodeRadius->SetBounds({ nodePos, i * nodePos, nodeSize, nodeSize });
        testNodeRadius->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeRadius->SetShadowColor(COLOR_BLACK);
        testNodeRadius->SetShadowRadius(radiusList[i]);
        testNodeRadius->SetShadowElevation(radiusList[radiusList.size() - i]); // reverse index

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeRadius->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeRadius);
        RegisterNode(testNodeRadius);
    }
}

// shadow filled
GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Filled_Test_1)
{
    const size_t nodePos = 500;
    const size_t nodeOffset = 100;
    const size_t nodeSize = 400;

    const float alphaList[] = {1.0, 0.5};

    SetSingleBgNode();

    for (size_t i = 0; i < NUM_4; i++) {
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNodeFilled = RSCanvasNode::Create();
        testNodeFilled->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeFilled->SetTranslate(nodeOffset, nodeOffset, 0);
        auto imageModifier = std::make_shared<ImageCustomModifier>();
        imageModifier->SetWidth(nodeSize);
        imageModifier->SetHeight(nodeSize);
        imageModifier->SetPixelMapPath(APPEARANCE_TEST_JPG_PATH);
        testNodeFilled->AddModifier(imageModifier);
        testNodeFilled->SetAlpha(0.5);
        testNodeFilled->SetShadowColor(COLOR_GREEN);
        testNodeFilled->SetShadowOffset(SHADOW_OFFSET_X, SHADOW_OFFSET_Y);
        testNodeFilled->SetShadowRadius(SHADOW_RADIUS);
        testNodeFilled->SetShadowIsFilled(i / 2);
        testNodeFilled->SetShadowAlpha(alphaList[i % 2]);

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNodeFilled->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNodeFilled);
        RegisterNode(testNodeFilled);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Filter_Test_1)
{
    const size_t nodePos = 500;
    const size_t nodeSize = 20;

    SetSingleBgNode();

    for (size_t i = 0; i < NUM_4; i++) {
        auto shadowPath = RSPath::CreateRSPath(drawingPath_[i]);
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetShadowColor(colorList[i]);
        testNode->SetShadowRadius(30);
        testNode->SetShadowPath(shadowPath);

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(NGFilterFrostedGlassTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Filter_Test_2)
{
    const size_t nodePos = 500;
    const size_t nodeSize = 20;
    const bool filledList[NUM_4] = {false, true, false, true};
    SetSingleBgNode();

    for (size_t i = 0; i < NUM_4; i++) {
        auto shadowPath = RSPath::CreateRSPath(drawingPath_[i]);
        size_t x = (i % 2) * nodePos;
        size_t y = (i / 2) * nodePos;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetShadowColor(colorList[i]);
        testNode->SetShadowElevation(30);
        testNode->SetShadowOffset(50, 50);
        testNode->SetShadowIsFilled(filledList[i]);
        testNode->SetShadowPath(shadowPath);

        auto frostedGlassFilter = CreateDefaultFrostedGlassFilter();
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

}  // namespace OHOS::Rosen
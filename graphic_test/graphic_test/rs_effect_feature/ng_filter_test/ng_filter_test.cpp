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
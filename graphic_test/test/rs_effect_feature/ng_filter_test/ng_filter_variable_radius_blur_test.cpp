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
#include "ng_filter_test_utils.h"
#include "ui_effect/filter/include/filter_variable_radius_blur_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr size_t screenWidth = 1200;
constexpr size_t screenHeight = 2000;

const std::vector<float> radiusParams = {
    -10.0f, 0.0f, 5.0f, 15.0f, 30.0f, 100.0f
};
}

class NGFilterVariableRadiusBlurTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void SetBgAndChildNodes(const size_t i, const size_t columnCount, const size_t sizeX,
        const size_t sizeY, std::shared_ptr<RSNGVariableRadiusBlurFilter>& variableRadiusBlurFilter)
    {
        const size_t x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        const size_t y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        auto backgroundTestNode =
            SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(variableRadiusBlurFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
};

GRAPHIC_TEST(NGFilterVariableRadiusBlurTest, EFFECT_TEST, Set_VariableRadiusBlur_Radius_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = radiusParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (size_t i = 0; i < rowCount; i++) {
        auto variableRadiusBlurFilter = std::make_shared<RSNGVariableRadiusBlurFilter>();
        variableRadiusBlurFilter->Setter<VariableRadiusBlurRadiusTag>(radiusParams[i]);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, variableRadiusBlurFilter);
    }
}

GRAPHIC_TEST(NGFilterVariableRadiusBlurTest, EFFECT_TEST, Set_VariableRadiusBlur_Radius_Mask_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = radiusParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (size_t i = 0; i < rowCount; i++) {
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
            Vector2f{doubleRippleMaskParams[i][0], doubleRippleMaskParams[i][1]});
        doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
            Vector2f{doubleRippleMaskParams[i][2], doubleRippleMaskParams[i][3]});
        doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[i][4]);
        doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[i][5]);
        doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[i][6]);

        auto variableRadiusBlurFilter = std::make_shared<RSNGVariableRadiusBlurFilter>();
        variableRadiusBlurFilter->Setter<VariableRadiusBlurRadiusTag>(radiusParams[i]);
        variableRadiusBlurFilter->Setter<VariableRadiusBlurMaskTag>(mask);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, variableRadiusBlurFilter);
    }
}
}  // namespace OHOS::Rosen

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

#include "ng_filter_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class NGFilterMagnifierTest : public RSGraphicTest {
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

void InitMagnifierPara(std::shared_ptr<RSNGMagnifierFilter> filter)
{
    filter->Setter<MagnifierFactorTag>(1.4f);
    filter->Setter<MagnifierWidthTag>(500.0f);
    filter->Setter<MagnifierHeightTag>(500.0f);
    filter->Setter<MagnifierCornerRadiusTag>(36.0f);
    filter->Setter<MagnifierBorderWidthTag>(2.0f);
    filter->Setter<MagnifierOffsetXTag>(0.0f);
    filter->Setter<MagnifierOffsetYTag>(0.0f);
    filter->Setter<MagnifierZoomOffsetXTag>(0.0f);
    filter->Setter<MagnifierZoomOffsetYTag>(0.0f);
    filter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    filter->Setter<MagnifierShadowOffsetYTag>(8.0f);
    filter->Setter<MagnifierShadowSizeTag>(10.0f);
    filter->Setter<MagnifierShadowStrengthTag>(0.08f);
    filter->Setter<MagnifierGradientMaskColor1Tag>(RSColor(0));
    filter->Setter<MagnifierGradientMaskColor2Tag>(RSColor(0));
    filter->Setter<MagnifierOuterContourColor1Tag>(RSColor(0));
    filter->Setter<MagnifierOuterContourColor2Tag>(RSColor(0));
}

GRAPHIC_TEST(NGFilterMagnifierTest, EFFECT_TEST, Set_NG_Filter_MAGNIFIER_Test_01)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    // Create magnifier filter
    auto filter = CreateFilter(RSNGEffectType::MAGNIFIER);
    auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(filter);
    InitMagnifierPara(magnifierFilter);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(magnifierFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterMagnifierTest, EFFECT_TEST, Set_NG_Filter_MAGNIFIER_Test_02)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    // Create magnifier filter
    auto filter = CreateFilter(RSNGEffectType::MAGNIFIER);
    auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(filter);
    InitMagnifierPara(magnifierFilter);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(50.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(50.0f);
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(magnifierFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterMagnifierTest, EFFECT_TEST, Set_NG_Filter_MAGNIFIER_Test_03)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    // Create magnifier filter
    auto filter = CreateFilter(RSNGEffectType::MAGNIFIER);
    auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(filter);
    InitMagnifierPara(magnifierFilter);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(500.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(0.0f);
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(magnifierFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterMagnifierTest, EFFECT_TEST, Set_NG_Filter_MAGNIFIER_Test_04)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    // Create magnifier filter
    auto filter = CreateFilter(RSNGEffectType::MAGNIFIER);
    auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(filter);
    InitMagnifierPara(magnifierFilter);
    magnifierFilter->Setter<MagnifierFactorTag>(2.0f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 1.4f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 1.4f);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(120.0f);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(8.0f);
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(magnifierFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterMagnifierTest, EFFECT_TEST, Set_NG_Filter_MAGNIFIER_Test_05)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    // Create magnifier filter
    auto filter = CreateFilter(RSNGEffectType::MAGNIFIER);
    auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(filter);
    InitMagnifierPara(magnifierFilter);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetYTag>(48.0f);
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(magnifierFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterMagnifierTest, EFFECT_TEST, Set_NG_Filter_MAGNIFIER_Test_06)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    // Create magnifier filter
    auto filter = CreateFilter(RSNGEffectType::MAGNIFIER);
    auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(filter);
    InitMagnifierPara(magnifierFilter);
    magnifierFilter->Setter<MagnifierFactorTag>(0.7f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(magnifierFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterMagnifierTest, EFFECT_TEST, Set_NG_Filter_MAGNIFIER_Test_07)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    // Create magnifier filter
    auto filter = CreateFilter(RSNGEffectType::MAGNIFIER);
    auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(filter);
    InitMagnifierPara(magnifierFilter);
    magnifierFilter->Setter<MagnifierFactorTag>(0.7f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(1000.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(1000.0f);
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(magnifierFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}
}  // namespace OHOS::Rosen
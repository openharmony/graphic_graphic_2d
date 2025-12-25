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
class NGFilterMagnifierTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void SetBgAndSdfChildNodes(const int i, const int columnCount, const int sizeX, const int sizeY,
        std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
    {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

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

GRAPHIC_TEST(NGFilterMagnifierTest, EFFECT_TEST, Set_NG_Filter_MAGNIFIER_Test_01)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    // Create magnifier filter
    auto filter = CreateFilter(RSNGEffectType::MAGNIFIER);
    auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(filter);
    magnifierFilter->Setter<MagnifierFactorTag>(1.4f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(36.0f);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(2.0f);
    magnifierFilter->Setter<MagnifierOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetYTag>(8.0f);
    magnifierFilter->Setter<MagnifierShadowSizeTag>(10.0f);
    magnifierFilter->Setter<MagnifierShadowStrengthTag>(0.08f);
    magnifierFilter->Setter<MagnifierGradientMaskColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierGradientMaskColor2Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor2Tag>(0.0f);
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
    magnifierFilter->Setter<MagnifierFactorTag>(1.4f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(36.0f);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(2.0f);
    magnifierFilter->Setter<MagnifierOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(50.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetYTag>(8.0f);
    magnifierFilter->Setter<MagnifierShadowSizeTag>(10.0f);
    magnifierFilter->Setter<MagnifierShadowStrengthTag>(0.08f);
    magnifierFilter->Setter<MagnifierGradientMaskColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierGradientMaskColor2Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor2Tag>(0.0f);
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
    magnifierFilter->Setter<MagnifierFactorTag>(1.4f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(36.0f);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(2.0f);
    magnifierFilter->Setter<MagnifierOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(500.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetYTag>(8.0f);
    magnifierFilter->Setter<MagnifierShadowSizeTag>(10.0f);
    magnifierFilter->Setter<MagnifierShadowStrengthTag>(0.08f);
    magnifierFilter->Setter<MagnifierGradientMaskColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierGradientMaskColor2Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor2Tag>(0.0f);
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
    magnifierFilter->Setter<MagnifierFactorTag>(2.0f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 1.4f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 1.4f);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(120.0f);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(8.0f);
    magnifierFilter->Setter<MagnifierOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetYTag>(8.0f);
    magnifierFilter->Setter<MagnifierShadowSizeTag>(10.0f);
    magnifierFilter->Setter<MagnifierShadowStrengthTag>(0.08f);
    magnifierFilter->Setter<MagnifierGradientMaskColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierGradientMaskColor2Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor2Tag>(0.0f);
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
    magnifierFilter->Setter<MagnifierFactorTag>(1.4f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(36.0f);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(2.0f);
    magnifierFilter->Setter<MagnifierOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetYTag>(48.0f);
    magnifierFilter->Setter<MagnifierShadowSizeTag>(80.0f);
    magnifierFilter->Setter<MagnifierShadowStrengthTag>(0.32f);
    magnifierFilter->Setter<MagnifierGradientMaskColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierGradientMaskColor2Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor2Tag>(0.0f);
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
    magnifierFilter->Setter<MagnifierFactorTag>(0.7f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(36.0f);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(2.0f);
    magnifierFilter->Setter<MagnifierOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetYTag>(8.0f);
    magnifierFilter->Setter<MagnifierShadowSizeTag>(10.0f);
    magnifierFilter->Setter<MagnifierShadowStrengthTag>(0.08f);
    magnifierFilter->Setter<MagnifierGradientMaskColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierGradientMaskColor2Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor2Tag>(0.0f);
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
    magnifierFilter->Setter<MagnifierFactorTag>(0.7f);
    magnifierFilter->Setter<MagnifierWidthTag>(sizeX / 2.0f);
    magnifierFilter->Setter<MagnifierHeightTag>(sizeY / 2.0f);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(36.0f);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(2.0f);
    magnifierFilter->Setter<MagnifierOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierOffsetYTag>(0.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetXTag>(1000.0f);
    magnifierFilter->Setter<MagnifierZoomOffsetYTag>(1000.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetXTag>(0.0f);
    magnifierFilter->Setter<MagnifierShadowOffsetYTag>(8.0f);
    magnifierFilter->Setter<MagnifierShadowSizeTag>(10.0f);
    magnifierFilter->Setter<MagnifierShadowStrengthTag>(0.08f);
    magnifierFilter->Setter<MagnifierGradientMaskColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierGradientMaskColor2Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor1Tag>(0.0f);
    magnifierFilter->Setter<MagnifierOuterContourColor2Tag>(0.0f);
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(magnifierFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}
}  // namespace OHOS::Rosen
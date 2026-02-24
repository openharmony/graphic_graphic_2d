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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
}  // namespace

class NGFilterMaterialBlurTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

// Test MaterialBlur with all properties
GRAPHIC_TEST(NGFilterMaterialBlurTest, EFFECT_TEST, Set_NG_Filter_Material_Blur_Radius_Test)
{
    auto materialBlurPtr = CreateFilter(RSNGEffectType::MATERIAL_BLUR);
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(materialBlurPtr);
    ASSERT_NE(materialBlurFilter, nullptr);

    // Set MaterialBlur tag
    materialBlurFilter->Setter<MaterialBlurRadiusTag>(25.0f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        backgroundNode->SetBackgroundNGFilter(materialBlurFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterMaterialBlurTest, EFFECT_TEST, Set_NG_Filter_Material_Blur_Saturation_Test)
{
    auto materialBlurPtr = CreateFilter(RSNGEffectType::MATERIAL_BLUR);
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(materialBlurPtr);
    ASSERT_NE(materialBlurFilter, nullptr);

    // Set Saturation tag
    materialBlurFilter->Setter<MaterialBlurSaturationTag>(0.5f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> saturationValues = {0.0f, 0.3f, 0.7f, 1.5f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        materialBlurFilter->Setter<MaterialBlurSaturationTag>(saturationValues[i]);
        backgroundNode->SetBackgroundNGFilter(materialBlurFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterMaterialBlurTest, EFFECT_TEST, Set_NG_Filter_Material_Blur_Brightness_Test)
{
    auto materialBlurPtr = CreateFilter(RSNGEffectType::MATERIAL_BLUR);
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(materialBlurPtr);
    ASSERT_NE(materialBlurFilter, nullptr);

    // Set Brightness tag
    materialBlurFilter->Setter<MaterialBlurBrightnessTag>(0.3f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> brightnessValues = {-0.5f, 0.0f, 0.5f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        materialBlurFilter->Setter<MaterialBlurBrightnessTag>(brightnessValues[i]);
        backgroundNode->SetBackgroundNGFilter(materialBlurFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterMaterialBlurTest, EFFECT_TEST, Set_NG_Filter_Material_Blur_MaskColor_Test)
{
    auto materialBlurPtr = CreateFilter(RSNGEffectType::MATERIAL_BLUR);
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(materialBlurPtr);
    ASSERT_NE(materialBlurFilter, nullptr);

    // Set MaskColor tag - Convert RGBA to RSColor
    uint32_t rgba = 0xCC8000FF;  // 0xCC8000FF = A=FF(255), R=80(128), G=00(0), B=CC(204)
    uint8_t a = (rgba >> 24) & 0xFF;
    uint8_t r = (rgba >> 16) & 0xFF;
    uint8_t g = (rgba >> 8) & 0xFF;
    uint8_t b = rgba & 0xFF;
    RSColor maskColor = RSColor(r, g, b, a);
    materialBlurFilter->Setter<MaterialBlurMaskColorTag>(maskColor);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<uint32_t> rgbaValues = {
        0x800000FF,  // R=128, G=0, B=0, A=255
        0x008000FF,  // R=0, G=128, B=0, A=255
        0x000080FF,  // R=0, G=0, B=128, A=255
        0xCCCCCCFF   // R=204, G=204, B=204, A=255
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        // Convert RGBA to RSColor
        uint32_t color = rgbaValues[i];
        uint8_t ca = (color >> 24) & 0xFF;
        uint8_t cr = (color >> 16) & 0xFF;
        uint8_t cg = (color >> 8) & 0xFF;
        uint8_t cb = color & 0xFF;
        RSColor colorRS = RSColor(cr, cg, cb, ca);
        materialBlurFilter->Setter<MaterialBlurMaskColorTag>(colorRS);
        backgroundNode->SetBackgroundNGFilter(materialBlurFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterMaterialBlurTest, EFFECT_TEST, Set_NG_Filter_Material_Blur_ColorMode_Test)
{
    auto materialBlurPtr = CreateFilter(RSNGEffectType::MATERIAL_BLUR);
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(materialBlurPtr);
    ASSERT_NE(materialBlurFilter, nullptr);

    // Set ColorMode tag
    materialBlurFilter->Setter<MaterialBlurColorModeTag>(1);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<int> colorModeValues = {0, 1, 2, 3};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        materialBlurFilter->Setter<MaterialBlurColorModeTag>(colorModeValues[i]);
        backgroundNode->SetBackgroundNGFilter(materialBlurFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterMaterialBlurTest, EFFECT_TEST, Set_NG_Filter_Material_Blur_DisableSystemAdaptation_Test)
{
    auto materialBlurPtr = CreateFilter(RSNGEffectType::MATERIAL_BLUR);
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(materialBlurPtr);
    ASSERT_NE(materialBlurFilter, nullptr);

    // Set DisableSystemAdaptation tag
    materialBlurFilter->Setter<MaterialBlurDisableSystemAdaptationTag>(false);

    int nodeWidth = 460;
    int nodeHeight = 1400;
    int startX = 120;
    int startY = 300;
    int gap = 40;

    for (int i = 0; i < 2; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {startX + (nodeWidth + gap) * i, startY, nodeWidth, nodeHeight});
        materialBlurFilter->Setter<MaterialBlurDisableSystemAdaptationTag>(i == 0);
        backgroundNode->SetBackgroundNGFilter(materialBlurFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterMaterialBlurTest, EFFECT_TEST, Set_NG_Filter_Material_Blur_Complete_Test)
{
    auto materialBlurPtr = CreateFilter(RSNGEffectType::MATERIAL_BLUR);
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(materialBlurPtr);
    ASSERT_NE(materialBlurFilter, nullptr);

    // Set all tags together
    materialBlurFilter->Setter<MaterialBlurRadiusTag>(30.0f);
    materialBlurFilter->Setter<MaterialBlurSaturationTag>(0.6f);
    materialBlurFilter->Setter<MaterialBlurBrightnessTag>(0.2f);
    // Convert RGBA to RSColor for MaskColor
    uint32_t rgba = 0xB36699FF;  // 0xB36699FF = A=FF(255), R=99(153), G=66(102), B=B3(179)
    uint8_t a = (rgba >> 24) & 0xFF;
    uint8_t r = (rgba >> 16) & 0xFF;
    uint8_t g = (rgba >> 8) & 0xFF;
    uint8_t b = rgba & 0xFF;
    RSColor maskColor = RSColor(r, g, b, a);
    materialBlurFilter->Setter<MaterialBlurMaskColorTag>(maskColor);
    materialBlurFilter->Setter<MaterialBlurColorModeTag>(2);
    materialBlurFilter->Setter<MaterialBlurDisableSystemAdaptationTag>(true);

    int nodeWidth = 430;
    int nodeHeight = 700;
    int start = 120;
    int gap = 40;
    int col = 2;
    int row = 2;

    for (int j = 0; j < row; j++) {
        for (int i = 0; i < col; i++) {
            auto backgroundNode = SetUpNodeBgImage(BG_PATH,
                {start + (nodeWidth + gap) * i, start + (nodeHeight + gap) * j,
                 nodeWidth, nodeHeight});
            backgroundNode->SetBackgroundNGFilter(materialBlurFilter);
            GetRootNode()->AddChild(backgroundNode);
            RegisterNode(backgroundNode);
        }
    }
}

} // namespace OHOS::Rosen

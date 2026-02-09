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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSNodePropertiesTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetSandBox: normal values - 1x2 matrix */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetSandBox_Matrix_1x2)
{
    std::vector<bool> sandboxList = {false, true};

    for (size_t i = 0; i < sandboxList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400});
        testNode->SetSandBox(sandboxList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFgBrightnessRates: normal values - matrix 2x2 */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetFgBrightnessRates_Matrix_2x2)
{
    std::vector<float> brightRates = {0, 0.5f, 1};
    std::vector<float> darkRates = {0, 0.5f};

    for (size_t row = 0; row < brightRates.size(); row++) {
        for (size_t col = 0; col < darkRates.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 300 + 50),
                 200, 200});
            testNode->SetFgBrightnessRates(brightRates[row], darkRates[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFgBrightnessRates: boundary values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetFgBrightnessRates_Boundary)
{
    std::vector<float> extremeRates = {
        0,
        0.1f,
        0.9f,
        1,
        1.1f,
        2
    };

    for (size_t i = 0; i < extremeRates.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 400 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200, 200});
        testNode->SetFgBrightnessRates(extremeRates[i], 0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFgBrightnessSaturation: normal values - 1x5 matrix */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetFgBrightnessSaturation_Matrix_1x5)
{
    std::vector<float> saturations = {0, 0.25f, 0.5f,
                                        0.75f, 1};

    for (size_t i = 0; i < saturations.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 200 + 50), 50},
            150, 150);
        testNode->SetFgBrightnessSaturation(saturations[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFgBrightnessSaturation: boundary values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetFgBrightnessSaturation_Boundary)
{
    std::vector<float> boundarySaturations = {
        0,
        std::numeric_limits<float>::min(),
        1,
        1.1f,
        2,
        std::numeric_limits<float>::max()
    };

    for (size_t i = 0; i < boundarySaturations.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 400 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200, 200});
        testNode->SetFgBrightnessSaturation(boundarySaturations[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFgBrightnessPosCoeff: normal values - matrix 2x2 */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetFgBrightnessPosCoeff_Matrix_2x2)
{
    std::vector<Vector4f> coeffs = {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0.5f, 0.5f, 0.5f, 0.5f},
        {0.25f, 0.5f, 0.75f, 1}
    };

    for (size_t i = 0; i < coeffs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200, 200});
        testNode->SetFgBrightnessPosCoeff(coeffs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFgBrightnessNegCoeff: normal values - matrix 2x2 */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetFgBrightnessNegCoeff_Matrix_2x2)
{
    std::vector<Vector4f> coeffs = {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0.5f, 0.5f, 0.5f, 0.5f},
        {0.25f, 0.5f, 0.75f, 1}
    };

    for (size_t i = 0; i < coeffs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 100),
             static_cast<float>((i / 2) * 300 + 700),
             200, 200});
        testNode->SetFgBrightnessNegCoeff(coeffs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFgBrightnessHdr: normal values - 1x3 matrix */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetFgBrightnessHdr_Matrix_1x3)
{
    std::vector<float> hdrValues = {0, 0.5f, 1};

    for (size_t i = 0; i < hdrValues.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 350 + 50), 50},
            300, 300);
        testNode->SetFgBrightnessHdr(hdrValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetCompositingFilter: normal values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetCompositingFilter)
{
    auto filterObj = std::make_unique<Filter>();
    auto para = std::make_shared<FilterInnerShadowPara>();
    para->SetShadowRadius(10);
    filterObj->AddPara(para);

    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100}, 300, 300);
    testNode->SetCompositingFilter(filterObj);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetHDRBrightness: normal values - 1x4 matrix */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetHDRBrightness_Matrix_1x4)
{
    std::vector<float> brightnessValues = {0, 0.3f, 0.6f, 1};

    for (size_t i = 0; i < brightnessValues.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 275 + 50), 50},
            250, 250);
        testNode->SetHDRBrightness(brightnessValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetHDRBrightness: boundary values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetHDRBrightness_Boundary)
{
    std::vector<float> boundaryValues = {
        0,
        std::numeric_limits<float>::min(),
        1,
        1.1f,
        std::numeric_limits<float>::max()
    };

    for (size_t i = 0; i < boundaryValues.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 400 + 100),
             static_cast<float>((i / 2) * 300 + 50),
             200, 200});
        testNode->SetHDRBrightness(boundaryValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetHDRBrightnessFactor: normal values - matrix 2x2 */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetHDRBrightnessFactor_Matrix_2x2)
{
    std::vector<float> brightnessFactors = {0, 0.5f, 1};
    std::vector<float> saturationFactors = {0, 1};

    for (size_t row = 0; row < brightnessFactors.size(); row++) {
        for (size_t col = 0; col < saturationFactors.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 300 + 50),
                 200, 200});
            testNode->SetHDRBrightnessFactor(brightnessFactors[row], saturationFactors[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetHDRUIBrightness: normal values - 1x3 matrix */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetHDRUIBrightness_Matrix_1x3)
{
    std::vector<float> uiBrightnessValues = {0, 0.5f, 1};

    for (size_t i = 0; i < uiBrightnessValues.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 350 + 50), 50},
            300, 300);
        testNode->SetHDRUIBrightness(uiBrightnessValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetAttractionEffect: normal values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetAttractionEffect)
{
    struct AttractionInfo {
        float x;
        float y;
        const char* description;
    };

    std::vector<AttractionInfo> attractions = {
        {100, 100, "Center attraction"},
        {0, 0, "Origin attraction"},
        {200, 100, "Right attraction"},
        {100, 200, "Bottom attraction"}
    };

    for (size_t i = 0; i < attractions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 400 + 100),
             static_cast<float>((i / 2) * 400 + 100),
             300, 300});
        testNode->SetAttractionEffect(attractions[i].x, attractions[i].y);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetAttractionEffectFraction: normal values - 1x4 matrix */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetAttractionEffectFraction_Matrix_1x4)
{
    std::vector<float> fractions = {0, 0.3f, 0.6f, 1};

    for (size_t i = 0; i < fractions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 275 + 50), 50},
            250, 250);
        testNode->SetAttractionEffectFraction(fractions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetAttractionEffectDstPoint: normal values - matrix 2x2 */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetAttractionEffectDstPoint_Matrix_2x2)
{
    std::vector<float> xValues = {0, 100};
    std::vector<float> yValues = {0, 100};

    for (size_t row = 0; row < xValues.size(); row++) {
        for (size_t col = 0; col < yValues.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 300 + 50),
                 200, 200});
            testNode->SetAttractionEffectDstPoint(xValues[row], yValues[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetAlwaysSnapshot: normal values - 1x2 matrix */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetAlwaysSnapshot_Matrix_1x2)
{
    std::vector<bool> snapshotList = {false, true};

    for (size_t i = 0; i < snapshotList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
        testNode->SetAlwaysSnapshot(snapshotList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Combined: HDR brightness properties - matrix 2x2x2 */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_HDR_Brightness_Combined_Matrix_2x2x2)
{
    std::vector<float> brightnessValues = {0, 1};
    std::vector<float> factorBrightness = {0, 1};
    std::vector<float> factorSaturation = {0, 1};

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            for (size_t k = 0; k < 2; k++) {
                auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                    {static_cast<float>(((i + j) % 2) * 200 + 100),
                     static_cast<float>((k + (i / 2)) * 200 + 100),
                     150, 150});
                testNode->SetHDRBrightness(brightnessValues[i]);
                testNode->SetHDRBrightnessFactor(factorBrightness[j], factorSaturation[k]);
                GetRootNode()->AddChild(testNode);
                RegisterNode(testNode);
            }
        }
    }
}

/* SetUIBackgroundFilter: normal values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetUIBackgroundFilter)
{
    auto filterObj = std::make_unique<Filter>();
    auto para = std::make_shared<FilterBlurPara>();
    para->SetRadius(15);
    filterObj->AddPara(para);

    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100}, 300, 300);
    testNode->SetUIBackgroundFilter(filterObj.get());
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetUICompositingFilter: normal values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetUICompositingFilter)
{
    auto filterObj = std::make_unique<Filter>();
    auto para = std::make_shared<PixelStretchPara>();
    Vector4f tmpPercent{0, 0.25f, 0.75f, 1};
    para->SetStretchPercent(tmpPercent);
    filterObj->AddPara(para);

    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {500, 100}, 300, 300);
    testNode->SetUICompositingFilter(filterObj.get());
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetUIForegroundFilter: normal values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetUIForegroundFilter)
{
    auto filterObj = std::make_unique<Filter>();
    auto para = std::make_shared<FilterBlurPara>();
    para->SetRadius(30);
    filterObj->AddPara(para);

    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {900, 100}, 300, 300);
    testNode->SetUIForegroundFilter(filterObj.get());
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetUIMaterialFilter: normal values */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_SetUIMaterialFilter)
{
    auto filterObj = std::make_unique<Filter>();
    auto para = std::make_shared<FilterMaterialPara>();
    para->SetElevation(10);
    filterObj->AddPara(para);

    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 500}, 300, 300);
    testNode->SetUIMaterialFilter(filterObj.get());
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Rapid parameter changes */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_Rapid_Parameter_Changes)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100}, 400, 400);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Rapid HDR brightness changes
    testNode->SetHDRBrightness(0);
    testNode->SetHDRBrightness(0.5f);
    testNode->SetHDRBrightness(1);
    testNode->SetHDRBrightness(0.5f);

    // Rapid saturation changes
    testNode->SetFgBrightnessSaturation(0);
    testNode->SetFgBrightnessSaturation(0.5f);
    testNode->SetFgBrightnessSaturation(1);
}

/* Combined: Foreground brightness properties - matrix 3x3 */
GRAPHIC_TEST(RSNodePropertiesTest, CONTENT_DISPLAY_TEST, RSNode_FgBrightness_Combined_Matrix_3x3)
{
    std::vector<float> rates = {0, 0.5f, 1};
    std::vector<float> saturations = {0, 0.5f, 1};
    std::vector<float> hdr = {0, 0.5f, 1};

    for (size_t row = 0; row < 2; row++) {
        for (size_t col = 0; col < saturations.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 200 + 100),
                 static_cast<float>(row * 300 + 100),
                 150, 150});
            testNode->SetFgBrightnessRates(rates[row], 0);
            testNode->SetFgBrightnessSaturation(saturations[col]);
            testNode->SetFgBrightnessHdr(hdr[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

} // namespace OHOS::Rosen

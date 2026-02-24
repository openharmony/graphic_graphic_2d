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
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Radius variations
const std::vector<float> variableRadiusBlurRadii = {10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f};

// Boundary values
const std::vector<float> variableRadiusBlurBoundaryRadii = {0.0f, 100.0f};

// Extreme values for robustness testing
const std::vector<float> variableRadiusBlurExtremeValues = {-10.0f, 150.0f};
}

class NGFilterVariableRadiusBlurTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGVariableRadiusBlurFilter>& variableRadiusBlurFilter)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGFilter(variableRadiusBlurFilter);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test variable radius blur with different radius values
 * Tests 8 different radius values
 */
GRAPHIC_TEST(NGFilterVariableRadiusBlurTest, EFFECT_TEST, Set_Variable_Radius_Blur_Radius_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 2;

    for (size_t i = 0; i < variableRadiusBlurRadii.size(); i++) {
        auto variableRadiusBlurFilter = std::make_shared<RSNGVariableRadiusBlurFilter>();
        InitVariableRadiusBlur(variableRadiusBlurFilter);
        variableRadiusBlurFilter->Setter<VariableRadiusBlurRadiusTag>(variableRadiusBlurRadii[i]);

        SetUpTestNode(i, columnCount, rowCount, variableRadiusBlurFilter);
    }
}

/*
 * Test variable radius blur with boundary values
 * Tests radius = 0 (min) and radius = 100 (max)
 */
GRAPHIC_TEST(NGFilterVariableRadiusBlurTest, EFFECT_TEST, Set_Variable_Radius_Blur_Boundary_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < variableRadiusBlurBoundaryRadii.size(); i++) {
        auto variableRadiusBlurFilter = std::make_shared<RSNGVariableRadiusBlurFilter>();
        InitVariableRadiusBlur(variableRadiusBlurFilter);
        variableRadiusBlurFilter->Setter<VariableRadiusBlurRadiusTag>(variableRadiusBlurBoundaryRadii[i]);

        SetUpTestNode(i, columnCount, rowCount, variableRadiusBlurFilter);
    }
}

/*
 * Test variable radius blur with extreme and invalid values
 * Tests negative value and value beyond maximum (>100)
 * Expected: System should handle gracefully (clamp or ignore)
 */
GRAPHIC_TEST(NGFilterVariableRadiusBlurTest, EFFECT_TEST, Set_Variable_Radius_Blur_Extreme_Values_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < variableRadiusBlurExtremeValues.size(); i++) {
        auto variableRadiusBlurFilter = std::make_shared<RSNGVariableRadiusBlurFilter>();
        InitVariableRadiusBlur(variableRadiusBlurFilter);
        variableRadiusBlurFilter->Setter<VariableRadiusBlurRadiusTag>(variableRadiusBlurExtremeValues[i]);

        SetUpTestNode(i, columnCount, rowCount, variableRadiusBlurFilter);
    }
}

} // namespace OHOS::Rosen

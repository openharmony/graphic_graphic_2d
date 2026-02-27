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

// Transition factor variations
const std::vector<float> maskTransitionFactors = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f, 1.25f};

// Parameter combinations for interaction testing
const std::vector<std::pair<float, bool>> maskTransitionFactorInverseCombinations = {
    {0.25f, true},   // Low factor, inverse mode
    {0.5f, true},    // Mid factor, inverse mode
    {0.75f, true},   // High factor, inverse mode
    {0.25f, false},  // Low factor, normal mode
    {0.75f, false}   // High factor, normal mode
};
}

class NGFilterMaskTransitionTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGMaskTransitionFilter>& maskTransitionFilter)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGFilter(maskTransitionFilter);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test mask transition with different factor values
 * Tests 6 different transition factors including boundary and out-of-range values
 */
GRAPHIC_TEST(NGFilterMaskTransitionTest, EFFECT_TEST, Set_Mask_Transition_Factor_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 2;

    for (size_t i = 0; i < maskTransitionFactors.size(); i++) {
        auto maskTransitionFilter = std::make_shared<RSNGMaskTransitionFilter>();
        InitMaskTransition(maskTransitionFilter);
        maskTransitionFilter->Setter<MaskTransitionFactorTag>(maskTransitionFactors[i]);

        SetUpTestNode(i, columnCount, rowCount, maskTransitionFilter);
    }
}

/*
 * Test mask transition with inverse flag
 * Tests normal and inverse transition modes
 */
GRAPHIC_TEST(NGFilterMaskTransitionTest, EFFECT_TEST, Set_Mask_Transition_Inverse_Test)
{
    // Test normal mode
    auto maskTransitionFilter = std::make_shared<RSNGMaskTransitionFilter>();
    InitMaskTransition(maskTransitionFilter);
    maskTransitionFilter->Setter<MaskTransitionFactorTag>(0.5f);
    maskTransitionFilter->Setter<MaskTransitionInverseTag>(false);

    auto testNode1 = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT});
    testNode1->SetBackgroundNGFilter(maskTransitionFilter);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Test inverse mode
    auto maskTransitionFilterInv = std::make_shared<RSNGMaskTransitionFilter>();
    InitMaskTransition(maskTransitionFilterInv);
    maskTransitionFilterInv->Setter<MaskTransitionFactorTag>(0.5f);
    maskTransitionFilterInv->Setter<MaskTransitionInverseTag>(true);

    auto testNode2 = SetUpNodeBgImage(TEST_IMAGE_PATH, {SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT});
    testNode2->SetBackgroundNGFilter(maskTransitionFilterInv);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}

/*
 * Test mask transition with factor and inverse combinations
 * Tests interaction between transition factor and inverse mode
 */
GRAPHIC_TEST(NGFilterMaskTransitionTest, EFFECT_TEST, Set_Mask_Transition_Factor_Inverse_Combination_Test)
{
    const size_t columnCount = 5;

    for (size_t i = 0; i < maskTransitionFactorInverseCombinations.size(); i++) {
        auto maskTransitionFilter = std::make_shared<RSNGMaskTransitionFilter>();
        InitMaskTransition(maskTransitionFilter);
        maskTransitionFilter->Setter<MaskTransitionFactorTag>(maskTransitionFactorInverseCombinations[i].first);
        maskTransitionFilter->Setter<MaskTransitionInverseTag>(maskTransitionFactorInverseCombinations[i].second);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT;
        const size_t x = i * sizeX;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, 0, sizeX, sizeY});
        testNode->SetBackgroundNGFilter(maskTransitionFilter);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen

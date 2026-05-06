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
}

class NGFilterBlurBubblesRiseTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

// Basic function test - Verify basic functionality of blur bubbles rise effect
GRAPHIC_TEST(NGFilterBlurBubblesRiseTest, EFFECT_TEST, Set_NG_Filter_Blur_Bubbles_Rise_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;

    for (int i = 0; i < 4; i++) {
        auto blurBubblesRisePtr = CreateFilter(RSNGEffectType::BLUR_BUBBLES_RISE);
        auto blurBubblesRiseFilter = std::static_pointer_cast<RSNGBlurBubblesRiseFilter>(blurBubblesRisePtr);

        // Set standard parameters
        blurBubblesRiseFilter->Setter<BlurBubblesRiseBlurIntensityTag>(1.0f);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseMixStrengthTag>(0.5f);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseProgressTag>(0.5f);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(blurBubblesRiseFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Blur intensity parameter test - Test different blur intensity values
GRAPHIC_TEST(NGFilterBlurBubblesRiseTest, EFFECT_TEST, Set_NG_Filter_Blur_Bubbles_Rise_BlurIntensity_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;
    std::vector<float> blurIntensityVec = {0.3f, 0.7f, 1.1f, 1.5f};

    for (int i = 0; i < 4; i++) {
        auto blurBubblesRisePtr = CreateFilter(RSNGEffectType::BLUR_BUBBLES_RISE);
        auto blurBubblesRiseFilter = std::static_pointer_cast<RSNGBlurBubblesRiseFilter>(blurBubblesRisePtr);

        blurBubblesRiseFilter->Setter<BlurBubblesRiseBlurIntensityTag>(blurIntensityVec[i]);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseMixStrengthTag>(0.5f);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseProgressTag>(0.5f);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(blurBubblesRiseFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Mix strength parameter test - Test different mix strength values
GRAPHIC_TEST(NGFilterBlurBubblesRiseTest, EFFECT_TEST, Set_NG_Filter_Blur_Bubbles_Rise_MixStrength_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;
    std::vector<float> mixStrengthVec = {0.1f, 0.4f, 0.7f, 1.0f};

    for (int i = 0; i < 4; i++) {
        auto blurBubblesRisePtr = CreateFilter(RSNGEffectType::BLUR_BUBBLES_RISE);
        auto blurBubblesRiseFilter = std::static_pointer_cast<RSNGBlurBubblesRiseFilter>(blurBubblesRisePtr);

        blurBubblesRiseFilter->Setter<BlurBubblesRiseBlurIntensityTag>(1.0f);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseMixStrengthTag>(mixStrengthVec[i]);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseProgressTag>(0.5f);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(blurBubblesRiseFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Progress parameter test - Test different progress values
GRAPHIC_TEST(NGFilterBlurBubblesRiseTest, EFFECT_TEST, Set_NG_Filter_Blur_Bubbles_Rise_Progress_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;
    std::vector<float> progressVec = {0.0f, 0.3f, 0.6f, 0.9f};

    for (int i = 0; i < 4; i++) {
        auto blurBubblesRisePtr = CreateFilter(RSNGEffectType::BLUR_BUBBLES_RISE);
        auto blurBubblesRiseFilter = std::static_pointer_cast<RSNGBlurBubblesRiseFilter>(blurBubblesRisePtr);

        blurBubblesRiseFilter->Setter<BlurBubblesRiseBlurIntensityTag>(1.0f);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseMixStrengthTag>(0.5f);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseProgressTag>(progressVec[i]);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(blurBubblesRiseFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Mask parameter test - Test different mask images
GRAPHIC_TEST(NGFilterBlurBubblesRiseTest, EFFECT_TEST, Set_NG_Filter_Blur_Bubbles_Rise_Mask_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;
    std::vector<std::string> maskPaths = {"", MASK_SHADOW_PATH, MASK_RGB_PATH, FG_TEST_JPG_PATH};

    for (int i = 0; i < 4; i++) {
        auto blurBubblesRisePtr = CreateFilter(RSNGEffectType::BLUR_BUBBLES_RISE);
        auto blurBubblesRiseFilter = std::static_pointer_cast<RSNGBlurBubblesRiseFilter>(blurBubblesRisePtr);

        blurBubblesRiseFilter->Setter<BlurBubblesRiseBlurIntensityTag>(1.0f);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseMixStrengthTag>(0.5f);
        blurBubblesRiseFilter->Setter<BlurBubblesRiseProgressTag>(0.5f);

        // Set mask
        if (!maskPaths[i].empty()) {
            auto mask = DecodePixelMap(maskPaths[i].c_str(), Media::AllocatorType::SHARE_MEM_ALLOC);
            blurBubblesRiseFilter->Setter<BlurBubblesRiseMaskImageTag>(mask);
        }

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(blurBubblesRiseFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}
} // namespace OHOS::Rosen
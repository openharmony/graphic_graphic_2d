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
#include "ui_effect/property/include/rs_ui_mask_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
const int FALLBACK_MASK_SIZE = 64;

std::string GetAvailableBackgroundPath()
{
    const std::vector<std::string> candidates = {
        BG_PATH,
        APPEARANCE_TEST_JPG_PATH,
        FG_TEST_JPG_PATH,
    };
    for (const auto& path : candidates) {
        auto pixelMap = DecodePixelMap(path, Media::AllocatorType::SHARE_MEM_ALLOC);
        if (pixelMap != nullptr) {
            return path;
        }
    }
    return BG_PATH;
}

std::shared_ptr<Media::PixelMap> GetAvailableMask()
{
    const std::vector<std::string> candidates = {
        MASK_RGB_PATH,
        FG_TEST_JPG_PATH,
        BG_PATH,
        APPEARANCE_TEST_JPG_PATH,
    };
    for (const auto& path : candidates) {
        auto pixelMap = DecodePixelMap(path, Media::AllocatorType::SHARE_MEM_ALLOC);
        if (pixelMap != nullptr) {
            return pixelMap;
        }
    }

    Media::InitializationOptions options;
    options.size.width = FALLBACK_MASK_SIZE;
    options.size.height = FALLBACK_MASK_SIZE;
    return Media::PixelMap::Create(options);
}
}  // namespace

class NGFilterDispersionExtendTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        backgroundPath_ = GetAvailableBackgroundPath();
        maskRGB = GetAvailableMask();
    }

    std::string backgroundPath_ = BG_PATH;
    std::shared_ptr<Media::PixelMap> maskRGB = nullptr;
};


GRAPHIC_TEST(NGFilterDispersionExtendTest, EFFECT_TEST, Set_NG_Filter_Dispersion_Opacity_Test)
{
    auto dispersionPtr = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(dispersionPtr);

    // Set Opacity tag
    dispersionFilter->Setter<DispersionOpacityTag>(0.6f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> opacityValues = {0.1f, 0.4f, 0.7f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        dispersionFilter->Setter<DispersionOpacityTag>(opacityValues[i]);
        backgroundNode->SetBackgroundNGFilter(dispersionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test Dispersion with uncovered properties: Mask, Opacity, RedOffset, GreenOffset, BlueOffset
GRAPHIC_TEST(NGFilterDispersionExtendTest, EFFECT_TEST, Set_NG_Filter_Dispersion_Mask_Test)
{
    auto dispersionPtr = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(dispersionPtr);

    // Create and set ImageMask
    ASSERT_NE(maskRGB, nullptr);
    std::shared_ptr<RSNGImageMask> imageMask = std::make_shared<RSNGImageMask>();
    imageMask->Setter<ImageMaskImageTag>(maskRGB);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        dispersionFilter->Setter<DispersionMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
        backgroundNode->SetBackgroundNGFilter(dispersionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterDispersionExtendTest, EFFECT_TEST, Set_NG_Filter_Dispersion_ColorOffsets_Test)
{
    auto dispersionPtr = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(dispersionPtr);

    // Set RedOffset, GreenOffset, BlueOffset tags
    dispersionFilter->Setter<DispersionRedOffsetTag>(Vector2f(0.3f, 0.0f));
    dispersionFilter->Setter<DispersionGreenOffsetTag>(Vector2f(0.2f, 0.0f));
    dispersionFilter->Setter<DispersionBlueOffsetTag>(Vector2f(0.1f, 0.0f));

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector2f> offsetValues = {
        Vector2f(0.1f, 0.0f),
        Vector2f(0.2f, 0.0f),
        Vector2f(0.3f, 0.0f),
        Vector2f(0.4f, 0.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        dispersionFilter->Setter<DispersionRedOffsetTag>(offsetValues[i]);
        dispersionFilter->Setter<DispersionGreenOffsetTag>(offsetValues[i]);
        dispersionFilter->Setter<DispersionBlueOffsetTag>(offsetValues[i]);
        backgroundNode->SetBackgroundNGFilter(dispersionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterDispersionExtendTest, EFFECT_TEST, Set_NG_Filter_Dispersion_Complete_Test)
{
    auto dispersionPtr = CreateFilter(RSNGEffectType::DISPERSION);
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(dispersionPtr);

    // Create and set ImageMask
    ASSERT_NE(maskRGB, nullptr);
    std::shared_ptr<RSNGImageMask> imageMask = std::make_shared<RSNGImageMask>();
    imageMask->Setter<ImageMaskImageTag>(maskRGB);
    // Set all tags together
    dispersionFilter->Setter<DispersionMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
    dispersionFilter->Setter<DispersionOpacityTag>(0.75f);
    dispersionFilter->Setter<DispersionRedOffsetTag>(Vector2f(0.25f, 0.0f));
    dispersionFilter->Setter<DispersionGreenOffsetTag>(Vector2f(0.15f, 0.0f));
    dispersionFilter->Setter<DispersionBlueOffsetTag>(Vector2f(0.1f, 0.0f));

    int nodeWidth = 220;
    int nodeHeight = 220;
    int start = 100;

    auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundNGFilter(dispersionFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen

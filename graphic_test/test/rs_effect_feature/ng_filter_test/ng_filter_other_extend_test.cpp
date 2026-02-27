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

class NGFilterOtherExtendTest : public RSGraphicTest {
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

// Test ContentLight RotationAngle
GRAPHIC_TEST(NGFilterOtherExtendTest, EFFECT_TEST, Set_NG_Filter_Content_Light_RotationAngle_Test)
{
    auto contentLightPtr = CreateFilter(RSNGEffectType::CONTENT_LIGHT);
    auto contentLightFilter = std::static_pointer_cast<RSNGContentLightFilter>(contentLightPtr);
    ASSERT_NE(contentLightFilter, nullptr);

    // Set RotationAngle tag
    contentLightFilter->Setter<ContentLightRotationAngleTag>(Vector3f(45.0f, 0.0f, 0.0f));

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector3f> angleValues = {
        Vector3f(0.0f, 0.0f, 0.0f),
        Vector3f(45.0f, 0.0f, 0.0f),
        Vector3f(90.0f, 0.0f, 0.0f),
        Vector3f(135.0f, 0.0f, 0.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        contentLightFilter->Setter<ContentLightRotationAngleTag>(angleValues[i]);
        backgroundNode->SetForegroundNGFilter(contentLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test MaskTransition Mask
GRAPHIC_TEST(NGFilterOtherExtendTest, EFFECT_TEST, Set_NG_Filter_MaskTransition_Mask_Test)
{
    auto maskTransitionPtr = CreateFilter(RSNGEffectType::MASK_TRANSITION);
    auto maskTransitionFilter = std::static_pointer_cast<RSNGMaskTransitionFilter>(maskTransitionPtr);
    ASSERT_NE(maskTransitionFilter, nullptr);

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
        maskTransitionFilter->Setter<MaskTransitionMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
        backgroundNode->SetForegroundNGFilter(maskTransitionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test VariableRadiusBlur Mask
GRAPHIC_TEST(NGFilterOtherExtendTest, EFFECT_TEST, Set_NG_Filter_VariableRadiusBlur_Mask_Test)
{
    auto variableRadiusBlurPtr = CreateFilter(RSNGEffectType::VARIABLE_RADIUS_BLUR);
    auto variableRadiusBlurFilter = std::static_pointer_cast<RSNGVariableRadiusBlurFilter>(variableRadiusBlurPtr);
    ASSERT_NE(variableRadiusBlurFilter, nullptr);

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
        variableRadiusBlurFilter->Setter<VariableRadiusBlurMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
        backgroundNode->SetForegroundNGFilter(variableRadiusBlurFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen

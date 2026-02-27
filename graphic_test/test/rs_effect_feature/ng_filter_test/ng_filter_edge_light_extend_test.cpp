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

class NGFilterEdgeLightExtendTest : public RSGraphicTest {
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

// Test EdgeLight with uncovered properties: Alpha, Mask, UseRawColor
GRAPHIC_TEST(NGFilterEdgeLightExtendTest, EFFECT_TEST, Set_NG_Filter_Edge_Light_Alpha_Test)
{
    auto edgeLightPtr = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(edgeLightPtr);

    // Set Alpha tag
    edgeLightFilter->Setter<EdgeLightAlphaTag>(0.7f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> alphaValues = {0.2f, 0.5f, 0.7f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        edgeLightFilter->Setter<EdgeLightAlphaTag>(alphaValues[i]);
        backgroundNode->SetForegroundNGFilter(edgeLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterEdgeLightExtendTest, EFFECT_TEST, Set_NG_Filter_Edge_Light_Mask_Test)
{
    auto edgeLightPtr = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(edgeLightPtr);

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
        edgeLightFilter->Setter<EdgeLightMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
        backgroundNode->SetForegroundNGFilter(edgeLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}


GRAPHIC_TEST(NGFilterEdgeLightExtendTest, EFFECT_TEST, Set_NG_Filter_Edge_Light_UseRawColor_Test)
{
    auto edgeLightPtr = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(edgeLightPtr);

    // Set UseRawColor tag
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(true);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;

    for (int i = 0; i < 2; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (nodeWidth + 20) * i, nodeWidth, nodeHeight});
        edgeLightFilter->Setter<EdgeLightUseRawColorTag>(i == 0);
        backgroundNode->SetForegroundNGFilter(edgeLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterEdgeLightExtendTest, EFFECT_TEST, Set_NG_Filter_Edge_Light_Complete_Test)
{
    auto edgeLightPtr = CreateFilter(RSNGEffectType::EDGE_LIGHT);
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(edgeLightPtr);

    // Create and set ImageMask
    ASSERT_NE(maskRGB, nullptr);
    std::shared_ptr<RSNGImageMask> imageMask = std::make_shared<RSNGImageMask>();
    imageMask->Setter<ImageMaskImageTag>(maskRGB);

    // Convert RGBA to Vector4f
    uint32_t rgba = 0xFF996600;
    uint8_t a = (rgba >> 24) & 0xFF;
    uint8_t r = (rgba >> 16) & 0xFF;
    uint8_t g = (rgba >> 8) & 0xFF;
    uint8_t b = rgba & 0xFF;
    Vector4f vec = Vector4f(
        static_cast<float>(r) / 255.0f,
        static_cast<float>(g) / 255.0f,
        static_cast<float>(b) / 255.0f,
        static_cast<float>(a) / 255.0f
    );

    // Set all tags together
    edgeLightFilter->Setter<EdgeLightColorTag>(vec);
    edgeLightFilter->Setter<EdgeLightBloomTag>(true);
    edgeLightFilter->Setter<EdgeLightAlphaTag>(0.8f);
    edgeLightFilter->Setter<EdgeLightMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(false);

    int nodeWidth = 250;
    int nodeHeight = 250;
    int start = 100;

    auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetForegroundNGFilter(edgeLightFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen

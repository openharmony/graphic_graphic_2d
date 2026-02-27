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

class NGFilterDirectionLightTest : public RSGraphicTest {
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

// Test DirectionLight with uncovered properties
GRAPHIC_TEST(NGFilterDirectionLightTest, EFFECT_TEST, Set_NG_Filter_Direction_Light_Mask_Test)
{
    auto directionLightPtr = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(directionLightPtr);

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
        directionLightFilter->Setter<DirectionLightMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
        backgroundNode->SetBackgroundNGFilter(directionLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}


GRAPHIC_TEST(NGFilterDirectionLightTest, EFFECT_TEST, Set_NG_Filter_Direction_Light_Factor_Test)
{
    auto directionLightPtr = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(directionLightPtr);

    // Set Factor tag
    directionLightFilter->Setter<DirectionLightFactorTag>(0.5f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> factorValues = {0.0f, 0.25f, 0.5f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        directionLightFilter->Setter<DirectionLightFactorTag>(factorValues[i]);
        backgroundNode->SetBackgroundNGFilter(directionLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterDirectionLightTest, EFFECT_TEST, Set_NG_Filter_Direction_Light_Direction_Test)
{
    auto directionLightPtr = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(directionLightPtr);

    // Set Direction tag
    directionLightFilter->Setter<DirectionLightDirectionTag>(Vector3f(0.5f, 0.5f, 0.0f));

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector3f> directionValues = {
        Vector3f(0.0f, 1.0f, 0.0f),
        Vector3f(1.0f, 0.0f, 0.0f),
        Vector3f(0.7f, 0.7f, 0.0f),
        Vector3f(-0.5f, 0.5f, 0.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        directionLightFilter->Setter<DirectionLightDirectionTag>(directionValues[i]);
        backgroundNode->SetBackgroundNGFilter(directionLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterDirectionLightTest, EFFECT_TEST, Set_NG_Filter_Direction_Light_Color_Test)
{
    auto directionLightPtr = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(directionLightPtr);

    // Set Color tag - Convert RGBA to Vector4f
    uint32_t rgba = 0xFFE6CCFF;  // 0xFFE6CCFF = A=FF, R=E6(230), G=CC(204), B=FF(255)
    uint8_t a = (rgba >> 24) & 0xFF;
    uint8_t r = (rgba >> 16) & 0xFF;
    uint8_t g = (rgba >> 8) & 0xFF;
    uint8_t b = rgba & 0xFF;
    Vector4f colorVec = Vector4f(
        static_cast<float>(r) / 255.0f,
        static_cast<float>(g) / 255.0f,
        static_cast<float>(b) / 255.0f,
        static_cast<float>(a) / 255.0f
    );
    directionLightFilter->Setter<DirectionLightColorTag>(colorVec);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<uint32_t> rgbaValues = {
        0xFFFFFFFF,  // White
        0xFFCC99FF,  // Orange-ish
        0xFFE699CC,  // Blue-ish
        0xE6B38080   // Yellow-ish
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        // Convert RGBA to Vector4f
        uint32_t color = rgbaValues[i];
        uint8_t ca = (color >> 24) & 0xFF;
        uint8_t cr = (color >> 16) & 0xFF;
        uint8_t cg = (color >> 8) & 0xFF;
        uint8_t cb = color & 0xFF;
        Vector4f vec = Vector4f(
            static_cast<float>(cr) / 255.0f,
            static_cast<float>(cg) / 255.0f,
            static_cast<float>(cb) / 255.0f,
            static_cast<float>(ca) / 255.0f
        );
        directionLightFilter->Setter<DirectionLightColorTag>(vec);
        backgroundNode->SetBackgroundNGFilter(directionLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterDirectionLightTest, EFFECT_TEST, Set_NG_Filter_Direction_Light_Intensity_Test)
{
    auto directionLightPtr = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(directionLightPtr);

    // Set Intensity tag
    directionLightFilter->Setter<DirectionLightIntensityTag>(0.7f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> intensityValues = {0.1f, 0.4f, 0.7f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        directionLightFilter->Setter<DirectionLightIntensityTag>(intensityValues[i]);
        backgroundNode->SetBackgroundNGFilter(directionLightFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterDirectionLightTest, EFFECT_TEST, Set_NG_Filter_Direction_Light_Complete_Test)
{
    auto directionLightPtr = CreateFilter(RSNGEffectType::DIRECTION_LIGHT);
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(directionLightPtr);

    // Create and set ImageMask
    ASSERT_NE(maskRGB, nullptr);
    std::shared_ptr<RSNGImageMask> imageMask = std::make_shared<RSNGImageMask>();
    imageMask->Setter<ImageMaskImageTag>(maskRGB);

    // Convert RGBA to Vector4f for color
    uint32_t rgba = 0xFFF2CCFF;  // 0xFFF2CCFF = A=FF, R=F2(242), G=CC(204), B=FF(255)
    uint8_t a = (rgba >> 24) & 0xFF;
    uint8_t r = (rgba >> 16) & 0xFF;
    uint8_t g = (rgba >> 8) & 0xFF;
    uint8_t b = rgba & 0xFF;
    Vector4f colorVec = Vector4f(
        static_cast<float>(r) / 255.0f,
        static_cast<float>(g) / 255.0f,
        static_cast<float>(b) / 255.0f,
        static_cast<float>(a) / 255.0f
    );

    // Set all tags together
    directionLightFilter->Setter<DirectionLightMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
    directionLightFilter->Setter<DirectionLightFactorTag>(0.6f);
    directionLightFilter->Setter<DirectionLightDirectionTag>(Vector3f(0.7f, 0.3f, 0.0f));
    directionLightFilter->Setter<DirectionLightColorTag>(colorVec);
    directionLightFilter->Setter<DirectionLightIntensityTag>(0.8f);

    int nodeWidth = 220;
    int nodeHeight = 220;
    int start = 100;

    auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundNGFilter(directionLightFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen

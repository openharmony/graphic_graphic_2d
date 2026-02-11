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
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitPixelMapMask(std::shared_ptr<RSNGPixelMapMask>& pixelMapMask)
{
    if (!pixelMapMask) {
        return;
    }
    // Src: source rectangle (left, top, right, bottom)
    pixelMapMask->Setter<PixelMapMaskSrcTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    // Dst: destination rectangle (left, top, right, bottom)
    pixelMapMask->Setter<PixelMapMaskDstTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    // FillColor: white opaque
    pixelMapMask->Setter<PixelMapMaskFillColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Image: PixelMap (non-animatable, typically set by test)
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const std::string DISTOR_IMAGE_PATH = "/data/local/tmp/Images/distorImage.png";
const std::string CLOCK_IMAGE_PATH = "/data/local/tmp/Images/clockImage.png";

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Source rectangle variations
const std::vector<Vector4f> pixelMapSrcs = {
    Vector4f{0.0f, 0.0f, 0.5f, 0.5f},      // Top-left quarter
    Vector4f{0.5f, 0.5f, 1.0f, 1.0f},      // Bottom-right quarter
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f}       // Full
};

// Extreme source values
const std::vector<Vector4f> pixelMapExtremeSrcs = {
    Vector4f{-1.0f, -1.0f, 0.5f, 0.5f},    // Negative start
    Vector4f{0.5f, 0.5f, 2.0f, 2.0f},       // Above max (1.0)
    Vector4f{0.5f, 0.5f, 100.0f, 100.0f},  // Extremely large
    Vector4f{-10.0f, -10.0f, 1.0f, 1.0f}   // Large negative
};
}

class NGMaskPixelMapTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const std::shared_ptr<RSNGPixelMapMask>& pixelMapMask)
    {
        const size_t sizeX = SCREEN_WIDTH;
        const size_t sizeY = SCREEN_HEIGHT;

        auto parentNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
        parentNode->SetFrame({0, 0, sizeX, sizeY});
        GetRootNode()->AddChild(parentNode);
        RegisterNode(parentNode);

        auto effectNode = RSEffectNode::Create();
        effectNode->SetBounds({0, 0, sizeX, sizeY});
        effectNode->SetFrame({0, 0, sizeX, sizeY});
        effectNode->SetBackgroundColor(0xffff0000);
        parentNode->AddChild(effectNode);
        RegisterNode(effectNode);

        auto harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
        harmoniumEffect->Setter<HarmoniumEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(pixelMapMask));
        harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(1.0f);

        auto effectChildNode = RSCanvasNode::Create();
        effectChildNode->SetBounds({0, 0, sizeX, sizeY});
        effectChildNode->SetFrame({0, 0, sizeX, sizeY});
        effectChildNode->SetBackgroundNGShader(harmoniumEffect);
        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }
};

/*
 * Test pixel map mask with different source rectangles
 */
GRAPHIC_TEST(NGMaskPixelMapTest, EFFECT_TEST, Set_Pixel_Map_Mask_Src_Test)
{
    for (size_t i = 0; i < pixelMapSrcs.size(); i++) {
        auto pixelMapMask = std::make_shared<RSNGPixelMapMask>();
        InitPixelMapMask(pixelMapMask);

        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        pixelMapMask->Setter<PixelMapMaskImageTag>(pixelMap);
        pixelMapMask->Setter<PixelMapMaskSrcTag>(pixelMapSrcs[i]);

        SetUpTestNode(pixelMapMask);
    }
}

/*
 * Test pixel map mask with extreme source rectangle values
 * Tests malicious inputs: negative values, above max, extremely large
 */
GRAPHIC_TEST(NGMaskPixelMapTest, EFFECT_TEST, Set_Pixel_Map_Mask_Src_Extreme_Values_Test)
{
    for (size_t i = 0; i < pixelMapExtremeSrcs.size(); i++) {
        auto pixelMapMask = std::make_shared<RSNGPixelMapMask>();
        InitPixelMapMask(pixelMapMask);

        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        pixelMapMask->Setter<PixelMapMaskImageTag>(pixelMap);
        pixelMapMask->Setter<PixelMapMaskSrcTag>(pixelMapExtremeSrcs[i]);

        SetUpTestNode(pixelMapMask);
    }
}

} // namespace OHOS::Rosen

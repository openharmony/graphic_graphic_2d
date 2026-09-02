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
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitFractalGlassMask(std::shared_ptr<RSNGFractalGlassMask>& fractalGlassMask)
{
    if (!fractalGlassMask) {
        return;
    }
    // Src: source rectangle (left, top, right, bottom) in normalized coords
    fractalGlassMask->Setter<FractalGlassMaskSrcTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    // Dst: destination rectangle (left, top, right, bottom) in normalized coords
    fractalGlassMask->Setter<FractalGlassMaskDstTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    // Num: number of glass stripes
    fractalGlassMask->Setter<FractalGlassMaskNumTag>(25.0f);
    // Strength: glass distortion strength
    fractalGlassMask->Setter<FractalGlassMaskStrengthTag>(1.0f);
    // Softness: glass edge softness
    fractalGlassMask->Setter<FractalGlassMaskSoftnessTag>(0.001f);
    // Symmetric: whether distortion is symmetric (default true)
    fractalGlassMask->Setter<FractalGlassMaskSymmetricTag>(true);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const std::string DISTOR_IMAGE_PATH = "/data/local/tmp/Images/distorImage.png";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Glass num variations
const std::vector<float> glassNums = {5.0f, 10.0f, 25.0f, 50.0f};

// Glass strength variations
const std::vector<float> glassStrengths = {0.5f, 1.0f, 1.5f, 2.0f};

// Glass softness variations
const std::vector<float> glassSoftnesses = {0.001f, 0.003f, 0.005f, 0.01f};

// Source rectangle variations
const std::vector<Vector4f> glassSrcs = {
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},       // Full
    Vector4f{0.0f, 0.0f, 0.5f, 0.5f},       // Top-left quarter
    Vector4f{0.25f, 0.25f, 0.75f, 0.75f}    // Center region
};

// Extreme glass num values
const std::vector<float> glassExtremeNums = {
    -1.0f, -10.0f, 51.0f, 100.0f, 9999.0f
};

// Extreme glass strength values
const std::vector<float> glassExtremeStrengths = {
    -1.0f, 0.0f, 2.5f, 100.0f, std::numeric_limits<float>::quiet_NaN()
};

// Extreme glass softness values
const std::vector<float> glassExtremeSoftnesses = {
    -1.0f, 0.0f, 0.02f, 1.0f, std::numeric_limits<float>::quiet_NaN()
};

// Parameter combinations
const std::vector<std::tuple<float, float, float>> glassParamCombinations = {
    {25.0f, 1.0f, 0.001f},  // Default
    {10.0f, 0.5f, 0.005f},  // Few stripes, weak, soft
    {50.0f, 2.0f, 0.001f},  // Many stripes, strong, sharp
    {25.0f, 1.5f, 0.01f}    // Medium stripes, medium strength, soft
};
}

class NGMaskFractalGlassTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const std::shared_ptr<RSNGFractalGlassMask>& fractalGlassMask)
    {
        const size_t sizeX = SCREEN_WIDTH;
        const size_t sizeY = SCREEN_HEIGHT;

        auto parentNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
        parentNode->SetFrame({0, 0, sizeX, sizeY});
        GetRootNode()->AddChild(parentNode);
        RegisterNode(parentNode);

        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        effectNode->SetBounds({0, 0, sizeX, sizeY});
        effectNode->SetFrame({0, 0, sizeX, sizeY});
        effectNode->SetBackgroundColor(0xffff0000);
        parentNode->AddChild(effectNode);
        RegisterNode(effectNode);

        auto harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
        harmoniumEffect->Setter<HarmoniumEffectMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(fractalGlassMask));
        harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(1.0f);

        auto effectChildNode =
            RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        effectChildNode->SetBounds({0, 0, sizeX, sizeY});
        effectChildNode->SetFrame({0, 0, sizeX, sizeY});
        effectChildNode->SetBackgroundNGShader(harmoniumEffect);
        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }
};

/*
 * Test fractal glass mask with different glass num values
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Num_Test)
{
    for (size_t i = 0; i < glassNums.size(); i++) {
        auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
        InitFractalGlassMask(fractalGlassMask);
        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
        fractalGlassMask->Setter<FractalGlassMaskNumTag>(glassNums[i]);

        SetUpTestNode(fractalGlassMask);
    }
}

/*
 * Test fractal glass mask with different glass strength values
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Strength_Test)
{
    for (size_t i = 0; i < glassStrengths.size(); i++) {
        auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
        InitFractalGlassMask(fractalGlassMask);
        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
        fractalGlassMask->Setter<FractalGlassMaskStrengthTag>(glassStrengths[i]);

        SetUpTestNode(fractalGlassMask);
    }
}

/*
 * Test fractal glass mask with different glass softness values
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Softness_Test)
{
    for (size_t i = 0; i < glassSoftnesses.size(); i++) {
        auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
        InitFractalGlassMask(fractalGlassMask);
        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
        fractalGlassMask->Setter<FractalGlassMaskSoftnessTag>(glassSoftnesses[i]);

        SetUpTestNode(fractalGlassMask);
    }
}

/*
 * Test fractal glass mask with different source rectangles
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Src_Test)
{
    for (size_t i = 0; i < glassSrcs.size(); i++) {
        auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
        InitFractalGlassMask(fractalGlassMask);
        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
        fractalGlassMask->Setter<FractalGlassMaskSrcTag>(glassSrcs[i]);

        SetUpTestNode(fractalGlassMask);
    }
}

/*
 * Test fractal glass mask with parameter combinations
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Parameter_Combination_Test)
{
    for (size_t i = 0; i < glassParamCombinations.size(); i++) {
        auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
        InitFractalGlassMask(fractalGlassMask);
        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
        fractalGlassMask->Setter<FractalGlassMaskNumTag>(std::get<0>(glassParamCombinations[i]));
        fractalGlassMask->Setter<FractalGlassMaskStrengthTag>(std::get<1>(glassParamCombinations[i]));
        fractalGlassMask->Setter<FractalGlassMaskSoftnessTag>(std::get<2>(glassParamCombinations[i]));

        SetUpTestNode(fractalGlassMask);
    }
}

/*
 * Test fractal glass mask with extreme glass num values
 * Tests malicious inputs: negative values, out-of-range values, extremely large values
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Num_Extreme_Values_Test)
{
    for (size_t i = 0; i < glassExtremeNums.size(); i++) {
        auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
        InitFractalGlassMask(fractalGlassMask);
        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
        fractalGlassMask->Setter<FractalGlassMaskNumTag>(glassExtremeNums[i]);

        SetUpTestNode(fractalGlassMask);
    }
}

/*
 * Test fractal glass mask with extreme strength values
 * Tests malicious inputs: negative, zero, out-of-range, NaN
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Strength_Extreme_Values_Test)
{
    for (size_t i = 0; i < glassExtremeStrengths.size(); i++) {
        auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
        InitFractalGlassMask(fractalGlassMask);
        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
        fractalGlassMask->Setter<FractalGlassMaskStrengthTag>(glassExtremeStrengths[i]);

        SetUpTestNode(fractalGlassMask);
    }
}

/*
 * Test fractal glass mask with extreme softness values
 * Tests malicious inputs: negative, zero, out-of-range, NaN
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Softness_Extreme_Values_Test)
{
    for (size_t i = 0; i < glassExtremeSoftnesses.size(); i++) {
        auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
        InitFractalGlassMask(fractalGlassMask);
        std::shared_ptr<Media::PixelMap> pixelMap =
            DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
        fractalGlassMask->Setter<FractalGlassMaskSoftnessTag>(glassExtremeSoftnesses[i]);

        SetUpTestNode(fractalGlassMask);
    }
}

/*
 * Test fractal glass mask with isSymmetric = true (symmetric distortion)
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Symmetric_True_Test)
{
    auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
    InitFractalGlassMask(fractalGlassMask);
    std::shared_ptr<Media::PixelMap> pixelMap =
        DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
    fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
    fractalGlassMask->Setter<FractalGlassMaskSymmetricTag>(true);

    SetUpTestNode(fractalGlassMask);
}

/*
 * Test fractal glass mask with isSymmetric = false (asymmetric distortion)
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_Symmetric_False_Test)
{
    auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
    InitFractalGlassMask(fractalGlassMask);
    std::shared_ptr<Media::PixelMap> pixelMap =
        DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
    fractalGlassMask->Setter<FractalGlassMaskImageTag>(pixelMap);
    fractalGlassMask->Setter<FractalGlassMaskSymmetricTag>(false);

    SetUpTestNode(fractalGlassMask);
}

/*
 * Test fractal glass mask without pixelMap (NoTexture path)
 * When no image is provided, shader uses refractMask=1.0 (full effect everywhere)
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_NoTexture_Test)
{
    auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
    InitFractalGlassMask(fractalGlassMask);
    // Do NOT set pixelMap — test NoTexture shader path

    SetUpTestNode(fractalGlassMask);
}

/*
 * Test fractal glass mask without pixelMap + isSymmetric = false
 */
GRAPHIC_TEST(NGMaskFractalGlassTest, EFFECT_TEST, Set_Fractal_Glass_Mask_NoTexture_Asymmetric_Test)
{
    auto fractalGlassMask = std::make_shared<RSNGFractalGlassMask>();
    InitFractalGlassMask(fractalGlassMask);
    fractalGlassMask->Setter<FractalGlassMaskSymmetricTag>(false);
    // Do NOT set pixelMap — test NoTexture shader path

    SetUpTestNode(fractalGlassMask);
}

} // namespace OHOS::Rosen
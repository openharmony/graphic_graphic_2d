/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
using MaskCreator = std::function<std::shared_ptr<RSNGMaskBase>()>;
using MaskConvertor = std::function<std::shared_ptr<RSNGMaskBase>(std::shared_ptr<MaskPara>)>;
const std::string g_foregroundImagePath = "/data/local/tmp/fg_test.jpg";

static std::unordered_map<RSNGEffectType, MaskCreator> creatorMask = {
    {RSNGEffectType::NOISY_FRAME_GRADIENT_MASK,
     [] {
         return std::make_shared<RSNGNoisyFrameGradientMask>();
     }},
};

std::shared_ptr<RSNGMaskBase> CreateMask(RSNGEffectType type)
{
    auto it = creatorMask.find(type);
    return it != creatorMask.end() ? it->second() : nullptr;
}

constexpr int COLOR_GRADIENMT_EFFECT_PARAMS_COUNT = 15;
std::vector<std::array<std::variant<Vector2f, float, Vector4f>, COLOR_GRADIENMT_EFFECT_PARAMS_COUNT>>
    colorGradientEffectParams = {
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector2f{0.0f, 1.0f},
        Vector2f{1.0f, 0.0f},
        Vector2f{0.0f, 0.0f},
        Vector2f{1.0f, 1.0f},
        5.4f, 5.4f, 5.4f, 5.4f, 4.0f, 6.0f, 20.0f
    }
};

constexpr int NOISY_FRAME_GRADIENT_MASK_PARAMS_COUNT = 9;
std::vector<std::array<std::variant<Vector2f, float, Vector4f>, NOISY_FRAME_GRADIENT_MASK_PARAMS_COUNT>>
    noisyFrameGradientMaskParams = {
    // test large corner radius
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        1.0f,
        Vector2f{0.0f, 0.01f},
        Vector2f{0.0f, 0.01f},
        Vector2f{0.0f, 0.01f},
        Vector2f{0.5f, 0.5f},
        Vector2f{0.0f, 0.0f},
        0.0f, 0.0f
    },
    // test width bigger than height
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.1f,
        Vector2f{0.0f, 0.01f},
        Vector2f{0.0f, 0.01f},
        Vector2f{0.0f, 0.01f},
        Vector2f{1.0f, 0.3f},
        Vector2f{0.0f, 0.0f},
        0.0f, 0.0f
    },
    // test height bigger than width
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.1f,
        Vector2f{0.0f, 0.01f},
        Vector2f{0.0f, 0.01f},
        Vector2f{0.0f, 0.01f},
        Vector2f{0.3f, 1.0f},
        Vector2f{0.0f, 0.0f},
        0.0f, 0.0f
    },
    // test large frame width
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.01f,
        Vector2f{0.0f, 1.0f},
        Vector2f{0.0f, 1.0f},
        Vector2f{0.0f, 1.0f},
        Vector2f{0.5f, 0.5f},
        Vector2f{0.0f, 0.0f},
        0.0f, 0.0f
    },
    // test rect not included in canvas
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.1f,
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{1.0f, 1.0f},
        Vector2f{1.1f, 1.0f},
        0.0f, 0.0f
    },
    // test no corner radius
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.0f,
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{1.0f, 1.0f},
        Vector2f{0.0f, 0.0f},
        0.0f, 0.0f
    }
};

enum class TestDataGroupParamsType {
    INVALID_DATA_MIN,
    VALID_DATA1,
    VALID_DATA2,
    VALID_DATA3,
    INVALID_AND_VALID_DATA,
    INVALID_DATA_MAX,
    COUNT
};

constexpr uint32_t NUM_0 = 0;
constexpr uint32_t NUM_1 = 1;
constexpr uint32_t NUM_2 = 2;
constexpr uint32_t NUM_3 = 3;
constexpr uint32_t NUM_4 = 4;
constexpr uint32_t NUM_5 = 5;
constexpr uint32_t NUM_6 = 6;
constexpr uint32_t NUM_7 = 7;
constexpr uint32_t NUM_8 = 8;
constexpr uint32_t NUM_9 = 9;
constexpr uint32_t NUM_10 = 10;
constexpr uint32_t NUM_11 = 11;
constexpr uint32_t NUM_12 = 12;
constexpr uint32_t NUM_13 = 13;
constexpr uint32_t NUM_14 = 14;
}

class NGSubstractedRectTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

void SetColorGradientEffectParams(const std::shared_ptr<RSNGColorGradientEffect>& colorGradient, int index)
{
    if (!colorGradient) {
        return;
    }
    colorGradient->Setter<ColorGradientEffectColor0Tag>(
        std::get<Vector4f>(colorGradientEffectParams[index][NUM_0]));
    colorGradient->Setter<ColorGradientEffectColor1Tag>(
        std::get<Vector4f>(colorGradientEffectParams[index][NUM_1]));
    colorGradient->Setter<ColorGradientEffectColor2Tag>(
        std::get<Vector4f>(colorGradientEffectParams[index][NUM_2]));
    colorGradient->Setter<ColorGradientEffectColor3Tag>(
        std::get<Vector4f>(colorGradientEffectParams[index][NUM_3]));
    colorGradient->Setter<ColorGradientEffectPosition0Tag>(
        std::get<Vector2f>(colorGradientEffectParams[index][NUM_4]));
    colorGradient->Setter<ColorGradientEffectPosition1Tag>(
        std::get<Vector2f>(colorGradientEffectParams[index][NUM_5]));
    colorGradient->Setter<ColorGradientEffectPosition2Tag>(
        std::get<Vector2f>(colorGradientEffectParams[index][NUM_6]));
    colorGradient->Setter<ColorGradientEffectPosition3Tag>(
        std::get<Vector2f>(colorGradientEffectParams[index][NUM_7]));
    colorGradient->Setter<ColorGradientEffectStrength0Tag>(
        std::get<float>(colorGradientEffectParams[index][NUM_8]));
    colorGradient->Setter<ColorGradientEffectStrength1Tag>(
        std::get<float>(colorGradientEffectParams[index][NUM_9]));
    colorGradient->Setter<ColorGradientEffectStrength2Tag>(
        std::get<float>(colorGradientEffectParams[index][NUM_10]));
    colorGradient->Setter<ColorGradientEffectStrength3Tag>(
        std::get<float>(colorGradientEffectParams[index][NUM_11]));
    colorGradient->Setter<ColorGradientEffectColorNumberTag>(
        std::get<float>(colorGradientEffectParams[index][NUM_12]));
    colorGradient->Setter<ColorGradientEffectBlendTag>(
        std::get<float>(colorGradientEffectParams[index][NUM_13]));
    colorGradient->Setter<ColorGradientEffectBlendKTag>(
        std::get<float>(colorGradientEffectParams[index][NUM_14]));
}


void SetNoisyFrameGradientMaskParams(const std::shared_ptr<RSNGNoisyFrameGradientMask>& mask, int index)
{
    if (!mask) {
        return;
    }
    mask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(
        std::get<Vector4f>(noisyFrameGradientMaskParams[index][NUM_0]));
    mask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(
        std::get<float>(noisyFrameGradientMaskParams[index][NUM_1]));
    mask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_2]));
    mask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_3]));
    mask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_4]));
    mask->Setter<NoisyFrameGradientMaskRRectWHTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_5]));
    mask->Setter<NoisyFrameGradientMaskRRectPosTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_6]));
    mask->Setter<NoisyFrameGradientMaskSlopeTag>(
        std::get<float>(noisyFrameGradientMaskParams[index][NUM_7]));
    mask->Setter<NoisyFrameGradientMaskProgressTag>(
        std::get<float>(noisyFrameGradientMaskParams[index][NUM_8]));
}

// Test Foreground Noisy Frame Gradient Mask and Color Gradient Effect
GRAPHIC_TEST(NGSubstractedRectTest, EFFECT_TEST, Set_Substracted_Rect_NG_Mask_Noisy_Frame_Gradient_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create Noisy Frame Gradient Mask
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameGradientMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);
        SetNoisyFrameGradientMaskParams(noisyFrameGradientMask, i);

        // Create Color Gradient Effect
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        SetColorGradientEffectParams(colorGradient, 0);
        colorGradient->Setter<ColorGradientEffectMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(noisyFrameGradientMask));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage(g_foregroundImagePath, { x, y, sizeX, sizeY });
        foregroundTestNode->SetForegroundShader(colorGradient);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
    }
}

}  // namespace OHOS::Rosen

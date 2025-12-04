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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
using MaskCreator = std::function<std::shared_ptr<RSNGMaskBase>()>;
using MaskConvertor = std::function<std::shared_ptr<RSNGMaskBase>(std::shared_ptr<MaskPara>)>;

static std::unordered_map<RSNGEffectType, MaskCreator> creatorMask = {
    {RSNGEffectType::DOUBLE_RIPPLE_MASK,
     [] {
         return std::make_shared<RSNGDoubleRippleMask>();
     }},
};

std::shared_ptr<RSNGMaskBase> CreateMask(RSNGEffectType type)
{
    auto it = creatorMask.find(type);
    return it != creatorMask.end() ? it->second() : nullptr;
}

// Param arrays
constexpr int DOUBLE_RIPPLE_MASK_PARAMS_COUNT = 7;
std::vector<std::array<float, DOUBLE_RIPPLE_MASK_PARAMS_COUNT>> doubleRippleMaskParams = {
    {-20.0f, -20.0f, -20.0f, -20.0f, -1.0f, -1.0f, -1.0f},
    {0.15f, 0.15f, 0.15f, 0.15f, 0.4f, 0.35f, 0.75f},
    {0.25f, 0.25f, 0.75f, 0.75f, 0.5f, 0.5f, 0.5f},
    {0.15f, 0.15f, 0.65f, 0.65f, 0.8f, 0.15f, 0.35f},
    {10.0f, 10.0f, 0.75f, 0.75f, 0.5f, 0.5f, 12.0f},
    {20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f},
};

constexpr int CIRCLE_FLOWLIGHT_PARAMS_COUNT = 9;
std::vector<std::array<Vector4f, CIRCLE_FLOWLIGHT_PARAMS_COUNT>> circleFlowlightParams = {
    {
        Vector4f{-1.0f, -1.0f, -1.0f, -1.0f}, Vector4f{-1.0f, -1.0f, -1.0f, -1.0f},
        Vector4f{-1.0f, -1.0f, -1.0f, -1.0f}, Vector4f{-1.0f, -1.0f, -1.0f, -1.0f},
        Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f}, Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f},
        Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f}, Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f},
        Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f},
    },
    {
        Vector4f{0.9921f, 0.5020f, 0.0196f, 1.0f}, Vector4f{0.0039f, 0.5255f, 1.000f, 1.0f},
        Vector4f{0.0569f, 0.3429f, 1.000f, 1.0f}, Vector4f{0.5510f, 0.3412f, 0.5098f, 1.0f},
        Vector4f{0.500f, 0.600f, 0.300f, 0.5f}, Vector4f{0.300f, 0.300f, 0.300f, 0.3f},
        Vector4f{0.200f, 0.400f, 0.600f, 0.8f}, Vector4f{0.300f, 0.700f, 0.300f, 0.7f},
        Vector4f{0.300f, 0.300f, 0.700f, 0.7f},
    },
    {
        Vector4f{0.5f, 0.5f, 0.5f, 1.0f}, Vector4f{0.5f, 0.5f, 0.5f, 1.0f},
        Vector4f{0.5f, 0.5f, 0.5f, 1.0f}, Vector4f{0.5f, 0.5f, 0.5f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
    },
    {
        Vector4f{0.8f, 0.2f, 0.6f, 1.0f}, Vector4f{0.2f, 0.8f, 0.4f, 1.0f},
        Vector4f{0.6f, 0.4f, 0.8f, 1.0f}, Vector4f{0.4f, 0.6f, 0.2f, 1.0f},
        Vector4f{2.0f, 0.5f, 1.5f, 0.8f}, Vector4f{0.5f, 2.0f, 0.8f, 1.5f},
        Vector4f{1.5f, 0.8f, 0.5f, 2.0f}, Vector4f{0.8f, 1.5f, 2.0f, 0.5f},
        Vector4f{0.5f, 1.5f, 0.8f, 2.0f},
    },
    {
        Vector4f{0.5f, 0.5f, 0.5f, 1.0f}, Vector4f{2.0f, 2.0f, 2.0f, 2.0f},
        Vector4f{0.5f, 0.5f, 0.5f, 1.0f}, Vector4f{2.0f, 2.0f, 2.0f, 2.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
    },
    {
        Vector4f{2.0f, 2.0f, 2.0f, 2.0f}, Vector4f{2.0f, 2.0f, 2.0f, 2.0f},
        Vector4f{2.0f, 2.0f, 2.0f, 2.0f}, Vector4f{2.0f, 2.0f, 2.0f, 2.0f},
        Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f}, Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
        Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f}, Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
        Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
    },
};

constexpr int ROUNDED_RECT_FLOWLIGHT_PARAMS_COUNT = 12;
std::vector<std::array<std::variant<Vector2f, float, Vector4f>, ROUNDED_RECT_FLOWLIGHT_PARAMS_COUNT>>
    roundedRectFlowlightParams = {
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        -1.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 0.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f},
        1.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 0.0f, 1.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 1.0f, 1.0f},
        1.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.8f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        1.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
        Vector4f{2.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        2.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        -1.0f
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
}

class NGEffectTest : public RSGraphicTest {
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

void SetDoubleRippleMaskParams(const std::shared_ptr<RSNGDoubleRippleMask>& mask, int index)
{
    mask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[index][NUM_0], doubleRippleMaskParams[index][NUM_1] });
    mask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[index][NUM_2], doubleRippleMaskParams[index][NUM_3] });
    mask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[index][NUM_4]);
    mask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[index][NUM_5]);
    mask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[index][NUM_6]);
}

void SetRoundedRectFlowlightParams(const std::shared_ptr<RSNGRoundedRectFlowlight>& flowlight, int index)
{
    flowlight->Setter<RoundedRectFlowlightStartEndPositionTag>(
        std::get<Vector2f>(roundedRectFlowlightParams[index][NUM_0]));
    flowlight->Setter<RoundedRectFlowlightWaveLengthTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_1]));
    flowlight->Setter<RoundedRectFlowlightWaveTopTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_2]));
    flowlight->Setter<RoundedRectFlowlightCornerRadiusTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_3]));
    flowlight->Setter<RoundedRectFlowlightBrightnessTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_4]));
    flowlight->Setter<RoundedRectFlowlightScaleTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_5]));
    flowlight->Setter<RoundedRectFlowlightSharpingTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_6]));
    flowlight->Setter<RoundedRectFlowlightFeatheringTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_7]));
    flowlight->Setter<RoundedRectFlowlightFeatheringBezierControlPointsTag>(
        std::get<Vector4f>(roundedRectFlowlightParams[index][NUM_8]));
    flowlight->Setter<RoundedRectFlowlightGradientBezierControlPointsTag>(
        std::get<Vector4f>(roundedRectFlowlightParams[index][NUM_9]));
    flowlight->Setter<RoundedRectFlowlightColorTag>(
        std::get<Vector4f>(roundedRectFlowlightParams[index][NUM_10]));
    flowlight->Setter<RoundedRectFlowlightProgressTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_11]));
}

void SetCircleFlowlightParams(const std::shared_ptr<RSNGCircleFlowlight>& flowlight, int index)
{
    flowlight->Setter<CircleFlowlightColor0Tag>(circleFlowlightParams[index][NUM_0]);
    flowlight->Setter<CircleFlowlightColor1Tag>(circleFlowlightParams[index][NUM_1]);
    flowlight->Setter<CircleFlowlightColor2Tag>(circleFlowlightParams[index][NUM_2]);
    flowlight->Setter<CircleFlowlightColor3Tag>(circleFlowlightParams[index][NUM_3]);
    flowlight->Setter<CircleFlowlightRotationFrequencyTag>(circleFlowlightParams[index][NUM_4]);
    flowlight->Setter<CircleFlowlightRotationAmplitudeTag>(circleFlowlightParams[index][NUM_5]);
    flowlight->Setter<CircleFlowlightRotationSeedTag>(circleFlowlightParams[index][NUM_6]);
    flowlight->Setter<CircleFlowlightGradientXTag>(circleFlowlightParams[index][NUM_7]);
    flowlight->Setter<CircleFlowlightGradientYTag>(circleFlowlightParams[index][NUM_8]);
    flowlight->Setter<CircleFlowlightProgressTag>(0.0f);
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_Circle_Flowlight_Foreground_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);

        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetForegroundShader(circleFlowlight);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_Circle_Flowlight_Background_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);
        
        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(circleFlowlight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_Multi_NG_Effect_Circle_Flowlight_First_Foreground_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);
        
        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        // Create rounded rect flowlight effect
        auto roundedRectFlowlight = std::make_shared<RSNGRoundedRectFlowlight>();
        SetRoundedRectFlowlightParams(roundedRectFlowlight, i);

        circleFlowlight->Append(roundedRectFlowlight);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetForegroundShader(circleFlowlight);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_Multi_NG_Effect_Circle_Flowlight_Second_Foreground_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);

        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));
        
        // Create rounded rect flowlight effect
        auto roundedRectFlowlight = std::make_shared<RSNGRoundedRectFlowlight>();
        SetRoundedRectFlowlightParams(roundedRectFlowlight, i);

        roundedRectFlowlight->Append(circleFlowlight);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetForegroundShader(roundedRectFlowlight);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_Multi_NG_Effect_Circle_Flowlight_First_Background_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);

        // Create rounded rect flowlight effect
        auto roundedRectFlowlight = std::make_shared<RSNGRoundedRectFlowlight>();
        SetRoundedRectFlowlightParams(roundedRectFlowlight, i);

        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        circleFlowlight->Append(roundedRectFlowlight);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(circleFlowlight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_Multi_NG_Effect_Circle_Flowlight_Second_Background_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);

        // Create rounded rect flowlight effect
        auto roundedRectFlowlight = std::make_shared<RSNGRoundedRectFlowlight>();
        SetRoundedRectFlowlightParams(roundedRectFlowlight, i);

        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        roundedRectFlowlight->Append(circleFlowlight);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(roundedRectFlowlight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

}  // namespace OHOS::Rosen

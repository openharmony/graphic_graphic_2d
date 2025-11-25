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
namespace{
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

enum class TestDataGroupParamsType {
    INVALID_DATA_MIN,
    VALID_DATA1,
    VALID_DATA2,
    VALID_DATA3,
    INVALID_AND_VALID_DATA,
    INVALID_DATA_MAX,
    COUNT
};
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

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_Circle_Flowlight_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
            Vector2f{ doubleRippleMaskParams[i][0], doubleRippleMaskParams[i][1]});
        doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
            Vector2f{ doubleRippleMaskParams[i][2], doubleRippleMaskParams[i][3]});
        doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[i][4]);
        doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[i][5]);
        doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[i][6]);
        // Create displacement distort filter
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        circleFlowlight->Setter<CircleFlowlightColor0Tag>(circleFlowlightParams[i][0]);
        circleFlowlight->Setter<CircleFlowlightColor1Tag>(circleFlowlightParams[i][1]);
        circleFlowlight->Setter<CircleFlowlightColor2Tag>(circleFlowlightParams[i][2]);
        circleFlowlight->Setter<CircleFlowlightColor3Tag>(circleFlowlightParams[i][3]);
        circleFlowlight->Setter<CircleFlowlightRotationFrequencyTag>(circleFlowlightParams[i][4]);
        circleFlowlight->Setter<CircleFlowlightRotationAmplitudeTag>(circleFlowlightParams[i][5]);
        circleFlowlight->Setter<CircleFlowlightRotationSeedTag>(circleFlowlightParams[i][6]);
        circleFlowlight->Setter<CircleFlowlightGradientXTag>(circleFlowlightParams[i][7]);
        circleFlowlight->Setter<CircleFlowlightGradientYTag>(circleFlowlightParams[i][8]);
        circleFlowlight->Setter<CircleFlowlightProgressTag>(0.0f);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetForegroundShader(circleFlowlight);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

}  // namespace OHOS::Rosen

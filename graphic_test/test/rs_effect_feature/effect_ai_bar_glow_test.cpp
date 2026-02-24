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
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

constexpr size_t screenWidth = 1200;
constexpr size_t screenHeight = 2000;
constexpr float FOREGROUND_BRIGHTNESS_SCALE = 0.55f;
constexpr size_t GRAY_BLOCK_INDEX = 3;
constexpr size_t PURE_COLOR_INDEX = 9;

struct AIBarGlowParams {
    Vector4f ltwh; // left, top, width, height
    float stretchFactor;
    float barAngle;
    Vector4f color0;
    Vector4f color1;
    Vector4f color2;
    Vector4f color3;
    Vector2f position0;
    Vector2f position1;
    Vector2f position2;
    Vector2f position3;
    Vector4f strength;
    float brightness;
    float progress;
};

std::vector<AIBarGlowParams> aiBarGlowParams = {
    // Test basic parameters
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 1.0f,
        .barAngle = 0.0f,
        .color0 = {1.0f, 0.0f, 0.0f, 1.0f},
        .color1 = {0.0f, 1.0f, 0.0f, 1.0f},
        .color2 = {0.0f, 0.0f, 1.0f, 1.0f},
        .color3 = {1.0f, 1.0f, 0.0f, 1.0f},
        .position0 = {0.0f, 0.0f},
        .position1 = {0.33f, 0.0f},
        .position2 = {0.66f, 0.0f},
        .position3 = {1.0f, 0.0f},
        .strength = {0.2f, 0.3f, 0.4f, 0.5f},
        .brightness = 1.0f,
        .progress = 0.5f
    },
    // Test different colors
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 1.0f,
        .barAngle = 0.0f,
        .color0 = {0.0f, 1.0f, 1.0f, 1.0f},
        .color1 = {1.0f, 0.0f, 1.0f, 1.0f},
        .color2 = {1.0f, 1.0f, 1.0f, 1.0f},
        .color3 = {0.5f, 0.5f, 0.5f, 1.0f},
        .position0 = {0.0f, 0.0f},
        .position1 = {0.33f, 0.0f},
        .position2 = {0.66f, 0.0f},
        .position3 = {1.0f, 0.0f},
        .strength = {0.3f, 0.3f, 0.3f, 0.3f},
        .brightness = 1.0f,
        .progress = 0.6f
    },
    // Test different stretch factor
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 0.5f,
        .barAngle = 0.0f,
        .color0 = {1.0f, 0.5f, 0.0f, 1.0f},
        .color1 = {0.5f, 1.0f, 0.0f, 1.0f},
        .color2 = {0.0f, 0.5f, 1.0f, 1.0f},
        .color3 = {1.0f, 0.0f, 0.5f, 1.0f},
        .position0 = {0.0f, 0.0f},
        .position1 = {0.33f, 0.0f},
        .position2 = {0.66f, 0.0f},
        .position3 = {1.0f, 0.0f},
        .strength = {0.2f, 0.4f, 0.6f, 0.8f},
        .brightness = 0.8f,
        .progress = 0.4f
    },
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 1.5f,
        .barAngle = 20.0f,
        .color0 = {1.0f, 0.2f, 0.7f, 1.0f},
        .color1 = {0.2f, 1.0f, 0.7f, 1.0f},
        .color2 = {0.9f, 0.6f, 0.1f, 1.0f},
        .color3 = {0.5f, 0.3f, 1.0f, 1.0f},
        .position0 = {0.0f, 0.0f},
        .position1 = {0.25f, 0.0f},
        .position2 = {0.75f, 0.0f},
        .position3 = {1.0f, 0.0f},
        .strength = {0.25f, 0.35f, 0.45f, 0.55f},
        .brightness = 1.0f,
        .progress = 0.7f
    },
    // Test different bar angle
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 1.0f,
        .barAngle = 15.0f,
        .color0 = {0.9f, 0.1f, 0.4f, 1.0f},
        .color1 = {0.1f, 0.9f, 0.4f, 1.0f},
        .color2 = {0.4f, 0.1f, 0.9f, 1.0f},
        .color3 = {0.9f, 0.4f, 0.1f, 1.0f},
        .position0 = {0.0f, 0.0f},
        .position1 = {0.33f, 0.0f},
        .position2 = {0.66f, 0.0f},
        .position3 = {1.0f, 0.0f},
        .strength = {0.5f, 0.4f, 0.3f, 0.2f},
        .brightness = 1.0f,
        .progress = 0.5f
    },
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 1.0f,
        .barAngle = -15.0f,
        .color0 = {0.7f, 0.3f, 0.5f, 1.0f},
        .color1 = {0.3f, 0.7f, 0.5f, 1.0f},
        .color2 = {0.5f, 0.3f, 0.7f, 1.0f},
        .color3 = {0.7f, 0.5f, 0.3f, 1.0f},
        .position0 = {0.0f, 0.0f},
        .position1 = {0.33f, 0.0f},
        .position2 = {0.66f, 0.0f},
        .position3 = {1.0f, 0.0f},
        .strength = {0.3f, 0.5f, 0.4f, 0.6f},
        .brightness = 0.9f,
        .progress = 0.6f
    },
    // Test different positions
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 1.0f,
        .barAngle = 0.0f,
        .color0 = {1.0f, 0.0f, 0.3f, 1.0f},
        .color1 = {0.3f, 1.0f, 0.0f, 1.0f},
        .color2 = {0.0f, 0.3f, 1.0f, 1.0f},
        .color3 = {1.0f, 0.3f, 0.3f, 1.0f},
        .position0 = {0.0f, 0.2f},
        .position1 = {0.33f, 0.4f},
        .position2 = {0.66f, 0.6f},
        .position3 = {1.0f, 0.8f},
        .strength = {0.4f, 0.4f, 0.4f, 0.4f},
        .brightness = 1.0f,
        .progress = 0.5f
    },
    // Test different brightness
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 1.0f,
        .barAngle = 0.0f,
        .color0 = {0.6f, 0.4f, 0.7f, 1.0f},
        .color1 = {0.4f, 0.7f, 0.6f, 1.0f},
        .color2 = {0.7f, 0.6f, 0.4f, 1.0f},
        .color3 = {0.6f, 0.7f, 0.4f, 1.0f},
        .position0 = {0.0f, 0.0f},
        .position1 = {0.33f, 0.0f},
        .position2 = {0.66f, 0.0f},
        .position3 = {1.0f, 0.0f},
        .strength = {0.2f, 0.3f, 0.4f, 0.5f},
        .brightness = 0.5f,
        .progress = 0.5f
    },
    {
        .ltwh = {0.1f, 0.1f, 0.8f, 0.8f},
        .stretchFactor = 1.0f,
        .barAngle = 0.0f,
        .color0 = {0.5f, 0.5f, 0.8f, 1.0f},
        .color1 = {0.5f, 0.8f, 0.5f, 1.0f},
        .color2 = {0.8f, 0.5f, 0.5f, 1.0f},
        .color3 = {0.8f, 0.8f, 0.5f, 1.0f},
        .position0 = {0.0f, 0.0f},
        .position1 = {0.33f, 0.0f},
        .position2 = {0.66f, 0.0f},
        .position3 = {1.0f, 0.0f},
        .strength = {0.3f, 0.4f, 0.5f, 0.6f},
        .brightness = 1.5f,
        .progress = 0.5f
    },
    // Test invalid values
    {
        .ltwh = {-0.1f, -0.1f, 0.5f, 0.5f},
        .stretchFactor = -0.5f,
        .barAngle = -180.0f,
        .color0 = {-0.1f, 0.0f, 0.0f, 1.0f},
        .color1 = {0.0f, -0.1f, 0.0f, 1.0f},
        .color2 = {0.0f, 0.0f, -0.1f, 1.0f},
        .color3 = {0.0f, 0.0f, 0.0f, -0.1f},
        .position0 = {-0.1f, -0.1f},
        .position1 = {-0.2f, -0.1f},
        .position2 = {-0.3f, -0.1f},
        .position3 = {-0.4f, -0.1f},
        .strength = {-0.1f, -0.2f, -0.3f, -0.4f},
        .brightness = -0.5f,
        .progress = -0.3f
    }
};

} // namespace

class AIBarGlowTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth_ = screenWidth;
    const int screenHeight_ = screenHeight;
};

void SetAIBarGlowParams(const std::shared_ptr<RSNGAIBarGlow>& shader, const AIBarGlowParams& params)
{
    if (!shader) {
        return;
    }
    shader->Setter<AIBarGlowLTWHTag>(params.ltwh);
    shader->Setter<AIBarGlowStretchFactorTag>(params.stretchFactor);
    shader->Setter<AIBarGlowBarAngleTag>(params.barAngle);
    shader->Setter<AIBarGlowColor0Tag>(params.color0);
    shader->Setter<AIBarGlowColor1Tag>(params.color1);
    shader->Setter<AIBarGlowColor2Tag>(params.color2);
    shader->Setter<AIBarGlowColor3Tag>(params.color3);
    shader->Setter<AIBarGlowPosition0Tag>(params.position0);
    shader->Setter<AIBarGlowPosition1Tag>(params.position1);
    shader->Setter<AIBarGlowPosition2Tag>(params.position2);
    shader->Setter<AIBarGlowPosition3Tag>(params.position3);
    shader->Setter<AIBarGlowStrengthTag>(params.strength);
    shader->Setter<AIBarGlowBrightnessTag>(params.brightness);
    shader->Setter<AIBarGlowProgressTag>(params.progress);
}

AIBarGlowParams BuildForegroundParams(size_t index)
{
    AIBarGlowParams params = aiBarGlowParams[index];
    params.brightness = params.brightness * FOREGROUND_BRIGHTNESS_SCALE;

    if (index == GRAY_BLOCK_INDEX) {
        params.ltwh = {0.2f, 0.15f, 0.6f, 0.7f};
        params.barAngle = 25.0f;
        params.strength = {0.2f, 0.3f, 0.4f, 0.5f};
    }

    // Replace invalid foreground case with a valid but low-intensity pattern.
    if (index == PURE_COLOR_INDEX) {
        params.ltwh = {0.15f, 0.15f, 0.7f, 0.7f};
        params.stretchFactor = 1.0f;
        params.barAngle = 10.0f;
        params.color0 = {0.9f, 0.2f, 0.2f, 1.0f};
        params.color1 = {0.2f, 0.9f, 0.2f, 1.0f};
        params.color2 = {0.2f, 0.2f, 0.9f, 1.0f};
        params.color3 = {0.9f, 0.8f, 0.2f, 1.0f};
        params.position0 = {0.0f, 0.0f};
        params.position1 = {0.33f, 0.0f};
        params.position2 = {0.66f, 0.0f};
        params.position3 = {1.0f, 0.0f};
        params.strength = {0.2f, 0.3f, 0.4f, 0.5f};
        params.brightness = 0.45f;
        params.progress = 0.5f;
    }
    return params;
}

GRAPHIC_TEST(AIBarGlowTest, EFFECT_TEST, Set_AI_Bar_Glow_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = aiBarGlowParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < aiBarGlowParams.size(); ++i) {
        auto shader = std::make_shared<RSNGAIBarGlow>();
        SetAIBarGlowParams(shader, BuildForegroundParams(i));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto node = RSCanvasNode::Create();
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff222222);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(AIBarGlowTest, EFFECT_TEST, Set_AI_Bar_Glow_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = aiBarGlowParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < aiBarGlowParams.size(); ++i) {
        auto shader = std::make_shared<RSNGAIBarGlow>();
        SetAIBarGlowParams(shader, aiBarGlowParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen

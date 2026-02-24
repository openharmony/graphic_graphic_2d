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

struct GradientFlowColorsParams {
    Vector4f color0;
    Vector4f color1;
    Vector4f color2;
    Vector4f color3;
    float gradientBegin;
    float gradientEnd;
    float effectAlpha;
    float progress;
};

std::vector<GradientFlowColorsParams> gradientFlowColorsParams = {
    // Test basic parameters - red to blue gradient
    {
        .color0 = {1.0f, 0.0f, 0.0f, 1.0f},
        .color1 = {0.66f, 0.33f, 0.0f, 1.0f},
        .color2 = {0.33f, 0.66f, 0.0f, 1.0f},
        .color3 = {0.0f, 1.0f, 0.0f, 1.0f},
        .gradientBegin = 0.0f,
        .gradientEnd = 1.0f,
        .effectAlpha = 1.0f,
        .progress = 0.5f
    },
    // Test different color combination
    {
        .color0 = {0.0f, 1.0f, 0.0f, 1.0f},
        .color1 = {0.0f, 0.66f, 0.33f, 1.0f},
        .color2 = {0.0f, 0.33f, 0.66f, 1.0f},
        .color3 = {0.0f, 0.0f, 1.0f, 1.0f},
        .gradientBegin = 0.0f,
        .gradientEnd = 1.0f,
        .effectAlpha = 0.9f,
        .progress = 0.6f
    },
    // Test blue to red gradient
    {
        .color0 = {0.0f, 0.0f, 1.0f, 1.0f},
        .color1 = {0.33f, 0.0f, 0.66f, 1.0f},
        .color2 = {0.66f, 0.0f, 0.33f, 1.0f},
        .color3 = {1.0f, 0.0f, 0.0f, 1.0f},
        .gradientBegin = 0.0f,
        .gradientEnd = 1.0f,
        .effectAlpha = 0.8f,
        .progress = 0.7f
    },
    // Test different gradient range
    {
        .color0 = {1.0f, 0.5f, 0.0f, 1.0f},
        .color1 = {0.5f, 1.0f, 0.0f, 1.0f},
        .color2 = {0.0f, 1.0f, 0.5f, 1.0f},
        .color3 = {0.0f, 0.5f, 1.0f, 1.0f},
        .gradientBegin = 0.2f,
        .gradientEnd = 0.8f,
        .effectAlpha = 1.0f,
        .progress = 0.4f
    },
    {
        .color0 = {1.0f, 0.0f, 0.5f, 1.0f},
        .color1 = {0.5f, 0.0f, 1.0f, 1.0f},
        .color2 = {0.0f, 0.5f, 1.0f, 1.0f},
        .color3 = {0.0f, 1.0f, 0.5f, 1.0f},
        .gradientBegin = 0.1f,
        .gradientEnd = 0.9f,
        .effectAlpha = 0.7f,
        .progress = 0.5f
    },
    // Test different effect alpha
    {
        .color0 = {0.8f, 0.2f, 0.6f, 1.0f},
        .color1 = {0.2f, 0.8f, 0.6f, 1.0f},
        .color2 = {0.6f, 0.6f, 0.2f, 1.0f},
        .color3 = {0.4f, 0.4f, 0.8f, 1.0f},
        .gradientBegin = 0.0f,
        .gradientEnd = 1.0f,
        .effectAlpha = 0.3f,
        .progress = 0.5f
    },
    {
        .color0 = {0.9f, 0.1f, 0.4f, 1.0f},
        .color1 = {0.1f, 0.9f, 0.4f, 1.0f},
        .color2 = {0.4f, 0.1f, 0.9f, 1.0f},
        .color3 = {0.9f, 0.4f, 0.1f, 1.0f},
        .gradientBegin = 0.0f,
        .gradientEnd = 1.0f,
        .effectAlpha = 0.6f,
        .progress = 0.5f
    },
    // Test different progress values
    {
        .color0 = {1.0f, 0.3f, 0.2f, 1.0f},
        .color1 = {0.3f, 1.0f, 0.2f, 1.0f},
        .color2 = {0.2f, 0.3f, 1.0f, 1.0f},
        .color3 = {1.0f, 0.2f, 0.3f, 1.0f},
        .gradientBegin = 0.0f,
        .gradientEnd = 1.0f,
        .effectAlpha = 1.0f,
        .progress = 0.0f
    },
    {
        .color0 = {0.7f, 0.5f, 0.3f, 1.0f},
        .color1 = {0.5f, 0.7f, 0.3f, 1.0f},
        .color2 = {0.3f, 0.5f, 0.7f, 1.0f},
        .color3 = {0.7f, 0.3f, 0.5f, 1.0f},
        .gradientBegin = 0.0f,
        .gradientEnd = 1.0f,
        .effectAlpha = 1.0f,
        .progress = 1.0f
    },
    // Test monochromatic gradient
    {
        .color0 = {1.0f, 1.0f, 1.0f, 1.0f},
        .color1 = {0.75f, 0.75f, 0.75f, 1.0f},
        .color2 = {0.5f, 0.5f, 0.5f, 1.0f},
        .color3 = {0.25f, 0.25f, 0.25f, 1.0f},
        .gradientBegin = 0.0f,
        .gradientEnd = 1.0f,
        .effectAlpha = 1.0f,
        .progress = 0.5f
    },
    // Test invalid values
    {
        .color0 = {-0.1f, 0.0f, 0.0f, 1.0f},
        .color1 = {0.0f, -0.1f, 0.0f, 1.0f},
        .color2 = {0.0f, 0.0f, -0.1f, 1.0f},
        .color3 = {0.0f, 0.0f, 0.0f, -0.1f},
        .gradientBegin = -0.5f,
        .gradientEnd = 1.5f,
        .effectAlpha = -0.3f,
        .progress = -0.5f
    },
    {
        .color0 = {2.0f, 0.0f, 0.0f, 1.0f},
        .color1 = {0.0f, 2.0f, 0.0f, 1.0f},
        .color2 = {0.0f, 0.0f, 2.0f, 1.0f},
        .color3 = {0.0f, 0.0f, 0.0f, 2.0f},
        .gradientBegin = 0.2f,
        .gradientEnd = -0.2f,
        .effectAlpha = 1.5f,
        .progress = 2.0f
    }
};

} // namespace

class GradientFlowColorsTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth_ = screenWidth;
    const int screenHeight_ = screenHeight;
};

void SetGradientFlowColorsParams(const std::shared_ptr<RSNGGradientFlowColors>& shader,
    const GradientFlowColorsParams& params)
{
    if (!shader) {
        return;
    }
    shader->Setter<GradientFlowColorsColor0Tag>(params.color0);
    shader->Setter<GradientFlowColorsColor1Tag>(params.color1);
    shader->Setter<GradientFlowColorsColor2Tag>(params.color2);
    shader->Setter<GradientFlowColorsColor3Tag>(params.color3);
    shader->Setter<GradientFlowColorsGradientBeginTag>(params.gradientBegin);
    shader->Setter<GradientFlowColorsGradientEndTag>(params.gradientEnd);
    shader->Setter<GradientFlowColorsEffectAlphaTag>(params.effectAlpha);
    shader->Setter<GradientFlowColorsProgressTag>(params.progress);
}

GRAPHIC_TEST(GradientFlowColorsTest, EFFECT_TEST, Set_Gradient_Flow_Colors_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = gradientFlowColorsParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < gradientFlowColorsParams.size(); ++i) {
        auto shader = std::make_shared<RSNGGradientFlowColors>();
        SetGradientFlowColorsParams(shader, gradientFlowColorsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto node = RSCanvasNode::Create();
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(GradientFlowColorsTest, EFFECT_TEST, Set_Gradient_Flow_Colors_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = gradientFlowColorsParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < gradientFlowColorsParams.size(); ++i) {
        auto shader = std::make_shared<RSNGGradientFlowColors>();
        SetGradientFlowColorsParams(shader, gradientFlowColorsParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen

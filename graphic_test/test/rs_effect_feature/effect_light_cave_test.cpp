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

struct LightCaveParams {
    Vector4f colorA;
    Vector4f colorB;
    Vector4f colorC;
    Vector2f position;
    Vector2f radiusXY;
    float progress;
};

std::vector<LightCaveParams> lightCaveParams = {
    // Test basic parameters
    {
        .colorA = {1.0f, 0.0f, 0.0f, 1.0f},
        .colorB = {0.0f, 1.0f, 0.0f, 1.0f},
        .colorC = {0.0f, 0.0f, 1.0f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.3f, 0.3f},
        .progress = 0.5f
    },
    // Test different color combinations
    {
        .colorA = {1.0f, 1.0f, 0.0f, 1.0f},
        .colorB = {1.0f, 0.0f, 1.0f, 1.0f},
        .colorC = {0.0f, 1.0f, 1.0f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.4f, 0.4f},
        .progress = 0.3f
    },
    // Test different positions
    {
        .colorA = {0.8f, 0.2f, 0.2f, 1.0f},
        .colorB = {0.2f, 0.8f, 0.2f, 1.0f},
        .colorC = {0.2f, 0.2f, 0.8f, 1.0f},
        .position = {0.2f, 0.2f},
        .radiusXY = {0.3f, 0.3f},
        .progress = 0.6f
    },
    {
        .colorA = {0.9f, 0.3f, 0.3f, 1.0f},
        .colorB = {0.3f, 0.9f, 0.3f, 1.0f},
        .colorC = {0.3f, 0.3f, 0.9f, 1.0f},
        .position = {0.8f, 0.8f},
        .radiusXY = {0.25f, 0.25f},
        .progress = 0.7f
    },
    // Test different radius
    {
        .colorA = {1.0f, 0.5f, 0.0f, 1.0f},
        .colorB = {0.0f, 1.0f, 0.5f, 1.0f},
        .colorC = {0.5f, 0.0f, 1.0f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.1f, 0.1f},
        .progress = 0.4f
    },
    {
        .colorA = {1.0f, 0.4f, 0.2f, 1.0f},
        .colorB = {0.2f, 1.0f, 0.4f, 1.0f},
        .colorC = {0.4f, 0.2f, 1.0f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.5f, 0.5f},
        .progress = 0.8f
    },
    // Test different progress
    {
        .colorA = {1.0f, 0.0f, 0.3f, 1.0f},
        .colorB = {0.3f, 1.0f, 0.0f, 1.0f},
        .colorC = {0.0f, 0.3f, 1.0f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.3f, 0.3f},
        .progress = 0.0f
    },
    {
        .colorA = {0.7f, 0.0f, 0.5f, 1.0f},
        .colorB = {0.5f, 0.7f, 0.0f, 1.0f},
        .colorC = {0.0f, 0.5f, 0.7f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.35f, 0.35f},
        .progress = 1.0f
    },
    // Test invalid values
    {
        .colorA = {-0.1f, 0.0f, 0.0f, 1.0f},
        .colorB = {0.0f, -0.1f, 0.0f, 1.0f},
        .colorC = {0.0f, 0.0f, -0.1f, 1.0f},
        .position = {-0.1f, 0.5f},
        .radiusXY = {-0.1f, -0.1f},
        .progress = -0.5f
    },
    {
        .colorA = {2.0f, 0.0f, 0.0f, 1.0f},
        .colorB = {0.0f, 2.0f, 0.0f, 1.0f},
        .colorC = {0.0f, 0.0f, 2.0f, 1.0f},
        .position = {1.5f, 1.5f},
        .radiusXY = {2.0f, 2.0f},
        .progress = 2.0f
    }
};

} // namespace

class LightCaveTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth_ = screenWidth;
    const int screenHeight_ = screenHeight;
};

void SetLightCaveParams(const std::shared_ptr<RSNGLightCave>& shader, const LightCaveParams& params)
{
    if (!shader) {
        return;
    }
    shader->Setter<LightCaveColorATag>(params.colorA);
    shader->Setter<LightCaveColorBTag>(params.colorB);
    shader->Setter<LightCaveColorCTag>(params.colorC);
    shader->Setter<LightCavePositionTag>(params.position);
    shader->Setter<LightCaveRadiusXYTag>(params.radiusXY);
    shader->Setter<LightCaveProgressTag>(params.progress);
}

GRAPHIC_TEST(LightCaveTest, EFFECT_TEST, Set_Light_Cave_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = lightCaveParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < lightCaveParams.size(); ++i) {
        auto shader = std::make_shared<RSNGLightCave>();
        SetLightCaveParams(shader, lightCaveParams[i]);

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

GRAPHIC_TEST(LightCaveTest, EFFECT_TEST, Set_Light_Cave_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = lightCaveParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < lightCaveParams.size(); ++i) {
        auto shader = std::make_shared<RSNGLightCave>();
        SetLightCaveParams(shader, lightCaveParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen

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

struct WavyRippleLightParams {
    Vector2f center;
    float radius;
    float thickness;
};

std::vector<WavyRippleLightParams> wavyRippleLightParams = {
    // Test basic parameters
    {{0.5f, 0.5f}, 0.3f, 0.05f},
    // Test different radius
    {{0.5f, 0.5f}, 0.1f, 0.03f},
    {{0.5f, 0.5f}, 0.5f, 0.04f},
    {{0.5f, 0.5f}, 0.8f, 0.06f},
    // Test different thickness
    {{0.5f, 0.5f}, 0.3f, 0.01f},
    {{0.5f, 0.5f}, 0.3f, 0.1f},
    {{0.5f, 0.5f}, 0.3f, 0.2f},
    // Test different center positions
    {{0.2f, 0.2f}, 0.3f, 0.05f},
    {{0.8f, 0.2f}, 0.3f, 0.05f},
    {{0.2f, 0.8f}, 0.3f, 0.05f},
    {{0.8f, 0.8f}, 0.3f, 0.05f},
    // Test invalid values
    {{-0.1f, 0.5f}, -0.1f, -0.01f},
    {{1.5f, 0.5f}, 2.0f, 0.5f}
};

} // namespace

class WavyRippleLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth_ = screenWidth;
    const int screenHeight_ = screenHeight;
};

void SetWavyRippleLightParams(const std::shared_ptr<RSNGWavyRippleLight>& shader,
    const WavyRippleLightParams& params)
{
    if (!shader) {
        return;
    }
    shader->Setter<WavyRippleLightCenterTag>(params.center);
    shader->Setter<WavyRippleLightRadiusTag>(params.radius);
    shader->Setter<WavyRippleLightThicknessTag>(params.thickness);
}

GRAPHIC_TEST(WavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = wavyRippleLightParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < wavyRippleLightParams.size(); ++i) {
        auto shader = std::make_shared<RSNGWavyRippleLight>();
        SetWavyRippleLightParams(shader, wavyRippleLightParams[i]);

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

GRAPHIC_TEST(WavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = wavyRippleLightParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < wavyRippleLightParams.size(); ++i) {
        auto shader = std::make_shared<RSNGWavyRippleLight>();
        SetWavyRippleLightParams(shader, wavyRippleLightParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen

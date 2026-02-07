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

struct BorderLightParams {
    Vector3f position;
    Vector4f color;
    float intensity;
    float width;
    Vector3f rotationAngle;
    float cornerRadius;
};

std::vector<BorderLightParams> borderLightParams = {
    // Test basic parameters
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {1.0f, 0.0f, 0.0f, 1.0f},
        .intensity = 0.5f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    // Test different colors
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {0.0f, 1.0f, 0.0f, 1.0f},
        .intensity = 0.6f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {0.0f, 0.0f, 1.0f, 1.0f},
        .intensity = 0.7f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    // Test different intensity
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {1.0f, 1.0f, 0.0f, 1.0f},
        .intensity = 0.1f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {1.0f, 0.5f, 0.0f, 1.0f},
        .intensity = 1.0f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    // Test different width
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {0.5f, 1.0f, 0.0f, 1.0f},
        .intensity = 0.5f,
        .width = 0.01f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {0.0f, 1.0f, 0.5f, 1.0f},
        .intensity = 0.5f,
        .width = 0.15f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    // Test different rotation angles
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {1.0f, 0.0f, 0.5f, 1.0f},
        .intensity = 0.5f,
        .width = 0.05f,
        .rotationAngle = {45.0f, 0.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {0.5f, 0.0f, 1.0f, 1.0f},
        .intensity = 0.5f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 45.0f, 0.0f},
        .cornerRadius = 0.1f
    },
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {1.0f, 0.5f, 0.5f, 1.0f},
        .intensity = 0.5f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 0.0f, 45.0f},
        .cornerRadius = 0.1f
    },
    // Test different corner radius
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {0.8f, 0.8f, 0.0f, 1.0f},
        .intensity = 0.5f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.0f
    },
    {
        .position = {0.0f, 0.0f, 0.0f},
        .color = {0.0f, 0.8f, 0.8f, 1.0f},
        .intensity = 0.5f,
        .width = 0.05f,
        .rotationAngle = {0.0f, 0.0f, 0.0f},
        .cornerRadius = 0.3f
    },
    // Test invalid values
    {
        .position = {-1.0f, -1.0f, -1.0f},
        .color = {-0.1f, 0.0f, 0.0f, 1.0f},
        .intensity = -0.5f,
        .width = -0.05f,
        .rotationAngle = {-360.0f, -360.0f, -360.0f},
        .cornerRadius = -0.1f
    },
    {
        .position = {2.0f, 2.0f, 2.0f},
        .color = {2.0f, 0.0f, 0.0f, 1.0f},
        .intensity = 2.0f,
        .width = 0.5f,
        .rotationAngle = {360.0f, 360.0f, 360.0f},
        .cornerRadius = 1.0f
    }
};

} // namespace

class BorderLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth_ = screenWidth;
    const int screenHeight_ = screenHeight;
};

void SetBorderLightParams(const std::shared_ptr<RSNGBorderLight>& shader, const BorderLightParams& params)
{
    if (!shader) {
        return;
    }
    shader->Setter<BorderLightPositionTag>(params.position);
    shader->Setter<BorderLightColorTag>(params.color);
    shader->Setter<BorderLightIntensityTag>(params.intensity);
    shader->Setter<BorderLightWidthTag>(params.width);
    shader->Setter<BorderLightRotationAngleTag>(params.rotationAngle);
    shader->Setter<BorderLightCornerRadiusTag>(params.cornerRadius);
}

GRAPHIC_TEST(BorderLightTest, EFFECT_TEST, Set_Border_Light_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = borderLightParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < borderLightParams.size(); ++i) {
        auto shader = std::make_shared<RSNGBorderLight>();
        SetBorderLightParams(shader, borderLightParams[i]);

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

GRAPHIC_TEST(BorderLightTest, EFFECT_TEST, Set_Border_Light_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = borderLightParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < borderLightParams.size(); ++i) {
        auto shader = std::make_shared<RSNGBorderLight>();
        SetBorderLightParams(shader, borderLightParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen

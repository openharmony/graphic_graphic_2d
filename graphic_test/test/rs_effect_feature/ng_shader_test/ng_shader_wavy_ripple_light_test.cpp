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
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
    
namespace {
const int32_t SCREEN_WIDTH = 1200;
const int32_t SCREEN_HEIGHT = 2000;

struct WavyRippleLightParams {
    Vector2f center;
    float radius;
    float thickness;
};

std::vector<WavyRippleLightParams> wavyRippleLightParamsCenter = {
    // Test different center positions
    {{0.2f, 0.2f}, 0.3f, 0.05f},
    {{0.8f, 0.2f}, 0.3f, 0.05f},
    {{0.2f, 0.8f}, 0.3f, 0.05f},
    {{0.8f, 0.8f}, 0.3f, 0.05f},
};

std::vector<WavyRippleLightParams> wavyRippleLightParamsRadius = {
    // Test different radius
    {{0.5f, 0.5f}, 0.1f, 0.03f},
    {{0.5f, 0.5f}, 0.5f, 0.04f},
    {{0.5f, 0.5f}, 0.8f, 0.06f},
};

std::vector<WavyRippleLightParams> wavyRippleLightParamsThickness = {
    // Test different thickness
    {{0.5f, 0.5f}, 0.3f, 0.01f},
    {{0.5f, 0.5f}, 0.3f, 0.1f},
    {{0.5f, 0.5f}, 0.3f, 0.2f},
};

std::vector<WavyRippleLightParams> wavyRippleLightParamsInvalidValues = {
    // Test invalid values
    {{-0.1f, 0.5f}, -0.1f, -0.01f},
    {{1.5f, 0.5f}, 2.0f, 0.5f},
};

} // namespace

class NGShaderWavyRippleLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
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

GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Center_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = wavyRippleLightParamsCenter.size();
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT * columnCount / rowCount;

    for (size_t i = 0; i < wavyRippleLightParamsCenter.size(); ++i) {
        auto shader = std::make_shared<RSNGWavyRippleLight>();
        SetWavyRippleLightParams(shader, wavyRippleLightParamsCenter[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        
        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Radius_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = wavyRippleLightParamsRadius.size();
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT * columnCount / rowCount;

    for (size_t i = 0; i < wavyRippleLightParamsRadius.size(); ++i) {
        auto shader = std::make_shared<RSNGWavyRippleLight>();
        SetWavyRippleLightParams(shader, wavyRippleLightParamsRadius[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        
        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Thickness_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = wavyRippleLightParamsThickness.size();
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT * columnCount / rowCount;

    for (size_t i = 0; i < wavyRippleLightParamsThickness.size(); ++i) {
        auto shader = std::make_shared<RSNGWavyRippleLight>();
        SetWavyRippleLightParams(shader, wavyRippleLightParamsThickness[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        
        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_InvalidValues_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = wavyRippleLightParamsInvalidValues.size();
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT * columnCount / rowCount;

    for (size_t i = 0; i < wavyRippleLightParamsInvalidValues.size(); ++i) {
        auto shader = std::make_shared<RSNGWavyRippleLight>();
        SetWavyRippleLightParams(shader, wavyRippleLightParamsInvalidValues[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        
        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

} // namespace OHOS::Rosen

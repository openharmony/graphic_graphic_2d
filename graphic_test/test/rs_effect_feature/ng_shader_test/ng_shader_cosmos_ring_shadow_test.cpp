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
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitCosmosRingShadow(std::shared_ptr<RSNGCosmosRingShadow>& shader, float ringRadius, float ringWidth,
    float blurRadius, float radialOffset, float softness, const Vector2f& offset, const Vector4f& color)
{
    if (!shader) {
        return;
    }
    shader->Setter<CosmosRingShadowShadowRingRadiusTag>(ringRadius);
    shader->Setter<CosmosRingShadowShadowRingWidthTag>(ringWidth);
    shader->Setter<CosmosRingShadowShadowRadiusTag>(blurRadius);
    shader->Setter<CosmosRingShadowShadowRadialOffsetTag>(radialOffset);
    shader->Setter<CosmosRingShadowSoftnessTag>(softness);
    shader->Setter<CosmosRingShadowShadowOffsetTag>(offset);
    shader->Setter<CosmosRingShadowShadowColorTag>(color);
}

namespace {
const int32_t SCREEN_WIDTH = 1200;
const int32_t SCREEN_HEIGHT = 2000;

constexpr float BASE_RING_RADIUS = 300.0f;
constexpr float BASE_RING_WIDTH = 30.0f;
constexpr float BASE_BLUR_RADIUS = 40.0f;
constexpr float BASE_RADIAL_OFFSET = 0.0f;
constexpr float BASE_SOFTNESS = 0.5f;
const Vector2f BASE_OFFSET = { 0.0f, 0.0f };
const Vector4f BASE_COLOR = { 0.0f, 0.0f, 0.0f, 0.7f };

// each param: normal + boundary + abnormal values merged
// shadowRingRadius [0, 4096]
const std::vector<float> ringRadiusValues = { 100.0f, 200.0f, 300.0f, 400.0f, 0.0f, 4096.0f, -1.0f, 99999.0f };
// shadowRingWidth [0, 1024]
const std::vector<float> ringWidthValues = { 20.0f, 60.0f, 180.0f, 540.0f, 0.0f, 1024.0f, -1.0f, 99999.0f };
// shadowRadius [0, 256]
const std::vector<float> blurRadiusValues = { 20.0f, 60.0f, 120.0f, 240.0f, 0.0f, 256.0f, -1.0f, 99999.0f };
// shadowRadialOffset [-4096, 4096]
const std::vector<float> radialOffsetValues = { -200.0f, 0.0f, 200.0f, -4096.0f, 4096.0f, -5000.0f, 99999.0f };
// softness [0, 1]
const std::vector<float> softnessValues = { 0.25f, 0.5f, 0.75f, 0.0f, 1.0f, -1.0f, 2.0f, 99999.0f };
// shadowOffset [-4096, 4096]
const std::vector<Vector2f> shadowOffsetValues = {
    Vector2f { 0.0f, 0.0f }, Vector2f { 60.0f, 60.0f }, Vector2f { -60.0f, -60.0f },
    Vector2f { -4096.0f, -4096.0f }, Vector2f { 4096.0f, 4096.0f }, Vector2f { -5000.0f, -5000.0f },
    Vector2f { 99999.0f, 99999.0f },
};
// shadowColor [0, 1]
const std::vector<Vector4f> colorValues = {
    Vector4f { 0.0f, 0.0f, 0.0f, 0.8f }, Vector4f { 1.0f, 0.0f, 0.0f, 0.8f },
    Vector4f { 0.0f, 0.0f, 1.0f, 0.8f }, Vector4f { 0.0f, 0.0f, 0.0f, 0.0f },
    Vector4f { 1.0f, 1.0f, 1.0f, 1.0f }, Vector4f { -1.0f, -1.0f, -1.0f, -1.0f },
    Vector4f { 2.0f, 2.0f, 2.0f, 2.0f }, Vector4f { 99999.0f, 99999.0f, 99999.0f, 99999.0f },
};
} // namespace

class NGShaderCosmosRingShadowTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    // left column: SetBackgroundNGShader
    void SetUpBgCell(size_t rowIdx, size_t totalRows, std::shared_ptr<RSNGShaderBase> shader)
    {
        int sizeX = SCREEN_WIDTH / 2;
        int sizeY = SCREEN_HEIGHT / static_cast<int>(totalRows);
        int x = 0;
        int y = static_cast<int>(rowIdx) * sizeY;
        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    // right column: SetOverlayNGShader
    void SetUpOverlayCell(size_t rowIdx, size_t totalRows, std::shared_ptr<RSNGShaderBase> shader)
    {
        int sizeX = SCREEN_WIDTH / 2;
        int sizeY = SCREEN_HEIGHT / static_cast<int>(totalRows);
        int x = sizeX;
        int y = static_cast<int>(rowIdx) * sizeY;
        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetOverlayNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    void SetUpBoth(size_t rowIdx, size_t totalRows, std::shared_ptr<RSNGShaderBase> shader)
    {
        SetUpBgCell(rowIdx, totalRows, shader);
        SetUpOverlayCell(rowIdx, totalRows, shader);
    }
};

GRAPHIC_TEST(NGShaderCosmosRingShadowTest, EFFECT_TEST, Cosmos_Ring_Shadow_RingRadius_Test)
{
    for (size_t i = 0; i < ringRadiusValues.size(); i++) {
        auto shader = std::make_shared<RSNGCosmosRingShadow>();
        InitCosmosRingShadow(shader, ringRadiusValues[i], BASE_RING_WIDTH, BASE_BLUR_RADIUS, BASE_RADIAL_OFFSET,
            BASE_SOFTNESS, BASE_OFFSET, BASE_COLOR);
        SetUpBoth(i, ringRadiusValues.size(), shader);
    }
}

GRAPHIC_TEST(NGShaderCosmosRingShadowTest, EFFECT_TEST, Cosmos_Ring_Shadow_RingWidth_Test)
{
    for (size_t i = 0; i < ringWidthValues.size(); i++) {
        auto shader = std::make_shared<RSNGCosmosRingShadow>();
        InitCosmosRingShadow(shader, BASE_RING_RADIUS, ringWidthValues[i], BASE_BLUR_RADIUS, BASE_RADIAL_OFFSET,
            BASE_SOFTNESS, BASE_OFFSET, BASE_COLOR);
        SetUpBoth(i, ringWidthValues.size(), shader);
    }
}

GRAPHIC_TEST(NGShaderCosmosRingShadowTest, EFFECT_TEST, Cosmos_Ring_Shadow_BlurRadius_Test)
{
    for (size_t i = 0; i < blurRadiusValues.size(); i++) {
        auto shader = std::make_shared<RSNGCosmosRingShadow>();
        InitCosmosRingShadow(shader, BASE_RING_RADIUS, BASE_RING_WIDTH, blurRadiusValues[i], BASE_RADIAL_OFFSET,
            BASE_SOFTNESS, BASE_OFFSET, BASE_COLOR);
        SetUpBoth(i, blurRadiusValues.size(), shader);
    }
}

GRAPHIC_TEST(NGShaderCosmosRingShadowTest, EFFECT_TEST, Cosmos_Ring_Shadow_RadialOffset_Test)
{
    for (size_t i = 0; i < radialOffsetValues.size(); i++) {
        auto shader = std::make_shared<RSNGCosmosRingShadow>();
        InitCosmosRingShadow(shader, BASE_RING_RADIUS, BASE_RING_WIDTH, BASE_BLUR_RADIUS, radialOffsetValues[i],
            BASE_SOFTNESS, BASE_OFFSET, BASE_COLOR);
        SetUpBoth(i, radialOffsetValues.size(), shader);
    }
}

GRAPHIC_TEST(NGShaderCosmosRingShadowTest, EFFECT_TEST, Cosmos_Ring_Shadow_Softness_Test)
{
    for (size_t i = 0; i < softnessValues.size(); i++) {
        auto shader = std::make_shared<RSNGCosmosRingShadow>();
        InitCosmosRingShadow(shader, BASE_RING_RADIUS, BASE_RING_WIDTH, BASE_BLUR_RADIUS, BASE_RADIAL_OFFSET,
            softnessValues[i], BASE_OFFSET, BASE_COLOR);
        SetUpBoth(i, softnessValues.size(), shader);
    }
}

GRAPHIC_TEST(NGShaderCosmosRingShadowTest, EFFECT_TEST, Cosmos_Ring_Shadow_ShadowOffset_Test)
{
    for (size_t i = 0; i < shadowOffsetValues.size(); i++) {
        auto shader = std::make_shared<RSNGCosmosRingShadow>();
        InitCosmosRingShadow(shader, BASE_RING_RADIUS, BASE_RING_WIDTH, BASE_BLUR_RADIUS, BASE_RADIAL_OFFSET,
            BASE_SOFTNESS, shadowOffsetValues[i], BASE_COLOR);
        SetUpBoth(i, shadowOffsetValues.size(), shader);
    }
}

GRAPHIC_TEST(NGShaderCosmosRingShadowTest, EFFECT_TEST, Cosmos_Ring_Shadow_Color_Test)
{
    for (size_t i = 0; i < colorValues.size(); i++) {
        auto shader = std::make_shared<RSNGCosmosRingShadow>();
        InitCosmosRingShadow(shader, BASE_RING_RADIUS, BASE_RING_WIDTH, BASE_BLUR_RADIUS, BASE_RADIAL_OFFSET,
            BASE_SOFTNESS, BASE_OFFSET, colorValues[i]);
        SetUpBoth(i, colorValues.size(), shader);
    }
}
} // namespace OHOS::Rosen

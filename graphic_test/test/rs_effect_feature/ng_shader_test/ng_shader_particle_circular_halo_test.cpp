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
#include "ui/rs_effect_node.h"
#include "rs_graphic_test_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitParticleCircularHalo(std::shared_ptr<RSNGParticleCircularHalo>& particleCircularHalo)
{
    if (!particleCircularHalo) {
        return;
    }
    // Center
    particleCircularHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
    // Radius
    particleCircularHalo->Setter<ParticleCircularHaloRadiusTag>(0.5f);
    // Noise
    particleCircularHalo->Setter<ParticleCircularHaloNoiseTag>(0.5f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Center positions
const std::vector<Vector2f> centers = {
    Vector2f{0.0f, 0.0f},
    Vector2f{0.5f, 0.5f},
    Vector2f{1.0f, 1.0f}
};

// Radius values
const std::vector<float> radii = {0.1f, 0.2f, 0.5f};

// Noise values
const std::vector<float> noises = {0.0f, 0.3f, 0.5f, 1.0f};

const std::vector<float> noiseValues = {0.0f, 0.3f, 0.5f, 0.7f, 1.0f};

std::shared_ptr<RSCanvasNode> CreateEffectChildNode(const int i, const int columnCount, const int rowCount,
    std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGParticleCircularHalo>& particleCircularHalo)
{
    auto sizeX = (columnCount != 0) ? (SCREEN_WIDTH / columnCount) : SCREEN_WIDTH;
    auto sizeY = (rowCount != 0) ? (SCREEN_HEIGHT * columnCount / rowCount) : SCREEN_HEIGHT;

    int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
    int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

    auto effectChildNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    effectChildNode->SetBounds(x, y, sizeX, sizeY);
    effectChildNode->SetFrame(x, y, sizeX, sizeY);
    effectChildNode->SetOverlayNGShader(particleCircularHalo);
    effectNode->AddChild(effectChildNode);
    return effectChildNode;
}
}

class NGShaderParticleCircularHaloTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void SetEffectChildNode(const int i, const int columnCount, const int rowCount,
        std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGParticleCircularHalo>& particleCircularHalo)
    {
        auto effectChildNode = CreateEffectChildNode(i, columnCount, rowCount, effectNode, particleCircularHalo);
        RegisterNode(effectChildNode);
    }

    std::shared_ptr<RSEffectNode> SetUpEffectNode()
    {
        auto backgroundTestNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        effectNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        effectNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        std::shared_ptr<Rosen::RSFilter> backFilter = Rosen::RSFilter::CreateMaterialFilter(10.f, 1, 1, 0,
            BLUR_COLOR_MODE::DEFAULT, true);
        effectNode->SetBackgroundFilter(backFilter);
        effectNode->SetClipToBounds(false);
        GetRootNode()->AddChild(backgroundTestNode);
        backgroundTestNode->AddChild(effectNode);
        RegisterNode(effectNode);
        RegisterNode(backgroundTestNode);
        return effectNode;
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGParticleCircularHalo>& particleHalo)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(particleHalo);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderParticleCircularHaloTest, EFFECT_TEST, Set_Particle_Circular_Halo_Radius_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = 1;
    auto effectNode = SetUpEffectNode();

    for (size_t i = 0; i < radii.size(); i++) {
        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        InitParticleCircularHalo(particleHalo);
        particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
        particleHalo->Setter<ParticleCircularHaloRadiusTag>(radii[i]);
        particleHalo->Setter<ParticleCircularHaloNoiseTag>(0.5f);

        SetEffectChildNode(i, columnCount, rowCount, effectNode, particleHalo);
    }
}

GRAPHIC_TEST(NGShaderParticleCircularHaloTest, EFFECT_TEST, Set_Particle_Circular_Halo_Noise_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = 1;
    auto effectNode = SetUpEffectNode();

    for (size_t i = 0; i < noises.size(); i++) {
        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        InitParticleCircularHalo(particleHalo);
        particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
        particleHalo->Setter<ParticleCircularHaloRadiusTag>(0.2f);
        particleHalo->Setter<ParticleCircularHaloNoiseTag>(noises[i]);

        SetEffectChildNode(i, columnCount, rowCount, effectNode, particleHalo);
    }
}

GRAPHIC_TEST(NGShaderParticleCircularHaloTest, EFFECT_TEST, Set_Particle_Circular_Halo_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    auto effectNode = SetUpEffectNode();
    
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        InitParticleCircularHalo(particleHalo);
        particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{600.0f, 1000.0f});
        particleHalo->Setter<ParticleCircularHaloRadiusTag>(extremeValues[i]);
        SetEffectChildNode(i, columnCount, rowCount, effectNode, particleHalo);
    }
}

} // namespace OHOS::Rosen
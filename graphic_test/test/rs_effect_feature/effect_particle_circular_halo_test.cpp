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

struct ParticleCircularHaloParams {
    Vector2f center;
    float radius;
    float noise;
};

std::vector<ParticleCircularHaloParams> particleCircularHaloParams = {
    // Test basic parameters
    {{0.5f, 0.5f}, 0.3f, 0.0f},
    // Test different radius
    {{0.5f, 0.5f}, 0.1f, 0.1f},
    {{0.5f, 0.5f}, 0.5f, 0.2f},
    {{0.5f, 0.5f}, 0.7f, 0.3f},
    // Test different noise levels
    {{0.5f, 0.5f}, 0.3f, 0.5f},
    {{0.5f, 0.5f}, 0.3f, 0.8f},
    {{0.5f, 0.5f}, 0.3f, 1.0f},
    // Test different center positions
    {{0.2f, 0.2f}, 0.3f, 0.2f},
    {{0.8f, 0.2f}, 0.3f, 0.2f},
    {{0.2f, 0.8f}, 0.3f, 0.2f},
    {{0.8f, 0.8f}, 0.3f, 0.2f},
    // Test invalid values
    {{-0.1f, 0.5f}, -0.1f, -0.1f},
    {{1.5f, 0.5f}, 2.0f, 2.0f}
};

} // namespace

class ParticleCircularHaloTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth_ = screenWidth;
    const int screenHeight_ = screenHeight;
};

void SetParticleCircularHaloParams(const std::shared_ptr<RSNGParticleCircularHalo>& shader,
    const ParticleCircularHaloParams& params)
{
    if (!shader) {
        return;
    }
    shader->Setter<ParticleCircularHaloCenterTag>(params.center);
    shader->Setter<ParticleCircularHaloRadiusTag>(params.radius);
    shader->Setter<ParticleCircularHaloNoiseTag>(params.noise);
}

GRAPHIC_TEST(ParticleCircularHaloTest, EFFECT_TEST, Set_Particle_Circular_Halo_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = particleCircularHaloParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < particleCircularHaloParams.size(); ++i) {
        auto shader = std::make_shared<RSNGParticleCircularHalo>();
        SetParticleCircularHaloParams(shader, particleCircularHaloParams[i]);

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

GRAPHIC_TEST(ParticleCircularHaloTest, EFFECT_TEST, Set_Particle_Circular_Halo_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = particleCircularHaloParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < particleCircularHaloParams.size(); ++i) {
        auto shader = std::make_shared<RSNGParticleCircularHalo>();
        SetParticleCircularHaloParams(shader, particleCircularHaloParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen

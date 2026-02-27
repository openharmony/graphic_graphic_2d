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

void InitParticleCircularHalo(std::shared_ptr<RSNGParticleCircularHalo>& particleCircularHalo)
{
    if (!particleCircularHalo) {
        return;
    }
    // Center
    particleCircularHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
    // Radius
    particleCircularHalo->Setter<ParticleCircularHaloRadiusTag>(50.0f);
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
    Vector2f{600.0f, 1000.0f},
    Vector2f{1200.0f, 2000.0f}
};

// Radius values
const std::vector<float> radii = {0.0f, 50.0f, 200.0f, 500.0f};

// Noise values
const std::vector<float> noises = {0.0f, 0.3f, 0.5f, 1.0f};
}

class NGShaderParticleCircularHaloTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
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
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < radii.size(); i++) {
        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        InitParticleCircularHalo(particleHalo);
        particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{600.0f, 1000.0f});
        particleHalo->Setter<ParticleCircularHaloRadiusTag>(radii[i]);
        particleHalo->Setter<ParticleCircularHaloNoiseTag>(0.5f);

        SetUpTestNode(i, columnCount, rowCount, particleHalo);
    }
}

GRAPHIC_TEST(NGShaderParticleCircularHaloTest, EFFECT_TEST, Set_Particle_Circular_Halo_Noise_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < noises.size(); i++) {
        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        InitParticleCircularHalo(particleHalo);
        particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{600.0f, 1000.0f});
        particleHalo->Setter<ParticleCircularHaloRadiusTag>(200.0f);
        particleHalo->Setter<ParticleCircularHaloNoiseTag>(noises[i]);

        SetUpTestNode(i, columnCount, rowCount, particleHalo);
    }
}

GRAPHIC_TEST(NGShaderParticleCircularHaloTest, EFFECT_TEST, Set_Particle_Circular_Halo_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        InitParticleCircularHalo(particleHalo);
        particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{600.0f, 1000.0f});
        particleHalo->Setter<ParticleCircularHaloRadiusTag>(extremeValues[i]);
        SetUpTestNode(i, columnCount, rowCount, particleHalo);
    }
}

} // namespace OHOS::Rosen
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
#include "ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
}

class NGFilterParticleAblationTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationProgressTag>(0.5f);
    particleAblationFilter->Setter<ParticleAblationAblationRateTag>(0.8f);
    particleAblationFilter->Setter<ParticleAblationCenters0Tag>(Vector2f{100, 100});
    particleAblationFilter->Setter<ParticleAblationCenters1Tag>(Vector2f{200, 200});
    particleAblationFilter->Setter<ParticleAblationCenters2Tag>(Vector2f{300, 300});
    particleAblationFilter->Setter<ParticleAblationCenters3Tag>(Vector2f{400, 400});
    particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(0.6f);
    particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(0.7f);
    particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(1000);
    particleAblationFilter->Setter<ParticleAblationWindTag>(Vector2f{1.0f, 0.5f});
    particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(10);
    particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(5);
    particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(3);
    particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(Vector2f{1.0f, 1.0f});

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_Progress_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationAblationRateTag>(0.8f);
    particleAblationFilter->Setter<ParticleAblationCenters0Tag>(Vector2f{100, 100});
    particleAblationFilter->Setter<ParticleAblationCenters1Tag>(Vector2f{200, 200});
    particleAblationFilter->Setter<ParticleAblationCenters2Tag>(Vector2f{300, 300});
    particleAblationFilter->Setter<ParticleAblationCenters3Tag>(Vector2f{400, 400});
    particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(0.6f);
    particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(0.7f);
    particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(1000);
    particleAblationFilter->Setter<ParticleAblationWindTag>(Vector2f{1.0f, 0.5f});
    particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(10);
    particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(5);
    particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(3);
    particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(Vector2f{1.0f, 1.0f});

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;
    std::vector<float> progressVec = {0.0f, 0.25f, 0.5f, 0.75f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        particleAblationFilter->Setter<ParticleAblationProgressTag>(progressVec[i]);
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_AblationRate_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationProgressTag>(0.5f);
    particleAblationFilter->Setter<ParticleAblationCenters0Tag>(Vector2f{100, 100});
    particleAblationFilter->Setter<ParticleAblationCenters1Tag>(Vector2f{200, 200});
    particleAblationFilter->Setter<ParticleAblationCenters2Tag>(Vector2f{300, 300});
    particleAblationFilter->Setter<ParticleAblationCenters3Tag>(Vector2f{400, 400});
    particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(0.6f);
    particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(0.7f);
    particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(1000);
    particleAblationFilter->Setter<ParticleAblationWindTag>(Vector2f{1.0f, 0.5f});
    particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(10);
    particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(5);
    particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(3);
    particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(Vector2f{1.0f, 1.0f});

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;
    std::vector<float> ablationRateVec = {0.2f, 0.4f, 0.6f, 0.8f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        particleAblationFilter->Setter<ParticleAblationAblationRateTag>(ablationRateVec[i]);
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_Centers_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationProgressTag>(0.5f);
    particleAblationFilter->Setter<ParticleAblationAblationRateTag>(0.8f);
    particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(0.6f);
    particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(0.7f);
    particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(1000);
    particleAblationFilter->Setter<ParticleAblationWindTag>(Vector2f{1.0f, 0.5f});
    particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(10);
    particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(5);
    particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(3);
    particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(Vector2f{1.0f, 1.0f});

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;
    std::vector<Vector2f> centers0Vec = {{100, 100}, {150, 150}, {200, 200}, {250, 250}};
    std::vector<Vector2f> centers1Vec = {{200, 200}, {250, 250}, {300, 300}, {350, 350}};
    std::vector<Vector2f> centers2Vec = {{300, 300}, {350, 350}, {400, 400}, {450, 450}};
    std::vector<Vector2f> centers3Vec = {{400, 400}, {450, 450}, {500, 500}, {550, 550}};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        particleAblationFilter->Setter<ParticleAblationCenters0Tag>(centers0Vec[i]);
        particleAblationFilter->Setter<ParticleAblationCenters1Tag>(centers1Vec[i]);
        particleAblationFilter->Setter<ParticleAblationCenters2Tag>(centers2Vec[i]);
        particleAblationFilter->Setter<ParticleAblationCenters3Tag>(centers3Vec[i]);
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_Glow_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationProgressTag>(0.5f);
    particleAblationFilter->Setter<ParticleAblationAblationRateTag>(0.8f);
    particleAblationFilter->Setter<ParticleAblationCenters0Tag>(Vector2f{100, 100});
    particleAblationFilter->Setter<ParticleAblationCenters1Tag>(Vector2f{200, 200});
    particleAblationFilter->Setter<ParticleAblationCenters2Tag>(Vector2f{300, 300});
    particleAblationFilter->Setter<ParticleAblationCenters3Tag>(Vector2f{400, 400});
    particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(1000);
    particleAblationFilter->Setter<ParticleAblationWindTag>(Vector2f{1.0f, 0.5f});
    particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(10);
    particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(5);
    particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(3);
    particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(Vector2f{1.0f, 1.0f});

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;
    std::vector<float> glowLevelVec = {0.2f, 0.4f, 0.6f, 0.8f};
    std::vector<float> glowBrightnessVec = {0.3f, 0.5f, 0.7f, 0.9f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(glowLevelVec[i]);
        particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(glowBrightnessVec[i]);
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_MaxParticleCount_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationProgressTag>(0.5f);
    particleAblationFilter->Setter<ParticleAblationAblationRateTag>(0.8f);
    particleAblationFilter->Setter<ParticleAblationCenters0Tag>(Vector2f{100, 100});
    particleAblationFilter->Setter<ParticleAblationCenters1Tag>(Vector2f{200, 200});
    particleAblationFilter->Setter<ParticleAblationCenters2Tag>(Vector2f{300, 300});
    particleAblationFilter->Setter<ParticleAblationCenters3Tag>(Vector2f{400, 400});
    particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(0.6f);
    particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(0.7f);
    particleAblationFilter->Setter<ParticleAblationWindTag>(Vector2f{1.0f, 0.5f});
    particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(10);
    particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(5);
    particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(3);
    particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(Vector2f{1.0f, 1.0f});

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;
    std::vector<int> maxParticleCountVec = {500, 1000, 1500, 2000};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});

        particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(maxParticleCountVec[i]);
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_Wind_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationProgressTag>(0.5f);
    particleAblationFilter->Setter<ParticleAblationAblationRateTag>(0.8f);
    particleAblationFilter->Setter<ParticleAblationCenters0Tag>(Vector2f{100, 100});
    particleAblationFilter->Setter<ParticleAblationCenters1Tag>(Vector2f{200, 200});
    particleAblationFilter->Setter<ParticleAblationCenters2Tag>(Vector2f{300, 300});
    particleAblationFilter->Setter<ParticleAblationCenters3Tag>(Vector2f{400, 400});
    particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(0.6f);
    particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(0.7f);
    particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(1000);
    particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(10);
    particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(5);
    particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(3);
    particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(Vector2f{1.0f, 1.0f});

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;
    std::vector<Vector2f> windVec = {{0.0f, 0.0f}, {0.5f, 0.0f}, {1.0f, 0.5f}, {1.5f, 1.0f}};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        particleAblationFilter->Setter<ParticleAblationWindTag>(windVec[i]);
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_Turb_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationProgressTag>(0.5f);
    particleAblationFilter->Setter<ParticleAblationAblationRateTag>(0.8f);
    particleAblationFilter->Setter<ParticleAblationCenters0Tag>(Vector2f{100, 100});
    particleAblationFilter->Setter<ParticleAblationCenters1Tag>(Vector2f{200, 200});
    particleAblationFilter->Setter<ParticleAblationCenters2Tag>(Vector2f{300, 300});
    particleAblationFilter->Setter<ParticleAblationCenters3Tag>(Vector2f{400, 400});
    particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(0.6f);
    particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(0.7f);
    particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(1000);
    particleAblationFilter->Setter<ParticleAblationWindTag>(Vector2f{1.0f, 0.5f});
    particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(Vector2f{1.0f, 1.0f});

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;
    std::vector<int> turbScaleVec = {5, 10, 15, 20};
    std::vector<int> turbEvoVec = {2, 5, 8, 10};
    std::vector<int> turbAmpVec = {1, 3, 5, 7};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(turbScaleVec[i]);
        particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(turbEvoVec[i]);
        particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(turbAmpVec[i]);
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterParticleAblationTest, EFFECT_TEST, Set_NG_Filter_Particle_Ablation_ExpansionSize_Test)
{
    auto particleAblationPtr = CreateFilter(RSNGEffectType::PARTICLE_ABLATION);
    auto particleAblationFilter = std::static_pointer_cast<RSNGParticleAblationFilter>(particleAblationPtr);
    particleAblationFilter->Setter<ParticleAblationProgressTag>(0.5f);
    particleAblationFilter->Setter<ParticleAblationAblationRateTag>(0.8f);
    particleAblationFilter->Setter<ParticleAblationCenters0Tag>(Vector2f{100, 100});
    particleAblationFilter->Setter<ParticleAblationCenters1Tag>(Vector2f{200, 200});
    particleAblationFilter->Setter<ParticleAblationCenters2Tag>(Vector2f{300, 300});
    particleAblationFilter->Setter<ParticleAblationCenters3Tag>(Vector2f{400, 400});
    particleAblationFilter->Setter<ParticleAblationGlowLevelTag>(0.6f);
    particleAblationFilter->Setter<ParticleAblationGlowBrightnessTag>(0.7f);
    particleAblationFilter->Setter<ParticleAblationMaxParticleCountTag>(1000);
    particleAblationFilter->Setter<ParticleAblationWindTag>(Vector2f{1.0f, 0.5f});
    particleAblationFilter->Setter<ParticleAblationTurbScaleTag>(10);
    particleAblationFilter->Setter<ParticleAblationTurbEvoTag>(5);
    particleAblationFilter->Setter<ParticleAblationTurbAmpTag>(3);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;
    std::vector<Vector2f> expansionSizeVec = {{0.5f, 0.5f}, {0.75f, 0.75f}, {1.0f, 1.0f}, {1.25f, 1.25f}};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        particleAblationFilter->Setter<ParticleAblationExpansionSizeTag>(expansionSizeVec[i]);
        backgroundNode->SetForegroundNGFilter(particleAblationFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

}

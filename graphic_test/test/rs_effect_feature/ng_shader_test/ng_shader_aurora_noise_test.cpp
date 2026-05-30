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

void InitAuroraNoise(std::shared_ptr<RSNGAuroraNoise>& auroraNoise)
{
    if (!auroraNoise) {
        return;
    }
    // Noise
    auroraNoise->Setter<AuroraNoiseNoiseTag>(0.5f);
    auroraNoise->Setter<AuroraNoiseFreqXTag>(3.0f);
    auroraNoise->Setter<AuroraNoiseFreqYTag>(5.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Noise values
const std::vector<float> noiseValues = {0.0f, 0.3f, 0.5f, 0.7f, 1.0f};

std::shared_ptr<RSCanvasNode> CreateEffectChildNode(const int i, const int columnCount, const int rowCount,
    std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGAuroraNoise>& auroraNoise)
{
    auto sizeX = (columnCount != 0) ? (SCREEN_WIDTH / columnCount) : SCREEN_WIDTH;
    auto sizeY = (rowCount != 0) ? (SCREEN_HEIGHT * columnCount / rowCount) : SCREEN_HEIGHT;

    int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
    int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

    auto effectChildNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    effectChildNode->SetBounds(x, y, sizeX, sizeY);
    effectChildNode->SetFrame(x, y, sizeX, sizeY);
    effectChildNode->SetOverlayNGShader(auroraNoise);
    effectNode->AddChild(effectChildNode);
    return effectChildNode;
}
}

class NGShaderAuroraNoiseTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void SetEffectChildNode(const int i, const int columnCount, const int rowCount,
        std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGAuroraNoise>& auroraNoise)
    {
        auto effectChildNode = CreateEffectChildNode(i, columnCount, rowCount, effectNode, auroraNoise);
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
        std::shared_ptr<RSNGAuroraNoise>& auroraNoise)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(auroraNoise);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test aurora noise with different noise values
 */
GRAPHIC_TEST(NGShaderAuroraNoiseTest, EFFECT_TEST, Set_Aurora_Noise_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    auto effectNode = SetUpEffectNode();

    for (size_t i = 0; i < noiseValues.size(); i++) {
        auto auroraNoise = std::make_shared<RSNGAuroraNoise>();
        InitAuroraNoise(auroraNoise);
        auroraNoise->Setter<AuroraNoiseNoiseTag>(noiseValues[i]);

        SetEffectChildNode(i, columnCount, rowCount, effectNode, auroraNoise);
    }
}

/*
 * Test aurora noise with extreme and invalid noise values
 * Tests malicious inputs: negative values, extremely large values
 */
GRAPHIC_TEST(NGShaderAuroraNoiseTest, EFFECT_TEST, Set_Aurora_Noise_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    auto effectNode = SetUpEffectNode();

    const std::vector<float> extremeNoise = {-1.0f, -0.5f, 2.0f, 9999.0f};

    for (size_t i = 0; i < extremeNoise.size(); i++) {
        auto auroraNoise = std::make_shared<RSNGAuroraNoise>();
        InitAuroraNoise(auroraNoise);
        auroraNoise->Setter<AuroraNoiseNoiseTag>(extremeNoise[i]);

        SetEffectChildNode(i, columnCount, rowCount, effectNode, auroraNoise);
    }
}

} // namespace OHOS::Rosen
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

void InitAIBarRectHalo(std::shared_ptr<RSNGAIBarRectHalo>& aiBarRectHalo)
{
    if (!aiBarRectHalo) {
        return;
    }
    // LTWH: left, top, width, height
    aiBarRectHalo->Setter<AIBarRectHaloLTWHTag>(Vector4f{0.0f, 0.0f, 100.0f, 100.0f});
    // Colors
    aiBarRectHalo->Setter<AIBarRectHaloColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    aiBarRectHalo->Setter<AIBarRectHaloColor1Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    aiBarRectHalo->Setter<AIBarRectHaloColor2Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    aiBarRectHalo->Setter<AIBarRectHaloColor3Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    // Positions (distributed to avoid overlap)
    aiBarRectHalo->Setter<AIBarRectHaloPosition0Tag>(Vector2f{0.2f, 0.2f});
    aiBarRectHalo->Setter<AIBarRectHaloPosition1Tag>(Vector2f{0.8f, 0.2f});
    aiBarRectHalo->Setter<AIBarRectHaloPosition2Tag>(Vector2f{0.8f, 0.8f});
    aiBarRectHalo->Setter<AIBarRectHaloPosition3Tag>(Vector2f{0.2f, 0.8f});
    // Strength
    aiBarRectHalo->Setter<AIBarRectHaloStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Brightness
    aiBarRectHalo->Setter<AIBarRectHaloBrightnessTag>(1.0f);
    // Progress
    aiBarRectHalo->Setter<AIBarRectHaloProgressTag>(0.5f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Progress values
const std::vector<float> progressValues = {0.0f, 0.3f, 0.5f, 1.0f};

// Brightness values
const std::vector<float> brightnessValues = {0.0f, 0.5f, 1.0f, 2.0f};

// Strength values
const std::vector<Vector4f> strengthValues = {
    Vector4f{0.0f, 0.0f, 0.0f, 0.0f},
    Vector4f{0.5f, 0.5f, 0.5f, 0.5f},
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f}
};
}

class NGShaderAIBarRectHaloTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGAIBarRectHalo>& aiBarRectHalo)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(aiBarRectHalo);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderAIBarRectHaloTest, EFFECT_TEST, Set_AI_Bar_Rect_Halo_Progress_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < progressValues.size(); i++) {
        auto aiBarRectHalo = std::make_shared<RSNGAIBarRectHalo>();
        InitAIBarRectHalo(aiBarRectHalo);
        aiBarRectHalo->Setter<AIBarRectHaloProgressTag>(progressValues[i]);
        aiBarRectHalo->Setter<AIBarRectHaloLTWHTag>(Vector4f{0.0f, 0.0f, 100.0f, 100.0f});

        SetUpTestNode(i, columnCount, rowCount, aiBarRectHalo);
    }
}

GRAPHIC_TEST(NGShaderAIBarRectHaloTest, EFFECT_TEST, Set_AI_Bar_Rect_Halo_Brightness_Strength_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < strengthValues.size(); i++) {
        auto aiBarRectHalo = std::make_shared<RSNGAIBarRectHalo>();
        InitAIBarRectHalo(aiBarRectHalo);
        aiBarRectHalo->Setter<AIBarRectHaloProgressTag>(progressValues[i]);
        aiBarRectHalo->Setter<AIBarRectHaloBrightnessTag>(brightnessValues[i]);
        aiBarRectHalo->Setter<AIBarRectHaloStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        aiBarRectHalo->Setter<AIBarRectHaloLTWHTag>(Vector4f{0.0f, 0.0f, 100.0f, 100.0f});

        SetUpTestNode(i, columnCount, rowCount, aiBarRectHalo);
    }
}

GRAPHIC_TEST(NGShaderAIBarRectHaloTest, EFFECT_TEST, Set_AI_Bar_Rect_Halo_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto aiBarRectHalo = std::make_shared<RSNGAIBarRectHalo>();
        InitAIBarRectHalo(aiBarRectHalo);
        aiBarRectHalo->Setter<AIBarRectHaloProgressTag>(extremeValues[i]);
        aiBarRectHalo->Setter<AIBarRectHaloLTWHTag>(Vector4f{0.0f, 0.0f, 100.0f, 100.0f});
        SetUpTestNode(i, columnCount, rowCount, aiBarRectHalo);
    }
}

} // namespace OHOS::Rosen
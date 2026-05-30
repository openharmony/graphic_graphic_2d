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
#include "ui_effect/property/include/rs_ui_shape_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitBorderSDFShader(
    std::shared_ptr<RSNGBorderSDFShader>& shader, const Vector4f& color, float width, bool isOutline)
{
    if (!shader) {
        return;
    }
    constexpr int32_t BORDER_STYLE_SOLID = 0;
    shader->Setter<BorderSDFShaderColorTag>(color);
    shader->Setter<BorderSDFShaderWidthTag>(width);
    shader->Setter<BorderSDFShaderIsOutlineTag>(isOutline);
    shader->Setter<BorderSDFShaderStyleTag>(BORDER_STYLE_SOLID);
}

void InitBorderSDFLGColor(std::shared_ptr<RSNGBorderSDFLGColor>& shader, float angle, float width, bool isOutline,
    int colorNumber, const Vector4f* colors, const float* positions)
{
    if (!shader) {
        return;
    }
    constexpr int32_t INDEX_0 = 0;
    constexpr int32_t INDEX_1 = 1;
    constexpr int32_t INDEX_2 = 2;
    constexpr int32_t INDEX_3 = 3;
    constexpr int32_t INDEX_4 = 4;
    shader->Setter<BorderSDFLGColorAngleTag>(angle);
    shader->Setter<BorderSDFLGColorWidthTag>(width);
    shader->Setter<BorderSDFLGColorIsOutlineTag>(isOutline);
    shader->Setter<BorderSDFLGColorColorNumberTag>(colorNumber);
    shader->Setter<BorderSDFLGColorColor0Tag>(colors[INDEX_0]);
    shader->Setter<BorderSDFLGColorColor1Tag>(colors[INDEX_1]);
    shader->Setter<BorderSDFLGColorColor2Tag>(colors[INDEX_2]);
    shader->Setter<BorderSDFLGColorColor3Tag>(colors[INDEX_3]);
    shader->Setter<BorderSDFLGColorColor4Tag>(colors[INDEX_4]);
    shader->Setter<BorderSDFLGColorPosition0Tag>(positions[INDEX_0]);
    shader->Setter<BorderSDFLGColorPosition1Tag>(positions[INDEX_1]);
    shader->Setter<BorderSDFLGColorPosition2Tag>(positions[INDEX_2]);
    shader->Setter<BorderSDFLGColorPosition3Tag>(positions[INDEX_3]);
    shader->Setter<BorderSDFLGColorPosition4Tag>(positions[INDEX_4]);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
constexpr int32_t SCREEN_WIDTH = 1200;
constexpr int32_t SCREEN_HEIGHT = 2000;
constexpr float SHAPE_CORNER_RADIUS = 20.0f;

const std::vector<Vector4f> borderColors = {
    Vector4f { 1.0f, 0.0f, 0.0f, 1.0f },
    Vector4f { 0.0f, 1.0f, 0.0f, 1.0f },
    Vector4f { 0.0f, 0.0f, 1.0f, 1.0f },
    Vector4f { 1.0f, 0.5f, 0.0f, 1.0f },
};

const std::vector<float> borderWidths = { 5.0f, 10.0f, 8.0f, 12.0f };

const std::vector<float> lgColorAngles = { 0.0f, 45.0f, 90.0f, 180.0f };
const std::vector<float> lgColorWidths = { 5.0f, 8.0f, 6.0f, 10.0f };

struct LGColorData {
    int colorNumber;
    Vector4f colors[5];
    float positions[5];
};

const std::vector<LGColorData> lgColorDataSets = {
    { 2, { Vector4f { 1.0f, 0.0f, 0.0f, 1.0f }, Vector4f { 0.0f, 0.0f, 1.0f, 1.0f }, {}, {}, {} },
        { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f } },
    { 3,
        { Vector4f { 1.0f, 0.0f, 0.0f, 1.0f }, Vector4f { 0.0f, 1.0f, 0.0f, 1.0f }, Vector4f { 0.0f, 0.0f, 1.0f, 1.0f },
            {}, {} },
        { 0.0f, 0.5f, 1.0f, 0.0f, 0.0f } },
    { 5,
        { Vector4f { 1, 0, 0, 1 }, Vector4f { 1, 1, 0, 1 }, Vector4f { 0, 1, 0, 1 }, Vector4f { 0, 0, 1, 1 },
            Vector4f { 1, 0, 1, 1 } },
        { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f } },
    { 2, { Vector4f { 1.0f, 0.5f, 0.0f, 1.0f }, Vector4f { 0.5f, 0.0f, 1.0f, 1.0f }, {}, {}, {} },
        { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f } },
};
} // namespace

class NGShaderBorderSDFTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNodeWithBorderSDF(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGShaderBase> shader, bool isOutline)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { x, y, sizeX, sizeY });
        auto shape = std::make_shared<RSNGSDFRRectShape>();
        RectF rect(0.0f, 0.0f, static_cast<float>(sizeX), static_cast<float>(sizeY));
        RRect rrect(rect, SHAPE_CORNER_RADIUS, SHAPE_CORNER_RADIUS);
        shape->Setter<SDFRRectShapeRRectTag>(rrect);
        testNode->SetSDFShape(shape);
        if (isOutline) {
            testNode->SetOutlineSDFShader(shader);
        } else {
            testNode->SetBorderSDFShader(shader);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderBorderSDFTest, EFFECT_TEST, Set_SDF_Border_Solid_Border_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderColors.size(); i++) {
        auto shader = std::make_shared<RSNGBorderSDFShader>();
        InitBorderSDFShader(shader, borderColors[i], borderWidths[i], false);
        SetUpTestNodeWithBorderSDF(i, columnCount, rowCount, shader, false);
    }
}

GRAPHIC_TEST(NGShaderBorderSDFTest, EFFECT_TEST, Set_SDF_Border_Solid_Outline_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderColors.size(); i++) {
        auto shader = std::make_shared<RSNGBorderSDFShader>();
        InitBorderSDFShader(shader, borderColors[i], borderWidths[i], true);
        SetUpTestNodeWithBorderSDF(i, columnCount, rowCount, shader, true);
    }
}

GRAPHIC_TEST(NGShaderBorderSDFTest, EFFECT_TEST, Set_SDF_Border_LGColor_Border_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lgColorDataSets.size(); i++) {
        auto shader = std::make_shared<RSNGBorderSDFLGColor>();
        InitBorderSDFLGColor(shader, lgColorAngles[i], lgColorWidths[i], false, lgColorDataSets[i].colorNumber,
            lgColorDataSets[i].colors, lgColorDataSets[i].positions);
        SetUpTestNodeWithBorderSDF(i, columnCount, rowCount, shader, false);
    }
}

GRAPHIC_TEST(NGShaderBorderSDFTest, EFFECT_TEST, Set_SDF_Border_LGColor_Outline_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lgColorDataSets.size(); i++) {
        auto shader = std::make_shared<RSNGBorderSDFLGColor>();
        InitBorderSDFLGColor(shader, lgColorAngles[i], lgColorWidths[i], true, lgColorDataSets[i].colorNumber,
            lgColorDataSets[i].colors, lgColorDataSets[i].positions);
        SetUpTestNodeWithBorderSDF(i, columnCount, rowCount, shader, true);
    }
}
} // namespace OHOS::Rosen

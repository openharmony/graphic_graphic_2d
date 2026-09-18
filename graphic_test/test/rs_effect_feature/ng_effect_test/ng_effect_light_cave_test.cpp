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
#include "ui/rs_effect_node.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string FG_TEST_JPG_PATH = "/data/local/tmp/fg_test.jpg";

constexpr size_t SCREEN_WIDTH = 1200;
constexpr size_t SCREEN_HEIGHT = 2000;
constexpr size_t COLUMN_COUNT = 2;
constexpr size_t TOTAL_CELLS = 6;

struct LightCaveEffectParams {
    Vector4f colorA;
    Vector4f colorB;
    Vector4f colorC;
    Vector2f position;
    Vector2f radiusXY;
    float progress;
};

const std::vector<LightCaveEffectParams> normalParams = {
    {
        .colorA = {1.0f, 0.0f, 0.0f, 1.0f},
        .colorB = {0.0f, 1.0f, 0.0f, 1.0f},
        .colorC = {0.0f, 0.0f, 1.0f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.3f, 0.3f},
        .progress = 0.5f
    },
    {
        .colorA = {1.0f, 1.0f, 0.0f, 1.0f},
        .colorB = {1.0f, 0.0f, 1.0f, 1.0f},
        .colorC = {0.0f, 1.0f, 1.0f, 1.0f},
        .position = {0.3f, 0.7f},
        .radiusXY = {0.4f, 0.4f},
        .progress = 0.3f
    },
    {
        .colorA = {0.8f, 0.2f, 0.2f, 1.0f},
        .colorB = {0.2f, 0.8f, 0.2f, 1.0f},
        .colorC = {0.2f, 0.2f, 0.8f, 1.0f},
        .position = {0.2f, 0.2f},
        .radiusXY = {0.25f, 0.25f},
        .progress = 0.6f
    },
    {
        .colorA = {0.9f, 0.3f, 0.3f, 1.0f},
        .colorB = {0.3f, 0.9f, 0.3f, 1.0f},
        .colorC = {0.3f, 0.3f, 0.9f, 1.0f},
        .position = {0.8f, 0.8f},
        .radiusXY = {0.35f, 0.35f},
        .progress = 0.7f
    },
    {
        .colorA = {1.0f, 0.5f, 0.0f, 1.0f},
        .colorB = {0.0f, 1.0f, 0.5f, 1.0f},
        .colorC = {0.5f, 0.0f, 1.0f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.1f, 0.1f},
        .progress = 0.4f
    },
    {
        .colorA = {1.0f, 0.4f, 0.2f, 1.0f},
        .colorB = {0.2f, 1.0f, 0.4f, 1.0f},
        .colorC = {0.4f, 0.2f, 1.0f, 1.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.5f, 0.5f},
        .progress = 0.8f
    }
};

const std::vector<LightCaveEffectParams> boundaryParams = {
    {
        .colorA = {0.0f, 0.0f, 0.0f, 1.0f},
        .colorB = {0.0f, 0.0f, 0.0f, 1.0f},
        .colorC = {0.0f, 0.0f, 0.0f, 1.0f},
        .position = {0.0f, 0.0f},
        .radiusXY = {0.0f, 0.0f},
        .progress = 0.0f
    },
    {
        .colorA = {1.0f, 1.0f, 1.0f, 1.0f},
        .colorB = {1.0f, 1.0f, 1.0f, 1.0f},
        .colorC = {1.0f, 1.0f, 1.0f, 1.0f},
        .position = {1.0f, 1.0f},
        .radiusXY = {1.0f, 1.0f},
        .progress = 1.0f
    },
    {
        .colorA = {0.5f, 0.5f, 0.5f, 0.5f},
        .colorB = {0.5f, 0.5f, 0.5f, 0.5f},
        .colorC = {0.5f, 0.5f, 0.5f, 0.5f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.5f, 0.5f},
        .progress = 0.5f
    },
    {
        .colorA = {0.0f, 0.5f, 1.0f, 1.0f},
        .colorB = {1.0f, 0.5f, 0.0f, 1.0f},
        .colorC = {0.5f, 1.0f, 0.5f, 1.0f},
        .position = {0.0f, 1.0f},
        .radiusXY = {0.0f, 1.0f},
        .progress = 0.01f
    },
    {
        .colorA = {0.99f, 0.01f, 0.5f, 0.99f},
        .colorB = {0.01f, 0.99f, 0.5f, 0.99f},
        .colorC = {0.5f, 0.5f, 0.99f, 0.99f},
        .position = {0.99f, 0.01f},
        .radiusXY = {0.99f, 0.01f},
        .progress = 0.99f
    },
    {
        .colorA = {0.1f, 0.9f, 0.5f, 0.1f},
        .colorB = {0.9f, 0.1f, 0.5f, 0.1f},
        .colorC = {0.5f, 0.5f, 0.1f, 0.1f},
        .position = {0.1f, 0.9f},
        .radiusXY = {0.1f, 0.9f},
        .progress = 0.1f
    }
};

const std::vector<LightCaveEffectParams> abnormalParams = {
    {
        .colorA = {-0.5f, -0.5f, -0.5f, 1.0f},
        .colorB = {-0.5f, -0.5f, -0.5f, 1.0f},
        .colorC = {-0.5f, -0.5f, -0.5f, 1.0f},
        .position = {-0.5f, -0.5f},
        .radiusXY = {-0.5f, -0.5f},
        .progress = -0.5f
    },
    {
        .colorA = {2.0f, 2.0f, 2.0f, 1.0f},
        .colorB = {2.0f, 2.0f, 2.0f, 1.0f},
        .colorC = {2.0f, 2.0f, 2.0f, 1.0f},
        .position = {2.0f, 2.0f},
        .radiusXY = {2.0f, 2.0f},
        .progress = 2.0f
    },
    {
        .colorA = {999.0f, 999.0f, 999.0f, 999.0f},
        .colorB = {999.0f, 999.0f, 999.0f, 999.0f},
        .colorC = {999.0f, 999.0f, 999.0f, 999.0f},
        .position = {999.0f, 999.0f},
        .radiusXY = {999.0f, 999.0f},
        .progress = 999.0f
    },
    {
        .colorA = {-999.0f, -999.0f, -999.0f, -999.0f},
        .colorB = {-999.0f, -999.0f, -999.0f, -999.0f},
        .colorC = {-999.0f, -999.0f, -999.0f, -999.0f},
        .position = {-999.0f, -999.0f},
        .radiusXY = {-999.0f, -999.0f},
        .progress = -999.0f
    },
    {
        .colorA = {0.0f, 0.0f, 0.0f, 0.0f},
        .colorB = {0.0f, 0.0f, 0.0f, 0.0f},
        .colorC = {0.0f, 0.0f, 0.0f, 0.0f},
        .position = {0.0f, 0.0f},
        .radiusXY = {0.0f, 0.0f},
        .progress = 0.0f
    },
    {
        .colorA = {1.0f, 1.0f, 1.0f, 0.0f},
        .colorB = {1.0f, 1.0f, 1.0f, 0.0f},
        .colorC = {1.0f, 1.0f, 1.0f, 0.0f},
        .position = {0.5f, 0.5f},
        .radiusXY = {0.5f, 0.5f},
        .progress = 1.0f
    }
};
}

class NGEffectLightCaveTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void SetLightCaveParams(const std::shared_ptr<RSNGLightCave>& shader, const LightCaveEffectParams& params)
    {
        if (!shader) {
            return;
        }
        shader->Setter<LightCaveColorATag>(params.colorA);
        shader->Setter<LightCaveColorBTag>(params.colorB);
        shader->Setter<LightCaveColorCTag>(params.colorC);
        shader->Setter<LightCavePositionTag>(params.position);
        shader->Setter<LightCaveRadiusXYTag>(params.radiusXY);
        shader->Setter<LightCaveProgressTag>(params.progress);
    }
};

GRAPHIC_TEST(NGEffectLightCaveTest, EFFECT_TEST, Set_Normal_Background_Test)
{
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / TOTAL_CELLS;

    for (size_t i = 0; i < TOTAL_CELLS; ++i) {
        auto shader = std::make_shared<RSNGLightCave>();
        SetLightCaveParams(shader, normalParams[i]);

        size_t x = (i % COLUMN_COUNT) * sizeX;
        size_t y = (i / COLUMN_COUNT) * sizeY;

        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectLightCaveTest, EFFECT_TEST, Set_Boundary_Background_Test)
{
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / TOTAL_CELLS;

    for (size_t i = 0; i < TOTAL_CELLS; ++i) {
        auto shader = std::make_shared<RSNGLightCave>();
        SetLightCaveParams(shader, boundaryParams[i]);

        size_t x = (i % COLUMN_COUNT) * sizeX;
        size_t y = (i / COLUMN_COUNT) * sizeY;

        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectLightCaveTest, EFFECT_TEST, Set_Abnormal_Background_Test)
{
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / TOTAL_CELLS;

    for (size_t i = 0; i < TOTAL_CELLS; ++i) {
        auto shader = std::make_shared<RSNGLightCave>();
        SetLightCaveParams(shader, abnormalParams[i]);

        size_t x = (i % COLUMN_COUNT) * sizeX;
        size_t y = (i / COLUMN_COUNT) * sizeY;

        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectLightCaveTest, EFFECT_TEST, Set_Normal_Foreground_Test)
{
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / TOTAL_CELLS;

    for (size_t i = 0; i < TOTAL_CELLS; ++i) {
        auto shader = std::make_shared<RSNGLightCave>();
        SetLightCaveParams(shader, normalParams[i]);

        size_t x = (i % COLUMN_COUNT) * sizeX;
        size_t y = (i / COLUMN_COUNT) * sizeY;

        auto backgroundNode = SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGEffectLightCaveTest, EFFECT_TEST, Set_Boundary_Foreground_Test)
{
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / TOTAL_CELLS;

    for (size_t i = 0; i < TOTAL_CELLS; ++i) {
        auto shader = std::make_shared<RSNGLightCave>();
        SetLightCaveParams(shader, boundaryParams[i]);

        size_t x = (i % COLUMN_COUNT) * sizeX;
        size_t y = (i / COLUMN_COUNT) * sizeY;

        auto backgroundNode = SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGEffectLightCaveTest, EFFECT_TEST, Set_Abnormal_Foreground_Test)
{
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / TOTAL_CELLS;

    for (size_t i = 0; i < TOTAL_CELLS; ++i) {
        auto shader = std::make_shared<RSNGLightCave>();
        SetLightCaveParams(shader, abnormalParams[i]);

        size_t x = (i % COLUMN_COUNT) * sizeX;
        size_t y = (i / COLUMN_COUNT) * sizeY;

        auto backgroundNode = SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}
}  // namespace OHOS::Rosen

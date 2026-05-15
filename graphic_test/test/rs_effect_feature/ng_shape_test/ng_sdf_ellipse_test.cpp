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

#include <string>
#include <vector>

#include "ng_sdf_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"

#include "effect/rs_render_shape_base.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int COLUMN_COUNT = 2;
constexpr int PROPERTY_ROW_COUNT = 2;
constexpr float DEFAULT_CENTER_X = 300.0f;
constexpr float DEFAULT_CENTER_Y = 260.0f;
constexpr float ZERO_SIZE = 0.0f;
constexpr float MIN_SIZE = 1.0f;
constexpr float THIN_SIZE = 12.0f;
constexpr float SMALL_SIZE = 48.0f;
constexpr float NORMAL_SIZE = 240.0f;
constexpr float LARGE_SIZE = 900.0f;
constexpr float OFFSCREEN_LEFT_X = -120.0f;
constexpr float OFFSCREEN_TOP_Y = -80.0f;
constexpr float OFFSCREEN_RIGHT_X = 720.0f;
constexpr float OFFSCREEN_BOTTOM_Y = 560.0f;
constexpr float NEGATIVE_WIDTH = -240.0f;
constexpr float NEGATIVE_HEIGHT = -180.0f;
const std::string BACKGROUND_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";

struct EllipseParam {
    Vector2f center;
    float width;
    float height;
};

const std::vector<EllipseParam> ellipseParams = {
    { Vector2f(300.0f, 260.0f), 320.0f, 320.0f },
    { Vector2f(300.0f, 260.0f), 460.0f, 180.0f },
    { Vector2f(300.0f, 260.0f), 180.0f, 460.0f },
    { Vector2f(250.0f, 220.0f), 300.0f, 140.0f },
    { Vector2f(350.0f, 300.0f), 160.0f, 360.0f },
    { Vector2f(300.0f, 260.0f), 520.0f, 80.0f },
};

const std::vector<EllipseParam> edgeCaseEllipseParams = {
    { Vector2f(DEFAULT_CENTER_X, DEFAULT_CENTER_Y), ZERO_SIZE, NORMAL_SIZE },
    { Vector2f(DEFAULT_CENTER_X, DEFAULT_CENTER_Y), NORMAL_SIZE, ZERO_SIZE },
    { Vector2f(DEFAULT_CENTER_X, DEFAULT_CENTER_Y), MIN_SIZE, MIN_SIZE },
    { Vector2f(DEFAULT_CENTER_X, DEFAULT_CENTER_Y), LARGE_SIZE, LARGE_SIZE },
    { Vector2f(OFFSCREEN_LEFT_X, DEFAULT_CENTER_Y), NORMAL_SIZE, SMALL_SIZE },
    { Vector2f(DEFAULT_CENTER_X, OFFSCREEN_TOP_Y), SMALL_SIZE, NORMAL_SIZE },
    { Vector2f(OFFSCREEN_RIGHT_X, OFFSCREEN_BOTTOM_Y), NORMAL_SIZE, NORMAL_SIZE },
    { Vector2f(DEFAULT_CENTER_X, DEFAULT_CENTER_Y), NEGATIVE_WIDTH, NEGATIVE_HEIGHT },
    { Vector2f(DEFAULT_CENTER_X, DEFAULT_CENTER_Y), LARGE_SIZE, THIN_SIZE },
    { Vector2f(DEFAULT_CENTER_X, DEFAULT_CENTER_Y), THIN_SIZE, LARGE_SIZE },
};
} // namespace

class NGSDFEllipseTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth_, screenHeight_);
    }

private:
    const int screenWidth_ = SCREEN_WIDTH;
    const int screenHeight_ = SCREEN_HEIGHT;
};

static void InitFrostedGlassFilter(std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
{
    if (frostedGlassFilter == nullptr) {
        frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    }

    frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(thickBlurParams);
    frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(thickWeightsEmboss);
    frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(thickWeightsEdl);
    frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(thickBgRates);
    frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(thickBgKBS);
    frostedGlassFilter->Setter<FrostedGlassBgPosTag>(thickBgPos);
    frostedGlassFilter->Setter<FrostedGlassBgNegTag>(thickBgNeg);
    frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(thickRefractParams);
    frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(thickSdParams);
    frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(thickSdRates);
    frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(thickSdKBS);
    frostedGlassFilter->Setter<FrostedGlassSdPosTag>(thickSdPos);
    frostedGlassFilter->Setter<FrostedGlassSdNegTag>(thickSdNeg);
    frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(thickEnvLightParams);
    frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(thickEnvLightRates);
    frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(thickEnvLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(thickEnvLightPos);
    frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(thickEnvLightNeg);
    frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(thickEdLightParams);
    frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(thickEdLightAngles);
    frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(thickEdLightDir);
    frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(thickEdLightRates);
    frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(thickEdLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(thickEdLightPos);
    frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(thickEdLightNeg);
}

static std::shared_ptr<RSNGShapeBase> CreateEllipseShape(const EllipseParam& param)
{
    auto childShape = CreateShape(RSNGEffectType::SDF_ELLIPSE_SHAPE);
    auto ellipseShape = std::static_pointer_cast<RSNGSDFEllipseShape>(childShape);
    ellipseShape->Setter<SDFEllipseShapeCenterTag>(param.center);
    ellipseShape->Setter<SDFEllipseShapeWidthTag>(param.width);
    ellipseShape->Setter<SDFEllipseShapeHeightTag>(param.height);
    return childShape;
}

static void SetUpSDFEllipseNode(
    const std::shared_ptr<RSCanvasNode>& node, const EllipseParam& param, int sizeX, int sizeY)
{
    auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    InitFrostedGlassFilter(frostedGlassFilter);
    Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
    node->SetBounds(bounds);
    node->SetFrame(bounds);
    node->SetMaterialNGFilter(frostedGlassFilter);
    node->SetSDFShape(CreateEllipseShape(param));
}

GRAPHIC_TEST(NGSDFEllipseTest, EFFECT_TEST, Set_SDF_EllipseShape_Test_1)
{
    int rowCount = static_cast<int>(ellipseParams.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;
        auto backgroundTestNode =
            RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().rsUiDirector_->GetRSUIContext());
        SetUpSDFEllipseNode(backgroundTestNode, ellipseParams[i], sizeX, sizeY);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFEllipseTest, EFFECT_TEST, Set_SDF_EllipseShape_Properties_Test_1)
{
    int rowCount = PROPERTY_ROW_COUNT;
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    Color color(0, 0, 0);
    Vector4<Color> outlineColor = { color, color, color, color };
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;
        auto backgroundTestNode =
            RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().rsUiDirector_->GetRSUIContext());
        SetUpSDFEllipseNode(backgroundTestNode, ellipseParams[i + 1], sizeX, sizeY);
        backgroundTestNode->SetBorderStyle(0, 0, 0, 0);
        backgroundTestNode->SetBorderWidth(5, 5, 5, 5);
        backgroundTestNode->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        backgroundTestNode->SetOutlineWidth(Vector4f(8.0f, 8.0f, 8.0f, 8.0f));
        backgroundTestNode->SetOutlineColor(outlineColor);
        backgroundTestNode->SetShadowRadius(25.0f);
        backgroundTestNode->SetShadowColor(0xFF00FF00);
        backgroundTestNode->SetClipToBounds(i == 1);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFEllipseTest, EFFECT_TEST, Set_SDF_EllipseShape_EdgeCase_Test_1)
{
    int rowCount = static_cast<int>(edgeCaseEllipseParams.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;
        auto backgroundTestNode =
            RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().rsUiDirector_->GetRSUIContext());
        SetUpSDFEllipseNode(backgroundTestNode, edgeCaseEllipseParams[i], sizeX, sizeY);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}
} // namespace OHOS::Rosen

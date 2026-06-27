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

#include <array>
#include <string>
#include <vector>

#include "effect/rs_render_shape_base.h"
#include "ng_sdf_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int COLUMN_COUNT = 2;
constexpr int CORNER_COUNT = 4;
constexpr int PROPERTY_ROW_COUNT = 2;
const std::string BACKGROUND_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";

struct RRectCornerParam {
    RectT<float> rect;
    std::array<Vector2f, CORNER_COUNT> radius;
};

const std::vector<RRectCornerParam> rRectCornerParams = {
    {
        RectT<float>{90.0f, 90.0f, 340.0f, 240.0f},
        {Vector2f(48.0f, 48.0f), Vector2f(48.0f, 48.0f),
            Vector2f(48.0f, 48.0f), Vector2f(48.0f, 48.0f)},
    },
    {
        RectT<float>{90.0f, 90.0f, 340.0f, 240.0f},
        {Vector2f(0.0f, 0.0f), Vector2f(30.0f, 30.0f),
            Vector2f(0.0f, 0.0f), Vector2f(30.0f, 30.0f)},
    },
    {
        RectT<float>{80.0f, 80.0f, 360.0f, 260.0f},
        {Vector2f(18.0f, 42.0f), Vector2f(68.0f, 22.0f),
            Vector2f(96.0f, 54.0f), Vector2f(34.0f, 88.0f)},
    },
    {
        RectT<float>{120.0f, 70.0f, 320.0f, 360.0f},
        {Vector2f(72.0f, 26.0f), Vector2f(28.0f, 92.0f),
            Vector2f(118.0f, 48.0f), Vector2f(46.0f, 136.0f)},
    },
    {
        RectT<float>{70.0f, 130.0f, 430.0f, 210.0f},
        {Vector2f(12.0f, 96.0f), Vector2f(140.0f, 36.0f),
            Vector2f(54.0f, 128.0f), Vector2f(104.0f, 24.0f)},
    },
    {
        RectT<float>{150.0f, 90.0f, 260.0f, 420.0f},
        {Vector2f(108.0f, 32.0f), Vector2f(44.0f, 156.0f),
            Vector2f(86.0f, 64.0f), Vector2f(20.0f, 118.0f)},
    },
    {
        RectT<float>{160.0f, 130.0f, 120.0f, 260.0f},
        {Vector2f(100.0f, 40.0f), Vector2f(100.0f, 30.0f),
            Vector2f(80.0f, 50.0f), Vector2f(80.0f, 60.0f)},
    },
    {
        RectT<float>{140.0f, 140.0f, 260.0f, 120.0f},
        {Vector2f(40.0f, 90.0f), Vector2f(30.0f, 70.0f),
            Vector2f(50.0f, 70.0f), Vector2f(60.0f, 90.0f)},
    },
    {
        RectT<float>{110.0f, 110.0f, 220.0f, 220.0f},
        {Vector2f(180.0f, 180.0f), Vector2f(160.0f, 160.0f),
            Vector2f(140.0f, 140.0f), Vector2f(120.0f, 120.0f)},
    },
    {
        RectT<float>{220.0f, 220.0f, 0.25f, 0.25f},
        {Vector2f(24.0f, 24.0f), Vector2f(18.0f, 18.0f),
            Vector2f(12.0f, 12.0f), Vector2f(6.0f, 6.0f)},
    },
};
}

class NGSDFRRectTest : public RSGraphicTest {
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

static void InitFrostedGlassFilterDefault(std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
{
    if (frostedGlassFilter == nullptr) {
        frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    }

    frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
    frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(defaultWeightsEmboss);
    frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(defaultWeightsEdl);
    frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(defaultBgRates);
    frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(defaultBgKBS);
    frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
    frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
    frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);
    frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
    frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
    frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
    frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
    frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);
    frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
    frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
    frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
    frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
    frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
    frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
    frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
    frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
    frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
    frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);
}

static RRect MakeRRectWithCornerRadii(const RRectCornerParam& param)
{
    return RRect(param.rect, param.radius.data());
}

static std::shared_ptr<RSNGShapeBase> CreateRRectShapeWithCornerRadii(const RRectCornerParam& param)
{
    auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
    rRectChildShape->Setter<SDFRRectShapeRRectTag>(MakeRRectWithCornerRadii(param));
    return childShape;
}

static void SetUpSDFRRectNode(const std::shared_ptr<RSCanvasNode>& node, const RRectCornerParam& param,
    int sizeX, int sizeY)
{
    auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    InitFrostedGlassFilter(frostedGlassFilter);
    Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
    node->SetBounds(bounds);
    node->SetFrame(bounds);
    node->SetMaterialNGFilter(frostedGlassFilter);
    node->SetSDFShape(CreateRRectShapeWithCornerRadii(param));
}

GRAPHIC_TEST(NGSDFRRectTest, EFFECT_TEST, Set_SDF_RRectShape_NonUniformRadius_Test_1)
{
    int rowCount = static_cast<int>(rRectCornerParams.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        SetUpSDFRRectNode(backgroundTestNode, rRectCornerParams[i], sizeX, sizeY);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFRRectTest, EFFECT_TEST, Set_SDF_RRectShape_NonUniformRadius_Properties_Test_1)
{
    int rowCount = PROPERTY_ROW_COUNT;
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    Color color(0, 0, 0);
    Vector4<Color> outlineColor = {color, color, color, color};
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        SetUpSDFRRectNode(backgroundTestNode, rRectCornerParams[i + 1], sizeX, sizeY);
        backgroundTestNode->SetBorderStyle(0, 0, 0, 0);
        backgroundTestNode->SetBorderWidth(5, 5, 5, 5);
        backgroundTestNode->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        backgroundTestNode->SetOutlineWidth(Vector4f(8.0f, 8.0f, 8.0f, 8.0f));
        backgroundTestNode->SetOutlineColor(outlineColor);
        backgroundTestNode->SetShadowRadius(25.0f);
        backgroundTestNode->SetShadowColor(0xFF00FF00);
        backgroundTestNode->SetClipToBounds(i == 1);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFRRectTest, EFFECT_TEST, FrostedGlass_Default_RRectShape_ClipTest)
{
    int rowCount = PROPERTY_ROW_COUNT;
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilterDefault(frostedGlassFilter);
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        if (i == 0) {
            bounds = {50, 50, 362, 800};
        } else {
            bounds = {50, 50, 462, 800};
        }
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        backgroundTestNode->SetCornerRadius(112.0f);
        backgroundTestNode->SetShadowRadius(10.0f);
        backgroundTestNode->SetClipToBounds(true);

        auto colorNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f colorBounds{0, 0, sizeX, sizeY};
        colorNode->SetBounds(colorBounds);
        colorNode->SetFrame(colorBounds);
        colorNode->SetBackgroundColor(0xFFF2A93B);
        backgroundTestNode->AddChild(colorNode);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        RegisterNode(colorNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFRRectTest, EFFECT_TEST, FrostedGlass_Default_Capsule_ClipTest)
{
    constexpr float capsuleRadius = 112.0f;
    const std::array<Rosen::Vector4f, 4> capsuleBounds = {
        Rosen::Vector4f{50, 50, 300, 200},
        Rosen::Vector4f{50, 50, 500, 200},
        Rosen::Vector4f{50, 50, 200, 300},
        Rosen::Vector4f{50, 50, 224, 225},
    };
    int rowCount = static_cast<int>(capsuleBounds.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilterDefault(frostedGlassFilter);
        backgroundTestNode->SetBounds(capsuleBounds[i]);
        backgroundTestNode->SetFrame(capsuleBounds[i]);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        backgroundTestNode->SetCornerRadius(capsuleRadius);
        backgroundTestNode->SetShadowRadius(10.0f);
        backgroundTestNode->SetClipToBounds(true);

        auto colorNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f colorBounds{0, 0, sizeX, sizeY};
        colorNode->SetBounds(colorBounds);
        colorNode->SetFrame(colorBounds);
        colorNode->SetBackgroundColor(0xFFF2A93B);
        backgroundTestNode->AddChild(colorNode);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        RegisterNode(colorNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}
} // namespace OHOS::Rosen

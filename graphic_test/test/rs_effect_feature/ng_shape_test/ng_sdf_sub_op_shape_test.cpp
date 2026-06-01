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
constexpr int32_t SCREEN_WIDTH = 1200;
constexpr int32_t SCREEN_HEIGHT = 2000;
constexpr int32_t COLUMN_COUNT = 2;
constexpr float SMOOTH_SPACING_SMALL = 1.0f;
constexpr float SMOOTH_SPACING_MEDIUM = 24.0f;
constexpr float SMOOTH_SPACING_LARGE = 120.0f;
const std::string BACKGROUND_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";

struct RRectPairParam {
    RRect left;
    RRect right;
    float spacing;
};

struct MixedShapeParam {
    bool leftEllipse;
    Vector2f ellipseCenter;
    float ellipseWidth;
    float ellipseHeight;
    RRect rrect;
    float spacing;
};

const std::vector<RRectPairParam> rrectSubParams = {
    { RRect { RectT<float> { 70, 80, 360, 260 }, 40.0f, 40.0f },
        RRect { RectT<float> { 230, 130, 220, 180 }, 30.0f, 30.0f }, SMOOTH_SPACING_SMALL },
    { RRect { RectT<float> { 90, 80, 420, 280 }, 50.0f, 50.0f },
        RRect { RectT<float> { 190, 120, 180, 160 }, 30.0f, 30.0f }, SMOOTH_SPACING_MEDIUM },
    { RRect { RectT<float> { 90, 90, 300, 240 }, 40.0f, 40.0f },
        RRect { RectT<float> { 270, 170, 260, 180 }, 40.0f, 40.0f }, SMOOTH_SPACING_LARGE },
};

const std::vector<MixedShapeParam> mixedSubParams = {
    { true, Vector2f(300.0f, 210.0f), 390.0f, 250.0f, RRect { RectT<float> { 250, 120, 210, 190 }, 30.0f, 30.0f },
        SMOOTH_SPACING_MEDIUM },
    { false, Vector2f(330.0f, 210.0f), 260.0f, 220.0f, RRect { RectT<float> { 95, 80, 400, 270 }, 45.0f, 45.0f },
        SMOOTH_SPACING_MEDIUM },
};

const std::vector<RRectPairParam> edgeSubParams = {
    { RRect { RectT<float> { 90, 90, 350, 260 }, 45.0f, 45.0f },
        RRect { RectT<float> { 20, 20, 560, 360 }, 60.0f, 60.0f }, SMOOTH_SPACING_SMALL },
    { RRect { RectT<float> { 80, 90, 210, 170 }, 25.0f, 25.0f },
        RRect { RectT<float> { 380, 110, 170, 150 }, 25.0f, 25.0f }, SMOOTH_SPACING_MEDIUM },
};

const std::vector<RRectPairParam> smoothSpacingEdgeParams = {
    { RRect { RectT<float> { 80, 90, 360, 260 }, 40.0f, 40.0f },
        RRect { RectT<float> { 220, 120, 180, 160 }, 30.0f, 30.0f }, 0.0f },
    { RRect { RectT<float> { 90, 90, 350, 260 }, 45.0f, 45.0f },
        RRect { RectT<float> { 210, 120, 180, 160 }, 30.0f, 30.0f }, -30.0f },
};
} // namespace

class NGSDFSubOpShapeTest : public RSGraphicTest {
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

static std::shared_ptr<RSNGShapeBase> CreateRRectShape(const RRect& rrect)
{
    auto shape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    auto rrectShape = std::static_pointer_cast<RSNGSDFRRectShape>(shape);
    rrectShape->Setter<SDFRRectShapeRRectTag>(rrect);
    return shape;
}

static std::shared_ptr<RSNGShapeBase> CreateEllipseShape(const Vector2f& center, float width, float height)
{
    auto shape = CreateShape(RSNGEffectType::SDF_ELLIPSE_SHAPE);
    auto ellipseShape = std::static_pointer_cast<RSNGSDFEllipseShape>(shape);
    ellipseShape->Setter<SDFEllipseShapeCenterTag>(center);
    ellipseShape->Setter<SDFEllipseShapeWidthTag>(width);
    ellipseShape->Setter<SDFEllipseShapeHeightTag>(height);
    return shape;
}

static std::shared_ptr<RSNGShapeBase> CreateTriangleShape()
{
    auto shape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
    auto triangleShape = std::static_pointer_cast<RSNGSDFTriangleShape>(shape);
    triangleShape->Setter<SDFTriangleShapeVertex0Tag>(Vector2f(300.0f, 70.0f));
    triangleShape->Setter<SDFTriangleShapeVertex1Tag>(Vector2f(460.0f, 330.0f));
    triangleShape->Setter<SDFTriangleShapeVertex2Tag>(Vector2f(140.0f, 330.0f));
    triangleShape->Setter<SDFTriangleShapeRadiusTag>(24.0f);
    return shape;
}

static std::shared_ptr<RSNGShapeBase> CreateSubOpShape(
    std::shared_ptr<RSNGShapeBase> left, std::shared_ptr<RSNGShapeBase> right)
{
    auto shape = CreateShape(RSNGEffectType::SDF_SUB_OP_SHAPE);
    auto subOpShape = std::static_pointer_cast<RSNGSDFSubOpShape>(shape);
    subOpShape->Setter<SDFSubOpShapeShapeXTag>(left);
    subOpShape->Setter<SDFSubOpShapeShapeYTag>(right);
    return shape;
}

static std::shared_ptr<RSNGShapeBase> CreateSmoothSubOpShape(
    std::shared_ptr<RSNGShapeBase> left, std::shared_ptr<RSNGShapeBase> right, float spacing)
{
    auto shape = CreateShape(RSNGEffectType::SDF_SMOOTH_SUB_OP_SHAPE);
    auto smoothSubOpShape = std::static_pointer_cast<RSNGSDFSmoothSubOpShape>(shape);
    smoothSubOpShape->Setter<SDFSmoothSubOpShapeShapeXTag>(left);
    smoothSubOpShape->Setter<SDFSmoothSubOpShapeShapeYTag>(right);
    smoothSubOpShape->Setter<SDFSmoothSubOpShapeSpacingTag>(spacing);
    return shape;
}

static void SetUpSDFSubOpNode(
    const std::shared_ptr<RSCanvasNode>& node, const std::shared_ptr<RSNGShapeBase>& shape, int sizeX, int sizeY)
{
    auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    InitFrostedGlassFilter(frostedGlassFilter);
    Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
    node->SetBounds(bounds);
    node->SetFrame(bounds);
    node->SetMaterialNGFilter(frostedGlassFilter);
    node->SetSDFShape(shape);
}

static void AddCaseNode(RSGraphicTest* test, const std::shared_ptr<RSNGShapeBase>& shape, int index, int rowCount)
{
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    if (rowCount == 0) {
        return;
    }
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    int x = (index % COLUMN_COUNT) * sizeX;
    int y = (index / COLUMN_COUNT) * sizeY;
    auto backgroundTestNode =
        RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().rsUiDirector_->GetRSUIContext());
    SetUpSDFSubOpNode(backgroundTestNode, shape, sizeX, sizeY);

    auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
    childNode->AddChild(backgroundTestNode);
    test->RegisterNode(backgroundTestNode);
    test->GetRootNode()->AddChild(childNode);
    test->RegisterNode(childNode);
}

GRAPHIC_TEST(NGSDFSubOpShapeTest, EFFECT_TEST, Set_SDF_SubOpShape_Normal_Test_1)
{
    int index = 0;
    int rowCount = static_cast<int>(rrectSubParams.size() + mixedSubParams.size());
    for (const auto& param : rrectSubParams) {
        auto left = CreateRRectShape(param.left);
        auto right = CreateRRectShape(param.right);
        AddCaseNode(this, CreateSubOpShape(left, right), index++, rowCount);
    }

    for (const auto& param : mixedSubParams) {
        auto ellipse = CreateEllipseShape(param.ellipseCenter, param.ellipseWidth, param.ellipseHeight);
        auto rrect = CreateRRectShape(param.rrect);
        AddCaseNode(this, param.leftEllipse ? CreateSubOpShape(ellipse, rrect) : CreateSubOpShape(rrect, ellipse),
            index++, rowCount);
    }
}

GRAPHIC_TEST(NGSDFSubOpShapeTest, EFFECT_TEST, Set_SDF_SmoothSubOpShape_Normal_Test_1)
{
    int index = 0;
    int rowCount = static_cast<int>(rrectSubParams.size() + 1);
    for (const auto& param : rrectSubParams) {
        auto left = CreateRRectShape(param.left);
        auto right = CreateRRectShape(param.right);
        AddCaseNode(this, CreateSmoothSubOpShape(left, right, param.spacing), index++, rowCount);
    }

    auto baseSub = CreateSubOpShape(CreateRRectShape(RRect { RectT<float> { 80, 80, 410, 280 }, 45.0f, 45.0f }),
        CreateEllipseShape(Vector2f(320.0f, 210.0f), 210.0f, 180.0f));
    AddCaseNode(this, CreateSmoothSubOpShape(baseSub, CreateTriangleShape(), SMOOTH_SPACING_MEDIUM), index++, rowCount);
}

GRAPHIC_TEST(NGSDFSubOpShapeTest, EFFECT_TEST, Set_SDF_SubOpShape_EdgeCase_Test_1)
{
    int index = 0;
    int rowCount = static_cast<int>(edgeSubParams.size() + smoothSpacingEdgeParams.size() + 6);
    for (const auto& param : edgeSubParams) {
        auto left = CreateRRectShape(param.left);
        auto right = CreateRRectShape(param.right);
        AddCaseNode(this, CreateSubOpShape(left, right), index++, rowCount);
    }

    for (const auto& param : smoothSpacingEdgeParams) {
        auto left = CreateRRectShape(param.left);
        auto right = CreateRRectShape(param.right);
        AddCaseNode(this, CreateSmoothSubOpShape(left, right, param.spacing), index++, rowCount);
    }

    auto validShape = CreateRRectShape(RRect { RectT<float> { 100, 90, 360, 250 }, 40.0f, 40.0f });
    AddCaseNode(this, CreateSubOpShape(nullptr, validShape), index++, rowCount);
    AddCaseNode(this, CreateSubOpShape(validShape, nullptr), index++, rowCount);
    AddCaseNode(this, CreateSubOpShape(nullptr, nullptr), index++, rowCount);
    AddCaseNode(this, CreateSmoothSubOpShape(nullptr, validShape, SMOOTH_SPACING_MEDIUM), index++, rowCount);
    AddCaseNode(this, CreateSmoothSubOpShape(validShape, nullptr, SMOOTH_SPACING_MEDIUM), index++, rowCount);
    AddCaseNode(this, CreateSmoothSubOpShape(nullptr, nullptr, SMOOTH_SPACING_MEDIUM), index++, rowCount);
}
} // namespace OHOS::Rosen

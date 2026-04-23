/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "effect/rs_render_shape_base.h"
#include "ng_sdf_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui/rs_union_node.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class NGSDFShapeTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    const int filterParaTypeCount = static_cast<int>(FilterPara::ParaType::CONTENT_LIGHT);
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

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_TriangleShape_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(triangleParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto childShape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape);
        auto [v0, v1, v2] = triangleParams[i];
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(0.0f);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_RoundedTriangleShape_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(triangleRadiusParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    Vector2f v0(100, 100);
    Vector2f v1(300, 100);
    Vector2f v2(200, 300);
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto childShape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape);
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(triangleRadiusParams[i]);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_UnionOp_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(triangleParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

        auto childShapeX = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShapeX = std::static_pointer_cast<RSNGSDFTriangleShape>(childShapeX);
        auto [v0, v1, v2] = triangleParams[i];
        triangleChildShapeX->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShapeX->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShapeX->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShapeX->Setter<SDFTriangleShapeRadiusTag>(0.0f);
        unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

        auto childShapeY = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShapeY = std::static_pointer_cast<RSNGSDFTriangleShape>(childShapeY);
        Vector2f v3(200, 150);
        Vector2f v4(400, 150);
        Vector2f v5(300, 350);
        triangleChildShapeY->Setter<SDFTriangleShapeVertex0Tag>(v3);
        triangleChildShapeY->Setter<SDFTriangleShapeVertex1Tag>(v4);
        triangleChildShapeY->Setter<SDFTriangleShapeVertex2Tag>(v5);
        triangleChildShapeY->Setter<SDFTriangleShapeRadiusTag>(0.0f);
        unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);

        backgroundTestNode->SetSDFShape(unionShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_SmoothUnion_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(triangleParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        std::shared_ptr<RSNGShapeBase> smoothUnionShape;
        auto [v0, v1, v2] = triangleParams[i];
        Vector2f v3(200, 150);
        Vector2f v4(400, 150);
        Vector2f v5(300, 350);
        InitSmoothUnionShapesByTriangle(smoothUnionShape, v0, v1, v2, 0.0f, v3, v4, v5, 0.0f, 30.0f);
        backgroundTestNode->SetSDFShape(smoothUnionShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_Transform_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(matrix3fParams1.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    Vector2f v0(100, 100);
    Vector2f v1(300, 100);
    Vector2f v2(200, 300);
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto sdfShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto transformShape = std::static_pointer_cast<RSNGSDFTransformShape>(sdfShape);
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams1[i].Inverse());

        auto childShape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape);
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(0.0f);
        transformShape->Setter<SDFTransformShapeShapeTag>(childShape);
        backgroundTestNode->SetSDFShape(sdfShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_Degenerate_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(degenerateTriangleParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto childShape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape);
        auto [v0, v1, v2] = degenerateTriangleParams[i];
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(0.0f);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_NegativeCoords_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(negativeCoordTriangleParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto childShape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape);
        auto [v0, v1, v2] = negativeCoordTriangleParams[i];
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(0.0f);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_ClockwiseCounter_Test)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    Vector2f cw0(100, 100);
    Vector2f cw1(300, 100);
    Vector2f cw2(200, 300);
    Vector2f ccw0(200, 300);
    Vector2f ccw1(300, 100);
    Vector2f ccw2(100, 100);
    std::vector<std::tuple<Vector2f, Vector2f, Vector2f>> triangleVertexOrders = {
        {cw0, cw1, cw2},
        {ccw0, ccw1, ccw2},
    };
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto childShape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape);
        auto [v0, v1, v2] = triangleVertexOrders[i];
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(0.0f);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_ExcessiveRadius_Test)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    Vector2f v0(100, 100);
    Vector2f v1(300, 100);
    Vector2f v2(200, 300);
    std::vector<float> excessiveRadiusParams = {50.0f, 100.0f, 500.0f};
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto childShape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape);
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(excessiveRadiusParams[i]);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_Properties_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(triangleParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto childShape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape);
        auto [v0, v1, v2] = triangleParams[i];
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(triangleRadiusParams[i]);

        backgroundTestNode->SetSDFShape(childShape);

        Color color(0, 0, 0);
        Vector4<Color> outlineColor = {color, color, color, color};
        backgroundTestNode->SetOutlineWidth(outlineWidthParams[i]);
        backgroundTestNode->SetOutlineColor(outlineColor);

        backgroundTestNode->SetShadowRadius(shadowRadiusParams[i]);
        backgroundTestNode->SetClipToBounds(true);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Triangle_RRect_SmoothUnion_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(triangleParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);
        auto childShape1 = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleChildShape = std::static_pointer_cast<RSNGSDFTriangleShape>(childShape1);
        auto [v0, v1, v2] = triangleParams[i];
        triangleChildShape->Setter<SDFTriangleShapeVertex0Tag>(v0);
        triangleChildShape->Setter<SDFTriangleShapeVertex1Tag>(v1);
        triangleChildShape->Setter<SDFTriangleShapeVertex2Tag>(v2);
        triangleChildShape->Setter<SDFTriangleShapeRadiusTag>(triangleRadiusParams[i]);

        auto childShape2 = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape2);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrectParams[i]);

        auto rootShape = CreateShape(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);
        auto sdfUnionRootShape = std::static_pointer_cast<RSNGSDFSmoothUnionOpShape>(rootShape);
        sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeShapeXTag>(childShape1);
        sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeShapeYTag>(childShape2);
        sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeSpacingTag>(30.0f);

        backgroundTestNode->SetSDFShape(rootShape);
        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}
}  // namespace OHOS::Rosen

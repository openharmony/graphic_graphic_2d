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
    // called before each tests
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
    frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(defaultBorderSize);
    frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(DEFAULT_CORNER_RADIUS);
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOpShape_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(rectYParams.size());
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
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 30.0f);
        backgroundTestNode->SetSDFShape(sdfShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOpShape_Test_2)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(rectYParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    uint16_t rectIndex = 1;
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
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[rectIndex], rectYParams[rectIndex], sdfShapeSpacingParams[i]);
        backgroundTestNode->SetSDFShape(sdfShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOpShape_Test_3)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    uint16_t rectIndex = 1;
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
        auto rootShape = CreateShape(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);
        auto sdfUnionRootShape = std::static_pointer_cast<RSNGSDFSmoothUnionOpShape>(rootShape);

        if (i % columnCount > 0) { // add first rrect sdf shape if index is second column
            auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
            rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(rectXParams[rectIndex]);
            sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeShapeXTag>(childShapeX);
        }
        if (i > 1) { // add second rrect sdf shape if index is > 1
            auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
            rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(rectYParams[rectIndex]);
            sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeShapeYTag>(childShapeY);
        }
        sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeSpacingTag>(sdfShapeSpacingParams[rectIndex]);
        backgroundTestNode->SetSDFShape(rootShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_RRectShape_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(rrectParams.size());
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
        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrectParams[i]);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_TransformShape_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(matrix3fParams1.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    RRect rrect{RectT<float>{100, 10, 300, 200}, 20.0f, 20.0f};
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
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams1[i]);

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrect);
        transformShape->Setter<SDFTransformShapeShapeTag>(childShape);
        backgroundTestNode->SetSDFShape(sdfShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_TransformShape_Test_2)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(matrix3fParams2.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    RRect rrect{RectT<float>{50, 50, 100, 400}, 50.0f, 50.0f};
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
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams2[i]);

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrect);
        transformShape->Setter<SDFTransformShapeShapeTag>(childShape);
        backgroundTestNode->SetSDFShape(sdfShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_TransformShape_Test_3)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(matrix3fParams3.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    RRect rrect{RectT<float>{100, 300, 400, 100}, 50.0f, 50.0f};
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
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams3[i]);

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrect);
        transformShape->Setter<SDFTransformShapeShapeTag>(childShape);
        backgroundTestNode->SetSDFShape(sdfShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_TransformShape_Test_4)
{
    int columnCount = 1;
    int rowCount = 1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    RRect rrect{RectT<float>{0.0f, 0.0f, 200.0f, 800.0f}, 100.0f, 100.0f};
    Matrix3f matrix1{1.0f, 0.0f, 100.0f, 0.0f, 1.0f, 400.0f, 0.0f, 0.0f, 1.0f};
    Matrix3f matrix2{1.0f, 0.0f, -700.0f, 0.0f, 1.0f, -800.0f, 0.0f, 0.0f, 1.0f};
    Matrix3f matrix{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0008f, 1.0f};
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

        auto sdfShape1 = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto transformShape1 = std::static_pointer_cast<RSNGSDFTransformShape>(sdfShape1);
        transformShape1->Setter<SDFTransformShapeMatrixTag>(matrix1);
        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrect);
        transformShape1->Setter<SDFTransformShapeShapeTag>(childShape);

        auto sdfShape2 = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto transformShape2 = std::static_pointer_cast<RSNGSDFTransformShape>(sdfShape2);
        transformShape2->Setter<SDFTransformShapeMatrixTag>(matrix);
        transformShape2->Setter<SDFTransformShapeShapeTag>(sdfShape1);
    
        auto sdfShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto transformShape = std::static_pointer_cast<RSNGSDFTransformShape>(sdfShape);
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix2);
        transformShape->Setter<SDFTransformShapeShapeTag>(sdfShape2);

        backgroundTestNode->SetSDFShape(sdfShape);
        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Union_Container_Spacing_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdfShapeSpacingParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        Rosen::Vector4f bounds(x, y, sizeX, sizeY);
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", bounds);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto unionNode = RSUnionNode::Create();
        Rosen::Vector4f unionNodeBounds(0, 0, sizeX, sizeY);
        unionNode->SetBounds(unionNodeBounds);
        unionNode->SetFrame(unionNodeBounds);
        unionNode->SetMaterialNGFilter(frostedGlassFilter);
        unionNode->SetUnionSpacing(sdfShapeSpacingParams[i]);
        backgroundTestNode->AddChild(unionNode);
        RegisterNode(unionNode);

        auto childX = RSCanvasNode::Create();
        auto childXRect = rectXParams[0].rect_;
        Rosen::Vector4f childXRectBounds(childXRect.GetLeft(), childXRect.GetTop(), childXRect.GetWidth(),
            childXRect.GetHeight());
        childX->SetBounds(childXRectBounds);
        childX->SetUseUnion(true);
        unionNode->AddChild(childX);
        RegisterNode(childX);

        auto childY = RSCanvasNode::Create();
        auto childYRect = rectYParams[0].rect_;
        Rosen::Vector4f childYRectBounds(childYRect.GetLeft(), childYRect.GetTop(), childYRect.GetWidth(),
            childYRect.GetHeight());
        childY->SetBounds(childYRectBounds);
        childY->SetUseUnion(true);
        unionNode->AddChild(childY);
        RegisterNode(childY);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Union_Container_Rect_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(rectXParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        Rosen::Vector4f bounds(x, y, sizeX, sizeY);
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", bounds);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);

        auto unionNode = RSUnionNode::Create();
        Rosen::Vector4f unionNodeBounds(0, 0, sizeX, sizeY);
        unionNode->SetBounds(unionNodeBounds);
        unionNode->SetFrame(unionNodeBounds);
        unionNode->SetMaterialNGFilter(frostedGlassFilter);
        unionNode->SetUnionSpacing(sdfShapeSpacingParams[1]);
        backgroundTestNode->AddChild(unionNode);
        RegisterNode(unionNode);

        auto childX = RSCanvasNode::Create();
        auto childXRect = rectXParams[i].rect_;
        Rosen::Vector4f childXRectBounds(childXRect.GetLeft(), childXRect.GetTop(), childXRect.GetWidth(),
            childXRect.GetHeight());
        childX->SetBounds(childXRectBounds);
        childX->SetUseUnion(true);
        unionNode->AddChild(childX);
        RegisterNode(childX);

        auto childY = RSCanvasNode::Create();
        auto childYRect = rectYParams[i].rect_;
        Rosen::Vector4f childYRectBounds(childYRect.GetLeft(), childYRect.GetTop(), childYRect.GetWidth(),
            childYRect.GetHeight());
        childY->SetBounds(childYRectBounds);
        childY->SetUseUnion(true);
        unionNode->AddChild(childY);
        RegisterNode(childY);
    }
}
}  // namespace OHOS::Rosen

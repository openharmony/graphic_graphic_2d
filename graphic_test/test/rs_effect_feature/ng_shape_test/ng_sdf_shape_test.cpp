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
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams1[i].Inverse());

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
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams2[i].Inverse());

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
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams3[i].Inverse());

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
        transformShape1->Setter<SDFTransformShapeMatrixTag>(matrix1.Inverse());
        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrect);
        transformShape1->Setter<SDFTransformShapeShapeTag>(childShape);

        auto sdfShape2 = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto transformShape2 = std::static_pointer_cast<RSNGSDFTransformShape>(sdfShape2);
        transformShape2->Setter<SDFTransformShapeMatrixTag>(matrix.Inverse());
        transformShape2->Setter<SDFTransformShapeShapeTag>(sdfShape1);
    
        auto sdfShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto transformShape = std::static_pointer_cast<RSNGSDFTransformShape>(sdfShape);
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix2.Inverse());
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

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_PixelmapShape_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdfPixelmapShapePath.size());
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

        auto childShape = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
        auto pixelmapChildShape = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShape);
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap(sdfPixelmapShapePath[i], Media::AllocatorType::SHARE_MEM_ALLOC);
        pixelmapChildShape->Setter<SDFPixelmapShapeImageTag>(pixelmap);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_PixelmapShape_Empty_Test)
{
    int columnCount = 1;
    int rowCount = 1;
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

        auto childShape = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
        auto pixelmapChildShape = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShape);
        std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
        pixelmapChildShape->Setter<SDFPixelmapShapeImageTag>(pixelmap);
        backgroundTestNode->SetSDFShape(childShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_PixelmapShape_W_Transform_1_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(matrix3fParams1.size());
    uint16_t pixelmapIndex = 0;
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

        auto childShape = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
        auto pixelmapChildShape = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShape);
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap(sdfPixelmapShapePath[pixelmapIndex], Media::AllocatorType::SHARE_MEM_ALLOC);
        pixelmapChildShape->Setter<SDFPixelmapShapeImageTag>(pixelmap);
        auto sdfShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto transformShape = std::static_pointer_cast<RSNGSDFTransformShape>(sdfShape);
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams1[i].Inverse());
        transformShape->Setter<SDFTransformShapeShapeTag>(childShape);
        backgroundTestNode->SetSDFShape(transformShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_PixelmapShape_W_Transform_2_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(matrix3fParams2.size());
    uint16_t pixelmapIndex = 0;
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

        auto childShape = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
        auto pixelmapChildShape = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShape);
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap(sdfPixelmapShapePath[pixelmapIndex], Media::AllocatorType::SHARE_MEM_ALLOC);
        pixelmapChildShape->Setter<SDFPixelmapShapeImageTag>(pixelmap);
        auto sdfShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto transformShape = std::static_pointer_cast<RSNGSDFTransformShape>(sdfShape);
        transformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams2[i].Inverse());
        transformShape->Setter<SDFTransformShapeShapeTag>(childShape);
        backgroundTestNode->SetSDFShape(transformShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_PixelmapShape_W_UNION_OP_Test)
{
    int columnCount = 1;
    int rowCount = 1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    uint16_t leftPixelmapIndex = 0;
    uint16_t rightPixelmapIndex = 1;
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
        std::shared_ptr<Media::PixelMap> pixelmapX =
            DecodePixelMap(sdfPixelmapShapePath[leftPixelmapIndex], Media::AllocatorType::SHARE_MEM_ALLOC);
        std::shared_ptr<Media::PixelMap> pixelmapY =
            DecodePixelMap(sdfPixelmapShapePath[rightPixelmapIndex], Media::AllocatorType::SHARE_MEM_ALLOC);
        InitSmoothUnionShapesByPixelmap(sdfShape, pixelmapX, pixelmapY, 0.0f);
        backgroundTestNode->SetSDFShape(sdfShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        auto clipNode = RSCanvasNode::Create();
        clipNode->SetBounds(bounds);
        clipNode->SetFrame(bounds);
        clipNode->SetClipToBounds(true);
        clipNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        childNode->AddChild(clipNode);
        RegisterNode(clipNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOpShape_Basic_Test)
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

        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        // 创建 UnionOpShape
        auto rootShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(rootShape);

        // 设置第一个子形状
        auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
        rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(rectXParams[i]);
        unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

        // 设置第二个子形状
        auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
        rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(rectYParams[i]);
        unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);

        backgroundTestNode->SetSDFShape(rootShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOpShape_W_Transform_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(matrix3fParams1.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    RRect rrectX{RectT<float>{100, 100, 200, 200}, 20.0f, 20.0f};
    RRect rrectY{RectT<float>{150, 150, 200, 200}, 20.0f, 20.0f};

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

        // 创建 UnionOpShape
        auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

        // 设置第一个子形状
        auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
        rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(rrectX);
        unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

        // 设置第二个子形状
        auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
        rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(rrectY);
        unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);

        // 创建 Transform 包裹 UnionOpShape
        auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
        sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams1[i].Inverse());
        sdfTransformShape->Setter<SDFTransformShapeShapeTag>(unionShape);

        backgroundTestNode->SetSDFShape(transformShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOpShape_Pixelmap_Test)
{
    int columnCount = 1;
    int rowCount = 2; // 测试不同的 pixelmap 组合
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

        // 创建 UnionOpShape
        auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

        // 设置第一个 Pixelmap 子形状
        auto childShapeX = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
        auto pixelmapChildShapeX = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShapeX);
        std::shared_ptr<Media::PixelMap> pixelmapX =
            DecodePixelMap(sdfPixelmapShapePath[i % sdfPixelmapShapePath.size()],
                          Media::AllocatorType::SHARE_MEM_ALLOC);
        pixelmapChildShapeX->Setter<SDFPixelmapShapeImageTag>(pixelmapX);
        unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

        // 设置第二个 Pixelmap 子形状
        auto childShapeY = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
        auto pixelmapChildShapeY = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShapeY);
        std::shared_ptr<Media::PixelMap> pixelmapY =
            DecodePixelMap(sdfPixelmapShapePath[(i + 1) % sdfPixelmapShapePath.size()],
                          Media::AllocatorType::SHARE_MEM_ALLOC);
        pixelmapChildShapeY->Setter<SDFPixelmapShapeImageTag>(pixelmapY);
        unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);

        backgroundTestNode->SetSDFShape(unionShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOp_Comparison_Test)
{
    int columnCount = 2; // 左边 UNION_OP，右边 SMOOTH_UNION_OP
    int rowCount = static_cast<int>(rectXParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        // 测试 UNION_OP（左列）
        {
            auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
            InitFrostedGlassFilter(frostedGlassFilter);

            auto backgroundTestNode = RSCanvasNode::Create();
            Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
            backgroundTestNode->SetBounds(bounds);
            backgroundTestNode->SetFrame(bounds);
            backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

            auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
            auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

            auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
            rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(rectXParams[i]);
            unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

            auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
            rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(rectYParams[i]);
            unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);

            backgroundTestNode->SetSDFShape(unionShape);

            auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
            childNode->AddChild(backgroundTestNode);
            RegisterNode(backgroundTestNode);
            GetRootNode()->AddChild(childNode);
            RegisterNode(childNode);
        }

        // 测试 SMOOTH_UNION_OP（右列，用于对比）
        {
            auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
            InitFrostedGlassFilter(frostedGlassFilter);

            auto backgroundTestNode = RSCanvasNode::Create();
            Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
            backgroundTestNode->SetBounds(bounds);
            backgroundTestNode->SetFrame(bounds);
            backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

            std::shared_ptr<RSNGShapeBase> smoothUnionShape;
            InitSmoothUnionShapes(smoothUnionShape, rectXParams[i], rectYParams[i], 30.0f);

            backgroundTestNode->SetSDFShape(smoothUnionShape);

            auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
            childNode->AddChild(backgroundTestNode);
            RegisterNode(backgroundTestNode);
            GetRootNode()->AddChild(childNode);
            RegisterNode(childNode);
        }
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOp_MixedShapes_Test)
{
    int columnCount = 2;
    int rowCount = 2;
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

        // 第一行：RRect在前，Pixelmap在后
        // 第二行：Pixelmap在前，RRect在后
        if (i == 0) {
            auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
            rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(
                RRect{RectT<float>{100, 100, 200, 200}, 20.0f, 20.0f});
            unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

            auto childShapeY = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
            auto pixelmapChildShapeY = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShapeY);
            std::shared_ptr<Media::PixelMap> pixelmapY =
                DecodePixelMap(sdfPixelmapShapePath[0], Media::AllocatorType::SHARE_MEM_ALLOC);
            pixelmapChildShapeY->Setter<SDFPixelmapShapeImageTag>(pixelmapY);
            unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);
        } else {
            auto childShapeX = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
            auto pixelmapChildShapeX = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShapeX);
            std::shared_ptr<Media::PixelMap> pixelmapX =
                DecodePixelMap(sdfPixelmapShapePath[0], Media::AllocatorType::SHARE_MEM_ALLOC);
            pixelmapChildShapeX->Setter<SDFPixelmapShapeImageTag>(pixelmapX);
            unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

            auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
            rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(
                RRect{RectT<float>{100, 100, 200, 200}, 20.0f, 20.0f});
            unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);
        }

        backgroundTestNode->SetSDFShape(unionShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOp_Nested_Test)
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

        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        // 根节点：UNION_OP
        auto rootUnion = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto rootUnionShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(rootUnion);

        // 设置第一个子形状：RRect
        auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
        rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(rectXParams[i]);
        rootUnionShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

        // 设置第二个子形状：嵌套的 UNION_OP
        auto nestedUnion = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto nestedUnionShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(nestedUnion);

        auto nestedChildX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto nestedRRectX = std::static_pointer_cast<RSNGSDFRRectShape>(nestedChildX);
        nestedRRectX->Setter<SDFRRectShapeRRectTag>(rectYParams[i]);
        nestedUnionShape->Setter<SDFUnionOpShapeShapeXTag>(nestedChildX);

        auto nestedChildY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto nestedRRectY = std::static_pointer_cast<RSNGSDFRRectShape>(nestedChildY);
        nestedRRectY->Setter<SDFRRectShapeRRectTag>(
            RRect{RectT<float>{250, 350, 150, 150}, 20.0f, 20.0f});
        nestedUnionShape->Setter<SDFUnionOpShapeShapeYTag>(nestedChildY);

        rootUnionShape->Setter<SDFUnionOpShapeShapeYTag>(nestedUnion);

        backgroundTestNode->SetSDFShape(rootUnion);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Transform_Rotation_Skew_Perspective_Test)
{
    int columnCount = 3;
    int rowCount = 5;

    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    // 合并所有 Transform 参数
    std::vector<Matrix3f> allTransformParams;
    allTransformParams.insert(allTransformParams.end(), matrix3fParams2.begin(), matrix3fParams2.end());
    allTransformParams.insert(allTransformParams.end(), matrix3fParams3.begin(), matrix3fParams3.end());

    for (int i = 0; i < static_cast<int>(allTransformParams.size()); i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
        sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(allTransformParams[i].Inverse());

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectShape->Setter<SDFRRectShapeRRectTag>(
            RRect{RectT<float>{100, 100, 200, 200}, 20.0f, 20.0f});
        sdfTransformShape->Setter<SDFTransformShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(transformShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_UnionOp_BoundaryValues_Test)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    // 边界值测试参数
    std::vector<RRect> boundaryRRects = {
        RRect{RectT<float>{100, 100, 0, 0}, 0.0f, 0.0f},       // 宽高为0
        RRect{RectT<float>{100, 100, 0, 200}, 20.0f, 20.0f},    // 宽度为0
        RRect{RectT<float>{100, 100, 200, 0}, 20.0f, 20.0f},    // 高度为0
        RRect{RectT<float>{100, 100, 200, 200}, 0.0f, 0.0f},    // 圆角为0
    };

    for (int i = 0; i < static_cast<int>(boundaryRRects.size()); i++) {
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

        auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
        rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(boundaryRRects[i]);
        unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

        auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
        rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(
            RRect{RectT<float>{150, 150, 200, 200}, 20.0f, 20.0f});
        unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);

        backgroundTestNode->SetSDFShape(unionShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_SmoothUnion_Pixelmap_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdfPixelmapShapePath.size()) / 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < columnCount; j++) {
            auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
            InitFrostedGlassFilter(frostedGlassFilter);

            int x = j * sizeX;
            int y = i * sizeY;

            auto backgroundTestNode = RSCanvasNode::Create();
            Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
            backgroundTestNode->SetBounds(bounds);
            backgroundTestNode->SetFrame(bounds);
            backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

            std::shared_ptr<RSNGShapeBase> smoothUnionShape;
            auto idx = i * columnCount + j;
            std::shared_ptr<Media::PixelMap> pixelmapX =
                DecodePixelMap(sdfPixelmapShapePath[idx % sdfPixelmapShapePath.size()],
                              Media::AllocatorType::SHARE_MEM_ALLOC);
            std::shared_ptr<Media::PixelMap> pixelmapY =
                DecodePixelMap(sdfPixelmapShapePath[(idx + 1) % sdfPixelmapShapePath.size()],
                              Media::AllocatorType::SHARE_MEM_ALLOC);
            InitSmoothUnionShapesByPixelmap(smoothUnionShape, pixelmapX, pixelmapY, 30.0f);

            backgroundTestNode->SetSDFShape(smoothUnionShape);

            auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
            childNode->AddChild(backgroundTestNode);
            RegisterNode(backgroundTestNode);
            GetRootNode()->AddChild(childNode);
            RegisterNode(childNode);
        }
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_SmoothUnion_Transform_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(matrix3fParams2.size());
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

        // 创建 SmoothUnion
        std::shared_ptr<RSNGShapeBase> smoothUnionShape;
        InitSmoothUnionShapes(smoothUnionShape,
            RRect{RectT<float>{100, 100, 200, 200}, 20.0f, 20.0f},
            RRect{RectT<float>{200, 200, 200, 200}, 20.0f, 20.0f},
            30.0f);

        // 创建 Transform 包裹 SmoothUnion
        auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
        sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams2[i].Inverse());
        sdfTransformShape->Setter<SDFTransformShapeShapeTag>(smoothUnionShape);

        backgroundTestNode->SetSDFShape(transformShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_SmoothUnion_Spacing_Boundary_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(sdfShapeSpacingParams.size()) / columnCount + 1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < static_cast<int>(sdfShapeSpacingParams.size()); i++) {
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
        InitSmoothUnionShapes(smoothUnionShape,
            RRect{RectT<float>{100, 100, 200, 200}, 20.0f, 20.0f},
            RRect{RectT<float>{200, 200, 200, 200}, 20.0f, 20.0f},
            sdfShapeSpacingParams[i]);

        backgroundTestNode->SetSDFShape(smoothUnionShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Nested_Three_Layers_Test)
{
    int columnCount = 2;
    int rowCount = 4;
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

        std::shared_ptr<RSNGShapeBase> rootShape;

        // 4种不同的嵌套结构
        if (i == 0) {
            // Transform → UNION_OP → RRect
            auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
            auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

            auto childX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectX = std::static_pointer_cast<RSNGSDFRRectShape>(childX);
            rRectX->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{100, 100, 150, 150}, 20.0f, 20.0f});
            unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childX);

            auto childY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectY = std::static_pointer_cast<RSNGSDFRRectShape>(childY);
            rRectY->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{200, 200, 150, 150}, 20.0f, 20.0f});
            unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childY);

            auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
            auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
            sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(
                Matrix3f{0.866f, -0.5f, 0.0f, 0.5f, 0.866f, 0.0f, 0.0f, 0.0f, 1.0f}.Inverse());
            sdfTransformShape->Setter<SDFTransformShapeShapeTag>(unionShape);
            rootShape = transformShape;

        } else if (i == 1) {
            // Transform → SmoothUnion → RRect
            std::shared_ptr<RSNGShapeBase> smoothUnionShape;
            InitSmoothUnionShapes(smoothUnionShape,
                RRect{RectT<float>{100, 100, 150, 150}, 20.0f, 20.0f},
                RRect{RectT<float>{200, 200, 150, 150}, 20.0f, 20.0f},
                30.0f);

            auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
            auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
            sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(
                Matrix3f{1.5f, 0.0f, 0.0f, 0.0f, 1.5f, 0.0f, 0.0f, 0.0f, 1.0f}.Inverse());
            sdfTransformShape->Setter<SDFTransformShapeShapeTag>(smoothUnionShape);
            rootShape = transformShape;

        } else if (i == 2) {
            // UNION_OP → Transform → RRect
            auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
            auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
            sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(
                Matrix3f{1.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}.Inverse());

            auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
            rRectShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{100, 100, 150, 150}, 20.0f, 20.0f});
            sdfTransformShape->Setter<SDFTransformShapeShapeTag>(childShape);

            auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
            auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);
            unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(transformShape);

            auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
            auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
            rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{200, 200, 150, 150}, 20.0f, 20.0f});
            unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);
            rootShape = unionShape;

        } else {
            // SmoothUnion → Transform → RRect (简化版：直接用SmoothUnion)
            std::shared_ptr<RSNGShapeBase> smoothUnionShape;
            InitSmoothUnionShapes(smoothUnionShape,
                RRect{RectT<float>{100, 100, 150, 150}, 20.0f, 20.0f},
                RRect{RectT<float>{200, 200, 150, 150}, 20.0f, 20.0f},
                30.0f);
            rootShape = smoothUnionShape;
        }

        backgroundTestNode->SetSDFShape(rootShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Multiple_Union_Test)
{
    int columnCount = 2;
    int rowCount = 2;
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

        // 根 UNION_OP
        auto rootUnion = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto rootUnionShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(rootUnion);

        // 第一个子形状：RRect
        auto childX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectX = std::static_pointer_cast<RSNGSDFRRectShape>(childX);
        rRectX->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{50, 50, 150, 150}, 20.0f, 20.0f});
        rootUnionShape->Setter<SDFUnionOpShapeShapeXTag>(childX);

        // 第二个子形状：UNION_OP
        auto nestedUnion = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto nestedUnionShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(nestedUnion);

        auto nestedX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto nestedRRectX = std::static_pointer_cast<RSNGSDFRRectShape>(nestedX);
        nestedRRectX->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{150, 150, 150, 150}, 20.0f, 20.0f});
        nestedUnionShape->Setter<SDFUnionOpShapeShapeXTag>(nestedX);

        auto nestedY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto nestedRRectY = std::static_pointer_cast<RSNGSDFRRectShape>(nestedY);
        nestedRRectY->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{250, 250, 150, 150}, 20.0f, 20.0f});
        nestedUnionShape->Setter<SDFUnionOpShapeShapeYTag>(nestedY);

        rootUnionShape->Setter<SDFUnionOpShapeShapeYTag>(nestedUnion);

        backgroundTestNode->SetSDFShape(rootUnion);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Pixelmap_Union_Comparison_Test)
{
    int columnCount = 2; // 左边 UNION_OP，右边 SMOOTH_UNION_OP
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < rowCount; i++) {
        // 测试 UNION_OP（左列）
        {
            auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
            InitFrostedGlassFilter(frostedGlassFilter);

            auto backgroundTestNode = RSCanvasNode::Create();
            Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
            backgroundTestNode->SetBounds(bounds);
            backgroundTestNode->SetFrame(bounds);
            backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

            auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
            auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

            auto childShapeX = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
            auto pixelmapChildShapeX = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShapeX);
            std::shared_ptr<Media::PixelMap> pixelmapX =
                DecodePixelMap(sdfPixelmapShapePath[i % sdfPixelmapShapePath.size()],
                              Media::AllocatorType::SHARE_MEM_ALLOC);
            pixelmapChildShapeX->Setter<SDFPixelmapShapeImageTag>(pixelmapX);
            unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(childShapeX);

            auto childShapeY = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
            auto pixelmapChildShapeY = std::static_pointer_cast<RSNGSDFPixelmapShape>(childShapeY);
            std::shared_ptr<Media::PixelMap> pixelmapY =
                DecodePixelMap(sdfPixelmapShapePath[(i + 1) % sdfPixelmapShapePath.size()],
                              Media::AllocatorType::SHARE_MEM_ALLOC);
            pixelmapChildShapeY->Setter<SDFPixelmapShapeImageTag>(pixelmapY);
            unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(childShapeY);

            backgroundTestNode->SetSDFShape(unionShape);

            auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, i * sizeY, sizeX, sizeY});
            childNode->AddChild(backgroundTestNode);
            RegisterNode(backgroundTestNode);
            GetRootNode()->AddChild(childNode);
            RegisterNode(childNode);
        }

        // 测试 SMOOTH_UNION_OP（右列，用于对比）
        {
            auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
            InitFrostedGlassFilter(frostedGlassFilter);

            auto backgroundTestNode = RSCanvasNode::Create();
            Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
            backgroundTestNode->SetBounds(bounds);
            backgroundTestNode->SetFrame(bounds);
            backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

            std::shared_ptr<RSNGShapeBase> smoothUnionShape;
            std::shared_ptr<Media::PixelMap> pixelmapX =
                DecodePixelMap(sdfPixelmapShapePath[i % sdfPixelmapShapePath.size()],
                              Media::AllocatorType::SHARE_MEM_ALLOC);
            std::shared_ptr<Media::PixelMap> pixelmapY =
                DecodePixelMap(sdfPixelmapShapePath[(i + 1) % sdfPixelmapShapePath.size()],
                              Media::AllocatorType::SHARE_MEM_ALLOC);
            InitSmoothUnionShapesByPixelmap(smoothUnionShape, pixelmapX, pixelmapY, 30.0f);

            backgroundTestNode->SetSDFShape(smoothUnionShape);

            auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {sizeX, i * sizeY, sizeX, sizeY});
            childNode->AddChild(backgroundTestNode);
            RegisterNode(backgroundTestNode);
            GetRootNode()->AddChild(childNode);
            RegisterNode(childNode);
        }
    }
}

GRAPHIC_TEST(NGSDFShapeTest, EFFECT_TEST, Set_SDF_Transform_InvalidMatrix_Test)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    // 异常矩阵
    std::vector<Matrix3f> invalidMatrices = {
        Matrix3f{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // 全0
        Matrix3f{-1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // 负缩放
        Matrix3f{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // 完全无效
    };

    for (int i = 0; i < static_cast<int>(invalidMatrices.size()); i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);

        // 注意：对无效矩阵调用 Inverse() 可能会产生问题
        // 这里直接使用原矩阵进行测试
        sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(invalidMatrices[i]);

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectShape->Setter<SDFRRectShapeRRectTag>(
            RRect{RectT<float>{100, 100, 200, 200}, 20.0f, 20.0f});
        sdfTransformShape->Setter<SDFTransformShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(transformShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}
}  // namespace OHOS::Rosen

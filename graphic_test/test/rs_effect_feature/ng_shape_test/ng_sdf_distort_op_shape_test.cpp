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

#include "ng_sdf_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class NGSDFDistortOpShapeTest : public RSGraphicTest {
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

GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_01)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    std::vector<Vector2f> cornerParams = {
        Vector2f{0.0f, 0.0f},
        Vector2f{0.1f, 0.1f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.3f, 0.3f},
        Vector2f{0.5f, 0.5f},
        Vector2f{0.8f, 0.8f}
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

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(cornerParams[i * 2]);
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(cornerParams[i * 2 + 1]);
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(cornerParams[i * 2]);
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(cornerParams[i * 2 + 1]);
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(Vector4f{0.5f, 0.5f, 0.5f, 0.5f});

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(distortShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_02)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    std::vector<Vector4f> distortionParams = {
        Vector4f{0.0f, 0.0f, 0.0f, 0.0f},
        Vector4f{0.2f, 0.2f, 0.2f, 0.2f},
        Vector4f{0.5f, 0.5f, 0.5f, 0.5f},
        Vector4f{0.8f, 0.8f, 0.8f, 0.8f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{-0.5f, -0.5f, -0.5f, -0.5f}
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

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.2f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.2f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{0.2f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{0.2f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(distortionParams[i * 2]);

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(distortShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_03)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    std::vector<RRect> rrectParams = {
        RRect{RectT<float>{50, 50, 200, 200}, 20.0f, 20.0f},
        RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f},
        RRect{RectT<float>{150, 150, 400, 400}, 80.0f, 80.0f},
        RRect{RectT<float>{200, 200, 500, 500}, 100.0f, 100.0f},
        RRect{RectT<float>{250, 250, 600, 600}, 150.0f, 150.0f},
        RRect{RectT<float>{300, 300, 700, 700}, 200.0f, 200.0f}
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

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.3f, 0.3f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.3f, 0.3f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{0.3f, 0.3f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{0.3f, 0.3f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(Vector4f{0.3f, 0.3f, 0.3f, 0.3f});

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrectParams[i * 2]);
        sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(distortShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_04)
{
    int columnCount = 2;
    int rowCount = 3;
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

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        float cornerValue = 0.1f * (i + 1);
        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{cornerValue, cornerValue});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{cornerValue, cornerValue});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{cornerValue, cornerValue});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{cornerValue, cornerValue});

        float distortValue = 0.2f * (i + 1);
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(
            Vector4f{distortValue, distortValue, distortValue, distortValue});

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(distortShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_05)
{
    int columnCount = 2;
    int rowCount = 3;
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

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.1f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.2f, 0.1f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{0.1f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{0.2f, 0.1f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(
            Vector4f{0.1f * (i + 1), 0.2f * (i + 1), 0.3f * (i + 1), 0.4f * (i + 1)});

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(distortShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_06)
{
    int columnCount = 2;
    int rowCount = 3;
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

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        float baseCorner = 0.1f + i * 0.05f;
        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{baseCorner, baseCorner});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{baseCorner + 0.02f, baseCorner + 0.02f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{baseCorner + 0.04f, baseCorner + 0.04f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{baseCorner + 0.06f, baseCorner + 0.06f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(Vector4f{0.5f, 0.5f, 0.5f, 0.5f});

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(distortShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_07)
{
    int columnCount = 2;
    int rowCount = 3;
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

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{200.0f, 200.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{200.0f, 200.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{200.0f, 200.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{200.0f, 200.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(
            Vector4f{-0.5f, 0.0f, 0.5f, 1.0f});

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

        backgroundTestNode->SetSDFShape(distortShape);

        auto childNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        childNode->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

}  // namespace OHOS::Rosen

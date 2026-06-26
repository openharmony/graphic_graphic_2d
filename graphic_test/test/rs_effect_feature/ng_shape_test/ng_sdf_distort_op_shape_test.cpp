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

#include "ng_sdf_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

const std::string FG_TEST_JPG_PATH = "/data/local/tmp/fg_test.jpg";

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
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector2f> cornerParams = {
        Vector2f{0.0f, 0.0f},
        Vector2f{0.1f, 0.1f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.3f, 0.3f},
        Vector2f{0.5f, 0.5f},
        Vector2f{0.7f, 0.7f},
        Vector2f{1.0f, 1.0f}
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(cornerParams[i]);
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(cornerParams[i + 1]);
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(cornerParams[i]);
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(cornerParams[i + 1]);
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
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector4f> distortionParams = {
        Vector4f{0.0f, 0.0f, 0.0f, 0.0f},
        Vector4f{0.2f, 0.2f, 0.2f, 0.2f},
        Vector4f{0.5f, 0.5f, 0.5f, 0.5f},
        Vector4f{0.8f, 0.8f, 0.8f, 0.8f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{-0.5f, -0.5f, -0.5f, -0.5f}
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.2f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.4f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{0.4f, 0.4f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{0.2f, 0.4f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(distortionParams[i]);

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
    auto sizeY = screenHeight / rowCount;

    std::vector<RRect> rrectParams = {
        RRect{RectT<float>{50, 50, 200, 200}, 20.0f, 20.0f},
        RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f},
        RRect{RectT<float>{150, 150, 400, 400}, 80.0f, 80.0f},
        RRect{RectT<float>{200, 200, 500, 500}, 100.0f, 100.0f},
        RRect{RectT<float>{250, 250, 600, 600}, 150.0f, 150.0f},
        RRect{RectT<float>{300, 300, 700, 700}, 200.0f, 200.0f}
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.2f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.4f, 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{0.4f, 0.4f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{0.2f, 0.4f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(Vector4f{0.3f, 0.3f, 0.3f, 0.3f});

        auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
        rRectChildShape->Setter<SDFRRectShapeRRectTag>(rrectParams[i]);
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
    auto sizeY = screenHeight / rowCount;

    for (int i = 0; i < columnCount * rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        float cornerValue = 0.1f * (i + 1);
        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.5f - cornerValue, 0.5f - cornerValue});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.5f + cornerValue, 0.5f - cornerValue});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{0.5f + cornerValue, 0.5f + cornerValue});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{0.5f - cornerValue, 0.5f + cornerValue});

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
    auto sizeY = screenHeight / rowCount;

    for (int i = 0; i < columnCount * rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.1f, 0.1f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.6f, 0.1f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{0.6f, 0.6f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{0.1f, 0.6f});
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
    auto sizeY = screenHeight / rowCount;

    for (int i = 0; i < columnCount * rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        float baseCorner = 0.1f + i * 0.1f;
        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{baseCorner + 0.1f, baseCorner + 0.1f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{baseCorner + 0.2f, baseCorner + 0.1f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{baseCorner + 0.2f, baseCorner + 0.2f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{baseCorner + 0.1f, baseCorner + 0.2f});
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
    auto sizeY = screenHeight / rowCount;

    for (int i = 0; i < columnCount * rowCount; i++) {
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundTestNode = RSCanvasNode::Create(false, false,
        RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f bounds{0, 0, sizeX, sizeY};
        backgroundTestNode->SetBounds(bounds);
        backgroundTestNode->SetFrame(bounds);
        backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

        auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
        auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);

        sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{100.0f, 100.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{101.0f, 100.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{101.0f, 101.0f});
        sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{100.0f, 101.0f});
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

// Test08: innerShape!=null, sync=true, RRect should be synced from node bounds
GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_08)
{
    auto backgroundTestNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    RegisterNode(backgroundTestNode);
    Rosen::Vector4f bounds{200.f, 200.f, 800.f, 1600.f};
    backgroundTestNode->SetBounds(bounds);
    backgroundTestNode->SetFrame(bounds);
    backgroundTestNode->SetCornerRadius(30.f);

    auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    InitFrostedGlassFilter(frostedGlassFilter);
    backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

    auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);
    sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.1f, 0.01f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.9f, 0.01f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{1.1f, 0.7f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{-0.1f, 0.7f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(Vector4f{0.5f, 0.5f, 0.1f, 0.1f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeSyncTag>(true);

    auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
    rRectChildShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{200.f, 200.f, 1000.f, 1000.f}, 0.0f, 0.0f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

    backgroundTestNode->SetSDFShape(distortShape);
    backgroundTestNode->SetShadowRadius(50.f);
    backgroundTestNode->SetShadowColor(0xFFFF0000);

    auto childNode = SetUpNodeBgImage(FG_TEST_JPG_PATH, {0, 0, screenWidth, screenHeight});
    childNode->AddChild(backgroundTestNode);
    GetRootNode()->AddChild(childNode);
    RegisterNode(childNode);
}

// Test09: innerShape!=null, sync=false, RRect should NOT be synced
GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_09)
{
    auto backgroundTestNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    RegisterNode(backgroundTestNode);
    Rosen::Vector4f bounds{200.f, 200.f, 800.f, 1600.f};
    backgroundTestNode->SetBounds(bounds);
    backgroundTestNode->SetFrame(bounds);

    auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    InitFrostedGlassFilter(frostedGlassFilter);
    backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

    auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);
    sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.1f, 0.01f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.9f, 0.01f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{1.1f, 0.7f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{-0.1f, 0.7f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(Vector4f{0.5f, 0.5f, 0.2f, 0.2f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeSyncTag>(false);

    auto childShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    auto rRectChildShape = std::static_pointer_cast<RSNGSDFRRectShape>(childShape);
    rRectChildShape->Setter<SDFRRectShapeRRectTag>(RRect{RectT<float>{0.0f, 0.0f, 800.0f, 800.0f}, 50.0f, 50.0f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeShapeTag>(childShape);

    backgroundTestNode->SetSDFShape(distortShape);
    backgroundTestNode->SetShadowRadius(30.f);
    backgroundTestNode->SetShadowColor(0xFF00FF00);

    auto childNode = SetUpNodeBgImage(FG_TEST_JPG_PATH, {0, 0, screenWidth, screenHeight});
    childNode->AddChild(backgroundTestNode);
    GetRootNode()->AddChild(childNode);
    RegisterNode(childNode);
}

// Test10: innerShape==null, auto create innerShape and sync RRect
GRAPHIC_TEST(NGSDFDistortOpShapeTest, EFFECT_TEST, Set_SDF_DistortOpShape_Test_10)
{
    auto backgroundTestNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    RegisterNode(backgroundTestNode);
    Rosen::Vector4f bounds{200.f, 200.f, 800.f, 1600.f};
    backgroundTestNode->SetBounds(bounds);
    backgroundTestNode->SetFrame(bounds);

    auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    InitFrostedGlassFilter(frostedGlassFilter);
    backgroundTestNode->SetMaterialNGFilter(frostedGlassFilter);

    auto distortShape = CreateShape(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    auto sdfDistortOpShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(distortShape);
    sdfDistortOpShape->Setter<SDFDistortOpShapeLUCornerTag>(Vector2f{0.2f, 0.1f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeRUCornerTag>(Vector2f{0.8f, 0.1f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeRBCornerTag>(Vector2f{1.2f, 0.8f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeLBCornerTag>(Vector2f{-0.2f, 0.8f});
    sdfDistortOpShape->Setter<SDFDistortOpShapeBarrelDistortionTag>(Vector4f{0.4f, 0.4f, 0.2f, 0.2f});
    backgroundTestNode->SetSDFShape(distortShape);
    backgroundTestNode->SetShadowRadius(30.f);
    backgroundTestNode->SetShadowColor(0xFF0000FF);

    auto childNode = SetUpNodeBgImage(FG_TEST_JPG_PATH, {0, 0, screenWidth, screenHeight});
    childNode->AddChild(backgroundTestNode);
    GetRootNode()->AddChild(childNode);
    RegisterNode(childNode);
}

}  // namespace OHOS::Rosen

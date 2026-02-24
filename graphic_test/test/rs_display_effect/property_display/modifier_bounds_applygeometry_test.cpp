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
#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "common/rs_obj_abs_geometry.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ModifierBoundsApplyGeometryTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/**
 * @tc.name: RSBoundsModifierApplyGeometry_001
 * @tc.desc: Test ApplyGeometry with normal bounds values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_001)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector4f bounds = { 100.0f, 150.0f, 300.0f, 400.0f };
    modifier->SetBounds(bounds);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_002
 * @tc.desc: Test ApplyGeometry with zero bounds
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_002)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector4f bounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    modifier->SetBounds(bounds);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_003
 * @tc.desc: Test ApplyGeometry with negative position
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_003)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector4f bounds = { -100.0f, -200.0f, 300.0f, 400.0f };
    modifier->SetBounds(bounds);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_004
 * @tc.desc: Test ApplyGeometry with very large bounds
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_004)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector4f bounds = { 0.0f, 0.0f, 1920.0f, 1080.0f };
    modifier->SetBounds(bounds);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_005
 * @tc.desc: Test ApplyGeometry with fractional values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_005)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector4f bounds = { 100.5f, 150.7f, 300.3f, 400.9f };
    modifier->SetBounds(bounds);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_006
 * @tc.desc: Test ApplyGeometry with geometry nullptr
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_006)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector4f bounds = { 100.0f, 150.0f, 300.0f, 400.0f };
    modifier->SetBounds(bounds);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = nullptr;
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({ 100, 150, 300, 400 });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_007
 * @tc.desc: Test ApplyGeometry with modifier without bounds set
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_007)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_008
 * @tc.desc: Test ApplyGeometry matrix with different width/height combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_008)
{
    std::vector<Vector4f> boundsList = {
        { 50.0f, 50.0f, 100.0f, 100.0f },
        { 50.0f, 50.0f, 200.0f, 100.0f },
        { 50.0f, 50.0f, 100.0f, 200.0f },
        { 50.0f, 50.0f, 150.0f, 150.0f },
    };

    for (size_t i = 0; i < boundsList.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBounds(boundsList[i]);

        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 50, 50, 400, 400 });

        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);

        testNode->SetBounds({
            static_cast<int>(geometry->GetX()),
            static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()),
            static_cast<int>(geometry->GetHeight())
        });

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_009
 * @tc.desc: Test ApplyGeometry matrix with different position combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_009)
{
    std::vector<Vector4f> boundsList = {
        { 0.0f, 0.0f, 300.0f, 300.0f },
        { 100.0f, 0.0f, 300.0f, 300.0f },
        { 0.0f, 100.0f, 300.0f, 300.0f },
        { 100.0f, 100.0f, 300.0f, 300.0f },
        { 200.0f, 200.0f, 300.0f, 300.0f },
    };

    for (size_t i = 0; i < boundsList.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBounds(boundsList[i]);

        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 50, 50, 400, 400 });

        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);

        testNode->SetBounds({
            static_cast<int>(geometry->GetX()),
            static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()),
            static_cast<int>(geometry->GetHeight())
        });

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_010
 * @tc.desc: Test ApplyGeometry with SetBoundsSize then ApplyGeometry
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_010)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector2f size = { 350.0f, 450.0f };
    modifier->SetBoundsSize(size);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_011
 * @tc.desc: Test ApplyGeometry with SetBoundsPosition then ApplyGeometry
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_011)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector2f position = { 200.0f, 300.0f };
    modifier->SetBoundsPosition(position);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_012
 * @tc.desc: Test ApplyGeometry with SetBoundsWidth/Height then ApplyGeometry
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_012)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->SetBoundsWidth(380.0f);
    modifier->SetBoundsHeight(420.0f);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_013
 * @tc.desc: Test ApplyGeometry with SetBoundsPositionX/Y then ApplyGeometry
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_013)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->SetBoundsPositionX(180.0f);
    modifier->SetBoundsPositionY(280.0f);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_014
 * @tc.desc: Test ApplyGeometry with multiple applications
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_014)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();

    Vector4f bounds1 = { 100.0f, 100.0f, 300.0f, 300.0f };
    modifier->SetBounds(bounds1);
    modifier->ApplyGeometry(geometry);

    Vector4f bounds2 = { 150.0f, 150.0f, 350.0f, 350.0f };
    modifier->SetBounds(bounds2);
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_015
 * @tc.desc: Test ApplyGeometry with minimal bounds (1x1)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_015)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector4f bounds = { 100.0f, 100.0f, 1.0f, 1.0f };
    modifier->SetBounds(bounds);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_016
 * @tc.desc: Test ApplyGeometry matrix with aspect ratio variations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_016)
{
    std::vector<Vector4f> boundsList = {
        { 50.0f, 50.0f, 100.0f, 100.0f },
        { 50.0f, 50.0f, 200.0f, 100.0f },
        { 50.0f, 50.0f, 100.0f, 200.0f },
        { 50.0f, 50.0f, 300.0f, 100.0f },
        { 50.0f, 50.0f, 100.0f, 300.0f },
        { 50.0f, 50.0f, 150.0f, 250.0f },
    };

    for (size_t i = 0; i < boundsList.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBounds(boundsList[i]);

        auto testNode = RSCanvasNode::Create();
        int yOffset = (i / 3) * 350;
        int xOffset = (i % 3) * 380;
        testNode->SetBounds({ xOffset + 50, yOffset + 50, 400, 400 });

        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);

        testNode->SetBounds({
            xOffset + 50 + static_cast<int>(geometry->GetX()),
            yOffset + 50 + static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()),
            static_cast<int>(geometry->GetHeight())
        });

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_017
 * @tc.desc: Test ApplyGeometry with bounds at screen edge
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_017)
{
    std::vector<Vector4f> boundsList = {
        { 0.0f, 0.0f, 300.0f, 300.0f },
        { 900.0f, 0.0f, 300.0f, 300.0f },
        { 0.0f, 1700.0f, 300.0f, 300.0f },
        { 900.0f, 1700.0f, 300.0f, 300.0f },
    };

    for (size_t i = 0; i < boundsList.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBounds(boundsList[i]);

        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 50, 50, 400, 400 });

        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);

        testNode->SetBounds({
            static_cast<int>(geometry->GetX()),
            static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()),
            static_cast<int>(geometry->GetHeight())
        });

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_018
 * @tc.desc: Test ApplyGeometry with very small fractional values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_018)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    Vector4f bounds = { 100.1f, 150.2f, 0.9f, 1.1f };
    modifier->SetBounds(bounds);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_019
 * @tc.desc: Test ApplyGeometry with alternating bounds updates
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_019)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();

    for (int i = 0; i < 3; i++) {
        Vector4f bounds = { 100.0f + i * 50, 100.0f + i * 50, 300.0f - i * 20, 300.0f - i * 20 };
        modifier->SetBounds(bounds);
        modifier->ApplyGeometry(geometry);
    }

    testNode->SetBounds({
        static_cast<int>(geometry->GetX()),
        static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()),
        static_cast<int>(geometry->GetHeight())
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_020
 * @tc.desc: Test ApplyGeometry with different modifier instances
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_020)
{
    std::vector<Vector4f> boundsList = {
        { 50.0f, 50.0f, 250.0f, 250.0f },
        { 300.0f, 50.0f, 250.0f, 250.0f },
        { 550.0f, 50.0f, 250.0f, 250.0f },
        { 50.0f, 300.0f, 250.0f, 250.0f },
        { 300.0f, 300.0f, 250.0f, 250.0f },
        { 550.0f, 300.0f, 250.0f, 250.0f },
    };

    for (size_t i = 0; i < boundsList.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBounds(boundsList[i]);

        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 50, 50, 400, 400 });

        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);

        testNode->SetBounds({
            static_cast<int>(geometry->GetX()),
            static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()),
            static_cast<int>(geometry->GetHeight())
        });

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_021
 * @tc.desc: Test SetBoundsPosition with normal values - matrix 4x3
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_021)
{
    std::vector<float> positionsX = {0, 50, 100, 150};
    std::vector<float> positionsY = {0, 50, 100};
    for (size_t row = 0; row < positionsY.size(); row++) {
        for (size_t col = 0; col < positionsX.size(); col++) {
            auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
            modifier->SetBoundsPosition({positionsX[col], positionsY[row]});
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {50, 50, 200, 200});
            std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
            modifier->ApplyGeometry(geometry);
            testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
                static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_022
 * @tc.desc: Test SetBoundsPosition with boundary values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_022)
{
    std::vector<Vector2f> positions = {
        {0, 0}, {-100, 0}, {0, -100}, {-100, -100}, {-200, -200}, {-500, -500}
    };
    for (size_t i = 0; i < positions.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBoundsPosition(positions[i]);
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 200), static_cast<float>((i / 2) * 300 + 200), 200, 200});
        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);
        testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_023
 * @tc.desc: Test SetBoundsPosition with extreme values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_023)
{
    std::vector<Vector2f> extremePositions = {
        {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()},
        {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()},
        {1000, 1000}, {-1000, -1000}, {5000, 5000}, {-5000, -5000},
        {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()}
    };
    for (size_t i = 0; i < extremePositions.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBoundsPosition(extremePositions[i]);
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 100 + 50), static_cast<float>((i / 2) * 100 + 50), 100, 100});
        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);
        testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_024
 * @tc.desc: Test SetBoundsPositionX with normal values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_024)
{
    std::vector<float> positions = {-100, -50, 0, 50, 100};
    for (size_t i = 0; i < positions.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBoundsPositionX(positions[i]);
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({static_cast<float>(i * 200 + 50), 100, 150, 150});
        testNode->SetBackgroundColor(0xff00ff00);
        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);
        testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_025
 * @tc.desc: Test SetBoundsPositionY with normal values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_025)
{
    std::vector<float> positions = {-100, -50, 0, 50, 100};
    for (size_t i = 0; i < positions.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBoundsPositionY(positions[i]);
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({100, static_cast<float>(i * 200 + 50), 150, 150});
        testNode->SetBackgroundColor(0xff0000ff);
        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);
        testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_026
 * @tc.desc: Test SetBoundsPositionX/Y combined
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_026)
{
    std::vector<float> positionsX = {-100, 0, 100};
    std::vector<float> positionsY = {-100, 0, 100};
    for (size_t row = 0; row < positionsY.size(); row++) {
        for (size_t col = 0; col < positionsX.size(); col++) {
            auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
            modifier->SetBoundsPosition({positionsX[col], positionsY[row]});
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 100), static_cast<float>(row * 300 + 100), 200, 200});
            std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
            modifier->ApplyGeometry(geometry);
            testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
                static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_027
 * @tc.desc: Test SetBoundsPosition with fractional values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_027)
{
    std::vector<Vector2f> fractionalPositions = {
        {0.5f, 0.5f}, {1.5f, 1.5f}, {10.5f, 10.5f}, {50.5f, 50.5f}, {100.5f, 100.5f}
    };
    for (size_t i = 0; i < fractionalPositions.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBoundsPosition(fractionalPositions[i]);
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 3) * 300 + 50);
        float y = static_cast<float>((i / 3) * 200 + 50);
        testNode->SetBounds({x, y, 200, 200});
        testNode->SetBackgroundColor(0xffccff00);
        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);
        testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_028
 * @tc.desc: Test SetBoundsPosition with quadrant positions
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_028)
{
    std::vector<Vector2f> quadrantPositions = {
        {-100, -100}, {100, -100}, {-100, 100}, {100, 100}
    };
    for (size_t i = 0; i < quadrantPositions.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBoundsPosition(quadrantPositions[i]);
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 400 + 200), static_cast<float>((i / 2) * 400 + 200), 300, 300});
        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);
        testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_029
 * @tc.desc: Test SetBoundsPosition with diagonal positions
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_029)
{
    std::vector<Vector2f> diagonalPositions = {
        {-200, -200}, {-100, -100}, {0, 0}, {100, 100}, {200, 200}
    };
    for (size_t i = 0; i < diagonalPositions.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBoundsPosition(diagonalPositions[i]);
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>(i * 200 + 50);
        float y = 100;
        testNode->SetBounds({x, y, 200, 200});
        testNode->SetBackgroundColor(0xffff00ff);
        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);
        testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_030
 * @tc.desc: Test SetBoundsPosition rapid sequential updates
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_030)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {100, 100, 300, 300});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    Vector2f pos1 = {50, 50};
    modifier->SetBoundsPosition(pos1);
    modifier->ApplyGeometry(geometry);
    Vector2f pos2 = {100, 100};
    modifier->SetBoundsPosition(pos2);
    modifier->ApplyGeometry(geometry);
    Vector2f pos3 = {150, 150};
    modifier->SetBoundsPosition(pos3);
    modifier->ApplyGeometry(geometry);
    Vector2f pos4 = {200, 200};
    modifier->SetBoundsPosition(pos4);
    modifier->ApplyGeometry(geometry);
    testNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
        static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/**
 * @tc.name: RSBoundsModifierApplyGeometry_031
 * @tc.desc: Test SetBoundsPosition with hierarchical positioning
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierBoundsApplyGeometryTest, CONTENT_DISPLAY_TEST, RSBoundsModifierApplyGeometry_031)
{
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({50, 50, 500, 500});
    parentNode->SetBackgroundColor(0xffcccccc);
    GetRootNode()->AddChild(parentNode);
    RegisterNode(parentNode);
    std::vector<Vector2f> childPositions = {{0, 0}, {50, 50}, {100, 100}, {150, 150}};
    for (size_t i = 0; i < childPositions.size(); i++) {
        auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
        modifier->SetBoundsPosition(childPositions[i]);
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({childPositions[i].x_, childPositions[i].y_, 100, 100});
        childNode->SetBackgroundColor(0xffff0000);
        std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
        modifier->ApplyGeometry(geometry);
        childNode->SetBounds({static_cast<int>(geometry->GetX()), static_cast<int>(geometry->GetY()),
            static_cast<int>(geometry->GetWidth()), static_cast<int>(geometry->GetHeight())});
        parentNode->AddChild(childNode);
        RegisterNode(childNode);
    }
}
} // namespace OHOS::Rosen

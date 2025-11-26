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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shape_base.h"
#include "effect/rs_render_shape_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class NGFilterSDFTest : public RSGraphicTest {
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

using ShapeCreator = std::function<std::shared_ptr<RSNGShapeBase>()>;
static std::unordered_map<RSNGEffectType, ShapeCreator> creatorShape = {
    {RSNGEffectType::SDF_UNION_OP_SHAPE,
        [] { return std::make_shared<RSNGSDFUnionOpShape>(); }},
    {RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE,
        [] { return std::make_shared<RSNGSDFSmoothUnionOpShape>(); }},
    {RSNGEffectType::SDF_RRECT_SHAPE,
        [] { return std::make_shared<RSNGSDFRRectShape>(); }}
};


static std::shared_ptr<RSNGShapeBase> CreateShape(RSNGEffectType type)
{
    auto it = creatorShape.find(type);
    return it != creatorShape.end() ? it->second() : nullptr;
}

static void InitSmoothUnionShapes(
    std::shared_ptr<RSNGShapeBase>& rootShape, RRect rRectX, RRect rRectY, float spacing)
{
    rootShape = CreateShape(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);
    auto sdfUnionRootShape = std::static_pointer_cast<RSNGSDFSmoothUnionOpShape>(rootShape);

    auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
    rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(rRectX);
    sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeShapeXTag>(childShapeX);

    auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
    rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(rRectY);
    sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeShapeYTag>(childShapeY);

    sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeSpacingTag>(spacing);
}

std::vector<RRect> rectXParams = {
    RRect{RectT<float>{50, 150, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{100, 200, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{150, 250, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{150, 250, 150, 150}, 2.0f, 2.0f},
    RRect{RectT<float>{100, 200, 0, 10}, 1.0f, 1.0f}, // invalid
    RRect{RectT<float>{150, 250, 0, 10}, 1.0f, 1.0f}, // invalid
};

std::vector<RRect> rectYParams = {
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{180, 280, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 2.0f, 2.0f},
    RRect{RectT<float>{200, 300, 0, 10}, 1.0f, 1.0f}, // invalid
};

std::vector<float> sdfShapeSpacingParams = {0.0f, 30.0f, 50.0f, 100.0f, 500000.0f, -5.0f};

GRAPHIC_TEST(NGFilterSDFTest, EFFECT_TEST, Set_NG_Filter_SDF_Shadow_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(rectYParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{x, y, sizeX, sizeY};
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 0.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetShadowRadius(25.0f); // 25.0f is radius
        testNode->SetShadowColor(0xFF00FF00);
        testNode->SetClipToBounds(i <= columnCount);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(NGFilterSDFTest, EFFECT_TEST, Set_NG_Filter_SDF_Shadow_Test_2)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdfShapeSpacingParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    uint16_t rectIndex = 1;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{x, y, sizeX, sizeY};
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[rectIndex], rectYParams[rectIndex], sdfShapeSpacingParams[i]);
        testNode->SetSDFShape(sdfShape);
        testNode->SetShadowRadius(25.0f); // 25.0f is radius
        testNode->SetShadowColor(0xFF00FF00);
        testNode->SetClipToBounds(true);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(NGFilterSDFTest, EFFECT_TEST, Set_NG_Filter_SDF_Border_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdfShapeSpacingParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    uint16_t rectIndex = 1;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{x, y, sizeX, sizeY};
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[rectIndex], rectYParams[rectIndex], sdfShapeSpacingParams[i]);
        testNode->SetSDFShape(sdfShape);
        testNode->SetBorderWidth(5.0f); // 5.0f is radius
        testNode->SetBorderColor(0xFFFF0000);
        testNode->SetBorderStyle(0);
        testNode->SetClipToBounds(i <= columnCount);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(NGFilterSDFTest, EFFECT_TEST, Set_NG_Filter_SDF_Shadow_Border_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(rectYParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{x, y, sizeX, sizeY};
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 0.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetShadowRadius(25.0f); // 25.0f is radius
        testNode->SetShadowColor(0xFF00FF00);
        testNode->SetBorderWidth(5.0f); // 5.0f is radius
        testNode->SetBorderColor(0xFFFF0000);
        testNode->SetBorderStyle(0);
        testNode->SetClipToBounds(true);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(NGFilterSDFTest, EFFECT_TEST, Set_NG_Filter_SDF_Shadow_Border_Test_2)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdfShapeSpacingParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    uint16_t rectIndex = 1;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{x, y, sizeX, sizeY};
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888); // 0xFF888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[rectIndex], rectYParams[rectIndex], sdfShapeSpacingParams[i]);
        testNode->SetSDFShape(sdfShape);
        testNode->SetShadowRadius(25.0f); // 25.0f is radius
        testNode->SetShadowColor(0xFF00FF00);
        testNode->SetBorderWidth(5.0f); // 5.0f is radius
        testNode->SetBorderColor(0xFFFF0000);
        testNode->SetBorderStyle(0);
        testNode->SetClipToBounds(i <= columnCount);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(NGFilterSDFTest, EFFECT_TEST, Set_NG_Filter_SDF_Clip_Test_1)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(sdfShapeSpacingParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{x, y, sizeX, sizeY};
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xff888888); // 0xff888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        uint16_t index = static_cast<uint16_t>(i / columnCount);
        InitSmoothUnionShapes(sdfShape, rectXParams[index], rectYParams[index], 30.0f);
        testNode->SetSDFShape(sdfShape);
        if (i < (rowCount - columnCount)) {
            testNode->SetShadowRadius(25.0f); // 25.0f is radius
            testNode->SetShadowColor(0xff00ff00);
            testNode->SetBorderWidth(5.0f); // 5.0f is radius
            testNode->SetBorderColor(0xffff0000);
            testNode->SetBorderStyle(0);
        }
        testNode->SetClipToBounds((i % columnCount) == 0);
        auto testChlidNode = RSCanvasNode::Create();
        Rosen::Vector4f childBounds{0, 200, sizeX, 150}; // 0, 200 is offect x y, 150 is height
        testChlidNode->SetBounds(childBounds);
        testChlidNode->SetFrame(childBounds);
        testChlidNode->SetBackgroundColor(0x88ff34b3);
        testNode->AddChild(testChlidNode);
        RegisterNode(testChlidNode);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
}  // namespace OHOS::Rosen

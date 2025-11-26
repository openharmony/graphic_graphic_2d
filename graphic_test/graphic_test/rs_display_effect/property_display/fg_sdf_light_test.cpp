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
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_shape_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ForegroundTest03 : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetSurfaceColor(RSColor(0xff000000));
    }
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

static std::vector<RRect> rectXParams = {
    RRect{RectT<float>{50, 150, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{100, 200, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{150, 250, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{150, 250, 150, 150}, 2.0f, 2.0f},
    RRect{RectT<float>{100, 200, 0, 10}, 1.0f, 1.0f}, // invalid
    RRect{RectT<float>{150, 250, 0, 10}, 1.0f, 1.0f}, // invalid
};

static std::vector<RRect> rectYParams = {
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{180, 280, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 2.0f, 2.0f},
    RRect{RectT<float>{200, 300, 0, 10}, 1.0f, 1.0f}, // invalid
};

static std::vector<float> sdfShapeSpacingParams = {0.0f, 30.0f, 50.0f, 100.0f, 500000.0f, -5.0f};

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBorderLight01)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetBackgroundColor(0xFF888888); // 0xFF888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 10.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(1);  // 1: BORDER

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBorderLight02)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(1);  // 1: BORDER

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFContentLight01)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetBackgroundColor(0xFF888888); // 0xFF888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 10.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(2);  // 2: CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFContentLight02)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(2);  // 2: CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBorderContentLight01)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetBackgroundColor(0xFF888888); // 0xFF888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 10.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(3);  // 3: BORDER_CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBorderContentLight02)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(3);  // 3: BORDER_CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBlendBorderLight01)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetBackgroundColor(0xFF888888); // 0xFF888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 10.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(6);  // 6: BLEND_BORDER

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBlendBorderLight02)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(6);  // 6: BLEND_BORDER

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBlendContentLight01)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetBackgroundColor(0xFF888888); // 0xFF888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 10.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(7);  // 7: BLEND_CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBlendContentLight02)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(7);  // 7: BLEND_CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBlendBorderContentLight01)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetBackgroundColor(0xFF888888); // 0xFF888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 10.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(8);  // 8: BLEND_BORDER_CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFBlendBorderContentLight02)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(8);  // 8: BLEND_BORDER_CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFNormalBorderContentLight01)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetBackgroundColor(0xFF888888); // 0xFF888888 is argb;
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, rectXParams[i], rectYParams[i], 10.0f);
        testNode->SetSDFShape(sdfShape);
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(10);  // 10: NORMAL_BORDER_CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest03, CONTENT_DISPLAY_TEST, SDFNormalBorderContentLight02)
{
    int columnCount = 2; // 2: columnCount
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
        testNode->SetClipToBounds(true);

        testNode->SetLightIntensity(1); // 1: Intensity
        testNode->SetLightColor(0xff00ff00);
        testNode->SetLightPosition(200, 200, 300);
        testNode->SetIlluminatedBorderWidth(50); // 50: BorderWidth
        testNode->SetIlluminatedType(10);  // 10: NORMAL_BORDER_CONTENT

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
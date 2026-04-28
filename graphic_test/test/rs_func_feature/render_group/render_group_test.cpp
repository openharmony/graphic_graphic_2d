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
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"

#include "render/rs_filter.h"
#include "render/rs_effect_base.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "common/rs_vector2.h"
#include "common/rs_mask.h"
#include "render/rs_properties_def.h"
#include "draw/drawing.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000;
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int NODE_SIZE = 500;
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
} // namespace

class RenderGroupTest : public RSGraphicTest {
private:
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    std::shared_ptr<RSCanvasNode> CreateTestCanvasNode(const size_t i, const size_t columnCount, const size_t rowCount)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({x, y, sizeX, sizeY});
        canvasNode->SetFrame({x, y, sizeX, sizeY});
        canvasNode->SetBackgroundColor(SK_ColorBLUE);

        return canvasNode;
    }

    std::shared_ptr<RSNGFrostedGlassEffect> InitFrostedGlassShader()
    {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        frostedGlass->Setter<FrostedGlassEffectWeightsEmbossTag>(Vector2f{1.0f, 0.5f});
        frostedGlass->Setter<FrostedGlassEffectWeightsEdlTag>(Vector2f{1.0f, 1.0f});
        frostedGlass->Setter<FrostedGlassEffectBgRatesTag>(Vector2f{-1.8792225f, 2.7626955f});
        frostedGlass->Setter<FrostedGlassEffectBgKBSTag>(Vector3f{0.0073494f, 0.0998859f, 1.2f});
        frostedGlass->Setter<FrostedGlassEffectBgPosTag>(Vector3f{0.3f, 0.5f, 0.5f});
        frostedGlass->Setter<FrostedGlassEffectBgNegTag>(Vector3f{0.5f, 1.0f, 1.0f});
        frostedGlass->Setter<FrostedGlassEffectRefractParamsTag>(Vector3f{1.0f, 0.3f, 0.3f});
        frostedGlass->Setter<FrostedGlassEffectSdParamsTag>(Vector3f{-50.0f, 6.0f, 6.62f});
        frostedGlass->Setter<FrostedGlassEffectSdRatesTag>(Vector2f{0.0f, 0.0f});
        frostedGlass->Setter<FrostedGlassEffectSdKBSTag>(Vector3f{0.9f, 0.0f, 1.0f});
        frostedGlass->Setter<FrostedGlassEffectSdPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
        frostedGlass->Setter<FrostedGlassEffectSdNegTag>(Vector3f{3.0f, 2.0f, 1.0f});
        frostedGlass->Setter<FrostedGlassEffectEnvLightParamsTag>(Vector2f{20.0f, 5.0f});
        frostedGlass->Setter<FrostedGlassEffectEnvLightRatesTag>(Vector2f{0.0f, 0.0f});
        frostedGlass->Setter<FrostedGlassEffectEnvLightKBSTag>(Vector3f{0.8f, 0.27451f, 2.0f});
        frostedGlass->Setter<FrostedGlassEffectEnvLightPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
        frostedGlass->Setter<FrostedGlassEffectEnvLightNegTag>(Vector3f{3.0f, 2.0f, 1.0f});
        frostedGlass->Setter<FrostedGlassEffectEdLightParamsTag>(Vector2f{2.0f, 2.0f});
        frostedGlass->Setter<FrostedGlassEffectEdLightAnglesTag>(Vector2f{40.0f, 20.0f});
        frostedGlass->Setter<FrostedGlassEffectEdLightDirTag>(Vector2f{2.5f, 2.5f});
        frostedGlass->Setter<FrostedGlassEffectEdLightRatesTag>(Vector2f{0.0f, 0.0f});
        frostedGlass->Setter<FrostedGlassEffectEdLightKBSTag>(Vector3f{0.6027f, 0.627451f, 2.0f});
        frostedGlass->Setter<FrostedGlassEffectEdLightPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
        frostedGlass->Setter<FrostedGlassEffectEdLightNegTag>(Vector3f{3.0f, 2.0f, 1.0f});
        return frostedGlass;
    }

    void ApplyFilterOrEffect(std::shared_ptr<RSCanvasNode>& imageNode, size_t index)
    {
        switch (index) {
            case 0:
                imageNode->SetBackgroundFilter(
                    RSFilter::CreateMaterialFilter(6, 6, 6, 0xb5c4b100, BLUR_COLOR_MODE::AVERAGE));
                break;

            case 1:
                imageNode->SetShadowColor(0xff000000);
                imageNode->SetShadowRadius(30);
                imageNode->SetShadowOffset(10, 10);
                break;

            case 2:
                imageNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(20, 20));
                break;

            case 3:
                imageNode->SetUseEffect(true);
                imageNode->SetUseEffectType(UseEffectType::EFFECT_COMPONENT);
                break;

            case 4: {
                auto frostedGlassShader = InitFrostedGlassShader();
                imageNode->SetBackgroundNGShader(frostedGlassShader);
                break;
            }

            case 5:
                imageNode->SetGrayScale(0.5f);
                break;

            default:
                break;
        }
    }

    std::shared_ptr<RSCanvasNode> CreateTransparentChildNode(const Vector4f& bounds)
    {
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds(bounds);
        childNode->SetFrame(bounds);
        childNode->SetBackgroundColor(Sk_ColorTRANSPARENT);
        return childNode;
    }

    void ApplySafeFilter(std::shared_ptr<RSCanvasNode>& node, size_t index)
    {
        switch (index) {
            case 0: { // Grey Coef Filter
                Vector2f greyCoef = {0.5f, 0.5f};
                node->SetGreyCoef(greyCoef);
                break;
            }
            
            case 1: { // Mask Filter (Gradient)
                Drawing::Brush brush;
                brush.SetAlpha(255);
                std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(brush);
                node->SetMask(mask);
                break;
            }
            
            case 2: { // LightUp Effect
                float lightUpDegree = 1.0f;
                node->SetLightUpEffectDegree(lightUpDegree);
                break;
            }
            
            case 3: { // Water Ripple
                RSWaterRipplePara waterRipplePara = {
                    2,        // waveCount
                    0.5f,     // rippleCenterX
                    0.5f,     // rippleCenterY
                    1          // rippleMode
                };
                float progress = 0.5f;
                node->SetWaterRippleParams(waterRipplePara, progress);
                break;
            }
            
            default:
                break;
        }
    }
};

/*
 * @tc.name: MARK_NODE_GROUP_001
 * @tc.desc: test there is filter need clip hole in render group, filter and image are set at the same node
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_001)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;

    // Create 6 canvas nodes in a 2x3 layout, each with different filter/effect
    for (size_t i = 0; i < 6; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});

        // Apply different filter/effect based on index
        ApplyFilterOrEffect(imageNode, i);
        canvasNode->AddChild(imageNode);

        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_002
 * @tc.desc: test filter on transparent child node of imageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_002)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;

    // Create 6 canvas nodes in a 2x3 layout, each with transparent child node
    for (size_t i = 0; i < 6; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        
        // Level 2: ImageNode (background layer with image)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
        
        // Level 3: ChildNode (transparent layer with filter)
        auto childNode = CreateTransparentChildNode({0, 0, sizeX, sizeY});
        
        // Apply different filter/effect based on index to transparent childNode
        ApplyFilterOrEffect(childNode, i);
        
        // Build the hierarchy: CanvasNode -> ImageNode -> ChildNode
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_003
 * @tc.desc: test safe filters that don't trigger HasFilterOrEffect (4 types)
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_003)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    // Create 4 canvas nodes in a 2x2 layout, each with different safe filter
    for (size_t i = 0; i < 4; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        
        // Level 2: ImageNode (background layer)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        
        // Apply safe filter based on index
        ApplySafeFilter(imageNode, i);
        
        // Build hierarchy: CanvasNode -> ImageNode
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_004
 * @tc.desc: test safe filters on transparent child nodes
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_004)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    // Create 4 canvas nodes in a 2x2 layout, each with transparent child node
    for (size_t i = 0; i < 4; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        
        // Level 2: ImageNode (background layer)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
        
        // Level 3: ChildNode (transparent layer with safe filter)
        auto childNode = CreateTransparentChildNode({0, 0, sizeX, sizeY});
        
        // Apply safe filter based on index to transparent childNode
        ApplySafeFilter(childNode, i);
        
        // Build hierarchy: CanvasNode -> ImageNode -> ChildNode
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_005
 * @tc.desc: test clip hole filters with larger ImageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_005)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + 200;
    const size_t imageSizeY = canvasSizeY + 200;
    
    for (size_t i = 0; i < 6; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        const int imageX = (int)canvasX - 100;
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {imageX, canvasY, imageSizeX, imageSizeY});
        
        auto childNode = CreateTransparentChildNode({0, 0, imageSizeX, imageSizeY});
        
        ApplyFilterOrEffect(childNode, i);
        
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_006
 * @tc.desc: test safe filters with larger ImageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_006)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + 200;
    const size_t imageSizeY = canvasSizeY + 200;
    
    for (size_t i = 0; i < 4; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        const int imageX = (int)canvasX - 100;
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {imageX, canvasY, imageSizeX, imageSizeY});
        
        auto childNode = CreateTransparentChildNode({0, 0, imageSizeX, imageSizeY});
        
        ApplySafeFilter(childNode, i);
        
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_007
 * @tc.desc: test MarkForegroundFilterCache auto-marking mechanism
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_007)
{
    const size_t sizeX = SCREEN_WIDTH;
    const size_t sizeY = SCREEN_HEIGHT;
    
    // CanvasNode (parent container, NO manual MarkNodeGroup!)
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, sizeX, sizeY});
    canvasNode->SetFrame({0, 0, sizeX, sizeY});
    canvasNode->SetBackgroundColor(SK_ColorBLUE);
    // Note: Do NOT call canvasNode->MarkNodeGroup(true) here
    // Let the foreground filter trigger auto-marking on server side
    GetRootNode()->AddChild(canvasNode);
    
    // ImageNode with foreground filter (triggers auto-marking)
    auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
    
    // Set foreground filter - this will trigger MarkForegroundFilterCache on server side
    imageNode->SetForegroundEffectRadius(10.0f);
    
    // Build hierarchy: CanvasNode -> ImageNode
    canvasNode->AddChild(imageNode);
    
    RegisterNode(canvasNode);
    RegisterNode(imageNode);
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_008
 * @tc.desc: test 6 clip hole filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_008)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 6 groups, each with different clip hole filter
    for (size_t i = 0; i < 6; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas (smaller than outer)
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                               canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true);
        
         // Apply safe filter directly on imageNode
        ApplyFilterOrEffect(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_009
 * @tc.desc: test 4 safe filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_009)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 4 groups, each with different safe filter
    for (size_t i = 0; i < 4; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas (smaller than outer)
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                               canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->AddChild(imageNode);
        
        // Child node with filter
        auto childNode = CreateTransparentChildNode(
            {0, 0, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        ApplySafeFilter(childNode, i);
        imageNode->AddChild(childNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_010
 * @tc.desc: test 6 clip hole filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_010)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 6 groups, each with different clip hole filters
    for (size_t i = 0; i < 6; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                            canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        ApplyFilterOrEffect(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_011
 * @tc.desc: test 4 safe filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_011)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 4 groups, each with different safe filters
    for (size_t i = 0; i < 4; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                            canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
   
        // Apply 1 safe filter
        ApplySafeFilter(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_012
 * @tc.desc: test 4 safe filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_012)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 4 groups, each with different safe filters
    for (size_t i = 0; i < 6; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                            canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        // Child node with filter
        auto childNode = CreateTransparentChildNode(
            {0, 0, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        // Apply 1 safe filter
        ApplyFilterOrEffect(childNode, i);
        
        imageNode->AddChild(childNode);
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_013
 * @tc.desc: test 4 safe filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_013)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 4 groups, each with different safe filters
    for (size_t i = 0; i < 4; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                            canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        // Child node with filter
        auto childNode = CreateTransparentChildNode(
            {0, 0, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        // Apply 1 safe filter
        ApplySafeFilter(childNode, i);
        
        imageNode->AddChild(childNode);
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_014
 * @tc.desc: test there is filter need clip hole in render group, filter and image are set at the same node
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_014)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;

    // Create 6 canvas nodes in a 2x3 layout, each with different filter/effect
    for (size_t i = 0; i < 6; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(canvasNode);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});

        // Apply different filter/effect based on index
        ApplyFilterOrEffect(imageNode, i);
        canvasNode->AddChild(imageNode);

        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_015
 * @tc.desc: test filter on transparent child node of imageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_015)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;

    // Create 6 canvas nodes in a 2x3 layout, each with transparent child node
    for (size_t i = 0; i < 6; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        
        // Level 2: ImageNode (background layer with image)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
        
        // Level 3: ChildNode (transparent layer with filter)
        auto childNode = CreateTransparentChildNode({0, 0, sizeX, sizeY});
        
        // Apply different filter/effect based on index to transparent childNode
        ApplyFilterOrEffect(childNode, i);
        
        // Build the hierarchy: CanvasNode -> ImageNode -> ChildNode
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_016
 * @tc.desc: test safe filters that don't trigger HasFilterOrEffect (4 types)
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_016)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    // Create 4 canvas nodes in a 2x2 layout, each with different safe filter
    for (size_t i = 0; i < 4; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        
        // Level 2: ImageNode (background layer)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        
        // Apply safe filter based on index
        ApplySafeFilter(imageNode, i);
        
        // Build hierarchy: CanvasNode -> ImageNode
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_017
 * @tc.desc: test safe filters on transparent child nodes
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_017)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    // Create 4 canvas nodes in a 2x2 layout, each with transparent child node
    for (size_t i = 0; i < 4; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        
        // Level 2: ImageNode (background layer)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
        
        // Level 3: ChildNode (transparent layer with safe filter)
        auto childNode = CreateTransparentChildNode({0, 0, sizeX, sizeY});
        
        // Apply safe filter based on index to transparent childNode
        ApplySafeFilter(childNode, i);
        
        // Build hierarchy: CanvasNode -> ImageNode -> ChildNode
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_018
 * @tc.desc: test clip hole filters with larger ImageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_018)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + 200;
    const size_t imageSizeY = canvasSizeY + 200;
    
    for (size_t i = 0; i < 6; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        const int imageX = (int)canvasX - 100;
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {imageX, canvasY, imageSizeX, imageSizeY});
        
        auto childNode = CreateTransparentChildNode({0, 0, imageSizeX, imageSizeY});
        
        ApplyFilterOrEffect(childNode, i);
        
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_019
 * @tc.desc: test safe filters with larger ImageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_019)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + 200;
    const size_t imageSizeY = canvasSizeY + 200;
    
    for (size_t i = 0; i < 4; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        const int imageX = (int)canvasX - 100;
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {imageX, canvasY, imageSizeX, imageSizeY});
        
        auto childNode = CreateTransparentChildNode({0, 0, imageSizeX, imageSizeY});
        
        ApplySafeFilter(childNode, i);
        
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_020
 * @tc.desc: test 6 clip hole filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_020)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 6 groups, each with different clip hole filter
    for (size_t i = 0; i < 6; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas (smaller than outer)
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                               canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true, false);
        
         // Apply safe filter directly on imageNode
        ApplyFilterOrEffect(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_021
 * @tc.desc: test 4 safe filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_021)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 4 groups, each with different safe filter
    for (size_t i = 0; i < 4; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas (smaller than outer)
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                               canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true, false);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->AddChild(imageNode);
        
        // Child node with filter
        auto childNode = CreateTransparentChildNode(
            {0, 0, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        ApplySafeFilter(childNode, i);
        imageNode->AddChild(childNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_022
 * @tc.desc: test 6 clip hole filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_022)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 6 groups, each with different clip hole filters
    for (size_t i = 0; i < 6; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                            canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true, false);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        ApplyFilterOrEffect(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_023
 * @tc.desc: test 4 safe filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_023)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 4 groups, each with different safe filters
    for (size_t i = 0; i < 4; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                            canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true, false);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
   
        // Apply 1 safe filter
        ApplySafeFilter(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_024
 * @tc.desc: test 4 safe filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_024)
{
    const size_t columnCount = 2;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 4 groups, each with different safe filters
    for (size_t i = 0; i < 6; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                            canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true, false);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        // Child node with filter
        auto childNode = CreateTransparentChildNode(
            {0, 0, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        // Apply 1 safe filter
        ApplyFilterOrEffect(childNode, i);
        
        imageNode->AddChild(childNode);
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_025
 * @tc.desc: test 4 safe filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_025)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 4 groups, each with different safe filters
    for (size_t i = 0; i < 4; i++) {
        // Outer canvas
        auto outerCanvas = RSCanvasNode::Create();
        outerCanvas->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        outerCanvas->SetBackgroundColor(SK_ColorBLUE);
        outerCanvas->MarkNodeGroup(true, false);
        GetRootNode()->AddChild(outerCanvas);
        
        const size_t canvasX = (i % columnCount) * canvasSizeX;
        const size_t canvasY = (i / columnCount) * canvasSizeY;
        
        // Inner canvas
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create();
        innerCanvas->SetBounds({canvasX + offset, canvasY + offset, 
                             canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->SetFrame({canvasX + offset, canvasY + offset, 
                            canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        innerCanvas->MarkNodeGroup(true, false);
        outerCanvas->AddChild(innerCanvas);
        
        // Image node
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, 
            {canvasX + offset, canvasY + offset, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        // Child node with filter
        auto childNode = CreateTransparentChildNode(
            {0, 0, canvasSizeX - 2*offset, canvasSizeY - 2*offset});
        
        // Apply 1 safe filter
        ApplySafeFilter(childNode, i);
        
        imageNode->AddChild(childNode);
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}
} // namespace OHOS::Rosen

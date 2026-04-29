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
#include "ui/rs_surface_node.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "common/rs_vector2.h"
#include "render/rs_mask.h"
#include "property/rs_properties_def.h"
#include "../ng_filter_test/ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000;
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
} // namespace

class RenderGroupTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    std::shared_ptr<RSCanvasNode> CreateTestCanvasNode(const size_t i, const size_t columnCount, const size_t rowCount)
    {
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto canvasNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        canvasNode->SetBounds({x, y, sizeX, sizeY});
        canvasNode->SetFrame({x, y, sizeX, sizeY});

        return canvasNode;
    }

    void ApplyFilterOrEffect(std::shared_ptr<RSCanvasNode>& imageNode, size_t index)
    {
        switch (index) {
            case 0:
                imageNode->SetBackgroundFilter(
                    RSFilter::CreateMaterialFilter(20., 1, 1, 0, BLUR_COLOR_MODE::AVERAGE, true));
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
                imageNode->SetGrayScale(0.5f);
                break;

            default:
                break;
        }
    }

    std::shared_ptr<RSCanvasNode> CreateTransparentChildNode(const Vector4f& bounds)
    {
        auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        childNode->SetBounds(bounds);
        childNode->SetFrame(bounds);
        return childNode;
    }

    void ApplySafeFilter(std::shared_ptr<RSCanvasNode>& node, size_t index)
    {
        switch (index) {
            case 0: { // Grey Coef Filter
                Vector2f greyCoef = {20.0f, 20.0f};
                node->SetGreyCoef(greyCoef);
                node->SetAlpha(0.333f);
                break;
            }
            
            case 1: { // Mask Filter (Gradient)
                Drawing::Brush brush = Drawing::Brush();
                brush.SetColor(0xffffffff);
                Drawing::Path path = Drawing::Path();
                path.AddRect(Drawing::Rect(50, 50, 900, 900));
                auto mask = RSMask::CreatePathMask(path, brush);
                node->SetMask(mask);
                break;
            }
            
            case 2: { // LightUp Effect
                float lightUpDegree = 0.3f;
                node->SetLightUpEffectDegree(lightUpDegree);
                break;
            }
            
            case 3: { // Water Ripple
                RSWaterRipplePara waterRipplePara = { 3, 0.3, 0.5 };
                float progress = 0.3;
                node->SetWaterRippleParams(waterRipplePara, progress);
                node->SetAlpha(0.375f);
                break;
            }
            
            default:
                break;
        }
    }

    void ApplyFilterOrEffect2(std::shared_ptr<RSCanvasNode>& imageNode, size_t index)
    {
        switch (index) {
            case 0: { // Aurora Noise
                auto auroraNoise = std::make_shared<RSNGAuroraNoise>();
                auroraNoise->Setter<AuroraNoiseNoiseTag>(0.3f);
                imageNode->SetBackgroundNGShader(auroraNoise);
                break;
            }
            case 1: { // Edge Light
                auto edgeLightPtr = CreateFilter(RSNGEffectType::EDGE_LIGHT);
                auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(edgeLightPtr);
                edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
                edgeLightFilter->Setter<EdgeLightAlphaTag>(1.0f);
                imageNode->SetBackgroundNGFilter(edgeLightFilter);
                break;
            }
            case 2: { // Grid Warp
                auto gridWarpFilter = std::make_shared<RSNGGridWarpFilter>();
                gridWarpFilter->Setter<GridWarpGridPoint0Tag>(Vector2f(0.0f, 0.0f));
                gridWarpFilter->Setter<GridWarpGridPoint1Tag>(Vector2f(0.3f, 0.0f));
                gridWarpFilter->Setter<GridWarpGridPoint2Tag>(Vector2f(0.6f, 0.0f));
                gridWarpFilter->Setter<GridWarpGridPoint3Tag>(Vector2f(1.0f, 0.0f));
                gridWarpFilter->Setter<GridWarpGridPoint4Tag>(Vector2f(0.0f, 0.5f));
                gridWarpFilter->Setter<GridWarpGridPoint5Tag>(Vector2f(0.0f, 0.5f));
                gridWarpFilter->Setter<GridWarpGridPoint6Tag>(Vector2f(0.0f, 0.5f));
                gridWarpFilter->Setter<GridWarpGridPoint7Tag>(Vector2f(0.0f, 0.5f));
                imageNode->SetBackgroundNGFilter(gridWarpFilter);
                break;
            }
            case 3: { // Bezier Warp
                auto bezierWarpPtr = CreateFilter(RSNGEffectType::BEZIER_WARP);
                auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(bezierWarpPtr);
                bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.1f, 0.1f));
                bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.2f, 0.15f));
                bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(0.4f, 0.15f));
                bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.5f, 0.1f));
                bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.1f, 0.4f));
                bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(0.3f, 0.4f));
                bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.45f, 0.4f));
                bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.5f, 0.4f));
                bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(0.6f, 0.4f));
                bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.75f, 0.4f));
                bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.85f, 0.4f));
                bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(0.9f, 0.9f));
                imageNode->SetBackgroundNGFilter(bezierWarpFilter);
                break;
            }
            default:
                break;
        }
    }

    void ApplyFilterOrEffect3(std::shared_ptr<RSCanvasNode>& imageNode, size_t index)
    {
        switch (index) {
            case 0: { // Content Light
                auto contentLightPtr = CreateFilter(RSNGEffectType::CONTENT_LIGHT);
                auto contentLightFilter = std::static_pointer_cast<RSNGContentLightFilter>(contentLightPtr);
                contentLightFilter->Setter<ContentLightPositionTag>(Vector3f(0.0f, 0.0f, 2.0f));
                contentLightFilter->Setter<ContentLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
                contentLightFilter->Setter<ContentLightIntensityTag>(10.0f);
                imageNode->SetBackgroundNGFilter(contentLightFilter);
                break;
            }
            case 1: { // Magnifier
                auto magnifierPtr = CreateFilter(RSNGEffectType::MAGNIFIER);
                auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(magnifierPtr);
                magnifierFilter->Setter<MagnifierFactorTag>(1.4f);
                magnifierFilter->Setter<MagnifierWidthTag>(500.0f);
                magnifierFilter->Setter<MagnifierHeightTag>(500.0f);
                magnifierFilter->Setter<MagnifierCornerRadiusTag>(36.0f);
                magnifierFilter->Setter<MagnifierBorderWidthTag>(2.0f);
                imageNode->SetForegroundNGFilter(magnifierFilter);
                break;
            }
            case 2: { // Sound Wave
                auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
                auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);
                soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f(1.0f, 0.5f, 0.0f, 0.9f));
                soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f(0.0f, 0.8f, 0.4f, 0.8f));
                soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f(0.3f, 0.3f, 0.9f, 0.7f));
                soundWaveFilter->Setter<SoundWaveColorProgressTag>(0.5f);
                soundWaveFilter->Setter<SoundWaveIntensityTag>(0.7f);
                soundWaveFilter->Setter<SoundWaveAlphaATag>(0.6f);
                soundWaveFilter->Setter<SoundWaveAlphaBTag>(0.4f);
                soundWaveFilter->Setter<SoundWaveProgressATag>(0.3f);
                soundWaveFilter->Setter<SoundWaveProgressBTag>(0.6f);
                soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(0.85f);
                imageNode->SetForegroundNGFilter(soundWaveFilter);
                break;
            }
            default:
                break;
        }
    }

private:
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;
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
    const size_t rowCount = 2;

    // Create 6 canvas nodes in a 2x2 layout, each with different filter/effect
    for (size_t i = 0; i < 4; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = 20;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX-offset, sizeY-offset});

        // Apply different filter/effect based on index
        ApplyFilterOrEffect(imageNode, i);
        canvasNode->AddChild(imageNode);

        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }

    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
    const size_t rowCount = 2;

    // Create 6 canvas nodes in a 2x3 layout, each with transparent child node
    for (size_t i = 0; i < 4; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = 20;
        const size_t offset1 = 60;

        // Level 2: ImageNode (background layer with image)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        // Level 3: ChildNode (transparent layer with filter)
        auto childNode = CreateTransparentChildNode({0, 0, sizeX - offset1, sizeY - offset1});
        
        // Apply different filter/effect based on index to transparent childNode
        ApplyFilterOrEffect(childNode, i);
        
        // Build the hierarchy: CanvasNode -> ImageNode -> ChildNode
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
        const size_t offset = 20;
        
        // Level 2: ImageNode (background layer)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        // Apply safe filter based on index
        ApplySafeFilter(imageNode, i);
        
        // Build hierarchy: CanvasNode -> ImageNode
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
        const size_t offset = 20;
        const size_t offset1 = 60;
        
        // Level 2: ImageNode (background layer)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        // Level 3: ChildNode (transparent layer with safe filter)
        auto childNode = CreateTransparentChildNode({0, 0, sizeX - offset1, sizeY - offset1});
        
        // Apply safe filter based on index to transparent childNode
        ApplySafeFilter(imageNode, i);
        
        // Build hierarchy: CanvasNode -> ImageNode -> ChildNode
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + 200;
    const size_t imageSizeY = canvasSizeY + 200;
    
    for (size_t i = 0; i < 4; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, imageSizeX, imageSizeY});
        
        ApplyFilterOrEffect(imageNode, i);
        
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, imageSizeX, imageSizeY});
        
        ApplySafeFilter(imageNode, i);

        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
    auto canvasNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    canvasNode->SetBounds({0, 0, sizeX, sizeY});
    canvasNode->SetFrame({0, 0, sizeX, sizeY});
    // Note: Do NOT call canvasNode->MarkNodeGroup(true) here
    // Let the foreground filter trigger auto-marking on server side
    GetRootNode()->AddChild(canvasNode);
    
    // ImageNode with foreground filter (triggers auto-marking)
    auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
    
    // Set foreground filter - this will trigger MarkForegroundFilterCache on server side
    imageNode->SetForegroundEffectRadius(30.0f);
    
    // Build hierarchy: CanvasNode -> ImageNode
    canvasNode->AddChild(imageNode);
    
    RegisterNode(canvasNode);
    RegisterNode(imageNode);
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 6 groups, each with different clip hole filter
    for (size_t i = 0; i < 4; i++) {
        // Outer canvas
        auto outerCanvas = CreateTestCanvasNode(i, columnCount, rowCount);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);

        // Inner canvas (smaller than outer)
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        innerCanvas->SetBounds({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->SetFrame({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        outerCanvas->AddChild(innerCanvas);

        // Image node
        auto imageNode = SetUpNodeBgImage(
            TEST_IMAGE_PATH, { 2 * offset, 2 * offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true);

        // Apply safe filter directly on imageNode
        ApplyFilterOrEffect(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
        auto outerCanvas = CreateTestCanvasNode(i, columnCount, rowCount);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);

        // Inner canvas (smaller than outer)
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        innerCanvas->SetBounds({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->SetFrame({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        outerCanvas->AddChild(innerCanvas);

        // Image node
        auto imageNode = SetUpNodeBgImage(
            TEST_IMAGE_PATH, { 2 * offset, 2 * offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true);

        ApplySafeFilter(imageNode, i);

        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_010
 * @tc.desc: test there is filter need clip hole in render group, filter and image are set at the same node
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_010)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    // Create 6 canvas nodes in a 2x2 layout, each with different filter/effect
    for (size_t i = 0; i < 4; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = 20;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX-offset, sizeY-offset});

        // Apply different filter/effect based on index
        ApplyFilterOrEffect2(imageNode, i);
        canvasNode->AddChild(imageNode);

        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }

    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_011
 * @tc.desc: test filter on transparent child node of imageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_011)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    // Create 6 canvas nodes in a 2x3 layout, each with transparent child node
    for (size_t i = 0; i < 4; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = 20;
        const size_t offset1 = 60;

        // Level 2: ImageNode (background layer with image)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        // Level 3: ChildNode (transparent layer with filter)
        auto childNode = CreateTransparentChildNode({0, 0, sizeX - offset1, sizeY - offset1});
        
        // Apply different filter/effect based on index to transparent childNode
        ApplyFilterOrEffect2(childNode, i);
        
        // Build the hierarchy: CanvasNode -> ImageNode -> ChildNode
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_012
 * @tc.desc: test clip hole filters with larger ImageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_012)
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

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, imageSizeX, imageSizeY});
        
        ApplyFilterOrEffect2(imageNode, i);
        
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_013
 * @tc.desc: test 6 clip hole filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_013)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 6 groups, each with different clip hole filter
    for (size_t i = 0; i < 4; i++) {
        // Outer canvas
        auto outerCanvas = CreateTestCanvasNode(i, columnCount, rowCount);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);

        // Inner canvas (smaller than outer)
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        innerCanvas->SetBounds({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->SetFrame({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        outerCanvas->AddChild(innerCanvas);

        // Image node
        auto imageNode = SetUpNodeBgImage(
            TEST_IMAGE_PATH, { 2 * offset, 2 * offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true);

        // Apply safe filter directly on imageNode
        ApplyFilterOrEffect2(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
    const size_t columnCount = 1;
    const size_t rowCount = 3;

    // Create 6 canvas nodes in a 2x2 layout, each with different filter/effect
    for (size_t i = 0; i < 3; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = 20;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX-offset, sizeY-offset});

        // Apply different filter/effect based on index
        ApplyFilterOrEffect3(imageNode, i);
        canvasNode->AddChild(imageNode);

        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }

    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
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
    const size_t columnCount = 1;
    const size_t rowCount = 3;

    // Create 6 canvas nodes in a 2x3 layout, each with transparent child node
    for (size_t i = 0; i < 3; i++) {
        // Level 1: CanvasNode (parent container)
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = 20;
        const size_t offset1 = 60;

        // Level 2: ImageNode (background layer with image)
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        // Level 3: ChildNode (transparent layer with filter)
        auto childNode = CreateTransparentChildNode({0, 0, sizeX - offset1, sizeY - offset1});
        
        // Apply different filter/effect based on index to transparent childNode
        ApplyFilterOrEffect3(childNode, i);
        
        // Build the hierarchy: CanvasNode -> ImageNode -> ChildNode
        imageNode->AddChild(childNode);
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
        RegisterNode(childNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_016
 * @tc.desc: test clip hole filters with larger ImageNode in render group
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_016)
{
    const size_t columnCount = 1;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + 200;
    const size_t imageSizeY = canvasSizeY + 200;
    
    for (size_t i = 0; i < 3; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, imageSizeX, imageSizeY});
        
        ApplyFilterOrEffect3(imageNode, i);
        
        canvasNode->AddChild(imageNode);
        
        RegisterNode(canvasNode);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: MARK_NODE_GROUP_017
 * @tc.desc: test 6 clip hole filters with outer-inner canvas structure
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RenderGroupTest, CONTENT_DISPLAY_TEST, MARK_NODE_GROUP_017)
{
    const size_t columnCount = 1;
    const size_t rowCount = 3;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    // 6 groups, each with different clip hole filter
    for (size_t i = 0; i < 3; i++) {
        // Outer canvas
        auto outerCanvas = CreateTestCanvasNode(i, columnCount, rowCount);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);

        // Inner canvas (smaller than outer)
        const int offset = 50;
        auto innerCanvas = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        innerCanvas->SetBounds({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->SetFrame({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        outerCanvas->AddChild(innerCanvas);

        // Image node
        auto imageNode = SetUpNodeBgImage(
            TEST_IMAGE_PATH, { 2 * offset, 2 * offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true);

        // Apply safe filter directly on imageNode
        ApplyFilterOrEffect3(imageNode, i);
        
        innerCanvas->AddChild(imageNode);
        
        RegisterNode(outerCanvas);
        RegisterNode(innerCanvas);
        RegisterNode(imageNode);
    }
    
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);
}
}

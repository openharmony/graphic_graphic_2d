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
    
    constexpr size_t DEFAULT_COLUMN_COUNT = 2;
    constexpr size_t DEFAULT_ROW_COUNT = 2;
    constexpr size_t SINGLE_COLUMN_COUNT = 1;
    constexpr size_t TRIPLE_ROW_COUNT = 3;
    constexpr size_t DEFAULT_GRID_COUNT = 4;
    constexpr size_t TRIPLE_GRID_COUNT = 3;
    
    constexpr size_t NODE_MARGIN = 20;
    constexpr size_t CHILD_NODE_MARGIN = 60;
    constexpr size_t CANVAS_INNER_OFFSET = 50;
    constexpr size_t IMAGE_SIZE_EXTENSION = 200;
    constexpr size_t MASK_RECT_START = 50;
    constexpr size_t MASK_RECT_END = 900;
    
    constexpr uint32_t COLOR_BLACK_OPAQUE = 0xff000000;
    constexpr uint32_t COLOR_WHITE_OPAQUE = 0xffffffff;
    
    constexpr float MATERIAL_BLUR_RADIUS = 20.0f;
    constexpr float MATERIAL_SATURATION = 1.0f;
    constexpr float MATERIAL_BRIGHTNESS = 1.0f;
    constexpr int MATERIAL_PARAM_ZERO = 0;
    
    constexpr int SHADOW_RADIUS = 30;
    constexpr int SHADOW_OFFSET_X = 10;
    constexpr int SHADOW_OFFSET_Y = 10;
    
    constexpr int BLUR_RADIUS_X = 20;
    constexpr int BLUR_RADIUS_Y = 20;
    
    constexpr float GRAYSCALE_HALF = 0.5f;
    constexpr float ALPHA_ONE_THIRD = 0.333f;
    constexpr float ALPHA_THREE_EIGHTHS = 0.375f;
    
    constexpr float LIGHT_UP_DEGREE = 0.3f;
    
    constexpr int WATER_RIPPLE_COUNT = 3;
    constexpr float WATER_RIPPLE_AMPLITUDE = 0.3f;
    constexpr float WATER_RIPPLE_PHASE = 0.5f;
    constexpr float WATER_RIPPLE_PROGRESS = 0.3f;
    
    constexpr float AURORA_NOISE_INTENSITY = 0.3f;
    
    constexpr float EDGE_LIGHT_ALPHA = 1.0f;
    constexpr float EDGE_LIGHT_COMPONENT_VALUE = 1.0f;
    
    constexpr float SOUND_WAVE_COLOR_PROGRESS = 0.5f;
    constexpr float SOUND_WAVE_INTENSITY = 0.7f;
    constexpr float SOUND_WAVE_ALPHA_A = 0.6f;
    constexpr float SOUND_WAVE_ALPHA_B = 0.4f;
    constexpr float SOUND_WAVE_PROGRESS_A = 0.3f;
    constexpr float SOUND_WAVE_PROGRESS_B = 0.6f;
    constexpr float SOUND_WAVE_TOTAL_ALPHA = 0.85f;
    
    constexpr float MAGNIFIER_FACTOR = 1.4f;
    constexpr float MAGNIFIER_SIZE = 500.0f;
    constexpr float MAGNIFIER_CORNER_RADIUS = 36.0f;
    constexpr float MAGNIFIER_BORDER_WIDTH = 2.0f;
    
    constexpr float CONTENT_LIGHT_INTENSITY = 10.0f;
    
    constexpr float FOREGROUND_EFFECT_RADIUS = 30.0f;
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
                    RSFilter::CreateMaterialFilter(MATERIAL_BLUR_RADIUS, MATERIAL_SATURATION, 
                        MATERIAL_BRIGHTNESS, MATERIAL_PARAM_ZERO, BLUR_COLOR_MODE::AVERAGE, true));
                break;

            case 1:
                imageNode->SetShadowColor(COLOR_BLACK_OPAQUE);
                imageNode->SetShadowRadius(SHADOW_RADIUS);
                imageNode->SetShadowOffset(SHADOW_OFFSET_X, SHADOW_OFFSET_Y);
                break;

            case 2:
                imageNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(BLUR_RADIUS_X, BLUR_RADIUS_Y));
                break;

            case 3:
                imageNode->SetGrayScale(GRAYSCALE_HALF);
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
            case 0: {
                Vector2f greyCoef = {MATERIAL_BLUR_RADIUS, MATERIAL_BLUR_RADIUS};
                node->SetGreyCoef(greyCoef);
                node->SetAlpha(ALPHA_ONE_THIRD);
                break;
            }
            
            case 1: {
                Drawing::Brush brush = Drawing::Brush();
                brush.SetColor(COLOR_WHITE_OPAQUE);
                Drawing::Path path = Drawing::Path();
                path.AddRect(Drawing::Rect(MASK_RECT_START, MASK_RECT_START, MASK_RECT_END, MASK_RECT_END));
                auto mask = RSMask::CreatePathMask(path, brush);
                node->SetMask(mask);
                break;
            }
            
            case 2: {
                float lightUpDegree = LIGHT_UP_DEGREE;
                node->SetLightUpEffectDegree(lightUpDegree);
                break;
            }
            
            case 3: {
                RSWaterRipplePara waterRipplePara = {WATER_RIPPLE_COUNT, WATER_RIPPLE_AMPLITUDE, WATER_RIPPLE_PHASE};
                float progress = WATER_RIPPLE_PROGRESS;
                node->SetWaterRippleParams(waterRipplePara, progress);
                node->SetAlpha(ALPHA_THREE_EIGHTHS);
                break;
            }
            
            default:
                break;
        }
    }

    void ApplyFilterOrEffect2(std::shared_ptr<RSCanvasNode>& imageNode, size_t index)
    {
        switch (index) {
            case 0: {
                auto auroraNoise = std::make_shared<RSNGAuroraNoise>();
                auroraNoise->Setter<AuroraNoiseNoiseTag>(AURORA_NOISE_INTENSITY);
                imageNode->SetBackgroundNGShader(auroraNoise);
                break;
            }
            case 1: {
                auto edgeLightPtr = CreateFilter(RSNGEffectType::EDGE_LIGHT);
                auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(edgeLightPtr);
                edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f(EDGE_LIGHT_COMPONENT_VALUE, 
                    EDGE_LIGHT_COMPONENT_VALUE, EDGE_LIGHT_COMPONENT_VALUE, EDGE_LIGHT_COMPONENT_VALUE));
                edgeLightFilter->Setter<EdgeLightAlphaTag>(EDGE_LIGHT_ALPHA);
                imageNode->SetBackgroundNGFilter(edgeLightFilter);
                break;
            }
            case 2: {
                auto gridWarpFilter = std::make_shared<RSNGGridWarpFilter>();
                gridWarpFilter->Setter<GridWarpGridPoint0Tag>(Vector2f(0.0f, 0.0f));
                gridWarpFilter->Setter<GridWarpGridPoint1Tag>(Vector2f(0.3f, 0.0f));
                gridWarpFilter->Setter<GridWarpGridPoint2Tag>(Vector2f(0.6f, 0.0f));
                gridWarpFilter->Setter<GridWarpGridPoint3Tag>(Vector2f(EDGE_LIGHT_COMPONENT_VALUE, 0.0f));
                gridWarpFilter->Setter<GridWarpGridPoint4Tag>(Vector2f(0.0f, 0.5f));
                gridWarpFilter->Setter<GridWarpGridPoint5Tag>(Vector2f(0.0f, 0.5f));
                gridWarpFilter->Setter<GridWarpGridPoint6Tag>(Vector2f(0.0f, 0.5f));
                gridWarpFilter->Setter<GridWarpGridPoint7Tag>(Vector2f(0.0f, 0.5f));
                imageNode->SetBackgroundNGFilter(gridWarpFilter);
                break;
            }
            case 3: {
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
            case 0: {
                auto contentLightPtr = CreateFilter(RSNGEffectType::CONTENT_LIGHT);
                auto contentLightFilter = std::static_pointer_cast<RSNGContentLightFilter>(contentLightPtr);
                contentLightFilter->Setter<ContentLightPositionTag>(Vector3f(0.0f, 0.0f, 2.0f));
                contentLightFilter->Setter<ContentLightColorTag>(Vector4f(EDGE_LIGHT_COMPONENT_VALUE, 
                    EDGE_LIGHT_COMPONENT_VALUE, EDGE_LIGHT_COMPONENT_VALUE, EDGE_LIGHT_COMPONENT_VALUE));
                contentLightFilter->Setter<ContentLightIntensityTag>(CONTENT_LIGHT_INTENSITY);
                imageNode->SetBackgroundNGFilter(contentLightFilter);
                break;
            }
            case 1: {
                auto magnifierPtr = CreateFilter(RSNGEffectType::MAGNIFIER);
                auto magnifierFilter = std::static_pointer_cast<RSNGMagnifierFilter>(magnifierPtr);
                magnifierFilter->Setter<MagnifierFactorTag>(MAGNIFIER_FACTOR);
                magnifierFilter->Setter<MagnifierWidthTag>(MAGNIFIER_SIZE);
                magnifierFilter->Setter<MagnifierHeightTag>(MAGNIFIER_SIZE);
                magnifierFilter->Setter<MagnifierCornerRadiusTag>(MAGNIFIER_CORNER_RADIUS);
                magnifierFilter->Setter<MagnifierBorderWidthTag>(MAGNIFIER_BORDER_WIDTH);
                imageNode->SetForegroundNGFilter(magnifierFilter);
                break;
            }
            case 2: {
                auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
                auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);
                soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f(EDGE_LIGHT_COMPONENT_VALUE, 0.5f, 0.0f, 0.9f));
                soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f(0.0f, 0.8f, 0.4f, 0.8f));
                soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f(0.3f, 0.3f, 0.9f, 0.7f));
                soundWaveFilter->Setter<SoundWaveColorProgressTag>(SOUND_WAVE_COLOR_PROGRESS);
                soundWaveFilter->Setter<SoundWaveIntensityTag>(SOUND_WAVE_INTENSITY);
                soundWaveFilter->Setter<SoundWaveAlphaATag>(SOUND_WAVE_ALPHA_A);
                soundWaveFilter->Setter<SoundWaveAlphaBTag>(SOUND_WAVE_ALPHA_B);
                soundWaveFilter->Setter<SoundWaveProgressATag>(SOUND_WAVE_PROGRESS_A);
                soundWaveFilter->Setter<SoundWaveProgressBTag>(SOUND_WAVE_PROGRESS_B);
                soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(SOUND_WAVE_TOTAL_ALPHA);
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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;

    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = NODE_MARGIN;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX-offset, sizeY-offset});

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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;

    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = NODE_MARGIN;
        const size_t offset1 = CHILD_NODE_MARGIN;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        auto childNode = CreateTransparentChildNode({0, 0, sizeX - offset1, sizeY - offset1});
        
        ApplyFilterOrEffect(childNode, i);
        
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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;

    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = NODE_MARGIN;
        
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        ApplySafeFilter(imageNode, i);
        
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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;

    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = NODE_MARGIN;
        const size_t offset1 = CHILD_NODE_MARGIN;
        
        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        auto childNode = CreateTransparentChildNode({0, 0, sizeX - offset1, sizeY - offset1});
        
        ApplySafeFilter(imageNode, i);
        
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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + IMAGE_SIZE_EXTENSION;
    const size_t imageSizeY = canvasSizeY + IMAGE_SIZE_EXTENSION;
    
    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + IMAGE_SIZE_EXTENSION;
    const size_t imageSizeY = canvasSizeY + IMAGE_SIZE_EXTENSION;
    
    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
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
    
    auto canvasNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    canvasNode->SetBounds({0, 0, sizeX, sizeY});
    canvasNode->SetFrame({0, 0, sizeX, sizeY});
    GetRootNode()->AddChild(canvasNode);
    
    auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
    
    imageNode->SetForegroundEffectRadius(FOREGROUND_EFFECT_RADIUS);
    
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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto outerCanvas = CreateTestCanvasNode(i, columnCount, rowCount);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);

        const int offset = CANVAS_INNER_OFFSET;
        auto innerCanvas = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        innerCanvas->SetBounds({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->SetFrame({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        outerCanvas->AddChild(innerCanvas);

        auto imageNode = SetUpNodeBgImage(
            TEST_IMAGE_PATH, { 2 * offset, 2 * offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true);

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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto outerCanvas = CreateTestCanvasNode(i, columnCount, rowCount);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);

        const int offset = CANVAS_INNER_OFFSET;
        auto innerCanvas = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        innerCanvas->SetBounds({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->SetFrame({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        outerCanvas->AddChild(innerCanvas);

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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;

    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = NODE_MARGIN;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX-offset, sizeY-offset});

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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;

    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = NODE_MARGIN;
        const size_t offset1 = CHILD_NODE_MARGIN;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        auto childNode = CreateTransparentChildNode({0, 0, sizeX - offset1, sizeY - offset1});
        
        ApplyFilterOrEffect2(childNode, i);
        
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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + IMAGE_SIZE_EXTENSION;
    const size_t imageSizeY = canvasSizeY + IMAGE_SIZE_EXTENSION;
    
    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
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
    const size_t columnCount = DEFAULT_COLUMN_COUNT;
    const size_t rowCount = DEFAULT_ROW_COUNT;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    for (size_t i = 0; i < DEFAULT_GRID_COUNT; i++) {
        auto outerCanvas = CreateTestCanvasNode(i, columnCount, rowCount);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);

        const int offset = CANVAS_INNER_OFFSET;
        auto innerCanvas = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        innerCanvas->SetBounds({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->SetFrame({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        outerCanvas->AddChild(innerCanvas);

        auto imageNode = SetUpNodeBgImage(
            TEST_IMAGE_PATH, { 2 * offset, 2 * offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true);

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
    const size_t columnCount = SINGLE_COLUMN_COUNT;
    const size_t rowCount = TRIPLE_ROW_COUNT;

    for (size_t i = 0; i < TRIPLE_GRID_COUNT; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = NODE_MARGIN;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX-offset, sizeY-offset});

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
    const size_t columnCount = SINGLE_COLUMN_COUNT;
    const size_t rowCount = TRIPLE_ROW_COUNT;

    for (size_t i = 0; i < TRIPLE_GRID_COUNT; i++) {
        auto canvasNode = CreateTestCanvasNode(i, columnCount, rowCount);
        canvasNode->MarkNodeGroup(true);
        GetRootNode()->AddChild(canvasNode);
        
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t offset = NODE_MARGIN;
        const size_t offset1 = CHILD_NODE_MARGIN;

        auto imageNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX - offset, sizeY - offset});
        
        auto childNode = CreateTransparentChildNode({0, 0, sizeX - offset1, sizeY - offset1});
        
        ApplyFilterOrEffect3(childNode, i);
        
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
    const size_t columnCount = SINGLE_COLUMN_COUNT;
    const size_t rowCount = TRIPLE_ROW_COUNT;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    const size_t imageSizeX = canvasSizeX + IMAGE_SIZE_EXTENSION;
    const size_t imageSizeY = canvasSizeY + IMAGE_SIZE_EXTENSION;
    
    for (size_t i = 0; i < TRIPLE_GRID_COUNT; i++) {
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
    const size_t columnCount = SINGLE_COLUMN_COUNT;
    const size_t rowCount = TRIPLE_ROW_COUNT;
    
    const size_t canvasSizeX = SCREEN_WIDTH / columnCount;
    const size_t canvasSizeY = SCREEN_HEIGHT / rowCount;
    
    for (size_t i = 0; i < TRIPLE_GRID_COUNT; i++) {
        auto outerCanvas = CreateTestCanvasNode(i, columnCount, rowCount);
        outerCanvas->MarkNodeGroup(true);
        GetRootNode()->AddChild(outerCanvas);

        const int offset = CANVAS_INNER_OFFSET;
        auto innerCanvas = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        innerCanvas->SetBounds({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->SetFrame({ offset, offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        outerCanvas->AddChild(innerCanvas);

        auto imageNode = SetUpNodeBgImage(
            TEST_IMAGE_PATH, { 2 * offset, 2 * offset, canvasSizeX - 2 * offset, canvasSizeY - 2 * offset });
        innerCanvas->AddChild(imageNode);
        innerCanvas->MarkNodeGroup(true);

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

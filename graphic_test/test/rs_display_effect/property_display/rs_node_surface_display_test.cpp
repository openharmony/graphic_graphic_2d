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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSNodeSurfaceDisplayTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

// Callback functions for tests
static void OnColorPicked(uint32_t color)
{
    // Color picked callback handler
}

static void OnBoundsChanged()
{
    // Bounds changed callback handler
}

static void OnAttach()
{
    // Attach callback handler
}

static void OnUpdate()
{
    // Update callback handler
}

static void OnInitType()
{
    // Init type callback handler
}

// ============================================================================
// SurfaceNode Extended Tests
// ============================================================================

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetBoundsChangedCallback_001
 * @tc.desc: Test SurfaceNode SetBoundsChangedCallback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetBoundsChangedCallback_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);
    surfaceNode->SetBoundsChangedCallback(OnBoundsChanged);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetSurfaceTexture_001
 * @tc.desc: Test SetSurfaceTexture
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetSurfaceTexture_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Set surface texture (placeholder - would need actual texture)
    surfaceNode->SetSurfaceTexture(nullptr);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetSurfaceTextureAttachCallBack_001
 * @tc.desc: Test SetSurfaceTextureAttachCallBack
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetSurfaceTextureAttachCallBack_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->SetSurfaceTextureAttachCallBack(OnAttach);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetSurfaceTextureUpdateCallBack_001
 * @tc.desc: Test SetSurfaceTextureUpdateCallBack
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetSurfaceTextureUpdateCallBack_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->SetSurfaceTextureUpdateCallBack(OnUpdate);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetSurfaceTextureInitTypeCallBack_001
 * @tc.desc: Test SetSurfaceTextureInitTypeCallBack
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetSurfaceTextureInitTypeCallBack_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->SetSurfaceTextureInitTypeCallBack(OnInitType);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetCompositeLayer_001
 * @tc.desc: Test SetCompositeLayer
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetCompositeLayer_001)
{
    std::vector<uint32_t> layerValues = { 0, 1, 2, 3 };

    for (size_t i = 0; i < layerValues.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetCompositeLayer(layerValues[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetSourceVirtualDisplayId_001
 * @tc.desc: Test SetSourceVirtualDisplayId
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetSourceVirtualDisplayId_001)
{
    std::vector<uint64_t> displayIds = { 0, 1, 100, UINT64_MAX };

    for (size_t i = 0; i < displayIds.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetSourceVirtualDisplayId(displayIds[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetRegionToBeMagnified_001
 * @tc.desc: Test SetRegionToBeMagnified
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetRegionToBeMagnified_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Set region to be magnified (x, y, width, height)
    surfaceNode->SetRegionToBeMagnified(100, 100, 200, 200);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SetContainerWindowTransparent_001
 * @tc.desc: Test SetContainerWindowTransparent
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SetContainerWindowTransparent_001)
{
    std::vector<bool> transparentValues = { true, false, true };

    for (size_t i = 0; i < transparentValues.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetContainerWindowTransparent(transparentValues[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_AttachToDisplay_001
 * @tc.desc: Test AttachToDisplay
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_AttachToDisplay_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Attach to display (using default display id)
    surfaceNode->AttachToDisplay(0);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_DetachToDisplay_001
 * @tc.desc: Test DetachToDisplay
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_DetachToDisplay_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->AttachToDisplay(0);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);

    surfaceNode->DetachToDisplay();
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_AttachToWindowContainer_001
 * @tc.desc: Test AttachToWindowContainer
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_AttachToWindowContainer_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Attach to window container (using placeholder id)
    surfaceNode->AttachToWindowContainer(0);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_DetachFromWindowContainer_001
 * @tc.desc: Test DetachFromWindowContainer
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_DetachFromWindowContainer_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->AttachToWindowContainer(0);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);

    surfaceNode->DetachFromWindowContainer();
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_TextureExport_Toggle_001
 * @tc.desc: Test SurfaceNode SetTextureExport toggle (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_TextureExport_Toggle_001)
{
    std::vector<bool> exportStates = { true, false };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < exportStates.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto surfaceNode = RSSurfaceNode::Create();
            surfaceNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            surfaceNode->SetBackgroundColor(0xffff0000);
            surfaceNode->SetAlpha(alphaList[col]);
            surfaceNode->SetTextureExport(exportStates[row]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);
        }
    }
}

// ============================================================================
// DisplayNode Extended Tests
// ============================================================================

/*
 * @tc.name: RSNodeSurfaceDisplayTest_Display_ClearChildren_001
 * @tc.desc: Test DisplayNode ClearChildren
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_Display_ClearChildren_001)
{
    auto displayNode = RSDisplayNode::Create();
    displayNode->SetBounds({ 50, 50, 1100, 1900 });
    displayNode->SetBackgroundColor(0xffff0000);

    for (int i = 0; i < 3; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + i * 350, 50, 300, 1800 });
        child->SetBackgroundColor(0xff00ff00);
        displayNode->AddChild(child);
    }

    GetRootNode()->AddChild(displayNode);
    RegisterNode(displayNode);
    displayNode->ClearChildren();
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_Display_MultipleDisplays_001
 * @tc.desc: Test multiple DisplayNode scenarios
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_Display_MultipleDisplays_001)
{
    for (int i = 0; i < 2; i++) {
        auto displayNode = RSDisplayNode::Create();
        displayNode->SetBounds({ 50 + i * 600, 50, 550, 1900 });
        displayNode->SetBackgroundColor(0xffff0000 - i * 0x0000ff00);
        displayNode->AddDisplayNodeToTree();
        GetRootNode()->AddChild(displayNode);
        RegisterNode(displayNode);
    }
}

// ============================================================================
// Complex Surface-Display Integration Tests
// ============================================================================

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SurfaceDisplay_Hierarchy_001
 * @tc.desc: Test SurfaceNode-DisplayNode hierarchy
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SurfaceDisplay_Hierarchy_001)
{
    auto displayNode = RSDisplayNode::Create();
    displayNode->SetBounds({ 50, 50, 1100, 1900 });
    displayNode->SetBackgroundColor(0xffff0000);

    for (int i = 0; i < 2; i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ 50 + i * 550, 50, 500, 1800 });
        surfaceNode->SetBackgroundColor(0xff00ff00);
        surfaceNode->AttachToDisplay(i);
        displayNode->AddChild(surfaceNode);
    }

    GetRootNode()->AddChild(displayNode);
    RegisterNode(displayNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SurfaceWithCallbacks_001
 * @tc.desc: Test SurfaceNode with multiple callbacks
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SurfaceWithCallbacks_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 600, 600 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Set all callbacks
    surfaceNode->SetBoundsChangedCallback(OnBoundsChanged);
    surfaceNode->SetSurfaceTextureAttachCallBack(OnAttach);
    surfaceNode->SetSurfaceTextureUpdateCallBack(OnUpdate);
    surfaceNode->SetSurfaceTextureInitTypeCallBack(OnInitType);

    // Set texture export
    surfaceNode->SetTextureExport(true);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SurfaceBoundaryTests_001
 * @tc.desc: Test SurfaceNode boundary conditions (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SurfaceBoundaryTests_001)
{
    std::vector<Vector4f> boundsList = {
        { 50, 50, 0, 100 },
        { 150, 50, 100, 0 },
        { 300, 50, 0, 0 },
        { 50, 200, -100, 100 },
        { 200, 200, 100, -100 },
        { 400, 200, -100, -100 },
        { 50, 400, 50, 50 },
        { 200, 400, 1000, 1000 },
        { 700, 400, 500, 500 }
    };

    for (size_t i = 0; i < boundsList.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        auto bounds = boundsList[i];
        surfaceNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetTextureExport(i % 2 == 0);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_SurfaceRotationScale_001
 * @tc.desc: Test SurfaceNode with rotation and scale (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_SurfaceRotationScale_001)
{
    std::vector<float> rotationList = { 0, 45, 90 };
    std::vector<std::pair<float, float>> scaleList = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f }
    };

    for (size_t row = 0; row < rotationList.size(); row++) {
        for (size_t col = 0; col < scaleList.size(); col++) {
            auto surfaceNode = RSSurfaceNode::Create();
            surfaceNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            surfaceNode->SetBackgroundColor(0xffff0000);
            surfaceNode->SetRotation(rotationList[row]);
            surfaceNode->SetScale(scaleList[col].first, scaleList[col].second);
            surfaceNode->SetTextureExport(true);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);
        }
    }
}

// ============================================================================
// ColorPicker Extended Tests (颜色选择器扩展测试)
// ============================================================================

/*
 * @tc.name: RSNodeSurfaceDisplayTest_ColorPicker_Callbacks_001
 * @tc.desc: Test SetColorPickerCallback and HasColorPickerCallback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_ColorPicker_Callbacks_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Check without callback
    bool hasCallback1 = testNode->HasColorPickerCallback();

    // Register callback
    testNode->RegisterColorPickerCallback(OnColorPicked);

    // Check with callback
    bool hasCallback2 = testNode->HasColorPickerCallback();

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Unregister callback
    testNode->UnregisterColorPickerCallback();

    // Check after unregister
    bool hasCallback3 = testNode->HasColorPickerCallback();
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_ColorPicker_ParamsMatrix_001
 * @tc.desc: Test SetColorPickerParams with matrix layout (3x3)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_ColorPicker_ParamsMatrix_001)
{
    std::vector<int> strategies = { 0, 1, 2 };
    std::vector<float> intervals = { 0.1f, 0.5f, 1.0f };

    for (size_t row = 0; row < strategies.size(); row++) {
        for (size_t col = 0; col < intervals.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetColorPickerParams(strategies[row], intervals[col], 0.1f);
            testNode->RegisterColorPickerCallback(OnColorPicked);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

// ============================================================================
// Material and Effect Extended Tests (材质和效果扩展测试)
// ============================================================================

/*
 * @tc.name: RSNodeSurfaceDisplayTest_MaterialQuality_Matrix_001
 * @tc.desc: Test SetMaterialWithQualityLevel with effects (3x4)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_MaterialQuality_Matrix_001)
{
    std::vector<int> qualityLevels = { 1, 2, 3 };
    std::vector<float> shadowRadius = { 0, 10, 20, 30 };

    for (size_t row = 0; row < qualityLevels.size(); row++) {
        for (size_t col = 0; col < shadowRadius.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 290 + 50, (int)row * 350 + 50, 250, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetShadowRadius(shadowRadius[col]);
            testNode->SetMaterialWithQualityLevel(qualityLevels[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

// ============================================================================
// Advanced Scenario Tests (高级场景测试)
// ============================================================================

/*
 * @tc.name: RSNodeSurfaceDisplayTest_ComplexSurfaceDisplay_001
 * @tc.desc: Test complex Surface-Display integration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_ComplexSurfaceDisplay_001)
{
    auto displayNode = RSDisplayNode::Create();
    displayNode->SetBounds({ 50, 50, 1100, 1900 });
    displayNode->SetBackgroundColor(0xffff0000);

    // Create multiple surfaces attached to display
    for (int i = 0; i < 3; i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ 50 + i * 360, 50, 340, 1800 });
        surfaceNode->SetBackgroundColor(0xff00ff00 - i * 0x0000ff00);
        surfaceNode->AttachToDisplay(i);
        surfaceNode->SetTextureExport(true);
        surfaceNode->SetCompositeLayer(i);
        surfaceNode->SetBoundsChangedCallback(OnBoundsChanged);

        displayNode->AddChild(surfaceNode);
    }

    GetRootNode()->AddChild(displayNode);
    RegisterNode(displayNode);
}

/*
 * @tc.name: RSNodeSurfaceDisplayTest_AllSurfaceCallbacks_001
 * @tc.desc: Test SurfaceNode with all callbacks and properties
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeSurfaceDisplayTest, CONTENT_DISPLAY_TEST, RSNodeSurfaceDisplayTest_AllSurfaceCallbacks_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 1100, 1800 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Set all callbacks
    surfaceNode->SetBoundsChangedCallback(OnBoundsChanged);
    surfaceNode->SetSurfaceTextureAttachCallBack(OnAttach);
    surfaceNode->SetSurfaceTextureUpdateCallBack(OnUpdate);
    surfaceNode->SetSurfaceTextureInitTypeCallBack(OnInitType);

    // Set all properties
    surfaceNode->SetTextureExport(true);
    surfaceNode->SetCompositeLayer(1);
    surfaceNode->SetSourceVirtualDisplayId(100);
    surfaceNode->SetContainerWindowTransparent(true);

    // Set transformation
    surfaceNode->SetRotation(45.0f);
    surfaceNode->SetScale(1.2f, 1.2f);
    surfaceNode->SetAlpha(0.8f);

    surfaceNode->AttachToDisplay(0);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

} // namespace OHOS::Rosen

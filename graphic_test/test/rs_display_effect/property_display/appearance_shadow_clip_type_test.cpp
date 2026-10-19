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
 
#include "parameters_defination.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
 
class ShadowClipTypeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
 
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};
 
// Shadow clip type with radius/elevation/filled/path shadow, BOUNDS_SHAPE vs DEFAULT_SHAPE
GRAPHIC_TEST(ShadowClipTypeTest, CONTENT_DISPLAY_TEST, Shadow_ClipType_Normal_Test_1)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;
 
    // 0: DEFAULT_SHAPE (clip by path), 1: BOUNDS_SHAPE (clip by bounds rrect)
    int clipTypeList[] = { 0, 1 };
 
    // Row 0-1: radius shadow (MaskFilter path) with different radii
    float radiusList[] = { 50, 100 };
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBackgroundColor(0xffc0c0c0);
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowRadius(radiusList[i % TWO_]);
        testNode->SetShadowClipType(clipTypeList[i / TWO_]);
        testNode->SetCornerRadius(50);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
 
    // Row 2: elevation shadow, BOUNDS_SHAPE vs DEFAULT_SHAPE
    for (int i = 0; i < TWO_; i++) {
        int x = i * nodePos;
        int y = 2 * nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBackgroundColor(0xffc0c0c0);
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowElevation(30);
        testNode->SetShadowOffset(20, 20);
        testNode->SetShadowClipType(clipTypeList[i]);
        testNode->SetCornerRadius(50);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
 
    // Row 3: isFilled=true (clip should not be applied), BOUNDS_SHAPE vs DEFAULT_SHAPE
    for (int i = 0; i < TWO_; i++) {
        int x = i * nodePos;
        int y = 3 * nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBackgroundColor(0xffc0c0c0);
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowRadius(50);
        testNode->SetShadowIsFilled(true);
        testNode->SetShadowClipType(clipTypeList[i]);
        testNode->SetCornerRadius(50);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
 
    // Row 4: shadow path, BOUNDS_SHAPE vs DEFAULT_SHAPE
    Drawing::Path drawingPath;
    drawingPath.AddCircle(160, 260, 245);
    for (int i = 0; i < TWO_; i++) {
        int x = i * nodePos;
        int y = 4 * nodePos;
        auto shadowPath = RSPath::CreateRSPath(drawingPath);
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBackgroundColor(0xffc0c0c0);
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowRadius(30);
        testNode->SetShadowPath(shadowPath);
        testNode->SetShadowClipType(clipTypeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
 
// Shadow clip type BOUNDS_SHAPE with different corner radius values
GRAPHIC_TEST(ShadowClipTypeTest, CONTENT_DISPLAY_TEST, Shadow_ClipType_CornerRadius_Test_1)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;
 
    std::vector<Vector4f> radiusList = {
        { 0, 0, 0, 0 },
        { 50, 50, 50, 50 },
        { 100, 100, 100, 100 },
        { 200, 100, 50, 200 }
    };
 
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBackgroundColor(0xffc0c0c0);
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowRadius(50);
        testNode->SetShadowClipType(1);
        testNode->SetCornerRadius(radiusList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen
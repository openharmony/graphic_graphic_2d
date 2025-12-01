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
#include <filesystem>
#include "display_manager.h"

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"

#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_CYAN = 0xFF00FFFF;
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
constexpr int ABILITY_BGALPHA = 255;
} //namespace

class DirtyRegionTest07 : public RSGraphicTest {
public:
    const Vector2f screenSize = GetScreenSize();
    // called before each tests
    void BeforeEach() override
    {
        SetSurfaceBounds({ 0, 0, screenSize.x_, screenSize.y_ });
        GetRootNode()->testSurfaceNodes_.back()->SetAbilityBGAlpha(ABILITY_BGALPHA);
    }

    void TestCaseCapture()
    {
        auto pixelMap =
            DisplayManager::GetInstance().GetScreenshot(DisplayManager::GetInstance().GetDefaultDisplayId());
        if (pixelMap) {
            pixelMap->crop({ 0, 0, 1200, 2000 });
            const ::testing::TestInfo* const testInfo =
            ::testing::UnitTest::GetInstance()->current_test_info();
            std::string fileName = "/data/local/graphic_test/rs_perform_feature/dirty_region/";
            namespace fs = std::filesystem;
            if (!fs::exists(fileName)) {
                if (!fs::create_directories(fileName)) {
                    std::cout << "create dir failed" << std::endl;
                    return;
                }
            } else {
                if (!fs::is_directory(fileName)) {
                    std::cout << "path is not dir" << std::endl;
                    return;
                }
            }
            fileName += testInfo->test_case_name() + std::string("_");
            fileName += testInfo->name() + std::string(".png");
            if (!WriteToPngWithPixelMap(fileName, *pixelMap)) {
                // NOT MODIFY THE COMMENTS
                std::cout << "[   FAILED   ] " << fileName << std::endl;
                return;
            }
            std::cout << "png write to " << fileName << std::endl;
        } else {
            // NOT MODIFY THE COMMENTS
            std::cout << "[   FAILED   ] " << "image is null" << std::endl;
        }
    }

    void DoAnimation(std::shared_ptr<RSCanvasNode>& canvasNode, Vector2f translate, int duration = 500)
    {
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(duration);
        auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
        RSNode::Animate(protocol, timingCurve, [&]() {
            canvasNode->SetTranslate(translate);
        }, []() {
            std::cout << "animation finish callback" << std::endl;
        });
    }
};

/*
 * @tc.name: SubTreeSkip01
 * @tc.desc: test canvas remove sub canvasNodes with weight = 0, height = 0
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip01)
{
    // bounds arrangement simulating the scenario of China Media Group's website
    Vector4f bounds = { 0, 2188, 1316, 210 };
    Vector4f bounds2 = { 0, 0, 1316, 210 };

    auto canvasNode1 = RSCanvasNode::Create();
    RegisterNode(canvasNode1);
    canvasNode1->SetBounds(bounds);
    canvasNode1->SetFrame(bounds);
    canvasNode1->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds3);
    backgroundNode->SetFrame(bounds3);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds4 = { 100, 1500, 200, 200 };
    auto imgNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds4);
    RegisterNode(imgNode);

    auto canvasNode2 = RSCanvasNode::Create();
    RegisterNode(canvasNode2);
    canvasNode2->SetBounds(bounds2);
    canvasNode2->SetFrame(bounds2);
    canvasNode2->SetTranslate({ 0, 0 });

    Vector4f bounds5 = { 800, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds5);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate3 = { -800, 0 };
    int duration = 10000;
    DoAnimation(animateNode, translate3, duration);

    auto pureCanvas = RSCanvasNode::Create();
    RegisterNode(pureCanvas);
    pureCanvas->SetBounds(bounds2);
    pureCanvas->SetFrame(bounds2);

    Vector4f bounds6 = { 0, 0, 200, 200 };
    auto canvasNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds6);
    RegisterNode(canvasNode3);
    canvasNode3->SetTranslate({ 0, 0 });

    auto canvasNode4 = RSCanvasNode::Create();
    RegisterNode(canvasNode4);
    canvasNode4->SetBounds(bounds2);
    canvasNode4->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(imgNode);
    backgroundNode->RSNode::AddChild(canvasNode1);
    canvasNode1->RSNode::AddChild(canvasNode2);
    canvasNode2->RSNode::AddChild(pureCanvas);
    canvasNode2->RSNode::AddChild(animateNode);
    pureCanvas->RSNode::AddChild(canvasNode4);
    pureCanvas->RSNode::AddChild(canvasNode3);

    Vector2f translate = { 0, -1419 };
    DoAnimation(canvasNode2, translate);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    Vector2f translate2 = { 800, 0 };
    DoAnimation(canvasNode3, translate2);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    Vector4f bounds7 = { 0, 2398, 0, 0 };
    canvasNode1->SetBounds(bounds7);
    canvasNode1->SetFrame(bounds7);
    pureCanvas->RemoveChild(canvasNode4);
    canvasNode2->RemoveChild(pureCanvas);
    canvasNode1->RemoveChild(canvasNode2);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip02
 * @tc.desc: test sub canvasNode with parent alpha 1.0f to 0.1f
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip02)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    float alpha = 0.1f;
    parentCanvasNode->SetAlpha(alpha);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip03
 * @tc.desc: test sub canvasNode with parent visible true to false
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip03)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    parentCanvasNode->SetVisible(false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip04
 * @tc.desc: test sub canvasNode with parent visible false to true
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip04)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetVisible(false);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    parentCanvasNode->SetVisible(true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip05
 * @tc.desc: test sub canvasNode with parent clipToBounds false to true
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip05)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    parentCanvasNode->SetClipToBounds(true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip06
 * @tc.desc: test sub canvasNode with parent clipToBounds true to false
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip06)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetClipToBounds(true);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    parentCanvasNode->SetClipToBounds(false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip07
 * @tc.desc: test sub canvasNode with parent clipToFrame false to true
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip07)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetFrame(bounds2);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    parentCanvasNode->SetClipToFrame(true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip08
 * @tc.desc: test sub canvasNode with parent clipToFrame true to false
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip08)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetFrame(bounds2);
    parentCanvasNode->SetClipToFrame(true);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    parentCanvasNode->SetClipToFrame(false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip09
 * @tc.desc: test sub canvasNode with parent clipPath large to small
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip09)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetFrame(bounds2);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    Drawing::Path drawingPath;
    drawingPath.AddRect(0, 0, 400, 600, Drawing::PathDirection::CW_DIRECTION);
    auto clipPath = RSPath::CreateRSPath(drawingPath);
    parentCanvasNode->SetClipBounds(clipPath);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip10
 * @tc.desc: test sub canvasNode with parent clipPath small to large
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip10)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetFrame(bounds2);
    Drawing::Path drawingPath;
    drawingPath.AddRect(0, 0, 400, 600, Drawing::PathDirection::CW_DIRECTION);
    auto clipPath = RSPath::CreateRSPath(drawingPath);
    parentCanvasNode->SetClipBounds(clipPath);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    Drawing::Path drawingPath2;
    drawingPath2.AddRect(0, 0, 800, 800, Drawing::PathDirection::CW_DIRECTION);
    auto clipPath2 = RSPath::CreateRSPath(drawingPath2);
    parentCanvasNode->SetClipBounds(clipPath2);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip11
 * @tc.desc: test sub canvasNode with parent clipRRect large to small
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip11)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetFrame(bounds2);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    Vector4f rect = { 0, 0, 400, 400 };
    Vector4f radius = { 100.0, 100.0, 100.0, 100.0 };
    parentCanvasNode->SetClipRRect(rect, radius);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip12
 * @tc.desc: test sub canvasNode with parent clipRRect small to large
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip12)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetFrame(bounds2);
    Vector4f rect = { 0, 0, 400, 400 };
    Vector4f radius = { 100.0, 100.0, 100.0, 100.0 };
    parentCanvasNode->SetClipRRect(rect, radius);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    Vector4f rect2 = { 0, 0, 800, 800 };
    Vector4f radius2 = { 50.0, 50.0, 50.0, 50.0 };
    parentCanvasNode->SetClipRRect(rect2, radius2);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: SubTreeSkip13
 * @tc.desc: test sub canvasNode with parent alpha 0.1f to 1.0f
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(DirtyRegionTest07, CONTENT_DISPLAY_TEST, SubTreeSkip13)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    float alpha = 0.1f;
    parentCanvasNode->SetAlpha(alpha);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    float alpha2 = 1.0f;
    parentCanvasNode->SetAlpha(alpha2);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}
} // namespace OHOS::Rosen
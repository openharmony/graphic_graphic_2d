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
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
constexpr float DEFAULT_BACKROUND_RADIUS = 10.0f;

Vector2f DEFAULT_TRANSLATE = { 600, 0 };
Vector4f DEFAULT_BOUNDS = { 0, 100, 400, 400 };
} //namespace

class DirtyRegionTest05 : public RSGraphicTest {
public:
    const Vector2f screenSize = GetScreenSize();
    // called before each tests
    void BeforeEach() override
    {
        SetSurfaceBounds({ 0, 0, screenSize.x_, screenSize.y_ });
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

    void DoAnimation(std::shared_ptr<RSCanvasNode>& canvasNode, Vector2f translate)
    {
        RSAnimationTimingProtocol protocol;
        int duration = 500; // the animation lasts for 0.5 second
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
 * @tc.name: SubSurface01
 * @tc.desc: test sub surface case
 * @tc.type: FUNC
 * @tc.require: issueICV2KE
 */
GRAPHIC_N_TEST(DirtyRegionTest05, CONTENT_DISPLAY_TEST, SubSurface01)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig1;
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig2;
    rsSurfaceNodeConfig1.SurfaceNodeName = "Surface";
    rsSurfaceNodeConfig1.isSync = false;
    rsSurfaceNodeConfig2.SurfaceNodeName = "SubSurface";
    rsSurfaceNodeConfig2.isSync = false;
    RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNode1 = RSSurfaceNode::Create(rsSurfaceNodeConfig1, rsSurfaceNodeType);
    RegisterNode(surfaceNode1);
    surfaceNode1->SetBounds(bounds);
    surfaceNode1->SetBackgroundColor(COLOR_RED);
    surfaceNode1->SetAbilityBGAlpha(255);

    auto surfaceNode2 = RSSurfaceNode::Create(rsSurfaceNodeConfig2, rsSurfaceNodeType);
    RegisterNode(surfaceNode2);
    surfaceNode2->SetBounds(bounds);

    auto filterNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds);
    RegisterNode(filterNode);
    filterNode->SetBackgroundBlurRadius(DEFAULT_BACKROUND_RADIUS);

    auto normalNode = RSCanvasNode::Create();
    RegisterNode(normalNode);
    normalNode->SetBounds(DEFAULT_BOUNDS);
    normalNode->SetBackgroundColor(COLOR_BLUE);
    normalNode->SetTranslate({ 0, 0 });
    DoAnimation(normalNode, DEFAULT_TRANSLATE);

    GetRootNode()->SetTestSurface(surfaceNode1);
    surfaceNode1->RSNode::AddChild(surfaceNode2);
    surfaceNode2->RSNode::AddChild(filterNode);
    surfaceNode2->RSNode::AddChild(normalNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

} // namespace OHOS::Rosen
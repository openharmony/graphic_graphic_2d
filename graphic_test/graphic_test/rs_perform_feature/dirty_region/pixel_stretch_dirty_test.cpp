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
    constexpr uint32_t COLOR_RED = 0xFFFF0000;
    constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;

    Vector2f DEFAULT_TRANSLATE = { 600, 0 };
    Vector4f DEFAULT_SMALL_BOUNDS = { 0, 100, 400, 400 };
    Vector4f DEFAULT_LARGE_BOUNDS = { 200, 200, 1000, 1000 };
    Vector4f DEFAULT_PIXEL_STRETCH_BOUNDS = { -50, -50, -50, -50 };
} //namespace

class DirtyRegionTest06 : public RSGraphicTest {
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
        pixelMap->crop({ 0, 0, 1200, 2000 });
        if (pixelMap) {
            const ::testing::TestInfo* const testInfo =
                ::testing::UnitTest::GetInstance()->current_test_info();
            std::string fileName = "/data/local/graphic_test/rs_perform_feature/dirty_region/";
            namespace fs = std::filesystem;
            if (!fs::exists(fileName)) {
                fs::create_directories(fileName);
                if (!std::filesystem::exists(fileName)) {
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
        int duration = 3000; // the animation lasts for 3 seconds
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
 * @tc.name: PixelStretch01
 * @tc.desc: test pixel stretch case
 * @tc.type: FUNC
 * @tc.require: issue20109
 */
GRAPHIC_N_TEST(DirtyRegionTest06, CONTENT_DISPLAY_TEST, PixelStretch01)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_LARGE_BOUNDS);
    RegisterNode(testNode);
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH_BOUNDS, Drawing::TileMode::CLAMP);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_SMALL_BOUNDS);
    testNode2->SetBackgroundColor(COLOR_RED);
    testNode2->SetTranslate({ 0, 0 });
    DoAnimation(testNode2, DEFAULT_TRANSLATE);
    auto rootNode = GetRootNode();
    if (rootNode != nullptr) {
        rootNode->AddChild(testNode);
        rootNode->AddChild(testNode2);
    }
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}
} // namespace OHOS::Rosen
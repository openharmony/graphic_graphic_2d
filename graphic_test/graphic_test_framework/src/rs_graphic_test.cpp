/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "rs_graphic_test_utils.h"
#include "rs_parameter_parse.h"
#include "rs_trace.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace std;
namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SURFACE_COLOR = 0xffffffff;
constexpr int64_t LOAD_TREE_WAIT_TIME = 3;
constexpr int PLAYBACK_PREPARE_WAIT_TIME = 5000;

bool ShouldRunCurrentTest()
{
    const ::testing::TestInfo* const testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
    const auto& extInfo = ::OHOS::Rosen::TestDefManager::Instance().GetTestInfo(
        testInfo->test_case_name(), testInfo->name());
    const auto& params = RSParameterParse::Instance();
    if (!extInfo) {
        LOGE("RSGraphicTest no testinfo %{public}s-%{public}s",
            testInfo->test_case_name(), testInfo->name());
        return false;
    }
    if (!params.filterTestTypes.empty() && params.filterTestTypes.count(extInfo->testType) == 0) {
        return false;
    }

    if (params.runTestMode != RSGraphicTestMode::ALL && extInfo->testMode != params.runTestMode) {
        return false;
    }

    return true;
}
}

uint32_t RSGraphicTest::imageWriteId_ = 0;

void RSGraphicTest::SetUpTestCase()
{
    imageWriteId_ = 0;
}

void RSGraphicTest::TearDownTestCase()
{
    return;
}

UIPoint RSGraphicTest::GetScreenCapacity(const string testCase)
{
    int testCnt = ::OHOS::Rosen::TestDefManager::Instance().GetTestCaseCnt(testCase);
    int cl = 1;
    int num = 1;
    while (num < testCnt) {
        cl++;
        num = cl * cl;
    }
    int rl = ceil(static_cast<double>(testCnt) / cl);
    return {cl, rl};
}
      
UIPoint RSGraphicTest::GetPos(int id, int cl)
{
    if (cl > 0) {
        int x = id % cl;
        int y = id / cl;
        return {x, y};
    }

    return {0, 0};
}

void RSGraphicTest::SetUp()
{
    shouldRunTest_ = ShouldRunCurrentTest();
    if (!shouldRunTest_) {
        GTEST_SKIP();
        return;
    }

    RSSurfaceNodeConfig config;
    string surfaceNodeName = "TestSurface";
    config.SurfaceNodeName = surfaceNodeName.append(to_string(imageWriteId_));
    auto testSurface = RSSurfaceNode::Create(config, false);

    testSurface->SetBounds({0, 0, GetScreenSize()[0], GetScreenSize()[1]});
    testSurface->SetFrame({0, 0, GetScreenSize()[0], GetScreenSize()[1]});
    testSurface->SetBackgroundColor(SURFACE_COLOR);
    GetRootNode()->SetTestSurface(testSurface);

    BeforeEach();

    const ::testing::TestInfo* const testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
    const auto& extInfo = ::OHOS::Rosen::TestDefManager::Instance().GetTestInfo(
        testInfo->test_case_name(), testInfo->name());
    if (extInfo == nullptr) {
        return;
    }
    auto size = GetScreenSize();
    cout << "SetUp:size:" << size.x_ << "*" << size.y_ << endl;
    if (!extInfo->isMultiple) {
        cout << "SetUp:isMultiple is false" << endl;
        SetScreenSurfaceBounds({0, 0, size.x_, size.y_});
        return;
    }

    cout << "SetUp:isMultiple is true" << endl;
    auto capacity = GetScreenCapacity(string(testInfo->test_case_name()));
    cout << "SetUp:capacity:" << capacity.x_ << "*" << capacity.y_ << endl;
    if (imageWriteId_ == 0) {
        SetScreenSurfaceBounds({0, 0, capacity.x_*size.x_, capacity.y_*size.y_});
        cout << "ScreenSurfaceBounds:[" << capacity.x_*size.x_ << "*" << capacity.y_*size.y_ << "]" << endl;
    }
    auto pos = GetPos(extInfo->testId, capacity.x_);
    cout << "pos:id:" << imageWriteId_ << "[" << pos.x_ << "." << pos.y_ << "]" << endl;

    testSurface->SetBounds({pos.x_*size.x_, pos.y_*size.y_, size.x_, size.y_});
    testSurface->SetFrame({pos.x_*size.x_, pos.y_*size.y_, size.x_, size.y_});
}

bool RSGraphicTest::WaitOtherTest()
{
    if (IsSingleTest()) {
        return false;
    }
    const ::testing::TestInfo* const testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
    const auto& extInfo = ::OHOS::Rosen::TestDefManager::Instance().GetTestInfo(
        testInfo->test_case_name(), testInfo->name());
    int testCaseCnt = ::OHOS::Rosen::TestDefManager::Instance().GetTestCaseCnt(
        string(testInfo->test_case_name()));
    if (extInfo == nullptr) {
        return true;
    }
    if (!extInfo->isMultiple) {
        return false;
    }
    if (++imageWriteId_ < testCaseCnt) {
        return true;
    }
    return false;
}

void RSGraphicTest::TestCaseCapture(bool isScreenshot)
{
    const ::testing::TestInfo* const testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
    const auto& extInfo = ::OHOS::Rosen::TestDefManager::Instance().GetTestInfo(
        testInfo->test_case_name(), testInfo->name());

    auto pixelMap = RSGraphicTestDirector::Instance().TakeScreenCaptureAndWait(
        RSParameterParse::Instance().surfaceCaptureWaitTime, isScreenshot);
    if (pixelMap) {
        std::string filename = GetImageSavePath(extInfo->filePath);
        filename += testInfo->test_case_name();
        if (!extInfo->isMultiple) {
            filename += std::string("_") + testInfo->name();
        }
        filename += std::string(".png");
        if (std::filesystem::exists(filename)) {
            LOGW("RSGraphicTest file exists %{public}s", filename.c_str());
        }
        if (!WriteToPngWithPixelMap(filename, *pixelMap)) {
            LOGE("RSGraphicTest::TearDown write image failed %{public}s-%{public}s",
                testInfo->test_case_name(), testInfo->name());
        }
        std::cout << "png write to " << filename << std::endl;
    }
}

void RSGraphicTest::TearDown()
{
    if (!shouldRunTest_ || RSParameterParse::Instance().skipCapture_) {
        GetRootNode()->ResetTestSurface();
        return;
    }

    const ::testing::TestInfo* const testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
    const auto& extInfo = ::OHOS::Rosen::TestDefManager::Instance().GetTestInfo(
        testInfo->test_case_name(), testInfo->name());

    if (WaitOtherTest()) {
        return;
    }

    StartUIAnimation();
    RSGraphicTestDirector::Instance().FlushMessage();
    WaitTimeout(RSParameterParse::Instance().testCaseWaitTime);

    bool isManualTest = false;
    if (extInfo) {
        isManualTest = (extInfo->testMode == RSGraphicTestMode::MANUAL);
    } else {
        LOGE("RSGraphicTest no testinfo %{public}s-%{public}s", testInfo->test_case_name(), testInfo->name());
        return;
    }

    bool isDynamicTest = extInfo->testMode == RSGraphicTestMode::DYNAMIC;
    if (isManualTest) {
        WaitTimeout(RSParameterParse::Instance().manualTestWaitTime);
    } else {
        TestCaseCapture(isDynamicTest);
    }

    AfterEach();
    WaitTimeout(RSParameterParse::Instance().testCaseWaitTime);

    if (isDynamicTest) {
        PlaybackStop();
    }

    GetRootNode()->ResetTestSurface();
    RSGraphicTestDirector::Instance().SendProfilerCommand("rssubtree_clear");
    RSGraphicTestDirector::Instance().FlushMessage();
    WaitTimeout(RSParameterParse::Instance().testCaseWaitTime);
}

void RSGraphicTest::RegisterNode(std::shared_ptr<RSNode> node)
{
    nodes_.push_back(node);
    GetRootNode()->RegisterNode(node);
}

void RSGraphicTest::AddFileRenderNodeTreeToNode(std::shared_ptr<RSNode> node, const std::string& filePath)
{
    if (!shouldRunTest_ || RSParameterParse::Instance().skipCapture_) {
        return;
    }
    //need flush client node to rs firstly
    RSGraphicTestDirector::Instance().FlushMessage();
    WaitTimeout(RSParameterParse::Instance().testCaseWaitTime);

    std::cout << "load subbtree to node file path is " << filePath << std::endl;
    std::string command = "rssubtree_load " + std::to_string(node->GetId()) + " " + filePath;
    RSGraphicTestDirector::Instance().SendProfilerCommand(command);
    RSGraphicTestDirector::Instance().FlushMessage();
    WaitTimeout(LOAD_TREE_WAIT_TIME * RSParameterParse::Instance().testCaseWaitTime);
}

void RSGraphicTest::PlaybackRecover(const std::string& filePath, float pauseTimeStamp)
{
    if (!shouldRunTest_ || RSParameterParse::Instance().skipCapture_) {
        return;
    }
    // playback prepare
    int64_t pid = 0;
    std::string command =
        "rsrecord_replay_prepare " + std::to_string(pid) + " " + std::to_string(pauseTimeStamp) + " " + filePath;
    std::cout << "Playback Prepare: " << command << std::endl;
    RSGraphicTestDirector::Instance().SendProfilerCommand(command);
    WaitTimeout(PLAYBACK_PREPARE_WAIT_TIME);

    // playback start
    command = "rsrecord_replay";
    std::cout << "Playback Start: " << command << std::endl;
    RSGraphicTestDirector::Instance().SendProfilerCommand(command);
    WaitTimeout(static_cast<int>(pauseTimeStamp * UNIT_SEC_TO_MS) + RSParameterParse::Instance().testCaseWaitTime);
}

void RSGraphicTest::PlaybackStop()
{
    // playback stop
    std::string command = "rsrecord_replay_stop";
    std::cout << "Playback Stop: " << command << std::endl;
    RSGraphicTestDirector::Instance().SendProfilerCommand(command);
}

std::shared_ptr<RSGraphicRootNode> RSGraphicTest::GetRootNode() const
{
    return RSGraphicTestDirector::Instance().GetRootNode();
}

Vector2f RSGraphicTest::GetScreenSize() const
{
    return RSGraphicTestDirector::Instance().GetScreenSize();
}

void RSGraphicTest::SetSurfaceBounds(const Vector4f& bounds)
{
    RSGraphicTestDirector::Instance().SetSurfaceBounds(bounds);
}

void RSGraphicTest::SetScreenSurfaceBounds(const Vector4f& bounds)
{
    RSGraphicTestDirector::Instance().SetScreenSurfaceBounds(bounds);
}

void RSGraphicTest::SetSurfaceColor(const RSColor& color)
{
    RSGraphicTestDirector::Instance().SetSurfaceColor(color);
}

void RSGraphicTest::SetScreenSize(float width, float height)
{
    RSGraphicTestDirector::Instance().SetScreenSize(width, height);
}

bool RSGraphicTest::IsSingleTest()
{
    return RSGraphicTestDirector::Instance().IsSingleTest();
}

std::string RSGraphicTest::GetImageSavePath(const std::string path)
{
    std::string imagePath = "/data/local/";
    size_t posCnt = path.rfind("/") + 1;
    std::string subPath = path.substr(0, posCnt);
    imagePath.append(subPath);

    namespace fs = std::filesystem;
    if (!fs::exists(imagePath)) {
        if (!fs::create_directories(imagePath)) {
            LOGE("RSGraphicTestDirector create dir failed");
        }
    } else {
        if (!fs::is_directory(imagePath)) {
            LOGE("RSGraphicTestDirector path is not dir");
        }
    }

    return imagePath;
}

void RSGraphicTest::StartUIAnimation()
{
    RSGraphicTestDirector::Instance().StartRunUIAnimation();
}
} // namespace Rosen
} // namespace OHOS
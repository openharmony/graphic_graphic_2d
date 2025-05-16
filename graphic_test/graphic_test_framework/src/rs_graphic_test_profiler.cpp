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

#include "rs_graphic_test_profiler.h"

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
constexpr uint32_t SURFACE_COLOR = 0xffffffff;
constexpr int FLUSH_RS_WAIT_TIME = 500;
constexpr int LOAD_STATIC_WAIT_TIME = 3000;
constexpr int NORMAL_WAIT_TIME = 1000;
constexpr int SCREEN_WIDTH = 1316;
constexpr int SCREEN_HEIGHT = 2832;
const std::string SAVE_IMAGE_PATH_NAME = "ScreenShot";

int RSGraphicTestProfiler::RunNodeTreeTest(const std::string& path)
{
    if (!std::filesystem::exists(path)) {
        std::cout << "root path is not exist :" << path << std::endl;
        return 0;
    }
    NodeTreeTestSetUp();

    runTestCaseNum_ = 0;
    rootPath_ = path;
    std::filesystem::path imagePath = GetImageSavePath();
    for (const auto& entry : std::filesystem::directory_iterator(rootPath_)) {
        const std::string fileName = entry.path().filename();
        if (fileName == SAVE_IMAGE_PATH_NAME) {
            continue;
        }
        std::filesystem::path target = imagePath / fileName;
        if (entry.is_directory()) {
            std::filesystem::create_directories(target);
            CopyDirectoryAndLoadNodeTreeFile(entry.path(), target);
        } else {
            LoadNodeTreeProfilerFile(entry.path(), target);
        }
    }
    // NOT MODIFY THE COMMENTS
    cout << "[   PASSED   ] " << runTestCaseNum_ << " test." << std::endl;
    return 1;
}

void RSGraphicTestProfiler::CopyDirectoryAndLoadNodeTreeFile(
    const std::filesystem::path& src, const std::filesystem::path& dest)
{
    for (const auto& entry : std::filesystem::directory_iterator(src)) {
        std::filesystem::path target = dest / entry.path().filename();
        if (entry.is_directory()) {
            std::filesystem::create_directories(target);
            CopyDirectoryAndLoadNodeTreeFile(entry.path(), target);
        } else {
            LoadNodeTreeProfilerFile(entry.path(), target);
        }
    }
}

std::shared_ptr<RSGraphicRootNode> RSGraphicTestProfiler::GetRootNode() const
{
    return RSGraphicTestDirector::Instance().GetRootNode();
}

Vector2f RSGraphicTestProfiler::GetScreenSize() const
{
    return RSGraphicTestDirector::Instance().GetScreenSize();
}

void RSGraphicTestProfiler::NodeTreeTestSetUp()
{
    system("setenforce 0");
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config, false);

    testSurface->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    testSurface->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    testSurface->SetBackgroundColor(SURFACE_COLOR);
    GetRootNode()->SetTestSurface(testSurface);

    RSGraphicTestDirector::Instance().SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    RSGraphicTestDirector::Instance().SetScreenSurfaceBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
}

void RSGraphicTestProfiler::LoadNodeTreeProfilerFile(const std::string& filePath, const std::string& savePath)
{
    runTestCaseNum_++;
    // NOT MODIFY THE COMMENTS
    cout << "[   RUN   ] " << filePath << std::endl;
    // 1.add load client node to add file 
    auto loadNode = RSCanvasNode::Create();
    loadNode->SetBounds({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
    GetRootNode()->AddChild(loadNode);
    // need flush client node to rs firstly
    RSGraphicTestDirector::Instance().FlushMessage();
    WaitTimeout(FLUSH_RS_WAIT_TIME);

    // 2.send load command
    std::cout << "load subbtree to node file path is " << filePath << std::endl;
    std::string command = "rssubtree_load " + std::to_string(loadNode->GetId()) + " " + filePath;
    RSGraphicTestDirector::Instance().SendProfilerCommand(command);
    WaitTimeout(LOAD_STATIC_WAIT_TIME);

    // 3.testcase capture
    WaitTimeout(NORMAL_WAIT_TIME);
    TestCaseCapture(false, savePath);
    
    // 4.clear
    GetRootNode()->RemoveChild(loadNode);
    RSGraphicTestDirector::Instance().SendProfilerCommand("rssubtree_clear");
    RSGraphicTestDirector::Instance().FlushMessage();
    WaitTimeout(NORMAL_WAIT_TIME);
}

void RSGraphicTestProfiler::TestCaseCapture(bool isScreenshot, const std::string& savePath)
{
    auto pixelMap = RSGraphicTestDirector::Instance().TakeScreenCaptureAndWait(
        RSParameterParse::Instance().surfaceCaptureWaitTime, isScreenshot);
    if (pixelMap) {
        std::string filename;
        size_t lastDotPos = savePath.find_last_of(".");
        if (lastDotPos == std::string::npos || lastDotPos == 0) {
            filename = savePath;
        } else {
            filename = savePath.substr(0, lastDotPos);
        }
        filename += std::string(".png");
        if (std::filesystem::exists(filename)) {
            cout << "image has already exist " << filename << std::endl;
        }
        if (!WriteToPngWithPixelMap(filename, *pixelMap)) {
            // NOT MODIFY THE COMMENTS
            cout << "[   FAILED   ] " << filename << std::endl;
        }
        std::cout << "png write to " << filename << std::endl;
        // NOT MODIFY THE COMMENTS
        cout << "[   OK   ] " << filename << std::endl;
    }
}

std::string RSGraphicTestProfiler::GetImageSavePath()
{
    std::filesystem::path rootPath(rootPath_);
    std::filesystem::path savePath = rootPath / SAVE_IMAGE_PATH_NAME;
    if (!std::filesystem::exists(savePath)) {
        if (!std::filesystem::create_directories(savePath)) {
            std::cout << "create image directories failed: " << savePath << std::endl;
        }
    } else {
        if (!std::filesystem::is_directory(savePath)) {
            std::cout << "image directories failed is not directory: " << savePath << std::endl;
        }
    }
    return savePath.string();
}
} // namespace Rosen
} // namespace OHOS
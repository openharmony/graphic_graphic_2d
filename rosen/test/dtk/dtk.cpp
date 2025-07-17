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

#include <dlfcn.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <message_parcel.h>
#include <surface.h>
#include "transaction/rs_transaction.h"
#include "wm/window.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"
#include "utils/log.h"
#include "dtk_test_register.h"
#include "dtk_test_utils.h"
#ifdef USE_M133_SKIA
#include "include/core/SkBitmap.h"
#include "include/core/SkStream.h"
#endif

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

shared_ptr<RSNode> rootNode;
shared_ptr<RSCanvasNode> canvasNode;
void Init(shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    cout << "[INFO] rs pixelmap demo Init Rosen Backend!" << endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);

    rootNode->SetBackgroundColor(Drawing::Color::COLOR_WHITE);

    rsUiDirector->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    canvasNode = RSCanvasNode::Create();
    canvasNode->SetFrame(0, 0, width, height);
    rootNode->AddChild(canvasNode, -1);
}

void ExportSkImage(sk_sp<SkImage>& image, const std::string& fileName)
{
    std::string dir("/data/local/tmp/dtk_output");
    if (access(dir.c_str(), F_OK) < 0) {
        return;
    }
    if (access(dir.c_str(), W_OK) < 0) {
        return;
    }

#ifdef USE_M133_SKIA
    sk_sp<SkData> snapshot = image->refEncodedData();
#else
    sk_sp<SkData> snapshot = image->encodeToData();
#endif
    std::string dumpFileName = dir + fileName + ".jpg";

    SkFILEWStream outStream(dumpFileName.c_str());
    auto img = snapshot;
    if (img == nullptr) {
        return;
    }
    outStream.write(img->bytes(), img->size());
    outStream.flush();
    cout << "[INFO] pixelmap write to jpg sucess" << endl;
}

void OnPixelMapCapture(std::shared_ptr<Media::PixelMap> pixelmap, std::string fileName)
{
    if (pixelmap == nullptr) {
        cout << "[ERROR] faild to get pixelmap, return nullptr!" << endl;
        return;
    }

    SkBitmap bitmap;
    auto skii = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(), SkColorType::kRGBA_8888_SkColorType,
        SkAlphaType::kPremul_SkAlphaType);
    if (!bitmap.installPixels(skii, (void*)pixelmap->GetPixels(), skii.minRowBytes64())) {
        std::cout << __func__ << "installPixels failed" << std::endl;
        return;
    }
    sk_sp<SkImage> image = bitmap.asImage();
    ExportSkImage(image, fileName);
}

void OnImageCapture(std::shared_ptr<Drawing::Image> image, std::string fileName)
{
    if (image == nullptr) {
        cout << "[ERROR] failed to get image, return nullptr" << endl;
        return;
    }

    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL};
    Drawing::Bitmap bitmap;
    bitmap.Build(image->GetWidth(), image->GetHeight(), format);
    image->ReadPixels(bitmap, 0, 0);
    SkBitmap skbitmap;
    auto skii = SkImageInfo::Make(image->GetWidth(), image->GetHeight(),
        SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kPremul_SkAlphaType);
    if (!skbitmap.installPixels(skii, bitmap.GetPixels(), skii.minRowBytes())) {
        std::cout << __func__ << "installPixels failed" << std::endl;
        return;
    }
    sk_sp<SkImage> skimage = skbitmap.asImage();
    ExportSkImage(skimage, fileName);
}

void SkipInitial(const std::shared_ptr<TestBase>& initCase, const std::shared_ptr<Drawing::Canvas>& initCanvas,
    const std::shared_ptr<Drawing::GPUContext>& initgpuCtx, const bool& isRunCase)
{
    // Render 10 frames to complete initialization before capturing the flame graph
    for (int i = 0; i < 10; i++) {
        initCase->SetCanvas(initCanvas.get());
        initCase->Recording();
        initgpuCtx->FlushAndSubmit(isRunCase);
    }
}

void runWithWindow(std::string testCaseName, std::function<std::shared_ptr<TestBase>()> creator, int testCount,
                   int frame, bool noDump)
{
    string demoName = "dtk_" + testCaseName;
    RSSystemProperties::GetUniRenderEnabled();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_POINTER);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, 1334, 2772 });
    auto window = Window::Create(demoName, option);

    window->Show();
    usleep(30000); // sleep 30000 microsecond
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        cout << "[ERROR] dtk create window failed: " << rect.width_ << rect.height_ << endl;
        window->Hide();
        window->Destroy();
        window = Window::Create(demoName, option);
        window->Show();
        usleep(30000); // sleep 30000 microsecond
        rect = window->GetRect();
    }
    cout << "[INFO] dtk create window success: " << rect.width_ << " " << rect.height_ << endl;
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    usleep(100000); // sleep 100000 microsecond
    
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    usleep(100000); // sleep 100000 microsecond
    cout << "[INFO] BeginRecording" << endl;

    auto testCase = creator();
    testCase->SetTestCount(testCount);
    for (int i = 0; i < frame; i++) {
        auto canvas = canvasNode->BeginRecording(rect.width_, rect.height_);
        testCase->SetCanvas((TestPlaybackCanvas*)(canvas));
        testCase->Recording();
        canvasNode->FinishRecording();
        rsUiDirector->SendMessages();
        usleep(16666); // sleep 16666 microsecond
    }
    usleep(50000); // sleep 50000 microsecond
    cout << "[INFO] FinishRecording" << endl;
    if (!noDump) {
        OnPixelMapCapture(window->Snapshot(), testCaseName);
    }
    window->Hide();
    window->Destroy();
}

void runSingleTestCase(std::string testCaseName, std::function<std::shared_ptr<TestBase>()> creator,
                       int testCount, int frame, bool offscreen, bool noDump, bool skipInitial, bool runLoadCase)
{
    cout << "[INFO]testCase--" << testCaseName << "-- starts! " << endl;
    runWithWindow(testCaseName, creator, testCount, frame, noDump);
    cout << "[INFO] rs draw demo end!" << endl;
}

bool getTestLevel(std::string fullTestCaseName, int& currentLevel)
{
    size_t pos1 = fullTestCaseName.find_last_of('_');
    if (std::string::npos == pos1) {
        return false;
    }
    size_t pos2 = fullTestCaseName.find_last_of('_', pos1 - 1);
    if (std::string::npos == pos2) {
        return false;
    }
    std::string keyLevelStr = fullTestCaseName.substr(pos2 + 2, pos1 - pos2 - 1);
    currentLevel = std::stoi(keyLevelStr);
    return true;
}

void printNames(bool isFullname)
{
    std::set<std::string> names;
    auto testCaseMap = DtkTestRegister::Instance()->getTestCaseMap();
    for (const auto& [key, value] : testCaseMap) {
        if (isFullname) {
            names.insert(key);
        } else {
            size_t pos1 = key.find_last_of('_');
            if (std::string::npos == pos1) {
                continue;
            }
            size_t pos2 = key.find_last_of('_', pos1 - 1);
            if (std::string::npos == pos2) {
                continue;
            }
            names.insert(key.substr(0, pos2));
        }
    }

    for (auto& name : names) {
        cout << name << endl;
    }
}

int findIntParam(unordered_map<string, int>& params, char* argv[], string op1, string op2,
                 int defaultValue)
{
    if (params.find(op1) != params.end()) {
        int i = params.find(op1)->second + 1;
        return stoi(argv[i]);
    } else if (params.find(op2) != params.end()) {
        int i = params.find(op2)->second + 1;
        return stoi(argv[i]);
    } else {
        return defaultValue;
    }
}

void dumpCovData(const std::string& soPath)
{
    using FuncType = void();
    auto handle = dlopen(soPath.c_str(), RTLD_NOW);
    void* func = nullptr;
    if (!handle) {
        cout << "[ERROR] Cannot find " << soPath << endl;
    } else {
        func = dlsym(handle, "__gcov_dump");
    }
    if (func) {
        reinterpret_cast<FuncType*>(func)();
        cout << "[INFO] Run __gcov_dump in " << soPath << "successfully" << endl;
    } else {
        cout << "[ERROR] Cannot find __gcov_dump in " << soPath << endl;
    }
}

const string DTK_HELP_LIST_STR = R"(
Usage: dtk {testCaseName} [OPTIONS...]
Examples:
  dtk drawrect
  dtk drawline -testLevel 0 -testIndex 0
  dtk drawrect -c 1 -f 1 -l 0 -i 0
  dtk drawrect --offscreen --noDump --delay
OPTIONS:
  -l, -testLevel: Test Level. Run all test levels if it is -1 or not set.
  -i, -testIndex: Test index. Run all test indexes if it is -1 or not set.
  -c, -testCount: Test count in a single frame. It needs to be supported by test case itself.
                  It will be 1 if it is not set.
  -f, -frame:     Test frames that affact test duration. It will be 1 if it is not set.
  -h, --help:     Give this help list)
  names:          Print all test case name.
  fullnames:      Print all test case name with testLevel and test indexs.
  offscreen:      It will not be shown on the screen.
  noDumnp:        It will not dump image(jpg) in /data/local/tmp/dtk_output/
  skipInitial:    It will Skip Initialization to capture a flame graph, It can only take effect in off-screen mode
  delay:          Delay 2s before running.
  all:            Run all test cases.
  coverage:       Manually call __gcov_dump before exit 0
  load:           Run CPU and Gpu load cases
)";

int main(int argc, char* argv[])
{
    if (argc < 2) { // should input more than 2 args
        cout << "[ERROR] args are too less, please try again!" << endl;
        return 0;
    }
    unordered_map<string, int> params;
    for (int i = 1; i < argc; ++i) {
        params.emplace(argv[i], i);
    }

    if (params.find("-h") != params.end() || params.find("--help") != params.end()) {
        cout << DTK_HELP_LIST_STR << endl;
        return 0;
    } else if (params.find("--names") != params.end()) {
        printNames(false);
        return 0;
    } else if (params.find("--fullnames") != params.end()) {
        printNames(true);
        return 0;
    }

    string testCaseName = argv[1];
    bool offscreen = params.find("--offscreen") != params.end();
    bool noDump = params.find("--noDump") != params.end();
    bool delay = params.find("--delay") != params.end();
    bool runAll = params.find("--all") != params.end();
    bool runLoadCase = params.find("--load") != params.end();
    bool enableCoverage = params.find("--coverage") != params.end();
    bool skipInitial = params.find("--skipInitial") != params.end();
    int testCount = findIntParam(params, argv, "-c", "-testCount", 1);
    int frame = findIntParam(params, argv, "-f", "-frame", 1);
    int testLevel = findIntParam(params, argv, "-l", "-testLevel", -1);
    int index = findIntParam(params, argv, "-i", "-testIndex", -1);

    if (delay) {
        sleep(2); // 预留 2s
    }

    auto testCaseMap = DtkTestRegister::Instance()->getTestCaseMap();
    bool testCaseExits = false;
    if (runAll) {
        for (const auto& [key, value] : testCaseMap) {
            testCaseExits = true;
            if (testLevel == -1) {
                // run all testcases on all testlevels and all testcaseIndexes
                runSingleTestCase(key, value, testCount, frame, offscreen, noDump, skipInitial, runLoadCase);
                continue;
            }
            int currentLevel;
            if (getTestLevel(key, currentLevel) && testLevel == currentLevel) {
                // run all testCaseIndexes of all testcases on a specific testLevel
                runSingleTestCase(key, value, testCount, frame, offscreen, noDump, skipInitial, runLoadCase);
            }
        }
    } else if (testLevel == -1) {
        // run all testlevels of a specific testcase
        for (const auto& [key, value] : testCaseMap) {
            if (key.rfind(testCaseName, 0) == 0) {
                testCaseExits = true;
                runSingleTestCase(key, value, testCount, frame, offscreen, noDump, skipInitial, runLoadCase);
            }
        }
    } else if (index == -1) {
        // run all testCaseIndexes on a specific testLevel of a specific testcase
        for (const auto& [key, value] : testCaseMap) {
            string prefix = testCaseName + "_L" + to_string(testLevel);
            if (key.rfind(prefix, 0) == 0) {
                testCaseExits = true;
                runSingleTestCase(key, value, testCount, frame, offscreen, noDump, skipInitial, runLoadCase);
            }
        }
    } else {
        // run a specific testCaseIndex on a specific testLevel of a specific testcase
        string fullTestCaseName = testCaseName + "_L" + to_string(testLevel) +
            "_" + to_string(index);
        if (testCaseMap.find(fullTestCaseName) != testCaseMap.end()) {
            testCaseExits = true;
            auto creator = testCaseMap.find(fullTestCaseName)->second;
            runSingleTestCase(fullTestCaseName, creator, testCount, frame, offscreen, noDump, skipInitial, runLoadCase);
        }
    }
    if (!testCaseExits) {
        cout << "[ERROR] testCaseName(or testlevel or testCaseIndex) not exists" << endl;
    }

    if (enableCoverage) {
        // 覆盖率文件导出到此路径
        setenv("GCOV_PREFIX", "/data/service/el0/render_services", 1);
        dumpCovData("/system/lib64/libddgr.z.so");
        dumpCovData("/system/lib64/libddgr_compiler.z.so");
        dumpCovData("/system/lib64/lib2d_graphics.z.so");
    }

    return 0;
}
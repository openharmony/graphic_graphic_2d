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

#include <charconv>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;
namespace OHOS {
namespace Rosen {
constexpr uint32_t SURFACE_COLOR = 0xffffffff;
constexpr int FLUSH_RS_WAIT_TIME = 500;
constexpr int LOAD_STATIC_WAIT_TIME = 3000;
constexpr int NORMAL_WAIT_TIME = 1000;
constexpr int PLAYBACK_PREPARE_OUT_TIME = 10000;
constexpr int PLAYBACK_PAUSE_OUT_TIME = 5000;
constexpr int PLAYBACK_OPERATE_INTERVAL_TIME = 100;
constexpr int SCREEN_WIDTH = 1316;
constexpr int SCREEN_HEIGHT = 2832;
constexpr int FRAME_WIDTH_NUM = 6;
const std::string SAVE_IMAGE_PATH_NAME = "ScreenShot";
const std::string OHR_CONFIG_FILE_NAME = "ohr_config.json";
//json config
const std::string OHR_LIST = "ohr_list";
const std::string OHR_NAME = "fileName";
const std::string OHR_START_TIME = "startTime";
const std::string OHR_END_TIME = "endTime";
const std::string OHR_TIME_INTERVAL = "timeInterval";
constexpr int OHR_INFO_NUM = 4;
//dump buffer
constexpr uint32_t CROP_X = 0;
constexpr uint32_t CROP_Y = 0;
constexpr uint32_t ALIGNMENT = 256;
constexpr uint32_t RGBA8888_FORMAT = 12;

void SafeSystem(const std::string& cmd, const std::string& errorMsg)
{
    int result = std::system(cmd.c_str());
    if (result != 0) {
        std::cout << "SafeSystem Error:" << errorMsg << "(code:" << result << ")"<< std::endl;
    }
}

void RSGraphicTestProfiler::SetUseBufferDump(bool useBufferDump)
{
    useBufferDump_ = useBufferDump;
}

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
    TearDown();
    // NOT MODIFY THE COMMENTS
    cout << "[   PASSED   ] " << runTestCaseNum_ << " test." << std::endl;
    return 1;
}

std::string MakePlaybackSaveDirectories(const std::string& path)
{
    size_t lastSlashPos = path.find_last_of('/');
    std::string fileNameExt = path.substr(lastSlashPos + 1);

    size_t dotPos = fileNameExt.find('.');
    std::string fileName = fileNameExt.substr(0, dotPos);

    return path.substr(0, lastSlashPos + 1) + fileName + "/" + fileNameExt;
}

void RSGraphicTestProfiler::AnalysePlaybackInfo(
    const std::filesystem::path& rootPath, const std::filesystem::path& imagePath, const cJSON* root)
{
    cJSON* item = root->child;
    PlaybackInfo info;
    int checkNum = 0;
    while (item != nullptr) {
        if (strcmp(item->string, OHR_NAME.c_str()) == 0 && cJSON_IsString(item)) {
            info.fileName = item->valuestring;
            checkNum++;
        } else if (strcmp(item->string, OHR_START_TIME.c_str()) == 0 && cJSON_IsNumber(item)) {
            info.startTime = item->valueint;
            checkNum++;
        } else if (strcmp(item->string, OHR_END_TIME.c_str()) == 0 && cJSON_IsNumber(item)) {
            info.endTime = item->valueint;
            checkNum++;
        } else if (strcmp(item->string, OHR_TIME_INTERVAL.c_str()) == 0 && cJSON_IsNumber(item)) {
            info.timeInterval = item->valueint;
            checkNum++;
        }
        item = item->next;
    }
    if (checkNum != OHR_INFO_NUM) {
        std::cout << "playback info param num error!" << std::endl;
        return;
    }
    std::filesystem::path filePath = rootPath / info.fileName;
    std::filesystem::path savePath = imagePath / MakePlaybackSaveDirectories(info.fileName);
    if (!std::filesystem::exists(filePath)) {
        std::cout << "playback file is not exist:" << filePath << std::endl;
        return;
    }
    if (savePath.has_parent_path()) {
        std::filesystem::create_directories(savePath.parent_path());
    }
    LoadPlaybackProfilerFile(filePath, savePath, info);
}

cJSON* ParseFileConfig(const std::string& path)
{
    std::ifstream configFile;
    configFile.open(path);
    std::stringstream configStream;
    configStream << configFile.rdbuf();
    configFile.close();
    std::string configString = configStream.str();

    cJSON* rootData = cJSON_Parse(configString.c_str());
    return rootData;
}

int RSGraphicTestProfiler::RunPlaybackTest(const std::string& filePath)
{
    if (!std::filesystem::exists(filePath)) {
        std::cout << "filePath is not exist :" << filePath << std::endl;
        return 0;
    }

    PlaybackTestSetUp();
    runTestCaseNum_ = 0;
    rootPath_ = filePath;
    std::filesystem::path rootPath = rootPath_;
    std::filesystem::path imagePath = GetImageSavePath();

    std::filesystem::path configPath = rootPath / OHR_CONFIG_FILE_NAME;
    if (!std::filesystem::exists(configPath)) {
        std::cout << "ohr config is not exist :" << configPath << std::endl;
        return 0;
    }

    cJSON* rootData = ParseFileConfig(configPath);
    if (rootData == nullptr) {
        cJSON_Delete(rootData);
        std::cout << "parse config file failed, check it path is:" << configPath << std::endl;
        return 0;
    }
    auto playbackConfig = cJSON_GetObjectItem(rootData, OHR_LIST.c_str());
    if (cJSON_IsArray(playbackConfig)) {
        int listSize = cJSON_GetArraySize(playbackConfig);
        for (int i = 0; i < listSize; i++) {
            cJSON* item = cJSON_GetArrayItem(playbackConfig, i);
            if (cJSON_IsObject(item)) {
                AnalysePlaybackInfo(rootPath, imagePath, item);
            }
        }
    }
    cJSON_Delete(rootData);
    TearDown();
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
    if (useBufferDump_) {
        DumpBufferSetUp();
    }
}

void RSGraphicTestProfiler::PlaybackTestSetUp()
{
    system("setenforce 0");
    if (useBufferDump_) {
        DumpBufferSetUp();
    }
}

void RSGraphicTestProfiler::TearDown()
{
    if (useBufferDump_) {
        DumpBufferTearDown();
    }
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
    if (useBufferDump_) {
        TestCaseBufferDump(true, savePath);
    } else {
        TestCaseCapture(false, savePath);
    }

    // 4.clear
    GetRootNode()->RemoveChild(loadNode);
    RSGraphicTestDirector::Instance().SendProfilerCommand("rssubtree_clear");
    RSGraphicTestDirector::Instance().FlushMessage();
    WaitTimeout(NORMAL_WAIT_TIME);
}

void RSGraphicTestProfiler::LoadPlaybackProfilerFile(
    const std::string& filePath, const std::string& savePath, PlaybackInfo info)
{
    runTestCaseNum_++;
    // NOT MODIFY THE COMMENTS
    cout << "[   RUN   ] " << filePath << std::endl;

    // playback prepare
    int64_t pid = 0;
    float startTime = static_cast<float>(info.startTime) / static_cast<float>(UNIT_SEC_TO_MS);
    std::string command =
        "rsrecord_replay_prepare " + std::to_string(pid) + " " + std::to_string(startTime) + " " + filePath;
    std::cout << "Playback Prepare: " << command << std::endl;
    RSGraphicTestDirector::Instance().SendProfilerCommand(command, PLAYBACK_PREPARE_OUT_TIME);
    WaitTimeout(PLAYBACK_OPERATE_INTERVAL_TIME);

    // playback start
    command = "rsrecord_replay";
    std::cout << "Playback Start: " << command << std::endl;
    RSGraphicTestDirector::Instance().SendProfilerCommand(command);
    WaitTimeout(info.startTime + NORMAL_WAIT_TIME);

    int frame = 1;
    for (int time = info.startTime; time <= info.endTime; time += info.timeInterval) {
        if (frame != 1) {
            float pauseTime = static_cast<float>(time) / static_cast<float>(UNIT_SEC_TO_MS);
            command = "rsrecord_pause_at " +  std::to_string(pauseTime);
            std::cout << "Playback Pause At: " << command << std::endl;
            RSGraphicTestDirector::Instance().SendProfilerCommand(command, PLAYBACK_PAUSE_OUT_TIME);
        }

        WaitTimeout(NORMAL_WAIT_TIME);
        std::string filename;
        size_t lastDotPos = savePath.find_last_of(".");
        if (lastDotPos == std::string::npos || lastDotPos == 0) {
            filename = savePath;
        } else {
            filename = savePath.substr(0, lastDotPos);
        }
        std::ostringstream frameStr;
        frameStr << std::setw(FRAME_WIDTH_NUM) << setfill('0') << frame;
        filename = filename + "_frame_" + frameStr.str() + "_" + std::to_string(time);
        if (useBufferDump_) {
            TestCaseBufferDump(true, filename);
        } else {
            TestCaseCapture(true, filename);
        }
        frame++;
    }
    // playback stop
    command = "rsrecord_replay_stop";
    std::cout << "Playback Stop: " << command << std::endl;
    RSGraphicTestDirector::Instance().SendProfilerCommand(command);
    command = "rssubtree_clear";
    RSGraphicTestDirector::Instance().SendProfilerCommand(command);
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
            return;
        }
        std::cout << "png write to " << filename << std::endl;
        // NOT MODIFY THE COMMENTS
        cout << "[   OK   ] " << filename << std::endl;
    } else {
        // NOT MODIFY THE COMMENTS
        cout << "[   FAILED   ] " << savePath << std::endl;
    }
}

size_t FindDigitStart(const std::string& str, size_t pos)
{
    while (pos > 0 && std::isdigit(str[pos - 1])) {
        --pos;
    }
    return pos;
}

size_t FindDigitEnd(const std::string& str, size_t pos)
{
    while (pos < str.size() && std::isdigit(str[pos])) {
        ++pos;
    }
    return pos;
}

constexpr size_t AlignValue(size_t value)
{
    return (value + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

bool GetDumpBufferSizeFromPath(const std::string& srcFileName, Size& dumpBufferSize)
{
    size_t posX = srcFileName.find_last_of('x');
    if (posX == std::string::npos || posX == 0) {
        std::cout << "Invaild filename format:missing 'x'" << std::endl;
        return false;
    }

    size_t widthStart = FindDigitStart(srcFileName, posX);
    size_t heightEnd = FindDigitEnd(srcFileName, posX + 1);

    std::string_view widthStr(&srcFileName[widthStart], posX - widthStart);
    std::string_view heightStr(&srcFileName[posX + 1], heightEnd - (posX + 1));

    if (auto [ptr, ec] = std::from_chars(widthStr.begin(), widthStr.end(), dumpBufferSize.width);
        ec != std::errc()) {
        std::cout << "Invaild width format" << std::endl;
        return false;
    }

    if (auto [ptr, ec] = std::from_chars(heightStr.begin(), heightStr.end(), dumpBufferSize.height);
        ec != std::errc()) {
        std::cout << "Invaild height format" << std::endl;
        return false;
    }
    return true;
}

bool CheckCropIsValid(Size dumpBufferSize, Vector2f screenSize)
{
    if (CROP_X < 0 || CROP_Y < 0 ||
        CROP_X + screenSize[0] > dumpBufferSize.width ||
        CROP_Y + screenSize[1] > dumpBufferSize.height) {
        std::cout << "Invalid crop size, trun to get the screen buffer" << std::endl;
        return false;
    }
    return true;
}

void RSGraphicTestProfiler::TestCaseBufferDump(bool isScreenshot, const std::string& savePath)
{
    SafeSystem("hidumper -s 10 -a buffer > null", "Hidumper command failed");

    std::string filename;
    size_t lastDotPos = savePath.find_last_of(".");
    if (lastDotPos == std::string::npos || lastDotPos == 0) {
        filename = savePath;
    } else {
        filename = savePath.substr(0, lastDotPos);
    }

    std::filesystem::path dstFileName = filename;
    for (const auto& entry : std::filesystem::directory_iterator("/data")) {
        if (entry.path().filename().string().find("bq_") == 0 &&
            entry.path().filename().string().find("_RCD") != std::string::npos) {
            std::filesystem::remove(entry.path());
        }
    }

    bool hasDumpFile = false;
    std::filesystem::create_directories(dstFileName.parent_path());
    for (const auto& entry : std::filesystem::directory_iterator("/data")) {
        if (entry.path().filename().string().find("bq_") == 0 &&
            entry.path().filename().string() != "bq_dump") {
            hasDumpFile = true;
            Size dumpBufferSize;
            GetDumpBufferSizeFromPath(entry.path(), dumpBufferSize);
            if (isScreenshot || !CheckCropIsValid(dumpBufferSize, GetScreenSize())) {
                dstFileName += "_" + std::to_string(RGBA8888_FORMAT) + "_" + std::to_string(dumpBufferSize.width) +
                    "x" + std::to_string(dumpBufferSize.height) + ".raw";
                std::filesystem::rename(entry.path(), dstFileName);
                break;
            }
            dstFileName += "_" + std::to_string(RGBA8888_FORMAT) + "_" +
                std::to_string(static_cast<int>(GetScreenSize()[0])) +
                "x" + std::to_string(static_cast<int>(GetScreenSize()[1])) + ".raw";
            if (!CropRawFile(entry.path().filename().string(), dstFileName, dumpBufferSize)) {
                std::cout << "CropRawFile fail" << std::endl;
                break;
            }
            std::filesystem::remove(entry.path());
            break;
        }
    }
    if (hasDumpFile) {
        std::cout << "dumpbuffer write to " << dstFileName << std::endl;
        // NOT MODIFY THE COMMENTS
        cout << "[   OK   ] " << dstFileName << std::endl;
    } else {
        // NOT MODIFY THE COMMENTS
        cout << "[   FAILED   ] " << dstFileName << std::endl;
    }
}

bool RSGraphicTestProfiler::CropRawFile(
    const std::string& srcFileName, const std::string& dstFileName, Size dumpBufferSize)
{
    char srcFilePath[PATH_MAX] = {0};
    if (srcFileName.length() <= 0 || srcFileName.length() > PATH_MAX || !realpath(srcFileName.c_str(), srcFilePath)) {
        std::cout << "Raw src file path is invalid :" << srcFileName << std::endl;
        return false;
    }
    std::ifstream input(srcFilePath, std::ios::binary);
    if (!input) {
        std::cout << "Failed to open input file" << std::endl;
        return false;
    }
    std::filesystem::path dstFilePath = dstFileName;
    char srcFileParentPath[PATH_MAX] = {0};
    if (!realpath(dstFilePath.parent_path().c_str(), srcFileParentPath)) {
        std::cout << "Raw dst file path is invalid :" << dstFilePath << std::endl;
        return false;
    }
    std::ofstream output(dstFilePath, std::ios::binary);
    const int bytesPerPixel = 4; //RGBA
    size_t stride = AlignValue(dumpBufferSize.width * bytesPerPixel);
    size_t cropRowSize = GetScreenSize()[0] * bytesPerPixel;
    size_t startOffset = CROP_Y * stride + CROP_X * bytesPerPixel;
    input.seekg(startOffset, std::ios::beg);
    for (int row = 0; row < GetScreenSize()[1]; ++row) {
        std::vector<char> buffer(cropRowSize);
        input.read(buffer.data(), cropRowSize);
        output.write(buffer.data(), cropRowSize);
        //Skip the rest of the line in the original image
        input.seekg(stride - (CROP_X + GetScreenSize()[0]) * bytesPerPixel, std::ios::cur);
    }
    return true;
}

void RSGraphicTestProfiler::DumpBufferSetUp()
{
    const std::string initCommands =
        "setenforce 0; "
        "param set debug.dumpstaticframe.enabled 1; "
        "param set rosen.afbc.enabled 0; "
        "param set rosen.hebc.enabled 0; "
        "touch /data/bq_dump";
    SafeSystem(initCommands, "Initialization commands failed");
}

void RSGraphicTestProfiler::DumpBufferTearDown()
{
    std::filesystem::remove("/data/bq_dump");
    const std::string cleanupCommands =
        "setenforce 1; "
        "param set debug.dumpstaticframe.enabled 0; "
        "param set rosen.afbc.enabled 1; "
        "param set rosen.hebc.enabled 1;";
    SafeSystem(cleanupCommands, "cleanup commands failed");
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
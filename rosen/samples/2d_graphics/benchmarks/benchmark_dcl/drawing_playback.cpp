/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "drawing_playback.h"
#include <sstream>
#include <fcntl.h>
#include <thread>
#include <unique_fd.h>

#include "include/core/SkStream.h"
#include "skia_adapter/skia_canvas.h"
#include "src/utils/SkMultiPictureDocumentPriv.h"
#include "src/utils/SkOSPath.h"
#include "transaction/rs_marshalling_helper.h"
#include "recording/draw_cmd_list.h"
#include "platform/common/rs_system_properties.h"

#include <fstream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include "directory_ex.h"

namespace {
bool IsValidFile(const std::string& realPathStr, const std::string& validFile = "/data/")
{
    return realPathStr.find(validFile) == 0;
}

std::string GetRealAndValidPath(const std::string& filePath)
{
    std::string realPathStr;
    if (!PathToRealPath(filePath, realPathStr)) {
        std::cout << "FileUtils: The file path is not valid!" << std::endl;
        return "";
    }
    if (IsValidFile(realPathStr)) {
        return realPathStr;
    } else {
        std::cout << "FileUtils: The file path is not valid!" << std::endl;
        return "";
    }
}

bool CreateFile(const std::string& filePath)
{
    std::string realPath = GetRealAndValidPath(filePath);
    if (realPath.empty()) {
        return false;
    }
    std::ofstream outFile(realPath);
    if (!outFile.is_open()) {
        std::cout << "FileUtils: file open failed!" << std::endl;
        return false;
    }
    outFile.clear();
    outFile.close();
    return true;
}

bool WriteStringToFilefd(int fd, const std::string& str)
{
    const char* p = str.data();
    ssize_t remaining = static_cast<ssize_t>(str.size());
    while (remaining > 0) {
        ssize_t n = write(fd, p, remaining);
        if (n == -1) {
            return false;
        }
        p += n;
        remaining -= n;
    }
    p = nullptr;
    return true;
}

bool WriteStringToFile(const std::string& str, const std::string& filePath)
{
    if (!CreateFile(filePath)) {
        return false;
    }
    if (filePath.empty()) {
        return false;
    }

    char realPath[PATH_MAX] = {0};
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        return false;
    }

    int fd = open(realPath, O_RDWR | O_CREAT, static_cast<mode_t>(0600));
    if (fd < 0) {
        std::cout << "FileUtils: fd open failed!" << std::endl;
        return false;
    }
    bool result = WriteStringToFilefd(fd, str);
    close(fd);
    return result;
}
};

namespace OHOS {
namespace Rosen {

MSKPSrc::MSKPSrc(const std::string& path) : fPath_(path)
{
    std::unique_ptr<SkStreamAsset> stream = SkStream::MakeFromFile(fPath_.c_str());

    int count = SkMultiPictureDocumentReadPageCount(stream.get());
    if (count > 0) {
        auto deserialContext = std::make_unique<SkSharingDeserialContext>();
        SkDeserialProcs procs;
        procs.fImageProc = SkSharingDeserialContext::deserializeImage;
        procs.fImageCtx = deserialContext.get();

        fPages_.reset(count);
        SkMultiPictureDocumentRead(stream.get(), &fPages_[0], count, &procs);
    }
}

bool MSKPSrc::NextFrame()
{
    if (++curFrameNum_ == fPages_.size()) {
        curFrameNum_ = 0;
        sleep(1);
        return false;
    }
    return true;
}

bool MSKPSrc::Draw(SkCanvas* c) const
{
    SkRect cullrect = fPages_[curFrameNum_].fPicture.get()->cullRect();
    std::cout << "cullrect.width()=" << cullrect.width() << "cullrect.width()=" << cullrect.height() << std::endl;
    c->drawPicture(fPages_[curFrameNum_].fPicture.get());
    return true;
}


DrawingDCL::DrawingDCL(int32_t argc, char* argv[])
{
    DCLCommand dclCommand = DCLCommand(argc, argv);
    UpdateParametersFromDCLCommand(dclCommand);
    skiaRecording.InitConfigsFromParam();
    if (iterateType_ == IterateType::REPLAY_MSKP) {
        mskpPtr = std::make_unique<MSKPSrc>("/data/OH.mskp");
    } else if (iterateType_ == IterateType::REPLAY_SKP) {
        SkFILEStream input("/data/OH.skp");
        skpPtr = SkPicture::MakeFromStream(&input);
    }
}

DrawingDCL::~DrawingDCL()
{
    mskpPtr.reset();
    std::cout << "~DrawingDCL" << std::endl;
}

void DrawingDCL::PrintDurationTime(const std::string & description,
    std::chrono::time_point<std::chrono::system_clock> start)
{
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - start);
    const double convertToMs = 1000000.0;
    std::cout << description << duration.count() / convertToMs << "ms" << std::endl;
}

bool DrawingDCL::IterateFrame(int &curLoop, int &frame)
{
    ++frame;
    if (frame > endFrame_) {
        frame = beginFrame_;
        if (++curLoop == loop_) {
            return false;
        }
    }
    return true;
}

bool DrawingDCL::ReplayMSKP(SkCanvas* skiaCanvas)
{
    std::string tmp;
    std::cout << "press any key to draw one skp in OH.(begin)" << std::endl;
    std::string timePrint = "lkx: draw time of frame " + std::to_string(mskpPtr->GetCurFrameNum()) + " is ";
    if (loop_ == 0) {
        std::cin>>tmp;
    }
    if (beginFrame_ == 1) {
        std::cout<<"Frame: " << std::to_string(mskpPtr->GetCurFrameNum()) <<" .sleep 4s..."<< std::endl;
        sleep(opItemStep_);
    }
    auto start = std::chrono::system_clock::now();
    mskpPtr->Draw(skiaCanvas);
    PrintDurationTime(timePrint, start);
    if (beginFrame_ == 1) {
        std::cout<<"Frame: " << std::to_string(mskpPtr->GetCurFrameNum()) <<" .sleep 4s again..."<< std::endl;
        sleep(opItemStep_);
    }
    std::cout << "press any key to draw one skp in OH.(end)" << std::endl;
    if (loop_ == 0) {
        std::cin>>tmp;
    }
    return mskpPtr->NextFrame();
}

void DrawingDCL::ReplaySKP(SkCanvas *skiaCanvas)
{
    static int frameNum = 0;
    std::cout << "repaly skp. the " << frameNum << "times" << std::endl;
    std::string tmp;
    std::cout << "press any key to draw one frame.(begin)" << std::endl;
    std::cin >> tmp;
    auto start = std::chrono::system_clock::now();
    skiaCanvas->drawPicture(skpPtr);
    std::cout << "press any key to draw one frame.(end)" << std::endl;
    PrintDurationTime("The frame PlayBack time is ", start);
    std::cin >> tmp;
}

bool DrawingDCL::PlayBackByFrame(Drawing::Canvas* canvas, bool isDumpPictures)
{
    //read DrawCmdList from file
    if (canvas == nullptr) {
        return false;
    }
    auto start = std::chrono::system_clock::now();
    static int curLoop = 0;
    std::string dclFile = inputFilePath_ + "frame" + std::to_string(curFrameNo_) + ".drawing";
    std::cout << "PlayBackByFrame dclFile:" << dclFile << std::endl;

    if (LoadDrawCmdList(dclFile) < 0) {
        std::cout << "failed to loadDrawCmdList" << std::endl;
        IterateFrame(curLoop, curFrameNo_);
        return false;
    }
    PrintDurationTime("Load DrawCmdList file time is ", start);
    start = std::chrono::system_clock::now();
    dcl_->Playback(*canvas);
    PrintDurationTime("The frame PlayBack time is ", start);
    return IterateFrame(curLoop, curFrameNo_);
}

bool DrawingDCL::PlayBackByOpItem(SkCanvas* skiaCanvas, bool isMoreOps)
{
    if (skiaCanvas == nullptr) {
        return false;
    }
    auto start = std::chrono::system_clock::now();
    // read drawCmdList from file
    std::string dclFile = inputFilePath_ + "frameByOpItem.drawing";
    std::cout << "PlayBackFrame dclFile:" << dclFile << std::endl;

    if (LoadDrawCmdList(dclFile) < 0) {
        std::cout << "failed to loadDrawCmdList" << std::endl;
        return false;
    }
    PrintDurationTime("Load DrawCmdList file time is ", start);
    // Playback
    static double opitemId = 0;
    if (!isMoreOps) {
        opitemId -= opItemStep_;
        if (opitemId < 0) {
            opitemId = 0;
        }
        std::cout<< "This is already the first OpItem." << std::endl;
    } else {
        opitemId += opItemStep_;
    }
    std::cout << "play back to opitemId = " << static_cast<int>(opitemId) << std::endl;
    return true;
}

bool DrawingDCL::GetDirectionAndStep(std::string command, bool &isMoreOps)
{
    if (command.empty()) {
        return true;
    }
    std::vector<std::string> words;
    std::stringstream ss(command);
    std::string word;
    while (ss >> word) {
        words.emplace_back(word);
    }
    const size_t twoParam = 2;
    if (words.size() != twoParam) {
        std::cout << "Wrong Parameter!" << std::endl;
        return false;
    }
    if (std::strcmp(words[0].c_str(), "l") == 0 || std::strcmp(words[0].c_str(), "L") == 0) {
        isMoreOps = false;
    } else if (std::strcmp(words[0].c_str(), "m") == 0 || std::strcmp(words[0].c_str(), "M") == 0) {
        isMoreOps = true;
    } else {
        std::cout << "Wrong Direction!" << std::endl;
        return false;
    }
    // check if the input for step is valid
    int dotPostion = -1;
    for (size_t i = 0; i < words[1].size(); ++i) {
        if (words[1][i] == '.' && dotPostion == -1) {
            dotPostion = static_cast<int>(i);
        } else if (words[1][i] >= '0' && words[1][i] <= '9') {
            continue;
        } else {
            std::cout << "Please enter right step!" << std::endl;
            return false;
        }
    }
    opItemStep_ = std::stod(words[1]);
    return true;
}

void DrawingDCL::UpdateParametersFromDCLCommand(const DCLCommand &dclCommand)
{
    iterateType_ = dclCommand.iterateType_;
    beginFrame_ = dclCommand.beginFrame_;
    endFrame_ = dclCommand.endFrame_;
    loop_ = dclCommand.loop_;
    opItemStep_ = dclCommand.opItemStep_;
    inputFilePath_ = dclCommand.inputFilePath_;
    outputFilePath_ = dclCommand.outputFilePath_;
}

void DrawingDCL::UpdateParameters(bool notNeeded)
{
    if (notNeeded) {
        return;
    }
    std::cout << "Please re-enter the parameters" << std::endl;
    std::string line;
    getline(std::cin, line);
    if (line.empty()) {
        return;
    }
    DCLCommand dclCommand = DCLCommand(line);
    UpdateParametersFromDCLCommand(dclCommand);
}

void DrawingDCL::Test(Drawing::Canvas* canvas, int width, int height)
{
    std::cout << "DrawingDCL::Test+" << std::endl;
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
        std::cout << "DrawingDCL::Test skia" << std::endl;
        orgSkiaCanvas_ = canvas->GetImpl<Drawing::SkiaCanvas>()->ExportSkCanvas();
        if (skiaRecording.GetCaptureEnabled()) {
            std::cout << "DrawingDCL::Test skia begin capture+" << std::endl;
            skiaCanvas_ = skiaRecording.BeginCapture(orgSkiaCanvas_, width, height);
            canvas->GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(skiaCanvas_);
        }
#endif
    auto start = std::chrono::system_clock::now();
    switch (iterateType_) {
        case IterateType::ITERATE_FRAME:
            UpdateParameters(PlayBackByFrame(canvas));
            break;
        case IterateType::ITERATE_OPITEM:
            break;
        case IterateType::ITERATE_OPITEM_MANUALLY: {
            static bool isMoreOps = true;
            std::string opActionsStr = isMoreOps ? "more" : "less";
            std::cout << "Do you want to execute " << opItemStep_ << " OpItems " << opActionsStr << " ?\n"
                "To Modify, enter the action (More or less) and step size, for example, \"M 2\". \n"
                " Press Enter to continue." << std::endl;
            std::string line;
            do {
                getline(std::cin, line);
            } while (!GetDirectionAndStep(line, isMoreOps));
            break;
        }
        case IterateType::REPLAY_MSKP:
            std::cout << "replay mskp... " << std::endl;
#ifdef RS_ENABLE_VK
            if (RSSystemProperties::IsUseVulkan()) {
                UpdateParameters(ReplayMSKP(skiaCanvas_) || (beginFrame_ == 1));
            }
#endif
            break;
        case IterateType::REPLAY_SKP:
            std::cout << "replay skp... " << std::endl;
#ifdef RS_ENABLE_VK
            if (RSSystemProperties::IsUseVulkan()) {
                ReplaySKP(skiaCanvas_);
            }
#endif
            break;
        case IterateType::OTHER:
            std::cout << "Unknown iteratetype, please reenter parameters!" << std::endl;
            break;
        default:
            std::cout << "Wrong iteratetype!" << std::endl;
            break;
    }
    PrintDurationTime("This frame draw time is: ", start);
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    if (skiaRecording.GetCaptureEnabled()) {
        skiaRecording.EndCapture();
    }
    canvas->GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(orgSkiaCanvas_);
#endif
    std::cout << "DrawingDCL::Test-" << std::endl;
}

void DrawingDCL::Output()
{
}

class MyAllocator : public DefaultAllocator {
public:
    MyAllocator(int fd, size_t size, uint8_t* mapFile) : fd_(fd), size_(size), mapFile_(mapFile) {}

    ~MyAllocator()
    {
        Dealloc(mapFile_);
    }

    void Dealloc(void *mapFile)
    {
        if (mapFile != mapFile_) {
            std::cout << "MyAllocator::Dealloc data addr not match!" << std::endl;
        }
        if (mapFile_ != nullptr) {
            if (::munmap(mapFile_, size_) == -1) {
                std::cout << "munmap failed!" <<std::endl;
            }
            mapFile_ = nullptr;
        }
        if (fd_ > 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }
private:
    int fd_;
    size_t size_;
    uint8_t *mapFile_;
};

std::string DrawingDCL::GetRealPathStr(const std::string& filePath)
{
    std::string realPathStr = "";
    char actualPath[PATH_MAX + 1] = {0};
    if (realpath(filePath.c_str(), actualPath) == nullptr) {
        std::cout << "The path of DrawCmdList file is empty!" << std::endl;
        return realPathStr;
    }
    realPathStr = actualPath;
    return realPathStr;
}

bool DrawingDCL::IsValidFile(const std::string& realPathStr)
{
    return realPathStr.find(dclFileDir_) == 0;
}

int DrawingDCL::LoadDrawCmdList(const std::string& dclFile)
{
    std::string realDclFilePathStr = GetRealPathStr(dclFile);
    if (realDclFilePathStr.empty()) {
        return -1;
    }
    if (!IsValidFile(realDclFilePathStr)) {
        std::cout << "The path of DrawCmdList file is not valid!" << std::endl;
        return -1;
    }
    UniqueFd fd(open(realDclFilePathStr.c_str(), O_RDONLY));
    if (fd.Get() < 0) {
        std::cout << "Open file failed" << dclFile.c_str() << std::endl;
        return -1;
    }
    struct stat statbuf;
    if (fstat(fd.Get(), &statbuf) < 0) {
        return -1;
    }
    std::cout << "statbuf.st_size = " << statbuf.st_size << std::endl;

    auto mapFile = static_cast<uint8_t *>(mmap(nullptr, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (mapFile == MAP_FAILED) {
        return -1;
    }
    std::cout << "mapFile OK" << std::endl;

    MessageParcel messageParcel(new MyAllocator(fd.Get(), statbuf.st_size, mapFile));
    messageParcel.SetMaxCapacity(recordingParcelMaxCapcity_);
    if (!messageParcel.ParseFrom(reinterpret_cast<uintptr_t>(mapFile), statbuf.st_size)) {
        munmap(mapFile, statbuf.st_size);
        return -1;
    }
    std::cout << "messageParcel GetDataSize() = " << messageParcel.GetDataSize() << std::endl;

    RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
    RSMarshallingHelper::Unmarshalling(messageParcel, dcl_);
    RSMarshallingHelper::EndNoSharedMem();
    if (dcl_ == nullptr) {
        std::cout << "dcl is nullptr" << std::endl;
        munmap(mapFile, statbuf.st_size);
        return -1;
    }
    std::cout << "The size of Ops is " << dcl_->GetOpItemSize() << std::endl;
    std::string opsFile = inputFilePath_ + "ops_frame" + std::to_string(curFrameNo_) + ".txt";
    WriteStringToFile(dcl_->GetOpsWithDesc(), opsFile);
        munmap(mapFile, statbuf.st_size);
    return 0;
}
}
}

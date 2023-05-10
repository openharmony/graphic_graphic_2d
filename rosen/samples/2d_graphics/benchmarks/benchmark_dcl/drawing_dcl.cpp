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
#include "drawing_dcl.h"
#include <sstream>
#include <fcntl.h>

namespace OHOS {
namespace Rosen {
DrawingDCL::DrawingDCL(int32_t argc, char* argv[])
{
    DCLCommand dclCommand = DCLCommand(argc, argv);
    UpdateParametersFromDCLCommand(dclCommand);
}

DrawingDCL::~DrawingDCL()
{
    std::cout << "~DrawingDCL" << std::endl;
    delete dcl_;
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

bool DrawingDCL::PlayBackByFrame(SkCanvas* skiaCanvas, bool isDumpPictures)
{
    //read DrawCmdList from file
    if (skiaCanvas == nullptr) {
        return false;
    }
    auto start = std::chrono::system_clock::now();
    static int frame = beginFrame_;
    static int curLoop = 0;
    std::string dclFile = inputFilePath_ + "frame" + std::to_string(frame) + ".txt";
    std::cout << "PlayBackFrame dclFile:" << dclFile << std::endl;

    if (LoadDrawCmdList(dclFile) < 0) {
        std::cout << "failed to loadDrawCmdList" << std::endl;
        IterateFrame(curLoop, frame);
        return false;
    }
    PrintDurationTime("Load DrawCmdList file time is ", start);
    start = std::chrono::system_clock::now();
    dcl_->Playback(*skiaCanvas);
    PrintDurationTime("The frame PlayBack time is ", start);

    return IterateFrame(curLoop, frame);
}

bool DrawingDCL::PlayBackByOpItem(SkCanvas* skiaCanvas, bool isMoreOps)
{
    if (skiaCanvas == nullptr) {
        return false;
    }
    auto start = std::chrono::system_clock::now();
    // read drawCmdList from file
    std::string dclFile = inputFilePath_ + "frameByOpItem.txt";
    std::cout << "PlayBackFrame dclFile:" << dclFile << std::endl;

    if (LoadDrawCmdList(dclFile) < 0) {
        std::cout << "failed to loadDrawCmdList" << std::endl;
        return false;
    }
    PrintDurationTime("Load DrawCmdList file time is ", start);
    // Playback
    static double opitemId = 0;
    double oldOpId = opitemId;
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
    if (opitemId < dcl_->GetSize()) {
        std::cout << dcl_->PlayBackForRecord(*skiaCanvas, 0, static_cast<int>(opitemId), static_cast<int>(oldOpId))
            << std::endl;
    } else {
        std::cout << dcl_->PlayBackForRecord(*skiaCanvas, 0, dcl_->GetSize(), static_cast<int>(oldOpId)) << std::endl;
        opitemId = 0;
        return false;
    }
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
            dotPostion = i;
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
    iterateType = dclCommand.iterateType_;
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

void DrawingDCL::Test(SkCanvas* canvas, int width, int height)
{
    std::cout << "DrawingDCL::Test+" << std::endl;
    auto start = std::chrono::system_clock::now();
    switch (iterateType) {
        case IterateType::ITERATE_FRAME:
            UpdateParameters(PlayBackByFrame(canvas));
            break;
        case IterateType::ITERATE_OPITEM:
            UpdateParameters(PlayBackByOpItem(canvas));
            break;
        case IterateType::ITERATE_OPITEM_MANUALLY:
            static bool isMoreOps = true;
            std::string opActionsStr = isMoreOps ? "more" : "less";
            std::cout << "Do you want to execute " << opItemStep_ << " OpItems " << opActionsStr << " ?\n"
                "To Modify, enter the action (More or less) and step size, for example, \"M 2\". \n"
                " Press Enter to continue." << std::endl;
            std::string line;
            do {
                getline(std::cin, line);
            } while (!GetDirectionAndStep(line, isMoreOps));
            UpdateParameters(PlayBackByOpItem(canvas, isMoreOps));
            break;
        default:
            std::cout << "Wrong IterateType" << std::endl;
            break;
    }
    PrintDurationTime("This frame draw time is: ", start);
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

int DrawingDCL::LoadDrawCmdList(std::string dclFile)
{
    char* realDclFilePath = realpath(dclFile, NULL);
    if (realDclFilePath == nullptr) {
        return -1;
    }
    int32_t fd = open(realDclFilePath, O_RDONLY);
    free(realDclFilePath);
    if (fd < 0) {
        std::cout << "Open file failed" << dclFile.c_str() << std::endl;
        return -1;
    }
    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0) {
        close(fd);
        fd = -1;
        return -1;
    }
    std::cout << "statbuf.st_size = " << statbuf.st_size << std::endl;

    auto mapFile = static_cast<uint8_t *>(mmap(nullptr, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (mapFile == MAP_FAILED) {
        close(fd);
        fd = -1;
        return -1;
    }
    std::cout << "mapFile OK" << std::endl;

    MessageParcel messageParcel(new MyAllocator(fd, statbuf.st_size, mapFile));
    messageParcel.SetMaxCapacity(recordingParcelMaxCapcity_);
    if (!messageParcel.ParseFrom(reinterpret_cast<uintptr_t>(mapFile), statbuf.st_size)) {
        munmap(mapFile, statbuf.st_size);
        close(fd);
        fd = -1;
        return -1;
    }
    std::cout << "messageParcel GetDataSize() = " << messageParcel.GetDataSize() << std::endl;

    dcl_ = DrawCmdList::Unmarshalling(messageParcel);
    if (dcl_ == nullptr) {
        std::cout << "dcl is nullptr" << std::endl;
        munmap(mapFile, statbuf.st_size);
        close(fd);
        fd = -1;
        return -1;
    }
    std::cout << "The size of Ops is " << dcl_->GetSize() << std::endl;
    munmap(mapFile, statbuf.st_size);
    close(fd);
    fd = -1;
    return 0;
}
}
}

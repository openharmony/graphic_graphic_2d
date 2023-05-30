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

#ifndef DRAWING_DCL_H
#define DRAWING_DCL_H

#include <chrono>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <message_parcel.h>
#include "pipeline/rs_draw_cmd_list.h"

#include "benchmark.h"
#include "dcl_command.h"

namespace OHOS {
namespace Rosen {
class DrawingDCL : public BenchMark {
public:
    DrawingDCL() { std::cout << "DrawingDCL" << std::endl; }
    DrawingDCL(int32_t argc, char* argv[]);
    ~DrawingDCL();
    bool GetDirectionAndStep(std::string command, bool &isMoreOps);
    bool IterateFrame(int &curLoop, int &frame);
    bool PlayBackByFrame(SkCanvas *skiaCanvas, bool isDumpPictures = false);
    bool PlayBackByOpItem(SkCanvas *skiaCanvas, bool isMoreOps = true);
    void UpdateParameters(bool notNeeded);
    void UpdateParametersFromDCLCommand(const DCLCommand& dclCommand);
    void PrintDurationTime(const std::string &description, std::chrono::time_point<std::chrono::system_clock> start);
    void Start() override {};
    void Stop() override {};
    void Test(SkCanvas *canvas, int width, int height) override;
    void Output() override;
    int LoadDrawCmdList(const std::string& dclFile);
    std::string GetRealPathStr(const std::string& filePath);
    bool IsValidFile(const std::string& realPathStr);

private:
    friend class DCLCommand;
    const static size_t recordingParcelMaxCapcity_ = 234 * 1000 * 1024;
    inline const static std::string dclFileDir_ = "/data/";
    DrawCmdList* dcl_ = nullptr;
    IterateType iterateType = IterateType::ITERATE_FRAME;
    int beginFrame_ = 0;
    int endFrame_ = 100;
    int loop_ = 1;
    double opItemStep_ = 1;
    std::string inputFilePath_ = "/data/lkx/";
    std::string outputFilePath_ = "/data/lkx/";
};
}
}

#endif
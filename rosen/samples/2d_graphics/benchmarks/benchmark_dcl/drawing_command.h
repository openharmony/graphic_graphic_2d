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
#ifndef DRAWING_COMMAND_H
#define DRAWING_COMMAND_H

#include <iostream>
#include <vector>
#include <unordered_map>

namespace OHOS {
namespace Rosen {
enum class IterateType {
    ITERATE_FRAME,
    ITERATE_OPITEM,
    ITERATE_OPITEM_MANUALLY,
    REPLAY_MSKP,
    REPLAY_SKP,
    OTHER,
};
class DCLCommand {
public:
    DCLCommand(int32_t argc, char* argv[]);
    explicit DCLCommand(std::string commandLine);
    ~DCLCommand()= default;
    void ParseCommand(std::vector<std::string> argv);
    void HandleCommand(std::string option, const std::string& augment);
    void HandleCommandIterateType(const std::string& inputStr);
    void CheckParameter();
private:
    friend class DrawingDCL;
    enum class CommandType {
        CT_T,
        CT_B,
        CT_E,
        CT_L,
        CT_S,
        CT_I,
        CT_O,
        CT_H,
    };

    const std::unordered_map<std::string, CommandType> commandMap_ = {
        { std::string("-t"), CommandType::CT_T }, { std::string("--type"), CommandType::CT_T },
        { std::string("-b"), CommandType::CT_B }, { std::string("--beginFrame"), CommandType::CT_B },
        { std::string("-e"), CommandType::CT_E }, { std::string("--endFrame"), CommandType::CT_E },
        { std::string("-l"), CommandType::CT_L }, { std::string("--loop"), CommandType::CT_L },
        { std::string("-s"), CommandType::CT_S }, { std::string("--step"), CommandType::CT_S },
        { std::string("-i"), CommandType::CT_I }, { std::string("--inputFilePath"), CommandType::CT_I },
        { std::string("-o"), CommandType::CT_O }, { std::string("--outputFilePath"), CommandType::CT_O },
        { std::string("-h"), CommandType::CT_H }, { std::string("--help"), CommandType::CT_H },
    };

    const std::string dclMsgErr_ = "error input!\n use command '--help' get more information\n";
    const std::string breakLine_ = std::string(80, '-');
    const std::string dclMsg_ = "usage: /bin/drawing_engine_sample dcl <option> <argument> \n" + breakLine_ +
        "\nThere are common commands list:\n"
        " -t,--type                  set the type of playback, \n"
        "                            \t0: iterate by frame,\n"
        "                            \t1: iterate by opItem,\n"
        "                            \t2: iterate by opItem using manual control,\n"
        " -b,--beginFrame            set the start number of frames for playback, \n"
        " -e,--beginFrame            set the end number of frames for playback, \n"
        " -l,--loop                  set the loops of iterating by frame, \n"
        " -s,--step                  set the step when iterating by opItem "
                                        "(the step can be a decimal), \n"
        " -i,--inputFilePath         set the input path for drawCmdList files, \n"
        " -o,--outputFilePath        set the output path for drawCmdList files, \n"
        " -h,--help                  get help, \n" + breakLine_ +
        "\nExample: /bin/drawing_ening_sample dcl -t 0 -b 1 -e 100 \n" + breakLine_ + "\n";

    IterateType iterateType_ = IterateType::ITERATE_FRAME;
    int32_t beginFrame_ = 0;
    int32_t endFrame_ = 100;
    int32_t loop_ = 1;
    double opItemStep_ = 1;
    std::string inputFilePath_ = "/data/lkx/";
    std::string outputFilePath_ = "/data/lkx/";
};
}
}


#endif
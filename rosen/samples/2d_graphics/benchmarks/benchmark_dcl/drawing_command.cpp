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

#include "drawing_command.h"
#include <sstream>
#include <vector>

namespace OHOS {
namespace Rosen {
DCLCommand::DCLCommand(int32_t argc, char* argv[])
{
    std::vector<std::string> argvNew(argv, argv + argc);
    ParseCommand(argvNew);
}

void DCLCommand::ParseCommand(std::vector<std::string> argv)
{
    const size_t twoParam = 2;
    const size_t threeParam = 3;
    switch (argv.size()) {
        case twoParam:
            std::cout << "iterate frame by default, beginFrame = " << beginFrame_ << ", endFrame = " <<
            endFrame_ << std::endl;
            break;
        case threeParam:
            if (strcmp(argv.back().c_str(), "--help") != 0 || strcmp(argv.back().c_str(), "-h") != 0) {
                std::cout << dclMsg_ << std::endl;
            }
            break;
        default:
            for (size_t i = 2; i < argv.size(); ++i) {
                std::string option = argv[i];
                std::string augment;
                if (i < argv.size() - 1) {
                    augment = argv[i + 1];
                }
                if (commandMap_.count(option) > 0) {
                    HandleCommand(option, augment);
                }
            }
            CheckParameter();
            break;
    }
}

DCLCommand::DCLCommand(std::string commandLine)
{
    std::istringstream ss(commandLine);
    std::string param;
    std::vector<std::string> params;
    if (commandLine.find("drawing_ending_sample") == std::string::npos) {
        params.emplace_back("drawing_engine_sample");
        params.emplace_back("dcl");
    }
    while (ss >> param) {
        params.emplace_back(param);
    }
    ParseCommand(params);
}

void DCLCommand::HandleCommandIterateType(const std::string& inputStr)
{
    switch (std::stoi(inputStr.c_str())) {
        case static_cast<int>(IterateType::ITERATE_FRAME):
            iterateType_ = IterateType::ITERATE_FRAME;
            break;
        case static_cast<int>(IterateType::ITERATE_OPITEM):
            iterateType_ = IterateType::ITERATE_OPITEM;
            break;
        case static_cast<int>(IterateType::ITERATE_OPITEM_MANUALLY):
            iterateType_ = IterateType::ITERATE_OPITEM_MANUALLY;
            break;
        case static_cast<int>(IterateType::REPLAY_MSKP):
            iterateType_ = IterateType::REPLAY_MSKP;
            break;
        case static_cast<int>(IterateType::REPLAY_SKP):
            iterateType_ = IterateType::REPLAY_SKP;
            break;
        default:
            std::cout <<"Wrong Parameter: iterateType" << std::endl;
            return;
    }
}

void DCLCommand::HandleCommand(std::string option, const std::string& augment)
{
    int inputNum = 0;
    switch (commandMap_.at(option)) {
        case CommandType::CT_T:
            HandleCommandIterateType(augment);
            break;
        case CommandType::CT_B:
            inputNum = std::stoi(augment.c_str());
            beginFrame_ = inputNum > 0 ? inputNum : 0;
            break;
        case CommandType::CT_E:
            inputNum = std::stoi(augment.c_str());
            endFrame_ = inputNum > 0 ? inputNum : 0;
            break;
        case CommandType::CT_L:
            inputNum = std::stoi(augment.c_str());
            loop_ = inputNum > 0 ? inputNum : 0;
            break;
        case CommandType::CT_S:
            opItemStep_ = std::stod(augment.c_str());
            opItemStep_ = opItemStep_ > 0 ? opItemStep_ : 1;
            break;
        case CommandType::CT_I:
            inputFilePath_ = augment;
            if (inputFilePath_.back() != '/') {
                inputFilePath_ += '/';
            }
            break;
        case CommandType::CT_O:
            outputFilePath_ = augment;
            if (outputFilePath_.back() != '/') {
                outputFilePath_ += '/';
            }
            break;
        case CommandType::CT_H:
            std::cout << dclMsg_ <<std::endl;
            break;
        default:
            std::cout << "other unknown args:" <<std::endl;
            break;
    }
}

void DCLCommand::CheckParameter()
{
    if (beginFrame_ > endFrame_) {
        std::cout << "Wrong Parameter: beginFrame or endFrame!" << std::endl;
        beginFrame_ = 0;
        endFrame_ = 0;
    }
    if (opItemStep_ < 0) {
        std::cout << "Wrong Parameter: opItemStep!" << std::endl;
        opItemStep_ = 1;
    }
}
}
}
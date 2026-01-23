/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "screen_special_layer_demo_utils.h"

#include <regex>
#include <sstream>

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t EXPECTED_MATCH_COUNT = 3;
constexpr size_t BUFFER_SIZE = 4096;
constexpr int ENABLE_GLOBAL_BLACKLIST_PARAM_SIZE = 4;
constexpr int DECIMAL_BASE = 10;
const char* GREP_PERSISTID_FROM_RSTREE = "hidumper -s 10 -a RSTree | grep persistId";
const std::string POPEN_ERR = "popen failed";
}

void ScreenSpecialLayerDemoUtils::PrintPersistId()
{
    auto persistIdMap = GetAllPersistId();
    for (const auto& [name, persistId] : persistIdMap) {
        std::cout << "name : " << name << ", persistId : " << persistId << std::endl;
    }
}

std::map<std::string, std::string> ScreenSpecialLayerDemoUtils::GetAllPersistId()
{
    // get rs dump info
    std::string output = DumpRsTree();
    if (output == POPEN_ERR) {
        std::cerr << POPEN_ERR;
        return {};
    }

    std::istringstream stream(output);
    // use regular expressions to filter the names and persistIds of surfaceNodes
    std::regex pattern(R"(persistId\s+\[([^\]]+)\].*Name\s+\[([^\]]+)\])");
    std::smatch match;
    std::string line;
    std::map<std::string, std::string> persistIdMap;
    while (std::getline(stream, line)) {
        if (std::regex_search(line, match, pattern) && match.size() == EXPECTED_MATCH_COUNT) {
            // match[2] : name
            // match[1] : persistId
            persistIdMap.insert({match[2], match[1]});
        }
    }
    return persistIdMap;
}

std::string ScreenSpecialLayerDemoUtils::DumpRsTree()
{
    // create a pipeline and execute the command
    FILE* pipe = popen(GREP_PERSISTID_FROM_RSTREE, "r");
    if (pipe == nullptr) {
        return POPEN_ERR;
    }

    // read command output
    std::ostringstream oss;
    std::vector<char> buffer(BUFFER_SIZE);
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        oss << buffer.data();
    }
    
    pclose(pipe);
    return oss.str();
}

void ScreenSpecialLayerDemoUtils::EnableGlobalBlackList(int argc, char* argv[])
{
    if (argc != ENABLE_GLOBAL_BLACKLIST_PARAM_SIZE) {
        std::cerr << "Invalid parameter.";
        return;
    }

    // argv[0]:file path, argv[1]:command, argv[2]:screenId, argv[3]:enabled status
    ScreenId screenId = std::strtoul(argv[2], nullptr, DECIMAL_BASE);
    bool enable = false;
    if (!ParseBoolParam(argv[3], enable)) {
        return;
    }

    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId, enable);
}

bool ScreenSpecialLayerDemoUtils::ParseBoolParam(char* input, bool& output)
{
    if (input == nullptr) {
        std::cerr << "input is nullptr, parse failed.";
        return false;
    }

    std::string str = input;
    if (str == "true") {
        output = true;
        return true;
    }
    if (str == "false") {
        output = false;
        return true;
    }
    std::cerr << "Invalid parameter.";
    return false;
}

void ScreenSpecialLayerDemoUtils::ModifySpecialLayerList(int argc, char* argv[])
{
    ParamType curParamType =  ParamType::NONE;
    Params param;
    // argv[0] is file path, argv[1] is command, prams start from argv[2]
    for (int i = 2; i < argc; i++) {
        std::string curArgv = std::string(argv[i]);
        if (curArgv == "-s") {
            curParamType = ParamType::SCREEN_ID;
            continue;
        } else if (curArgv == "-n") {
            curParamType = ParamType::NODE_LIST;
            continue;
        } else if (curArgv == "-t") {
            curParamType = ParamType::TYPE_LIST;
            continue;
        }

        switch (curParamType) {
            case ParamType::SCREEN_ID :
                param.screenId = std::strtoul(argv[i], nullptr, DECIMAL_BASE);
                break;
            case ParamType::NODE_LIST :
                param.nodeList.push_back(std::strtoul(argv[i], nullptr, DECIMAL_BASE));
                break;
            case ParamType::TYPE_LIST :
                param.typeList.push_back(std::strtoul(argv[i], nullptr, DECIMAL_BASE));
                break;
            default :
                std::cerr << "Invalid parameter.";
                return;
        }
    }

    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto func = std::string(argv[1]);
    auto it = MODIFY_LIST_FUNC_MAP.find(func);
    if (it != MODIFY_LIST_FUNC_MAP.end()) {
        it->second(rsInterfaces, param);
    } else {
        std::cerr << "Invalid parameter.";
    }
}
} // namespace Rosen
} // namespace OHOS
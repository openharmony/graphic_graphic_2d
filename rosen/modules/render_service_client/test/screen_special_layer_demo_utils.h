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

#ifndef SCREEN_SPECIAL_LAYER_DEMO_UTILS_H
#define SCREEN_SPECIAL_LAYER_DEMO_UTILS_H

#include <map>
#include <string>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

enum class ParamType {
    NONE,
    SCREEN_ID,
    NODE_LIST,
    TYPE_LIST
};

struct Params {
    ScreenId screenId = INVALID_SCREEN_ID;
    std::vector<NodeId> nodeList = {};
    std::vector<NodeType> typeList = {};
};

using ModifyListFunc = std::function<void(RSInterfaces&, Params&)>;

const std::map<std::string, ModifyListFunc> MODIFY_LIST_FUNC_MAP {
    { "SetBlackList",
        [](RSInterfaces& interface, Params& p) { interface.SetVirtualScreenBlackList(p.screenId, p.nodeList); }},
    { "AddBlackList",
        [](RSInterfaces& interface, Params& p) { interface.AddVirtualScreenBlackList(p.screenId, p.nodeList); }},
    { "RemoveBlackList",
        [](RSInterfaces& interface, Params& p) { interface.RemoveVirtualScreenBlackList(p.screenId, p.nodeList); }},
    { "AddWhiteList",
        [](RSInterfaces& interface, Params& p) { interface.AddVirtualScreenWhiteList(p.screenId, p.nodeList); }},
    { "RemoveWhiteList",
        [](RSInterfaces& interface, Params& p) { interface.RemoveVirtualScreenWhiteList(p.screenId, p.nodeList); }},
    { "SetTypeBlackList",
        [](RSInterfaces& interface, Params& p) { interface.SetVirtualScreenTypeBlackList(p.screenId, p.typeList); }}
};

class ScreenSpecialLayerDemoUtils {
public:
    static void PrintPersistId();
    static std::map<std::string, std::string> GetAllPersistId();
    static std::string DumpRsTree();
    static void EnableGlobalBlackList(int argc, char* argv[]);
    static bool ParseBoolParam(char* input, bool& output);
    static void ModifySpecialLayerList(int argc, char* argv[]);
};
} // namespace Rosen
} // namespace OHOS
#endif // SCREEN_SPECIAL_LAYER_DEMO_UTILS_H
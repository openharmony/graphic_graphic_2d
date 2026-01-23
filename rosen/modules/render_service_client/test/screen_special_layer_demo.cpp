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

#include <algorithm>
#include <iostream>

using namespace OHOS::Rosen;

namespace {
constexpr const int MIN_PARAM_SIZE = 2;
}

int main(int argc, char* argv[])
{
    if (argc < MIN_PARAM_SIZE) {
        std::cerr << "Functions:\n"
            "1. PrintPersistId\n"
            "   a) Description: Print the persistId of all surfaceNodes.\n"
            "   b) Example: hdc shell ./data/screen_special_layer_demo PrintPersistId\n\n"

            "2. SetBlackList / AddBlackList / RemoveBlackList / AddWhiteList / RemoveWhiteList\n"
            "   a) Description: Modify the blacklist/whitelist of a virtual screen.\n"
            "   b) Parameter list: -s The id of the virtual screen whose blacklist/whitelist is to be modified\n"
            "                      -n The list of nodeIds for the blacklist/whitelist modification\n"
            "   c) Examples:\n"
            "      --- Set blacklist [1, 2] for screen 1:\n"
            "      hdc shell ./data/screen_special_layer_demo SetBlackList -s 1 -n 1 2\n"
            "      --- Set empty blacklist for screen 1:\n"
            "      hdc shell ./data/screen_special_layer_demo SetBlackList -s 1 -n\n"
            "      --- Add blacklist [1, 2] for screen 1:\n"
            "      hdc shell ./data/screen_special_layer_demo AddBlackList -s 1 -n 1 2\n\n"

            "3. SetTypeBlackList\n"
            "   a) Description: Set the typeBlacklist of a virtual screen.\n"
            "   b) Parameter list: -s The id of the virtual screen whose typeBlackList is to be modified\n"
            "                      -t The list of node types for the typeBlackList.\n"
            "   c) Example:\n"
            "      --- Set type blacklist [APP_WINDOW_NODE(1), CURSOR_NODE(12)] for screen 1:\n"
            "      hdc shell ./data/screen_special_layer_demo SetTypeBlackList -s 1 -t 1 12\n\n"

            "4. EnableGlobalBlackList\n"
            "   a) Description: Set whether to enable the global blacklist for a virtual screen.\n"
            "   b) Example:\n"
            "      --- Enable the global blacklist for screen 1 :\n"
            "      hdc shell ./data/screen_special_layer_demo EnableGlobalBlackList 1 true\n";
        return 1;
    }

    std::string func = std::string(argv[1]);
    if (func == "PrintPersistId") {
        ScreenSpecialLayerDemoUtils::PrintPersistId();
    } else if (func == "EnableGlobalBlackList") {
        ScreenSpecialLayerDemoUtils::EnableGlobalBlackList(argc, argv);
    } else if (MODIFY_LIST_FUNC_MAP.find(func) != MODIFY_LIST_FUNC_MAP.end()) {
        ScreenSpecialLayerDemoUtils::ModifySpecialLayerList(argc, argv);
    } else {
        std::cerr << "Invalid command.";
        return -1;
    }
    return 0;
}
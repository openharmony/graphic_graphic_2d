/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <iostream>
#include <optional>
#include <string>

#include "transaction/rs_interfaces.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "SetHgmExclusiveScreen Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demo calls RSInterfaces::SetHgmExclusiveScreen." << std::endl;
    std::cout << "SetHgmExclusiveScreen sets the exclusive screen for HGM." << std::endl;
    std::cout << "Passing nullopt disables exclusive mode." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Set exclusive screen (enter screenId)" << std::endl;
    std::cout << "2. Disable exclusive mode (pass nullopt)" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "========================================" << std::endl;

    auto& interfaces = RSInterfaces::GetInstance();

    while (true) {
        std::cout << std::endl;
        std::cout << "Please select option (0-2): ";
        int option = -1;
        std::cin >> option;

        if (option == 0) {
            std::cout << "Exiting." << std::endl;
            break;
        } else if (option == 1) {
            std::cout << "Please enter screenId (uint64_t): ";
            ScreenId screenId = 0;
            std::cin >> screenId;
            bool result = interfaces.SetHgmExclusiveScreen(screenId);
            std::cout << "SetHgmExclusiveScreen(screenId=" << screenId << ") returned: "
                      << (result ? "true (success)" : "false (failure)") << std::endl;
        } else if (option == 2) {
            bool result = interfaces.SetHgmExclusiveScreen(std::nullopt);
            std::cout << "SetHgmExclusiveScreen(nullopt) returned: "
                      << (result ? "true (success)" : "false (failure)") << std::endl;
        } else {
            std::cout << "Invalid option, please enter 0, 1, or 2." << std::endl;
        }
    }

    return 0;
}

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

#include <cstdlib>
#include <iostream>
#include <vector>
#include "transaction/rs_interfaces.h"

using namespace OHOS::Rosen;

int main()
{
    std::cout << "\n===== Demo screen need native p3 start =====" << std::endl;

    RSInterfaces& rsIf = RSInterfaces::GetInstance();

    bool running = true;
    while (running) {
        std::vector<ScreenId> screenIds = rsIf.GetAllScreenIds();
        std::cout << "GetAllScreenIds count: " << screenIds.size() << std::endl;
        for (ScreenId id : screenIds) {
            std::cout << "Screen ID: " << id << std::endl;
        }

        std::cout << "Please input screen id and skipColorSpaceConversion value (separated by space), "
                  << "e.g. '1001 1', or input 'q' to exit: ";
        std::string input;
        std::cin >> input;
        if (!std::cin.good() || input == "q" || input == "Q") {
            std::cout << "Exiting..." << std::endl;
            running = false;
            continue;
        }

        ScreenId selectedId = static_cast<ScreenId>(std::stoull(input));
        bool skipColorSpaceConversion = false;
        std::cin >> input;
        if (!std::cin.good()) {
            std::cout << "Invalid input, exiting..." << std::endl;
            running = false;
            continue;
        }
        skipColorSpaceConversion = (std::atoi(input.c_str()) != 0);
        rsIf.SetSkipColorSpaceConversion(selectedId, skipColorSpaceConversion);
        std::cout << "SetSkipColorSpaceConversion(id=" << selectedId << ", "
                  << (skipColorSpaceConversion ? "true" : "false") << ") called." << std::endl;
        std::cout << "Check demo color." << std::endl;
    }
    std::cout << "\n===== Demo finished =====" << std::endl;
    return 0;
}

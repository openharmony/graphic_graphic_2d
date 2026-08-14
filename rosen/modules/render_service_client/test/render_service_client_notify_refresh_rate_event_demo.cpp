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
#include <string>

#include "transaction/rs_interfaces.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
constexpr const char* GAMES_VOTER = "VOTER_GAMES";
constexpr const char* SKIP_VIRTUAL_DISPLAY_FLAG = ":SKIPVIRTUALDISPLAY";
constexpr const char* DEFAULT_PKG_NAME = "com.demo.game";
constexpr int32_t DEFAULT_APP_TYPE = 1;
constexpr uint32_t DEFAULT_MIN_REFRESH_RATE = 60;
constexpr uint32_t DEFAULT_MAX_REFRESH_RATE = 120;
}

static void ReportGamesEvent(const std::string& description, bool eventStatus)
{
    EventInfo eventInfo;
    eventInfo.eventName = GAMES_VOTER;
    eventInfo.eventStatus = eventStatus;
    eventInfo.minRefreshRate = DEFAULT_MIN_REFRESH_RATE;
    eventInfo.maxRefreshRate = DEFAULT_MAX_REFRESH_RATE;
    eventInfo.description = description;

    std::cout << "NotifyRefreshRateEvent: eventName=" << eventInfo.eventName
              << " eventStatus=" << (eventStatus ? "true" : "false")
              << " min=" << eventInfo.minRefreshRate
              << " max=" << eventInfo.maxRefreshRate
              << " description=\"" << eventInfo.description << "\"" << std::endl;

    auto& interfaces = RSInterfaces::GetInstance();
    interfaces.NotifyRefreshRateEvent(eventInfo);
    std::cout << "NotifyRefreshRateEvent sent." << std::endl;
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "SkipVirtualDisplayVote Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demo calls RSInterfaces::NotifyRefreshRateEvent with VOTER_GAMES." << std::endl;
    std::cout << "When the game event description contains ':SKIPVIRTUALDISPLAY'," << std::endl;
    std::cout << "HGM will skip VOTER_VIRTUALDISPLAY refresh-rate votes." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Report game event WITH skip flag (skip virtual display vote)" << std::endl;
    std::cout << "2. Report game event WITHOUT skip flag (normal)" << std::endl;
    std::cout << "3. Exit game event (clear skip flag)" << std::endl;
    std::cout << "4. Custom description" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "========================================" << std::endl;

    while (true) {
        std::cout << std::endl;
        std::cout << "Please select option (0-4): ";
        int option = -1;
        std::cin >> option;

        if (option == 0) {
            std::cout << "Exiting." << std::endl;
            break;
        } else if (option == 1) {
            pid_t gamePid = 0;
            std::cout << "Please enter game pid: ";
            std::cin >> gamePid;
            std::string description = std::string(DEFAULT_PKG_NAME) + ":" + std::to_string(gamePid) + ":" +
                std::to_string(DEFAULT_APP_TYPE) + SKIP_VIRTUAL_DISPLAY_FLAG;
            ReportGamesEvent(description, true);
        } else if (option == 2) {
            pid_t gamePid = 0;
            std::cout << "Please enter game pid: ";
            std::cin >> gamePid;
            std::string description = std::string(DEFAULT_PKG_NAME) + ":" + std::to_string(gamePid) + ":" +
                std::to_string(DEFAULT_APP_TYPE);
            ReportGamesEvent(description, true);
        } else if (option == 3) {
            pid_t gamePid = 0;
            std::cout << "Please enter game pid: ";
            std::cin >> gamePid;
            std::string description = std::string(DEFAULT_PKG_NAME) + ":" + std::to_string(gamePid) + ":" +
                std::to_string(DEFAULT_APP_TYPE);
            ReportGamesEvent(description, false);
        } else if (option == 4) {
            std::cout << "Please enter description: ";
            std::string description;
            std::cin >> description;
            int status = 0;
            std::cout << "Please enter eventStatus (0=exit, 1=enter): ";
            std::cin >> status;
            ReportGamesEvent(description, status != 0);
        } else {
            std::cout << "Invalid option, please enter 0, 1, 2, 3, or 4." << std::endl;
        }
    }

    return 0;
}

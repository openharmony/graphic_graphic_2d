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
#include <thread>
#include <chrono>

#include "transaction/rs_frame_stability_types.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS;
using namespace Rosen;

void PrintMenu()
{
    std::cout << "\n============================================\n";
    std::cout << "Frame Stability Interface Test Demo\n";
    std::cout << "============================================\n";
    std::cout << "Please select an interface to test:\n";
    std::cout << "1. RegisterFrameStabilityDetection\n";
    std::cout << "2. UnregisterFrameStabilityDetection\n";
    std::cout << "3. StartFrameStabilityCollection\n";
    std::cout << "4. GetFrameStabilityResult\n";
    std::cout << "0. Exit\n";
    std::cout << "============================================\n";
    std::cout << "Enter your choice: ";
}

FrameStabilityTarget GetFrameStabilityTarget()
{
    FrameStabilityTarget target;
    
    uint64_t id = 0;
    std::cout << "Enter target ID (default 0): ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        id = std::stoull(input);
    }
    target.id = id;
    target.type = FrameStabilityTargetType::SCREEN; // Set a default type
    
    return target;
}

FrameStabilityConfig GetFrameStabilityConfig()
{
    FrameStabilityConfig config;
    
    uint32_t stableDuration = 200;
    std::cout << "Enter stableDuration in milliseconds (100-5000, default 200): ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        stableDuration = std::stoul(input);
    }
    config.stableDuration = stableDuration;
    
    float changePercent = 0.0f;
    std::cout << "Enter changePercent (0.0-1.0, default 0.0): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        changePercent = std::stof(input);
    }
    config.changePercent = changePercent;
    
    return config;
}

void TestRegisterFrameStabilityDetection()
{
    std::cout << "\n--- Test RegisterFrameStabilityDetection ---\n";
    
    FrameStabilityTarget target = GetFrameStabilityTarget();
    FrameStabilityConfig config = GetFrameStabilityConfig();
    
    std::cout << "\nRegistering frame stability detection callback...\n";
    auto callback = [](bool isStable) {
        std::cout << "\n[Callback] FrameStabilityChanged triggered!\n";
        std::cout << "[Callback] Is stable: " << (isStable ? "true" : "false") << "\n";
    };
    
    int32_t ret = RSInterfaces::GetInstance().RegisterFrameStabilityDetection(target, config, callback);
    std::cout << "RegisterFrameStabilityDetection returned: " << ret;
    if (ret == 0) {
        std::cout << " (SUCCESS)\n";
    } else {
        std::cout << " (FAILED)\n";
    }
}

void TestUnregisterFrameStabilityDetection()
{
    std::cout << "\n--- Test UnregisterFrameStabilityDetection ---\n";
    
    FrameStabilityTarget target = GetFrameStabilityTarget();
    
    int32_t ret = RSInterfaces::GetInstance().UnregisterFrameStabilityDetection(target);
    std::cout << "UnregisterFrameStabilityDetection returned: " << ret;
    if (ret == 0) {
        std::cout << " (SUCCESS)\n";
    } else {
        std::cout << " (FAILED)\n";
    }
}

void TestStartFrameStabilityCollection()
{
    std::cout << "\n--- Test StartFrameStabilityCollection ---\n";
    
    FrameStabilityTarget target = GetFrameStabilityTarget();
    FrameStabilityConfig config = GetFrameStabilityConfig();
    
    int32_t ret = RSInterfaces::GetInstance().StartFrameStabilityCollection(target, config);
    std::cout << "StartFrameStabilityCollection returned: " << ret;
    if (ret == 0) {
        std::cout << " (SUCCESS)\n";
        std::cout << "Frame stability collection started\n";
    } else {
        std::cout << " (FAILED)\n";
    }
}

void TestGetFrameStabilityResult()
{
    std::cout << "\n--- Test GetFrameStabilityResult ---\n";
    
    FrameStabilityTarget target = GetFrameStabilityTarget();
    
    bool result = false;
    int32_t ret = RSInterfaces::GetInstance().GetFrameStabilityResult(target, result);
    std::cout << "GetFrameStabilityResult returned: " << ret;
    if (ret == 0) {
        std::cout << " (SUCCESS)\n";
        std::cout << "Frame stability result: " << (result ? "STABLE" : "UNSTABLE") << "\n";
    } else {
        std::cout << " (FAILED)\n";
    }
}

int main()
{
    std::cout << "============================================\n";
    std::cout << "Frame Stability Interface Test Demo\n";
    std::cout << "============================================\n";
    std::cout << "This demo tests 4 frame stability interfaces.\n";
    std::cout << "You can test each interface with custom parameters.\n";
    std::cout << "Press Ctrl+C to exit at any time.\n";
    std::cout << "============================================\n";
    
    while (true) {
        PrintMenu();
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice.empty()) {
            continue;
        }
        
        if (choice == "0") {
            std::cout << "\nExiting demo...\n";
            break;
        }
        
        switch (choice[0]) {
            case '1':
                TestRegisterFrameStabilityDetection();
                break;
            case '2':
                TestUnregisterFrameStabilityDetection();
                break;
            case '3':
                TestStartFrameStabilityCollection();
                break;
            case '4':
                TestGetFrameStabilityResult();
                break;
            default:
                std::cout << "\nInvalid choice! Please enter 0-4.\n";
                break;
        }
        
        std::cout << "\nPress Enter to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }
    
    std::cout << "Demo exited successfully.\n";
    return 0;
}
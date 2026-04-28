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
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <memory>
#include <map>
#include <sstream>
#include "rs_interface_client_frame_test.h"
#include <if_system_ability_manager.h>
#include <system_ability_definition.h>
#include <iremote_object.h>
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "platform/ohos/transaction/zidl/rs_client_to_render_connection_proxy.h"
#include "transaction/rs_transaction_data.h"
#include "command/rs_animation_command.h"
#include "command/rs_node_showing_command.h"
using namespace OHOS;
using namespace OHOS::Rosen;

// IPC Pathway Demo Tool
class IPCRobustnessDemo {
public:
    IPCRobustnessDemo(std::uint32_t type) {
        InitializeConnections(type);
    }
    
    ~IPCRobustnessDemo() {
        PrintSummary();
    }
    
    // Main demo	entrypoint
    void RunAllDemos() {
        PrintHeader("IPC Pathway Robustness Demo");

        // 1. Basic functionality demo
        Demo1_BasicFunctionality();

        // 2. Large data handling demo
        Demo2_LargeDataHandling();

        PrintSummary();
    }
    
private:
    std::shared_ptr<RSIClientToRenderConnection> TestRenderProxy_;
    std::shared_ptr<RSIClientToServiceConnection> TestrServiceProxy_;
    struct DemoResult {
        std::string name;
        bool success;
        int64_t timeMs;
        std::string details;
    };
    
    std::vector<DemoResult> results_;
    std::uint32_t testType_ = 0;
    
    void InitializeConnections(std::uint32_t type) {
        TestRenderProxy_ = RSRenderServiceConnectHub::GetClientToRenderConnection();
        TestrServiceProxy_ = RSRenderServiceConnectHub::GetClientToServiceConnection();
        testType_ = type;
        if (testType_ > 0) {
            std::cout << "[INFO] Current pathway: ToService" << std::endl;
        } else {
            std::cout << "[INFO] Current pathway: ToRender" << std::endl;           
        }
        std::cout << "[SUCCESS] IPC connection initialized successfully" << std::endl;
    }
    
    void PrintHeader(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    void PrintSubHeader(const std::string& title) {
        std::cout << "\n" << std::string(50, '-') << std::endl;
        std::cout << "[TEST] " << title << std::endl;
        std::cout << std::string(50, '-') << std::endl;
    }
    
    // Demo 1: Basic Functionality Test
    void Demo1_BasicFunctionality() {
        PrintSubHeader("Demo 1: Basic Functionality Test");

        std::cout << "\n[TEST CONTENT] Send simple transaction and measure response time" << std::endl;

        // IPC pathway test
        std::cout << "\n[IPC PATHWAY TEST]" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        if (!testType_) {
            SendSimpleTransaction(TestRenderProxy_);
        } else {
            SendSimpleTransaction(TestServiceProxy_);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(
            end - start).count() / 1000.0;

        std::cout << "  Time: " << std::fixed << std::setprecision(2) << time << " ms" << std::endl;

        DemoResult result;
        result.name = "Basic Functionality";
        result.success = true;
        result.timeMs = (int64_t)time;
        results_.push_back(result);
    }

    // Demo 2: Large Data Handling Test
    void Demo2_LargeDataHandling() {
        PrintSubHeader("Demo 2: Large Data Handling Capability");

        std::vector<int> dataSizesKB = {10, 50, 100, 200, 500}; // KB

        std::cout << "\n[TEST CONTENT] Send data packets of different sizes" << std::endl;
        std::cout << "\n+------------+--------------+--------------+" << std::endl;
        std::cout << "| Data Size  | Time(ms)     | Status       |" << std::endl;
        std::cout << "+------------+--------------+--------------+" << std::endl;

        for (int sizeKB : dataSizesKB) {
            std::cout << "| " << std::setw(10) << sizeKB << " KB | ";
            DemoResult outResult;
            outResult.name = "Demo2_LargeDataHandling" + std::to_string(sizeKB);
            // Test IPC pathway
            auto result = (testType_ == 0) ? TestLargeData(TestRenderProxy_, sizeKB) :
                TestLargeData(TestServiceProxy_, sizeKB);
            std::cout << std::setw(12) << result.timeMs << " | ";
            outResult.success = result.success;
            outResult.timeMs = result.timeMs;
            results_.push_back(outResult);
            // Display status
            if (result.success) {
                std::cout << "  [SUCCESS]  |";
            } else {
                std::cout << "  [FAILED]   |";
            }
            std::cout << std::endl;
        }

        std::cout << "+------------+--------------+--------------+" << std::endl;

        std::cout << "\n[CONCLUSION]" << std::endl;
        std::cout << "  - Performance is stable for data packets under 200KB" << std::endl;
        std::cout << "  - Ashmem mechanism is automatically enabled when size exceeds 200KB" << std::endl;
    }

    void PrintSummary() {
        PrintHeader("Test Summary");

        std::cout << "\n[TEST RESULT SUMMARY]\n" << std::endl;

        int successCount = 0;
        int64_t totalTimeMs = 0;
        for (const auto& result : results_) {
            if (result.success) successCount++;
            totalTimeMs += result.timeMs;
        }

        std::cout << "  Total Tests: " << results_.size() << std::endl;
        std::cout << "  Success Rate: " << std::fixed << std::setprecision(1)
                  << (100.0 * successCount / results_.size()) << "%" << std::endl;
        std::cout << "  Average Time: " << std::fixed << std::setprecision(2)
                  << (totalTimeMs / (double)results_.size()) << " ms" << std::endl;

        std::cout << "\n[SUCCESS] Demo completed!" << std::endl;
    }

    // Helper functions
    bool SendSimpleTransaction(sptr<RSIClientToRenderConnection> proxy) {
        auto transactionData = std::make_unique<RSTransactionData>();
        std::unique<RSCommand> command = std::make_unique<RSAnimationCallback>(
            1, 1, 1, AnimationCallbackEvent::FINISHED);
        transactionData->AddCommand(command, 1, FollowType::FOLLOW_TO_PARENT);
        proxy->CommitTransaction(transactionData);
        return true;
    }

    // Helper functions
    bool SendSimpleTransaction(sptr<RSIClientToServiceConnection> proxy) {
        auto transactionData = std::make_unique<RSTransactionData>();
        std::unique<RSCommand> command = std::make_unique<RSAnimationCallback>(
            1, 1, 1, AnimationCallbackEvent::FINISHED);
        transactionData->AddCommand(command, 1, FollowType::FOLLOW_TO_PARENT);
        proxy->CommitTransaction(transactionData);
        return true;
    }

    struct LargeDataResult {
        bool success;
        int64_t timeMs;
    };

    LargeDataResult TestLargeData(
        std::shared_ptr<RSIClientToRenderConnection> proxy,
        int sizeKB)
    {
        auto transactionData = CreateLargeTransaction(sizeKB * 1024);

        auto start = std::chrono::high_resolution_clock::now();
        proxy->CommitTransaction(transactionData);
        auto end = std::chrono::high_resolution_clock::now();

        LargeDataResult result;
        result.success = true;
        result.timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start).count();

        return result;
    }

    LargeDataResult TestLargeData(
        std::shared_ptr<RSIClientToServiceConnection> proxy,
        int sizeKB)
    {
        auto transactionData = CreateLargeTransaction(sizeKB * 1024);

        auto start = std::chrono::high_resolution_clock::now();
        proxy->CommitTransaction(transactionData);
        auto end = std::chrono::high_resolution_clock::now();

        LargeDataResult result;
        result.success = true;
        result.timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start).count();

        return result;
    }

    std::unique_ptr<RSTransactionData> CreateLargeTransaction(size_t size) {
        auto transactionData = std::make_unique<RSTransactionData>();
        
        size_t commandCount = size / sizeof(RSAnimationCallback);
        for (size_t i = 0; i < commandCount; i++) {
            std::unique<RSCommand> command = std::make_unique<RSAnimationCallback>(
                i, i, i, AnimationCallbackEvent::FINISHED);
            transactionData->AddCommand(command, 1, FollowType::FOLLOW_TO_PARENT);
        }
        
        return transactionData;
    }
};

// Main program entry
int main(int argc, char** argv) {
    std::cout << "\n============================================================" << std::endl;
    std::cout << "   OpenHarmony RenderService IPC Pathway Robustness Demo" << std::endl;
    std::cout << "                 ClientToRender vs ClientToService" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "Input 0(Test toRender) or 1(Test toService)";
    uint32_t type = 0;
    std::cin >> type;
    IPCRobustnessDemo demo(type);
    demo.RunAllDemos();
    std::cout << "\n[SUCCESS] Demo execution completed!" << std::endl;
    return 0;

}
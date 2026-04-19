/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef INTERFACE_CLIENT_FRAME_TEST
#define INTERFACE_CLIENT_FRAME_TEST
#include "transaction/rs_transaction.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"

// Test registration macro
#define REGISTER_TEST(test_name, test_func) \
    InterfaceClientFrameTest::GetInstance().RegisterTest(test_name, test_func)

class InterfaceClientFrameTest {
public:
    InterfaceClientFrameTest();
    ~InterfaceClientFrameTest() = default;
    static InterfaceClientFrameTest &GetInstance();
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> GetRootSurfaceNode()
    {
        return rootSurfaceNode_;
    }
    std::shared_ptr<OHOS::Rosen::RSCanvasNode> GetRootCanvasNode()
    {
        return rootCanvasNode_;
    }

    // Register test with name and function
    void RegisterTest(const std::string& testName, std::function<void(InterfaceClientFrameTest &)> func)
    {
        execFuncMap[testName] = func;
    }

    // Legacy method for backward compatibility (can be deprecated later)
    void SetExecFun(uint32_t code, std::function<void(InterfaceClientFrameTest &)> func)
    {
        (void)code; // Mark parameter as intentionally unused
        (void)func; // Mark parameter as intentionally unused
        std::cout << "Warning: SetExecFun with uint32_t code is deprecated, use RegisterTest with string name instead" << std::endl;
    }

    void Run();
private:
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> rootSurfaceNode_ = nullptr;
    std::shared_ptr<OHOS::Rosen::RSCanvasNode> rootCanvasNode_ = nullptr;

    std::unordered_map<std::string, std::function<void(InterfaceClientFrameTest &)>> execFuncMap;
};
#endif
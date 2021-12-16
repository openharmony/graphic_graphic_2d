/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "wmservice_native_test_6.h"

#include <gslogger.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMServiceNativeTest6 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "rotation animation";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmservice";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 6;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 500;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto wmsc = WindowManagerServiceClient::GetInstance();
        wmsc->Init();
        auto wms = wmsc->GetService();
        auto gret = wms->StartRotationAnimation(0, 90);
        GSLOG2SO(INFO) << "return: " << GSErrorStr(gret);
    }
} g_autoload;
} // namespace

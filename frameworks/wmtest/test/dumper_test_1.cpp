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

#include "other_native_test_1.h"

#include <csignal>
#include <string>
#include <unistd.h>

#include <graphic_dumper_helper.h>
#include <option_parser.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class DumperTest1 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "dumper native test";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "!dumper";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 1;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    static void Handler(int32_t signal)
    {
        int32_t signals[] = {SIGINT, SIGKILL, SIGTERM, SIGTSTP, SIGQUIT, SIGHUP};
        for (uint32_t i = 0; i < sizeof(signals) / sizeof(*signals); i++) {
            if (signals[i] == signal) {
                exit(0);
                break;
            }
        }
    }

    void Run(int32_t argc, const char **argv) override
    {
        handler = AppExecFwk::EventHandler::Current();
        printf("graphic dumper test start!\n");
        std::signal(SIGINT, Handler);
        std::signal(SIGKILL, Handler);
        std::signal(SIGTERM, Handler);
        std::signal(SIGTSTP, Handler);
        std::signal(SIGQUIT, Handler);
        std::signal(SIGHUP, Handler);

        OptionParser parser;
        parser.AddArguments(tagInfo);
        parser.Parse(argc, argv);

        dumper = GraphicDumperHelper::GetInstance();
        configListener = dumper->AddConfigChangeListener(tagInfo + ".info",
            std::bind(&DumperTest1::OnConfigChange, this, std::placeholders::_1, std::placeholders::_2));
        dumpListener = dumper->AddDumpListener(tagInfo + ".info", std::bind(&DumperTest1::OnDump, this));

        AfterRun();
    }

    void AfterRun()
    {
        auto str = "ABCEDFGHIG0123456789ABCEDFGHIG0123456789ABCEDFGHIG0123456789";
        int32_t ret = dumper->SendInfo("log." + tagInfo, "[%d]%s just for test log %d!!!\n", getpid(), str, count++);
        if (ret != 0) {
            printf("graphic dumper service died!\n");
            ExitDump();
            return;
        }

        ret = dumper->SendInfo("log." + tagInfo + ".1", "[%d]%s\njust for test2 log %d!!!\n", getpid(), str, count++);
        if (ret != 0) {
            printf("graphic dumper service died!\n");
            ExitDump();
        }

        constexpr int32_t delayTime = 10;
        PostTask(std::bind(&DumperTest1::AfterRun, this), delayTime);
    }

    void ExitDump()
    {
        if (configListener != 0) {
            dumper->RemoveConfigChangeListener(configListener);
        }
        if (dumpListener != 0) {
            dumper->RemoveDumpListener(dumpListener);
        }
        ExitTest();
    }

    void OnConfigChange(const std::string &key, const std::string &val)
    {
        auto func = [this]() {
            auto str = "********** !!! just for test send info !!! **********\n";
            int32_t ret = dumper->SendInfo(tagInfo, str);
            printf("graphic dumper ret = %d\n", ret);
            if (ret != 0) {
                printf("graphic dumper service died!\n");
                handler->PostTask(std::bind(&DumperTest1::ExitDump, this));
            }
        };

        printf("%s -> %s\n", key.c_str(), val.c_str());
        if (key.find("info") != std::string::npos && val.compare("true") == 0) {
            handler->PostTask(func);
        }
    }

    void OnDump()
    {
        printf("OnDump\n");
        dumper->SendInfo(tagInfo, "pid[%d] send dump info \n", getpid());
    }

private:
    static inline std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::string tagInfo = "A.B.info";
    sptr<GraphicDumperHelper> dumper = nullptr;
    int32_t configListener = 0;
    int32_t dumpListener = 0;
    int32_t count = 0;
} g_autoload;
} // namespace

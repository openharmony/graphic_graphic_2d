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

#ifndef SAFUZZ_ARGUMENT_PARSER_H
#define SAFUZZ_ARGUMENT_PARSER_H

#include <getopt.h>
#include <string>

#include "configs/safuzz_config.h"
#include "nocopyable.h"
#include "pipeline/config_manager.h"
#include "tools/ipc_utils.h"

namespace OHOS {
namespace Rosen {
class ArgumentParser {
public:
    ArgumentParser() = default;
    ~ArgumentParser() = default;

    bool Init(int argc, char *const *argv);
    bool Process();

private:
    DISALLOW_COPY_AND_MOVE(ArgumentParser);

    /*** init ***/
    bool ParseOpt(int opt);
    // -h --help
    void ShowHelp() const;
    // -p --permission
    bool SetSystemPermission() const;
    // -u --user
    bool SetUserType();
    // -c --config
    bool SetTestCaseConfigPath();
    // -i --iterations
    bool SetIterations();
    // -s --sendRequestTimes
    bool SetSendRequestTimes();
    // -t --testCaseDesc
    bool SetRunSingleTestCaseFlag();
    // -a --all
    void SetRunAllTestCasesFlag();

    /*** prepare ***/
    bool PrepareTestEnvironment();
    bool PrepareUserInfo();
    bool PrepareConfigManager();
    bool PrepareConnectionProxy();

    /*** process ***/
    bool RunSingleTestCase();
    bool RunAllTestCases();
    bool RunTestCase(const TestCaseDesc& testCaseDesc) const;
    bool RunTestCaseWithParcelSplit(const TestCaseConfig& testCaseConfig) const;
    bool RunTestCaseWithoutParcelSplit(const TestCaseConfig& testCaseConfig) const;

private:
    /*** init ***/
    const char OPTION_STRING[14] = "hpu:c:i:s:t:a";
    const struct option LONG_OPTIONS[9] = { { "help", no_argument, nullptr, 'h' },
                                            { "systemPermission", no_argument, nullptr, 'p' },
                                            { "user", required_argument, nullptr, 'u' },
                                            { "config", required_argument, nullptr, 'c' },
                                            { "iterations", required_argument, nullptr, 'i' },
                                            { "sendRequestTimes", required_argument, nullptr, 's' },
                                            { "testCaseDesc", required_argument, nullptr, 't' },
                                            { "all", no_argument, nullptr, 'a' },
                                            { nullptr, 0, nullptr, 0 } };
    std::string userType_ = USER_TYPE_DEFAULT;
    std::string testCaseConfigPath_ = TEST_CASE_CONFIG_PATH_DEFAULT;
    int iterations_ = TEST_CASE_ITERATIONS_DEFAULT;
    int sendRequestTimes_ = SEND_REQUEST_TIMES_DEFAULT;
    TestCaseDesc singleTestCaseDesc_;
    bool runSingleTestCaseFlag_ = false;
    bool runAllTestCasesFlag_ = false;

    /*** prepare ***/
    bool isTestEnvironmentPrepared_ = false;
    ConfigManager configManager_;
    sptr<IRemoteObject> connProxy_ = nullptr;
    std::u16string connProxyDescriptor_;
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_ARGUMENT_PARSER_H

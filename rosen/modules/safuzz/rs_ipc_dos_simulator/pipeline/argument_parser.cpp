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

#include "pipeline/argument_parser.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <pthread.h>
#include <pwd.h>
#include <unistd.h>
#include <vector>

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "string_ex.h"

#include "configs/safuzz_config.h"
#include "configs/test_case_config.h"
#include "common/safuzz_log.h"
#include "ipc/message_parcel_utils.h"
#include "ipc/rs_irender_service_connection_ipc_interface_code_utils.h"
#include "pipeline/safuzz_thread.h"
#include "platform/ohos/rs_irender_service_ipc_interface_code.h"
#include "tools/token_utils.h"

namespace OHOS {
namespace Rosen {
bool ArgumentParser::Init(int argc, char *const *argv)
{
    int opt = -1;
    while ((opt = getopt_long(argc, argv, OPTION_STRING, LONG_OPTIONS, nullptr)) != -1) {
        if (!ParseOpt(opt)) {
            SAFUZZ_LOGE("ArgumentParser::Init option '-%c' failed", static_cast<char>(opt));
            return false;
        }
    }
    return true;
}

bool ArgumentParser::Process()
{
    if (runSingleTestCaseFlag_ && !RunSingleTestCase()) {
        SAFUZZ_LOGE("ArgumentParser::Process RunSingleTestCase failed");
        return false;
    }
    if (runAllTestCasesFlag_ && !RunAllTestCases()) {
        SAFUZZ_LOGE("ArgumentParser::Process RunAllTestCases failed");
        return false;
    }
    return true;
}

bool ArgumentParser::ParseOpt(int opt)
{
    bool ret = true;
    switch (opt) {
        case 'h': {
            ShowHelp();
            break;
        }
        case 'p': {
            ret = SetSystemPermission();
            break;
        }
        case 'u': {
            ret = SetUserType();
            break;
        }
        case 'c': {
            ret = SetTestCaseConfigPath();
            break;
        }
        case 'i': {
            ret = SetIterations();
            break;
        }
        case 's': {
            ret = SetSendRequestTimes();
            break;
        }
        case 't': {
            ret = SetRunSingleTestCaseFlag();
            break;
        }
        case 'a': {
            SetRunAllTestCasesFlag();
            break;
        }
        default: {
            SAFUZZ_LOGE("ArgumentParser::ParseOpt option '-%c' not supported", static_cast<char>(opt));
            ret = false;
            break;
        }
    }
    return ret;
}

void ArgumentParser::ShowHelp() const
{
    fprintf(stdout, "****** RenderService SAFuzzer Usage Guideline ******\n");
    fprintf(stdout,
        "  -h                                       |          |  help for the tool\n"
        "  --help                                   |          |  help for the tool\n"
        "  -p                                       |          |  add system_basic permission for SAFuzz process\n"
        "  --permission                             |          |  add system_basic permission for SAFuzz process\n"
        "  -u [USER_TYPE]                           |  string  |  user type: [root|system|shell], default: shell\n"
        "  --user [USER_TYPE]                       |  string  |  user type: [root|system|shell], default: shell\n"
        "  -c [CONFIG_PATH]                         |  string  |  test case config absolute/relative path, "
                                                                  "default: /data/local/tmp/test_case_config.json\n"
        "  --config [CONFIG_PATH]                   |  string  |  test case config absolute/relative path, "
                                                                  "default: /data/local/tmp/test_case_config.json\n"
        "  -i [TEST_CASE_ITERATIONS]                |  int     |  test case iterations, default: 100\n"
        "  --iterations [TEST_CASE_ITERATIONS]      |  int     |  test case iterations, default: 100\n"
        "  -s [SEND_REQUEST_TIMES]                  |  int     |  send request times in each thread, default: 1\n"
        "  --sendRequestTimes [SEND_REQUEST_TIMES]  |  int     |  send request times in each thread, default: 1\n"
        "  -t [TEST_CASE_DESCRIPTION]               |  string  |  run one test case\n"
        "  --testCaseDesc [TEST_CASE_DESCRIPTION]   |  string  |  run one test case\n"
        "  -a                                       |          |  run all test cases\n"
        "  --all                                    |          |  run all test cases\n\n");
}

bool ArgumentParser::SetSystemPermission() const
{
    if (!TokenUtils::ConfigureTokenInfo()) {
        SAFUZZ_LOGE("ArgumentParser::SetSystemPermission failed");
        return false;
    }
    return true;
}

bool ArgumentParser::SetUserType()
{
    if (optarg == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::SetUserType lack of user type param from command line");
        return false;
    }

    userType_ = std::string(optarg);
    if (userType_ != "root" && userType_ != "system" && userType_ != "shell") {
        SAFUZZ_LOGE("ArgumentParser::SetUserType user type %s can not be identified", userType_.c_str());
        return false;
    }
    return true;
}

bool ArgumentParser::SetTestCaseConfigPath()
{
    if (optarg == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::SetTestCaseConfigPath lack of testCaseConfigPath param from command line");
        return false;
    }

    testCaseConfigPath_ = std::string(optarg);
    if (testCaseConfigPath_.size() == 0) {
        SAFUZZ_LOGE("ArgumentParser::SetTestCaseConfigPath testCaseConfigPath is empty");
        return false;
    }
    return true;
}

bool ArgumentParser::SetIterations()
{
    if (optarg == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::SetIterations lack of iterations param from command line");
        return false;
    }

    int iterations = strtol(optarg, nullptr, 10);
    if (iterations <= 0) {
        SAFUZZ_LOGE("ArgumentParser::SetIterations iterations param should be a positive integer");
        return false;
    }
    iterations_ = iterations;
    return true;
}

bool ArgumentParser::SetSendRequestTimes()
{
    if (optarg == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::SetSendRequestTimes lack of sendRequestTimes param from command line");
        return false;
    }

    int sendRequestTimes = strtol(optarg, nullptr, 10);
    if (sendRequestTimes <= 0) {
        SAFUZZ_LOGE("ArgumentParser::SetSendRequestTimes sendRequestTimes param should be a positive integer");
        return false;
    }
    sendRequestTimes_ = sendRequestTimes;
    return true;
}

bool ArgumentParser::SetRunSingleTestCaseFlag()
{
    if (optarg == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::SetRunSingleTestCaseFlag lack of testCaseDesc param from command line");
        return false;
    }
    singleTestCaseDesc_ = std::string(optarg);
    runSingleTestCaseFlag_ = true;
    return true;
}

void ArgumentParser::SetRunAllTestCasesFlag()
{
    runAllTestCasesFlag_ = true;
}

bool ArgumentParser::PrepareTestEnvironment()
{
    if (isTestEnvironmentPrepared_) {
        return true;
    }
    bool success = PrepareUserInfo() && PrepareConfigManager() && PrepareConnectionProxy();
    if (success) {
        isTestEnvironmentPrepared_ = true;
    } else {
        SAFUZZ_LOGE("ArgumentParser::PrepareTestEnvironment failed");
    }
    return success;
}

bool ArgumentParser::PrepareUserInfo()
{
    struct passwd* pw = getpwnam(userType_.c_str());
    if (pw == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::PrepareUserInfo passwd is nullptr");
        return false;
    }
    setuid(pw->pw_uid);
    setgid(pw->pw_gid);
    return true;
}

bool ArgumentParser::PrepareConfigManager()
{
    bool success = configManager_.Init(testCaseConfigPath_);
    if (!success) {
        SAFUZZ_LOGE("ArgumentParser::PrepareConfigManager failed");
        return false;
    }
    return true;
}

bool ArgumentParser::PrepareConnectionProxy()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::PrepareConnectionProxy systemAbilityManager is nullptr");
        return false;
    }

    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(RS_SAID);
    if (object == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::PrepareConnectionProxy no service found for said %d", RS_SAID);
        return false;
    }

    IPCObjectProxy* proxy = reinterpret_cast<IPCObjectProxy*>(object.GetRefPtr());
    std::u16string descriptor = proxy->GetInterfaceDescriptor();
    SAFUZZ_LOGI("ArgumentParser::PrepareConnectionProxy descriptor is %s", Str16ToStr8(descriptor).c_str());

    if (!data.WriteInterfaceToken(descriptor)) {
        SAFUZZ_LOGE("ArgumentParser::PrepareConnectionProxy WriteInterfaceToken failed");
        return false;
    }

    sptr<CustomizedRemoteStub> remoteStub = new (std::nothrow) CustomizedRemoteStub();
    if (remoteStub == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::PrepareConnectionProxy remoteStub is nullptr");
        return false;
    }

    if (!data.WriteRemoteObject(remoteStub)) {
        SAFUZZ_LOGE("ArgumentParser::PrepareConnectionProxy WriteRemoteObject failed");
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::CREATE_CONNECTION);
    int status = object->SendRequest(code, data, reply, option);
    SAFUZZ_LOGI("ArgumentParser::PrepareConnectionProxy SendRequest status %d", status);

    connProxy_ = reply.ReadRemoteObject();
    if (connProxy_ == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::PrepareConnectionProxy get connection proxy failed");
        return false;
    }

    connProxyDescriptor_ = connProxy_->GetInterfaceDescriptor();
    SAFUZZ_LOGI("ArgumentParser::PrepareConnectionProxy inner descriptor is %s",
        Str16ToStr8(connProxyDescriptor_).c_str());
    return true;
}

bool ArgumentParser::RunSingleTestCase()
{
    if (!PrepareTestEnvironment()) {
        SAFUZZ_LOGE("ArgumentParser::RunSingleTestCase prepare test environment failed");
        return false;
    }

    if (!RunTestCase(singleTestCaseDesc_)) {
        SAFUZZ_LOGE("ArgumentParser::RunSingleTestCase %s failed", singleTestCaseDesc_.c_str());
        return false;
    }
    SAFUZZ_LOGI("ArgumentParser::RunSingleTestCase %s success", singleTestCaseDesc_.c_str());
    return true;
}

bool ArgumentParser::RunAllTestCases()
{
    if (!PrepareTestEnvironment()) {
        SAFUZZ_LOGE("ArgumentParser::RunAllTestCases prepare test environment failed");
        return false;
    }

    int testCaseTotal = configManager_.GetTestCaseTotal();
    int testCaseSuccessTotal = 0;
    for (int testCaseId = 1;; testCaseId++) {
        std::optional<TestCaseDesc> testCaseDesc = configManager_.GetNextTestCaseDesc();
        if (!testCaseDesc.has_value()) {
            break;
        }
        SAFUZZ_LOGI("ArgumentParser::RunAllTestCases [%d/%d] %s start",
            testCaseId, testCaseTotal, testCaseDesc.value().c_str());
        if (!RunTestCase(testCaseDesc.value())) {
            SAFUZZ_LOGE("ArgumentParser::RunAllTestCases [%d/%d] %s failed",
                testCaseId, testCaseTotal, testCaseDesc.value().c_str());
            continue;
        }
        testCaseSuccessTotal++;
        SAFUZZ_LOGI("ArgumentParser::RunAllTestCases [%d/%d] %s end normally",
            testCaseId, testCaseTotal, testCaseDesc.value().c_str());
    }
    int testCaseFailedTotal = testCaseTotal - testCaseSuccessTotal;
    SAFUZZ_LOGI("ArgumentParser::RunAllTestCases summary: success %d / failed %d / total %d",
        testCaseSuccessTotal, testCaseFailedTotal, testCaseTotal);
    return testCaseFailedTotal == 0;
}

bool ArgumentParser::RunTestCase(const TestCaseDesc& testCaseDesc) const
{
    SAFUZZ_LOGI("ArgumentParser::RunTestCase %s start", testCaseDesc.c_str());

    if (connProxy_ == nullptr) {
        SAFUZZ_LOGE("ArgumentParser::RunTestCase connectionProxy is nullptr");
        return false;
    }

    std::optional<TestCaseConfig> testCaseConfig = configManager_.GetTestCaseConfigByDesc(testCaseDesc);
    if (!testCaseConfig.has_value()) {
        SAFUZZ_LOGE("ArgumentParser::RunTestCase get testCaseConfig failed");
        return false;
    }

    std::string interfaceName = testCaseConfig.value().interfaceName;
    bool isParcelSplitEnabled = RSIRenderServiceConnectionInterfaceCodeUtils::IsParcelSplitEnabled(interfaceName);
    if (isParcelSplitEnabled && testCaseConfig.value().writeInterfaceToken) {
        SAFUZZ_LOGE("ArgumentParser::RunTestCase %s writeInterfaceToken is not supported in %s",
            testCaseDesc.c_str(), interfaceName.c_str());
        return false;
    }

    for (int iter = 0; iter < iterations_; iter++) {
        SAFUZZ_LOGI("ArgumentParser::RunTestCase %s iter[%d/%d] start",
            testCaseDesc.c_str(), iter + 1, iterations_);

        if (isParcelSplitEnabled) {
            if (!RunTestCaseWithParcelSplit(testCaseConfig.value())) {
                SAFUZZ_LOGE("ArgumentParser::RunTestCase %s iter[%d/%d] RunTestCaseWithParcelSplit failed",
                    testCaseDesc.c_str(), iter + 1, iterations_);
                continue;
            }
        } else {
            if (!RunTestCaseWithoutParcelSplit(testCaseConfig.value())) {
                SAFUZZ_LOGE("ArgumentParser::RunTestCase %s iter[%d/%d] RunTestCaseWithoutParcelSplit failed",
                    testCaseDesc.c_str(), iter + 1, iterations_);
                continue;
            }
        }

        SAFUZZ_LOGI("ArgumentParser::RunTestCase %s iter[%d/%d] end normally",
            testCaseDesc.c_str(), iter + 1, iterations_);
    }

    SAFUZZ_LOGI("ArgumentParser::RunTestCase %s end normally", testCaseDesc.c_str());
    return true;
}

bool ArgumentParser::RunTestCaseWithParcelSplit(const TestCaseConfig& testCaseConfig) const
{
    TestCaseParams testCaseParams = { .inputParams       = testCaseConfig.inputParams,
                                      .commandList       = testCaseConfig.commandList,
                                      .commandListRepeat = testCaseConfig.commandListRepeat };

    std::vector<std::shared_ptr<MessageParcel>> data;
    if (!MessageParcelUtils::WriteParamsToVector(data, testCaseParams)) {
        SAFUZZ_LOGE("ArgumentParser::RunTestCaseWithParcelSplit WriteParams failed");
        return false;
    }

    MessageParcel reply[SAFUZZ_THREAD_COUNT];

    MessageOption option;
    if (!MessageParcelUtils::SetOption(option, testCaseConfig.messageOption)) {
        SAFUZZ_LOGE("ArgumentParser::RunTestCaseWithParcelSplit SetOption failed");
        return false;
    }

    MultiDataSaFuzzThreadParams threadParams[SAFUZZ_THREAD_COUNT];
    for (size_t i = 0; i < SAFUZZ_THREAD_COUNT; i++) {
        threadParams[i] = { .sa     = connProxy_,
                            .code   = testCaseConfig.interfaceCode,
                            .data   = &data,
                            .reply  = &reply[i],
                            .option = &option,
                            .loops  = sendRequestTimes_ };
    }

    // trigger race
    pthread_t threads[SAFUZZ_THREAD_COUNT];
    for (size_t i = 0; i < SAFUZZ_THREAD_COUNT; i++) {
        pthread_create(&threads[i], nullptr, MultiDataSaFuzzThread, static_cast<void*>(&threadParams[i]));
    }
    for (size_t i = 0; i < SAFUZZ_THREAD_COUNT; i++) {
        pthread_join(threads[i], nullptr);
    }
    return true;
}

bool ArgumentParser::RunTestCaseWithoutParcelSplit(const TestCaseConfig& testCaseConfig) const
{
    TestCaseParams testCaseParams = { .inputParams       = testCaseConfig.inputParams,
                                      .commandList       = testCaseConfig.commandList,
                                      .commandListRepeat = testCaseConfig.commandListRepeat };

    MessageParcel data;
    if (testCaseConfig.writeInterfaceToken && !data.WriteInterfaceToken(connProxyDescriptor_)) {
        SAFUZZ_LOGE("ArgumentParser::RunTestCaseWithoutParcelSplit WriteInterfaceToken failed");
        return false;
    }
    if (!MessageParcelUtils::WriteParams(data, testCaseParams)) {
        SAFUZZ_LOGE("ArgumentParser::RunTestCaseWithoutParcelSplit WriteParams failed");
        return false;
    }

    MessageParcel reply[SAFUZZ_THREAD_COUNT];

    MessageOption option;
    if (!MessageParcelUtils::SetOption(option, testCaseConfig.messageOption)) {
        SAFUZZ_LOGE("ArgumentParser::RunTestCaseWithoutParcelSplit SetOption failed");
        return false;
    }

    SaFuzzThreadParams threadParams[SAFUZZ_THREAD_COUNT];
    for (size_t i = 0; i < SAFUZZ_THREAD_COUNT; i++) {
        threadParams[i] = { .sa     = connProxy_,
                            .code   = testCaseConfig.interfaceCode,
                            .data   = &data,
                            .reply  = &reply[i],
                            .option = &option,
                            .loops  = sendRequestTimes_ };
    }

    // trigger race
    pthread_t threads[SAFUZZ_THREAD_COUNT];
    for (size_t i = 0; i < SAFUZZ_THREAD_COUNT; i++) {
        pthread_create(&threads[i], nullptr, SaFuzzThread, static_cast<void*>(&threadParams[i]));
    }
    for (size_t i = 0; i < SAFUZZ_THREAD_COUNT; i++) {
        pthread_join(threads[i], nullptr);
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS

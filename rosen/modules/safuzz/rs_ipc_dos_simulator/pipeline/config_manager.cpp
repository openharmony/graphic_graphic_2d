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

#include "pipeline/config_manager.h"

#include <fstream>
#include <sstream>

#include "common/safuzz_log.h"
#include "ipc/rs_irender_service_connection_ipc_interface_code_utils.h"

namespace OHOS {
namespace Rosen {
bool ConfigManager::Init(const std::string& testCaseConfigPath)
{
    testCaseConfigPath_ = testCaseConfigPath;
    return InitRoot() && InitConfig();
}

bool ConfigManager::InitRoot()
{
    std::ifstream configFile;
    configFile.open(testCaseConfigPath_);
    if (!configFile.is_open()) {
        SAFUZZ_LOGE("ConfigManager::InitRoot can not open config file");
        return false;
    }
    std::stringstream filterParamsStream;
    filterParamsStream << configFile.rdbuf();
    configFile.close();
    paramsString_ = filterParamsStream.str();

    root_ = cJSON_Parse(paramsString_.c_str());
    if (root_ == nullptr) {
        SAFUZZ_LOGE("ConfigManager::InitRoot can not parse config to json");
        return false;
    }
    if (!cJSON_IsArray(root_)) {
        SAFUZZ_LOGE("ConfigManager::InitRoot root is not json array");
        return false;
    }
    testCaseTotal_ = cJSON_GetArraySize(root_);
    return true;
}

bool ConfigManager::InitConfig()
{
    for (int testCaseIndex = 0; testCaseIndex < testCaseTotal_; ++testCaseIndex) {
        cJSON* testCaseItem = cJSON_GetArrayItem(root_, testCaseIndex);
        if (testCaseItem == nullptr) {
            SAFUZZ_LOGE("ConfigManager::InitConfig can not get testCaseItem from root");
            return false;
        }
        if (!cJSON_IsObject(testCaseItem)) {
            SAFUZZ_LOGE("ConfigManager::InitConfig testCaseItem is not json object");
            return false;
        }

        cJSON* testCaseDescJson = cJSON_GetObjectItem(testCaseItem, "testCaseDesc");
        if (testCaseDescJson == nullptr || !cJSON_IsString(testCaseDescJson)) {
            SAFUZZ_LOGE("ConfigManager::InitConfig can not get testCaseDesc from testCaseItem");
            return false;
        }
        std::string testCaseDesc = static_cast<std::string>(testCaseDescJson->valuestring);
        if (configs_.find(testCaseDesc) != configs_.end()) {
            SAFUZZ_LOGE("ConfigManager::InitConfig duplicate testCaseDesc %s", testCaseDesc.c_str());
            return false;
        }

        cJSON* interfaceNameJson = cJSON_GetObjectItem(testCaseItem, "interfaceName");
        if (interfaceNameJson == nullptr || !cJSON_IsString(interfaceNameJson)) {
            SAFUZZ_LOGE("ConfigManager::InitConfig can not get interfaceName from testCaseItem");
            return false;
        }
        std::string interfaceName = static_cast<std::string>(interfaceNameJson->valuestring);

        int interfaceCode = RSIRenderServiceConnectionInterfaceCodeUtils::GetCodeFromName(interfaceName);
        if (interfaceCode < 0) {
            SAFUZZ_LOGE("ConfigManager::InitConfig can not identify interfaceName %s", interfaceName.c_str());
            return false;
        }

        cJSON* inputParamsJson = cJSON_GetObjectItem(testCaseItem, "inputParams");
        if (inputParamsJson == nullptr || !cJSON_IsString(inputParamsJson)) {
            SAFUZZ_LOGE("ConfigManager::InitConfig can not get inputParams from testCaseItem");
            return false;
        }
        std::string inputParams = static_cast<std::string>(inputParamsJson->valuestring);

        cJSON* messageOptionJson = cJSON_GetObjectItem(testCaseItem, "messageOption");
        if (messageOptionJson == nullptr || !cJSON_IsString(messageOptionJson)) {
            SAFUZZ_LOGE("ConfigManager::InitConfig can not get messageOption from testCaseItem");
            return false;
        }
        std::string messageOption = static_cast<std::string>(messageOptionJson->valuestring);

        cJSON* writeInterfaceTokenJson = cJSON_GetObjectItem(testCaseItem, "writeInterfaceToken");
        if (writeInterfaceTokenJson == nullptr || !cJSON_IsBool(writeInterfaceTokenJson)) {
            SAFUZZ_LOGE("ConfigManager::InitConfig can not get writeInterfaceToken from testCaseItem");
            return false;
        }
        bool writeInterfaceToken = static_cast<bool>(writeInterfaceTokenJson->valueint);

        std::string commandList = "";
        cJSON* commandListJson = cJSON_GetObjectItem(testCaseItem, "commandList");
        if (commandListJson != nullptr) {
            if (!cJSON_IsString(commandListJson)) {
                SAFUZZ_LOGE("ConfigManager::InitConfig can not get commandList from testCaseItem");
                return false;
            }
            commandList = static_cast<std::string>(commandListJson->valuestring);
        }

        int commandListRepeat = 0;
        cJSON* commandListRepeatJson = cJSON_GetObjectItem(testCaseItem, "commandListRepeat");
        if (commandListRepeatJson != nullptr) {
            if (!cJSON_IsNumber(commandListRepeatJson)) {
                SAFUZZ_LOGE("ConfigManager::InitConfig can not get commandListRepeat from testCaseItem");
                return false;
            }
            commandListRepeat = static_cast<int>(commandListRepeatJson->valueint);
            if (commandListRepeat < 0) {
                SAFUZZ_LOGE("ConfigManager::InitConfig commandListRepeat %d is negative", commandListRepeat);
                return false;
            }
        }

        TestCaseConfig testCaseConfig = { .testCaseDesc        = testCaseDesc,
                                          .testCaseIndex       = testCaseIndex,
                                          .interfaceCode       = interfaceCode,
                                          .interfaceName       = interfaceName,
                                          .inputParams         = inputParams,
                                          .messageOption       = messageOption,
                                          .writeInterfaceToken = writeInterfaceToken,
                                          .commandList         = commandList,
                                          .commandListRepeat   = commandListRepeat };

        descriptions_.push_back(testCaseDesc);
        configs_.emplace(testCaseDesc, testCaseConfig);
    }
    return true;
}

int ConfigManager::GetTestCaseTotal() const
{
    return testCaseTotal_;
}

std::optional<TestCaseDesc> ConfigManager::GetNextTestCaseDesc()
{
    if (testCaseIndex_ >= testCaseTotal_) {
        return std::nullopt;
    }
    TestCaseDesc desc = descriptions_[testCaseIndex_++];
    return desc;
}

std::optional<TestCaseConfig> ConfigManager::GetTestCaseConfigByDesc(const TestCaseDesc& desc) const
{
    auto it = configs_.find(desc);
    if (it == configs_.end()) {
        SAFUZZ_LOGE("ConfigManager::GetTestCaseConfigByDesc desc %s not found", desc.c_str());
        return std::nullopt;
    }
    return it->second;
}
} // namespace Rosen
} // namespace OHOS

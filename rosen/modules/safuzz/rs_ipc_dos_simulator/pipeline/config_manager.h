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

#ifndef SAFUZZ_CONFIG_MANAGER_H
#define SAFUZZ_CONFIG_MANAGER_H

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "cJSON.h"

#include "configs/safuzz_config.h"
#include "configs/test_case_config.h"
#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
class ConfigManager {
public:
    ConfigManager() = default;
    ~ConfigManager() = default;

    bool Init(const std::string& testCaseConfigPath = TEST_CASE_CONFIG_PATH_DEFAULT);
    int GetTestCaseTotal() const;
    std::optional<TestCaseDesc> GetNextTestCaseDesc();
    std::optional<TestCaseConfig> GetTestCaseConfigByDesc(const TestCaseDesc& desc) const;

private:
    DISALLOW_COPY_AND_MOVE(ConfigManager);

    bool InitRoot();
    bool InitConfig();

    std::string testCaseConfigPath_ = TEST_CASE_CONFIG_PATH_DEFAULT;
    std::string paramsString_;
    cJSON* root_ = nullptr;
    int testCaseTotal_ = 0;
    int testCaseIndex_ = 0;
    std::vector<TestCaseDesc> descriptions_;
    std::unordered_map<TestCaseDesc, TestCaseConfig> configs_;
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_CONFIG_MANAGER_H

 /*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYSTEM_PARAMETERS_MOCK_H
#define SYSTEM_PARAMETERS_MOCK_H

#include <memory>
#include <string>

#include "gmock/gmock.h"

namespace OHOS {
namespace system {

class ParameterInterface {
public:
    virtual ~ParameterInterface() = default;
    virtual bool GetBoolParameter(const std::string& key, bool def) = 0;
};

class ParameterMock : public ParameterInterface {
public:
    MOCK_METHOD(bool, GetBoolParameter, (const std::string& key, bool def), (override));
};

void SetParameterGlobalMock(std::shared_ptr<ParameterMock> ptr);

/*
 * Returns true if the system parameter `key` has the value "1", "y", "yes", "on", or "true",
 * false for "0", "n", "no", "off", or "false", or `def` otherwise.
 */
bool GetBoolParameter(const std::string& key, bool def);

} // namespace system
} // namespace OHOS

#endif // SYSTEM_PARAMETERS_MOCK_H
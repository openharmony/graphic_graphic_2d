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

#ifndef SAFUZZ_TEST_CASE_CONFIG_H
#define SAFUZZ_TEST_CASE_CONFIG_H

#include <string>

namespace OHOS {
namespace Rosen {
using TestCaseDesc = std::string;

struct TestCaseConfig {
    TestCaseDesc testCaseDesc;
    int testCaseIndex;
    int interfaceCode;
    std::string interfaceName;
    std::string inputParams;
    std::string messageOption;
    bool writeInterfaceToken;
    std::string commandList;
    int commandListRepeat;
};

struct TestCaseParams {
    std::string inputParams;
    std::string commandList;
    int commandListRepeat;
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_TEST_CASE_CONFIG_H

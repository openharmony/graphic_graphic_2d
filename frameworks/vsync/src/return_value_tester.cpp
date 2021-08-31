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

#include "return_value_tester.h"
namespace OHOS {
namespace Vsync {
sptr<ReturnValueTester> ReturnValueTester::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new ReturnValueTester();
        }
    }
    return instance;
}

void ReturnValueTester::SetValue(int next, const std::any &rval)
{
    anyMap[next + id] = rval;
}

bool ReturnValueTester::HasReturnValue()
{
    return anyMap.find(id++) != anyMap.end();
}

std::any ReturnValueTester::GetValue()
{
    auto ret = anyMap[id - 1];
    anyMap.erase(id - 1);
    return ret;
}
} // namespace Vsync
} // namespace OHOS

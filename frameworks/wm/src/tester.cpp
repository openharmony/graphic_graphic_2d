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

#include "tester.h"

#include <unistd.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMTester"};
} // namespace

Tester& Tester::Get()
{
    static Tester instance;
    return instance;
}

bool Tester::NeedReturnTestNew(const std::string &str)
{
    bool isFound = set.find(str) != set.end();
    WMLOGFD("NeedReturnTestNew: %{public}s %{public}d", str.c_str(), isFound);
    return isFound;
}

void *Tester::ReturnTestNew(const std::string &str)
{
    auto ret = ptrs[str];
    WMLOGFD("ReturnTestNew: %{public}s %{public}p", str.c_str(), ret);
    set.erase(str);
    ptrs.erase(str);
    return ret;
}

void Tester::SetTestNew(const std::string &str, void *ptr)
{
    WMLOGFD("SetTestNew: %{public}s %{public}p", str.c_str(), ptr);
    set.insert(str);
    ptrs[str] = ptr;
}

void Tester::ScheduleForConcurrent()
{
    if (needSchedule == true) {
        WMLOGFD("need schedule");
        needSchedule = false;
        sleep(0);
    }
}

void Tester::SetSchedule()
{
    WMLOGFD("set schedule");
    needSchedule = true;
}
} // namespace OHOS

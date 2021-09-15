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

#ifndef FRAMEWORKS_WM_INCLUDE_TESTER_H
#define FRAMEWORKS_WM_INCLUDE_TESTER_H

#include <map>
#include <set>
#include <string>

#define TESTER_NEW(type, ...) (                                        \
        Tester::Get().NeedReturnTestNew(#type) ?                       \
        reinterpret_cast<type *>(Tester::Get().ReturnTestNew(#type)) : \
        new type(##__VA_ARGS__)                                        \
    )

namespace OHOS {
class Tester {
public:
    static Tester &Get();

    bool NeedReturnTestNew(const std::string &str);
    void *ReturnTestNew(const std::string &str);
    void SetTestNew(const std::string &str, void *ptr);

    void ScheduleForConcurrent();
    void SetSchedule();

private:
    Tester() = default;
    ~Tester() = default;

    std::set<std::string> set;
    std::map<std::string, void *> ptrs;

    bool needSchedule = false;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_TESTER_H

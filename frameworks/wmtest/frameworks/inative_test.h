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

#ifndef FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_H
#define FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_H

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace OHOS {
class INativeTest;
using VisitTestFunc = std::function<void(INativeTest *)>;

class INativeTest {
public:
    enum {
        LAST_TIME_FOREVER = 999999999,
    };

    static void VisitTests(VisitTestFunc func);
    INativeTest();
    virtual ~INativeTest() = default;

    virtual std::string GetDescription() const = 0;
    virtual std::string GetDomain() const = 0;
    virtual int32_t GetID() const = 0;
    virtual uint32_t GetLastTime() const = 0;
    virtual void Run(int32_t argc, const char **argv) = 0;
private:
    static inline std::vector<INativeTest *> tests;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_H

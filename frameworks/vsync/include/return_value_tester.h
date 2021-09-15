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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_RETURN_VALUE_TESTER_H
#define FRAMEWORKS_VSYNC_INCLUDE_RETURN_VALUE_TESTER_H

#include <any>
#include <refbase.h>
#include <mutex>
#include <map>

namespace OHOS {
namespace Vsync {
class ReturnValueTester : public RefBase {
public:
    static sptr<ReturnValueTester> GetInstance();
    void SetValue(int next, const std::any &rval);
    std::any GetValue();
    bool HasReturnValue();

    template<class T>
    static void Set(const T &next, const T &returnvalue)
    {
        ReturnValueTester::GetInstance()->SetValue(next, returnvalue);
    }

    template<class T>
    static T Get(const T &b)
    {
        auto tester = ReturnValueTester::GetInstance();
        if (tester->HasReturnValue()) {
            T ret = b;
            const std::any &retval = tester->GetValue();
            auto pRet = std::any_cast<T>(&retval);
            if (pRet != nullptr) {
                ret = *pRet;
            } else {
                fprintf(stderr, "pRet is null\n");
                ret = b;
            }
            return ret;
        } else {
            return b;
        }
    }

private:
    ReturnValueTester() = default;
    virtual ~ReturnValueTester() = default;
    static inline sptr<ReturnValueTester> instance = nullptr;
    std::map<int, std::any> anyMap;
    int id = 0;
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_RETURN_VALUE_TESTER_H

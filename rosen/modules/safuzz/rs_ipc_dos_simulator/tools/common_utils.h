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

#ifndef SAFUZZ_COMMON_UTILS_H
#define SAFUZZ_COMMON_UTILS_H

#include <functional>
#include <string>
#include <type_traits>
#include <vector>

#include "string_ex.h"

#include "common/safuzz_log.h"

namespace OHOS {
namespace Rosen {
class CommonUtils {
public:
    static std::vector<std::string> SplitString(const std::string& str, const char delim);

    template<typename T>
    static std::string PrintVector(std::vector<T> dataVec, std::function<std::string(T)> dataPrinter)
    {
        if (dataPrinter == nullptr) {
            SAFUZZ_LOGE("CommonUtils::PrintVector dataPrinter is nullptr");
            return "";
        }
        std::string res = "{ ";
        int count = 0;
        for (const T& data : dataVec) {
            if (++count > PRINT_COUNT_UPPER_BOUND) {
                res += "... ";
                break;
            }
            res += dataPrinter(data);
            res += ", ";
        }
        res += "}";
        return res;
    }

    template<typename T>
    static inline const std::function<std::string(T)> BASIC_PRINTER = [](T data) {
        return std::to_string(data);
    };

    static inline const std::function<std::string(std::string)> STRING_PRINTER = [](std::string data) {
        return data;
    };

    static inline const std::function<std::string(std::u16string)> U16STRING_PRINTER = [](std::u16string data) {
        return Str16ToStr8(data);
    };

private:
    static constexpr int PRINT_COUNT_UPPER_BOUND = 10;
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_COMMON_UTILS_H

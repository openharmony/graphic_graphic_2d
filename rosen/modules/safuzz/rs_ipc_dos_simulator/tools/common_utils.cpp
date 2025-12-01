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

#include <cstdlib>
#include <sstream>

#include "tools/common_utils.h"

namespace OHOS {
namespace Rosen {
std::vector<std::string> CommonUtils::SplitString(const std::string& str, const char delim)
{
    std::vector<std::string> res;
    std::istringstream iss(str);
    std::string token;
    while (getline(iss, token, delim)) {
        res.push_back(token);
    }
    return res;
}

std::optional<float> CommonUtils::StringToFloat(const std::string& str)
{
    char* end = nullptr;
    errno = 0;
    float val = strtof(str.c_str(), &end);
    if (end == str) {
        SAFUZZ_LOGE("Conversion failed: No valid number.");
        return std::nullopt;
    }
    if (errno == ERANGE) {
        SAFUZZ_LOGE("Conversion overflow or underflow.");
        return std::nullopt;
    }
    return val;
}

std::optional<double> CommonUtils::StringToDouble(const std::string& str)
{
    char* end = nullptr;
    errno = 0;
    float val = strtod(str.c_str(), &end);
    if (end == str) {
        SAFUZZ_LOGE("Conversion failed: No valid number.");
        return std::nullopt;
    }
    if (errno == ERANGE) {
        SAFUZZ_LOGE("Conversion overflow or underflow.");
        return std::nullopt;
    }
    return val;
}
} // namespace Rosen
} // namespace OHOS

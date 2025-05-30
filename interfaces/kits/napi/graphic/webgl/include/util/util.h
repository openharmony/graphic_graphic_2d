/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef WEBGL_UTIL_H
#define WEBGL_UTIL_H

#include <vector>
#include <string>
#include <cerrno>
#include <cstdlib>
#include "log.h"
#include "util/object_manager.h"
#include "napi/n_exporter.h"
#include "context/webgl_rendering_context_basic_base.h"
#include "context/webgl_context_attributes.h"

namespace OHOS {
namespace Rosen {
class Util {
public:
    static void SplitString(const std::string& str, std::vector<std::string>& vec, const std::string& pattern);
    static bool GetContextInfo(napi_env env, napi_value thisVar,
        std::string &contextId, std::vector<std::string> &info);
    static WebGLRenderingContextBasicBase *GetContextObject(napi_env env, napi_value thisVar);

    static inline int32_t StringToInt(const std::string& value)
    {
        errno = 0;
        char* pEnd = nullptr;
        int64_t result = std::strtol(value.c_str(), &pEnd, 10);
        if (pEnd == value.c_str() || (result < INT_MIN || result > INT_MAX) || errno == ERANGE) {
            return 0;
        } else {
            return result;
        }
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_UTIL_H

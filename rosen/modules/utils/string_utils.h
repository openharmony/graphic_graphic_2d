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

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <iostream>

#include <hilog/log.h>

#include "securec.h"


namespace OHOS {
namespace Rosen {
static constexpr HiviewDFX::HiLogLabel TAG = { LOG_CORE, 0, "StringUtils" };
constexpr int STRING_BUFFER_SIZE = 4096;
template <typename...Args>
void AppendFormat(std::string& out, const char* fmt, Args&& ... args)
{
    char buf[STRING_BUFFER_SIZE] = {0};
    int len = ::snprintf_s(buf, sizeof(buf), sizeof(buf)-1, fmt, args...);
    if (len <= 0) {
        HiviewDFX::HiLog::Error(TAG, "failed to excute snprintf.");
        return;
    }
    out += buf;
}
}
}
#endif
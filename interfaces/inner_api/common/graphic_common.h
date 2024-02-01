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

#ifndef INTERFACES_INNERKITS_COMMON_GRAPHIC_COMMON_H
#define INTERFACES_INNERKITS_COMMON_GRAPHIC_COMMON_H

#ifdef __cplusplus
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <ostream>
#include <unistd.h>

namespace OHOS {
#endif

#include "graphic_common_c.h"

#ifdef __cplusplus
namespace {
const std::map<GSError, std::string> GSErrorStrs = {
    {GSERROR_OK,                    "<200 ok>"},
    {GSERROR_INVALID_ARGUMENTS,     "<400 invalid arguments>"},
    {GSERROR_NO_PERMISSION,         "<403 no permission>"},
    {GSERROR_CONNOT_CONNECT_SAMGR,  "<404 connot connect to samgr>"},
    {GSERROR_CONNOT_CONNECT_SERVER, "<404 connot connect to server>"},
    {GSERROR_CONNOT_CONNECT_WESTON, "<404 connot connect to weston>"},
    {GSERROR_NO_BUFFER,             "<406 no buffer>"},
    {GSERROR_NO_ENTRY,              "<406 no entry>"},
    {GSERROR_OUT_OF_RANGE,          "<406 out of range>"},
    {GSERROR_NO_SCREEN,             "<406 no screen>"},
    {GSERROR_INVALID_OPERATING,     "<412 invalid operating>"},
    {GSERROR_NO_CONSUMER,           "<412 no consumer>"},
    {GSERROR_NOT_INIT,              "<412 not init>"},
    {GSERROR_TYPE_ERROR,            "<412 type error>"},
    {GSERROR_API_FAILED,            "<500 api call failed>"},
    {GSERROR_INTERNAL,              "<500 internal error>"},
    {GSERROR_NO_MEM,                "<500 no memory>"},
    {GSERROR_PROXY_NOT_INCLUDE,     "<500 proxy not include>"},
    {GSERROR_SERVER_ERROR,          "<500 server occur error>"},
    {GSERROR_ANIMATION_RUNNING,     "<500 animation is running>"},
    {GSERROR_NOT_IMPLEMENT,         "<501 not implement>"},
    {GSERROR_NOT_SUPPORT,           "<501 not support>"},
    {GSERROR_BINDER,                "<504 binder occur error>"},
};
}

inline std::string LowErrorStrSpecial(GSError err)
{
    if (err == LOWERROR_INVALID) {
        // int to string (in 1000)
        char num[] = {
            static_cast<char>(err / 0x64 % 0xa), static_cast<char>(err / 0xa % 0xa), static_cast<char>(err % 0xa), 0
        };
        return std::string("with low error <") + num + ">";
    } else if (err == LOWERROR_FAILURE) {
        return "with low error <failure>";
    }
    return "";
}

#ifdef _WIN32
#define strerror_r(err, buf, len) strerror_s((buf), (len), (err))
#endif

inline std::string LowErrorStr(GSError lowerr)
{
    std::string lowError = LowErrorStrSpecial(lowerr);
    if (lowError == "" && lowerr != 0) {
        char buf[256] = {0}; // 256 mean buffer max length
        strerror_r(lowerr, buf, sizeof buf);
        lowError = std::string("with low error <") + buf + ">";
    }
    return lowError;
}

inline std::string GSErrorStr(GSError err)
{
    GSError diff = static_cast<GSError>(err % LOWERROR_MAX);
    auto it = GSErrorStrs.find(static_cast<GSError>(err - diff));
    if (it == GSErrorStrs.end()) {
        return "<GSError error index out of range>";
    }
    return it->second + LowErrorStr(diff);
}

inline std::string SurfaceErrorStr(GSError err)
{
    return GSErrorStr(err);
}

inline std::ostream &operator <<(std::ostream &os, const GSError &err)
{
    os << GSErrorStr(err);
    return os;
}

inline bool operator ==(GSError a, GSError b)
{
    return static_cast<int32_t>(a) / LOWERROR_MAX == static_cast<int32_t>(b) / LOWERROR_MAX;
}

inline bool operator !=(GSError a, GSError b)
{
    return static_cast<int32_t>(a) / LOWERROR_MAX != static_cast<int32_t>(b) / LOWERROR_MAX;
}

using WMError = GSError;
using SurfaceError = GSError;
using VsyncError = GSError;
} // namespace OHOS
#endif // __cplusplus

#endif // INTERFACES_INNERKITS_COMMON_GRAPHIC_COMMON_H

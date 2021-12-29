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

#ifndef ROSEN_COMMON_GRAPHIC_ERROR_H
#define ROSEN_COMMON_GRAPHIC_ERROR_H

#ifdef __cplusplus
#include <cstdint>
#include <cstring>
#include <map>
#include <string>

namespace OHOS {
namespace Rosen {
#endif

// Graphic Rosen Error, number just find fast, it may change
enum RosenError : int32_t {
    ROSEN_ERROR_OK                    = 0,

    // 400 BAD_REQUEST
    ROSEN_ERROR_INVALID_ARGUMENTS     = 40001000,

    // 403 FORBIDDEN
    ROSEN_ERROR_NO_PERMISSION         = 40301000,

    // 404 NOT_FOUND
    ROSEN_ERROR_CONNOT_CONNECT_SAMGR  = 40401000,
    ROSEN_ERROR_CONNOT_CONNECT_SERVER = 40402000,
    ROSEN_ERROR_CONNOT_CONNECT_WESTON = 40403000,

    // 406 NOT_ACCEPTABLE
    ROSEN_ERROR_NO_BUFFER             = 40601000,
    ROSEN_ERROR_NO_ENTRY              = 40602000,
    ROSEN_ERROR_OUT_OF_RANGE          = 40603000,

    // 412 PRECONDITION_FAILED
    ROSEN_ERROR_INVALID_OPERATING     = 41201000,
    ROSEN_ERROR_NO_CONSUMER           = 41202000,
    ROSEN_ERROR_NOT_INIT              = 41203000,
    ROSEN_ERROR_TYPE_ERROR            = 41204000,
    ROSEN_ERROR_DESTROYED_OBJECT      = 41205000,

    // 500 INTERNAL ERROR
    ROSEN_ERROR_API_FAILED            = 50001000,
    ROSEN_ERROR_INTERNEL              = 50002000,
    ROSEN_ERROR_NO_MEM                = 50003000,
    ROSEN_ERROR_PROXY_NOT_INCLUDE     = 50004000,
    ROSEN_ERROR_SERVER_ERROR          = 50005000,

    // 501 NOT_IMPLEMENTED
    ROSEN_ERROR_NOT_IMPLEMENT         = 50101000,
    ROSEN_ERROR_NOT_SUPPORT           = 50102000,

    // 504 GATEWAY ERROR
    ROSEN_ERROR_BINDER                = 50401000,
};

#ifdef __cplusplus
static const std::map<RosenError, std::string> RosenErrorStrs = {
    {ROSEN_ERROR_OK,                    "<200 ok>"},
    {ROSEN_ERROR_INVALID_ARGUMENTS,     "<400 invalid arguments>"},
    {ROSEN_ERROR_NO_PERMISSION,         "<403 no permission>"},
    {ROSEN_ERROR_CONNOT_CONNECT_SAMGR,  "<404 connot connect to samgr>"},
    {ROSEN_ERROR_CONNOT_CONNECT_SERVER, "<404 connot connect to server>"},
    {ROSEN_ERROR_CONNOT_CONNECT_WESTON, "<404 connot connect to weston>"},
    {ROSEN_ERROR_NO_BUFFER,             "<406 no buffer>"},
    {ROSEN_ERROR_NO_ENTRY,              "<406 no entry>"},
    {ROSEN_ERROR_OUT_OF_RANGE,          "<406 out of range>"},
    {ROSEN_ERROR_INVALID_OPERATING,     "<412 invalid operating>"},
    {ROSEN_ERROR_NO_CONSUMER,           "<412 no consumer>"},
    {ROSEN_ERROR_NOT_INIT,              "<412 not init>"},
    {ROSEN_ERROR_TYPE_ERROR,            "<412 type error>"},
    {ROSEN_ERROR_API_FAILED,            "<500 api call failed>"},
    {ROSEN_ERROR_INTERNEL,              "<500 internal error>"},
    {ROSEN_ERROR_NO_MEM,                "<500 no memory>"},
    {ROSEN_ERROR_PROXY_NOT_INCLUDE,     "<500 proxy not include>"},
    {ROSEN_ERROR_SERVER_ERROR,          "<500 server occur error>"},
    {ROSEN_ERROR_NOT_IMPLEMENT,         "<501 not implement>"},
    {ROSEN_ERROR_NOT_SUPPORT,           "<501 not support>"},
    {ROSEN_ERROR_BINDER,                "<504 binder occur error>"},
};

// backstore error
enum BSError : int32_t {
    BS_ERROR_OK = ROSEN_ERROR_OK,
    BS_ERROR_ERROR = ROSEN_ERROR_INTERNEL,
    BS_ERROR_BINDER_ERROR = ROSEN_ERROR_BINDER,
    BS_ERROR_NULLPTR = ROSEN_ERROR_INVALID_ARGUMENTS,
    BS_ERROR_NO_ENTRY = ROSEN_ERROR_NO_ENTRY,
    BS_ERROR_INVALID_OPERATING = ROSEN_ERROR_INVALID_OPERATING,
    BS_ERROR_NO_BUFFER = ROSEN_ERROR_NO_BUFFER,
    BS_ERROR_INVALID_PARAM = ROSEN_ERROR_INVALID_ARGUMENTS,
    BS_ERROR_INIT = ROSEN_ERROR_INTERNEL,
    BS_ERROR_NOMEM = ROSEN_ERROR_NO_MEM,
    BS_ERROR_API_FAILED = ROSEN_ERROR_API_FAILED,
    BS_ERROR_NOT_SUPPORT = ROSEN_ERROR_NOT_SUPPORT,
    BS_ERROR_OUT_OF_RANGE = ROSEN_ERROR_OUT_OF_RANGE,
    BS_ERROR_TYPE_ERROR = ROSEN_ERROR_TYPE_ERROR,
    BS_ERROR_NO_CONSUMER = ROSEN_ERROR_NO_CONSUMER,
};
} // namespace Rosen
} // namespace OHOS
#endif // __cplusplus

#endif // INTERFACES_INNERKITS_COMMON_GRAPHIC_COMMON_H

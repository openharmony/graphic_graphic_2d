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

#include "graphic_napi_common.h"

#include <securec.h>

namespace OHOS {
napi_status SetMemberInt32(napi_env env, napi_value result, const char *key, int32_t value)
{
    napi_value num;
    GNAPI_INNER(napi_create_int32(env, value, &num));
    GNAPI_INNER(napi_set_named_property(env, result, key, num));
    return napi_ok;
}

napi_status SetMemberUint32(napi_env env, napi_value result, const char *key, uint32_t value)
{
    napi_value num;
    GNAPI_INNER(napi_create_uint32(env, value, &num));
    GNAPI_INNER(napi_set_named_property(env, result, key, num));
    return napi_ok;
}

napi_status SetMemberUndefined(napi_env env, napi_value result, const char *key)
{
    napi_value undefined;
    GNAPI_INNER(napi_get_undefined(env, &undefined));
    GNAPI_INNER(napi_set_named_property(env, result, key, undefined));
    return napi_ok;
}

namespace {
struct AssertFailedParam {
    char errormsg[0x100];
};

bool AssertFailedAsync(napi_env env, std::unique_ptr<AssertFailedParam> &param)
{
    return false;
}

napi_value AssertFailedResolve(napi_env env, std::unique_ptr<AssertFailedParam> &param)
{
    napi_value msg;
    NAPI_CALL(env, napi_create_string_utf8(env, param->errormsg, NAPI_AUTO_LENGTH, &msg));
    napi_value error;
    NAPI_CALL(env, napi_create_error(env, nullptr, msg, &error));
    return error;
}
} // namespace

napi_value AssertFailedPromise(napi_env env, const char *fmt, ...)
{
    auto param = std::make_unique<AssertFailedParam>();
    auto &errormsg = param->errormsg;
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf_s(errormsg, sizeof(errormsg), sizeof(errormsg) - 1, fmt, ap);
    va_end(ap);
    if (ret < 0) {
        return CreateError(env, "failed to prepare msg");
    }
    GNAPI_LOG("AssertFailedPromise %{public}s", errormsg);
    return CreatePromise<AssertFailedParam>(env, __PRETTY_FUNCTION__, AssertFailedAsync, AssertFailedResolve, param);
}

napi_value CreateError(napi_env env, const char *fmt, ...)
{
    char errormsg[0x100];
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf_s(errormsg, sizeof(errormsg), sizeof(errormsg) - 1, fmt, ap);
    va_end(ap);
    if (ret < 0) {
        return CreateError(env, "failed to prepare msg");
    }
    GNAPI_LOG("CreateError %{public}s", errormsg);

    napi_value msg;
    NAPI_CALL(env, napi_create_string_utf8(env, errormsg, NAPI_AUTO_LENGTH, &msg));
    napi_value error;
    NAPI_CALL(env, napi_create_error(env, nullptr, msg, &error));
    return error;
}
} // namespace OHOS

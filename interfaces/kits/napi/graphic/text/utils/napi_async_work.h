/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_NAPI_ASYNC_WORK_H
#define OHOS_ROSEN_NAPI_ASYNC_WORK_H

#include <functional>
#include <memory>
#include <string>
#include "refbase.h"

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
#define MAX_LOG_SIZE 1024
/* check condition related to argc/argv, return and logging. */
#define NAPI_CHECK_ARGS(context, condition, specifyStatus, code, retValue, fmt, ...) \
    do { \
        if (!(condition)) { \
            (context)->status = specifyStatus; \
            (context)->errCode = static_cast<int32_t>(code); \
            char buffer[MAX_LOG_SIZE] = {0}; \
            int res = snprintf_s(buffer, MAX_LOG_SIZE, MAX_LOG_SIZE - 1, fmt, ##__VA_ARGS__); \
            if (res < 0) { \
                TEXT_LOGE("Snprintf err, errcode %{public}d", res); \
                retValue; \
            } \
            (context)->errMessage = std::string(buffer); \
            TEXT_LOGE("Test (" #condition ") failed: %{public}s", buffer); \
            retValue; \
        } \
    } while (0)

#define NAPI_CHECK_STATUS_RETURN_VOID(context, message, code)                        \
    do {                                                               \
        if ((context)->status != napi_ok) {                               \
            (context)->errMessage = std::string(message);                      \
            (context)->errCode = code;                      \
            TEXT_LOGE("test (context->status == napi_ok) failed: " message);  \
            return;                                                    \
        }                                                              \
    } while (0)

using NapiCbInfoParser = std::function<void(size_t argc, napi_value* argv)>;
using NapiAsyncExecute = std::function<void(void)>;
using NapiAsyncComplete = std::function<void(napi_value&)>;

struct ContextBase : RefBase {
    virtual ~ContextBase();
    void GetCbInfo(napi_env env, napi_callback_info info, NapiCbInfoParser parser = NapiCbInfoParser(),
                   bool sync = false);

    napi_env env = nullptr;
    napi_value output = nullptr;
    napi_status status = napi_invalid_arg;
    std::string errMessage;
    int32_t errCode = -1;
    napi_value self = nullptr;
    void* native = nullptr;

private:
    napi_deferred deferred = nullptr;
    napi_async_work work = nullptr;
    napi_ref callbackRef = nullptr;
    napi_ref selfRef = nullptr;

    NapiAsyncExecute execute = nullptr;
    NapiAsyncComplete complete = nullptr;

    static constexpr size_t ARGC_MAX = 6;

    friend class NapiAsyncWork;
};

class NapiAsyncWork {
public:
    static napi_value Enqueue(napi_env env, sptr<ContextBase> contextBase, const std::string& name,
                              NapiAsyncExecute execute = NapiAsyncExecute(),
                              NapiAsyncComplete complete = NapiAsyncComplete());

private:
    enum {
        /* AsyncCallback / Promise output result index  */
        RESULT_ERROR = 0,
        RESULT_DATA = 1,
        RESULT_ALL = 2
    };
    static void GenerateOutput(sptr<ContextBase> ctxt);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_NAPI_ASYNC_WORK_H
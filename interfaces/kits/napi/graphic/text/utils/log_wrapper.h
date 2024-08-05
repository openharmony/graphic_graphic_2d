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

#ifndef TEXT_LOG_WRAPPER_H
#define TEXT_LOG_WRAPPER_H

#ifdef TEXT_OHOS
#include "hilog/log.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "JsText"

#ifdef TEXT_OHOS
#define TEXT_LOGI(format, ...)              \
    HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)
#define TEXT_LOGD(format, ...)               \
    HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__)
#define TEXT_LOGE(format, ...)               \
    HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__)
#else
#define TEXT_LOGI(format, ...)
#define TEXT_LOGD(format, ...)
#define TEXT_LOGE(format, ...)
#endif

#define TEXT_ERROR_CHECK(ret, statement, format, ...) \
    do { \
        if (!(ret)) { \
            TEXT_LOGE(format, ##__VA_ARGS__);          \
            statement; \
        } \
    } while (0)

#define TEXT_INFO_CHECK(ret, statement, format, ...) \
    do { \
        if (!(ret)) { \
            TEXT_LOGI(format, ##__VA_ARGS__);        \
            statement; \
        } \
    } while (0)

#define TEXT_CHECK(ret, statement)     \
    do {                               \
        if (!(ret)) {                  \
            statement;                 \
        }                              \
    } while (0)

#define TEXT_CHECK_RETURN_VALUE(ret, result) \
    do {                                     \
        if (!(ret)) {                        \
            return result;                   \
        }                                    \
    } while (0)

#define NAPI_CHECK_AND_THROW_ERROR(ret, errorCode, errorMessage)   \
    do {                                               \
        if (!(ret)) {                                  \
            TEXT_LOGE("%{public}s", #errorMessage);          \
            return NapiThrowError(env, errorCode, errorMessage); \
        }                                              \
    } while (0)

#define NAPI_CHECK_AND_CLOSE_SCOPE(env, statement, scope, ret) \
    do { \
        if ((statement) != napi_ok) { \
            TEXT_LOGE("%{public}s failed", #statement); \
            napi_close_handle_scope(env, scope); \
            return ret; \
        } \
    } while (0)

#endif // TEXT_LOG_WRAPPER_H
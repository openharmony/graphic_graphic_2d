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

#ifndef GRAPHIC_RS_TRACE_H
#define GRAPHIC_RS_TRACE_H

#if !defined(ROSEN_TRACE_DISABLE) || defined(CROSS_PLATFORM)
#include "hitrace_meter.h"
// deprecated:USE RS_TRACE_BEGIN_XXX instead
#define ROSEN_TRACE_BEGIN(tag, name) StartTrace(tag, name)
#define RS_TRACE_BEGIN(name) ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, name)
// deprecated:USE RS_TRACE_END_XXX instead
#define ROSEN_TRACE_END(tag) FinishTrace(tag)
#define RS_TRACE_END() ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL)
#define RS_TRACE_NAME(name) HITRACE_METER_NAME(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, name)
#define RS_TRACE_NAME_FMT(fmt, ...) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, fmt, ##__VA_ARGS__)
#define RS_ASYNC_TRACE_BEGIN(name, value) StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, name, value)
#define RS_ASYNC_TRACE_END(name, value) FinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, name, value)
#define RS_TRACE_INT(name, value) CountTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, name, value)
#define RS_TRACE_FUNC() RS_TRACE_NAME(__func__)

// USER trace macros - basic version
#define RS_USER_TRACE_BEGIN(screenId, name) \
    StartTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, "[screen_id=%" PRIu64 "] " name, screenId)
#define RS_USER_TRACE_END(screenId) FinishTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL)
#define RS_USER_TRACE_NAME(screenId, name) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, "[screen_id=%" PRIu64 "] " name, screenId)
#define RS_USER_TRACE_NAME_FMT(screenId, fmt, ...) \
    HITRACE_METER_FMT(                             \
        HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, "[screen_id=%" PRIu64 "] " fmt, screenId, ##__VA_ARGS__)
#define RS_USER_ASYNC_TRACE_BEGIN(screenId, name, value) \
    StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, "[screen_id=%" PRIu64 "] " name, value, screenId)
#define RS_USER_ASYNC_TRACE_END(screenId, name, value) \
    FinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, "[screen_id=%" PRIu64 "] " name, value, screenId)
#define RS_USER_TRACE_INT(screenId, name, value) \
    CountTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, name, value)
#define RS_USER_TRACE_FUNC(screenId) RS_USER_TRACE_NAME(screenId, __func__)

// DEBUG level
#define RS_TRACE_BEGIN_DEBUG(name, customArgs) \
    StartTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, name, customArgs)
#define RS_TRACE_END_DEBUG(name) \
    FinishTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP)
#define RS_TRACE_NAME_DEBUG(name, customArgs) \
    HITRACE_METER_NAME_EX(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, name, customArgs)
#define RS_TRACE_NAME_FMT_DEBUG(customArgs, fmt, ...) \
    HITRACE_METER_FMT_EX(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, \
    customArgs, fmt, ##__VA_ARGS__)
#define RS_ASYNC_TRACE_BEGIN_DEBUG(name, value, customCategory, customArgs) \
    StartAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, name, value, \
    customCategory, customArgs)
#define RS_ASYNC_TRACE_END_DEBUG(name, value) \
    FinishAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_INT_DEBUG(name, value) \
    CountTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_FUNC_DEBUG() RS_TRACE_NAME_DEBUG(__func__, "")

// USER DEBUG level
#define RS_USER_TRACE_BEGIN_DEBUG(screenId, name, customArgs) \
    StartTraceEx(                                             \
        HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, customArgs)
#define RS_USER_TRACE_END_DEBUG(screenId) \
    FinishTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP)
#define RS_USER_TRACE_NAME_DEBUG(screenId, name, customArgs) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, screenId)
#define RS_USER_TRACE_NAME_FMT_DEBUG(screenId, customArgs, fmt, ...) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " fmt, screenId, ##__VA_ARGS__)
#define RS_USER_ASYNC_TRACE_BEGIN_DEBUG(screenId, name, value, customCategory, customArgs) \
    StartAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP,    \
        "[screen_id=%" PRIu64 "] " name, value, customCategory, customArgs)
#define RS_USER_ASYNC_TRACE_END_DEBUG(screenId, name, value) \
    FinishAsyncTraceEx(                                      \
        HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, value)
#define RS_USER_TRACE_INT_DEBUG(screenId, name, value) \
    CountTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_DEBUG, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_USER_TRACE_FUNC_DEBUG(screenId) RS_USER_TRACE_NAME_DEBUG(screenId, __func__, "")

// INFO level
#define RS_TRACE_BEGIN_INFO(name, customArgs) \
    StartTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, name, customArgs)
#define RS_TRACE_END_INFO(name) \
    FinishTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP)
#define RS_TRACE_NAME_INFO(name, customArgs) \
    HITRACE_METER_NAME_EX(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, name, customArgs)
#define RS_TRACE_NAME_FMT_INFO(customArgs, fmt, ...) \
    HITRACE_METER_FMT_EX(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, \
    customArgs, fmt, ##__VA_ARGS__)
#define RS_ASYNC_TRACE_BEGIN_INFO(name, value, customCategory, customArgs) \
    StartAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, name, value, \
    customCategory, customArgs)
#define RS_ASYNC_TRACE_END_INFO(name, value) \
    FinishAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_INT_INFO(name, value) \
    CountTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_FUNC_INFO() RS_TRACE_NAME_INFO(__func__, "")

// USER INFO level
#define RS_USER_TRACE_BEGIN_INFO(screenId, name, customArgs) \
    StartTraceEx(                                            \
        HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, customArgs)
#define RS_USER_TRACE_END_INFO(screenId) FinishTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP)
#define RS_USER_TRACE_NAME_INFO(screenId, name, customArgs) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, screenId)
#define RS_USER_TRACE_NAME_FMT_INFO(screenId, customArgs, fmt, ...) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " fmt, screenId, ##__VA_ARGS__)
#define RS_USER_ASYNC_TRACE_BEGIN_INFO(screenId, name, value, customCategory, customArgs) \
    StartAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP,    \
        "[screen_id=%" PRIu64 "] " name, value, customCategory, customArgs)
#define RS_USER_ASYNC_TRACE_END_INFO(screenId, name, value) \
    FinishAsyncTraceEx(                                     \
        HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, value)
#define RS_USER_TRACE_INT_INFO(screenId, name, value) \
    CountTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_INFO, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_USER_TRACE_FUNC_INFO(screenId) RS_USER_TRACE_NAME_INFO(screenId, __func__, "")

// CRITICAL level
#define RS_TRACE_BEGIN_CRITICAL(name, customArgs) \
    StartTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, name, customArgs)
#define RS_TRACE_END_CRITICAL(name) \
    FinishTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP)
#define RS_TRACE_NAME_CRITICAL(name, customArgs) \
    HITRACE_METER_NAME_EX(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, name, customArgs)
#define RS_TRACE_NAME_FMT_CRITICAL(customArgs, fmt, ...) \
    HITRACE_METER_FMT_EX(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, \
    customArgs, fmt, ##__VA_ARGS__)
#define RS_ASYNC_TRACE_BEGIN_CRITICAL(name, value, customCategory, customArgs) \
    StartAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, name, value, \
    customCategory, customArgs)
#define RS_ASYNC_TRACE_END_CRITICAL(name, value) \
    FinishAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_INT_CRITICAL(name, value) \
    CountTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_FUNC_CRITICAL() RS_TRACE_NAME_CRITICAL(__func__, "")

// USER CRITICAL level
#define RS_USER_TRACE_BEGIN_CRITICAL(screenId, name, customArgs)                                                       \
    StartTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, \
        customArgs)
#define RS_USER_TRACE_END_CRITICAL(screenId) \
    FinishTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP)
#define RS_USER_TRACE_NAME_CRITICAL(screenId, name, customArgs) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, screenId)
#define RS_USER_TRACE_NAME_FMT_CRITICAL(screenId, customArgs, fmt, ...) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " fmt, screenId, ##__VA_ARGS__)
#define RS_USER_ASYNC_TRACE_BEGIN_CRITICAL(screenId, name, value, customCategory, customArgs) \
    StartAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP,    \
        "[screen_id=%" PRIu64 "] " name, value, customCategory, customArgs)
#define RS_USER_ASYNC_TRACE_END_CRITICAL(screenId, name, value) \
    FinishAsyncTraceEx(                                         \
        HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, value)
#define RS_USER_TRACE_INT_CRITICAL(screenId, name, value) \
    CountTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_CRITICAL, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_USER_TRACE_FUNC_CRITICAL(screenId) RS_USER_TRACE_NAME_CRITICAL(screenId, __func__, "")

// COMMERCIAL level
#define RS_TRACE_BEGIN_COMMERCIAL(name, customArgs) \
    StartTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, name, customArgs)
#define RS_TRACE_END_COMMERCIAL(name) \
    FinishTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP)
#define RS_TRACE_NAME_COMMERCIAL(name, customArgs) \
    HITRACE_METER_NAME_EX(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, name, customArgs)
#define RS_TRACE_NAME_FMT_COMMERCIAL(customArgs, fmt, ...) \
    HITRACE_METER_FMT_EX(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, \
    customArgs, fmt, ##__VA_ARGS__)
#define RS_ASYNC_TRACE_BEGIN_COMMERCIAL(name, value, customCategory, customArgs) \
    StartAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, name, value, \
    customCategory, customArgs)
#define RS_ASYNC_TRACE_END_COMMERCIAL(name, value) \
    FinishAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_INT_COMMERCIAL(name, value) \
    CountTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_FUNC_COMMERCIAL() RS_TRACE_NAME_COMMERCIAL(__func__, "")

// USER COMMERCIAL level
#define RS_USER_TRACE_BEGIN_COMMERCIAL(screenId, name, customArgs)                      \
    StartTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, \
        "[screen_id=%" PRIu64 "] " name, customArgs)
#define RS_USER_TRACE_END_COMMERCIAL(screenId) \
    FinishTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP)
#define RS_USER_TRACE_NAME_COMMERCIAL(screenId, name, customArgs) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, screenId)
#define RS_USER_TRACE_NAME_FMT_COMMERCIAL(screenId, customArgs, fmt, ...) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " fmt, screenId, ##__VA_ARGS__)
#define RS_USER_ASYNC_TRACE_BEGIN_COMMERCIAL(screenId, name, value, customCategory, customArgs) \
    StartAsyncTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP,    \
        "[screen_id=%" PRIu64 "] " name, value, customCategory, customArgs)
#define RS_USER_ASYNC_TRACE_END_COMMERCIAL(screenId, name, value) \
    FinishAsyncTraceEx(                                           \
        HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, "[screen_id=%" PRIu64 "] " name, value)
#define RS_USER_TRACE_INT_COMMERCIAL(screenId, name, value) \
    CountTraceEx(HiTraceOutputLevel::HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_USER_TRACE_FUNC_COMMERCIAL(screenId) RS_USER_TRACE_NAME_COMMERCIAL(screenId, __func__, "")

#elif defined(ROSEN_ANDROID) && !defined(RUNTIME_MODE_RELEASE)
#include "rs_trace_crossplatform.h"
#else

#define ROSEN_TRACE_BEGIN(tag, name)
#define RS_TRACE_BEGIN(name)
#define ROSEN_TRACE_END(tag)
#define RS_TRACE_END()
#define RS_TRACE_NAME_FMT(fmt, ...)
#define RS_ASYNC_TRACE_BEGIN(name, value)
#define RS_ASYNC_TRACE_END(name, value)
#define RS_TRACE_INT(name, value)
#define RS_TRACE_NAME(name)
#define RS_TRACE_FUNC()

// USER trace macros - basic version
#define RS_USER_TRACE_BEGIN(screenId, name)
#define RS_USER_TRACE_END(screenId)
#define RS_USER_TRACE_NAME(screenId, name)
#define RS_USER_TRACE_NAME_FMT(screenId, fmt, ...)
#define RS_USER_ASYNC_TRACE_BEGIN(screenId, name, value)
#define RS_USER_ASYNC_TRACE_END(screenId, name, value)
#define RS_USER_TRACE_INT(screenId, name, value)
#define RS_USER_TRACE_FUNC(screenId)

// DEBUG level
#define RS_TRACE_BEGIN_DEBUG(name, customArgs)
#define RS_TRACE_END_DEBUG(name)
#define RS_TRACE_NAME_DEBUG(name, customArgs)
#define RS_TRACE_NAME_FMT_DEBUG(customArgs, fmt, ...)
#define RS_ASYNC_TRACE_BEGIN_DEBUG(name, value, customCategory, customArgs)
#define RS_ASYNC_TRACE_END_DEBUG(name, value)
#define RS_TRACE_INT_DEBUG(name, value)
#define RS_TRACE_FUNC_DEBUG()

// USER DEBUG level
#define RS_USER_TRACE_BEGIN_DEBUG(screenId, name, customArgs)
#define RS_USER_TRACE_END_DEBUG(screenId)
#define RS_USER_TRACE_NAME_DEBUG(screenId, name, customArgs)
#define RS_USER_TRACE_NAME_FMT_DEBUG(screenId, customArgs, fmt, ...)
#define RS_USER_ASYNC_TRACE_BEGIN_DEBUG(screenId, name, value, customCategory, customArgs)
#define RS_USER_ASYNC_TRACE_END_DEBUG(screenId, name, value)
#define RS_USER_TRACE_INT_DEBUG(screenId, name, value)
#define RS_USER_TRACE_FUNC_DEBUG(screenId)

// INFO level
#define RS_TRACE_BEGIN_INFO(name, customArgs)
#define RS_TRACE_END_INFO(name)
#define RS_TRACE_NAME_INFO(name, customArgs)
#define RS_TRACE_NAME_FMT_INFO(customArgs, fmt, ...)
#define RS_ASYNC_TRACE_BEGIN_INFO(name, value, customCategory, customArgs)
#define RS_ASYNC_TRACE_END_INFO(name, value)
#define RS_TRACE_INT_INFO(name, value)
#define RS_TRACE_FUNC_INFO()

// USER INFO level
#define RS_USER_TRACE_BEGIN_INFO(screenId, name, customArgs)
#define RS_USER_TRACE_END_INFO(screenId)
#define RS_USER_TRACE_NAME_INFO(screenId, name, customArgs)
#define RS_USER_TRACE_NAME_FMT_INFO(screenId, customArgs, fmt, ...)
#define RS_USER_ASYNC_TRACE_BEGIN_INFO(screenId, name, value, customCategory, customArgs)
#define RS_USER_ASYNC_TRACE_END_INFO(screenId, name, value)
#define RS_USER_TRACE_INT_INFO(screenId, name, value)
#define RS_USER_TRACE_FUNC_INFO(screenId)

// CRITICAL level
#define RS_TRACE_BEGIN_CRITICAL(name, customArgs)
#define RS_TRACE_END_CRITICAL(name)
#define RS_TRACE_NAME_CRITICAL(name, customArgs)
#define RS_TRACE_NAME_FMT_CRITICAL(customArgs, fmt, ...)
#define RS_ASYNC_TRACE_BEGIN_CRITICAL(name, value, customCategory, customArgs)
#define RS_ASYNC_TRACE_END_CRITICAL(name, value)
#define RS_TRACE_INT_CRITICAL(name, value)
#define RS_TRACE_FUNC_CRITICAL()

// USER CRITICAL level
#define RS_USER_TRACE_BEGIN_CRITICAL(screenId, name, customArgs)
#define RS_USER_TRACE_END_CRITICAL(screenId)
#define RS_USER_TRACE_NAME_CRITICAL(screenId, name, customArgs)
#define RS_USER_TRACE_NAME_FMT_CRITICAL(screenId, customArgs, fmt, ...)
#define RS_USER_ASYNC_TRACE_BEGIN_CRITICAL(screenId, name, value, customCategory, customArgs)
#define RS_USER_ASYNC_TRACE_END_CRITICAL(screenId, name, value)
#define RS_USER_TRACE_INT_CRITICAL(screenId, name, value)
#define RS_USER_TRACE_FUNC_CRITICAL(screenId)

// COMMERCIAL level
#define RS_TRACE_BEGIN_COMMERCIAL(name, customArgs)
#define RS_TRACE_END_COMMERCIAL(name)
#define RS_TRACE_NAME_COMMERCIAL(name, customArgs)
#define RS_TRACE_NAME_FMT_COMMERCIAL(customArgs, fmt, ...)
#define RS_ASYNC_TRACE_BEGIN_COMMERCIAL(name, value, customCategory, customArgs)
#define RS_ASYNC_TRACE_END_COMMERCIAL(name, value)
#define RS_TRACE_INT_COMMERCIAL(name, value)
#define RS_TRACE_FUNC_COMMERCIAL()

// USER COMMERCIAL level
#define RS_USER_TRACE_BEGIN_COMMERCIAL(screenId, name, customArgs)
#define RS_USER_TRACE_END_COMMERCIAL(screenId)
#define RS_USER_TRACE_NAME_COMMERCIAL(screenId, name, customArgs)
#define RS_USER_TRACE_NAME_FMT_COMMERCIAL(screenId, customArgs, fmt, ...)
#define RS_USER_ASYNC_TRACE_BEGIN_COMMERCIAL(screenId, name, value, customCategory, customArgs)
#define RS_USER_ASYNC_TRACE_END_COMMERCIAL(screenId, name, value)
#define RS_USER_TRACE_INT_COMMERCIAL(screenId, name, value)
#define RS_USER_TRACE_FUNC_COMMERCIAL(screenId)
#endif //ROSEN_TRACE_DISABLE

#endif // GRAPHIC_RS_TRACE_H

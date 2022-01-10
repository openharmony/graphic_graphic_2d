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

#ifndef ROSEN_TRACE_DISABLE
#include "bytrace.h"
#define ROSEN_TRACE_BEGIN(tag, name) StartTrace(tag, name)
#define RS_TRACE_BEGIN(name) ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, name)
#define ROSEN_TRACE_END(tag) FinishTrace(tag)
#define RS_TRACE_END() ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP)
#define RS_TRACE_NAME(name) BYTRACE_NAME(BYTRACE_TAG_GRAPHIC_AGP, name)
#define RS_ASYNC_TRACE_BEGIN(name, value) StartAsyncTrace(BYTRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_ASYNC_TRACE_END(name, value) FinishAsyncTrace(BYTRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_INT(name, value) CountTrace(BYTRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_FUNC() RS_TRACE_NAME(__func__)
#else
#define ROSEN_TRACE_BEGIN(tag, name)
#define RS_TRACE_BEGIN(name)
#define ROSEN_TRACE_END(tag)
#define RS_TRACE_END()
#define RS_TRACE_NAME(name)
#define RS_ASYNC_TRACE_BEGIN(name, value)
#define RS_ASYNC_TRACE_END(name, value)
#define RS_TRACE_INT(name, value)
#define RS_TRACE_FUNC()
#endif

#endif // GRAPHIC_RS_TRACE_H

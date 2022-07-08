/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_OPENGL_WRAPPER_DEBUG_LAYER_TEST_H
#define FRAMEWORKS_OPENGL_WRAPPER_DEBUG_LAYER_TEST_H

#include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*GetNextLayerAddr)(void*, const char*);
extern __eglMustCastToProperFunctionPointerType DebugLayerInitialize(const void *funcTbl,
    GetNextLayerAddr getAddr);
extern __eglMustCastToProperFunctionPointerType DebugLayerGetProcAddr(const char *name,
    __eglMustCastToProperFunctionPointerType next);

#ifdef __cplusplus
};
#endif

#endif // FRAMEWORKS_OPENGL_WRAPPER_DEBUG_LAYER_TEST_H
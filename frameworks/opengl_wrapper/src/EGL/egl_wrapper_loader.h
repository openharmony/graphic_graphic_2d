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

#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_LOADER_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_LOADER_H
#include <EGL/egl.h>

namespace OHOS {
struct EglWrapperDispatchTable;
struct EglHookTable;
struct GlHookTable;

using FunctionPointerType = __eglMustCastToProperFunctionPointerType;

class EglWrapperLoader {
public:
    static EglWrapperLoader& GetInstance();
    ~EglWrapperLoader();
    bool Load(EglWrapperDispatchTable *table);
    bool Unload(EglWrapperDispatchTable *table);
    void *GetProcAddrFromDriver(const char *name);
private:
    EglWrapperLoader() : dlEglHandle_(nullptr), dlGlHandle1_(nullptr),
        dlGlHandle2_(nullptr), dlGlHandle3_(nullptr) {};
    bool LoadEgl(const char *libName, EglHookTable *table);
#ifdef OPENGL_WRAPPER_ENABLE_GL4
    bool LoadGlFromMesa(char const * const *glName, FunctionPointerType *entry);
#endif
    void *LoadGl(const char *libName, char const * const *glName, FunctionPointerType *entry);
    bool LoadVendorDriver(EglWrapperDispatchTable *table);
    void *dlEglHandle_;
    void *dlGlHandle1_;
    void *dlGlHandle2_;
    void *dlGlHandle3_;
};
} // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_LOADER_H

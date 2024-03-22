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

#ifndef FRAMEWORKS_OPENGL_WRAPPER_THREAD_PRIVATE_DATA_H
#define FRAMEWORKS_OPENGL_WRAPPER_THREAD_PRIVATE_DATA_H

#include <pthread.h>
#include <EGL/egl.h>

#include "hook.h"
namespace OHOS {
struct ThreadPrivateData {
    ThreadPrivateData() : error(EGL_SUCCESS), ctx(nullptr), table(nullptr) {}
    EGLint      error;
    EGLContext  ctx;
    GlHookTable *table;
};

class ThreadPrivateDataCtl {
public:
    static ThreadPrivateData* GetPrivateData();
    static void ClearError();
    static void ClearPrivateData();
    static EGLint GetError();
    static void SetError(EGLint error);
    static void SetContext(EGLContext ctx);
    static EGLContext GetContext();
    static void SetGlHookTable(GlHookTable *table);
    static GlHookTable *GetGlHookTable();
private:
    static void KeyInit();
    static void ValidateKey();
    static pthread_key_t key_;
    static pthread_once_t onceCtl_;
};
} // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_THREAD_PRIVATE_DATA_H

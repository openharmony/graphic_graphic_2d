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
#include "thread_private_data_ctl.h"

#include "EGL/egl_wrapper_entry.h"
#include "wrapper_log.h"
namespace OHOS {
constexpr int32_t PTHREAD_KEY_T_NOT_INITIALIZED = -1;
pthread_key_t ThreadPrivateDataCtl::key_ = PTHREAD_KEY_T_NOT_INITIALIZED;
pthread_once_t ThreadPrivateDataCtl::onceCtl_ = PTHREAD_ONCE_INIT;

void ThreadPrivateDataCtl::KeyInit()
{
    if (pthread_key_create(&key_, nullptr) != 0) {
        WLOGE("Failed to create thread key.");
        return;
    }
    WLOGD("pthread_key_create. key = %{public}d", key_);
}

void ThreadPrivateDataCtl::ValidateKey()
{
    pthread_once(&onceCtl_, KeyInit);
}

ThreadPrivateData* ThreadPrivateDataCtl::GetPrivateData()
{
    ValidateKey();
    ThreadPrivateData *data = static_cast<ThreadPrivateData *>(pthread_getspecific(key_));
    if (data == nullptr) {
        data = new ThreadPrivateData;
        pthread_setspecific(key_, data);
    }
    return data;
}

void ThreadPrivateDataCtl::ClearPrivateData()
{
    if (key_ != static_cast<pthread_key_t>(PTHREAD_KEY_T_NOT_INITIALIZED)) {
        ThreadPrivateData *data = static_cast<ThreadPrivateData *>(pthread_getspecific(key_));
        if (data) {
            pthread_setspecific(key_, nullptr);
            delete data;
        }
    }
}

#ifdef EGL_ERROR_ENABLE
void ThreadPrivateDataCtl::ClearError()
{
    OHOS::EglGetErrorImpl();
}
#endif

EGLint ThreadPrivateDataCtl::GetError()
{
    if (key_ == static_cast<pthread_key_t>(PTHREAD_KEY_T_NOT_INITIALIZED)) {
        return EGL_SUCCESS;
    }

    ThreadPrivateData *data = static_cast<ThreadPrivateData *>(pthread_getspecific(key_));
    if (!data) {
        return EGL_SUCCESS;
    }

    EGLint error = data->error;
    data->error = EGL_SUCCESS;

    return error;
}

void ThreadPrivateDataCtl::SetError(EGLint error)
{
    ValidateKey();
    ThreadPrivateData *data = GetPrivateData();
    if (data->error != error) {
        WLOGW("ThreadPrivateDataCtl::SetError error = %{public}d.", error);
        data->error = error;
    }
}

void ThreadPrivateDataCtl::SetContext(EGLContext ctx)
{
    ValidateKey();
    GetPrivateData()->ctx = ctx;
}

EGLContext ThreadPrivateDataCtl::GetContext()
{
    if (key_ == static_cast<pthread_key_t>(PTHREAD_KEY_T_NOT_INITIALIZED)) {
        return EGL_NO_CONTEXT;
    }

    ThreadPrivateData *data = static_cast<ThreadPrivateData *>(pthread_getspecific(key_));
    if (!data) {
        return EGL_NO_CONTEXT;
    }

    return data->ctx;
}

void ThreadPrivateDataCtl::SetGlHookTable(GlHookTable *table)
{
    ValidateKey();
    GetPrivateData()->table = table;
}

GlHookTable *ThreadPrivateDataCtl::GetGlHookTable()
{
    if (key_ == static_cast<pthread_key_t>(PTHREAD_KEY_T_NOT_INITIALIZED)) {
        return nullptr;
    }

    ThreadPrivateData *data = static_cast<ThreadPrivateData *>(pthread_getspecific(key_));
    if (!data) {
        return nullptr;
    }

    return data->table;
}
} // namespace OHOS

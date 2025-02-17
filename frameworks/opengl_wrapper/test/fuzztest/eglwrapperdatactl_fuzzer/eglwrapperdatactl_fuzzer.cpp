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
#include "eglwrapperdatactl_fuzzer.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <securec.h>
#include <unistd.h>

#include "thread_private_data_ctl.h"

namespace OHOS {
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
    }

    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < 0) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        EGLint error = GetData<EGLint>();
        EGLContext ctx = GetData<EGLContext>();
        GlHookTable table = GetData<GlHookTable>();
        GLenum texture = GetData<GLenum>();

        // test
        ThreadPrivateDataCtl::SetError(error);
        ThreadPrivateDataCtl::SetContext(ctx);
        ThreadPrivateDataCtl::SetGlHookTable(&table);
        (void)ThreadPrivateDataCtl::GetPrivateData();
        (void)ThreadPrivateDataCtl::GetError();
        (void)ThreadPrivateDataCtl::GetContext();
        (void)ThreadPrivateDataCtl::GetGlHookTable();
        ThreadPrivateDataCtl::ClearError();
        ThreadPrivateDataCtl::ClearPrivateData();
        glActiveTexture(texture);
        glActiveTexture(texture);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
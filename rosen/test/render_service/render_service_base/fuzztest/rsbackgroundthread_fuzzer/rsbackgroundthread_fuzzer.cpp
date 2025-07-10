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

#include "rsbackgroundthread_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "common/rs_background_thread.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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
bool DoPostTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSBackgroundThread::Instance().PostTask([]() {});
    return true;
}
bool DoPostSyncTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSBackgroundThread::Instance().PostSyncTask([]() {});
    return true;
}
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifdef RS_ENABLE_GL
bool DoCreateShareEglContext(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSBackgroundThread::Instance().CreateShareEglContext();
#if defined(ACE_ENABLE_GL) || defined(RS_ENABLE_GL)
    RSBackgroundThread::Instance().CreateShareEglContext();
    RSBackgroundThread::Instance().renderContext_ = new RenderContext();
    delete RSBackgroundThread::Instance().renderContext_;
    RSBackgroundThread::Instance().renderContext_ = nullptr;
#endif
    return true;
}
#endif
bool DoInitRenderContext(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RenderContext* context = new RenderContext();
    RSBackgroundThread::Instance().InitRenderContext(context);
    return true;
}
bool DoGetShareGPUContext(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSBackgroundThread::Instance().GetShareGPUContext();
    return true;
}
bool DoCreateShareGPUContext(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSBackgroundThread::Instance().GetShareGPUContext();
#ifdef RS_ENABLE_GL
    RSBackgroundThread::Instance().GetShareGPUContext();
#endif
#ifdef RS_ENABLE_VK
    RSBackgroundThread::Instance().GetShareGPUContext();
#endif
    return true;
}
bool DoCleanGrResource(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSBackgroundThread::Instance().CleanGrResource();
    return true;
}
#endif
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoPostTask(data, size);     // PostTask
    OHOS::Rosen::DoPostSyncTask(data, size); // PostSyncTask
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifdef RS_ENABLE_GL
    OHOS::Rosen::DoCreateShareEglContext(data, size); // CreateShareEglContext
#endif
    OHOS::Rosen::DoInitRenderContext(data, size);     // InitRenderContext
    OHOS::Rosen::DoGetShareGPUContext(data, size);    // GetShareGPUContext
    OHOS::Rosen::DoCreateShareGPUContext(data, size); // CreateShareGPUContext
    OHOS::Rosen::DoCleanGrResource(data, size);       // CleanGrResource
#endif
    return 0;
}

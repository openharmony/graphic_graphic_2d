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

#include "rsthreadhandlergeneric_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "common/rs_thread_handler_generic.cpp"
#include "common/rs_thread_handler_generic.h"
#include "common/rs_thread_looper_impl.cpp"

namespace OHOS {
namespace Rosen {
auto rsThreadHandlerGeneric = std::make_shared<RSThreadHandlerGeneric>();
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
bool DoCreateTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSTaskMessage::RSTask task = []() {};
    rsThreadHandlerGeneric->CreateTask(task);
    return true;
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
    int param = GetData<int>();
    auto taskHandle = std::make_shared<RSTaskMessage>();
    rsThreadHandlerGeneric->PostTask(taskHandle, param);
    return true;
}
bool DoPostTaskDelay(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int nsecs = GetData<int>();
    int param = GetData<int>();
    auto taskHandle = std::make_shared<RSTaskMessage>();
    rsThreadHandlerGeneric->PostTaskDelay(taskHandle, nsecs, param);
    return true;
}
bool DoCancelTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto taskHandle = std::make_shared<RSTaskMessage>();
    rsThreadHandlerGeneric->CancelTask(taskHandle);
    return true;
}
bool DoIsValid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    rsThreadHandlerGeneric->IsValid();
    return true;
}
bool DoCreate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSThreadHandler::Create();
    return true;
}
bool DoStaticCreateTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSTaskMessage::RSTask task = []() {};
    RSThreadHandler::StaticCreateTask(task);
    return true;
}
bool DoGenericMessageHandler(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSTaskMessage::RSTask task = []() {};
    int param = GetData<int>();
    GenericMessageHandler genericMessageHandler(task);
    genericMessageHandler.Process(param);
    return true;
}
bool DoGenericThreadMessage(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSTaskMessage::RSTask task = []() {};
    GenericThreadMessage genericThreadMessage(task);
    genericThreadMessage.GetMessageHandler();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreateTask(data, size);            // CreateTask
    OHOS::Rosen::DoPostTask(data, size);              // PostTask
    OHOS::Rosen::DoPostTaskDelay(data, size);         // PostTaskDelay
    OHOS::Rosen::DoCancelTask(data, size);            // CancelTask
    OHOS::Rosen::DoIsValid(data, size);               // IsValid
    OHOS::Rosen::DoCreate(data, size);                // Create
    OHOS::Rosen::DoStaticCreateTask(data, size);      // StaticCreateTask
    OHOS::Rosen::DoGenericMessageHandler(data, size); // GenericMessageHandler
    OHOS::Rosen::DoGenericThreadMessage(data, size);  // GenericThreadMessage
    return 0;
}
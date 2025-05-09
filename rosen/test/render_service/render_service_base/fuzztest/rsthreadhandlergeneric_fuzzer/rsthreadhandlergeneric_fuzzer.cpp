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
const uint8_t DO_CREATE_TASK = 0;
const uint8_t DO_POST_TASK = 1;
const uint8_t DO_POST_TASK_DELAY = 2;
const uint8_t DO_CANCEL_TASK = 3;
const uint8_t DO_ISVALID = 4;
const uint8_t DO_CREATE = 5;
const uint8_t DO_STATIC_CREATE_TASK = 6;
const uint8_t DO_GENERIC_MESSAGE_HANDLER = 7;
const uint8_t DO_GENERIC_THREA_DMESSAGE = 8;
const uint8_t TARGET_SIZE = 9;

const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

void DoCreateTask()
{
    RSTaskMessage::RSTask task = []() {};
    rsThreadHandlerGeneric->CreateTask(task);
}

void DoPostTask()
{
    int param = GetData<int>();
    auto taskHandle = std::make_shared<RSTaskMessage>();
    rsThreadHandlerGeneric->PostTask(taskHandle, param);
}

void DoPostTaskDelay()
{
    int nsecs = GetData<int>();
    int param = GetData<int>();
    auto taskHandle = std::make_shared<RSTaskMessage>();
    rsThreadHandlerGeneric->PostTaskDelay(taskHandle, nsecs, param);
}

void DoCancelTask()
{
    auto taskHandle = std::make_shared<RSTaskMessage>();
    rsThreadHandlerGeneric->CancelTask(taskHandle);
}

void DoIsValid()
{
    rsThreadHandlerGeneric->IsValid();
}

void DoCreate()
{
    RSThreadHandler::Create();
}

void DoStaticCreateTask()
{
    RSTaskMessage::RSTask task = []() {};
    RSThreadHandler::StaticCreateTask(task);
}

void DoGenericMessageHandler()
{
    RSTaskMessage::RSTask task = []() {};
    int param = GetData<int>();
    GenericMessageHandler genericMessageHandler(task);
    genericMessageHandler.Process(param);
}

void DoGenericThreadMessage()
{
    RSTaskMessage::RSTask task = []() {};
    GenericThreadMessage genericThreadMessage(task);
    genericThreadMessage.GetMessageHandler();
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE_TASK:
            OHOS::Rosen::DoCreateTask();            // CreateTask
            break;
        case OHOS::Rosen::DO_POST_TASK:
            OHOS::Rosen::DoPostTask();              // PostTask
            break;
        case OHOS::Rosen::DO_POST_TASK_DELAY:
            OHOS::Rosen::DoPostTaskDelay();         // PostTaskDelay
            break;
        case OHOS::Rosen::DO_CANCEL_TASK:
            OHOS::Rosen::DoCancelTask();            // CancelTask
            break;
        case OHOS::Rosen::DO_ISVALID:
            OHOS::Rosen::DoIsValid();               // IsValid
            break;
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate();                // Create
            break;
        case OHOS::Rosen::DO_STATIC_CREATE_TASK:
            OHOS::Rosen::DoStaticCreateTask();      // StaticCreateTask
            break;
        case OHOS::Rosen::DO_GENERIC_MESSAGE_HANDLER:
            OHOS::Rosen::DoGenericMessageHandler(); // GenericMessageHandler
            break;
        case OHOS::Rosen::DO_GENERIC_THREA_DMESSAGE:
            OHOS::Rosen::DoGenericThreadMessage();  // GenericThreadMessage
            break;
        default:
            return -1;
    }
    return 0;
}
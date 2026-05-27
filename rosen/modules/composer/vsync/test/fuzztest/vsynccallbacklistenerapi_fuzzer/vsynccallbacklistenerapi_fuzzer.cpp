/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "vsynccallbacklistenerapi_fuzzer.h"

#include <cstdint>
#include <event_handler.h>
#include <fcntl.h>
#include <functional>
#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <securec.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "graphic_common.h"
#include "graphic_common_c.h"
#include "vsync_controller.h"
#include "vsync_distributor.h"
#include "vsync_receiver.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr size_t STRING_LEN_MAX = 32;
constexpr uint8_t CALLBACK_TYPE_COUNT = 4;
constexpr uint8_t USER_DATA_TYPE_COUNT = 3;

enum class FrameCallbackTypeForSet : uint8_t {
    RECORD_TIMESTAMP = 0,
    INCREMENT_COUNTER = 1,
    WITH_ID_RECORD_TIMESTAMP = 2,
    WITH_ID_INCREMENT_COUNTER = 3,
};

enum class FrameCallbackTypeForAdd : uint8_t {
    DO_NOTHING = 0,
    VALIDATE_USER_DATA = 1,
    WITH_ID_DO_NOTHING = 2,
    BOTH_NULLPTR = 3,
};

enum class FdShutDownCallbackType : uint8_t {
    NULLPTR = 0,
    CLOSE_FD = 1,
    DO_NOTHING = 2,
    VALIDATE_FD = 3,
};

enum class ReadableCallbackType : uint8_t {
    NULLPTR = 0,
    ALWAYS_READY = 1,
    NEVER_READY = 2,
    CHECK_BY_FD = 3,
};

const uint8_t DO_SET_CALLBACK = 0;
const uint8_t DO_SET_NAME = 1;
const uint8_t DO_SET_RNV_FLAG = 2;
const uint8_t DO_ADD_CALLBACK = 3;
const uint8_t DO_REGISTER_FD_SHUT_DOWN_CALLBACK = 4;
const uint8_t DO_REGISTER_READABLE_CALLBACK = 5;
const uint8_t DO_SET_FD_CLOSED_FLAG = 6;
const uint8_t TARGET_SIZE = 7;
const int MAX_FD = 256;

void OnVSyncRecordTimestamp(int64_t now, void* data)
{
    if (data != nullptr) {
        int64_t* timestamp = reinterpret_cast<int64_t*>(data);
        *timestamp = now;
    }
}

void OnVSyncIncrementCounter(int64_t now, void* data)
{
    if (data != nullptr) {
        int32_t* counter = reinterpret_cast<int32_t*>(data);
        (*counter)++;
    }
}

void OnVSyncDoNothing(int64_t now, void* data) {}

void OnVSyncValidateUserData(int64_t now, void* data)
{
    if (data == nullptr) {
        return;
    }
    uintptr_t userDataVal = reinterpret_cast<uintptr_t>(data);
    if (userDataVal > 0 && userDataVal < UINTPTR_MAX) {
        volatile int32_t dummy = static_cast<int32_t>(userDataVal % 100);
        if (dummy == 0) {
            data = nullptr;
        }
    }
}

void OnVSyncWithIdRecordTimestamp(int64_t now, int64_t vsyncId, void* data)
{
    if (data != nullptr) {
        int64_t* info = reinterpret_cast<int64_t*>(data);
        info[0] = now;
        info[1] = vsyncId;
    }
}

void OnVSyncWithIdIncrementCounter(int64_t now, int64_t vsyncId, void* data)
{
    if (data != nullptr) {
        int32_t* counter = reinterpret_cast<int32_t*>(data);
        (*counter)++;
        if (vsyncId > 0) {
            (*counter)++;
        }
    }
}

void OnVSyncWithIdDoNothing(int64_t now, int64_t vsyncId, void* data) {}

void OnFdShutDownCloseFd(int32_t fd)
{
    if (fd >= 0) {
        close(fd);
    }
}

void OnFdShutDownDoNothing(int32_t fd) {}

void OnFdShutDownValidateFd(int32_t fd)
{
    if (fd < 0) {
        return;
    }
    close(fd);
}

bool OnReadableAlwaysReady(int32_t fd)
{
    return true;
}

bool OnReadableNeverReady(int32_t fd)
{
    return false;
}

bool OnReadableCheckByFd(int32_t fd)
{
    return fd > 0 && (fd <= MAX_FD);
}

void DoSetCallback(FuzzedDataProvider& fdp)
{
    auto listener = std::make_shared<VSyncCallBackListener>();
    VSyncCallBackListener::FrameCallback callback;
    uint8_t callbackType = fdp.ConsumeIntegral<uint8_t>() % CALLBACK_TYPE_COUNT;
    uint8_t userDataVal = fdp.ConsumeIntegral<uint8_t>();
    callback.userData_ = (userDataVal % USER_DATA_TYPE_COUNT == 0)
                             ? nullptr
                             : reinterpret_cast<void*>(static_cast<uintptr_t>(userDataVal));
    switch (static_cast<FrameCallbackTypeForSet>(callbackType)) {
        case FrameCallbackTypeForSet::RECORD_TIMESTAMP:
            callback.callback_ = OnVSyncRecordTimestamp;
            callback.callbackWithId_ = nullptr;
            break;
        case FrameCallbackTypeForSet::INCREMENT_COUNTER:
            callback.callback_ = OnVSyncIncrementCounter;
            callback.callbackWithId_ = nullptr;
            break;
        case FrameCallbackTypeForSet::WITH_ID_RECORD_TIMESTAMP:
            callback.callback_ = nullptr;
            callback.callbackWithId_ = OnVSyncWithIdRecordTimestamp;
            break;
        case FrameCallbackTypeForSet::WITH_ID_INCREMENT_COUNTER:
            callback.callback_ = nullptr;
            callback.callbackWithId_ = OnVSyncWithIdIncrementCounter;
            break;
        default:
            callback.callback_ = OnVSyncRecordTimestamp;
            callback.callbackWithId_ = nullptr;
            break;
    }
    listener->SetCallback(callback);
}

void DoSetName(FuzzedDataProvider& fdp)
{
    auto listener = std::make_shared<VSyncCallBackListener>();
    std::string name = fdp.ConsumeRandomLengthString(STRING_LEN_MAX);
    listener->SetName(name);
}

void DoSetRnvFlag(FuzzedDataProvider& fdp)
{
    auto listener = std::make_shared<VSyncCallBackListener>();
    uint8_t flagVal = fdp.ConsumeIntegral<uint8_t>();
    bool rnvFlag = static_cast<bool>(flagVal % 2);
    listener->SetRNVFlag(rnvFlag);
}

void DoAddCallback(FuzzedDataProvider& fdp)
{
    auto listener = std::make_shared<VSyncCallBackListener>();
    VSyncCallBackListener::FrameCallback callback;
    uint8_t callbackType = fdp.ConsumeIntegral<uint8_t>() % CALLBACK_TYPE_COUNT;
    uint8_t userDataVal = fdp.ConsumeIntegral<uint8_t>();
    callback.userData_ = (userDataVal % USER_DATA_TYPE_COUNT == 0)
                             ? nullptr
                             : reinterpret_cast<void*>(static_cast<uintptr_t>(userDataVal));
    switch (static_cast<FrameCallbackTypeForAdd>(callbackType)) {
        case FrameCallbackTypeForAdd::DO_NOTHING:
            callback.callback_ = OnVSyncDoNothing;
            callback.callbackWithId_ = nullptr;
            break;
        case FrameCallbackTypeForAdd::VALIDATE_USER_DATA:
            callback.callback_ = OnVSyncValidateUserData;
            callback.callbackWithId_ = nullptr;
            break;
        case FrameCallbackTypeForAdd::WITH_ID_DO_NOTHING:
            callback.callback_ = nullptr;
            callback.callbackWithId_ = OnVSyncWithIdDoNothing;
            break;
        case FrameCallbackTypeForAdd::BOTH_NULLPTR:
            callback.callback_ = nullptr;
            callback.callbackWithId_ = nullptr;
            break;
        default:
            callback.callback_ = OnVSyncDoNothing;
            callback.callbackWithId_ = nullptr;
            break;
    }
    listener->AddCallback(callback);
}

void DoRegisterFdShutDownCallback(FuzzedDataProvider& fdp)
{
    auto listener = std::make_shared<VSyncCallBackListener>();
    uint16_t callbackVal = fdp.ConsumeIntegral<uint16_t>();
    uint8_t callbackType = static_cast<uint8_t>(callbackVal % CALLBACK_TYPE_COUNT);
    VSyncCallBackListener::FdShutDownCallback cb = nullptr;
    switch (static_cast<FdShutDownCallbackType>(callbackType)) {
        case FdShutDownCallbackType::NULLPTR:
            cb = nullptr;
            break;
        case FdShutDownCallbackType::CLOSE_FD:
            cb = OnFdShutDownCloseFd;
            break;
        case FdShutDownCallbackType::DO_NOTHING:
            cb = OnFdShutDownDoNothing;
            break;
        case FdShutDownCallbackType::VALIDATE_FD:
            cb = OnFdShutDownValidateFd;
            break;
        default:
            cb = nullptr;
            break;
    }
    listener->RegisterFdShutDownCallback(cb);
}

void DoRegisterReadableCallback(FuzzedDataProvider& fdp)
{
    auto listener = std::make_shared<VSyncCallBackListener>();
    uint32_t callbackVal = fdp.ConsumeIntegral<uint32_t>();
    uint8_t callbackType = static_cast<uint8_t>(callbackVal % CALLBACK_TYPE_COUNT);
    VSyncCallBackListener::ReadableCallback cb = nullptr;
    switch (static_cast<ReadableCallbackType>(callbackType)) {
        case ReadableCallbackType::NULLPTR:
            cb = nullptr;
            break;
        case ReadableCallbackType::ALWAYS_READY:
            cb = OnReadableAlwaysReady;
            break;
        case ReadableCallbackType::NEVER_READY:
            cb = OnReadableNeverReady;
            break;
        case ReadableCallbackType::CHECK_BY_FD:
            cb = OnReadableCheckByFd;
            break;
        default:
            cb = nullptr;
            break;
    }
    listener->RegisterReadableCallback(cb);
}

void DoSetFdClosedFlag(FuzzedDataProvider& fdp)
{
    auto listener = std::make_shared<VSyncCallBackListener>();
    uint64_t flagVal = fdp.ConsumeIntegral<uint64_t>();
    bool fdClosed = static_cast<bool>(flagVal % 2);
    listener->SetFdClosedFlagLocked(fdClosed);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 1) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_CALLBACK:
            OHOS::Rosen::DoSetCallback(fdp);
            break;
        case OHOS::Rosen::DO_SET_NAME:
            OHOS::Rosen::DoSetName(fdp);
            break;
        case OHOS::Rosen::DO_SET_RNV_FLAG:
            OHOS::Rosen::DoSetRnvFlag(fdp);
            break;
        case OHOS::Rosen::DO_ADD_CALLBACK:
            OHOS::Rosen::DoAddCallback(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_FD_SHUT_DOWN_CALLBACK:
            OHOS::Rosen::DoRegisterFdShutDownCallback(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_READABLE_CALLBACK:
            OHOS::Rosen::DoRegisterReadableCallback(fdp);
            break;
        case OHOS::Rosen::DO_SET_FD_CLOSED_FLAG:
            OHOS::Rosen::DoSetFdClosedFlag(fdp);
            break;
        default:
            break;
    }
    return 0;
}
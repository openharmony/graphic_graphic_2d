/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "registerCallback_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
// Grouped by related functionality
const uint8_t DO_REGISTER_TRANSACTION_DATA_CALLBACK = 0;
const uint8_t DO_REGISTER_UIEXTENSION_CALLBACK = 1;
const uint8_t DO_REGISTER_SURFACE_BUFFER_CALLBACK = 2;           // Surface buffer registration
const uint8_t DO_UNREGISTER_SURFACE_BUFFER_CALLBACK = 3;         // Surface buffer unregistration (paired with REGISTER)
const uint8_t DO_REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK = 4;
const uint8_t TARGET_SIZE = 5;
constexpr size_t STR_LEN = 10;

void DoRegisterTransactionDataCallback(FuzzedDataProvider& fdp)
{
    uint64_t token = fdp.ConsumeIntegral<uint64_t>();
    uint64_t timeStamp = fdp.ConsumeIntegral<uint64_t>();
    std::function<void()> callback = []() {
        // Simple callback construction
    };
    g_rsInterfaces->RegisterTransactionDataCallback(token, timeStamp, callback);
}

void DoRegisterUIExtensionCallback(FuzzedDataProvider& fdp)
{
    uint64_t userId = fdp.ConsumeIntegral<uint64_t>();
    UIExtensionCallback callback = [](std::shared_ptr<RSUIExtensionData> data, uint64_t id) {
        (void)data;
        (void)id;
    };
    g_rsInterfaces->RegisterUIExtensionCallback(userId, callback, fdp.ConsumeBool());
}

void DoRegisterSurfaceBufferCallback(FuzzedDataProvider& fdp)
{
    int32_t pid = fdp.ConsumeIntegral<int32_t>();
    uint64_t uid = fdp.ConsumeIntegral<uint64_t>();
    auto callback = [](void* addr, uint32_t size, wptr<Damageable> damageable) {
        (void)addr;
        (void)size;
        (void)damageable;
    };
    g_rsInterfaces->RegisterSurfaceBufferCallback(pid, uid, callback);
}

void DoUnregisterSurfaceBufferCallback(FuzzedDataProvider& fdp)
{
    int32_t pid = fdp.ConsumeIntegral<int32_t>();
    uint64_t uid = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->UnregisterSurfaceBufferCallback(pid, uid);
}

void DoRegisterSelfDrawingNodeRectChangeCallback(FuzzedDataProvider& fdp)
{
    RectConstraint constraint;
    constraint.left_ = fdp.ConsumeIntegral<int32_t>();
    constraint.top_ = fdp.ConsumeIntegral<int32_t>();
    constraint.width_ = fdp.ConsumeIntegral<uint32_t>();
    constraint.height_ = fdp.ConsumeIntegral<uint32_t>();
    SelfDrawingNodeRectChangeCallback callback = [](std::shared_ptr<RSSelfDrawingNodeRectData> data) {
        (void)data;
    };
    g_rsInterfaces->RegisterSelfDrawingNodeRectChangeCallback(constraint, callback);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_REGISTER_TRANSACTION_DATA_CALLBACK:
            OHOS::Rosen::DoRegisterTransactionDataCallback(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_UIEXTENSION_CALLBACK:
            OHOS::Rosen::DoRegisterUIExtensionCallback(fdp);
            break;
        // Surface buffer callback operations (grouped together)
        case OHOS::Rosen::DO_REGISTER_SURFACE_BUFFER_CALLBACK:
            OHOS::Rosen::DoRegisterSurfaceBufferCallback(fdp);
            break;
        case OHOS::Rosen::DO_UNREGISTER_SURFACE_BUFFER_CALLBACK:
            OHOS::Rosen::DoUnregisterSurfaceBufferCallback(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterSelfDrawingNodeRectChangeCallback(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}

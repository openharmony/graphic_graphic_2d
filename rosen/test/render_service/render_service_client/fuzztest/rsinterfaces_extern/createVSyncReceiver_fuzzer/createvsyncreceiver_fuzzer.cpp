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

#include "createvsyncreceiver_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_CREATE_VSYNC_RECEIVER_SIMPLE = 0;
const uint8_t DO_CREATE_VSYNC_RECEIVER_ADVANCED = 1;
const uint8_t TARGET_SIZE = 2;
constexpr uint32_t STR_LEN = 10;

void DoCreateVSyncReceiverSimple(FuzzedDataProvider& fdp)
{
    // Construct parameters for simple version
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);

    // Use fuzz data to decide whether to pass looper
    bool hasLooper = fdp.ConsumeBool();
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper = nullptr;

    // Call the simple interface
    if (hasLooper) {
        g_rsInterfaces->CreateVSyncReceiver(name, looper);
    } else {
        g_rsInterfaces->CreateVSyncReceiver(name);
    }
}

void DoCreateVSyncReceiverAdvanced(FuzzedDataProvider& fdp)
{
    // Construct parameters for advanced version
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();

    // Use fuzz data to decide whether to pass looper
    bool hasLooper = fdp.ConsumeBool();
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper = nullptr;

    NodeId windowNodeId = fdp.ConsumeIntegral<NodeId>();
    bool fromXcomponent = fdp.ConsumeBool();

    // Call the advanced interface
    if (hasLooper) {
        g_rsInterfaces->CreateVSyncReceiver(name, id, looper, windowNodeId, fromXcomponent);
    } else {
        g_rsInterfaces->CreateVSyncReceiver(name, id, nullptr, windowNodeId, fromXcomponent);
    }
}

} // anonymous namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSInterfaces
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
        case OHOS::Rosen::DO_CREATE_VSYNC_RECEIVER_SIMPLE:
            OHOS::Rosen::DoCreateVSyncReceiverSimple(fdp);
            break;
        case OHOS::Rosen::DO_CREATE_VSYNC_RECEIVER_ADVANCED:
            OHOS::Rosen::DoCreateVSyncReceiverAdvanced(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}

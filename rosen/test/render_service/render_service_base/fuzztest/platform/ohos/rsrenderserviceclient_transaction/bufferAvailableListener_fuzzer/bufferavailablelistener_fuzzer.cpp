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

#include "bufferavailablelistener_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "ipc_callbacks/buffer_available_callback.h"
#include "transaction/rs_render_pipeline_client.h"

namespace OHOS {
namespace Rosen {

// Global RSRenderPipelineClient instance, initialized in LLVMFuzzerInitialize
std::shared_ptr<RSRenderPipelineClient> g_renderPipelineClient = nullptr;

namespace {
const uint8_t DO_SET_BUFFER_AVAILABLE_LISTENER = 0;
const uint8_t TARGET_SIZE = 1;

void DoSetBufferAvailableListener(FuzzedDataProvider& fdp)
{
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();
    bool isFromRenderThread = fdp.ConsumeBool();

    // Use fuzz data to decide whether to register callback
    bool shouldRegisterCallback = fdp.ConsumeBool();

    if (shouldRegisterCallback) {
        // Use simple lambda for callback
        BufferAvailableCallback callback = []() {
            // Callback implementation
        };

        g_renderPipelineClient->RegisterBufferAvailableListener(static_cast<NodeId>(id), callback, isFromRenderThread);
    } else {
        // Register nullptr callback
        BufferAvailableCallback callback = nullptr;
        g_renderPipelineClient->RegisterBufferAvailableListener(static_cast<NodeId>(id), callback, isFromRenderThread);
    }

    g_renderPipelineClient->UnregisterBufferAvailableListener(static_cast<NodeId>(id));
}

} // anonymous namespace
} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSRenderPipelineClient using std::make_shared (consistent with business code and TDD)
    OHOS::Rosen::g_renderPipelineClient = std::make_shared<OHOS::Rosen::RSRenderPipelineClient>();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_renderPipelineClient == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_BUFFER_AVAILABLE_LISTENER:
            OHOS::Rosen::DoSetBufferAvailableListener(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}

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

#include "canvasBuffer_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_render_interface.h"

#include "platform/drawing/rs_surface.h"

namespace OHOS {
namespace Rosen {

static RSRenderInterface* g_renderInterface = nullptr;

namespace {
const uint8_t DO_REGISTER_CANVAS_CALLBACK = 0;
const uint8_t DO_SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER = 1;
const uint8_t TARGET_SIZE = 2;
} // namespace

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
class TestRSCanvasSurfaceBufferCallback : public RSICanvasSurfaceBufferCallback {
public:
    TestRSCanvasSurfaceBufferCallback() = default;
    ~TestRSCanvasSurfaceBufferCallback() noexcept override = default;

    void OnCanvasSurfaceBufferChanged(NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override {}

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

void DoRegisterCanvasCallback(FuzzedDataProvider& fdp)
{
    // Use fuzz data to decide whether to register callback
    bool shouldRegister = fdp.ConsumeBool();

    if (!shouldRegister) {
        // Test case: don't register callback (pass nullptr)
        sptr<RSICanvasSurfaceBufferCallback> callback = nullptr;
        g_renderInterface->RegisterCanvasCallback(callback);
    } else {
        // Test case: register callback
        sptr<RSICanvasSurfaceBufferCallback> callback = new TestRSCanvasSurfaceBufferCallback();
        g_renderInterface->RegisterCanvasCallback(callback);
    }
}

void DoSubmitCanvasPreAllocatedBuffer(FuzzedDataProvider& fdp)
{
    NodeId nodeId = fdp.ConsumeIntegral<uint64_t>();
    uint32_t resetSurfaceIndex = fdp.ConsumeIntegral<uint32_t>();
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    g_renderInterface->SubmitCanvasPreAllocatedBuffer(nodeId, buffer, resetSurfaceIndex);
}
#endif

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    g_renderInterface = &RSRenderInterface::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 1) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);

    if (g_renderInterface == nullptr) {
        return 0;
    }
    uint8_t command = fdp.ConsumeIntegral<uint8_t>() % TARGET_SIZE;
    switch (command) {
        case DO_REGISTER_CANVAS_CALLBACK:
            DoRegisterCanvasCallback(fdp);
            break;
        case DO_SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER:
            DoSubmitCanvasPreAllocatedBuffer(fdp);
            break;
        default:
            break;
    }

    return 0;
}

} // namespace Rosen
} // namespace OHOS

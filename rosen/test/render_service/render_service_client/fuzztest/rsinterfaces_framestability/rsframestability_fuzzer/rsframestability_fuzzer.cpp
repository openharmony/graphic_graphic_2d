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

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_frame_stability_types.h"

namespace OHOS {
namespace Rosen {

RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_REGISTER_FRAME_STABILITY_DETECTION = 0;
const uint8_t DO_UNREGISTER_FRAME_STABILITY_DETECTION = 1;
const uint8_t DO_START_FRAME_STABILITY_COLLECTION = 2;
const uint8_t DO_GET_FRAME_STABILITY_RESULT = 3;
const uint8_t TARGET_SIZE = 4;
const uint32_t MAX_TARGET_TYPE = 2;
void DoRegisterFrameStabilityDetection(FuzzedDataProvider& fdp)
{
    FrameStabilityTarget target;
    target.id = fdp.ConsumeIntegral<uint64_t>();
    target.type = static_cast<FrameStabilityTargetType>(fdp.ConsumeIntegral<uint32_t>() % MAX_TARGET_TYPE);
    FrameStabilityConfig config;
    config.stableDuration = fdp.ConsumeIntegral<uint32_t>();
    config.changePercent = fdp.ConsumeFloatingPoint<float>();
    FrameStabilityCallback callback = [](bool isStable) {
        (void)isStable;
    };
    g_rsInterfaces->RegisterFrameStabilityDetection(target, config, callback);
}

void DoUnregisterFrameStabilityDetection(FuzzedDataProvider& fdp)
{
    FrameStabilityTarget target;
    target.id = fdp.ConsumeIntegral<uint64_t>();
    target.type = static_cast<FrameStabilityTargetType>(fdp.ConsumeIntegral<uint32_t>() % MAX_TARGET_TYPE);
    g_rsInterfaces->UnregisterFrameStabilityDetection(target);
}

void DoStartFrameStabilityCollection(FuzzedDataProvider& fdp)
{
    FrameStabilityTarget target;
    target.id = fdp.ConsumeIntegral<uint64_t>();
    target.type = static_cast<FrameStabilityTargetType>(fdp.ConsumeIntegral<uint32_t>() % MAX_TARGET_TYPE);
    FrameStabilityConfig config;
    config.stableDuration = fdp.ConsumeIntegral<uint32_t>();
    config.changePercent = fdp.ConsumeFloatingPoint<float>();
    g_rsInterfaces->StartFrameStabilityCollection(target, config);
}

void DoGetFrameStabilityResult(FuzzedDataProvider& fdp)
{
    FrameStabilityTarget target;
    target.id = fdp.ConsumeIntegral<uint64_t>();
    target.type = static_cast<FrameStabilityTargetType>(fdp.ConsumeIntegral<uint32_t>() % MAX_TARGET_TYPE);
    bool result = false;
    g_rsInterfaces->GetFrameStabilityResult(target, result);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr || size == 0) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_REGISTER_FRAME_STABILITY_DETECTION:
            OHOS::Rosen::DoRegisterFrameStabilityDetection(fdp);
            break;
        case OHOS::Rosen::DO_UNREGISTER_FRAME_STABILITY_DETECTION:
            OHOS::Rosen::DoUnregisterFrameStabilityDetection(fdp);
            break;
        case OHOS::Rosen::DO_START_FRAME_STABILITY_COLLECTION:
            OHOS::Rosen::DoStartFrameStabilityCollection(fdp);
            break;
        case OHOS::Rosen::DO_GET_FRAME_STABILITY_RESULT:
            OHOS::Rosen::DoGetFrameStabilityResult(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}

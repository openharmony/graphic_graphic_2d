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

#include "pixelmapfromsurface_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "core/transaction/rs_interfaces.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
void DoCreatePixelMapFromSurfaceId(FuzzedDataProvider& fdp)
{
    // Construct surfaceId from fuzz data
    uint64_t surfaceId = fdp.ConsumeIntegral<uint64_t>();

    // Construct Rect
    Rect srcRect = {
        .left_ = fdp.ConsumeIntegral<int32_t>(),
        .top_ = fdp.ConsumeIntegral<int32_t>(),
        .width_ = fdp.ConsumeIntegral<int32_t>(),
        .height_ = fdp.ConsumeIntegral<int32_t>()
    };

    // Construct transformEnabled flag
    bool transformEnabled = fdp.ConsumeBool();

    // Call the interface
    std::shared_ptr<Media::PixelMap> result =
        g_rsInterfaces->CreatePixelMapFromSurfaceId(surfaceId, srcRect, transformEnabled);
    (void)result;
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSInterfaces
    OHOS::Rosen::g_rsInterfaces = &RSInterfaces::GetInstance();
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
    OHOS::Rosen::DoCreatePixelMapFromSurfaceId(fdp);

    return 0;
}

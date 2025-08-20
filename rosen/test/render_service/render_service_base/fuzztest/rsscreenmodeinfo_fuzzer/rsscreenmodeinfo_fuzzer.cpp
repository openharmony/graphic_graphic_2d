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

#include "rsscreenmodeinfo_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "screen_manager/rs_screen_mode_info.h"

namespace OHOS {
namespace Rosen {

bool DoSetScreenWidth(FuzzedDataProvider& fdp)
{
    // test
    RSScreenModeInfo screenModeInfo;
    int32_t width = fdp.ConsumeIntegral<int32_t>();
    screenModeInfo.SetScreenWidth(width);
    return true;
}

bool DoSetScreenHeight(FuzzedDataProvider& fdp)
{
    // test
    RSScreenModeInfo screenModeInfo;
    int32_t height = fdp.ConsumeIntegral<int32_t>();
    screenModeInfo.SetScreenHeight(height);
    return true;
}

bool DoSetScreenRefreshRate(FuzzedDataProvider& fdp)
{
    // test
    RSScreenModeInfo screenModeInfo;
    uint32_t refreshRate = fdp.ConsumeIntegral<uint32_t>();
    screenModeInfo.SetScreenRefreshRate(refreshRate);
    return true;
}

bool DoSetScreenModeId(FuzzedDataProvider& fdp)
{
    // test
    RSScreenModeInfo screenModeInfo;
    int32_t id = fdp.ConsumeIntegral<int32_t>();
    screenModeInfo.SetScreenModeId(id);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }
    auto fdp = FuzzedDataProvider(data, size);
    /* Run your code on data */
    OHOS::Rosen::DoSetScreenWidth(fdp);
    OHOS::Rosen::DoSetScreenHeight(fdp);
    OHOS::Rosen::DoSetScreenRefreshRate(fdp);
    OHOS::Rosen::DoSetScreenModeId(fdp);
    return 0;
}
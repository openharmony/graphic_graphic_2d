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

#include "rsframeratelinkercommand_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "command/rs_frame_rate_linker_command.h"

namespace OHOS {
namespace Rosen {
bool DoFrameRateLinkerCommand(FuzzedDataProvider& fdp)
{
    RSContext context;
    FrameRateLinkerId id = fdp.ConsumeIntegral<FrameRateLinkerId>();
    int32_t minRate = fdp.ConsumeIntegral<int32_t>();
    int32_t maxRate = fdp.ConsumeIntegral<int32_t>();
    int32_t preferredRate = fdp.ConsumeIntegral<int32_t>();
    int32_t animatorExpectedFrameRate = fdp.ConsumeIntegral<int32_t>();
    FrameRateRange range(minRate, maxRate, preferredRate);

    RSFrameRateLinkerCommandHelper::Destroy(context, id);
    RSFrameRateLinkerCommandHelper::UpdateRange(context, id, range, animatorExpectedFrameRate);
    return true;
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::DoFrameRateLinkerCommand(fdp);
    return 0;
}

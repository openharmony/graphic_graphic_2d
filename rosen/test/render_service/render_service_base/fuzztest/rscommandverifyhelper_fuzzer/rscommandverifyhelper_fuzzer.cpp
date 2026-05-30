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

#include "rscommandverifyhelper_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "command/rs_command_verify_helper.h"

namespace OHOS {
namespace Rosen {
bool DoCommandVerifyHelper(FuzzedDataProvider& fdp)
{
    pid_t pid = fdp.ConsumeIntegral<pid_t>();

    RsCommandVerifyHelper& helper = RsCommandVerifyHelper::GetInstance();
    helper.RegisterNonSystemPid(pid);
    helper.AddSurfaceNodeCreateCnt(pid);
    helper.IsSurfaceNodeCreateCommandVaild(pid);
    helper.SubSurfaceNodeCreateCnt(pid);
    helper.RemoveCntWithPid(pid);
    return true;
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::DoCommandVerifyHelper(fdp);
    return 0;
}

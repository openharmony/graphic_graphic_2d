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

#include "vsyncsafe_fuzzer.h"

#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>
#include "vsync_distributor.h"
#include "vsync_controller.h"

namespace OHOS {
enum IpcCode : std::int16_t {
    SET_QOS_VSYNC_RATE_BY_PID_PUBLIC = 0,
    SET_QOS_VSYNC_RATE_BY_PID = 1,
};
    static sptr<Rosen::VSyncGenerator> vsyncGenerator_ = Rosen::CreateVSyncGenerator();
    static sptr<Rosen::VSyncController> vsyncController_ = new Rosen::VSyncController(vsyncGenerator_, 0);
    static sptr<Rosen::VSyncDistributor> vsyncDistributor_ = new Rosen::VSyncDistributor(vsyncController_, "Fuzz");
    static sptr<Rosen::VSyncConnection> conn_ = new Rosen::VSyncConnection(vsyncDistributor_, "Fuzz");

    void DoSomethingInterestingWithMyAPI(FuzzedDataProvider& fdp)
    {
        static const int ipcCodes[] = {
        IpcCode::SET_QOS_VSYNC_RATE_BY_PID_PUBLIC,
        IpcCode::SET_QOS_VSYNC_RATE_BY_PID,
        };
        int code = fdp.PickValueInArray(ipcCodes);
        switch (code) {
            case IpcCode::SET_QOS_VSYNC_RATE_BY_PID_PUBLIC: {
                uint32_t pid = fdp.ConsumeIntegral<uint32_t>();
                uint32_t rate = fdp.ConsumeIntegral<uint32_t>();
                bool isSystemAnimateScene = fdp.ConsumeIntegral<bool>();
                vsyncDistributor_->SetQosVSyncRateByPidPublic(pid, rate, isSystemAnimateScene);
                break;
            }
            case IpcCode::SET_QOS_VSYNC_RATE_BY_PID: {
                uint32_t pid = fdp.ConsumeIntegral<uint32_t>();
                uint32_t rate = fdp.ConsumeIntegral<uint32_t>();
                bool isSystemAnimateScene = fdp.ConsumeIntegral<bool>();
                vsyncDistributor_->SetQosVSyncRateByPid(pid, rate, isSystemAnimateScene);
                break;
            }
            default: {
                break;
            }
        }
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OHOS::DoSomethingInterestingWithMyAPI(fdp);
    return 0;
}

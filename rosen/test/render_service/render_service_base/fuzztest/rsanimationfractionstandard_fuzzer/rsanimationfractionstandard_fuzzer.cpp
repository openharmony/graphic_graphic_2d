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

#include "rsanimationfractionstandard_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "animation/rs_animation_fraction.h"

namespace OHOS {
namespace Rosen {
namespace {
enum class FuzzMethod {
    SETANIMATIONSCALE = 0,
    GETANIMATIONFRACTION,
    UPDATEREMAINTIMEFRACTION,
    SETDIRECTIONAFTERSTART,
    SETLASTFRAMETIME,
    SETREPEATCALLBACKENABLE,
    ISSTARTRUNNING,
    UPDATEREVERSESTATE,
    ONANIMATIONSCALECHANGEDCALLBACK,
    END
};
} // namespace

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    RSAnimationFraction::Init();
    RSAnimationFraction fraction = {};
    FuzzedDataProvider FD(data, size);
    uint8_t methodCount = static_cast<uint8_t>(FuzzMethod::END) + 1;
    uint8_t methodOrder = FD.ConsumeIntegral<uint8_t>() % methodCount;
    switch (static_cast<FuzzMethod>(methodOrder)) {
        case FuzzMethod::SETANIMATIONSCALE: {
            RSAnimationFraction::SetAnimationScale(FD.ConsumeFloatingPoint<float>());
            break;
        }
        case FuzzMethod::GETANIMATIONFRACTION: {
            int64_t delayTime = FD.ConsumeIntegral<int64_t>();
            fraction.GetAnimationFraction(FD.ConsumeIntegral<int64_t>(), delayTime);
            break;
        }
        case FuzzMethod::UPDATEREMAINTIMEFRACTION: {
            fraction.UpdateRemainTimeFraction(FD.ConsumeFloatingPoint<float>(), FD.ConsumeIntegral<int>());
            break;
        }
        case FuzzMethod::SETDIRECTIONAFTERSTART: {
            fraction.SetDirectionAfterStart(static_cast<ForwardDirection>(FD.ConsumeIntegralInRange<uint8_t>(0, 1)));
            break;
        }
        case FuzzMethod::SETLASTFRAMETIME: {
            fraction.SetLastFrameTime(FD.ConsumeIntegral<int64_t>());
            break;
        }
        case FuzzMethod::SETREPEATCALLBACKENABLE: {
            fraction.SetRepeatCallbackEnable(FD.ConsumeBool());
            break;
        }
        case FuzzMethod::ISSTARTRUNNING: {
            fraction.IsStartRunning(FD.ConsumeIntegral<int64_t>(), FD.ConsumeIntegral<int64_t>());
            break;
        }
        case FuzzMethod::UPDATEREVERSESTATE: {
            fraction.UpdateReverseState(FD.ConsumeBool());
            break;
        }
        case FuzzMethod::ONANIMATIONSCALECHANGEDCALLBACK: {
            RSAnimationFraction::OnAnimationScaleChangedCallback(
                "persist.sys.graphic.animationscale", std::to_string(FD.ConsumeIntegral<int64_t>()).c_str(), nullptr);
            break;
        }
        default:
            return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

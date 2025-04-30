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

#include "rsanimationfraction_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "animation/rs_animation_fraction.h"
namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

void RSAnimationFractionFuzzerTest()
{
    // get data
    float animationScale = GetData<float>();
    int64_t time = GetData<int64_t>();
    bool isInStartDelay = GetData<bool>();
    bool isFinished = GetData<bool>();
    bool isRepeatFinished = GetData<bool>();
    bool isEnable = GetData<bool>();
    float fraction = GetData<float>();
    int remainTime = GetData<int>();
    ForwardDirection direction = GetData<ForwardDirection>();
    int64_t lastFrameTime = GetData<int64_t>();

    // test
    RSAnimationFraction::Init();
    RSAnimationFraction::GetAnimationScale();
    RSAnimationFraction::SetAnimationScale(animationScale);
    auto animationFraction = std::make_shared<RSAnimationFraction>();
    std::tie(fraction, isInStartDelay, isFinished, isRepeatFinished) = animationFraction->GetAnimationFraction(time);
    animationFraction->UpdateRemainTimeFraction(fraction, remainTime);
    animationFraction->GetRemainingRepeatCount();
    animationFraction->GetStartFraction();
    animationFraction->GetEndFraction();
    animationFraction->SetDirectionAfterStart(direction);
    animationFraction->SetLastFrameTime(lastFrameTime);
    animationFraction->GetLastFrameTime();
    animationFraction->GetCurrentIsReverseCycle();
    animationFraction->GetCurrentTimeFraction();
    animationFraction->SetRepeatCallbackEnable(isEnable);
    animationFraction->GetRepeatCallbackEnable();
    animationFraction->ResetFraction();
}

void RSAnimationFractionFuzzerTest1()
{
    // get data
    float animationScale = GetData<float>();
    int64_t time = GetData<int64_t>();
    int64_t startDelayNs = GetData<int64_t>();

    // test
    RSAnimationFraction::Init();
    RSAnimationFraction::GetAnimationScale();
    RSAnimationFraction::SetAnimationScale(animationScale);
    RSAnimationFraction::OnAnimationScaleChangedCallback("persist.sys.graphic.animationscale", "0", nullptr);
    RSAnimationFraction fraction;
    fraction.GetAnimationFraction(time);
    fraction.UpdateRemainTimeFraction(animationScale);
    fraction.GetStartFraction();
    fraction.GetEndFraction();
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    fraction.SetLastFrameTime(time);
    fraction.GetLastFrameTime();
    fraction.ResetFraction();
    fraction.GetRemainingRepeatCount();
    fraction.GetCurrentIsReverseCycle();
    fraction.SetRepeatCallbackEnable(true);
    fraction.GetRepeatCallbackEnable();
    fraction.IsInRepeat();
    fraction.IsFinished();
    fraction.UpdateReverseState(true);
    fraction.IsStartRunning(time, startDelayNs);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    RSAnimationFractionFuzzerTest();
    RSAnimationFractionFuzzerTest1();
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

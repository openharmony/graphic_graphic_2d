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

#include "rsanimationtimingcurve_fuzzer.h"

#include <securec.h>

#include "rs_animation.h"
#include "rs_animation_timing_curve.h"

namespace OHOS {
    using namespace Rosen;
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
    }

    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    /*
    * get a string from g_data
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

    void RsAnimationTimingCurveFuzzTest()
    {
        float ctrlX1 = GetData<float>();
        float ctrlY1 = GetData<float>();
        float ctrlX2 = GetData<float>();
        float ctrlY2 = GetData<float>();
        float velocity = GetData<float>();
        float mass = GetData<float>();
        float stiffness = GetData<float>();
        float damping = GetData<float>();
        float response = GetData<float>();
        float dampingRatio = GetData<float>();
        int32_t steps = GetData<int32_t>();
        float duration = GetData<float>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();

        auto customCurve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return input; });
        auto cubicCurve = RSAnimationTimingCurve::CreateCubicCurve(ctrlX1, ctrlY1, ctrlX2, ctrlY2);
        auto stepCurve = RSAnimationTimingCurve::CreateStepsCurve(steps, position);
        auto springCurve = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass, stiffness, damping);
        auto spring = RSAnimationTimingCurve::CreateSpring(response, dampingRatio);
        auto emptyCurve = std::make_shared<RSAnimationTimingCurve>();
        auto copyCurve = std::make_shared<RSAnimationTimingCurve>(stepCurve);
        auto interpolatingSpringCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(
            mass, stiffness, damping, velocity, dampingRatio);
        auto interpolator = interpolatingSpringCurve.GetInterpolator(duration);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;
        RsAnimationTimingCurveFuzzTest();
        return true;
    }
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}


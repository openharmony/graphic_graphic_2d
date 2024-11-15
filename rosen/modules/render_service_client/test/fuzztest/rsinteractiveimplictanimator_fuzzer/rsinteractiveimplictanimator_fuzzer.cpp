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

#include "rsinteractiveimplictanimator_fuzzer.h"

#include <securec.h>

#include "rs_animation_timing_curve.h"
#include "rs_interactive_implict_animator.h"
#include "ui/rs_canvas_node.h"

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

    void RSInteractiveImplictAnimatorAddImplictAnimationFuzzTest()
    {
        // get data
        auto duration = GetData<int>();
        auto translateX = GetData<float>();
        auto translateY = GetData<float>();
        auto fraction = GetData<float>();

        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(duration);
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetTranslate({ 0, 0 });
        auto interactiveAnimator = RSInteractiveImplictAnimator::Create(
            protocol, RSAnimationTimingCurve::DEFAULT);
        interactiveAnimator->SetFinishCallBack([]() {});
        interactiveAnimator->CallFinishCallback();
        interactiveAnimator->FinishOnCurrent();
        interactiveAnimator->AddImplictAnimation([&]() {
            canvasNode->SetTranslate({ translateX, translateY });
        });
        interactiveAnimator->StartAnimation();
        interactiveAnimator->AddImplictAnimation([]() {});
        interactiveAnimator->StartAnimation();
        interactiveAnimator->PauseAnimation();
        interactiveAnimator->ContinueAnimation();
        interactiveAnimator->PauseAnimation();
        interactiveAnimator->SetFraction(fraction);
        interactiveAnimator->GetFraction();
        interactiveAnimator->fractionAnimationId_ = 0;
        interactiveAnimator->GetFraction();
        interactiveAnimator->GetStatus();
        interactiveAnimator->ReverseAnimation();
        interactiveAnimator->FinishOnCurrent();
    }

    void RSInteractiveImplictAnimatorAddAnimationFuzzTest()
    {
        // get data
        auto duration = GetData<int>();
        auto translateX = GetData<float>();
        auto translateY = GetData<float>();
        auto position = GetData<RSInteractiveAnimationPosition>();
        auto fraction = GetData<float>();

        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(duration);
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetTranslate({ 0, 0 });
        auto interactiveAnimator = RSInteractiveImplictAnimator::Create(
            protocol, RSAnimationTimingCurve::DEFAULT);
        interactiveAnimator->SetFinishCallBack([]() {});
        interactiveAnimator->AddAnimation([&]() {
            RSNode::Animate(protocol, RSAnimationTimingCurve::DEFAULT, [&]() {
                canvasNode->SetTranslate({ translateX, translateY });
            }, []() {});
        });
        interactiveAnimator->StartAnimation();
        interactiveAnimator->AddAnimation([]() {});
        interactiveAnimator->StartAnimation();
        interactiveAnimator->PauseAnimation();
        interactiveAnimator->ContinueAnimation();
        interactiveAnimator->PauseAnimation();
        interactiveAnimator->SetFraction(fraction);
        interactiveAnimator->GetFraction();
        interactiveAnimator->GetStatus();
        interactiveAnimator->ReverseAnimation();
        interactiveAnimator->FinishAnimation(position);
    }

    void RSInteractiveImplictAnimatorUniEnabledFuzzTest()
    {
        // get data
        auto duration = GetData<int>();
        auto translateX = GetData<float>();
        auto translateY = GetData<float>();
        auto fraction = GetData<float>();

        // init env
        auto isUniEnabled = RSSystemProperties::GetUniRenderEnabled();
        RSSystemProperties::isUniRenderEnabled_ = !isUniEnabled;

        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(duration);
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetTranslate({ 0, 0 });
        auto interactiveAnimator = RSInteractiveImplictAnimator::Create(
            protocol, RSAnimationTimingCurve::DEFAULT);
        interactiveAnimator->SetFinishCallBack([]() {});
        interactiveAnimator->AddAnimation([&]() {
            canvasNode->SetTranslate({ translateX, translateY });
        });
        interactiveAnimator->StartAnimation();
        interactiveAnimator->PauseAnimation();
        interactiveAnimator->ContinueAnimation();
        interactiveAnimator->PauseAnimation();
        interactiveAnimator->SetFraction(fraction);
        interactiveAnimator->GetFraction();
        interactiveAnimator->GetStatus();
        interactiveAnimator->ReverseAnimation();
        interactiveAnimator->FinishAnimation(RSInteractiveAnimationPosition::CURRENT);

        RSSystemProperties::isUniRenderEnabled_ = isUniEnabled;
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
        RSInteractiveImplictAnimatorAddImplictAnimationFuzzTest();
        RSInteractiveImplictAnimatorAddAnimationFuzzTest();
        RSInteractiveImplictAnimatorUniEnabledFuzzTest();
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


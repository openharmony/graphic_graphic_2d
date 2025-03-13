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

#include "rsrenderinteractiveimplictanimator_fuzzer.h"

#include <securec.h>

#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interactive_implict_animator_map.h"
#include "animation/rs_render_interactive_implict_animator.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"

namespace OHOS {
    using namespace Rosen;
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    void RSRenderInteractiveImplictAnimatorAndMapFuzzerTest()
    {
        // get data
        auto animatorId = GetData<InteractiveImplictAnimatorId>();
        auto nodeId = GetData<NodeId>();
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        auto position = GetData<RSInteractiveAnimationPosition>();
        auto fraction = GetData<float>();
        auto value1 = GetData<float>();
        auto value2 = GetData<float>();
        auto value3 = GetData<float>();
        auto context = std::make_shared<RSContext>();

        // test
        auto interactiveAnimator = std::make_shared<RSRenderInteractiveImplictAnimator>(
            animatorId, context);
        auto renderNode = std::make_shared<RSCanvasRenderNode>(nodeId);
        context->GetMutableNodeMap().RegisterRenderNode(renderNode);
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(value1);
        auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(value2);
        auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(value3);
        auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
            animationId, propertyId, property, property1, property2);
        renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);
        std::vector<std::pair<NodeId, AnimationId>> animations;
        animations.emplace_back(nodeId, animationId);
        interactiveAnimator->AddAnimations(animations);
        interactiveAnimator->PauseAnimator();
        interactiveAnimator->ReverseAnimator();
        interactiveAnimator->PauseAnimator();
        interactiveAnimator->SetFractionAnimator(fraction);
        interactiveAnimator->ContinueAnimator();
        interactiveAnimator->FinishAnimator(position);
        
        context->GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(interactiveAnimator);
        context->GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(animatorId);
        context->GetInteractiveImplictAnimatorMap().UnregisterInteractiveImplictAnimator(animatorId);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        data_ = data;
        size_ = size;
        pos = 0;
        RSRenderInteractiveImplictAnimatorAndMapFuzzerTest();
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}


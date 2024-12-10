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

#include "rsnodeshowingcommand_fuzzer.h"

#include <securec.h>

#include "animation/rs_render_curve_animation.h"
#include "command/rs_node_showing_command.h"
#include "pipeline/rs_render_node.h"

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

    void RSNodeGetShowingPropertyAndCancelAnimationFuzzerTest()
    {
        // get data
        RSContext context;
        NodeId nodeId = GetData<NodeId>();
        PropertyId propertyId = GetData<PropertyId>();
        auto time = GetData<uint64_t>();

        auto property = std::make_shared<RSRenderProperty<Drawing::Matrix>>();
        auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(nodeId, property);

        Parcel parcel;
        auto copy = RSNodeGetShowingPropertyAndCancelAnimation::Unmarshalling(parcel);
        task->ReadFromParcel(parcel);
        task->CheckHeader(parcel);
        task->Process(context);

        Parcel parcel1;
        RSMarshallingHelper::Marshalling(parcel1, nodeId);
        RSMarshallingHelper::Marshalling(parcel1, time);
        copy = RSNodeGetShowingPropertyAndCancelAnimation::Unmarshalling(parcel1);
        task->ReadFromParcel(parcel1);
        task->CheckHeader(parcel1);
        task->Process(context);

        Parcel parcel2;
        task->Marshalling(parcel2);
        copy = RSNodeGetShowingPropertyAndCancelAnimation::Unmarshalling(parcel2);
        task->ReadFromParcel(parcel2);
        task->CheckHeader(parcel2);
        task->Process(context);

        std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSBaseRenderNode>(nodeId);
        context.GetMutableNodeMap().RegisterRenderNode(node);
        task->Process(context);
        auto modifier = std::make_shared<RSGeometryTransRenderModifier>(property);
        node->modifiers_[propertyId] = modifier;
        task->Process(context);

        delete copy;
        copy = nullptr;
    }

    void RSNodeGetShowingPropertiesAndCancelAnimationFuzzerTest()
    {
        // get data
        RSContext context;
        NodeId nodeId = GetData<NodeId>();
        PropertyId propertyId = GetData<PropertyId>();
        AnimationId animationId = GetData<AnimationId>();
        auto time = GetData<uint64_t>();

        auto property = std::make_shared<RSRenderProperty<Drawing::Matrix>>();
        std::vector<AnimationId> animationIds = { animationId };
        std::pair<std::pair<NodeId, PropertyId>, std::pair<std::shared_ptr<RSRenderPropertyBase>,
            std::vector<AnimationId>>>
            newEntry(std::make_pair(nodeId, propertyId), std::make_pair(property, animationIds));
        auto task = std::make_shared<RSNodeGetShowingPropertiesAndCancelAnimation>(time);
        task->propertiesMap_.insert(newEntry);

        Parcel parcel;
        auto copy = RSNodeGetShowingPropertiesAndCancelAnimation::Unmarshalling(parcel);
        task->ReadFromParcel(parcel);
        task->CheckHeader(parcel);
        task->Process(context);

        Parcel parcel1;
        RSMarshallingHelper::Marshalling(parcel1, time);
        copy = RSNodeGetShowingPropertiesAndCancelAnimation::Unmarshalling(parcel1);
        task->ReadFromParcel(parcel1);
        task->CheckHeader(parcel1);
        task->Process(context);

        Parcel parcel2;
        task->Marshalling(parcel2);
        copy = RSNodeGetShowingPropertiesAndCancelAnimation::Unmarshalling(parcel2);
        task->ReadFromParcel(parcel2);
        task->CheckHeader(parcel2);
        task->Process(context);

        std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSBaseRenderNode>(nodeId);
        context.GetMutableNodeMap().RegisterRenderNode(node);
        task->Process(context);
        auto modifier = std::make_shared<RSGeometryTransRenderModifier>(property);
        node->modifiers_[propertyId] = modifier;
        task->Process(context);

        delete copy;
        copy = nullptr;
    }

    void RSNodeGetAnimationsValueFractionFuzzerTest()
    {
        // get data
        RSContext context;
        NodeId nodeId = GetData<NodeId>();
        AnimationId animationId = GetData<AnimationId>();
        auto time = GetData<uint64_t>();

        auto task = std::make_shared<RSNodeGetAnimationsValueFraction>(time, nodeId, animationId);

        Parcel parcel;
        auto copy = RSNodeGetAnimationsValueFraction::Unmarshalling(parcel);
        task->ReadFromParcel(parcel);
        task->CheckHeader(parcel);
        task->Process(context);

        Parcel parcel1;
        RSMarshallingHelper::Marshalling(parcel1, time);
        copy = RSNodeGetAnimationsValueFraction::Unmarshalling(parcel1);
        task->ReadFromParcel(parcel1);
        task->CheckHeader(parcel1);
        task->Process(context);

        Parcel parcel2;
        task->Marshalling(parcel2);
        copy = RSNodeGetAnimationsValueFraction::Unmarshalling(parcel2);
        task->ReadFromParcel(parcel2);
        task->CheckHeader(parcel2);
        task->Process(context);

        std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSBaseRenderNode>(nodeId);
        context.GetMutableNodeMap().RegisterRenderNode(node);
        task->Process(context);
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
            animationId, GetData<PropertyId>(), property, property1, property2);
        node->GetAnimationManager().AddAnimation(renderCurveAnimation);
        task->Process(context);

        delete copy;
        copy = nullptr;
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
        RSNodeGetShowingPropertyAndCancelAnimationFuzzerTest();
        RSNodeGetShowingPropertiesAndCancelAnimationFuzzerTest();
        RSNodeGetAnimationsValueFractionFuzzerTest();
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


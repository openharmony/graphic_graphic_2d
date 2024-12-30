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

#include "rsanimationtraceutils_fuzzer.h"

#include <securec.h>

#include "animation/rs_animation_trace_utils.h"
#include "pipeline/rs_root_render_node.h"

namespace OHOS {
    using namespace Rosen;
    static bool g_isInitDebugTraceEnv = false;
    namespace {
        constexpr size_t STR_LEN = 10;
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

    void RSAnimationTraceUtilsFuzzerTest()
    {
        // init debug trace env
        if (!g_isInitDebugTraceEnv) {
            system("param set persist.rosen.animationtrace.enabled 1");
            g_isInitDebugTraceEnv = true;
        }

        // get data
        const std::string nodeName = GetStringFromData(STR_LEN);
        const std::string info = GetStringFromData(STR_LEN);
        const uint64_t nodeId = GetData<uint64_t>();
        const uint64_t propertyId = GetData<uint64_t>();
        const uint64_t animationId = GetData<uint64_t>();
        const ImplicitAnimationParamType animationType = static_cast<ImplicitAnimationParamType>(GetData<int>());
        const int animationDelay = GetData<int>();
        const int animationDur = GetData<int>();
        const int repeat = GetData<int>();
        bool isAddLogInfo = GetData<bool>();
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto value = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        const float fraction = GetData<float>();
        const int64_t time = GetData<int64_t>();
        const std::string interfaceName = GetStringFromData(STR_LEN);
        const std::string frameNodeTag = GetStringFromData(STR_LEN);
        const int32_t frameNodeId = GetData<int32_t>();
        const RSUINodeType nodeType = static_cast<RSUINodeType>(GetData<uint32_t>());
        RSModifierType modifierType = static_cast<RSModifierType>(GetData<int16_t>());
        auto node = std::make_shared<RSRenderNode>(nodeId);

        // test
        RSAnimationTraceUtils::GetInstance().AddAnimationNameTrace(nodeName);
        RSAnimationTraceUtils::GetInstance().AddAnimationFinishTrace(info, nodeId, animationId, isAddLogInfo);
        RSAnimationTraceUtils::GetInstance().AddAnimationCallFinishTrace(
            nodeId, animationId, modifierType, isAddLogInfo);
        RSAnimationTraceUtils::GetInstance().AddAnimationCreateTrace(nodeId, nodeName, propertyId, animationId,
            animationType, modifierType, startValue, endValue, animationDelay, animationDur, repeat, interfaceName,
            frameNodeId, frameNodeTag, nodeType);
        RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(
            node.get(), animationId, propertyId, fraction, value, time, animationDur, repeat);
        RSAnimationTraceUtils::GetInstance().AddSpringInitialVelocityTrace(
            propertyId, animationId, initialVelocity, value);
        RSAnimationTraceUtils::GetInstance().AddAnimationCancelTrace(nodeId, propertyId);
        RSAnimationTraceUtils::GetInstance().AddChangeAnimationValueTrace(propertyId, endValue);
        RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(value);
        RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValueInner(value);
    }

    void RSAnimationTraceUtilsFuzzerTest2()
    {
        // init debug trace env
        if (!g_isInitDebugTraceEnv) {
            system("param set persist.rosen.animationtrace.enabled 1");
            g_isInitDebugTraceEnv = true;
        }

        // get data
        const ImplicitAnimationParamType animationType = static_cast<ImplicitAnimationParamType>(GetData<int>());
        const RSUINodeType nodeType = static_cast<RSUINodeType>(GetData<uint32_t>());
        RSModifierType modifierType = static_cast<RSModifierType>(GetData<int16_t>());
        int16_t red = GetData<int16_t>();
        int16_t green = GetData<int16_t>();
        int16_t blue = GetData<int16_t>();
        int16_t alpha = GetData<int16_t>();

        // test
        RSColor rscolor = RSColor(red, green, blue, alpha);
        RSAnimationTraceUtils::GetInstance().GetColorString(rscolor);
        const std::string key = GetStringFromData(STR_LEN);
        const std::string value1 = GetStringFromData(STR_LEN);
        RSAnimationTraceUtils::OnAnimationTraceEnabledChangedCallback(key.c_str(), value1.c_str(), nullptr);
        RSAnimationTraceUtils::GetInstance().GetModifierTypeString(modifierType);
        RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(animationType);
        RSAnimationTraceUtils::GetInstance().GetNodeTypeString(nodeType);
    }

    void ParseRenderPropertyValueTest()
    {
        // get data
    auto floatProperty = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(floatProperty, RSPropertyType::FLOAT);
    auto red = RgbPalette::Red();
    auto colorProperty = std::make_shared<RSRenderAnimatableProperty<Color>>(red);
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(colorProperty, RSPropertyType::RS_COLOR);
    auto matrixProperty = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>();
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(matrixProperty,
        RSPropertyType::MATRIX3F);
    auto quaternionProperty = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(
        Quaternion(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()));
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(quaternionProperty,
        RSPropertyType::QUATERNION);
    auto filterProperty = std::make_shared<RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(nullptr);
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(filterProperty,
        RSPropertyType::FILTER);
    auto vector2fProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        Vector2f(GetData<float>(), GetData<float>()));
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector2fProperty,
        RSPropertyType::VECTOR2F);
    auto vector4fProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(
        Vector4f(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()));
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector4fProperty,
        RSPropertyType::VECTOR4F);
    auto vector4ColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(Vector4<Color>(red));
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector4ColorProperty,
        RSPropertyType::VECTOR4_COLOR);
    RRect rect;
    auto invalidProperty = std::make_shared<RSRenderAnimatableProperty<RRect>>(rect);
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(invalidProperty, RSPropertyType::INVALID);
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
        RSAnimationTraceUtilsFuzzerTest();
        RSAnimationTraceUtilsFuzzerTest2();
        ParseRenderPropertyValueTest();
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


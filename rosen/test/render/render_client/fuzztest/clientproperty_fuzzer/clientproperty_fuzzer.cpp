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

#include "clientproperty_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "animation/rs_animation.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

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

bool DoSetAndGet(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    RSProperty<float> rsProperty(value);
    float value1 = GetData<float>();
    rsProperty.Set(value1);
    return ROSEN_EQ(rsProperty.Get(), value1);
}

bool DoSetThreshold01(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::COARSE);
    return ROSEN_EQ(prop->GetThreshold(), 1.0f / 256.0f);
}

bool DoSetThreshold02(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::LAYOUT);
    return ROSEN_EQ(prop->GetThreshold(), 0.5f);
}

bool DoSetThreshold03(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::FINE);
    return ROSEN_EQ(prop->GetThreshold(), 1.0f / 3072.0f);
}

bool DoSetThreshold04(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::MEDIUM);
    return ROSEN_EQ(prop->GetThreshold(), 1.0f / 1000.0f);
}

bool DoSetThreshold05(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::COLOR);
    return ROSEN_EQ(prop->GetThreshold(), 0.0f);
}

bool DoSetThreshold06(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::DEFAULT);
    return ROSEN_EQ(prop->GetThreshold(), 1.0f / 256.0f);
}

bool DoSetThreshold07(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::ZERO);
    return ROSEN_EQ(prop->GetThreshold(), 0.0f);
}

bool DoSetValueFromRender(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto renderprop = prop->GetRenderProperty();
    prop->SetValueFromRender(renderprop);
    return true;
}

bool DoAttachMod(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    prop->AttachModifier(modifier);
    return true;
}

bool DoGetRenderProp(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto renderprop = prop->GetRenderProperty();
    return true;
}

bool DoAddr(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    std::shared_ptr<RSPropertyBase> property;
    prop->Add(property);
    return true;
}

bool DoAddPathAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->AddPathAnimation();
    return true;
}

bool DoAnimateWithInitialVelocity(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    RSAnimationTimingProtocol timingProtocol;
    auto customCurveFunc = std::function<float(float)>();
    RSAnimationTimingCurve rsAnimationTimingCurve(customCurveFunc);
    std::shared_ptr<RSPropertyBase> property;
    prop->AnimateWithInitialVelocity(timingProtocol, rsAnimationTimingCurve, property, prop);
    return true;
}

bool DoGetStagingValue(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->GetStagingValue();
    return true;
}

bool DoRequestCancelAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->RequestCancelAnimation();
    return true;
}

bool DoSetPropertyUnit(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetPropertyUnit(RSPropertyUnit::PIXEL_POSITION);
    return true;
}

bool DoGetId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->GetId();
    return true;
}

bool DoGetShowingValue(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->GetShowingValueAndCancelAnimation();
    return true;
}

bool DoUpdateCustomAnimat(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->UpdateCustomAnimation();
    return true;
}

bool DoUpdateOnAllAnimat(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->UpdateOnAllAnimationFinish();
    return true;
}

bool DoRSProperty(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSPropertyBase propertyBase;
    propertyBase.MarkModifierDirty();
    propertyBase.MarkNodeDirty();
    return true;
}

bool DoOper(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    auto a = std::make_shared<RSPropertyBase>();
    a += b;
    auto b = std::make_shared<RSPropertyBase>();
    a -= b;
    float value = GetData<float>();
    a *= value;
    a = a + b;
    a = a - b;
    a = a * value;
    if (a == b) {
    }
    if (a != b) {
    }
    return true;
}

bool DoUpdateToRender01(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    PropertyUpdateType enumNum = static_cast<PropertyUpdateType>(GetData<int>() % 3);

    Gravity valueG = GetData<Gravity>();
    RSProperty rsPropertyGravity(valueG);
    rsPropertyGravity.UpdateToRender(valueG, enumNum);
    Matrix3f valueMatrix3f = GetData<Matrix3f>();
    RSProperty rsPropertyMatrix3f(valueMatrix3f);
    rsPropertyMatrix3f.UpdateToRender(valueMatrix3f, enumNum);
    Quaternion valueQuaternion = GetData<Quaternion>();
    RSProperty rsPropertyQuaternion(valueQuaternion);
    rsPropertyQuaternion.UpdateToRender(valueQuaternion, enumNum);
    auto rsFilter = std::make_shared<RSFilter>();
    RSProperty rsPropertyRSFilter(rsFilter);
    rsPropertyRSFilter.UpdateToRender(rsFilter, enumNum);
    bool valuebool = GetData<bool>();
    RSProperty rsPropertyBool(valuebool);
    rsPropertyBool.UpdateToRender(valuebool, enumNum);
    int valueint = GetData<int>();
    RSProperty rsPropertyInt(valueint);
    rsPropertyInt.UpdateToRender(valueint, enumNum);
    float valuef = GetData<float>();
    RSProperty rsPropertyFloat(valuef);
    rsPropertyFloat.UpdateToRender(valuef, enumNum);
    Color valueColor = GetData<Color>();
    RSProperty rsPropertyColor(valueColor);
    rsPropertyColor.UpdateToRender(valueColor, enumNum);
    auto rsImage = std::make_shared<RSImage>();
    RSProperty rsPropertyRSImage(rsImage);
    rsPropertyRSImage.UpdateToRender(rsImage, enumNum);
    auto rsMask = std::make_shared<RSMask>();
    RSProperty rsPropertyRSMask(rsMask);
    rsPropertyRSMask.UpdateToRender(rsMask, enumNum);
    auto rsPath = std::make_shared<RSPath>();
    RSProperty rsPropertyRSPath(rsPath);
    rsPropertyRSPath.UpdateToRender(rsPath, enumNum);
    RSDynamicBrightnessPara rsDynamicBrightnessPara;
    RSProperty rsPropertyRSDynamicBrightnessPara(rsDynamicBrightnessPara);
    rsPropertyRSDynamicBrightnessPara.UpdateToRender(rsDynamicBrightnessPara, enumNum);
    return true;
}

bool DoUpdateToRender02(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float valuef = GetData<float>();
    PropertyUpdateType enumNum = static_cast<PropertyUpdateType>(GetData<int>() % 3);
    GradientDirection direct = static_cast<GradientDirection>(GetData<int>() % 11);
    std::vector<std::pair<float, float>> fractions;
    auto rsLinearGradientBlurPara = std::make_shared<RSLinearGradientBlurPara>(valuef, fractions, direct);
    RSProperty rsPropertyRSLinearGradientBlurPara(rsLinearGradientBlurPara);
    rsPropertyRSLinearGradientBlurPara.UpdateToRender(rsLinearGradientBlurPara, enumNum);
    RSWaterRipplePara rsWaterRipplePara;
    RSProperty rsPropertyRSWaterRipplePara(rsWaterRipplePara);
    rsPropertyRSWaterRipplePara.UpdateToRender(rsWaterRipplePara, enumNum);
    RSFlyOutPara rsFlyOutPara;
    RSProperty rsPropertyRSWaterRippleParaRSFlyOutPara(rsFlyOutPara);
    rsPropertyRSWaterRippleParaRSFlyOutPara.UpdateToRender(rsFlyOutPara, enumNum);
    Vector2f valueVector2f = GetData<Vector2f>();
    auto motionBlurParam = std::make_shared<MotionBlurParam>(valuef, valueVector2f);
    RSProperty rsPropertyMotionBlurParam(motionBlurParam);
    rsPropertyMotionBlurParam.UpdateToRender(motionBlurParam, enumNum);
    auto rsMagnifierParams = std::make_shared<RSMagnifierParams>();
    RSProperty rsPropertyRSMagnifierParams(rsMagnifierParams);
    rsPropertyRSMagnifierParams.UpdateToRender(rsMagnifierParams, enumNum);
    uint32_t emitterIndex = GetData<uint32_t>();
    auto emitterUpdater = std::make_shared<EmitterUpdater>(emitterIndex);
    RSProperty rsPropertyEmitterUpdater(emitterUpdater);
    rsPropertyEmitterUpdater.UpdateToRender(emitterUpdater, enumNum);
    auto particleNoiseFields = std::make_shared<ParticleNoiseFields>();
    RSProperty rsPropertyParticleNoiseFields(particleNoiseFields);
    rsPropertyParticleNoiseFields.UpdateToRender(particleNoiseFields, enumNum);
    auto rsShader = std::make_shared<RSShader>();
    RSProperty rsPropertyRSShader(rsShader);
    rsPropertyRSShader.UpdateToRender(rsShader, enumNum);
    RSProperty rsPropertyVector2f(valueVector2f);
    rsPropertyVector2f.UpdateToRender(valueVector2f, enumNum);
    return true;
}

bool DoUpdateToRender03(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    PropertyUpdateType enumNum = static_cast<PropertyUpdateType>(GetData<int>() % 3);
    Vector4<uint32_t> valueVector4u32 = GetData<Vector4<uint32_t>>();
    RSProperty rsPropertyVector4u32(valueVector4u32);
    rsPropertyVector4u32.UpdateToRender(valueVector4u32, enumNum);
    Vector4<Color> valueVector4C = GetData<Vector4<Color>>();
    RSProperty rsPropertyVector4Color(valueVector4C);
    rsPropertyVector4Color.UpdateToRender(valueVector4C, enumNum);
    Vector4f valueVector4f = GetData<Vector4f>();
    RSProperty rsPropertyVector4f(valueVector4f);
    rsPropertyVector4f.UpdateToRender(valueVector4f, enumNum);
    RRect valueRRect = GetData<RRect>();
    RSProperty rsPropertyRRect(valueRRect);
    rsPropertyRRect.UpdateToRender(valueRRect, enumNum);
    return true;
}

bool DoIsValid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float valuef = GetData<float>();
    RSProperty rsPropertyFloat(valuef);
    rsPropertyFloat.IsValid(valuef);
    Vector2f valueVector2f = GetData<Vector2f>();
    RSProperty rsPropertyVector2f(valueVector2f);
    rsPropertyVector2f.IsValid(valueVector2f);
    Vector4f valueVector4f = GetData<Vector4f>();
    RSProperty rsPropertyVector4f(valueVector4f);
    rsPropertyVector4f.IsValid(valueVector4f);
    return true;
}

bool DoGetPropertyType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float valuef = GetData<float>();
    RSAnimatableProperty rsAnimatablePropertyFloat(valuef);
    rsAnimatablePropertyFloat.GetPropertyType();
    Color valueColor = GetData<Color>();
    RSAnimatableProperty rsAnimatablePropertyColor(valueColor);
    rsAnimatablePropertyColor.GetPropertyType();
    Matrix3f valueMatrix3f = GetData<Matrix3f>();
    RSAnimatableProperty rsAnimatablePropertyMatrix3f(valueMatrix3f);
    rsAnimatablePropertyMatrix3f.GetPropertyType();
    Vector2f valueVector2f = GetData<Vector2f>();
    RSAnimatableProperty rsAnimatablePropertyVector2f(valueVector2f);
    rsAnimatablePropertyVector2f.GetPropertyType();
    Vector4f valueVector4f = GetData<Vector4f>();
    RSAnimatableProperty rsAnimatablePropertyVector4f(valueVector4f);
    rsAnimatablePropertyVector4f.GetPropertyType();
    Quaternion valueQuaternion = GetData<Quaternion>();
    RSAnimatableProperty rsAnimatablePropertyQuaternion(valueQuaternion);
    rsAnimatablePropertyQuaternion.GetPropertyType();
    auto rsFilter = std::make_shared<RSFilter>();
    RSAnimatableProperty rsAnimatablePropertyRSFilter(rsFilter);
    rsAnimatablePropertyRSFilter.GetPropertyType();
    Vector4<Color> valueVector4C = GetData<Vector4<Color>>();
    RSAnimatableProperty rsAnimatablePropertyVector4Color(valueVector4C);
    rsAnimatablePropertyVector4Color.GetPropertyType();
    RRect valueRRect = GetData<RRect>();
    RSAnimatableProperty rsAnimatablePropertyRRect(valueRRect);
    rsAnimatablePropertyRRect.GetPropertyType();
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSetAndGet(data, size);
    OHOS::Rosen::DoSetThreshold01(data, size);
    OHOS::Rosen::DoSetThreshold02(data, size);
    OHOS::Rosen::DoSetThreshold03(data, size);
    OHOS::Rosen::DoSetThreshold04(data, size);
    OHOS::Rosen::DoSetThreshold05(data, size);
    OHOS::Rosen::DoSetThreshold06(data, size);
    OHOS::Rosen::DoSetThreshold07(data, size);
    OHOS::Rosen::DoGetRenderProp(data, size);
    OHOS::Rosen::DoSetValueFromRender(data, size);
    OHOS::Rosen::DoAttachMod(data, size);
    OHOS::Rosen::DoAddr(data, size);
    OHOS::Rosen::DoAddPathAnimation(data, size);
    OHOS::Rosen::DoAnimateWithInitialVelocity(data, size);
    OHOS::Rosen::DoGetId(data, size);
    OHOS::Rosen::DoGetShowingValue(data, size);
    OHOS::Rosen::DoGetStagingValue(data, size);
    OHOS::Rosen::DoRequestCancelAnimation(data, size);
    OHOS::Rosen::DoSetPropertyUnit(data, size);
    OHOS::Rosen::DoUpdateCustomAnimation(data, size);
    OHOS::Rosen::DoUpdateOnAllAnimat(data, size);
    OHOS::Rosen::DoRSProperty(data, size);
    OHOS::Rosen::DoOper(data, size);
    OHOS::Rosen::DoUpdateToRender01(data, size);
    OHOS::Rosen::DoUpdateToRender02(data, size);
    OHOS::Rosen::DoUpdateToRender03(data, size);
    OHOS::Rosen::DoIsValid(data, size);
    OHOS::Rosen::DoGetPropertyType(data, size);
    return 0;
}


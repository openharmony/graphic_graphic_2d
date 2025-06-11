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

#include "rsclientproperty_fuzzer.h"

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
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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

bool DoSetAndGet(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    RSProperty<float> rsProperty(value);
    float value1 = GetData<float>();
    rsProperty.Set(value1);
    return ROSEN_EQ(rsProperty.Get(), value1);
}

bool DoSetThresholdType001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::LAYOUT);
    return ROSEN_EQ(prop->GetThreshold(), 0.5f);
}

bool DoSetThresholdType002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::COARSE);
    return ROSEN_EQ(prop->GetThreshold(), 1.0f / 256.0f);
}

bool DoSetThresholdType003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::MEDIUM);
    return ROSEN_EQ(prop->GetThreshold(), 1.0f / 1000.0f);
}

bool DoSetThresholdType004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::FINE);
    return ROSEN_EQ(prop->GetThreshold(), 1.0f / 3072.0f);
}

bool DoSetThresholdType005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::COLOR);
    return ROSEN_EQ(prop->GetThreshold(), 0.0f);
}

bool DoSetThresholdType006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::DEFAULT);
    return ROSEN_EQ(prop->GetThreshold(), 1.0f / 256.0f);
}

bool DoSetThresholdType007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetThresholdType(ThresholdType::ZERO);
    return ROSEN_EQ(prop->GetThreshold(), 0.0f);
}

bool DoGetRenderProperty(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto renderprop = prop->GetRenderProperty();
    return true;
}

bool DoSetValueFromRender(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto renderprop = prop->GetRenderProperty();
    prop->SetValueFromRender(renderprop);
    return true;
}

bool DoAttachModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    prop->AttachModifier(modifier);
    return true;
}

bool DoAdd(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
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
    g_data = data;
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
    g_data = data;
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

bool DoGetId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->GetId();
    return true;
}

bool DoGetShowingValueAndCancelAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->GetShowingValueAndCancelAnimation();
    return true;
}

bool DoGetStagingValue(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
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
    g_data = data;
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
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->SetPropertyUnit(RSPropertyUnit::PIXEL_POSITION);
    return true;
}

bool DoUpdateCustomAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->UpdateCustomAnimation();
    return true;
}

bool DoUpdateOnAllAnimationFinish(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    prop->UpdateOnAllAnimationFinish();
    return true;
}

bool DoRSPropertyBase(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSProperty<float> propertyBase;
    propertyBase.MarkModifierDirty();
    propertyBase.MarkNodeDirty();
    return true;
}

bool DoOperator(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSPropertyBase> a = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> b = std::make_shared<RSProperty<float>>();
    a += b;
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

bool DoUpdateToRender001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    PropertyUpdateType EnumNumber = static_cast<PropertyUpdateType>(GetData<int>() % 3);
    bool valuebool = GetData<bool>();
    RSProperty rsPropertyBool(valuebool);
    rsPropertyBool.UpdateToRender(valuebool, EnumNumber);
    float valuefloat = GetData<float>();
    RSProperty rsPropertyFloat(valuefloat);
    rsPropertyFloat.UpdateToRender(valuefloat, EnumNumber);
    int valueint = GetData<int>();
    RSProperty rsPropertyInt(valueint);
    rsPropertyInt.UpdateToRender(valueint, EnumNumber);
    Color valueColor = GetData<Color>();
    RSProperty rsPropertyColor(valueColor);
    rsPropertyColor.UpdateToRender(valueColor, EnumNumber);
    Gravity valueGravity = GetData<Gravity>();
    RSProperty rsPropertyGravity(valueGravity);
    rsPropertyGravity.UpdateToRender(valueGravity, EnumNumber);
    Matrix3f valueMatrix3f = GetData<Matrix3f>();
    RSProperty rsPropertyMatrix3f(valueMatrix3f);
    rsPropertyMatrix3f.UpdateToRender(valueMatrix3f, EnumNumber);
    Quaternion valueQuaternion = GetData<Quaternion>();
    RSProperty rsPropertyQuaternion(valueQuaternion);
    rsPropertyQuaternion.UpdateToRender(valueQuaternion, EnumNumber);
    auto rsFilter = std::make_shared<RSFilter>();
    RSProperty rsPropertyRSFilter(rsFilter);
    rsPropertyRSFilter.UpdateToRender(rsFilter, EnumNumber);
    auto rsImage = std::make_shared<RSImage>();
    RSProperty rsPropertyRSImage(rsImage);
    rsPropertyRSImage.UpdateToRender(rsImage, EnumNumber);
    auto rsMask = std::make_shared<RSMask>();
    RSProperty rsPropertyRSMask(rsMask);
    rsPropertyRSMask.UpdateToRender(rsMask, EnumNumber);
    auto rsPath = std::make_shared<RSPath>();
    RSProperty rsPropertyRSPath(rsPath);
    rsPropertyRSPath.UpdateToRender(rsPath, EnumNumber);
    RSDynamicBrightnessPara rsDynamicBrightnessPara;
    RSProperty rsPropertyRSDynamicBrightnessPara(rsDynamicBrightnessPara);
    rsPropertyRSDynamicBrightnessPara.UpdateToRender(rsDynamicBrightnessPara, EnumNumber);
    return true;
}

bool DoUpdateToRender002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float valuefloat = GetData<float>();
    PropertyUpdateType EnumNumber = static_cast<PropertyUpdateType>(GetData<int>() % 3);
    GradientDirection direction = static_cast<GradientDirection>(GetData<int>() % 11);
    std::vector<std::pair<float, float>> fractionStops;
    auto rsLinearGradientBlurPara = std::make_shared<RSLinearGradientBlurPara>(valuefloat, fractionStops, direction);
    RSProperty rsPropertyRSLinearGradientBlurPara(rsLinearGradientBlurPara);
    rsPropertyRSLinearGradientBlurPara.UpdateToRender(rsLinearGradientBlurPara, EnumNumber);
    RSWaterRipplePara rsWaterRipplePara;
    RSProperty rsPropertyRSWaterRipplePara(rsWaterRipplePara);
    rsPropertyRSWaterRipplePara.UpdateToRender(rsWaterRipplePara, EnumNumber);
    RSFlyOutPara rsFlyOutPara;
    RSProperty rsPropertyRSWaterRippleParaRSFlyOutPara(rsFlyOutPara);
    rsPropertyRSWaterRippleParaRSFlyOutPara.UpdateToRender(rsFlyOutPara, EnumNumber);
    Vector2f valueVector2f = GetData<Vector2f>();
    auto motionBlurParam = std::make_shared<MotionBlurParam>(valuefloat, valueVector2f);
    RSProperty rsPropertyMotionBlurParam(motionBlurParam);
    rsPropertyMotionBlurParam.UpdateToRender(motionBlurParam, EnumNumber);
    auto rsMagnifierParams = std::make_shared<RSMagnifierParams>();
    RSProperty rsPropertyRSMagnifierParams(rsMagnifierParams);
    rsPropertyRSMagnifierParams.UpdateToRender(rsMagnifierParams, EnumNumber);
    uint32_t emitterIndex = GetData<uint32_t>();
    auto emitterUpdater = std::make_shared<EmitterUpdater>(emitterIndex);
    RSProperty rsPropertyEmitterUpdater(emitterUpdater);
    rsPropertyEmitterUpdater.UpdateToRender(emitterUpdater, EnumNumber);
    auto particleNoiseFields = std::make_shared<ParticleNoiseFields>();
    RSProperty rsPropertyParticleNoiseFields(particleNoiseFields);
    rsPropertyParticleNoiseFields.UpdateToRender(particleNoiseFields, EnumNumber);
    auto rsShader = std::make_shared<RSShader>();
    RSProperty rsPropertyRSShader(rsShader);
    rsPropertyRSShader.UpdateToRender(rsShader, EnumNumber);
    RSProperty rsPropertyVector2f(valueVector2f);
    rsPropertyVector2f.UpdateToRender(valueVector2f, EnumNumber);
    return true;
}

bool DoUpdateToRender003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    PropertyUpdateType EnumNumber = static_cast<PropertyUpdateType>(GetData<int>() % 3);
    Vector4<uint32_t> valueVector4u32 = GetData<Vector4<uint32_t>>();
    RSProperty rsPropertyVector4u32(valueVector4u32);
    rsPropertyVector4u32.UpdateToRender(valueVector4u32, EnumNumber);
    Vector4<Color> valueVector4Color = GetData<Vector4<Color>>();
    RSProperty rsPropertyVector4Color(valueVector4Color);
    rsPropertyVector4Color.UpdateToRender(valueVector4Color, EnumNumber);
    Vector4f valueVector4f = GetData<Vector4f>();
    RSProperty rsPropertyVector4f(valueVector4f);
    rsPropertyVector4f.UpdateToRender(valueVector4f, EnumNumber);
    RRect valueRRect = GetData<RRect>();
    RSProperty rsPropertyRRect(valueRRect);
    rsPropertyRRect.UpdateToRender(valueRRect, EnumNumber);
    return true;
}

bool DoIsValid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float valuefloat = GetData<float>();
    RSProperty rsPropertyFloat(valuefloat);
    rsPropertyFloat.IsValid(valuefloat);
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
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float valuefloat = GetData<float>();
    RSAnimatableProperty rsAnimatablePropertyFloat(valuefloat);
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
    Vector4<Color> valueVector4Color = GetData<Vector4<Color>>();
    RSAnimatableProperty rsAnimatablePropertyVector4Color(valueVector4Color);
    rsAnimatablePropertyVector4Color.GetPropertyType();
    RRect valueRRect = GetData<RRect>();
    RSAnimatableProperty rsAnimatablePropertyRRect(valueRRect);
    rsAnimatablePropertyRRect.GetPropertyType();
    return true;
}

bool DoRSPropertyBase002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto property = std::make_shared<RSProperty<float>>();
    property->GetThreshold();
    auto rsRenderPropertyBase = std::make_shared<RSRenderProperty<bool>>();
    property->SetValueFromRender(rsRenderPropertyBase);
    bool isCustom = property->GetIsCustom();
    property->SetIsCustom(isCustom);
    auto propertyBase = std::make_shared<RSProperty<float>>();
    property->SetValue(propertyBase);
    property->GetPropertyType();
    property->UpdateOnAllAnimationFinish();
    property->UpdateCustomAnimation();
    property->AddPathAnimation();
    property->RemovePathAnimation();
    property->UpdateShowingValue(rsRenderPropertyBase);
    property->GetRenderProperty();
    property->GetShowingValueAndCancelAnimation();
    property->IsEqual(propertyBase);
    return true;
}

bool DoRSProperty(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSProperty<float>>(value);
    auto property2 = std::make_shared<RSProperty<float>>(value);
    property->SetValue(property2);
    property->Clone();
    return true;
}

bool DoRSAnimatableProperty(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    uint64_t id = GetData<uint64_t>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    bool isDelta = GetData<bool>();
    property->UpdateExtendedAnimatableProperty(value, isDelta);
    property->RemovePathAnimation();
    auto rsRenderProperty = std::make_shared<RSRenderProperty<float>>(value, id);
    property->UpdateShowingValue(rsRenderProperty);
    property->NotifyPropertyChange();
    property->Multiply(value);
    auto property2 = std::make_shared<RSAnimatableProperty<float>>(value);
    property->IsEqual(property2);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSetAndGet(data, size);
    OHOS::Rosen::DoSetThresholdType001(data, size);
    OHOS::Rosen::DoSetThresholdType002(data, size);
    OHOS::Rosen::DoSetThresholdType003(data, size);
    OHOS::Rosen::DoSetThresholdType004(data, size);
    OHOS::Rosen::DoSetThresholdType005(data, size);
    OHOS::Rosen::DoSetThresholdType006(data, size);
    OHOS::Rosen::DoSetThresholdType007(data, size);
    OHOS::Rosen::DoGetRenderProperty(data, size);
    OHOS::Rosen::DoSetValueFromRender(data, size);
    OHOS::Rosen::DoAttachModifier(data, size);
    OHOS::Rosen::DoAdd(data, size);
    OHOS::Rosen::DoAddPathAnimation(data, size);
    OHOS::Rosen::DoAnimateWithInitialVelocity(data, size);
    OHOS::Rosen::DoGetId(data, size);
    OHOS::Rosen::DoGetShowingValueAndCancelAnimation(data, size);
    OHOS::Rosen::DoGetStagingValue(data, size);
    OHOS::Rosen::DoRequestCancelAnimation(data, size);
    OHOS::Rosen::DoSetPropertyUnit(data, size);
    OHOS::Rosen::DoUpdateCustomAnimation(data, size);
    OHOS::Rosen::DoUpdateOnAllAnimationFinish(data, size);
    OHOS::Rosen::DoRSPropertyBase(data, size);
    OHOS::Rosen::DoOperator(data, size);
    OHOS::Rosen::DoUpdateToRender001(data, size);
    OHOS::Rosen::DoUpdateToRender002(data, size);
    OHOS::Rosen::DoUpdateToRender003(data, size);
    OHOS::Rosen::DoIsValid(data, size);
    OHOS::Rosen::DoGetPropertyType(data, size);
    OHOS::Rosen::DoRSPropertyBase002(data, size);
    OHOS::Rosen::DoRSProperty(data, size);
    OHOS::Rosen::DoRSAnimatableProperty(data, size);
    return 0;
}


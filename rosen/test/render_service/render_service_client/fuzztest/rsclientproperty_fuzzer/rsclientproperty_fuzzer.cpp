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
    return 0;
}


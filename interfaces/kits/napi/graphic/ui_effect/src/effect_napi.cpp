/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "effect_napi.h"
#include "ui_effect_napi_utils.h"

namespace OHOS {
namespace Rosen {

using namespace UIEffect;
static const std::string CLASS_NAME = "VisualEffect";
 
EffectNapi::EffectNapi()
{
}
 
EffectNapi::~EffectNapi()
{
}
 
thread_local napi_ref EffectNapi::sConstructor_ = nullptr;
 
napi_value EffectNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createEffect", CreateEffect),
        DECLARE_NAPI_STATIC_FUNCTION("createBrightnessBlender", CreateBrightnessBlender),
    };
 
    napi_value constructor = nullptr;
 
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(),
                                           NAPI_AUTO_LENGTH, Constructor,
                                           nullptr,
                                           sizeof(static_prop) / sizeof(static_prop[0]), static_prop,
                                           &constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, UIEFFECT_LOG_E("EffectNapi Init define class fail"));
 
    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, UIEFFECT_LOG_E("EffectNapi Init create reference fail"));

    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, UIEFFECT_LOG_E("EffectNapi Init get global fail"));
 
    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        UIEFFECT_LOG_E("EffectNapi Init set global named property fail"));
 
    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, UIEFFECT_LOG_E("EffectNapi Init set named property fail"));
 
    status = napi_define_properties(env, exports, UIEFFECT_ARRAY_SIZE(static_prop), static_prop);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, UIEFFECT_LOG_E("EffectNapi Init define properties fail"));
    return exports;
}
 
napi_value EffectNapi::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, UIEFFECT_LOG_E("EffectNapi Constructor parsing input fail"));
 
    EffectNapi *effectNapi = new(std::nothrow) EffectNapi();
    UIEFFECT_NAPI_CHECK_RET_D(effectNapi != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi Constructor effectNapi is nullptr"));
    
    status = napi_wrap(env, jsThis, effectNapi, EffectNapi::Destructor, nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, effectNapi,
        UIEFFECT_LOG_E("EffectNapi Constructor wrap fail"));
    return jsThis;
}
 
void EffectNapi::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    EffectNapi *effectNapi = static_cast<EffectNapi*>(nativeObject);
    UIEFFECT_NAPI_CHECK_RET_VOID_D(effectNapi != nullptr,
        UIEFFECT_LOG_E("EffectNapi Destructor nativeObject is nullptr"));
 
    delete effectNapi;
    effectNapi = nullptr;
}
 
napi_value EffectNapi::CreateEffect(napi_env env, napi_callback_info info)
{
    VisualEffect* effectObj = new(std::nothrow) VisualEffect();
    UIEFFECT_NAPI_CHECK_RET_D(effectObj != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateEffect effectObj is nullptr"));

    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, effectObj,
        UIEFFECT_LOG_E("EffectNapi CreateEffect create object fail"));
    status = napi_wrap(
        env, object, effectObj,
        [](napi_env env, void* data, void* hint) {
            VisualEffect* effectObj = (VisualEffect*)data;
            delete effectObj;
            effectObj = nullptr;
        },
        nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, effectObj,
        UIEFFECT_LOG_E("EffectNapi CreateEffect wrap fail"));
    napi_property_descriptor resultFuncs[] = {
        DECLARE_NAPI_FUNCTION("backgroundColorBlender", SetBackgroundColorBlender),
        DECLARE_NAPI_FUNCTION("hdrBrightnessRatio", SetHDRUIbrightness),
    };
    status = napi_define_properties(env, object, sizeof(resultFuncs) / sizeof(resultFuncs[0]), resultFuncs);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, effectObj,
        UIEFFECT_LOG_E("EffectNapi CreateEffect define properties fail"));
    return object;
}

napi_value ParseJsValue(napi_env env, napi_value jsObject, const std::string& name)
{
    napi_value value = nullptr;
    napi_get_named_property(env, jsObject, name.c_str(), &value);
    return value;
}

bool CheckCreateBrightnessBlender(napi_env env, napi_value jsObject)
{
    bool result = true;
    napi_status status = napi_has_named_property(env, jsObject, "cubicRate", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "quadraticRate", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "linearRate", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "degree", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "saturation", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "fraction", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "positiveCoefficient", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "negativeCoefficient", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    return true;
}

napi_value EffectNapi::CreateBrightnessBlender(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        UIEFFECT_LOG_E("CreateBrightnessBlender failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi CreateBrightnessBlender failed, is not system app");
        return nullptr;
    }

    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value argv[NUM_1];
    napi_value thisVar = nullptr;
    napi_status status;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateBrightnessBlender parsing input fail"));

    napi_value nativeObj = argv[0];
    UIEFFECT_NAPI_CHECK_RET_D(nativeObj != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateBrightnessBlender nativeObj is nullptr"));

    BrightnessBlender* blender = new(std::nothrow) BrightnessBlender();
    UIEFFECT_NAPI_CHECK_RET_D(blender != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateBrightnessBlender blender is nullptr"));

    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(CheckCreateBrightnessBlender(env, nativeObj) &&
        ParseBrightnessBlender(env, nativeObj, blender), nullptr, blender,
        UIEFFECT_LOG_E("EffectNapi CreateBrightnessBlender fail"));

    status = napi_wrap(
        env, nativeObj, blender,
        [](napi_env env, void* data, void* hint) {
            BrightnessBlender* blenderObj = (BrightnessBlender*)data;
            delete blenderObj;
        },
        nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, blender,
        UIEFFECT_LOG_E("EffectNapi CreateBrightnessBlender wrap fail"));

    return nativeObj;
}
 
static bool IsArrayForNapiValue(napi_env env, napi_value param, uint32_t &arraySize)
{
    bool isArray = false;
    arraySize = 0;
    if ((napi_is_array(env, param, &isArray) != napi_ok) || (isArray == false)) {
        return false;
    }
    if (napi_get_array_length(env, param, &arraySize) != napi_ok) {
        return false;
    }
    return true;
}
 
bool ParseJsVec3Value(napi_env env, napi_value jsObject, const std::string& name, Vector3f& vecTmp)
{
    napi_value param = nullptr;
    napi_get_named_property(env, jsObject, name.c_str(), &param);
    if (param == nullptr) {
        return false;
    }
    napi_valuetype valueType = napi_undefined;
    valueType = UIEffectNapiUtils::GetType(env, param);
    if (valueType == napi_undefined) {
        return true;
    }
    uint32_t arraySize = 0;
    if (!IsArrayForNapiValue(env, param, arraySize)) {
        UIEFFECT_LOG_E("ParseJsVec3Value: get args fail, not array");
        return false;
    }
    if (arraySize < NUM_3) {
        UIEFFECT_LOG_E("ParseJsVec3Value: get args fail, array size less than 3");
        return false;
    }
    for (size_t i = 0; i < NUM_3; i++) {
        napi_value jsValue;
        if ((napi_get_element(env, param, i, &jsValue)) != napi_ok) {
            UIEFFECT_LOG_E("ParseJsVec3Value: get args fail, get value of element fail");
            return false;
        }
        double value = 0.0;
        if (napi_get_value_double(env, jsValue, &value) == napi_ok) {
            vecTmp[i] = value;
        } else {
            UIEFFECT_LOG_E("ParseJsVec3Value: get args fail, value of element not double");
            return false;
        }
    }
    return true;
}
 
bool EffectNapi::ParseBrightnessBlender(napi_env env, napi_value jsObject, BrightnessBlender* blender)
{
    double val;
    Vector3f tmpVector3;
    int parseTimes = 0;
    if (ParseJsDoubleValue(env, jsObject, "cubicRate", val)) {
        blender->SetCubicRate(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, jsObject, "quadraticRate", val)) {
        blender->SetQuadRate(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, jsObject, "linearRate", val)) {
        blender->SetLinearRate(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, jsObject, "degree", val)) {
        blender->SetDegree(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, jsObject, "saturation", val)) {
        blender->SetSaturation(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, jsObject, "fraction", val)) {
        blender->SetFraction(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsVec3Value(env, jsObject, "positiveCoefficient", tmpVector3)) {
        blender->SetPositiveCoeff(tmpVector3);
        parseTimes++;
    }
    if (ParseJsVec3Value(env, jsObject, "negativeCoefficient", tmpVector3)) {
        blender->SetNegativeCoeff(tmpVector3);
        parseTimes++;
    }
    return (parseTimes == NUM_8);
}
 
napi_value EffectNapi::SetBackgroundColorBlender(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        UIEFFECT_LOG_E("SetBackgroundColorBlender failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi SetBackgroundColorBlender failed, is not system app");
        return nullptr;
    }
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argValue, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        UIEFFECT_LOG_E("EffectNapi SetBackgroundColorBlender parsing input fail"));
 
    std::shared_ptr<BrightnessBlender> blender = std::make_shared<BrightnessBlender>();
    UIEFFECT_NAPI_CHECK_RET_D(blender != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi SetBackgroundColorBlender blender is nullptr"));
    UIEFFECT_NAPI_CHECK_RET_D(ParseBrightnessBlender(env, argValue[0], blender.get()), nullptr,
        UIEFFECT_LOG_E("EffectNapi SetBackgroundColorBlender blender is nullptr"));
 
    std::shared_ptr<BackgroundColorEffectPara> para = std::make_shared<BackgroundColorEffectPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi SetBackgroundColorBlender para is nullptr"));
    para->SetBlender(blender);
 
    VisualEffect* effectObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&effectObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && effectObj != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi SetBackgroundColorBlender effectObj is nullptr"));
    effectObj->AddPara(para);
    return thisVar;
}

napi_value EffectNapi::SetHDRUIBrightness(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        UIEFFECT_LOG_E("EffectNapi SetHDRUIBrightness: is not system app");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "The SetHDRUIBrightness is only accessible to system applications.");
        return nullptr;
    }
    size_t argc = NUM_1;
    napi_value argv[NUM_1] = {0};
    napi_value thisVar = nullptr;
    if (napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr) != napi_ok) {
        UIEFFECT_LOG_E("EffectNapi SetHDRUIBrightness: parsing input fail");
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_number) {
        UIEFFECT_LOG_E("EffectNapi SetHDRUIBrightness: input is not number");
        return nullptr;
    }
    double brightnessRatio = 1.0;
    if (napi_get_value_double(env, argv[0], &brightnessRatio) != napi_ok) {
        UIEFFECT_LOG_E("EffectNapi SetHDRUIBrightness: parsing float fail");
        return nullptr;
    }
    VisualEffect* effectObj = nullptr;
    if (napi_unwrap(env, thisVar, reinterpret_cast<void**>(&effectObj)) != napi_ok || effectObj == nullptr) {
        UIEFFECT_LOG_E("EffectNapi SetHDRUIBrightness: effectObj is nullptr");
        return nullptr;
    }
    std::shared_ptr<HdrEffectPara> para = std::make_shared<HdrEffectPara>();
    para->SetBrightnessRatio(static_cast<float>(brightnessRatio));
    effectObj->AddPara(para);
    return thisVar;
}
}  // namespace Rosen
}  // namespace OHOS

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

#include "mask_napi.h"

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
        DECLARE_NAPI_STATIC_FUNCTION("createHdrBrightnessBlender", CreateHdrBrightnessBlender),
        DECLARE_NAPI_STATIC_FUNCTION("createShadowBlender", CreateShadowBlender),
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
        DECLARE_NAPI_FUNCTION("borderLight", CreateBorderLight),
        DECLARE_NAPI_FUNCTION("colorGradient", CreateColorGradientEffect),
        DECLARE_NAPI_FUNCTION("liquidMaterial", CreateHarmoniumEffect),
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
    if (!UIEffectNapiUtils::IsSystemApp() && !UIEffectNapiUtils::IsFormRenderServiceCall()) {
        UIEFFECT_LOG_E("CreateBrightnessBlender failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi CreateBrightnessBlender failed, is not system app or frs call");
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

napi_value EffectNapi::CreateHdrBrightnessBlender(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        UIEFFECT_LOG_E("CreateHdrBrightnessBlender failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi CreateHdrBrightnessBlender failed, is not system app");
        return nullptr;
    }

    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value argv[NUM_1];
    napi_value thisVar = nullptr;
    napi_status status;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateHdrBrightnessBlender parsing input fail"));

    napi_value nativeObj = argv[0];
    UIEFFECT_NAPI_CHECK_RET_D(nativeObj != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateHdrBrightnessBlender nativeObj is nullptr"));

    BrightnessBlender* blender = new(std::nothrow) BrightnessBlender();
    UIEFFECT_NAPI_CHECK_RET_D(blender != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateHdrBrightnessBlender blender is nullptr"));

    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(CheckCreateBrightnessBlender(env, nativeObj) &&
        ParseBrightnessBlender(env, nativeObj, blender), nullptr, blender,
        UIEFFECT_LOG_E("EffectNapi CreateBrightnessBlender fail"));

    blender->SetHdr(true);

    status = napi_wrap(
        env, nativeObj, blender,
        [](napi_env env, void* data, void* hint) {
            BrightnessBlender* blenderObj = (BrightnessBlender*)data;
            delete blenderObj;
        },
        nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, blender,
        UIEFFECT_LOG_E("EffectNapi CreateHdrBrightnessBlender wrap fail"));

    return nativeObj;
}

napi_value EffectNapi::CreateBorderLight(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi CreateBorderLight failed, is not system app");
        return nullptr;
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_4] = {0};
    size_t argCount = NUM_4;
    UIEFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateBorderLight parsing input fail"));

    if (argCount != NUM_4) {
        UIEFFECT_LOG_E("Args number less than 4");
        return nullptr;
    }
    std::shared_ptr<BorderLightEffectPara> para = std::make_shared<BorderLightEffectPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateBorderLight para is nullptr"));

    UIEFFECT_NAPI_CHECK_RET_D(GetBorderLight(env, argValue, argCount, para), nullptr,
        UIEFFECT_LOG_E("EffectNapi GetBorderLight fail"));

    float lightIntensity = 0.f;
    lightIntensity = GetSpecialValue(env, argValue[NUM_2]);
    para->SetLightIntensity(lightIntensity);

    float lightWidth = 0.f;
    lightWidth = GetSpecialValue(env, argValue[NUM_3]);
    para->SetLightWidth(lightWidth);

    VisualEffect* visualEffectObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&visualEffectObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && visualEffectObj != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateBorderLight napi_unwrap fail"));
    visualEffectObj->AddPara(para);

    return thisVar;
}

bool EffectNapi::GetBorderLight(napi_env env, napi_value *param, size_t length,
    std::shared_ptr<BorderLightEffectPara>& para)
{
    if (length < NUM_2) {
        UIEFFECT_LOG_E("FilterNapi GetBorderLight array length is less than 2");
        return false;
    }

    Vector3f lightPosition = {0.0f, 0.0f, 0.0f};
    Vector4f lightColor = {0.0f, 0.0f, 0.0f, 0.0f};

    if (!ParseJsVector3f(env, param[0], lightPosition)) {
        UIEFFECT_LOG_E("FilterNapi GetBorderLight parse lightPosition fail");
        return false;
    }
    para->SetLightPosition(lightPosition);

    if (!ParseJsRGBAColor(env, param[1], lightColor)) {
        UIEFFECT_LOG_E("FilterNapi GetBorderLight parse lightColor fail");
        return false;
    }
    para->SetLightColor(lightColor);
    return true;
}

float EffectNapi::GetSpecialValue(napi_env env, napi_value argValue)
{
    double tmp = 0.0;
    if (UIEffectNapiUtils::GetType(env, argValue) == napi_number &&
        napi_get_value_double(env, argValue, &tmp) == napi_ok && tmp >= 0) {
            return static_cast<float>(tmp);
    }
    return tmp;
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

bool ParseJsVec4Value(napi_env env, napi_value jsObject, const std::string& name, Vector4f& vecTmp)
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
        UIEFFECT_LOG_E("ParseJsVec4Value: get args fail, not array");
        return false;
    }
    if (arraySize < NUM_4) {
        UIEFFECT_LOG_E("ParseJsVec4Value: get args fail, array size less than 3");
        return false;
    }
 
    for (size_t i = 0; i < NUM_4; i++) {
        napi_value jsValue;
        if ((napi_get_element(env, param, i, &jsValue)) != napi_ok) {
            UIEFFECT_LOG_E("ParseJsVec4Value: get args fail, get value of element fail");
            return false;
        }
        double value = 0.0;
        if (napi_get_value_double(env, jsValue, &value) == napi_ok) {
            vecTmp[i] = value;
        } else {
            UIEFFECT_LOG_E("ParseJsVec4Value: get args fail, value of element not double");
            return false;
        }
    }
    return true;
}

bool ParseJsVectorValue(napi_env env, napi_value jsObject, const std::string& name, std::vector<Vector2f>& vecTmp)
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
        UIEFFECT_LOG_E("ParseJsVectorValue: get args fail, not array");
        return false;
    }

    if (arraySize > NUM_10) {
        UIEFFECT_LOG_E("ParseJsVectorValue: the length of array is over 10");
        return false;
    }
 
    for (size_t i = 0; i < arraySize; i++) {
        napi_value jsValue;
        if ((napi_get_element(env, param, i, &jsValue)) != napi_ok) {
            UIEFFECT_LOG_E("ParseJsVectorValue: get args fail, get value of element fail");
            return false;
        }
        Vector2f position;
        if (!ParseJsVector2f(env, jsValue, position)) {
            UIEFFECT_LOG_E("ParseJsVectorValue: parse args fail, parse Vector2f fail");
            return false;
        }
        vecTmp.push_back(position);
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

bool ParseHarmoniumEffectPara(napi_env env, napi_value jsObject, HarmoniumEffectPara* para)
{
    double val;
    std::vector<Vector2f> tmpVector;
    Vector4f tmpVector4;
    int parseTimes = 0;
    bool enableVal = false;
 
    if (ParseJsBoolValue(env, jsObject, "enable", enableVal)) {
        para->SetEnable(static_cast<bool>(enableVal));
        parseTimes++;
    }
 
    if (ParseJsDoubleValue(env, jsObject, "distortProgress", val)) {
        para->SetDistortProgress(static_cast<float>(val));
        parseTimes++;
    }
 
    if (ParseJsDoubleValue(env, jsObject, "rippleProgress", val)) {
        para->SetRippleProgress(static_cast<float>(val));
        parseTimes++;
    }
 
    if (ParseJsDoubleValue(env, jsObject, "materialFactor", val)) {
        para->SetMaterialFactor(static_cast<float>(val));
        parseTimes++;
    }
 
    if (ParseJsDoubleValue(env, jsObject, "distortFactor", val)) {
        para->SetDistortFactor(static_cast<float>(val));
        parseTimes++;
    }
 
    if (ParseJsDoubleValue(env, jsObject, "reflectionFactor", val)) {
        para->SetReflectionFactor(static_cast<float>(val));
        parseTimes++;
    }
 
    if (ParseJsDoubleValue(env, jsObject, "refractionFactor", val)) {
        para->SetRefractionFactor(static_cast<float>(val));
        parseTimes++;
    }
 
    if (ParseJsVec4Value(env, jsObject, "tintColor", tmpVector4)) {
        para->SetTintColor(tmpVector4);
        parseTimes++;
    }
 
    if (ParseJsVectorValue(env, jsObject, "ripplePosition", tmpVector)) {
        para->SetRipplePosition(tmpVector);
        parseTimes++;
    }
 
    return (parseTimes == NUM_9);
}

bool ParseHarmoniumSelectablePara(napi_env env, napi_value jsObject, HarmoniumEffectPara* para)
{
    double val;
    Vector3f tmpVector3;
    int parseTimes = 0;

    if (ParseJsDoubleValue(env, jsObject, "rate", val)) {
        para->SetRate(static_cast<float>(val));
        parseTimes++;
    }

    if (ParseJsDoubleValue(env, jsObject, "lightUpDegree", val)) {
        para->SetLightUpDegree(static_cast<float>(val));
        parseTimes++;
    }

    if (ParseJsDoubleValue(env, jsObject, "cubicCoeff", val)) {
        para->SetCubicCoeff(static_cast<float>(val));
        parseTimes++;
    }

    if (ParseJsDoubleValue(env, jsObject, "quadCoeff", val)) {
        para->SetQuadCoeff(static_cast<float>(val));
        parseTimes++;
    }

    if (ParseJsDoubleValue(env, jsObject, "saturation", val)) {
        para->SetSaturation(static_cast<float>(val));
        parseTimes++;
    }

    if (ParseJsVec3Value(env, jsObject, "posRgb", tmpVector3)) {
        para->SetPosRGB(tmpVector3);
        parseTimes++;
    }

    if (ParseJsVec3Value(env, jsObject, "negRgb", tmpVector3)) {
        para->SetNegRGB(tmpVector3);
        parseTimes++;
    }

    if (ParseJsDoubleValue(env, jsObject, "fraction", val)) {
        para->SetFraction(static_cast<float>(val));
        parseTimes++;
    }

    return (parseTimes == NUM_8);
}
 

napi_value EffectNapi::CreateShadowBlender(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        UIEFFECT_LOG_E("CreateShadowBlender failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi CreateShadowBlender failed, is not system app");
        return nullptr;
    }

    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value argv[NUM_1];
    napi_value thisVar = nullptr;
    napi_status status;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateShadowBlender parsing input fail"));

    napi_value nativeObj = argv[0];
    UIEFFECT_NAPI_CHECK_RET_D(nativeObj != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateShadowBlender nativeObj is nullptr"));

    ShadowBlender* blender = new(std::nothrow) ShadowBlender();
    UIEFFECT_NAPI_CHECK_RET_D(blender != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateShadowBlender blender is nullptr"));

    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(CheckCreateShadowBlender(env, nativeObj) &&
        ParseShadowBlender(env, nativeObj, blender), nullptr, blender,
        UIEFFECT_LOG_E("EffectNapi CreateShadowBlender parse failed"));

    status = napi_wrap(
        env, nativeObj, blender,
        [](napi_env env, void* data, void* hint) {
            ShadowBlender* blenderObj = (ShadowBlender*)data;
            delete blenderObj;
        },
        nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, blender,
        UIEFFECT_LOG_E("EffectNapi CreateShadowBlender wrap fail"));

    return nativeObj;
}

bool EffectNapi::CheckCreateShadowBlender(napi_env env, napi_value jsObject)
{
    bool result = true;
    napi_status status = napi_has_named_property(env, jsObject, "cubicCoeff", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "quadraticCoeff", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "linearCoeff", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    status = napi_has_named_property(env, jsObject, "constantTerm", &result);
    if (!((status == napi_ok) && result)) {
        return false;
    }
    return true;
}

bool EffectNapi::ParseShadowBlender(napi_env env, napi_value jsObject, ShadowBlender* blender)
{
    double val;
    int parseTimes = 0;
    if (ParseJsDoubleValue(env, jsObject, "cubicCoeff", val)) {
        blender->SetCubicCoeff(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, jsObject, "quadraticCoeff", val)) {
        blender->SetQuadraticCoeff(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, jsObject, "linearCoeff", val)) {
        blender->SetLinearCoeff(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, jsObject, "constantTerm", val)) {
        blender->SetConstantTerm(static_cast<float>(val));
        parseTimes++;
    }
    return (parseTimes == NUM_4);
}
 
napi_value EffectNapi::SetBackgroundColorBlender(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp() && !UIEffectNapiUtils::IsFormRenderServiceCall()) {
        UIEFFECT_LOG_E("SetBackgroundColorBlender failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi SetBackgroundColorBlender failed, is not system app or frs call");
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

bool EffectNapi::GetColorGradientArray(napi_env env, napi_value* argValue,
    std::shared_ptr<ColorGradientEffectPara>& para, uint32_t arraySize)
{
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, false,
        UIEFFECT_LOG_E("EffectNapi GetColorGradientArray para is nullptr"));
    std::vector<Vector4f> colorValue;
    std::vector<Vector2f> posValue;
    std::vector<float> strengthValue;

    for (size_t i = 0; i < arraySize; i++) {
        napi_value jsValueColor;
        napi_value jsValuePos;
        napi_value jsValueStrength;
        if ((napi_get_element(env, argValue[NUM_0], i, &jsValueColor)) != napi_ok ||
            (napi_get_element(env, argValue[NUM_1], i, &jsValuePos)) != napi_ok ||
            (napi_get_element(env, argValue[NUM_2], i, &jsValueStrength)) != napi_ok) {
            UIEFFECT_LOG_E("GetColorGradientArray get args fail");
            return false;
        }

        Vector4f color;
        if (!ParseJsRGBAColor(env, jsValueColor, color)) { return false; }
        colorValue.push_back(color);

        Vector2f position;
        if (!ParseJsPoint(env, jsValuePos, position)) { return false; }
        posValue.push_back(position);

        strengthValue.push_back(GetSpecialValue(env, jsValueStrength));
    }

    para->SetColors(colorValue);
    para->SetPositions(posValue);
    para->SetStrengths(strengthValue);

    return true;
}

napi_value EffectNapi::CreateColorGradientEffect(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi CreateColorGradientEffect failed, is not system app");
        return nullptr;
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_4] = {0};
    size_t argCount = NUM_4;
    UIEFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateColorGradientEffect parsing input fail"));
    size_t minCount = static_cast<size_t>(NUM_3);
    size_t maxCount = static_cast<size_t>(NUM_4);
    UIEFFECT_NAPI_CHECK_RET_D(minCount <= argCount && argCount <= maxCount, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateColorGradientEffect Args count error"));
  
    auto para = std::make_shared<ColorGradientEffectPara>();
    uint32_t arraySizeColor = 0;
    uint32_t arraySizePos = 0;
    uint32_t arraySizeStrength = 0;
    if (!IsArrayForNapiValue(env, argValue[NUM_0], arraySizeColor) ||
        !IsArrayForNapiValue(env, argValue[NUM_1], arraySizePos) ||
        !IsArrayForNapiValue(env, argValue[NUM_2], arraySizeStrength)) {
        UIEFFECT_LOG_E("CreateColorGradientEffect get args fail, not array");
        return nullptr;
    }
    if (arraySizeColor != arraySizePos || arraySizeColor != arraySizeStrength || arraySizeStrength > NUM_12) {
        UIEFFECT_LOG_E("CreateColorGradientEffect param Error");
        return nullptr;
    }

    UIEFFECT_NAPI_CHECK_RET_D(GetColorGradientArray(env, argValue, para, arraySizeColor), nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateColorGradientEffect parsing array fail"));

    if (argCount > NUM_3) {
        Mask* mask = nullptr;
        if (napi_unwrap(env, argValue[NUM_3], reinterpret_cast<void**>(&mask)) && mask != nullptr) {
            para->SetMask(mask->GetMaskPara());
        }
    }

    VisualEffect* visualEffectObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&visualEffectObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && visualEffectObj != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateColorGradientEffect napi_unwrap fail"));
    visualEffectObj->AddPara(para);
    return thisVar;
}

napi_value EffectNapi::CreateHarmoniumEffect(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        UIEFFECT_LOG_E("CreateHarmoniumEffect failed, is not system app");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "EffectNapi CreateHarmoniumEffect failed, is not system app");
        return nullptr;
    }
    static const size_t requireArgc = NUM_4;
    size_t realArgc = NUM_4;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value argv[requireArgc] = {0};
    napi_value thisVar = nullptr;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && requireArgc == realArgc, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateHarmoniumEffect parsing input fail"));

    std::shared_ptr<HarmoniumEffectPara> para = std::make_shared<HarmoniumEffectPara>();
    UIEFFECT_NAPI_CHECK_RET_D(ParseHarmoniumEffectPara(env, argv[0], para.get()), nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateHarmoniumEffect fail"));

    Mask* useEffectMask = nullptr;
    status = napi_unwrap(env, argv[NUM_1], reinterpret_cast<void**>(&useEffectMask));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && useEffectMask != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateHarmoniumEffect useEffectMask napi_unwrap fail"));
    para->SetUseEffectMask(useEffectMask->GetMaskPara());

    if (realArgc >= NUM_3) {
        Mask* mask = nullptr;
        if (napi_unwrap(env, argv[NUM_2], reinterpret_cast<void**>(&mask)) == napi_ok && mask != nullptr) {
            para->SetMask(mask->GetMaskPara());
        }
    }

    // parse selectable para
    if (realArgc == NUM_4) {
        if (!ParseHarmoniumSelectablePara(env, argv[NUM_3], para.get())) {
            UIEFFECT_LOG_E("EffectNapi CreateHarmoniumEffect fail");
        }
    }

    VisualEffect* visualEffectObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&visualEffectObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && visualEffectObj != nullptr, nullptr,
        UIEFFECT_LOG_E("EffectNapi CreateHarmoniumEffect napi_unwrap fail"));
    visualEffectObj->AddPara(para);
    return thisVar;
}

}  // namespace Rosen
}  // namespace OHOS

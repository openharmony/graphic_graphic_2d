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
#include "filter_napi.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "ui_effect_napi_utils.h"

namespace OHOS {
namespace Rosen {

using namespace UIEffect;

std::map<int32_t, Drawing::TileMode> INDEX_TO_TILEMODE = {
    { NUM_0, Drawing::TileMode::CLAMP },
    { NUM_1, Drawing::TileMode::REPEAT },
    { NUM_2, Drawing::TileMode::MIRROR },
    { NUM_3, Drawing::TileMode::DECAL },
};

std::map<int32_t, GradientDirection> INDEX_TO_DIRECTION = {
    { NUM_0, GradientDirection::LEFT },
    { NUM_1, GradientDirection::TOP },
    { NUM_2, GradientDirection::RIGHT },
    { NUM_3, GradientDirection::BOTTOM },
    { NUM_4, GradientDirection::LEFT_TOP },
    { NUM_5, GradientDirection::LEFT_BOTTOM },
    { NUM_6, GradientDirection::RIGHT_TOP },
    { NUM_7, GradientDirection::RIGHT_BOTTOM },
    { NUM_8, GradientDirection::NONE },
};

static const std::string CLASS_NAME = "Filter";

FilterNapi::FilterNapi()
{
}

FilterNapi::~FilterNapi()
{
}

thread_local napi_ref FilterNapi::sConstructor_ = nullptr;

napi_value TileModeInit(napi_env env)
{
    UIEFFECT_NAPI_CHECK_RET_D(env != nullptr, nullptr, FILTER_LOG_E("FilterNapi TileModeInit env is nullptr"));
    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, FILTER_LOG_E("FilterNapi TileModeInit fail to get object"));

    for (auto& [tileModeName, tileMode] : STRING_TO_JS_MAP) {
        napi_value value = nullptr;
        status = napi_create_int32(env, static_cast<int32_t>(tileMode), &value);
        UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
            FILTER_LOG_E("FilterNapi TileModeInit fail to create int32"));
        status = napi_set_named_property(env, object, tileModeName.c_str(), value);
        UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
            FILTER_LOG_E("FilterNapi TileModeInit fail to set tileModeName"));
    }
    return object;
}

napi_value FilterNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createFilter", CreateFilter),
    };

    napi_value constructor = nullptr;

    napi_status status = napi_define_class(env, CLASS_NAME.c_str(),
                                           NAPI_AUTO_LENGTH, Constructor,
                                           nullptr,
                                           sizeof(static_prop) / sizeof(static_prop[0]), static_prop,
                                           &constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, FILTER_LOG_E("FilterNapi Init define class fail"));

    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, FILTER_LOG_E("FilterNapi Init create reference fail"));

    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, FILTER_LOG_E("FilterNapi Init get global fail"));

    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        FILTER_LOG_E("FilterNapi Init set global named property fail"));

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, FILTER_LOG_E("FilterNapi Init set named property fail"));

    status = napi_define_properties(env, exports, UIEFFECT_ARRAY_SIZE(static_prop), static_prop);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, FILTER_LOG_E("FilterNapi Init define properties fail"));

    auto tileModeFormat = TileModeInit(env);
    status = napi_set_named_property(env, exports, "TileMode", tileModeFormat);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, FILTER_LOG_E("FilterNapi Init set TileMode fail"));
    return exports;
}

napi_value FilterNapi::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, FILTER_LOG_E("FilterNapi Constructor parsing input fail"));

    FilterNapi *filterNapi = new(std::nothrow) FilterNapi();
    UIEFFECT_NAPI_CHECK_RET_D(filterNapi != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi Constructor filterNapi is nullptr"));
    status = napi_wrap(env, jsThis, filterNapi, FilterNapi::Destructor, nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, filterNapi,
        FILTER_LOG_E("FilterNapi Constructor wrap fail"));
    return jsThis;
}

void FilterNapi::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    FilterNapi *filterNapi = static_cast<FilterNapi*>(nativeObject);
    UIEFFECT_NAPI_CHECK_RET_VOID_D(filterNapi != nullptr,
        FILTER_LOG_E("FilterNapi Destructor nativeObject is nullptr"));

    delete filterNapi;
    filterNapi = nullptr;
}

napi_value FilterNapi::CreateFilter(napi_env env, napi_callback_info info)
{
    Filter* filterObj = new(std::nothrow) Filter();
    UIEFFECT_NAPI_CHECK_RET_D(filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi CreateFilter filterObj is nullptr"));

    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, filterObj,
        FILTER_LOG_E("FilterNapi CreateFilter create object fail"));
    status = napi_wrap(
        env, object, filterObj,
        [](napi_env env, void* data, void* hint) {
            Filter* filterObj = (Filter*)data;
            delete filterObj;
            filterObj = nullptr;
        },
        nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, filterObj,
        FILTER_LOG_E("FilterNapi CreateFilter wrap fail"));
    napi_property_descriptor resultFuncs[] = {
        DECLARE_NAPI_FUNCTION("blur", SetBlur),
        DECLARE_NAPI_FUNCTION("pixelStretch", SetPixelStretch),
        DECLARE_NAPI_FUNCTION("waterRipple", SetWaterRipple),
        DECLARE_NAPI_FUNCTION("flyInFlyOutEffect", SetFlyOut),
        DECLARE_NAPI_FUNCTION("colorGradient", SetColorGradient),
        DECLARE_NAPI_FUNCTION("distort", SetDistort),
        DECLARE_NAPI_FUNCTION("radiusGradientBlur", SetRadiusGradientBlurPara),
        DECLARE_NAPI_FUNCTION("displacementDistort", SetDisplacementDistort),
        DECLARE_NAPI_FUNCTION("edgeLight", SetEdgeLight),
        DECLARE_NAPI_FUNCTION("bezierWarp", SetBezierWarp),
        DECLARE_NAPI_FUNCTION("maskDispersion", SetMaskDispersion),
        DECLARE_NAPI_FUNCTION("hdrBrightnessRatio", SetHDRBrightnessRatio),
        DECLARE_NAPI_FUNCTION("contentLight", SetContentLight)
    };
    status = napi_define_properties(env, object, sizeof(resultFuncs) / sizeof(resultFuncs[0]), resultFuncs);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, filterObj,
        FILTER_LOG_E("FilterNapi CreateFilter define properties fail"));
    return object;
}

napi_value FilterNapi::SetBlur(napi_env env, napi_callback_info info)
{
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value argv[NUM_1];
    napi_value _this;
    napi_status status;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, _this);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        FILTER_LOG_E("FilterNapi SetBlur parsing input fail"));

    float radius = 0.0f;
    if (UIEffectNapiUtils::GetType(env, argv[NUM_0]) == napi_number) {
        double tmp = 0.0f;
        if (napi_get_value_double(env, argv[NUM_0], &tmp) == napi_ok) {
            radius = static_cast<float>(tmp);
        }
    }
    Filter* filterObj = nullptr;
    status = napi_unwrap(env, _this, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetBlur napi_unwrap fail"));
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr, FILTER_LOG_E("FilterNapi SetBlur para is nullptr"));
    para->SetRadius(radius);
    filterObj->AddPara(para);

    return _this;
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

static bool GetStretchPercent(napi_env env, napi_value param, std::shared_ptr<PixelStretchPara>& para)
{
    napi_valuetype valueType = napi_undefined;
    valueType = UIEffectNapiUtils::GetType(env, param);
    if (valueType == napi_undefined) {
        return true;
    }
    uint32_t arraySize = 0;
    if (!IsArrayForNapiValue(env, param, arraySize)) {
        FILTER_LOG_E("GetStretchPercent get args fail, not array");
        return false;
    }
    if (arraySize < NUM_4) {
        FILTER_LOG_E("GetStretchPercent coordinates num less than 4");
        return false;
    }
    Vector4f tmpPercent_{ 0.f, 0.f, 0.f, 0.f };
    for (size_t i = 0; i < NUM_4; i++) {
        napi_value jsValue;
        if ((napi_get_element(env, param, i, &jsValue)) != napi_ok) {
            FILTER_LOG_E("GetStretchPercent get args fail");
            return false;
        }
        double value = 0.0;
        if (napi_get_value_double(env, jsValue, &value) == napi_ok) {
            tmpPercent_[i] = value;
        } else {
            FILTER_LOG_E("GetStretchPercent region coordinates not double");
            return false;
        }
    }
    para->SetStretchPercent(tmpPercent_);
    return true;
}

napi_value FilterNapi::SetPixelStretch(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetPixelStretch failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetPixelStretch failed, is not system app");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_3] = {0};
    size_t argCount = NUM_3;
    UIEFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        FILTER_LOG_E("FilterNapi SetPixelStretch parsing input fail"));

    Drawing::TileMode tileMode = Drawing::TileMode::CLAMP;
    std::shared_ptr<PixelStretchPara> para = std::make_shared<PixelStretchPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetPixelStretch para is nullptr"));

    if (argCount >= NUM_1) {
        UIEFFECT_NAPI_CHECK_RET_D(GetStretchPercent(env, argValue[NUM_0], para), nullptr,
            FILTER_LOG_E("FilterNapi SetPixelStretch parsing coordinates fail"));
    }
    if (argCount >= NUM_2) {
        tileMode = ParserArgumentType(env, argValue[NUM_1]);
    }
    para->SetTileMode(tileMode);
    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetPixelStretch napi_unwrap fail"));
    filterObj->AddPara(para);

    return thisVar;
}

static bool GetBezierControlPoints(napi_env env, napi_value param, std::shared_ptr<BezierWarpPara>& para)
{
    uint32_t arraySize = 0;
    if (!IsArrayForNapiValue(env, param, arraySize)) {
        FILTER_LOG_E("GetBezierControlPoints get args fail, not array");
        return false;
    }
    if (arraySize != NUM_12) {
        FILTER_LOG_E("GetBezierControlPoints coordinates num is not 12");
        return false;
    }

    std::array<Vector2f, NUM_12> bezierPoints;
    /* Fill array with data from input array*/
    for (size_t i = 0; i < NUM_12; ++i) {
        napi_value tempPoint = nullptr;
        if (napi_get_element(env, param, i, &tempPoint) != napi_ok) {
            FILTER_LOG_E("GetBezierControlPoints GetPointfromArray is wrong");
            return false;
        }

        if (!ParseJsPoint(env, tempPoint, bezierPoints[i])) {
            FILTER_LOG_E("GetBezierControlPoints ParseJsPoint is wrong");
            return false;
        }
    }
    para->SetBezierControlPoints(bezierPoints);
    return true;
}

napi_value FilterNapi::SetBezierWarp(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetBezierWarp failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetBezierWarp failed, is not system app");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    size_t realArgc = NUM_1;
    const size_t requireArgc = NUM_1;
    napi_value argv[NUM_1] = {0};
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        FILTER_LOG_E("FilterNapi SetBezierWarp parsing input fail"));

    std::shared_ptr<BezierWarpPara> para = std::make_shared<BezierWarpPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr, FILTER_LOG_E("FilterNapi SetBezierWarp para is nullptr"));

    UIEFFECT_NAPI_CHECK_RET_D(GetBezierControlPoints(env, argv[NUM_0], para), nullptr,
        FILTER_LOG_E("FilterNapi SetBezierWarp coordinates fail"));

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetBezierWarp napi_unwrap fail"));
    filterObj->AddPara(para);
    return thisVar;
}

static bool GetContentLight(napi_env env, napi_value* param, size_t length, std::shared_ptr<ContentLightPara>& para)
{
    if (length < NUM_2) {
        UIEFFECT_LOG_E("FilterNapi GetContentLight array length is less than 2");
        return false;
    }

    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    Vector4f lightColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

    if (!ParseJsVector3f(env, param[NUM_0], lightPosition)) {
        FILTER_LOG_E("FilterNapi GetContentLight parse lightPosition fail");
        return false;
    }
    para->SetLightPosition(lightPosition);

    if (!ParseJsRGBAColor(env, param[NUM_1], lightColor)) {
        FILTER_LOG_E("FilterNapi GetContentColor parse lightColor fail");
        return false;
    }
    para->SetLightColor(lightColor);
    return true;
}

napi_value FilterNapi::SetContentLight(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetContentLight failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi contentLight failed, is not system app");
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
        FILTER_LOG_E("FilterNapi SetContentLight parsing input fail"));

    std::shared_ptr<ContentLightPara> para = std::make_shared<ContentLightPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetContentLight para is nullptr"));

    if (argCount != NUM_4) {
        FILTER_LOG_E("Args number less than 4");
        return thisVar;
    }

    UIEFFECT_NAPI_CHECK_RET_D(GetContentLight(env, argValue, argCount, para), nullptr,
        FILTER_LOG_E("FilterNapi GetContentLight fail"));

    float lightIntensity = 0.f;
    lightIntensity = GetSpecialValue(env, argValue[NUM_2]);
    para->SetLightIntensity(lightIntensity);
    
    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetContentLight napi_unwrap fail"));
    filterObj->AddPara(para);

    return thisVar;
}

GradientDirection FilterNapi::ParserGradientDirection(napi_env env, napi_value argv)
{
    if (UIEffectNapiUtils::GetType(env, argv) == napi_number) {
        int32_t direction = 0;
        if (napi_get_value_int32(env, argv, &direction) == napi_ok) {
            auto iter = INDEX_TO_DIRECTION.find(direction);
            if (iter != INDEX_TO_DIRECTION.end()) {
                return iter->second;
            }
        }
    }
    return GradientDirection::NONE;
}

static bool GetLinearFractionStops(napi_env env, napi_value param, std::shared_ptr<RadiusGradientBlurPara>& para)
{
    uint32_t arraySize = 0;
    if (!IsArrayForNapiValue(env, param, arraySize)) {
        FILTER_LOG_E("GetLinearFractionStops get args fail, not array");
        return false;
    }
    if (arraySize < NUM_2 || arraySize > NUM_1000) {
        FILTER_LOG_E("GetLinearFractionStops fractionStops num less than 2 or greater than 1000");
        return false;
    }

    std::vector<std::pair<float, float>> tmpPercent;
    float lastPos = 0.0f;
    for (size_t i = 0; i < arraySize; i++) {
        napi_value jsValue;
        if ((napi_get_element(env, param, i, &jsValue)) != napi_ok) {
            FILTER_LOG_E("GetLinearFractionStops get args fail");
            return false;
        }
        napi_value napiBlurPercent;
        napi_value napiBlurPosition;
        if (napi_get_element(env, jsValue, 0, &napiBlurPercent) != napi_ok ||
            napi_get_element(env, jsValue, 1, &napiBlurPosition) != napi_ok) {
            FILTER_LOG_E("GetLinearFractionStops get args fail");
            return false;
        }
        double blurPercent = 0.0;
        double blurPosition = 0.0;
        if (napi_get_value_double(env, napiBlurPercent, &blurPercent) != napi_ok ||
            napi_get_value_double(env, napiBlurPosition, &blurPosition) != napi_ok) {
            FILTER_LOG_E("GetLinearFractionStops region coordinates not double");
            return false;
        }
        if (blurPosition < lastPos) {
            FILTER_LOG_E("GetLinearFractionStops is not increasing");
            return false;
        } else {
            lastPos = blurPosition;
        }
        tmpPercent.push_back(std::pair(blurPercent, blurPosition));
    }
    para->SetFractionStops(tmpPercent);
    return true;
}

napi_value FilterNapi::SetRadiusGradientBlurPara(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetRadiusGradientBlurPara failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetRadiusGradientBlurPara failed, is not system app");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;

    napi_value argValue[NUM_2] = {0};
    size_t argCount = NUM_2;
    UIEFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        FILTER_LOG_E("FilterNapi SetRadiusGradientBlurPara parsing input fail"));

    std::shared_ptr<RadiusGradientBlurPara> para = std::make_shared<RadiusGradientBlurPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetRadiusGradientBlurPara para is nullptr"));

    if (argCount != NUM_2) {
        FILTER_LOG_E("Args number less than 2");
    }

    float blurRadius = GetSpecialValue(env, argValue[NUM_0]);
    para->SetBlurRadius(blurRadius);

    napi_value fractionValue;
    napi_get_named_property(env, argValue[NUM_1], "fractionStops", &fractionValue);
    UIEFFECT_NAPI_CHECK_RET_D(GetLinearFractionStops(env, fractionValue, para), nullptr,
        FILTER_LOG_E("FilterNapi SetRadiusGradientBlurPara parsing coordinates fail"));

    GradientDirection direction = GradientDirection::NONE;
    napi_value directionValue;
    napi_get_named_property(env, argValue[NUM_1], "direction", &directionValue);
    direction = ParserGradientDirection(env, directionValue);
    para->SetDirection(direction);

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetRadiusGradientBlurPara napi_unwrap fail"));
    filterObj->AddPara(para);

    return thisVar;
}


float FilterNapi::GetSpecialValue(napi_env env, napi_value argValue)
{
    double tmp = 0.0f;
    if (UIEffectNapiUtils::GetType(env, argValue) == napi_number &&
        napi_get_value_double(env, argValue, &tmp) == napi_ok && tmp >= 0) {
            return static_cast<float>(tmp);
    }
    return tmp;
}

uint32_t FilterNapi::GetSpecialIntValue(napi_env env, napi_value argValue)
{
    uint32_t tmp = 0;
    if (UIEffectNapiUtils::GetType(env, argValue) == napi_number &&
        napi_get_value_uint32(env, argValue, &tmp) == napi_ok) {
            return tmp;
    }
    return tmp;
}

bool FilterNapi::GetSpecialBoolValue(napi_env env, napi_value argValue, bool defaultValue)
{
    bool tmp = defaultValue;
    if (UIEffectNapiUtils::GetType(env, argValue) == napi_boolean &&
        napi_get_value_bool(env, argValue, &tmp) == napi_ok) {
            return tmp;
    }
    return tmp;
}

napi_value FilterNapi::SetWaterRipple(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetWaterRipple failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetWaterRipple failed, is not system app");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_5] = {0};
    size_t argCount = NUM_5;
    UIEFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        FILTER_LOG_E("FilterNapi SetWaterRipple parsing input fail"));

    std::shared_ptr<WaterRipplePara> para = std::make_shared<WaterRipplePara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetWaterRipple para is nullptr"));

    float progress = 0.0f;
    uint32_t waveCount = 0;
    float rippleCenterX = 0.0f;
    float rippleCenterY = 0.0f;
    uint32_t rippleMode = 0;

    if (argCount != NUM_5) {
        FILTER_LOG_E("Args number less than 5");
        return thisVar;
    }

    progress = GetSpecialValue(env, argValue[NUM_0]);
    waveCount = GetSpecialIntValue(env, argValue[NUM_1]);
    rippleCenterX = GetSpecialValue(env, argValue[NUM_2]);
    rippleCenterY = GetSpecialValue(env, argValue[NUM_3]);
    rippleMode = GetSpecialIntValue(env, argValue[NUM_4]);

    para->SetProgress(progress);
    para->SetWaveCount(waveCount);
    para->SetRippleCenterX(rippleCenterX);
    para->SetRippleCenterY(rippleCenterY);
    para->SetRippleMode(rippleMode);

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetWaterRipple napi_unwrap fail"));
    filterObj->AddPara(para);

    return thisVar;
}

napi_value FilterNapi::SetFlyOut(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetFlyOut failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetFlyOut failed, is not system app");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_2] = {0};
    size_t argCount = NUM_2;
    UIEFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        FILTER_LOG_E("FilterNapi SetFlyOut parsing input fail"));

    std::shared_ptr<FlyOutPara> para = std::make_shared<FlyOutPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr, FILTER_LOG_E("FilterNapi SetFlyOut para is nullptr"));

    if (argCount != NUM_2) {
        FILTER_LOG_E("Args number less than 2");
        return thisVar;
    }

    float degree = GetSpecialValue(env, argValue[NUM_0]);
    uint32_t flyMode = GetSpecialIntValue(env, argValue[NUM_1]);

    para->SetDegree(degree);
    para->SetFlyMode(flyMode);

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetWaterRipple napi_unwrap fail"));
    filterObj->AddPara(para);

    return thisVar;
}

bool FilterNapi::GetColorGradientArray(napi_env env, napi_value* argValue, std::shared_ptr<ColorGradientPara>& para,
    uint32_t arraySize)
{
    std::vector<float> colorValue;
    std::vector<float> posValue;
    std::vector<float> strengthValue;

    for (size_t i = 0; i < arraySize; i++) {
        napi_value jsValueColor;
        napi_value jsValuePos;
        napi_value jsValueStrength;
        if ((napi_get_element(env, argValue[NUM_0], i, &jsValueColor)) != napi_ok ||
            (napi_get_element(env, argValue[NUM_1], i, &jsValuePos)) != napi_ok ||
            (napi_get_element(env, argValue[NUM_2], i, &jsValueStrength)) != napi_ok) {
            FILTER_LOG_E("GetColorGradientArray get args fail");
            return false;
        }

        Vector4f color;
        if (!ParseJsRGBAColor(env, jsValueColor, color)) { return false; }
        colorValue.push_back(color[0]);
        colorValue.push_back(color[1]);
        colorValue.push_back(color[2]); // 2 element of color
        colorValue.push_back(color[3]); // 3 element of color

        double position[NUM_2] = { 0.0 };
        if (!ConvertFromJsPoint(env, jsValuePos, position, NUM_2)) { return false; }
        posValue.push_back(static_cast<float>(position[0]));
        posValue.push_back(static_cast<float>(position[1]));

        strengthValue.push_back(GetSpecialValue(env, jsValueStrength));
    }

    para->SetColors(colorValue);
    para->SetPositions(posValue);
    para->SetStrengths(strengthValue);

    return true;
}

napi_value FilterNapi::SetColorGradient(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetColorGradient failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi colorGradient failed, is not system app");
        return nullptr;
    }

    napi_status status;
    napi_value thisVar = nullptr;

    const size_t maxArgc = NUM_4;
    const size_t minArgc = NUM_3;
    size_t realArgc = NUM_4;
    napi_value argValue[maxArgc] = {0};
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argValue, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc <= maxArgc && realArgc >= minArgc, nullptr,
        FILTER_LOG_E("FilterNapi SetColorGradient parsing input fail"));

    uint32_t arraySizeColor = 0;
    uint32_t arraySizePos = 0;
    uint32_t arraySizeStrength = 0;
    if (!IsArrayForNapiValue(env, argValue[NUM_0], arraySizeColor) ||
        !IsArrayForNapiValue(env, argValue[NUM_1], arraySizePos) ||
        !IsArrayForNapiValue(env, argValue[NUM_2], arraySizeStrength)) {
        FILTER_LOG_E("SetColorGradient get args fail, not array");
        return nullptr;
    }
    if (arraySizeColor != arraySizePos || arraySizeColor != arraySizeStrength ||
        arraySizeStrength < NUM_0 || arraySizeStrength > NUM_12) {
        FILTER_LOG_E("SetColorGradient param Error");
        return nullptr;
    }

    std::shared_ptr<ColorGradientPara> para = std::make_shared<ColorGradientPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetColorGradient para is nullptr"));
    UIEFFECT_NAPI_CHECK_RET_D(GetColorGradientArray(env, argValue, para, arraySizeColor), nullptr,
        FILTER_LOG_E("FilterNapi SetColorGradient parsing array fail"));

    if (realArgc == NUM_4) {
        Mask* mask = nullptr;
        status = napi_unwrap(env, argValue[NUM_3], reinterpret_cast<void**>(&mask));
        UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && mask != nullptr, nullptr,
            FILTER_LOG_E("FilterNapi SetColorGradient napi_unwrap mask fail"));
        para->SetMask(mask->GetMaskPara());
    }

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetColorGradient napi_unwrap fail"));
    filterObj->AddPara(para);

    return thisVar;
}

napi_value FilterNapi::SetDistort(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetDistort failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetDistort failed, is not system app");
        return nullptr;
    }
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value argv[NUM_1] = {0};
    napi_value thisVar = nullptr;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        FILTER_LOG_E("FilterNapi SetDistort parsing input fail"));
    float distortionK = 0.0f;
    if (UIEffectNapiUtils::GetType(env, argv[NUM_0]) == napi_number) {
        double tmp = 0.0f;
        if (napi_get_value_double(env, argv[NUM_0], &tmp) == napi_ok) {
            distortionK = static_cast<float>(tmp);
        }
    }
    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetDistort napi_unwrap fail"));
    std::shared_ptr<DistortPara> para = std::make_shared<DistortPara>();
    UIEFFECT_NAPI_CHECK_RET_D(para != nullptr, nullptr, FILTER_LOG_E("FilterNapi SetDistort para is nullptr"));
    para->SetDistortionK(distortionK);
    filterObj->AddPara(para);
    return thisVar;
}

napi_value FilterNapi::SetDisplacementDistort(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetDisplacementDistort failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetDisplacementDistort failed, is not system app");
        return nullptr;
    }
    const size_t requireArgc = NUM_2;
    const size_t minArgc = NUM_1;
    size_t realArgc = NUM_2;
    napi_status status;
    napi_value argv[requireArgc] = {0};
    napi_value thisVar = nullptr;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc <= requireArgc && realArgc >= minArgc, nullptr,
        FILTER_LOG_E("FilterNapi SetDisplacementDistort parsing input fail"));

    auto para = std::make_shared<DisplacementDistortPara>();
    Mask* mask = nullptr;
    status = napi_unwrap(env, argv[NUM_0], reinterpret_cast<void**>(&mask));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && mask != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetDisplacementDistort unwrap mask fail"));
    para->SetMask(mask->GetMaskPara());

    if (realArgc == NUM_2) {
        Vector2f factor;
        UIEFFECT_NAPI_CHECK_RET_D(ParseJsVector2f(env, argv[NUM_1], factor), nullptr,
            FILTER_LOG_E("FilterNapi SetDisplacementDistort parse factor fail"));
        para->SetFactor(factor);
    }

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetDisplacementDistort unwrap filterObj fail"));
    filterObj->AddPara(para);
    return thisVar;
}

napi_value FilterNapi::SetEdgeLight(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetEdgeLight failed, is not system app");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetEdgeLight failed, is not system app");
        return nullptr;
    }
    static const size_t maxArgc = NUM_4;
    static const size_t minArgc = NUM_1;
    size_t realArgc = maxArgc;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value argv[maxArgc] = {0};
    napi_value thisVar = nullptr;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && minArgc <= realArgc && realArgc <= maxArgc, nullptr,
        FILTER_LOG_E("FilterNapi SetEdgeLight parsing input fail"));

    auto para = std::make_shared<EdgeLightPara>();
    float alpha = GetSpecialValue(env, argv[NUM_0]);
    para->SetAlpha(std::clamp(alpha, 0.f, 1.f));

    Vector4f color;
    if (realArgc >= NUM_2 && ParseJsRGBAColor(env, argv[NUM_1], color)) {
        para->SetColor(color);
    }

    Mask* mask = nullptr;
    if (realArgc >= NUM_3 &&
        napi_unwrap(env, argv[NUM_2], reinterpret_cast<void**>(&mask)) == napi_ok) {
        para->SetMask(mask->GetMaskPara());
    }

    if (realArgc >= NUM_4) {
        bool bloom = GetSpecialBoolValue(env, argv[NUM_3], true);
        para->SetBloom(bloom);
    }

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetEdgeLight unwrap filterObj fail"));
    filterObj->AddPara(para);
    return thisVar;
}

napi_value FilterNapi::SetMaskDispersion(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("SetMaskDispersion failed, is not system app");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "FilterNapi SetMaskDispersion failed, is not system app");
        return nullptr;
    }
    static const size_t requireArgc = NUM_5;
    size_t realArgc = NUM_5;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value argv[requireArgc] = {0};
    napi_value thisVar = nullptr;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && requireArgc == realArgc, nullptr,
        FILTER_LOG_E("FilterNapi SetMaskDispersion parsing input fail"));

    auto para = std::make_shared<DispersionPara>();

    Mask* mask = nullptr;
    status = napi_unwrap(env, argv[NUM_0], reinterpret_cast<void**>(&mask));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && mask != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetMaskDispersion unwrap mask fail"));
    para->SetMask(mask->GetMaskPara());

    float opacity = 0.0f;
    if (UIEffectNapiUtils::GetType(env, argv[NUM_1]) == napi_number) {
        double tmp = 0.0f;
        if (napi_get_value_double(env, argv[NUM_1], &tmp) == napi_ok) {
            opacity = static_cast<float>(tmp);
        }
    }
    para->SetOpacity(opacity);

    Vector2f redOffset;
    UIEFFECT_NAPI_CHECK_RET_D(ParseJsVector2f(env, argv[NUM_2], redOffset), nullptr,
        FILTER_LOG_E("FilterNapi SetMaskDispersion parse redOffset fail"));
    para->SetRedOffset(redOffset);

    Vector2f greenOffset;
    UIEFFECT_NAPI_CHECK_RET_D(ParseJsVector2f(env, argv[NUM_2], greenOffset), nullptr,
        FILTER_LOG_E("FilterNapi SetMaskDispersion parse greenOffset fail"));
    para->SetRedOffset(greenOffset);

    Vector2f blueOffset;
    UIEFFECT_NAPI_CHECK_RET_D(ParseJsVector2f(env, argv[NUM_2], blueOffset), nullptr,
        FILTER_LOG_E("FilterNapi SetMaskDispersion parse blueOffset fail"));
    para->SetBlueOffset(blueOffset);

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && filterObj != nullptr, nullptr,
        FILTER_LOG_E("FilterNapi SetMaskDispersion unwrap filterObj fail"));
    filterObj->AddPara(para);
    return thisVar;
}

napi_value FilterNapi::SetHDRBrightnessRatio(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        FILTER_LOG_E("FilterNapi SetHDRBrightnessRatio not system app");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "The SetHDRBrightnessRatio is only accessible to system applications.");
        return nullptr;
    }
    static const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value argv[requireArgc] = {0};
    napi_value thisVar = nullptr;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && requireArgc == realArgc, nullptr,
        FILTER_LOG_E("FilterNapi SetHDRBrightnessRatio parsing input fail"));

    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && valueType == napi_number, nullptr,
        FILTER_LOG_E("FilterNapi SetHDRBrightnessRatio input is not number"));

    double brightnessRatio = 1.0;
    status = napi_get_value_double(env, argv[0], &brightnessRatio);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        FILTER_LOG_E("FilterNapi SetHDRBrightnessRatio parsing double fail"));

    if (std::isnan(brightnessRatio)) {
        FILTER_LOG_E("FilterNapi SetHDRBrightnessRatio brightnessRatio is nan");
        brightnessRatio = 1.0f;
    }

    Filter* filterObj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj));
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        FILTER_LOG_E("FilterNapi SetHDRBrightnessRatio filterObj is nullptr"));

    std::shared_ptr<HDRBrightnessRatioPara> para = std::make_shared<HDRBrightnessRatioPara>();
    para->SetBrightnessRatio(static_cast<float>(brightnessRatio));
    filterObj->AddPara(para);
    return thisVar;
}

Drawing::TileMode FilterNapi::ParserArgumentType(napi_env env, napi_value argv)
{
    if (UIEffectNapiUtils::GetType(env, argv) == napi_number) {
        double tmp = 0.0f;
        if (napi_get_value_double(env, argv, &tmp) == napi_ok) {
            int32_t mode = static_cast<int32_t>(tmp);
            auto iter = INDEX_TO_TILEMODE.find(mode);
            if (iter != INDEX_TO_TILEMODE.end()) {
                return iter->second;
            }
        }
    }

    return Drawing::TileMode::CLAMP;
}

}  // namespace Rosen
}  // namespace OHOS

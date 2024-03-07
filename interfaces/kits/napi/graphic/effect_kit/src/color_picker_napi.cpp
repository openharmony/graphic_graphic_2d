/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "image_napi_utils.h"
#include "effect_errors.h"
#include "image_napi_utils.h"
#include "color_picker.h"
#include "color.h"
#include "pixel_map_napi.h"
#include "hilog/log.h"
#include "effect_utils.h"
#include "color_picker_napi.h"

using OHOS::HiviewDFX::HiLog;
namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
    constexpr uint32_t NUM_3 = 3;
    constexpr uint32_t NUM_4 = 4;
}

namespace OHOS {
namespace Rosen {
static const std::string CLASS_NAME = "ColorPicker";
thread_local napi_ref ColorPickerNapi::sConstructor_ = nullptr;
std::shared_ptr<ColorPicker> ColorPickerNapi::sColorPicker_ = nullptr;

// context
struct ColorPickerAsyncContext {
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    uint32_t status;
    // build error msg
    napi_value errorMsg = nullptr;
    ColorPickerNapi *nConstructor;
    std::shared_ptr<ColorPicker> rColorPicker;
    std::shared_ptr<Media::PixelMap> rPixelMap;
    ColorManager::Color color;
    bool regionFlag = false;
    double coordinatesBuffer[4];
};

static void BuildMsgOnError(napi_env env,
                            const std::unique_ptr<ColorPickerAsyncContext>& context,
                            bool assertion,
                            const std::string& msg);

static napi_value BuildJsColor(napi_env env, ColorManager::Color color);

static void CommonCallbackRoutine(napi_env env, ColorPickerAsyncContext* &asyncContext, const napi_value &valueParam)
{
    napi_value result[NUM_2] = {0};
    napi_value retVal;
    napi_value callback = nullptr;

    napi_get_undefined(env, &result[NUM_0]);
    napi_get_undefined(env, &result[NUM_1]);

    if (asyncContext->status == SUCCESS) {
        result[NUM_1] = valueParam;
    } else if (asyncContext->errorMsg != nullptr) {
        result[NUM_0] = asyncContext->errorMsg;
    } else {
        napi_create_string_utf8(env, "Internal error", NAPI_AUTO_LENGTH, &(result[NUM_0]));
    }

    if (asyncContext->deferred) {
        if (asyncContext->status == SUCCESS) {
            napi_resolve_deferred(env, asyncContext->deferred, result[NUM_1]);
        } else {
            napi_reject_deferred(env, asyncContext->deferred, result[NUM_0]);
        }
    } else {
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, NUM_2, result, &retVal);
        napi_delete_reference(env, asyncContext->callbackRef);
    }

    napi_delete_async_work(env, asyncContext->work);

    delete asyncContext;
    asyncContext = nullptr;
}

ColorPickerNapi::ColorPickerNapi()
    :env_(nullptr), wrapper_(nullptr)
{
}

ColorPickerNapi::~ColorPickerNapi()
{
    if (nativeColorPicker_ != nullptr) {
        nativeColorPicker_ = nullptr;
    }

    if (wrapper_ != nullptr) {
        napi_delete_reference(env_, wrapper_);
    }
}

napi_value ColorPickerNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor props[] = {
        DECLARE_NAPI_FUNCTION("getMainColor", GetMainColor),
        DECLARE_NAPI_FUNCTION("getMainColorSync", GetMainColorSync),
        DECLARE_NAPI_FUNCTION("getLargestProportionColor", GetLargestProportionColor),
        DECLARE_NAPI_FUNCTION("getHighestSaturationColor", GetHighestSaturationColor),
        DECLARE_NAPI_FUNCTION("getAverageColor", GetAverageColor),
        DECLARE_NAPI_FUNCTION("isBlackOrWhiteOrGrayColor", IsBlackOrWhiteOrGrayColor),
    };

    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createColorPicker", CreateColorPicker),
    };

    napi_value constructor = nullptr;

    napi_status status = napi_define_class(env, CLASS_NAME.c_str(),
                                           NAPI_AUTO_LENGTH, Constructor,
                                           nullptr,
                                           IMG_ARRAY_SIZE(props), props,
                                           &constructor);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, EFFECT_LOG_E("define class fail"));

    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, EFFECT_LOG_E("create reference fail"));

    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, EFFECT_LOG_E("Init:get global fail"));

    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, EFFECT_LOG_E("Init:set global named property fail"));

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, EFFECT_LOG_E("set named property fail"));

    status = napi_define_properties(env, exports, IMG_ARRAY_SIZE(static_prop), static_prop);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, EFFECT_LOG_E("define properties fail"));

    EFFECT_LOG_I("Init success");
    return exports;
}

napi_value ColorPickerNapi::Constructor(napi_env env, napi_callback_info info)
{
    napi_value undefineVar = nullptr;
    napi_get_undefined(env, &undefineVar);

    napi_status status;
    napi_value thisVar = nullptr;
    napi_get_undefined(env, &thisVar);

    IMG_JS_NO_ARGS(env, info, status, thisVar);

    IMG_NAPI_CHECK_RET(IMG_IS_READY(status, thisVar), undefineVar);
    std::unique_ptr<ColorPickerNapi> pColorPickerNapi = std::make_unique<ColorPickerNapi>();

    IMG_NAPI_CHECK_RET(IMG_NOT_NULL(pColorPickerNapi), undefineVar);

    pColorPickerNapi->env_ = env;
    pColorPickerNapi->nativeColorPicker_ = sColorPicker_;

    status = napi_wrap(env, thisVar,
                       reinterpret_cast<void*>(pColorPickerNapi.get()),
                       ColorPickerNapi::Destructor,
                       nullptr,
                       nullptr);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         undefineVar,
                         EFFECT_LOG_E("Failure wrapping js to native napi"));

    pColorPickerNapi.release();
    sColorPicker_ = nullptr;

    return thisVar;
}

void ColorPickerNapi::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    ColorPickerNapi *pColorPickerNapi = reinterpret_cast<ColorPickerNapi*>(nativeObject);

    if (IMG_NOT_NULL(pColorPickerNapi)) {
        pColorPickerNapi->~ColorPickerNapi();
    }
}

static void CreateColorPickerFromPixelmapExecute(napi_env env, void* data)
{
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    if (context->regionFlag) {
        context->rColorPicker = ColorPicker::CreateColorPicker(
            context->rPixelMap, context->coordinatesBuffer, errorCode);
    } else {
        context->rColorPicker = ColorPicker::CreateColorPicker(context->rPixelMap, errorCode);
    }
    context->status = ERROR;
    if (IMG_NOT_NULL(context->rColorPicker) && errorCode == SUCCESS) {
        context->status = SUCCESS;
    }
}

void ColorPickerNapi::CreateColorPickerFromPixelmapComplete(napi_env env, napi_status status, void* data)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;

    auto context = static_cast<ColorPickerAsyncContext*>(data);
    status = napi_get_reference_value(env, sConstructor_, &constructor);
    if (IMG_IS_OK(status)) {
        sColorPicker_ = context->rColorPicker;
        status = napi_new_instance(env, constructor, NUM_0, nullptr, &result);
    }

    if (!IMG_IS_OK(status)) {
        context->status = ERROR;
        EFFECT_LOG_E("New instance could not be obtained");
        napi_get_undefined(env, &result);
    }

    CommonCallbackRoutine(env, context, result);
}

static void CreateColorPickerErrorComplete(napi_env env, napi_status status, void* data)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    context->status = ERROR;
    CommonCallbackRoutine(env, context, result);
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

static bool GetRegionCoordinates(napi_env env, napi_value param, std::unique_ptr<ColorPickerAsyncContext>& asyncContext)
{
    napi_valuetype valueType = napi_undefined;
    valueType = Media::ImageNapiUtils::getType(env, param);
    if (valueType == napi_undefined) {
        asyncContext->coordinatesBuffer[NUM_0] = 0.0;
        asyncContext->coordinatesBuffer[NUM_1] = 0.0;
        asyncContext->coordinatesBuffer[NUM_2] = 1.0;
        asyncContext->coordinatesBuffer[NUM_3] = 1.0;
        return true;
    }
    uint32_t arraySize = 0;
    if (!IsArrayForNapiValue(env, param, arraySize)) {
        EFFECT_LOG_E("GetRegionCoordinates get args fail, not array");
        return false;
    }
    if (arraySize < NUM_4) {
        EFFECT_LOG_E("GetRegionCoordinates coordinates num less than 4");
        return false;
    }
    for (size_t i = 0; i < NUM_4; i++) {
        napi_value jsValue;
        if ((napi_get_element(env, param, i, &jsValue)) != napi_ok) {
            EFFECT_LOG_E("GetRegionCoordinates get args fail");
            return false;
        }
        double value = 0.0;
        if (napi_get_value_double(env, jsValue, &value) == napi_ok) {
            asyncContext->coordinatesBuffer[i] = std::clamp<double>(value, 0.0, 1.0);
        } else {
            EFFECT_LOG_E("GetRegionCoordinates region coordinates not double");
            return false;
        }
    }
    if ((asyncContext->coordinatesBuffer[NUM_2] <= asyncContext->coordinatesBuffer[NUM_0]) ||
                (asyncContext->coordinatesBuffer[NUM_3] <= asyncContext->coordinatesBuffer[NUM_1])) {
        EFFECT_LOG_E("GetRegionCoordinates right must be greater than left, bottom must be greater than top");
        return false;
    };
    return true;
}

napi_value ColorPickerNapi::CreateColorPicker(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    int32_t refCount = 1;
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_4] = {0};
    size_t argCount = NUM_4;
    ImageType imgType = ImageType::TYPE_UNKOWN;
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, EFFECT_LOG_E("fail to napi_get_cb_info"));
    std::unique_ptr<ColorPickerAsyncContext> asyncContext = std::make_unique<ColorPickerAsyncContext>();
    if (argCount >= NUM_1) {
        imgType = ParserArgumentType(env, argValue[NUM_1 - 1]);
        if (imgType == ImageType::TYPE_PIXEL_MAP) {
            asyncContext->rPixelMap = Media::PixelMapNapi::GetPixelMap(env, argValue[NUM_1 - 1]);
            BuildMsgOnError(env, asyncContext, IMG_NOT_NULL(asyncContext->rPixelMap), "Pixmap mismatch");
        } else {
            BuildMsgOnError(env, asyncContext, false, "image type mismatch");
        }
    }
    IMG_NAPI_CHECK_RET_D(asyncContext->errorMsg == nullptr, nullptr, EFFECT_LOG_E("image type mismatch."));
    if (argCount >= NUM_2) {
        if (Media::ImageNapiUtils::getType(env, argValue[argCount - 1]) == napi_function) {
            napi_create_reference(env, argValue[argCount - 1], refCount, &asyncContext->callbackRef);
        }
        if (Media::ImageNapiUtils::getType(env, argValue[NUM_1]) != napi_function) {
            IMG_NAPI_CHECK_RET_D(GetRegionCoordinates(
                env, argValue[NUM_1], asyncContext), nullptr, EFFECT_LOG_E("fail to parse coordinates"));
            asyncContext->regionFlag = true;
        }
    }

    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &(asyncContext->deferred), &result);
    }
    if (asyncContext->errorMsg != nullptr) {
        IMG_CREATE_CREATE_ASYNC_WORK(env, status, "CreateColorPickerError", [](napi_env env, void* data) {},
            CreateColorPickerErrorComplete, asyncContext, asyncContext->work);
    } else if (imgType == ImageType::TYPE_PIXEL_MAP) {
        IMG_CREATE_CREATE_ASYNC_WORK(env, status, "CreateColorPickerFromPixelMap", CreateColorPickerFromPixelmapExecute,
            CreateColorPickerFromPixelmapComplete, asyncContext, asyncContext->work);
    }
    
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, EFFECT_LOG_E("fail to create async work"));
    return result;
}

napi_value ColorPickerNapi::GetScaledPixelMap(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    EFFECT_LOG_I("GetScaledPixelMap");
    IMG_JS_NO_ARGS(env, info, status, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         nullptr,
                         EFFECT_LOG_E("GetScaledPixelMap, fail to napi_get_cb_info"));
    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker),
                         nullptr,
                         EFFECT_LOG_E("GetScaledPixelMap, fail to unwrap context"));

    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker->nativeColorPicker_),
                         nullptr,
                         EFFECT_LOG_E("GetScaledPixelMap, empty native colorPicker"));

    auto result = thisColorPicker->nativeColorPicker_->GetScaledPixelMap();
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, result),
                         nullptr,
                         EFFECT_LOG_E("empty pixelmap"));

    EFFECT_LOG_I("GetPixelMap.w,h=%{public}d,%{public}d", result->GetWidth(), result->GetHeight());
    return Media::PixelMapNapi::CreatePixelMap(env, result);
}

static void GetMainColorExecute(napi_env env, void* data)
{
    EFFECT_LOG_I("[ColorPicker]Get color execute");
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    errorCode = context->rColorPicker->GetMainColor(context->color);
    if (errorCode == SUCCESS) {
        context->status = SUCCESS;
    } else {
        context->status = ERROR;
    }
}

static void GetMainColorComplete(napi_env env, napi_status status, void* data)
{
    EFFECT_LOG_I("[ColorPicker]Get color Complete");
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    if (context->status == SUCCESS) {
        EFFECT_LOG_I("[ColorPicker]build color");
        result = BuildJsColor(env, context->color);
    }
    EFFECT_LOG_I("[ColorPicker]Get color[ARGB] %{public}f,%{public}f,%{public}f,%{public}f",
                 context->color.a,
                 context->color.r,
                 context->color.g,
                 context->color.b);
    CommonCallbackRoutine(env, context, result);
}

napi_value ColorPickerNapi::GetMainColor(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    int32_t refCount = 1;
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get MainColor");
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         nullptr,
                         EFFECT_LOG_E("GetMainColor, fail to napi_get_cb_info"));
    
    std::unique_ptr<ColorPickerAsyncContext> asyncContext = std::make_unique<ColorPickerAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->nConstructor));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, asyncContext->nConstructor),
                         nullptr,
                         EFFECT_LOG_E("GetMainColor, fail to unwrap context"));
    asyncContext->rColorPicker = asyncContext->nConstructor->nativeColorPicker_;
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, asyncContext->rColorPicker),
                         nullptr,
                         EFFECT_LOG_E("GetMainColor, empty native ColorPicker"));
    if (argCount == NUM_1 && Media::ImageNapiUtils::getType(env, argValue[argCount - 1]) == napi_function) {
        napi_create_reference(env, argValue[argCount - 1], refCount, &asyncContext->callbackRef);
    }
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &(asyncContext->deferred), &result);
    } else {
        napi_get_undefined(env, &result);
    }

    IMG_CREATE_CREATE_ASYNC_WORK(env, status, "GetMainColor",
                                 GetMainColorExecute,
                                 GetMainColorComplete,
                                 asyncContext, asyncContext->work);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         nullptr,
                         EFFECT_LOG_E("fail to create async work"));
    return result;
}

napi_value BuildJsColor(napi_env env, ColorManager::Color color)
{
    EFFECT_LOG_I("build color");
    napi_value result = nullptr;
    napi_value clrRed = nullptr;
    napi_value clrGreen = nullptr;
    napi_value clrBlue = nullptr;
    napi_value clrAlpha = nullptr;

    napi_create_object(env, &result);

    int colorRed = static_cast<int>(color.r * 255.0f);
    int colorGreen = static_cast<int>(color.g * 255.0f);
    int colorBlue = static_cast<int>(color.b * 255.0f);
    int colorAlpha = static_cast<int>(color.a * 255.0f);

    napi_create_int32(env, colorRed, &clrRed);
    napi_set_named_property(env, result, "red", clrRed);

    napi_create_int32(env, colorGreen, &clrGreen);
    napi_set_named_property(env, result, "green", clrGreen);

    napi_create_int32(env, colorBlue, &clrBlue);
    napi_set_named_property(env, result, "blue", clrBlue);
    
    napi_create_int32(env, colorAlpha, &clrAlpha);
    napi_set_named_property(env, result, "alpha", clrAlpha);

    return result;
}


napi_value ColorPickerNapi::GetMainColorSync(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get MainColor sync");
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         nullptr,
                         EFFECT_LOG_E("GetMainColorSync, fail to napi_get_cb_info"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker),
                         nullptr,
                         EFFECT_LOG_E("GetMainColorSync, fail to unwrap context"));
    
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker->nativeColorPicker_),
                         nullptr,
                         EFFECT_LOG_E("GetMainColorSync, empty native ColorPicker"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetMainColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

// 11 new color picker interface
napi_value ColorPickerNapi::GetLargestProportionColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get Largest Proportion Color");
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         nullptr,
                         EFFECT_LOG_E("GetLargestProportionColor, fail to napi_get_cb_info"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker),
                         nullptr,
                         EFFECT_LOG_E("GetLargestProportionColor, fail to unwrap context"));
    
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker->nativeColorPicker_),
                         nullptr,
                         EFFECT_LOG_E("GetLargestProportionColor, empty native ColorPicker"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetLargestProportionColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::GetHighestSaturationColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get Highest Saturation Color");
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         nullptr,
                         EFFECT_LOG_E("GetHighestSaturationColor, fail to napi_get_cb_info"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker),
                         nullptr,
                         EFFECT_LOG_E("GetHighestSaturationColor, fail to unwrap context"));
    
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker->nativeColorPicker_),
                         nullptr,
                         EFFECT_LOG_E("GetHighestSaturationColor, empty native ColorPicker"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetHighestSaturationColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::GetAverageColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get Average Color");
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         nullptr,
                         EFFECT_LOG_E("GetAverageColor, fail to napi_get_cb_info"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker),
                         nullptr,
                         EFFECT_LOG_E("GetAverageColor, fail to unwrap context"));
    
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker->nativeColorPicker_),
                         nullptr,
                         EFFECT_LOG_E("GetAverageColor, empty native ColorPicker"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetAverageColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::IsBlackOrWhiteOrGrayColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Is Black Or White Or Gray Color");
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
                         nullptr,
                         EFFECT_LOG_E("IsBlackOrWhiteOrGrayColor, fail to napi_get_cb_info"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker),
                         nullptr,
                         EFFECT_LOG_E("IsBlackOrWhiteOrGrayColor, fail to unwrap context"));
    
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, thisColorPicker->nativeColorPicker_),
                         nullptr,
                         EFFECT_LOG_E("IsBlackOrWhiteOrGrayColor, empty native ColorPicker"));

    unsigned int color = 0;
    if (argCount != 1) {
        return nullptr;
    }
    if (Media::ImageNapiUtils::getType(env, argValue[0]) == napi_number) {
        unsigned int scale = 0;
        if (IMG_IS_OK(napi_get_value_uint32(env, argValue[0], &scale))) {
            color = scale;
        }
    }

    bool rst = thisColorPicker->nativeColorPicker_->IsBlackOrWhiteOrGrayColor(color);
    napi_value result = nullptr;
    napi_get_boolean(env, rst, &result);
    return result;
}

ImageType ColorPickerNapi::ParserArgumentType(napi_env env, napi_value argv)
{
    napi_value constructor = nullptr;
    napi_value global = nullptr;
    bool isInstance = false;
    napi_status ret = napi_invalid_arg;

    napi_get_global(env, &global);

    ret = napi_get_named_property(env, global, "ImageSource", &constructor);
    if (ret != napi_ok) {
        EFFECT_LOG_E("Get ImageSourceNapi property failed!");
    }

    ret = napi_instanceof(env, argv, constructor, &isInstance);
    if (ret == napi_ok && isInstance) {
        EFFECT_LOG_I("This is ImageSourceNapi type!");
        return ImageType::TYPE_IMAGE_SOURCE;
    }

    ret = napi_get_named_property(env, global, "PixelMap", &constructor);
    if (ret != napi_ok) {
        EFFECT_LOG_E("Get PixelMapNapi property failed!");
    }

    ret = napi_instanceof(env, argv, constructor, &isInstance);
    if (ret == napi_ok && isInstance) {
        return ImageType::TYPE_PIXEL_MAP;
    }

    EFFECT_LOG_E("InValued type!");
    return ImageType::TYPE_UNKOWN;
}

void BuildMsgOnError(napi_env env,
                     const std::unique_ptr<ColorPickerAsyncContext>& context,
                     bool assertion,
                     const std::string& msg)
{
    if (!assertion) {
        EFFECT_LOG_E("%{public}s", msg.c_str());
        napi_create_string_utf8(env, msg.c_str(), NAPI_AUTO_LENGTH, &(context->errorMsg));
    }
}
}  // namespace Rosen
}  // namespace OHOS

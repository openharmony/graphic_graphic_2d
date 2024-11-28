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

#include "effect_kit_napi_utils.h"
#include "effect_errors.h"
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
    constexpr double PROPORTION_COLORS_NUM_LIMIT = 10.0; // proportion colors limit num 10
}

namespace OHOS {
namespace Rosen {
static const std::string CLASS_NAME = "ColorPicker";
thread_local napi_ref ColorPickerNapi::sConstructor_ = nullptr;
thread_local std::shared_ptr<ColorPicker> ColorPickerNapi::sColorPicker_ = nullptr;

// context
struct ColorPickerAsyncContext {
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    uint32_t status;
    // build error msg
    napi_value errorMsg = {nullptr};
    ColorPickerNapi *nConstructor = {nullptr};
    std::shared_ptr<ColorPicker> rColorPicker = {nullptr};
    std::shared_ptr<Media::PixelMap> rPixelMap = {nullptr};
    ColorManager::Color color;
    bool regionFlag = {false};
    double coordinatesBuffer[4];
};

static void BuildMsgOnError(napi_env env,
                            const std::unique_ptr<ColorPickerAsyncContext>& context,
                            bool assertion,
                            const std::string& msg);

static napi_value BuildJsColor(napi_env env, ColorManager::Color& color);

static void CommonCallbackRoutine(napi_env env, ColorPickerAsyncContext* &asyncContext, const napi_value &valueParam)
{
    EFFECT_NAPI_CHECK_RET_VOID_D(asyncContext != nullptr,
        EFFECT_LOG_E("CommonCallbackRoutine ColorPickerAsyncContext is nullptr"));
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
    EFFECT_LOG_D("ColorPickerNapi::ColorPickerNapi");
}

ColorPickerNapi::~ColorPickerNapi()
{
    EFFECT_LOG_D("ColorPickerNapi::~ColorPickerNapi");
    nativeColorPicker_ = nullptr;

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
        DECLARE_NAPI_FUNCTION("getMorandiBackgroundColor", GetMorandiBackgroundColor),
        DECLARE_NAPI_FUNCTION("getMorandiShadowColor", GetMorandiShadowColor),
        DECLARE_NAPI_FUNCTION("getDeepenImmersionColor", GetDeepenImmersionColor),
        DECLARE_NAPI_FUNCTION("getImmersiveBackgroundColor", GetImmersiveBackgroundColor),
        DECLARE_NAPI_FUNCTION("getImmersiveForegroundColor", GetImmersiveForegroundColor),
        DECLARE_NAPI_FUNCTION("discriminatePitureLightDegree", DiscriminatePitureLightDegree),
        DECLARE_NAPI_FUNCTION("getReverseColor", GetReverseColor),
        DECLARE_NAPI_FUNCTION("getTopProportionColors", GetTopProportionColors),
    };

    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createColorPicker", CreateColorPicker),
    };

    napi_value constructor = nullptr;

    napi_status status = napi_define_class(env, CLASS_NAME.c_str(),
                                           NAPI_AUTO_LENGTH, Constructor,
                                           nullptr,
                                           EFFECT_ARRAY_SIZE(props), props,
                                           &constructor);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("ColorPickerNapi Init define class fail"));

    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("ColorPickerNapi Init create reference fail"));

    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("ColorPickerNapi Init get global fail"));

    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi Init set global named property fail"));

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("ColorPickerNapi Init set named property fail"));

    status = napi_define_properties(env, exports, EFFECT_ARRAY_SIZE(static_prop), static_prop);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("ColorPickerNapi Init define properties fail"));

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

    EFFECT_JS_NO_ARGS(env, info, status, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisVar != nullptr, undefineVar,
        EFFECT_LOG_E("ColorPickerNapi Constructor parsing input fail"));
    
    ColorPickerNapi* pColorPickerNapi = new (std::nothrow) ColorPickerNapi();
    EFFECT_NAPI_CHECK_RET_D(pColorPickerNapi != nullptr, undefineVar,
        EFFECT_LOG_E("ColorPickerNapi Constructor pColorPickerNapi is nullptr"));

    pColorPickerNapi->env_ = env;
    pColorPickerNapi->nativeColorPicker_ = sColorPicker_;

    status = napi_wrap(env, thisVar, pColorPickerNapi, ColorPickerNapi::Destructor, nullptr, nullptr);
    EFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, undefineVar, pColorPickerNapi,
        EFFECT_LOG_E("ColorPickerNapi Constructor wrap fail"));

    sColorPicker_ = nullptr;
    return thisVar;
}

void ColorPickerNapi::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    EFFECT_LOG_D("ColorPickerNapi::Destructor");
    ColorPickerNapi *pColorPickerNapi = static_cast<ColorPickerNapi*>(nativeObject);
    EFFECT_NAPI_CHECK_RET_VOID_D(pColorPickerNapi != nullptr,
        EFFECT_LOG_E("ColorPickerNapi Destructor nativeObject is nullptr"));
    delete pColorPickerNapi;
    pColorPickerNapi = nullptr;
}

static void CreateColorPickerFromPixelMapExecute(napi_env env, void* data)
{
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    EFFECT_NAPI_CHECK_RET_VOID_D(context != nullptr,
        EFFECT_LOG_E("CreateColorPickerFromPixelMapExecute empty context"));

    context->status = ERROR;
    EFFECT_NAPI_CHECK_RET_VOID_D(context->errorMsg == nullptr,
        EFFECT_LOG_E("CreateColorPickerFromPixelMapExecute mismatch args"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    if (context->regionFlag) {
        context->rColorPicker =
            ColorPicker::CreateColorPicker(context->rPixelMap, context->coordinatesBuffer, errorCode);
    } else {
        context->rColorPicker = ColorPicker::CreateColorPicker(context->rPixelMap, errorCode);
    }
    if (context->rColorPicker != nullptr && errorCode == SUCCESS) {
        context->status = SUCCESS;
    }
}

void ColorPickerNapi::CreateColorPickerFromPixelMapComplete(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    EFFECT_NAPI_CHECK_RET_VOID_D(context != nullptr,
        EFFECT_LOG_E("ColorPickerNapi CreateColorPickerFromPixelMapComplete empty context"));

    if (context->errorMsg != nullptr) {
        context->status = ERROR;
        EFFECT_LOG_E("ColorPickerNapi::CreateColorPickerFromPixelMapComplete mismatch args");
        return;
    }

    napi_value constructor = nullptr;
    napi_value result = nullptr;
    status = napi_get_reference_value(env, sConstructor_, &constructor);
    EFFECT_NAPI_CHECK_RET_VOID_D(status == napi_ok,
        EFFECT_LOG_E("ColorPickerNapi CreateColorPickerFromPixelMapComplete napi_get_reference_value fail"));
    sColorPicker_ = context->rColorPicker;
    status = napi_new_instance(env, constructor, NUM_0, nullptr, &result);
    if (status != napi_ok) {
        context->status = ERROR;
        EFFECT_LOG_E("ColorPickerNapi::CreateColorPickerFromPixelMapComplete New instance could not be obtained");
        napi_get_undefined(env, &result);
    }

    CommonCallbackRoutine(env, context, result);
}

static void CreateColorPickerErrorComplete(napi_env env, napi_status status, void *data)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    EFFECT_NAPI_CHECK_RET_VOID_D(context != nullptr,
        EFFECT_LOG_E("CreateColorPickerErrorComplete empty context"));

    context->status = ERROR;
    EFFECT_NAPI_CHECK_RET_VOID_D(context->errorMsg == nullptr,
        EFFECT_LOG_E("CreateColorPickerErrorComplete mismatch args"));
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
    valueType = EffectKitNapiUtils::GetInstance().GetType(env, param);
    if (valueType == napi_undefined) {
        asyncContext->coordinatesBuffer[NUM_0] = 0.0;
        asyncContext->coordinatesBuffer[NUM_1] = 0.0;
        asyncContext->coordinatesBuffer[NUM_2] = 1.0;
        asyncContext->coordinatesBuffer[NUM_3] = 1.0;
        return true;
    }
    uint32_t arraySize = 0;
    EFFECT_NAPI_CHECK_RET_D(IsArrayForNapiValue(env, param, arraySize), false,
        EFFECT_LOG_E("GetRegionCoordinates get args fail, not array"));
    EFFECT_NAPI_CHECK_RET_D(arraySize >= NUM_4, false,
        EFFECT_LOG_E("GetRegionCoordinates coordinates num less than 4"));
    for (size_t i = 0; i < NUM_4; i++) {
        napi_value jsValue;
        EFFECT_NAPI_CHECK_RET_D(napi_get_element(env, param, i, &jsValue) == napi_ok, false,
            EFFECT_LOG_E("GetRegionCoordinates get args fail"));
        double value = 0.0;
        EFFECT_NAPI_CHECK_RET_D(napi_get_value_double(env, jsValue, &value) == napi_ok, false,
            EFFECT_LOG_E("GetRegionCoordinates region coordinates not double"));
        asyncContext->coordinatesBuffer[i] = std::clamp<double>(value, 0.0, 1.0);
    }
    bool coordinatesValid = asyncContext->coordinatesBuffer[NUM_2] > asyncContext->coordinatesBuffer[NUM_0] &&
        asyncContext->coordinatesBuffer[NUM_3] > asyncContext->coordinatesBuffer[NUM_1];
    EFFECT_NAPI_CHECK_RET_D(coordinatesValid, false,
        EFFECT_LOG_E("GetRegionCoordinates right must be greater than left, bottom must be greater than top"));
    return true;
}

std::unique_ptr<ColorPickerAsyncContext> ColorPickerNapi::InitializeAsyncContext(
    napi_env env, napi_status& status, napi_value* argValue, size_t argCount)
{
    auto asyncContext = std::make_unique<ColorPickerAsyncContext>();
    if (argCount >= NUM_1) {
        ImageType imgType = ParserArgumentType(env, argValue[NUM_1 - 1]);
        if (imgType == ImageType::TYPE_PIXEL_MAP) {
            asyncContext->rPixelMap = Media::PixelMapNapi::GetPixelMap(env, argValue[NUM_1 - 1]);
            BuildMsgOnError(env, asyncContext, asyncContext->rPixelMap != nullptr, "Pixmap mismatch");
        } else {
            BuildMsgOnError(env, asyncContext, false, "image type mismatch");
        }
    }

    return asyncContext;
}

bool ColorPickerNapi::ProcessCallbackAndCoordinates(napi_env env, napi_value* argValue, size_t argCount,
    napi_value& result, std::unique_ptr<ColorPickerAsyncContext>& asyncContext)
{
    int32_t refCount = 1;

    if (argCount >= NUM_2) {
        if (EffectKitNapiUtils::GetInstance().GetType(env, argValue[NUM_1]) != napi_function) {
            if (!GetRegionCoordinates(env, argValue[NUM_1], asyncContext)) {
                BuildMsgOnError(env, asyncContext, false, "fail to parse coordinates");
                return false;
            }
            asyncContext->regionFlag = true;
        }
        if (EffectKitNapiUtils::GetInstance().GetType(env, argValue[argCount - 1]) == napi_function) {
            napi_create_reference(env, argValue[argCount - 1], refCount, &asyncContext->callbackRef);
        }
    }
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &(asyncContext->deferred), &result);
    }
    return true;
}

napi_value ColorPickerNapi::CreateColorPicker(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_4] = { 0 };
    size_t argCount = NUM_4;
    ImageType imgType = ImageType::TYPE_UNKOWN;

    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi CreateColorPicker parsing input fail"));

    auto asyncContext = InitializeAsyncContext(env, status, argValue, argCount);
    EFFECT_NAPI_CHECK_RET_D(asyncContext != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi CreateColorPicker asyncContext is nullptr"));

    EFFECT_NAPI_CHECK_RET_D(ProcessCallbackAndCoordinates(env, argValue, argCount, result, asyncContext), nullptr,
        EFFECT_LOG_E("ColorPickerNapi ProcessCallbackAndCoordinates fail"));

    if (asyncContext->errorMsg != nullptr) {
        EffectKitNapiUtils::GetInstance().CreateAsyncWork(
            env, status, "CreateColorPickerError", [](napi_env env, void* data) {}, CreateColorPickerErrorComplete,
            asyncContext, asyncContext->work);
    } else {
        imgType = ParserArgumentType(env, argValue[NUM_1 - 1]); // Re-evaluate image type if necessary
        if (imgType == ImageType::TYPE_PIXEL_MAP) {
            EffectKitNapiUtils::GetInstance().CreateAsyncWork(env, status, "CreateColorPickerFromPixelMap",
                CreateColorPickerFromPixelMapExecute, CreateColorPickerFromPixelMapComplete, asyncContext,
                asyncContext->work);
        }
    }

    if (status != napi_ok) {
        if (asyncContext->callbackRef != nullptr) {
            napi_delete_reference(env, asyncContext->callbackRef);
        }
        if (asyncContext->deferred != nullptr) {
            napi_create_string_utf8(env, "fail to create async work", NAPI_AUTO_LENGTH, &result);
            napi_reject_deferred(env, asyncContext->deferred, result);
        }
        EFFECT_LOG_E("ColorPickerNapi CreateColorPicker creating async work fail");
    }
    return result;
}

static void GetMainColorExecute(napi_env env, void* data)
{
    EFFECT_LOG_I("[ColorPicker]Get color execute");
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    EFFECT_NAPI_CHECK_RET_VOID_D(context != nullptr,
        EFFECT_LOG_E("GetMainColorExecute empty context"));

    if (context->errorMsg != nullptr) {
        context->status = ERROR;
        EFFECT_LOG_E("GetMainColorExecute mismatch args");
        return;
    }

    if (context->rColorPicker == nullptr) {
        context->status = ERROR;
        EFFECT_LOG_E("GetMainColorExecute rColorPicker is nullptr");
        return;
    }
    uint32_t errorCode = context->rColorPicker->GetMainColor(context->color);
    context->status = errorCode == SUCCESS ? SUCCESS : ERROR;
}

static void GetMainColorComplete(napi_env env, napi_status status, void* data)
{
    EFFECT_LOG_I("[ColorPicker]Get color Complete");
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    auto context = static_cast<ColorPickerAsyncContext*>(data);
    EFFECT_NAPI_CHECK_RET_VOID_D(context != nullptr,
        EFFECT_LOG_E("GetMainColorComplete empty context"));

    if (context->errorMsg != nullptr) {
        context->status = ERROR;
        EFFECT_LOG_E("GetMainColorComplete mismatch args");
        return;
    }

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
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMainColor parsing input fail"));
    
    std::unique_ptr<ColorPickerAsyncContext> asyncContext = std::make_unique<ColorPickerAsyncContext>();
    EFFECT_NAPI_CHECK_RET_D(asyncContext != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMainColor asyncContext is nullptr"));

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->nConstructor));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && asyncContext->nConstructor != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMainColor unwrap context fail"));
    asyncContext->rColorPicker = asyncContext->nConstructor->nativeColorPicker_;
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && asyncContext->rColorPicker != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMainColor native ColorPicker is nullptr"));
    if (argCount == NUM_1 && EffectKitNapiUtils::GetInstance().GetType(env, argValue[argCount - 1]) == napi_function) {
        napi_create_reference(env, argValue[argCount - 1], refCount, &asyncContext->callbackRef);
    }
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &(asyncContext->deferred), &result);
    } else {
        napi_get_undefined(env, &result);
    }

    EffectKitNapiUtils::GetInstance().CreateAsyncWork(
        env, status, "GetMainColor", GetMainColorExecute, GetMainColorComplete, asyncContext, asyncContext->work);
    if (status != napi_ok) {
        if (asyncContext->callbackRef != nullptr) {
            napi_delete_reference(env, asyncContext->callbackRef);
        }
        if (asyncContext->deferred != nullptr) {
            napi_create_string_utf8(
                env, "fail to create async work", NAPI_AUTO_LENGTH, &result);
            napi_reject_deferred(env, asyncContext->deferred, result);
        }
        EFFECT_LOG_E("ColorPickerNapi GetMainColor creating async work fail");
    }

    return result;
}

napi_value BuildJsColor(napi_env env, ColorManager::Color& color)
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
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMainColorSync parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMainColorSync unwrap native ColorPicker fail"));

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
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetLargestProportionColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetLargestProportionColor unwrap native ColorPicker fail"));

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
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetHighestSaturationColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetHighestSaturationColor unwrap native ColorPicker fail"));

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
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetAverageColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetAverageColor unwrap native ColorPicker fail"));

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
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    EFFECT_LOG_I("Is Black Or White Or Gray Color");
    EFFECT_JS_ARGS(env, info, status, realArgc, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        EFFECT_LOG_E("ColorPickerNapi IsBlackOrWhiteOrGrayColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi IsBlackOrWhiteOrGrayColor unwrap native ColorPicker fail"));

    unsigned int color = 0;
    if (EffectKitNapiUtils::GetInstance().GetType(env, argValue[NUM_0]) == napi_number) {
        unsigned int scale = 0;
        if (napi_get_value_uint32(env, argValue[NUM_0], &scale) == napi_ok) {
            color = scale;
        }
    }

    bool rst = thisColorPicker->nativeColorPicker_->IsBlackOrWhiteOrGrayColor(color);
    napi_value result = nullptr;
    napi_get_boolean(env, rst, &result);
    return result;
}

napi_value ColorPickerNapi::GetMorandiBackgroundColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get Morandi Background Color");
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMorandiBackgroundColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMorandiBackgroundColor unwrap native ColorPicker fail"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetMorandiBackgroundColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::GetMorandiShadowColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get Morandi Shadow Color");
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMorandiShadowColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetMorandiShadowColor unwrap native ColorPicker fail"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetMorandiShadowColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::GetDeepenImmersionColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get Deepen Immersion Color");
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetDeepenImmersionColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetDeepenImmersionColor unwrap native ColorPicker fail"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetDeepenImmersionColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::GetImmersiveBackgroundColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get Immersive Background Color");
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetImmersiveBackgroundColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetImmersiveBackgroundColor unwrap native ColorPicker fail"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetImmersiveBackgroundColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::GetImmersiveForegroundColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("GetImmersiveForegroundColor, Get Immersive Foreground Color");
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetImmersiveForegroundColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetImmersiveForegroundColor unwrap native ColorPicker fail"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetImmersiveForegroundColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::DiscriminatePitureLightDegree(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Discriminate Piture Light Degree");
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi DiscriminatePitureLightDegree parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi DiscriminatePitureLightDegree unwrap native ColorPicker fail"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result;
    PictureLightColorDegree rst;
    errorCode = thisColorPicker->nativeColorPicker_->DiscriminatePitureLightDegree(rst);
    if (errorCode == SUCCESS) {
        napi_create_int32(env, rst, &result);
    } else {
        rst = UNKOWN_LIGHT_COLOR_DEGREE_PICTURE;
        napi_create_int32(env, rst, &result);
    }
    return result;
}

napi_value ColorPickerNapi::GetReverseColor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = 1;
    EFFECT_LOG_I("Get Reverse Color");
    EFFECT_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetReverseColor parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetReverseColor unwrap native ColorPicker fail"));

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;

    napi_value result = nullptr;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetReverseColor(color);
    if (errorCode == SUCCESS) {
        result = BuildJsColor(env, color);
    } else {
        napi_get_undefined(env, &result);
    }
    return result;
}

napi_value ColorPickerNapi::GetTopProportionColors(napi_env env, napi_callback_info info)
{
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    EFFECT_LOG_I("Get Top Proportion Colors");
    EFFECT_JS_ARGS(env, info, status, realArgc, argValue, thisVar);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetTopProportionColors parsing input fail"));

    ColorPickerNapi *thisColorPicker = nullptr;

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&thisColorPicker));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisColorPicker != nullptr &&
        thisColorPicker->nativeColorPicker_ != nullptr, nullptr,
        EFFECT_LOG_E("ColorPickerNapi GetTopProportionColors unwrap native ColorPicker fail"));
 
    unsigned int colorsNum = 0;
    if (EffectKitNapiUtils::GetInstance().GetType(env, argValue[NUM_0]) == napi_number) {
        double number = 0;
        if (napi_get_value_double(env, argValue[NUM_0], &number) == napi_ok) {
            colorsNum = static_cast<unsigned int>(std::clamp(number, 0.0, PROPORTION_COLORS_NUM_LIMIT));
        }
    }
 
    napi_value arrayValue = nullptr;
    std::vector<ColorManager::Color> colors = thisColorPicker->nativeColorPicker_->GetTopProportionColors(colorsNum);
    napi_create_array_with_length(env, std::max(1u, static_cast<uint32_t>(colors.size())), &arrayValue);
    for (uint32_t i = 0; i < std::max(1u, static_cast<uint32_t>(colors.size())); ++i) {
        napi_value colorValue = i >= colors.size() ?  nullptr : BuildJsColor(env, colors[i]);
        napi_set_element(env, arrayValue, i, colorValue);
    }
    return arrayValue;
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
        return ImageType::TYPE_UNKOWN;
    }

    ret = napi_instanceof(env, argv, constructor, &isInstance);
    if (ret == napi_ok && isInstance) {
        EFFECT_LOG_I("This is ImageSourceNapi type!");
        return ImageType::TYPE_IMAGE_SOURCE;
    }

    ret = napi_get_named_property(env, global, "PixelMap", &constructor);
    if (ret != napi_ok) {
        EFFECT_LOG_E("Get PixelMapNapi property failed!");
        return ImageType::TYPE_UNKOWN;
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

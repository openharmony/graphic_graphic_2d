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
#include "image_napi_utils.h"
#include "../include/filter_napi.h"

namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
    constexpr uint32_t NUM_3 = 3;
    constexpr uint32_t NUM_4 = 4;
}

namespace OHOS {
namespace Rosen {

std::map<int32_t, Drawing::TileMode> INDEX_TO_TILEMODE = {
    { NUM_0, Drawing::TileMode::CLAMP },
    { NUM_1, Drawing::TileMode::REPEAT },
    { NUM_2, Drawing::TileMode::MIRROR },
    { NUM_3, Drawing::TileMode::DECAL },
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
    if (env == nullptr) {
        FILTER_LOG_E("[NAPI] FilterNapi Engine is nullptr");
        return nullptr;
    }
    napi_value object = nullptr;
    napi_create_object(env, &object);
    if (object == nullptr) {
        FILTER_LOG_E("[NAPI] FilterNapi Failed to get object");
        return nullptr;
    }

    for (auto& [TileModeName, TileMode] : STRING_TO_JS_MAP) {
        napi_value value = nullptr;
        napi_create_int32(env, static_cast<int32_t>(TileMode), &value);
        napi_set_named_property(env, object, TileModeName.c_str(), value);
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
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, FILTER_LOG_E("define class fail"));

    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    if (!IMG_IS_OK(status)) {
        FILTER_LOG_I("FilterNapi Init napi_create_reference falid");
        return nullptr;
    }
    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, FILTER_LOG_E("Init:get global fail"));

    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, FILTER_LOG_E("Init:set global named property fail"));

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, FILTER_LOG_E("set named property fail"));

    status = napi_define_properties(env, exports, IMG_ARRAY_SIZE(static_prop), static_prop);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, FILTER_LOG_E("define properties fail"));

    auto tileModeFormat = TileModeInit(env);
    napi_set_named_property(env, exports, "TileMode", tileModeFormat);
    return exports;
}

napi_value FilterNapi::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        FILTER_LOG_E("failed to napi_get_cb_info");
        return nullptr;
    }

    FilterNapi *filterNapi = new(std::nothrow) FilterNapi();
    if (filterNapi == nullptr) {
        FILTER_LOG_E("new filterNapi is nullptr");
        return nullptr;
    }
    status = napi_wrap(env, jsThis, filterNapi, FilterNapi::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete filterNapi;
        FILTER_LOG_E("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void FilterNapi::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    FilterNapi *filterNapi = reinterpret_cast<FilterNapi*>(nativeObject);

    if (IMG_NOT_NULL(filterNapi)) {
        filterNapi->~FilterNapi();
    }
}

napi_value FilterNapi::CreateFilter(napi_env env, napi_callback_info info)
{
    Filter* filterObj = new(std::nothrow) Filter();
    if (filterObj == nullptr) {
        FILTER_LOG_E("new filterNapi is nullptr");
        return nullptr;
    }

    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_wrap(
        env, object, filterObj,
        [](napi_env env, void* data, void* hint) {
            Filter* filterObj = (Filter*)data;
            delete filterObj;
        },
        nullptr, nullptr);
    napi_property_descriptor resultFuncs[] = {
        DECLARE_NAPI_FUNCTION("blur", SetBlur),
        DECLARE_NAPI_FUNCTION("pixelStretch", SetPixelStretch),
    };
    NAPI_CALL(env, napi_define_properties(env, object, sizeof(resultFuncs) / sizeof(resultFuncs[0]), resultFuncs));
    return object;
}

napi_value FilterNapi::SetBlur(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value _this;
    napi_status status;
    IMG_JS_ARGS(env, info, status, argc, argv, _this);
    if (!IMG_IS_OK(status)) {
        FILTER_LOG_I("FilterNapi parse input falid");
        return nullptr;
    }
    float radius = 0.0f;
    if (argc != 1) {
        return nullptr;
    }
    if (Media::ImageNapiUtils::getType(env, argv[0]) == napi_number) {
        double tmp = 0.0f;
        if (IMG_IS_OK(napi_get_value_double(env, argv[0], &tmp))) {
            if (tmp >= 0) {
                radius = static_cast<float>(tmp);
            }
        }
    }
    Filter* filterObj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&filterObj)));
    if (filterObj == nullptr) {
        FILTER_LOG_E("filterNapi is nullptr");
        return nullptr;
    }
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    if (para == nullptr) {
        FILTER_LOG_E("para is nullptr");
        return nullptr;
    }
    para->radius_ = radius;
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
    valueType = Media::ImageNapiUtils::getType(env, param);
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
    para->stretchPercent_ = tmpPercent_;
    return true;
}

napi_value FilterNapi::SetPixelStretch(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_3] = {0};
    size_t argCount = NUM_3;
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, FILTER_LOG_E("fail to napi_get_cb_info"));

    Drawing::TileMode tileMode = Drawing::TileMode::CLAMP;
    std::shared_ptr<PixelStretchPara> para = std::make_shared<PixelStretchPara>();

    if (argCount >= NUM_1) {
        tileMode = ParserArgumentType(env, argValue[NUM_1 - 1]);
    }
    para->stretchTileMode_ = tileMode;
    if (argCount >= NUM_2) {
        IMG_NAPI_CHECK_RET_D(GetStretchPercent(env, argValue[NUM_1], para),
            nullptr, FILTER_LOG_E("fail to parse coordinates"));
    }

    Filter* filterObj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&filterObj)));
    if (filterObj == nullptr) {
        FILTER_LOG_E("filterNapi is nullptr");
        return nullptr;
    }
    filterObj->AddPara(para);

    return thisVar;
}

Drawing::TileMode FilterNapi::ParserArgumentType(napi_env env, napi_value argv)
{
    int32_t mode = 0;
    if (Media::ImageNapiUtils::getType(env, argv) == napi_number) {
        double tmp = 0.0f;
        if (IMG_IS_OK(napi_get_value_double(env, argv, &tmp))) {
            mode = tmp;
        }
    }
    return INDEX_TO_TILEMODE[mode];
}

}  // namespace Rosen
}  // namespace OHOS

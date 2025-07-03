/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <atomic>
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include "filter_napi.h"
#include "effect_errors.h"
#include "effect_utils.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "node_api.h"
#include "pixel_map.h"
#include "effect_kit_napi_utils.h"
#include "napi/native_node_api.h"

namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
    const std::map<std::string, OHOS::Rosen::Drawing::TileMode> STRING_TO_JS_MAP = {
        { "CLAMP", OHOS::Rosen::Drawing::TileMode::CLAMP },
        { "REPEAT", OHOS::Rosen::Drawing::TileMode::REPEAT },
        { "MIRROR", OHOS::Rosen::Drawing::TileMode::MIRROR },
        { "DECAL", OHOS::Rosen::Drawing::TileMode::DECAL },
    };
}

namespace OHOS {
namespace Rosen {
struct FilterAsyncContext {
    napi_env env;
    napi_async_work work;
    napi_deferred deferred; // promise
    napi_ref callback;      // callback
    uint32_t status = SUCCESS;
    napi_value this_;

    // build error msg
    napi_value errorMsg = nullptr;

    // param
    FilterNapi* filterNapi = nullptr;
    bool forceCPU = true;
    std::shared_ptr<Media::PixelMap> dstPixelMap_;
};

static std::shared_mutex filterNapiManagerMutex;
static std::unordered_map<FilterNapi*, std::atomic_bool> filterNapiManager;

static const std::string CLASS_NAME = "Filter";

void BuildMsgOnError(napi_env env, const std::unique_ptr<FilterAsyncContext>& ctx,
    bool assertion, const std::string& msg)
{
    if (!assertion) {
        EFFECT_LOG_E("%{public}s", msg.c_str());
        napi_create_string_utf8(env, msg.c_str(), NAPI_AUTO_LENGTH, &(ctx->errorMsg));
    }
}

static void FilterAsyncCommonComplete(napi_env env, const FilterAsyncContext* ctx, const napi_value& valueParam)
{
    EFFECT_NAPI_CHECK_RET_VOID_D(ctx != nullptr,
        EFFECT_LOG_E("FilterAsyncCommonComplete FilterAsyncContext is nullptr"));
    napi_value result[NUM_2] = {};
    napi_get_undefined(env, &result[NUM_0]);
    napi_get_undefined(env, &result[NUM_1]);

    if (ctx->status == SUCCESS) {
        result[0] = valueParam;
    } else if (ctx->errorMsg != nullptr) {
        result[1] = ctx->errorMsg;
    } else {
        napi_create_string_utf8(env, "FilterNapi Internal Error", NAPI_AUTO_LENGTH, &result[NUM_1]);
    }

    if (ctx->deferred) {
        if (ctx->status == SUCCESS) {
            napi_resolve_deferred(env, ctx->deferred, result[NUM_0]);
        } else {
            napi_reject_deferred(env, ctx->deferred, result[NUM_1]);
        }
    } else {
        napi_value callback = nullptr;
        napi_get_reference_value(env, ctx->callback, &callback);
        napi_call_function(env, ctx->this_, callback, NUM_2, result, nullptr);
        napi_delete_reference(env, ctx->callback);
    }

    napi_delete_async_work(env, ctx->work);

    {
        std::unique_lock<std::shared_mutex> lock(filterNapiManagerMutex);
        auto manager = filterNapiManager.find(ctx->filterNapi);
        if (manager != filterNapiManager.end()) {
            if ((*manager).second.load()) {
                delete ctx->filterNapi;
            }
            filterNapiManager.erase(manager);
        }
    }

    delete ctx;
    ctx = nullptr;
}

FilterNapi::FilterNapi() : env_(nullptr), wrapper_(nullptr)
{
    EFFECT_LOG_D("FilterNapi");
}

FilterNapi::~FilterNapi()
{
    EFFECT_LOG_D("~FilterNapi");
    napi_delete_reference(env_, wrapper_);
}

void FilterNapi::Destructor(napi_env env,
                            void* nativeObject,
                            void* finalize_hint)
{
    EFFECT_LOG_D("FilterNapi Destructor");
    FilterNapi* obj = static_cast<FilterNapi*>(nativeObject);
    EFFECT_NAPI_CHECK_RET_VOID_D(obj != nullptr, EFFECT_LOG_E("FilterNapi Destructor nativeObject is nullptr"));

    std::shared_lock<std::shared_mutex> lock(filterNapiManagerMutex);
    auto manager = filterNapiManager.find(obj);
    if (manager == filterNapiManager.end()) {
        delete obj;
        obj = nullptr;
    } else {
        (*manager).second.store(true);
    }
}

thread_local napi_ref FilterNapi::sConstructor_ = nullptr;

napi_value TileModeInit(napi_env env)
{
    EFFECT_NAPI_CHECK_RET_D(env != nullptr, nullptr, EFFECT_LOG_E("FilterNapi TileModeInit env is nullptr"));
    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi TileModeInit fail to get object"));

    for (auto& [tileModeName, tileMode] : STRING_TO_JS_MAP) {
        napi_value value = nullptr;
        status = napi_create_int32(env, static_cast<int32_t>(tileMode), &value);
        EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
            EFFECT_LOG_E("FilterNapi TileModeInit fail to create int32"));
        status = napi_set_named_property(env, object, tileModeName.c_str(), value);
        EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
            EFFECT_LOG_E("FilterNapi TileModeInit fail to set tileModeName"));
    }
    return object;
}

napi_value FilterNapi::Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("blur", Blur),
        DECLARE_NAPI_FUNCTION("brightness", Brightness),
        DECLARE_NAPI_FUNCTION("grayscale", Grayscale),
        DECLARE_NAPI_FUNCTION("invert", Invert),
        DECLARE_NAPI_FUNCTION("setColorMatrix", SetColorMatrix),
        DECLARE_NAPI_FUNCTION("getPixelMap", GetPixelMap),
        DECLARE_NAPI_FUNCTION("getPixelMapAsync", GetPixelMapAsync),
        DECLARE_NAPI_FUNCTION("getEffectPixelMap", GetPixelMapAsync),
        DECLARE_NAPI_FUNCTION("getEffectPixelMapSync", GetPixelMap),
    };
    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createEffect", CreateEffect),
    };
    napi_value constructor = nullptr;
    status = napi_define_class(
        env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Init define class fail"));

    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Init create reference fail"));

    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Init get global fail"));

    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Init set global named property fail"));

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Init set named property fail"));

    status = napi_define_properties(env, exports, EFFECT_ARRAY_SIZE(static_prop), static_prop);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Init define properties fail"));

    auto tileModeFormat = TileModeInit(env);
    status = napi_set_named_property(env, exports, "TileMode", tileModeFormat);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Init set TileMode fail"));
    return exports;
}

napi_value FilterNapi::CreateEffect(napi_env env, napi_callback_info info)
{
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_status status;
    napi_value thisValue = nullptr;
    napi_value argv[requireArgc] = {nullptr};
    EFFECT_JS_ARGS(env, info, status, realArgc, argv, thisValue);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        EFFECT_LOG_E("FilterNapi CreateEffect parsing input fail"));

    napi_valuetype valueType;
    napi_value instance = nullptr;
    valueType = EffectKitNapiUtils::GetInstance().GetType(env, argv[0]);
    if (valueType == napi_object) {
        napi_value cons = nullptr;
        status = napi_get_reference_value(env, sConstructor_, &cons);
        EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
            EFFECT_LOG_E("FilterNapi CreateEffect napi_get_reference_value fail"));
        status = napi_new_instance(env, cons, requireArgc, argv, &instance);
        EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
            EFFECT_LOG_E("FilterNapi CreateEffect napi_new_instance fail"));
    }
    return instance;
}

napi_value FilterNapi::Constructor(napi_env env, napi_callback_info info)
{
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value argv[NUM_1];
    napi_value _this;
    napi_status status;
    EFFECT_JS_ARGS(env, info, status, realArgc, argv, _this);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        EFFECT_LOG_E("FilterNapi Constructor parsing input fail"));

    FilterNapi* filterNapi = new(std::nothrow) FilterNapi();
    EFFECT_NAPI_CHECK_RET_D(filterNapi != nullptr, nullptr,
        EFFECT_LOG_E("FilterNapi Constructor filterNapi is nullptr"));

    napi_valuetype valueType = napi_undefined;
    valueType = EffectKitNapiUtils::GetInstance().GetType(env, argv[NUM_0]);

    EFFECT_NAPI_CHECK_RET_DELETE_POINTER(valueType == napi_object, nullptr, filterNapi,
        EFFECT_LOG_E("FilterNapi Constructor parse input PixelMapNapi fail, the type is napi_undefined"));
    
    Media::PixelMapNapi* tempPixelMap = nullptr;
    status = napi_unwrap(env, argv[NUM_0], reinterpret_cast<void**>(&tempPixelMap));
    EFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok && tempPixelMap != nullptr, nullptr, filterNapi,
        EFFECT_LOG_E("FilterNapi Constructor parse input PixelMapNapi fail, the PixelMap is nullptr"));
    std::shared_ptr<Media::PixelMap> sharPixelPoint = tempPixelMap->GetPixelNapiInner();
    EFFECT_NAPI_CHECK_RET_DELETE_POINTER(sharPixelPoint != nullptr, nullptr, filterNapi,
        EFFECT_LOG_E("FilterNapi Constructor fail, the srcPixelMap is nullptr"));
    filterNapi->srcPixelMap_ = sharPixelPoint;

    size_t filterSize = sizeof(FilterNapi) + (sharPixelPoint->GetCapacity()) * 2; // 2: srcPixelMap + dstPixelMap
    status = napi_wrap_with_size(env, _this, filterNapi, FilterNapi::Destructor, nullptr, nullptr, filterSize);
    EFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, filterNapi,
        EFFECT_LOG_E("FilterNapi Constructor wrap fail"));

    return _this;
}

DrawingError FilterNapi::Render(bool forceCPU)
{
    EffectImageRender imageRender;
    return imageRender.Render(srcPixelMap_, effectFilters_, forceCPU, dstPixelMap_);
}

void FilterNapi::AddEffectFilter(std::shared_ptr<EffectImageFilter> filter)
{
    effectFilters_.emplace_back(filter);
}

std::shared_ptr<Media::PixelMap> FilterNapi::GetDstPixelMap()
{
    return dstPixelMap_;
}

std::shared_ptr<Media::PixelMap>  FilterNapi::GetSrcPixelMap()
{
    return srcPixelMap_;
}

napi_value FilterNapi::GetPixelMap(napi_env env, napi_callback_info info)
{
    size_t argc = NUM_1;
    napi_value argv[NUM_1];
    napi_value _this;
    napi_status status;
    EFFECT_JS_ARGS(env, info, status, argc, argv, _this);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr,
        EFFECT_LOG_E("FilterNapi GetPixelMap parsing input fail"));

    bool forceCPU = false;
    if (EffectKitNapiUtils::GetInstance().GetType(env, argv[NUM_0]) == napi_boolean) {
        if (napi_get_value_bool(env, argv[NUM_0], &forceCPU) != napi_ok) {
            EFFECT_LOG_I("FilterNapi parsing forceCPU fail");
        }
    }
    FilterNapi* thisFilter = nullptr;
    status = napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisFilter != nullptr, nullptr,
        EFFECT_LOG_E("FilterNapi GetPixelMap CreatPixelMap fail"));
    auto renderRet = thisFilter->Render(forceCPU);
    EFFECT_NAPI_CHECK_RET_D(renderRet == DrawingError::ERR_OK, nullptr,
        EFFECT_LOG_E("FilterNapi GetPixelMap Render fail"));
    return Media::PixelMapNapi::CreatePixelMap(env, thisFilter->GetDstPixelMap());
}

void FilterNapi::GetPixelMapAsyncComplete(napi_env env, napi_status status, void* data)
{
    auto ctx = static_cast<FilterAsyncContext*>(data);
    EFFECT_NAPI_CHECK_RET_VOID_D(ctx != nullptr, EFFECT_LOG_E("FilterNapi GetPixelMapAsyncComplete empty context"));

    napi_value value = nullptr;
    if (ctx->dstPixelMap_ == nullptr) {
        ctx->status = ERROR;
        napi_create_string_utf8(env, "FilterNapi dst pixel map is null", NAPI_AUTO_LENGTH, &(ctx->errorMsg));
    } else {
        std::lock_guard<std::mutex> lock(getPixelMapAsyncCompleteMutex_);
        value = Media::PixelMapNapi::CreatePixelMap(env, ctx->dstPixelMap_);
    }
    FilterAsyncCommonComplete(env, ctx, value);
}

void FilterNapi::GetPixelMapAsyncExecute(napi_env env, void* data)
{
    auto ctx = static_cast<FilterAsyncContext*>(data);
    EFFECT_NAPI_CHECK_RET_VOID_D(ctx != nullptr, EFFECT_LOG_E("FilterNapi GetPixelMapAsyncExecute empty context"));

    bool managerFlag = false;
    {
        std::shared_lock<std::shared_mutex> lock(filterNapiManagerMutex);
        auto manager = filterNapiManager.find(ctx->filterNapi);
        if (manager == filterNapiManager.end()) {
            ctx->status = ERROR;
            return;
        }
        managerFlag = (*manager).second.load();
    }

    if (!managerFlag) {
        std::lock_guard<std::mutex> lock2(getPixelMapAsyncExecuteMutex_);
        if (ctx->filterNapi->Render(ctx->forceCPU) != DrawingError::ERR_OK) {
            ctx->status = ERROR;
            return;
        }
        ctx->dstPixelMap_ = ctx->filterNapi->GetDstPixelMap();
    }
}

static void GetPixelMapAsyncErrorComplete(napi_env env, napi_status status, void* data)
{
    auto ctx = static_cast<FilterAsyncContext*>(data);
    EFFECT_NAPI_CHECK_RET_VOID_D(ctx != nullptr,
        EFFECT_LOG_E("FilterNapi GetPixelMapAsyncErrorComplete empty context"));

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    ctx->status = ERROR;
    FilterAsyncCommonComplete(env, ctx, result);
}

napi_value FilterNapi::GetPixelMapAsync(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    size_t argc = NUM_1;
    napi_value argv[NUM_1];
    napi_status status;
    std::unique_ptr<FilterAsyncContext> ctx = std::make_unique<FilterAsyncContext>();
    EFFECT_JS_ARGS(env, info, status, argc, argv, ctx->this_);
    BuildMsgOnError(env, ctx, status == napi_ok, "FilterNapi GetPixelMapAsync parsing input fail");
    NAPI_CALL(env, napi_unwrap(env, ctx->this_, reinterpret_cast<void**>(&(ctx->filterNapi))));
    BuildMsgOnError(env, ctx, (ctx->filterNapi != nullptr), "FilterNapi GetPixelMapAsync filter is nullptr");
    if (EffectKitNapiUtils::GetInstance().GetType(env, argv[0]) == napi_boolean) {
        EFFECT_NAPI_CHECK_RET_D(napi_get_value_bool(env, argv[NUM_0], &(ctx->forceCPU)) == napi_ok, nullptr,
            EFFECT_LOG_E("FilterNapi: GetPixelMapAsync parsing forceCPU fail"));
    }

    if (argc >= NUM_1) {
        if (EffectKitNapiUtils::GetInstance().GetType(env, argv[argc - 1]) == napi_function) {
            napi_create_reference(env, argv[argc - 1], 1, &(ctx->callback));
        }
    }
    if (ctx->callback == nullptr) {
        napi_create_promise(env, &(ctx->deferred), &result);
    }

    {
        std::unique_lock<std::shared_mutex> lock(filterNapiManagerMutex);
        filterNapiManager[ctx->filterNapi].store(false);
    }

    if (ctx->errorMsg != nullptr) {
        EffectKitNapiUtils::GetInstance().CreateAsyncWork(
            env, status, "GetPixelMapAsyncError",
            [](napi_env env, void* data) { EFFECT_LOG_E("FilterNapi GetPixelMapAsync extracting param fail"); },
            GetPixelMapAsyncErrorComplete, ctx, ctx->work);
    } else {
        EffectKitNapiUtils::GetInstance().CreateAsyncWork(
            env, status, "GetPixelMapAsync", GetPixelMapAsyncExecute, GetPixelMapAsyncComplete, ctx, ctx->work);
    }
    if (status != napi_ok) {
        if (ctx->callback != nullptr) {
            napi_delete_reference(env, ctx->callback);
        }
        if (ctx->deferred != nullptr) {
            napi_create_string_utf8(
                env, "FilterNapi GetPixelMapAsync creating async work fail", NAPI_AUTO_LENGTH, &result);
            napi_reject_deferred(env, ctx->deferred, result);
        }
        EFFECT_LOG_E("FilterNapi GetPixelMapAsync creating async work fail");
    }
    return result;
}

napi_value FilterNapi::Blur(napi_env env, napi_callback_info info)
{
    size_t argc = NUM_2;
    napi_value argv[NUM_2];
    napi_value _this;
    napi_status status;
    EFFECT_JS_ARGS(env, info, status, argc, argv, _this);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Blur parsing input fail"));

    float radius = 0.0f;
    if (EffectKitNapiUtils::GetInstance().GetType(env, argv[NUM_0]) == napi_number) {
        double scale = -1.0f;
        if (napi_get_value_double(env, argv[0], &scale) == napi_ok) {
            if (scale >= 0) {
                radius = static_cast<float>(scale);
            }
        }
    }
    auto tileMode = Drawing::TileMode::DECAL;
    if (argc == NUM_1) {
        EFFECT_LOG_D("FilterNapi parsing input with default drawingTileMode.");
    } else if (argc >= NUM_2) {
        int32_t drawingTileMode = 0;
        EFFECT_NAPI_CHECK_RET_D(napi_get_value_int32(env, argv[1], &drawingTileMode) == napi_ok,
            nullptr, EFFECT_LOG_E("FilterNapi Blur parsing tileMode fail"));
        EFFECT_NAPI_CHECK_RET_D(drawingTileMode >= static_cast<int32_t>(Drawing::TileMode::CLAMP) &&
            drawingTileMode <= static_cast<int32_t>(Drawing::TileMode::DECAL),
            nullptr, EFFECT_LOG_E("FilterNapi Blur invalid tileMode"));
        tileMode = static_cast<Drawing::TileMode>(drawingTileMode);
    } else {
        // nothing to do
    }

    FilterNapi* thisFilter = nullptr;
    status = napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisFilter != nullptr, nullptr,
        EFFECT_LOG_E("FilterNapi Blur napi_unwrap fail"));

    auto blur = EffectImageFilter::Blur(radius, tileMode);
    thisFilter->AddEffectFilter(blur);
    return _this;
}

napi_value FilterNapi::Brightness(napi_env env, napi_callback_info info)
{
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value argv[NUM_1];
    napi_value _this;
    napi_status status;
    EFFECT_JS_ARGS(env, info, status, realArgc, argv, _this);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        EFFECT_LOG_E("FilterNapi Brightness parsing input fail"));

    float fBright = 0.0f;
    if (EffectKitNapiUtils::GetInstance().GetType(env, argv[NUM_0]) == napi_number) {
        double dBright = -1.0f;
        if (napi_get_value_double(env, argv[NUM_0], &dBright) == napi_ok) {
            if (dBright >= 0 && dBright <= 1) {
                fBright = static_cast<float>(dBright);
            }
        }
    }

    FilterNapi* thisFilter = nullptr;
    status = napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisFilter != nullptr, nullptr,
        EFFECT_LOG_E("FilterNapi Brightness napi_unwrap fail"));
    auto brightness = EffectImageFilter::Brightness(fBright);
    thisFilter->AddEffectFilter(brightness);
    return _this;
}

napi_value FilterNapi::Grayscale(napi_env env, napi_callback_info info)
{
    napi_value _this;
    napi_status status;
    EFFECT_JS_NO_ARGS(env, info, status, _this);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Grayscale parsing input fail"));
    FilterNapi* thisFilter = nullptr;
    status = napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisFilter != nullptr, nullptr,
        EFFECT_LOG_E("FilterNapi Grayscale napi_unwrap fail"));
    auto grayscale = EffectImageFilter::Grayscale();
    thisFilter->AddEffectFilter(grayscale);
    return _this;
}

napi_value FilterNapi::Invert(napi_env env, napi_callback_info info)
{
    napi_value _this;
    napi_status status;
    EFFECT_JS_NO_ARGS(env, info, status, _this);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, EFFECT_LOG_E("FilterNapi Invert parsing input fail"));
    FilterNapi* thisFilter = nullptr;
    status = napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisFilter != nullptr, nullptr,
        EFFECT_LOG_E("FilterNapi Invert napi_unwrap fail"));
    auto invert = EffectImageFilter::Invert();
    thisFilter->AddEffectFilter(invert);
    return _this;
}

static uint32_t ParseColorMatrix(napi_env env, napi_value val, Drawing::ColorMatrix &colorMatrix)
{
    bool isArray = false;
    napi_is_array(env, val, &isArray);
    if (!isArray) {
        EFFECT_LOG_E("Parse color matrix is not an array");
        return ERR_INVALID_PARAM;
    }

    uint32_t len = 0;
    if (napi_get_array_length(env, val, &len) != napi_ok ||
        len != Drawing::ColorMatrix::MATRIX_SIZE) {
        EFFECT_LOG_E("Parse color matrix napi_get_array_length fail %{public}u", len);
        return ERR_INVALID_PARAM;
    }

    float matrix[Drawing::ColorMatrix::MATRIX_SIZE] = { 0 };
    for (size_t i = 0; i < len; i++) {
        double itemVal;
        napi_value item;
        napi_get_element(env, val, i, &item);
        if (napi_get_value_double(env, item, &itemVal) != napi_ok) {
            EFFECT_LOG_E("Parsing format in item fail %{public}zu", i);
            return ERR_INVALID_PARAM;
        }
        matrix[i] = static_cast<float>(itemVal);
        EFFECT_LOG_D("FilterNapi color matrix [%{public}zu] = %{public}f.", i, matrix[i]);
    }
    colorMatrix.SetArray(matrix);
    return SUCCESS;
}

napi_value FilterNapi::SetColorMatrix(napi_env env, napi_callback_info info)
{
    const size_t requireArgc = NUM_1;
    size_t realArgc = NUM_1;
    napi_value argv[NUM_1];
    napi_value _this;
    napi_status status;
    uint32_t res = 0;
    Drawing::ColorMatrix colorMatrix;
    EFFECT_JS_ARGS(env, info, status, realArgc, argv, _this);
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        EFFECT_LOG_E("FilterNapi SetColorMatrix parsing input fail"));
    
    res = ParseColorMatrix(env, argv[NUM_0], colorMatrix);
    EFFECT_NAPI_CHECK_RET_D(res == SUCCESS, nullptr, EFFECT_LOG_E("FilterNapi SetColorMatrix Color matrix mismatch"));

    FilterNapi* thisFilter = nullptr;
    status = napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter));
    EFFECT_NAPI_CHECK_RET_D(status == napi_ok && thisFilter != nullptr, nullptr,
        EFFECT_LOG_E("FilterNapi SetColorMatrix napi_unwrap fail"));
    auto applyColorMatrix = EffectImageFilter::ApplyColorMatrix(colorMatrix);
    thisFilter->AddEffectFilter(applyColorMatrix);
    return _this;
}
}
}

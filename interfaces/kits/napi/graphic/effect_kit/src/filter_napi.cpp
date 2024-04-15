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
#include "sk_image_chain.h"
#include "image_napi_utils.h"
#include "sk_image_filter_factory.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
}

namespace OHOS {
namespace Rosen {
struct FilterAsyncContext {
    napi_env env;
    napi_async_work work;
    napi_deferred deferred; // promise
    napi_ref callback;      // callback
    uint32_t status;
    napi_value this_;

    // build error msg
    napi_value errorMsg = nullptr;

    // param
    FilterNapi* filterNapi = nullptr;
    bool forceCPU = false;
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

FilterNapi::FilterNapi() : env_(nullptr), wrapper_(nullptr) {}

FilterNapi::~FilterNapi()
{
    napi_delete_reference(env_, wrapper_);
}

void FilterNapi::Destructor(napi_env env,
                            void* nativeObject,
                            void* finalize_hint)
{
    FilterNapi* obj = static_cast<FilterNapi *>(nativeObject);

    std::shared_lock<std::shared_mutex> lock(filterNapiManagerMutex);
    auto manager = filterNapiManager.find(obj);
    if (manager == filterNapiManager.end()) {
        delete obj;
    } else {
        (*manager).second.store(true);
    }
}

thread_local napi_ref FilterNapi::sConstructor_ = nullptr;

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
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi Init napi_define_class falid");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi Init napi_create_reference falid");
        return nullptr;
    }
    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi Init napi_get_global falid");
        return nullptr;
    }

    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi Init napi_set_named_property falid");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi Init napi_set_named_property falid");
        return nullptr;
    }
    status = napi_define_properties(env, exports, IMG_ARRAY_SIZE(static_prop), static_prop);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi Init napi_set_named_property falid");
        return nullptr;
    }
    return exports;
}
napi_value FilterNapi::CreateEffect(napi_env env, napi_callback_info info)
{
    const size_t requireArgc = 1;
    size_t realArgc = 1;
    napi_status status;
    napi_value thisValue = nullptr;
    napi_value argv[requireArgc] = {nullptr};
    IMG_JS_ARGS(env, info, status, realArgc, argv, thisValue);
    if (status != napi_ok) {
        EFFECT_LOG_I("FilterNapi CreateEffect parsr input Faild");
        return nullptr;
    }
    if (realArgc != requireArgc) {
        EFFECT_LOG_I("FilterNapi CreateEffect the realArgc does not equal requireArgc");
        return nullptr;
    }
    napi_valuetype valueType;
    napi_value instance = nullptr;
    valueType = Media::ImageNapiUtils::getType(env, argv[0]);
    if (valueType == napi_object) {
        napi_value cons = nullptr;
        status = napi_get_reference_value(env, sConstructor_, &cons);
        if (!IMG_IS_OK(status)) {
            EFFECT_LOG_I("FilterNapi CreateEffect napi_get_reference_value falid");
            return nullptr;
        }
        status = napi_new_instance(env, cons, requireArgc, argv, &instance);
        if (!IMG_IS_OK(status)) {
            EFFECT_LOG_I("FilterNapi CreateEffect napi_new_instance falid");
            return nullptr;
        }
    }
    return instance;
}

napi_value FilterNapi::Constructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value _this;
    napi_status status;
    IMG_JS_ARGS(env, info, status, argc, argv, _this);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi Constructor falid");
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    if (argc == 1) {
        valueType = Media::ImageNapiUtils::getType(env, argv[0]);
    }
    if (valueType == napi_undefined) {
        EFFECT_LOG_I("FilterNapi parse input PixelMapNapi faild the type is napi_undefined");
    }
    FilterNapi* filterNapi = new FilterNapi();
    if (valueType == napi_object) {
        Media::PixelMapNapi* tempPixelMap = nullptr;
        napi_unwrap(env, argv[0], reinterpret_cast<void**>(&tempPixelMap));
        if (tempPixelMap == nullptr) {
            EFFECT_LOG_I("Constructor failed when parse input PixelMapNapi, the PixelMap is NULL!");
        } else {
            std::shared_ptr<Media::PixelMap>* sharPixelPoint = tempPixelMap->GetPixelMap();
            filterNapi->srcPixelMap_ = *sharPixelPoint;
        }
    }

    NAPI_CALL(env, napi_wrap(env,
                            _this,
                            filterNapi,
                            FilterNapi::Destructor,
                            nullptr, /* finalize_hint */
                            nullptr));
    return _this;
}

void FilterNapi::Render(bool forceCPU)
{
    Rosen::SKImageChain skImage(srcPixelMap_);
    for (auto filter : skFilters_) {
        skImage.SetFilters(filter);
    }
    skImage.ForceCPU(forceCPU);
    skImage.Draw();
    dstPixelMap_ =  skImage.GetPixelMap();
}

void FilterNapi::AddNextFilter(sk_sp<SkImageFilter> filter)
{
    skFilters_.emplace_back(filter);
}

std::shared_ptr<Media::PixelMap> FilterNapi::GetDstPixelMap()
{
    return dstPixelMap_;
}

napi_value FilterNapi::GetPixelMap(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value _this;
    napi_status status;
    IMG_JS_ARGS(env, info, status, argc, argv, _this);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi parse input falid");
        return nullptr;
    }
    bool forceCPU = false;
    if (Media::ImageNapiUtils::getType(env, argv[0]) == napi_boolean) {
        if (!IMG_IS_OK(napi_get_value_bool(env, argv[0], &forceCPU))) {
            EFFECT_LOG_I("FilterNapi parse foceCPU falid");
        }
    }
    FilterNapi* thisFilter = nullptr;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter)));
    if (thisFilter == nullptr) {
        EFFECT_LOG_I("FilterNapi CreatPixelMap is Faild");
        return nullptr;
    }
    thisFilter->Render(forceCPU);
    return Media::PixelMapNapi::CreatePixelMap(env, thisFilter->GetDstPixelMap());
}

void FilterNapi::GetPixelMapAsyncComplete(napi_env env, napi_status status, void* data)
{
    auto ctx = static_cast<FilterAsyncContext*>(data);
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

    bool managerFlag = false;
    {
        std::shared_lock<std::shared_mutex> lock(filterNapiManagerMutex);
        auto manager = filterNapiManager.find(ctx->filterNapi);
        if (manager == filterNapiManager.end()) {
            ctx->status = ERROR;
            napi_create_string_utf8(env, "FilterNapi filterNapi not found in manager",
                NAPI_AUTO_LENGTH, &(ctx->errorMsg));
            return;
        }
        managerFlag = (*manager).second.load();
    }
    
    if (!managerFlag) {
        std::lock_guard<std::mutex> lock2(getPixelMapAsyncExecuteMutex_);
        ctx->filterNapi->Render(ctx->forceCPU);
        ctx->dstPixelMap_ = ctx->filterNapi->GetDstPixelMap();
    }
}

static void GetPixelMapAsyncErrorComplete(napi_env env, napi_status status, void* data)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    auto ctx = static_cast<FilterAsyncContext*>(data);
    ctx->status = ERROR;
    FilterAsyncCommonComplete(env, ctx, result);
}

napi_value FilterNapi::GetPixelMapAsync(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    
    size_t argc = 1;
    napi_value argv[1];
    napi_status status;

    std::unique_ptr<FilterAsyncContext> ctx = std::make_unique<FilterAsyncContext>();
    IMG_JS_ARGS(env, info, status, argc, argv, ctx->this_);
    BuildMsgOnError(env, ctx, IMG_IS_OK(status), "FilterNapi: GetPixelMapAsync parse input failed");

    NAPI_CALL(env, napi_unwrap(env, ctx->this_, reinterpret_cast<void**>(&(ctx->filterNapi))));
    BuildMsgOnError(env, ctx, (ctx->filterNapi != nullptr), "FilterNapi: GetPixelMapAsync filter is null");

    if (Media::ImageNapiUtils::getType(env, argv[0]) == napi_boolean) {
        if (!IMG_IS_OK(napi_get_value_bool(env, argv[0], &(ctx->forceCPU)))) {
            EFFECT_LOG_I("FilterNapi: GetPixelMapAsync parse forceCPU failed");
        }
    }
    ctx->forceCPU = true;

    if (argc >= NUM_1) {
        if (Media::ImageNapiUtils::getType(env, argv[argc - 1]) == napi_function) {
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
        IMG_CREATE_CREATE_ASYNC_WORK(env, status, "GetPixelMapAsyncError", [](napi_env env, void* data) {},
            GetPixelMapAsyncErrorComplete, ctx, ctx->work);
    } else {
        IMG_CREATE_CREATE_ASYNC_WORK(env, status, "GetPixelMapAsync", GetPixelMapAsyncExecute,
            GetPixelMapAsyncComplete, ctx, ctx->work);
    }

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr,
        EFFECT_LOG_E("FilterNapi: GetPixelMapAsync fail to create async work"));
    return result;
}

napi_value FilterNapi::Blur(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value _this;
    napi_status status;
    IMG_JS_ARGS(env, info, status, argc, argv, _this);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi parse input falid");
        return nullptr;
    }
    float radius = 0.0f;
    if (argc != 1) {
        return nullptr;
    }
    if (Media::ImageNapiUtils::getType(env, argv[0]) == napi_number) {
        double scale = -1.0f;
        if (IMG_IS_OK(napi_get_value_double(env, argv[0], &scale))) {
            if (scale >= 0) {
                radius = static_cast<float>(scale);
            }
        }
    }
    FilterNapi* thisFilter = nullptr;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter)));
    if (thisFilter == nullptr) {
        EFFECT_LOG_I("FilterNapi napi_unwrap is Faild");
        return nullptr;
    }
    auto blur = Rosen::SKImageFilterFactory::Blur(radius);
    thisFilter->AddNextFilter(blur);
    return _this;
}

napi_value FilterNapi::Brightness(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value _this;
    napi_status status;
    IMG_JS_ARGS(env, info, status, argc, argv, _this);
    if (!IMG_IS_OK(status)) {
        EFFECT_LOG_I("FilterNapi parse input falid");
        return nullptr;
    }
    float fBright = 0.0f;
    if (argc != 1) {
        return nullptr;
    }
    if (Media::ImageNapiUtils::getType(env, argv[0]) == napi_number) {
        double dBright = -1.0f;
        if (IMG_IS_OK(napi_get_value_double(env, argv[0], &dBright))) {
            if (dBright >= 0 && dBright <= 1) {
                fBright = static_cast<float>(dBright);
            }
        }
    }

    FilterNapi* thisFilter = nullptr;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter)));
    if (thisFilter == nullptr) {
        EFFECT_LOG_I("FilterNapi napi_unwrap is Faild");
        return nullptr;
    }
    auto brightness = Rosen::SKImageFilterFactory::Brightness(fBright);
    thisFilter->AddNextFilter(brightness);
    return _this;
}

napi_value FilterNapi::Grayscale(napi_env env, napi_callback_info info)
{
    napi_value _this;
    napi_status status;
    IMG_JS_NO_ARGS(env, info, status, _this);
    FilterNapi* thisFilter = nullptr;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter)));
    if (thisFilter == nullptr) {
        EFFECT_LOG_I("FilterNapi napi_unwrap is Faild");
        return nullptr;
    }
    auto grayscale = Rosen::SKImageFilterFactory::Grayscale();
    thisFilter->AddNextFilter(grayscale);
    return _this;
}

napi_value FilterNapi::Invert(napi_env env, napi_callback_info info)
{
    napi_value _this;
    napi_status status;
    IMG_JS_NO_ARGS(env, info, status, _this);
    FilterNapi* thisFilter = nullptr;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter)));
    if (thisFilter == nullptr) {
        EFFECT_LOG_I("FilterNapi napi_unwrap is Faild");
        return nullptr;
    }
    auto invert = Rosen::SKImageFilterFactory::Invert();
    thisFilter->AddNextFilter(invert);
    return _this;
}

static uint32_t ParseColorMatrix(napi_env env, napi_value val, PixelColorMatrix &colorMatrix)
{
    bool isArray = false;
    napi_is_array(env, val, &isArray);
    if (!isArray) {
        EFFECT_LOG_E("Parse color matrix is not an array");
        return ERR_INVALID_PARAM;
    }

    uint32_t len = 0;
    if (napi_get_array_length(env, val, &len) != napi_ok ||
        len != PixelColorMatrix::MATRIX_SIZE) {
        EFFECT_LOG_E("Parse color matrix napi_get_array_length failed %{public}u", len);
        return ERR_INVALID_PARAM;
    }

    for (size_t i = 0; i < len; i++) {
        double itemVal;
        napi_value item;
        napi_get_element(env, val, i, &item);
        if (napi_get_value_double(env, item, &itemVal) != napi_ok) {
            EFFECT_LOG_E("Parse format in item failed %{public}zu", i);
            return ERR_INVALID_PARAM;
        }
        colorMatrix.val[i] = static_cast<float>(itemVal);
        EFFECT_LOG_D("FilterNapi color matrix [%{public}zu] = %{public}f.", i, colorMatrix.val[i]);
    }
    return SUCCESS;
}

napi_value FilterNapi::SetColorMatrix(napi_env env, napi_callback_info info)
{
    size_t argc = NUM_1;
    napi_value argv[NUM_1];
    napi_value _this;
    napi_status status;
    uint32_t res = 0;
    PixelColorMatrix colorMatrix;
    IMG_JS_ARGS(env, info, status, argc, argv, _this);
    if (!IMG_IS_OK(status) || argc != NUM_1) {
        EFFECT_LOG_E("FilterNapi parse input falid");
        napi_throw_error(env, std::to_string(ERR_INVALID_PARAM).c_str(),
            "FilterNapi parse input falid, Color matrix mismatch");
        return nullptr;
    }
    res = ParseColorMatrix(env, argv[NUM_0], colorMatrix);
    if (res != SUCCESS) {
        EFFECT_LOG_E("Color matrix mismatch");
        napi_throw_error(env, std::to_string(res).c_str(), "Color matrix mismatch");
        return nullptr;
    }

    FilterNapi* thisFilter = nullptr;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&thisFilter)));
    if (thisFilter == nullptr) {
        EFFECT_LOG_E("FilterNapi napi_unwrap is Faild");
        return nullptr;
    }
    auto applyColorMatrix = Rosen::SKImageFilterFactory::ApplyColorMatrix(colorMatrix);
    thisFilter->AddNextFilter(applyColorMatrix);
    return _this;
}
}
}

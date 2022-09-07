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
#include "filter_napi.h"
#include "effect_utils.h"
#include "sk_image_chain.h"
#include "image_napi_utils.h"
#include "sk_image_filter_factory.h"

namespace OHOS {
namespace Rosen {
static const std::string CLASS_NAME = "Filter";
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
    delete obj;
}

thread_local napi_ref FilterNapi::sConstructor_ = nullptr;

napi_value FilterNapi::Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("blur", Blur),
        DECLARE_NAPI_FUNCTION("brightness", Brightness),
        DECLARE_NAPI_FUNCTION("grayscale", Grayscale),
        DECLARE_NAPI_FUNCTION("getPixelMap", GetPixelMap),
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
                            &filterNapi->wrapper_));
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
}
}

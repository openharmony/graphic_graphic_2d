/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ani_effect_kit_utils.h"
#include "ani_filter.h"

#include <atomic>
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "effect_errors.h"
#include "effect_utils.h"
#include "effect/shader_effect.h"
#include "filter_napi.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "pixel_map.h"
#include "sk_image_chain.h"
#include "sk_image_filter_factory.h"

namespace OHOS {
namespace Rosen {

AniFilter::AniFilter()
{
    EFFECT_LOG_I("AniFilter constructor");
}

AniFilter::~AniFilter()
{
    EFFECT_LOG_I("AniFilter destructor");
}

static const std::string ANI_CLASS_FILTER = "C{@ohos.effectKit.effectKit.FilterInternal}";

std::shared_ptr<Media::PixelMap> AniFilter::GetDstPixelMap()
{
    return dstPixelMap_;
}

std::shared_ptr<Media::PixelMap> AniFilter::GetSrcPixelMap()
{
    return srcPixelMap_;
}

DrawError AniFilter::Render(bool forceCPU)
{
    Rosen::SKImageChain skImage(srcPixelMap_);
    DrawError ret = skImage.Render(skFilters_, forceCPU, dstPixelMap_);

    return ret;
}

void AniFilter::AddNextFilter(sk_sp<SkImageFilter> filter)
{
    skFilters_.emplace_back(filter);
}

ani_object AniFilter::Blur(ani_env* env, ani_object obj, ani_double param)
{
    SkTileMode tileMode = SkTileMode::kDecal;
    AniFilter* aniFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    if (aniFilter == nullptr) {
        EFFECT_LOG_E("GetFilterFromEnv failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    float radius = 0.0f;
    if (param >= 0) {
        radius = static_cast<float>(param);
    }
    auto blur = Rosen::SKImageFilterFactory::Blur(radius, static_cast<SkTileMode>(tileMode));
    aniFilter->AddNextFilter(blur);

    return AniEffectKitUtils::CreateAniObject(
        env, ANI_CLASS_FILTER.c_str(), "l:", reinterpret_cast<ani_long>(aniFilter));
}

ani_object AniFilter::Grayscale(ani_env* env, ani_object obj)
{
    AniFilter* aniFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    if (aniFilter == nullptr) {
        EFFECT_LOG_E("GetFilterFromEnv failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    auto grayscale = Rosen::SKImageFilterFactory::Grayscale();
    aniFilter->AddNextFilter(grayscale);

    return AniEffectKitUtils::CreateAniObject(
        env, ANI_CLASS_FILTER.c_str(), nullptr, reinterpret_cast<ani_long>(aniFilter));
}

ani_object AniFilter::GetEffectPixelMap(ani_env* env, ani_object obj)
{
    AniFilter* thisFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    bool falseCpu = false;
    if (!thisFilter) {
        EFFECT_LOG_E("thisFilter is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (thisFilter->Render(falseCpu) != DrawError::ERR_OK) {
        EFFECT_LOG_E("Render error");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return Media::PixelMapTaiheAni::CreateEtsPixelMap(env, thisFilter->GetDstPixelMap());
}

ani_object AniFilter::CreateEffect(ani_env* env, ani_object para)
{
    auto aniFilter = std::make_unique<AniFilter>();

    auto pixelMap = OHOS::Media::PixelMapTaiheAni::GetNativePixelMap(env, para);
    if (!pixelMap) {
        EFFECT_LOG_E("AniFilter: pixelMap is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    aniFilter->srcPixelMap_ = pixelMap;

    static const char* className = ANI_CLASS_FILTER.c_str();
    const char* methodSig = "l:";
    return AniEffectKitUtils::CreateAniObject(
        env, className, methodSig, reinterpret_cast<ani_long>(aniFilter.release()));
}

ani_object AniFilter::Blur(ani_env* env, ani_object obj, ani_double param, ani_enum_item enumItem)
{
    AniFilter* aniFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    if (aniFilter == nullptr) {
        EFFECT_LOG_E("GetFilterFromEnv failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    
    float radius = 0.0f;
    if (param >= 0) {
        radius = static_cast<float>(param);
    }
    
    ani_size enumIndex;
    env->EnumItem_GetIndex(enumItem, &enumIndex);
    Drawing::TileMode tileMode = static_cast<Drawing::TileMode>(enumIndex);
    
    auto blur = Rosen::SKImageFilterFactory::Blur(radius, static_cast<SkTileMode>(tileMode));
    aniFilter->AddNextFilter(blur);
    
    return AniEffectKitUtils::CreateAniObject(env, ANI_CLASS_FILTER.c_str(), nullptr,
        reinterpret_cast<ani_long>(aniFilter));
}

ani_object AniFilter::Brightness(ani_env* env, ani_object obj, ani_double param)
{
    AniFilter* aniFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    if (aniFilter == nullptr) {
        EFFECT_LOG_E("GetFilterFromEnv failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    
    float bright = 0.0f;
    if (param >= 0.0 && param <= 1.0) {
        bright = static_cast<float>(param);
    }
    
    auto brightness = Rosen::SKImageFilterFactory::Brightness(bright);
    aniFilter->AddNextFilter(brightness);
    
    return AniEffectKitUtils::CreateAniObject(
        env, ANI_CLASS_FILTER.c_str(), nullptr, reinterpret_cast<ani_long>(aniFilter));
}

ani_object AniFilter::Invert(ani_env* env, ani_object obj)
{
    AniFilter* aniFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    if (aniFilter == nullptr) {
        EFFECT_LOG_E("GetFilterFromEnv failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    
    auto invert = Rosen::SKImageFilterFactory::Invert();
    aniFilter->AddNextFilter(invert);
    
    return AniEffectKitUtils::CreateAniObject(
        env, ANI_CLASS_FILTER.c_str(), nullptr, reinterpret_cast<ani_long>(aniFilter));
}

ani_object AniFilter::SetColorMatrix(ani_env* env, ani_object obj, ani_object arrayObj)
{
    AniFilter* aniFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    if (aniFilter == nullptr) {
        EFFECT_LOG_E("GetFilterFromEnv failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    
    ani_double length;
    if (ANI_OK != env->Object_GetPropertyByName_Double(arrayObj, "length", &length)) {
        EFFECT_LOG_E("get stretchSizes length failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    
    PixelColorMatrix colormatrix;
    int matrixLen = PixelColorMatrix::MATRIX_SIZE;
    for (int i = 0; i < int(length) && i < matrixLen; ++i) {
        ani_double val;
        ani_ref ref;
        if (ANI_OK != env->Object_CallMethodByName_Ref(arrayObj, "$_get", "i:C{std.core.Object}", &ref, (ani_int)i) ||
            ANI_OK != env->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "toDouble", ":d", &val)) {
            EFFECT_LOG_E("Object_CallMethodByName_Ref or Object_CallMethodByName_Double failed");
            return AniEffectKitUtils::CreateAniUndefined(env);
        }
        colormatrix.val[i] = static_cast<float>(val);
    }
    
    auto applyColorMatrix = Rosen::SKImageFilterFactory::ApplyColorMatrix(colormatrix);
    aniFilter->AddNextFilter(applyColorMatrix);
    
    return AniEffectKitUtils::CreateAniObject(
        env, ANI_CLASS_FILTER.c_str(), nullptr, reinterpret_cast<ani_long>(aniFilter));
}

ani_object AniFilter::GetPixelMap(ani_env* env, ani_object obj)
{
    AniFilter* thisFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    bool falseCpu = false;
    if (!thisFilter) {
        EFFECT_LOG_E("thisFilter is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (thisFilter->Render(falseCpu) != DrawError::ERR_OK) {
        EFFECT_LOG_E("Render error");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return Media::PixelMapTaiheAni::CreateEtsPixelMap(env, thisFilter->GetDstPixelMap());
}

ani_object AniFilter::CreateEffectFromPtr(ani_env* env, std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        EFFECT_LOG_E("AniFilter::CreateEffectFromPtr pixelMap is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    auto aniFilter = std::make_unique<AniFilter>();
    aniFilter->srcPixelMap_ = pixelMap;
    static const char* className = ANI_CLASS_FILTER.c_str();
    const char* methodSig = "l:";
    return AniEffectKitUtils::CreateAniObject(
        env, className, methodSig, reinterpret_cast<ani_long>(aniFilter.release()));
}

ani_object AniFilter::KitTransferStaticEffect(ani_env* env, ani_class cls, ani_object obj)
{
    void *unwrapResult = nullptr;
    if (!arkts_esvalue_unwrap(env, obj, &unwrapResult)) {
        EFFECT_LOG_E("AniFilter::KitTransferStaticEffect fail to unwrap input");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        EFFECT_LOG_E("AniFilter::KitTransferStaticEffect unwrapResult is nullptr");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    FilterNapi* napiFilter = static_cast<FilterNapi*>(unwrapResult);
    return static_cast<ani_object>(CreateEffectFromPtr(env, napiFilter->GetSrcPixelMap()));
}

ani_object AniFilter::kitTransferDynamicEffect(ani_env* env,  ani_class cls, ani_long obj)
{
    AniFilter* aniFilter = reinterpret_cast<AniFilter*>(obj);
    if (aniFilter == nullptr) {
        EFFECT_LOG_E("AniFilter::kitTransferDynamicEffect aniFilter is nullptr");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    napi_env napiEnv = {};
    if (!arkts_napi_scope_open(env, &napiEnv)) {
        EFFECT_LOG_E("AniFilter::kitTransferDynamicEffect napi scope open failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    napi_value napiFilter = FilterNapi::CreateEffectFromPtr(napiEnv, aniFilter->GetSrcPixelMap());
    hybridgref ref {};
    if (!hybridgref_create_from_napi(napiEnv, napiFilter, &ref)) {
        EFFECT_LOG_E("AniFilter::kitTransferDynamicEffect create hybridgref failed");
        arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    ani_object result {};
    if (!hybridgref_get_esvalue(env, ref, &result)) {
        EFFECT_LOG_E("AniFilter::kitTransferDynamicEffect get esvalue failed");
        hybridgref_delete_from_napi(napiEnv, ref);
        arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!hybridgref_delete_from_napi(napiEnv, ref)) {
        EFFECT_LOG_E("AniFilter::kitTransferDynamicEffect delete hybridgref fail");
        arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr)) {
        EFFECT_LOG_E("AniFilter::kitTransferDynamicEffect napi close scope fail");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return result;
}

ani_status AniFilter::Init(ani_env* env)
{
    static const char* className = ANI_CLASS_FILTER.c_str();
    ani_class cls;
    if (env->FindClass(className, &cls) != ANI_OK) {
        EFFECT_LOG_E("Not found @ohos.effectKit.effectKit.FilterInternal");
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "blurNative", "d:C{@ohos.effectKit.effectKit.Filter}",
            reinterpret_cast<void*>(static_cast<ani_object(*)(ani_env*, ani_object, ani_double)>
            (&OHOS::Rosen::AniFilter::Blur)) },
        ani_native_function { "grayscaleNative", ":C{@ohos.effectKit.effectKit.Filter}",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::Grayscale) },
        ani_native_function { "getEffectPixelMapNative", ":C{@ohos.multimedia.image.image.PixelMap}",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::GetEffectPixelMap) },
        ani_native_function { "blurNative",
            "dC{@ohos.effectKit.effectKit.TileMode}:C{@ohos.effectKit.effectKit.Filter}",
            reinterpret_cast<void*>(static_cast<ani_object(*)(ani_env*, ani_object, ani_double, ani_enum_item)>
            (&OHOS::Rosen::AniFilter::Blur)) },
        ani_native_function { "brightnessNative", "d:C{@ohos.effectKit.effectKit.Filter}",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::Brightness) },
        ani_native_function { "invertNative", ":C{@ohos.effectKit.effectKit.Filter}",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::Invert) },
        ani_native_function { "setColorMatrixNative", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::SetColorMatrix) },
        ani_native_function { "getPixelMapNative", ":C{@ohos.multimedia.image.image.PixelMap}",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::GetPixelMap) },
    };
    ani_status ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        EFFECT_LOG_E("AniFilter::Init Class_BindNativeMethods ret : %{public}d", ret);
        return ANI_ERROR;
    }
    std::array static_methods = {
        ani_native_function { "kitTransferStaticNative", "C{std.interop.ESValue}:C{std.core.Object}",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::KitTransferStaticEffect) },
        ani_native_function { "kitTransferDynamicNative", "l:C{std.interop.ESValue}",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::kitTransferDynamicEffect) }
    };
    ret = env->Class_BindStaticNativeMethods(cls, static_methods.data(), static_methods.size());
    if (ret != ANI_OK) {
        EFFECT_LOG_E("AniFilter::Init Class_BindStaticNativeMethods ret : %{public}d", ret);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace Rosen
} // namespace OHOS
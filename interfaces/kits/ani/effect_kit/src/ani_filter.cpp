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

#include "ani_filter.h"
#include "ani_effect_kit_utils.h"
#include <atomic>
#include <cstddef>
#include "effect/shader_effect.h"
#include "effect_errors.h"
#include "effect_utils.h"
#include <mutex>
#include "pixel_map.h"
#include <shared_mutex>
#include "sk_image_chain.h"
#include "sk_image_filter_factory.h"
#include <unordered_map>

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

static const std::string ANI_CLASS_FILTER = "L@ohos/effectKit/effectKit/FilterInternal;";

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
        env, ANI_CLASS_FILTER.c_str(), "J:V", reinterpret_cast<ani_long>(aniFilter));
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

    return AniEffectKitUtils::CreateAniObject(
        env, ANI_CLASS_FILTER.c_str(), "J:V", reinterpret_cast<ani_long>(aniFilter.release()));
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
    int matrixLen = 20;
    for (int i = 0; i < int(length) && i < matrixLen; ++i) {
        ani_float floatValue;
        if (ANI_OK != env->Object_CallMethodByName_Float(arrayObj, "$_get", "I:F", &floatValue, (ani_int)i)) {
            EFFECT_LOG_E("stretchSizes Object_CallMethodByName_Float_A failed");
            return AniEffectKitUtils::CreateAniUndefined(env);
        }
        colormatrix.val[i] = static_cast<float>(floatValue);
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

ani_status AniFilter::Init(ani_env* env)
{
    static const char* className = ANI_CLASS_FILTER.c_str();
    ani_class cls;
    if (env->FindClass(className, &cls) != ANI_OK) {
        EFFECT_LOG_E("Not found L@ohos/effectKit/effectKit/FilterInternal");
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "blurNative", "D:L@ohos/effectKit/effectKit/Filter;",
            reinterpret_cast<void*>(static_cast<ani_object(*)(ani_env*, ani_object, ani_double)>
            (&OHOS::Rosen::AniFilter::Blur)) },
        ani_native_function { "grayscaleNative", ":L@ohos/effectKit/effectKit/Filter;",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::Grayscale) },
        ani_native_function { "getEffectPixelMapNative", ":L@ohos/multimedia/image/image/PixelMap;",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::GetEffectPixelMap) },
        ani_native_function { "blurNative", "DL@ohos/effectKit/effectKit/TileMode;:L@ohos/effectKit/effectKit/Filter;",
            reinterpret_cast<void*>(static_cast<ani_object(*)(ani_env*, ani_object, ani_double, ani_enum_item)>
            (&OHOS::Rosen::AniFilter::Blur)) },
        ani_native_function { "brightnessNative", "D:L@ohos/effectKit/effectKit/Filter;",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::Brightness) },
        ani_native_function { "invertNative", ":L@ohos/effectKit/effectKit/Filter;",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::Invert) },
        ani_native_function { "setColorMatrixNative", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::SetColorMatrix) },
        ani_native_function { "getPixelMapNative", ":L@ohos/multimedia/image/image/PixelMap;",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::GetPixelMap) },
    };
    ani_status ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        EFFECT_LOG_I("Class_BindNativeMethods failed: %{public}d", ret);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace Rosen
} // namespace OHOS

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

#include <atomic>
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "ani_filter.h"
#include "ani_effect_kit_utils.h"
#include "effect_errors.h"
#include "effect_utils.h"
#include "pixel_map.h"
#include "sk_image_chain.h"
#include "sk_image_filter_factory.h"


namespace OHOS {
namespace Rosen {

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
    auto blur = Rosen::SKImageFilterFactory::Blur(radius, tileMode);
    aniFilter->AddNextFilter(blur);

    static const char* className = ANI_CLASS_FILTER.c_str();
    const char* methodSig = "J:V";
    return AniEffectKitUtils::CreateAniObject(
        env, className, methodSig, reinterpret_cast<ani_long>(aniFilter));
}

ani_object AniFilter::GetEffectPixelMap(ani_env* env, ani_object obj)
{
    AniFilter* thisFilter = AniEffectKitUtils::GetFilterFromEnv(env, obj);
    if (!thisFilter) {
        EFFECT_LOG_E("thisFilter is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    bool forceCpu = false;
    if (thisFilter->Render(forceCpu) != DrawError::ERR_OK) {
        EFFECT_LOG_E("Render error");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return Media::PixelMapAni::CreatePixelMap(env, thisFilter->GetDstPixelMap());
}

ani_object AniFilter::CreateEffect(ani_env* env, ani_object para)
{
    auto aniFilter = std::make_unique<AniFilter>();
    std::shared_ptr<Media::PixelMap> pixelMap(AniEffectKitUtils::GetPixelMapFromEnv(env, para));
    if (!pixelMap) {
        EFFECT_LOG_E("pixelMap is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    aniFilter->srcPixelMap_ = pixelMap;

    static const char* className = ANI_CLASS_FILTER.c_str();
    const char* methodSig = "J:V";
    return AniEffectKitUtils::CreateAniObject(
        env, className, methodSig, reinterpret_cast<ani_long>(aniFilter.release()));
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
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::Blur) },
        ani_native_function { "getEffectPixelMapNative", ":L@ohos/multimedia/image/image/PixelMap;",
            reinterpret_cast<void*>(OHOS::Rosen::AniFilter::GetEffectPixelMap) }
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

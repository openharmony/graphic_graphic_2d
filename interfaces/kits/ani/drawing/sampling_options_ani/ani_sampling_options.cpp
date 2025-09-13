/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ani_sampling_options.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/sampling_options_napi/js_sampling_options.h"

namespace OHOS::Rosen {
namespace Drawing {

const char* ANI_CLASS_SAMPLING_OPTIONS_NAME = "@ohos.graphics.drawing.drawing.SamplingOptions";

ani_status AniSamplingOptions::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_SAMPLING_OPTIONS_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_SAMPLING_OPTIONS_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "C{@ohos.graphics.drawing.drawing.FilterMode}:",
            reinterpret_cast<void*>(ConstructorWithFilterMode) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_SAMPLING_OPTIONS_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "samplingOptionsTransferStaticNative", nullptr,
            reinterpret_cast<void*>(SamplingOptionsTransferStatic) },
        ani_native_function { "getSamplingOptionsAddr", nullptr, reinterpret_cast<void*>(GetSamplingOptionsAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_SAMPLING_OPTIONS_NAME);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}


void AniSamplingOptions::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<SamplingOptions> samplingOptions = std::make_shared<SamplingOptions>();
    AniSamplingOptions* aniSamplingOptions = new AniSamplingOptions(samplingOptions);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniSamplingOptions))) {
        ROSEN_LOGE("AniSamplingOptions::Constructor failed create aniSamplingOptions");
        delete aniSamplingOptions;
        return;
    }
}

void AniSamplingOptions::ConstructorWithFilterMode(ani_env* env, ani_object obj, ani_enum_item filterModeObj)
{
    ani_int filterMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(filterModeObj, &filterMode)) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    std::shared_ptr<SamplingOptions> samplingOptions = std::make_shared<SamplingOptions>(FilterMode(filterMode));
    AniSamplingOptions* aniSamplingOptions = new AniSamplingOptions(samplingOptions);

    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniSamplingOptions))) {
        ROSEN_LOGE("AniSamplingOptions::Constructor failed create aniSamplingOptions");
        delete aniSamplingOptions;
        return;
    }
}

ani_object AniSamplingOptions::SamplingOptionsTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniSamplingOptions::SamplingOptionsTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniSamplingOptions::SamplingOptionsTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsSamplingOptions = reinterpret_cast<JsSamplingOptions*>(unwrapResult);
    if (jsSamplingOptions->GetSamplingOptions() == nullptr) {
        ROSEN_LOGE("AniSamplingOptions::SamplingOptionsTransferStatic jsSamplingOptions is null");
        return nullptr;
    }

    auto aniSamplingOptions = new AniSamplingOptions(jsSamplingOptions->GetSamplingOptions());
    if (ANI_OK != env->Object_SetFieldByName_Long(output, NATIVE_OBJ, reinterpret_cast<ani_long>(aniSamplingOptions))) {
        ROSEN_LOGE("AniSamplingOptions::SamplingOptionsTransferStatic failed create aniSamplingOptions");
        delete aniSamplingOptions;
        return nullptr;
    }
    return output;
}

ani_long AniSamplingOptions::GetSamplingOptionsAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniSamplingOptions = GetNativeFromObj<AniSamplingOptions>(env, input);
    if (aniSamplingOptions == nullptr || aniSamplingOptions->GetSamplingOptions() == nullptr) {
        ROSEN_LOGE("AniSamplingOptions::GetSamplingOptionsAddr aniSamplingOptions is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniSamplingOptions->GetSamplingOptionsPtrAddr());
}

std::shared_ptr<SamplingOptions>* AniSamplingOptions::GetSamplingOptionsPtrAddr()
{
    return &m_samplingOptions;
}

AniSamplingOptions::~AniSamplingOptions()
{
    m_samplingOptions = nullptr;
}

std::shared_ptr<SamplingOptions> AniSamplingOptions::GetSamplingOptions()
{
    return m_samplingOptions;
}

} // namespace Drawing
} // namespace OHOS::Rosen

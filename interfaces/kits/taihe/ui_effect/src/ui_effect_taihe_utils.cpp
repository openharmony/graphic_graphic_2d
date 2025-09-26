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

#include "ui_effect_taihe_utils.h"

#include "ani.h"
#include "common/rs_vector3.h"
#include "effect/include/brightness_blender.h"
#include "effect/shader_effect.h"
#include "filter/include/filter_para.h"
#include "ohos.graphics.uiEffect.TileMode.proj.0.hpp"
#include "taihe/array.hpp"
#include "taihe/runtime.hpp"

#ifdef ENABLE_IPC_SECURITY
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#endif

namespace ANI::UIEffect {
bool IsSystemApp()
{
#ifdef ENABLE_IPC_SECURITY
    uint64_t tokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return OHOS::Security::AccessToken::AccessTokenKit::IsSystemAppByFullTokenID(tokenId);
#else
    return true;
#endif
}

Drawing::TileMode ConvertTileModeFromTaiheTileMode(TileMode tileMode)
{
    switch (tileMode.get_key()) {
        case TileMode::key_t::CLAMP:
            return Drawing::TileMode::CLAMP;
        case TileMode::key_t::REPEAT:
            return Drawing::TileMode::REPEAT;
        case TileMode::key_t::MIRROR:
            return Drawing::TileMode::MIRROR;
        case TileMode::key_t::DECAL:
            return Drawing::TileMode::DECAL;
        default: {
            UIEFFECT_LOG_E("ConvertTileModeFromTaiheTileMode unknown tileMode: %{public}d",
                tileMode.get_key());
            return Drawing::TileMode::CLAMP;
        }
    }
}

bool IsAniTuple(uintptr_t opaque)
{
    ani_env* env = get_env();
    if (env == nullptr) {
        UIEFFECT_LOG_E("IsAniTuple failed, env is nullptr");
        return false;
    }

    ani_status status = ANI_OK;
    ani_class cls = nullptr;
    status = env->FindClass("std.core.Tuple", &cls);
    if (status != ANI_OK) {
        UIEFFECT_LOG_E("IsAniTuple failed, FindClass failed, status: %{public}d", status);
        return false;
    }

    ani_boolean res = false;
    status = env->Object_InstanceOf((ani_object)opaque, cls, &res);
    if (status != ANI_OK) {
        UIEFFECT_LOG_E("IsAniTuple failed, Object_InstanceOf failed, status: %{public}d", status);
        return false;
    }

    return res;
}

bool ConvertVector4fFromTaiheArray(OHOS::Rosen::Vector4f& vector4f, taihe::array_view<double> array)
{
    if (array.size() < OHOS::Rosen::Vector4f::V4SIZE) {
        UIEFFECT_LOG_E("ConvertVector4fFromTaiheArray array size is less than 4");
        return false;
    }

    for (uint32_t i = 0; i < OHOS::Rosen::Vector4f::V4SIZE; ++i) {
        vector4f.data_[i] = static_cast<float>(array[i]);
    }

    return true;
}

bool ConvertVector3fFromAniTuple(OHOS::Rosen::Vector3f& vector3f, uintptr_t opaque)
{
    ani_env* env = get_env();
    if (env == nullptr) {
        UIEFFECT_LOG_E("ConvertVector3fFromAniTuple failed, env is nullptr");
        return false;
    }

    if (!IsAniTuple(opaque)) {
        UIEFFECT_LOG_E("ConvertVector3fFromAniTuple failed, opaque is not tuple");
        return false;
    }

    auto tuple = reinterpret_cast<ani_tuple_value>(opaque);
    ani_size length = 0;
    ani_status status = env->TupleValue_GetNumberOfItems(tuple, &length);
    if (status != ANI_OK) {
        UIEFFECT_LOG_E("ConvertVector3fFromAniTuple failed, "
            "TupleValue_GetNumberOfItems failed, status: %{public}d", status);
        return false;
    }

    if (length < OHOS::Rosen::Vector3f::V3SIZE) {
        UIEFFECT_LOG_E("ConvertVector3fFromAniTuple failed, array size is less than 3");
        return false;
    }

    ani_double itemValue = 0.f;
    for (uint32_t i = 0; i < OHOS::Rosen::Vector3f::V3SIZE; ++i) {
        ani_status getItemStatus = env->TupleValue_GetItem_Double(tuple, i, &itemValue);
        if (getItemStatus != ANI_OK) {
            UIEFFECT_LOG_E("ConvertVector3fFromAniTuple failed, "
                "get item as double from tuple value failed, status: %{public}d", getItemStatus);
            return false;
        }
        vector3f.data_[i] = itemValue;
    }

    return true;
}

// without OHOS::Rosen::BrightnessBlender SetHdr
bool ParseBrightnessBlender(OHOS::Rosen::BrightnessBlender& blender, const BrightnessBlender& brightnessBlender)
{
    blender.SetCubicRate(brightnessBlender.cubicRate);
    blender.SetQuadRate(brightnessBlender.quadraticRate);
    blender.SetLinearRate(brightnessBlender.linearRate);
    blender.SetDegree(brightnessBlender.degree);
    blender.SetSaturation(brightnessBlender.saturation);

    OHOS::Rosen::Vector3f vector3f;
    if (!ConvertVector3fFromAniTuple(vector3f, brightnessBlender.positiveCoefficient)) {
        UIEFFECT_LOG_E("ParseBrightnessBlender parse positiveCoefficient failed");
        return false;
    }
    blender.SetPositiveCoeff(vector3f);

    if (!ConvertVector3fFromAniTuple(vector3f, brightnessBlender.negativeCoefficient)) {
        UIEFFECT_LOG_E("ParseBrightnessBlender parse negativeCoefficient failed");
        return false;
    }
    blender.SetNegativeCoeff(vector3f);

    blender.SetFraction(brightnessBlender.fraction);
    return true;
}
} // namespace ANI::UIEffect
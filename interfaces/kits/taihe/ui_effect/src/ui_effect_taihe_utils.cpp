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
#include "ohos.graphics.uiEffect.uiEffect.TileMode.proj.0.hpp"
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

void ClampVector4f(OHOS::Rosen::Vector4f& v, float minx, float maxn)
{
    for (auto& data : v.data_) {
        data = std::clamp(data, minx, maxn);
    }
}

uint32_t ConvertUint32FromTaiheFlyMode(FlyMode flyMode)
{
    auto key = flyMode.get_key();
    switch (key) {
        case FlyMode::key_t::TOP:
        case FlyMode::key_t::BOTTOM:
            return static_cast<uint32_t>(FlyMode(key).get_value());
        default: {
            UIEFFECT_LOG_E("ConvertUint32FromTaiheFlyMode unknown flyMode");
            return 0;
        }
    }
}

uint32_t ConvertUint32FromTaiheWaterRippleMode(WaterRippleMode waterRippleMode)
{
    auto key = waterRippleMode.get_key();
    switch (key) {
        case WaterRippleMode::key_t::SMALL2MEDIUM_RECV:
        case WaterRippleMode::key_t::SMALL2MEDIUM_SEND:
        case WaterRippleMode::key_t::SMALL2SMALL:
        case WaterRippleMode::key_t::MINI_RECV:
            return static_cast<uint32_t>(WaterRippleMode(key).get_value());
        default: {
            UIEFFECT_LOG_E("ConvertUint32FromTaiheWaterRippleMode unknown waterRippleMode");
            return 0;
        }
    }
}

bool ParseRadialGradientValues(taihe::array_view<uintptr_t> gradients,
    std::vector<float>& colors, std::vector<float>& positions)
{
    size_t length = gradients.size();
    colors.reserve(length);
    positions.reserve(length);

    OHOS::Rosen::Vector2f vector2f;
    for (size_t i = 0; i < length; ++i) {
        if (!ConvertVector2fFromAniTuple(vector2f, gradients[i])) {
            return false;
        }
        colors.emplace_back(static_cast<float>(vector2f[NUM_0]));
        positions.emplace_back(static_cast<float>(vector2f[NUM_1]));
    }

    return true;
}


bool GetDoublePropertyByName(ani_env *env, ani_object object, const char *name, ani_double result)
{
    if (env == nullptr || object == nullptr || name == nullptr) {
        return false;
    }
    ani_status status = ANI_ERROR;

    if ((status = env->Object_GetPropertyByName_Double(object, name, &result)) != ANI_OK) {
        return false;
    }
    return true;
}

bool ConvertVector2fFromAniPoint(uintptr_t point, OHOS::Rosen::Vector2f& values)
{
    ani_env *env = get_env();
    ani_object ani_obj = reinterpret_cast<ani_object>(point);
    ani_double x = 0.0;
    ani_double y = 0.0;

    if (!(GetDoublePropertyByName(env, ani_obj, "x", x) && GetDoublePropertyByName(env, ani_obj, "y", y))) {
        return false;
    }
    values[NUM_0] = static_cast<float>(x);
    values[NUM_1] = static_cast<float>(y);
    return true;
}

bool ConvertVector3fFromAniPoint3D(uintptr_t point3D, OHOS::Rosen::Vector3f& values)
{
    ani_env *env = get_env();
    ani_object ani_obj = reinterpret_cast<ani_object>(point3D);
    ani_double x = 0.0;
    ani_double y = 0.0;
    ani_double z = 0.0;

    if (!(GetDoublePropertyByName(env, ani_obj, "x", x) && GetDoublePropertyByName(env, ani_obj, "y", y)
        && GetDoublePropertyByName(env, ani_obj, "z", z))) {
        return false;
    }
    values[NUM_0] = static_cast<float>(x);
    values[NUM_1] = static_cast<float>(y);
    values[NUM_2] = static_cast<float>(z);
    return true;
}

bool ConvertVector4fFromAniRect(uintptr_t rect, OHOS::Rosen::Vector4f& values)
{
    ani_env *env = get_env();
    ani_object ani_obj = reinterpret_cast<ani_object>(rect);
    ani_double left = 0.0;
    ani_double right = 0.0;
    ani_double top = 0.0;
    ani_double bottom = 0.0;

    bool flag = GetDoublePropertyByName(env, ani_obj, "left", left) &&
        GetDoublePropertyByName(env, ani_obj, "right", right) && GetDoublePropertyByName(env, ani_obj, "top", top) &&
        GetDoublePropertyByName(env, ani_obj, "bottom", bottom);
    if (!flag) {
        return false;
    }
    values[NUM_0] = static_cast<float>(left);
    values[NUM_1] = static_cast<float>(top);
    values[NUM_2] = static_cast<float>(right);
    values[NUM_3] = static_cast<float>(bottom);
    return true;
}

bool ConvertVector4fFromAniColor(uintptr_t color, OHOS::Rosen::Vector4f& values)
{
    ani_env *env = get_env();
    ani_object ani_obj = reinterpret_cast<ani_object>(color);
    ani_double colorR = 0.0;
    ani_double colorG = 0.0;
    ani_double colorB = 0.0;
    ani_double colorA = 0.0;

    bool flag = GetDoublePropertyByName(env, ani_obj, "red", colorR) &&
        GetDoublePropertyByName(env, ani_obj, "green", colorG) &&
        GetDoublePropertyByName(env, ani_obj, "blue", colorB) && GetDoublePropertyByName(env, ani_obj, "alpha", colorA);
    if (!flag) {
        return false;
    }
    values[NUM_0] = static_cast<float>(colorR);
    values[NUM_1] = static_cast<float>(colorG);
    values[NUM_2] = static_cast<float>(colorB);
    values[NUM_3] = static_cast<float>(colorA);
    return true;
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

bool ConvertVector2fFromAniTuple(OHOS::Rosen::Vector2f& vector2f, uintptr_t opaque)
{
    ani_env* env = get_env();
    if (env == nullptr) {
        UIEFFECT_LOG_E("ConvertVector2fFromAniTuple failed, env is nullptr");
        return false;
    }

    if (!IsAniTuple(opaque)) {
        UIEFFECT_LOG_E("ConvertVector2fFromAniTuple failed, opaque is not tuple");
        return false;
    }

    auto tuple = reinterpret_cast<ani_tuple_value>(opaque);
    ani_size length = 0;
    ani_status status = env->TupleValue_GetNumberOfItems(tuple, &length);
    if (status != ANI_OK) {
        UIEFFECT_LOG_E("ConvertVector2fFromAniTuple failed, "
            "TupleValue_GetNumberOfItems failed, status: %{public}d", status);
        return false;
    }

    if (length < OHOS::Rosen::Vector2f::V2SIZE) {
        UIEFFECT_LOG_E("ConvertVector2fFromAniTuple failed, array size is less than 2");
        return false;
    }

    ani_double itemValue = 0.f;
    for (uint32_t i = 0; i < OHOS::Rosen::Vector2f::V2SIZE; ++i) {
        ani_status getItemStatus = env->TupleValue_GetItem_Double(tuple, i, &itemValue);
        if (getItemStatus != ANI_OK) {
            UIEFFECT_LOG_E("ConvertVector2fFromAniTuple failed, "
                "get item as double from tuple value failed, status: %{public}d", getItemStatus);
            return false;
        }
        vector2f.data_[i] = itemValue;
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
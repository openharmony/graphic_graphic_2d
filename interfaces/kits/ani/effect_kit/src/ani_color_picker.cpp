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

#include <string>
#include "ani_color_picker.h"
#include "ani_effect_kit_utils.h"
#include "color.h"
#include "color_picker.h"
#include "effect_errors.h"
#include "effect_utils.h"
#include "hilog/log.h"
#include "pixel_map_taihe_ani.h"
#include "color_picker_napi.h"

#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"


namespace OHOS {
namespace Rosen {

static const std::string ANI_CLASS_COLOR_PICKER = "L@ohos/effectKit/effectKit/ColorPickerInternal;";
static const std::string ANI_CLASS_COLOR = "L@ohos/effectKit/effectKit/ColorInternal;";
constexpr int REGION_COORDINATE_NUM = 4;

struct AniColorPickerAsyncContext {
    AniColorPicker *colorPicker;
    ColorManager::Color color;
    uint32_t status;
    ani_object errorMsg;
    ani_resolver resolver;
    ani_ref callback;
    ani_object thisObj;
    std::shared_ptr<ColorPicker> rColorPicker;

    AniColorPickerAsyncContext()
        : colorPicker(nullptr), color(0u), status(0),
          errorMsg(nullptr), resolver(nullptr), callback(nullptr), thisObj(nullptr),
          rColorPicker(nullptr) {}
};

static ani_object BuildColor(ani_env *env, const ColorManager::Color &color)
{
    ani_object result{};
    ani_object clrRed{};
    ani_object clrGreen{};
    ani_object clrBlue{};
    ani_object clrAlpha{};

    static const char *className = ANI_CLASS_COLOR.c_str();
    
    result = AniEffectKitUtils::CreateAniObject(env, className, nullptr, reinterpret_cast<ani_long>(nullptr));

    ani_int colorRed = static_cast<ani_int>(color.r * 255.0f);
    ani_int colorGreen = static_cast<ani_int>(color.g * 255.0f);
    ani_int colorBlue = static_cast<ani_int>(color.b * 255.0f);
    ani_int colorAlpha = static_cast<ani_int>(color.a * 255.0f);

    ani_variable clrRedVar = (ani_variable)clrRed;
    env->Variable_SetValue_Int(clrRedVar, colorRed);
    env->Object_SetFieldByName_Int(result, "red", colorRed);

    ani_variable clrGreenVar = (ani_variable)clrGreen;
    env->Variable_SetValue_Int(clrGreenVar, colorGreen);
    env->Object_SetFieldByName_Int(result, "green", colorGreen);

    ani_variable clrBlueVar = (ani_variable)clrBlue;
    env->Variable_SetValue_Int(clrBlueVar, colorBlue);
    env->Object_SetFieldByName_Int(result, "blue", colorBlue);

    ani_variable clrAlphaVar = (ani_variable)clrAlpha;
    env->Variable_SetValue_Int(clrAlphaVar, colorAlpha);
    env->Object_SetFieldByName_Int(result, "alpha", colorAlpha);

    return result;
}


ani_object AniColorPicker::GetMainColorSync(ani_env *env, ani_object obj)
{
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[GetMainColorSync] Error1, failed to retrieve ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_) {
        EFFECT_LOG_E("[GetMainColorSync] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetMainColor(color);
    if (errorCode != SUCCESS) {
        EFFECT_LOG_E("[GetMainColorSync] Error3, failed to get main color (error code: %u)", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return BuildColor(env, color);
}

ani_object AniColorPicker::GetLargestProportionColor(ani_env *env, ani_object obj)
{
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[GetLargestProportionColor] Error1, failed to retrieve ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_) {
        EFFECT_LOG_E("[GetLargestProportionColor] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetLargestProportionColor(color);
    if (errorCode != SUCCESS) {
        EFFECT_LOG_E("[GetLargestProportionColor] Error3, "
            "failed to get largest proportion color (error code: %u)", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return BuildColor(env, color);
}

ani_object AniColorPicker::GetTopProportionColors(ani_env *env, ani_object obj, ani_int param)
{
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[GetTopProportionColors] Error1, failed to retrieve ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_) {
        EFFECT_LOG_E("[GetTopProportionColors] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    constexpr int PROPORTION_COLORS_NUM_LIMIT = 10;
    unsigned int colorsNum = static_cast<unsigned int>(std::clamp(param, 0, PROPORTION_COLORS_NUM_LIMIT));
    std::vector<ColorManager::Color> colors;
    colors = thisColorPicker->nativeColorPicker_->GetTopProportionColors(colorsNum);
    ani_class cls;
    if (env->FindClass(ANI_CLASS_COLOR.c_str(), &cls) != ANI_OK) {
        EFFECT_LOG_E("[GetTopProportionColors] Error3, failed to find Color class");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    uint32_t arrLen = std::max(1u, static_cast<uint32_t>(colors.size()));
    ani_array_ref arrayValue = nullptr;
    ani_ref nullRef = nullptr;
    if (env->GetUndefined(&nullRef) != ANI_OK) {
        EFFECT_LOG_E("GetUndefined failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (env->Array_New_Ref(cls, arrLen, nullRef, &arrayValue) != ANI_OK) {
        EFFECT_LOG_E("[GetTopProportionColors] Error4, failed to create array");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    for (uint32_t i = 0; i < arrLen; ++i) {
        ani_object colorValue;
        if (i < colors.size()) {
            colorValue = BuildColor(env, colors[i]);
        } else {
            colorValue = AniEffectKitUtils::CreateAniUndefined(env);
        }
        ani_ref colorRef = static_cast<ani_ref>(colorValue);
        if (env->Array_Set_Ref(arrayValue, i, colorRef) != ANI_OK) {
            EFFECT_LOG_E("[GetTopProportionColors] Error5, failed to set array element");
            return AniEffectKitUtils::CreateAniUndefined(env);
        }
    }
    return static_cast<ani_object>(arrayValue);
}

ani_object AniColorPicker::GetHighestSaturationColor(ani_env *env, ani_object obj)
{
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[GetHighestSaturationColor] Error1, failed to retrieve ColorPicker wrapper.");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_) {
        EFFECT_LOG_E("[GetHighestSaturationColor] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetHighestSaturationColor(color);
    if (errorCode != SUCCESS) {
        EFFECT_LOG_E("[GetHighestSaturationColor] Error3, "
            "failed to get highest saturation color (error code: %u)", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return BuildColor(env, color);
}

ani_object AniColorPicker::GetAverageColor(ani_env *env, ani_object obj)
{
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[GetAverageColor] Error1, failed to retrieve ColorPicker wrapper.");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_) {
        EFFECT_LOG_E("[GetAverageColor] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetAverageColor(color);
    if (errorCode != SUCCESS) {
        EFFECT_LOG_E("GetAverageColor: Error3, GetAverageColor failed! errorCode=%u", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return BuildColor(env, color);
}

ani_boolean AniColorPicker::IsBlackOrWhiteOrGrayColor(ani_env *env, ani_object obj, ani_int colorValue)
{
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[IsBlackOrWhiteOrGrayColor] Error1, failed to retrieve ColorPicker wrapper.");
        return false;
    }
    if (!thisColorPicker->nativeColorPicker_) {
        EFFECT_LOG_E("[IsBlackOrWhiteOrGrayColor] Error2, failed to retrieve native ColorPicker wrapper");
        return false;
    }

    uint32_t color = static_cast<uint32_t>(colorValue);
    ani_boolean rst = thisColorPicker->nativeColorPicker_->IsBlackOrWhiteOrGrayColor(color);
    
    return rst;
}

ani_object AniColorPicker::CreateColorPickerNormal(ani_env *env, ani_object para)
{
    auto colorPicker = std::make_unique<AniColorPicker>();

    auto pixelMap = OHOS::Media::PixelMapTaiheAni::GetNativePixelMap(env, para);
    if (!pixelMap) {
        EFFECT_LOG_E("AniColorPicker: pixelMap is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    colorPicker->srcPixelMap_ = pixelMap;
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    colorPicker->nativeColorPicker_ = ColorPicker::CreateColorPicker(pixelMap, errorCode);
    if (colorPicker->nativeColorPicker_ == nullptr || errorCode != SUCCESS) {
        EFFECT_LOG_E("AniColorPicker: Failed to create ColorPicker, errorCode: %u", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    static const char *className = ANI_CLASS_COLOR_PICKER.c_str();
    const char *methodSig = "J:V";
    return AniEffectKitUtils::CreateAniObject(
        env, className, methodSig, reinterpret_cast<ani_long>(colorPicker.release()));
}

ani_object AniColorPicker::CreateColorPickerWithRegion(ani_env* env, ani_object para, ani_object region)
{
    auto colorPicker = std::make_unique<AniColorPicker>();
    auto pixelMap = OHOS::Media::PixelMapTaiheAni::GetNativePixelMap(env, para);
    if (!pixelMap) {
        EFFECT_LOG_E("AniColorPicker::CreateColorPickerWithRegion: pixelMap is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    colorPicker->srcPixelMap_ = pixelMap;

    ani_int length;
    if (ANI_OK != env->Object_GetPropertyByName_Int(region, "length", &length)) {
        EFFECT_LOG_E("get region length failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    if (int(length) >= REGION_COORDINATE_NUM) {
        for (int i = 0; i < REGION_COORDINATE_NUM; i++) {
            ani_ref doubleValueRef{};
            ani_status status = env->Object_CallMethodByName_Ref(region, "$_get", "i:C{std.core.Object}",
                &doubleValueRef, (ani_int)i);
            ani_double doubleValue;
            status = env->Object_CallMethodByName_Double(static_cast<ani_object>(doubleValueRef),
                "unboxed", ":d", &doubleValue);
            if (status != ANI_OK) {
                EFFECT_LOG_E("[CreateColorPicker] region Object_CallMethodByName_Double failed at i=%d", i);
                return AniEffectKitUtils::CreateAniUndefined(env);
            }
            colorPicker->coordinatesBuffer_[i] = std::clamp<double>(doubleValue, 0.0, 1.0);
        }
    }

    bool coordinatesValid =
        (colorPicker->coordinatesBuffer_[2] > colorPicker->coordinatesBuffer_[0]) &&
        (colorPicker->coordinatesBuffer_[3] > colorPicker->coordinatesBuffer_[1]);
    if (!coordinatesValid) {
        EFFECT_LOG_E("AniColorPicker::CreateColorPickerWithRegion: region must satisfy right > left && bottom > top");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    colorPicker->nativeColorPicker_ = ColorPicker::CreateColorPicker(
        pixelMap, colorPicker->coordinatesBuffer_, errorCode);
    if (colorPicker->nativeColorPicker_ == nullptr || errorCode != SUCCESS) {
        EFFECT_LOG_E("AniColorPicker::CreateColorPickerWithRegion: Failed to create ColorPicker, errorCode: %u",
            errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    static const char* className = ANI_CLASS_COLOR_PICKER.c_str();
    const char* methodSig = "J:V";
    return AniEffectKitUtils::CreateAniObject(env, className, methodSig,
        reinterpret_cast<ani_long>(colorPicker.release()));
}

ani_object AniColorPicker::CreateColorPickerFromPtr(ani_env* env, std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        EFFECT_LOG_E("AniColorPicker::CreateColorPickerFromPtr pixelMap is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    auto aniColorPicker = std::make_unique<AniColorPicker>();
    aniColorPicker->srcPixelMap_ = pixelMap;
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    aniColorPicker->nativeColorPicker_ = ColorPicker::CreateColorPicker(pixelMap, errorCode);
    if (aniColorPicker->nativeColorPicker_ == nullptr || errorCode != SUCCESS) {
        EFFECT_LOG_E("AniColorPicker::CreateColorPickerFromPtr failed to create ColorPicker, errorCode: %u", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    static const char* className = ANI_CLASS_COLOR_PICKER.c_str();
    // ets constructor function, parameter type and return value type
    const char* methodSig = "J:V";
    return AniEffectKitUtils::CreateAniObject(
        env, className, methodSig, reinterpret_cast<ani_long>(aniColorPicker.release()));
}

ani_object AniColorPicker::KitTransferStaticColorPicker(ani_env* env, ani_class cls, ani_object obj)
{
    void *unwrapResult = nullptr;
    if (!arkts_esvalue_unwrap(env, obj, &unwrapResult)) {
        EFFECT_LOG_E("AniColorPicker::KitTransferStaticColorPicker fail to unwrap input");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        EFFECT_LOG_E("AniColorPicker::KitTransferStaticColorPicker unwrapResult is nullptr");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    ColorPickerNapi* napiColorPicker = static_cast<ColorPickerNapi*>(unwrapResult);
    if (napiColorPicker->GetColorPicker()) {
        std::shared_ptr<Media::PixelMap> pixelMap = napiColorPicker->GetColorPicker()->GetScaledPixelMap();
        return static_cast<ani_object>(CreateColorPickerFromPtr(env, pixelMap));
    }
    return nullptr;
}

ani_object AniColorPicker::kitTransferDynamicColorPicker(ani_env* env, ani_class cls, ani_long obj)
{
    AniColorPicker* aniColorPicker = reinterpret_cast<AniColorPicker*>(obj);
    if (aniColorPicker == nullptr) {
        EFFECT_LOG_E("AniColorPicker::kitTransferDynamicColorPicker aniColorPicker is nullptr");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    napi_env napiEnv = {};
    if (!arkts_napi_scope_open(env, &napiEnv)) {
        EFFECT_LOG_E("AniColorPicker::kitTransferDynamicColorPicker napi scope open failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    napi_value napiColorPicker = ColorPickerNapi::CreateColorPickerFromPtr(napiEnv, aniColorPicker->nativeColorPicker_);
    hybridgref ref {};
    if (!hybridgref_create_from_napi(napiEnv, napiColorPicker, &ref)) {
        EFFECT_LOG_E("AniColorPicker::kitTransferDynamicColorPicker create hybridgref failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    ani_object result {};
    if (!hybridgref_get_esvalue(env, ref, &result)) {
        EFFECT_LOG_E("AniColorPicker::kitTransferDynamicColorPicker get esvalue failed");
        hybridgref_delete_from_napi(napiEnv, ref);
        arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!hybridgref_delete_from_napi(napiEnv, ref)) {
        arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
        EFFECT_LOG_E("AniColorPicker::kitTransferDynamicColorPicker delete hybridgref fail");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr)) {
        EFFECT_LOG_E("AniColorPicker::kitTransferDynamicColorPicker napi close scope fail");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return result;
}

ani_status AniColorPicker::Init(ani_env *env)
{
    static const char *className = ANI_CLASS_COLOR_PICKER.c_str();
    ani_class cls;
    if (env->FindClass(className, &cls) != ANI_OK) {
        EFFECT_LOG_E("Not found L@ohos/effectKit/effectKit/ColorPickerInternal");
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function{"getMainColorSyncNative", nullptr,
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetMainColorSync)},
        ani_native_function{"getLargestProportionColorNative", nullptr,
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetLargestProportionColor)},
        ani_native_function{"getTopProportionColorsNative", nullptr,
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetTopProportionColors)},
        ani_native_function{"getHighestSaturationColorNative", nullptr,
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetHighestSaturationColor)},
        ani_native_function{"getAverageColorNative", nullptr,
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetAverageColor)},
        ani_native_function{"isBlackOrWhiteOrGrayColorNative", nullptr,
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::IsBlackOrWhiteOrGrayColor)},
    };
    std::array static_methods = {
        ani_native_function { "kitTransferStaticNative", "Lstd/interop/ESValue;:Lstd/core/Object;",
            reinterpret_cast<void*>(OHOS::Rosen::AniColorPicker::KitTransferStaticColorPicker) },
        ani_native_function { "kitTransferDynamicNative", "J:Lstd/interop/ESValue;",
            reinterpret_cast<void*>(OHOS::Rosen::AniColorPicker::kitTransferDynamicColorPicker) }
    };

    ani_status ret1 = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    ani_status ret2 = env->Class_BindStaticNativeMethods(cls, static_methods.data(), static_methods.size());
    if (ret1 != ANI_OK || ret2 != ANI_OK) {
        EFFECT_LOG_E("Class_BindNativeMethods ret1 : %{public}d", ret1);
        EFFECT_LOG_E("Class_BindNativeMethods ret2 : %{public}d", ret2);
        return ANI_ERROR;
    }
    return ANI_OK;
}

} // namespace Rosen
} // namespace OHOS
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

#include "ani_color_picker.h"
#include <string>
#include "ani_effect_kit_utils.h"
#include "effect_errors.h"
#include "color_picker.h"
#include "color.h"
#include "pixel_map_taihe_ani.h"
#include "hilog/log.h"
#include "effect_utils.h"

namespace OHOS {
namespace Rosen {

static const std::string ANI_CLASS_COLOR_PICKER = "L@ohos/effectKit/effectKit/ColorPickerInternal;";
static const std::string ANI_CLASS_COLOR = "L@ohos/effectKit/effectKit/Color;";
constexpr int REGION_COORDINATE_NUM = 4;

struct AniColorPickerAsyncContext {
    AniColorPicker *colorPicker;
    ColorManager::Color color;
    uint32_t status;
    ani_object errorMsg;
    ani_resolver resolver;
    ani_ref callback;
    ani_object thisObj;
    std::shared_ptr<ColorPicker> rColorPicker = {nullptr};
};

// 统一声明所有static函数，便于维护
static void GetMainColorExecute(ani_env *env, ani_status status, void *data);
static void GetMainColorComplete(ani_env *env, ani_status status, void *data);
static void CommonCallbackRoutine(ani_env *env, AniColorPickerAsyncContext *&asyncContext, const ani_object &valueParam);
static ani_object BuildColor(ani_env *env, const ColorManager::Color &color);

static void GetMainColorExecute(ani_env *env, ani_status status, void *data)
{
    AniColorPickerAsyncContext *context = static_cast<AniColorPickerAsyncContext *>(data);

    if (context->errorMsg != nullptr)
    {
        context->status = ERROR;
        EFFECT_LOG_E("GetMainColorExecute mismatch args");
        return;
    }

    if (context->colorPicker == nullptr || context->rColorPicker == nullptr)
    {
        context->status = ERROR;
        EFFECT_LOG_E("GetMainColorExecute colorPicker or rColorPicker is nullptr");
        return;
    }

    uint32_t errorCode = context->rColorPicker->GetMainColor(context->color);
    context->status = errorCode == SUCCESS ? SUCCESS : ERROR;
}

static void GetMainColorComplete(ani_env *env, ani_status status, void *data)
{
    ani_object res{};
    ani_ref ref;
    env->GetUndefined(&ref);
    res = static_cast<ani_object>(ref);
    AniColorPickerAsyncContext *context = static_cast<AniColorPickerAsyncContext *>(data);
    if (context->errorMsg != nullptr)
    {
        context->status = ERROR;
        EFFECT_LOG_E("GetMainColorComplete mismatch args");
        return;
    }
    if (context->status == SUCCESS)
    {
        res = BuildColor(env, context->color);
    }
    CommonCallbackRoutine(env, context, res);
}

static void CommonCallbackRoutine(ani_env *env, AniColorPickerAsyncContext *&asyncContext, const ani_object &valueParam)
{
    ani_object result[2] = {nullptr, nullptr};
    ani_ref retVal = nullptr;

    {
        ani_ref ref0;
        env->GetUndefined(&ref0);
        result[0] = static_cast<ani_object>(ref0);
    }
    {
        ani_ref ref1;
        env->GetUndefined(&ref1);
        result[1] = static_cast<ani_object>(ref1);
    }

    if (asyncContext->status == SUCCESS)
    {
        result[1] = valueParam;
    }
    else if (asyncContext->errorMsg != nullptr)
    {
        ani_string str;
        env->String_NewUTF8("Internal error", 14U, &str);
        result[0] = static_cast<ani_object>(str);
    }

    if (asyncContext->resolver != nullptr)
    {
        if (asyncContext->status == SUCCESS)
        {
            env->PromiseResolver_Resolve(asyncContext->resolver, result[1]);
        }
        else
        {
            env->PromiseResolver_Reject(asyncContext->resolver, reinterpret_cast<ani_error>(result[0]));
        }
    }
    else
    {
        ani_wref wref;
        env->WeakReference_Create(asyncContext->callback, &wref);
        ani_boolean was_released = 0;
        ani_ref callbackRef = nullptr;
        env->WeakReference_GetReference(wref, &was_released, &callbackRef);
        ani_class cls;
        env->FindClass(ANI_CLASS_COLOR_PICKER.c_str(), &cls);
        env->Class_CallStaticMethod_Ref(cls, nullptr, &retVal, callbackRef, 2, result);
    }

    delete asyncContext;
    asyncContext = nullptr;
}

static ani_object BuildColor(ani_env *env, const ColorManager::Color &color)
{
    ani_object result{};
    ani_object clrRed{};
    ani_object clrGreen{};
    ani_object clrBlue{};
    ani_object clrAlpha{};

    static const char *className = ANI_CLASS_COLOR.c_str();
    // 这里不能用this，需传递nullptr或实际对象指针，若无对象可用nullptr
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

ani_object AniColorPicker::GetMainColor(ani_env *env, ani_object info)
{
    ani_object result{};
    ani_object thisVar = info;
    std::unique_ptr<AniColorPickerAsyncContext> asyncContext = std::make_unique<AniColorPickerAsyncContext>();
    // 获取 nativeColorPicker 指针
    asyncContext->colorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, thisVar);
    // 设置 rColorPicker 指向实际的 ColorPicker 实例
    if (asyncContext->colorPicker != nullptr)
    {
        asyncContext->rColorPicker = asyncContext->colorPicker->nativeColorPicker_;
    }

    // ANI 不支持 CreateAsyncWork，直接同步执行
    GetMainColorExecute(env, ANI_OK, asyncContext.get());
    GetMainColorComplete(env, ANI_OK, asyncContext.get());

    if (asyncContext->status != SUCCESS)
    {
        // 错误处理
        if (asyncContext->callback != nullptr)
        {
            env->GlobalReference_Delete(asyncContext->callback);
        }
        if (asyncContext->resolver != nullptr)
        {
            ani_string errorStr;
            env->String_NewUTF8("fail to get main color", 22, &errorStr);
            ani_error error = reinterpret_cast<ani_error>(errorStr);
            env->PromiseResolver_Reject(asyncContext->resolver, error);
        }
    }
    asyncContext.release();
    return result;
}

ani_object AniColorPicker::GetMainColorSync(ani_env *env, ani_object obj)
{
    // 从 ANI 对象中获取 C++ 实例指针
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker)
    {
        EFFECT_LOG_E("[GetMainColorSync] Error1, failed to retrieve ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_)
    {
        EFFECT_LOG_E("[GetMainColorSync] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    // 调用原生方法获取主色
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetMainColor(color);
    // 根据结果返回对应值
    if (errorCode != SUCCESS)
    {
        EFFECT_LOG_E("[GetMainColorSync] Error3, failed to get main color (error code: %u)", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env); // 失败时返回 undefined
    }
    return BuildColor(env, color); // 创建并返回 Color 对象
}

ani_object AniColorPicker::GetLargestProportionColor(ani_env *env, ani_object obj)
{
    // 从 ANI 对象中获取 C++ 实例指针
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker)
    {
        EFFECT_LOG_E("[GetLargestProportionColor] Error1, failed to retrieve ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_)
    {
        EFFECT_LOG_E("[GetLargestProportionColor] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    // 调用原生方法获取占比最大的颜色
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetLargestProportionColor(color);
    // 根据结果返回对应值
    if (errorCode != SUCCESS)
    {
        EFFECT_LOG_E("[GetLargestProportionColor] Error3, failed to get largest proportion color (error code: %u)", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env); // 失败时返回 undefined
    }
    return BuildColor(env, color); // 创建并返回 Color 对象
}

ani_object AniColorPicker::GetTopProportionColors(ani_env *env, ani_object obj, ani_double param)
{
    // 获取实例
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker)
    {
        EFFECT_LOG_E("[GetTopProportionColors] Error1, failed to retrieve ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_)
    {
        EFFECT_LOG_E("[GetTopProportionColors] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    // 处理颜色数量（限制范围）
    constexpr double PROPORTION_COLORS_NUM_LIMIT = 256.0;
    unsigned int colorsNum = static_cast<unsigned int>(std::clamp(param, 0.0, PROPORTION_COLORS_NUM_LIMIT));
    // 获取颜色列表 - 修正方法调用
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
    env->GetUndefined(&nullRef);
    if (env->Array_New_Ref(cls, arrLen, nullRef, &arrayValue) != ANI_OK)
    {
        EFFECT_LOG_E("[GetTopProportionColors] Error4, failed to create array");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    // 填充数组
    for (uint32_t i = 0; i < arrLen; ++i)
    {
        ani_object colorValue;
        if (i < colors.size()) {
            colorValue = BuildColor(env, colors[i]);
        } else {
            colorValue = AniEffectKitUtils::CreateAniUndefined(env);
        }
        ani_ref colorRef = static_cast<ani_ref>(colorValue);
        if (env->Array_Set_Ref(arrayValue, i, colorRef) != ANI_OK)
        {
            EFFECT_LOG_E("[GetTopProportionColors] Error5, failed to set array element");
            return AniEffectKitUtils::CreateAniUndefined(env);
        }
    }
    return static_cast<ani_object>(arrayValue);
}

ani_object AniColorPicker::GetHighestSaturationColor(ani_env *env, ani_object obj)
{
    // 从 ANI 对象中获取 C++ 实例指针
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[GetHighestSaturationColor] Error1, failed to retrieve ColorPicker wrapper.");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_) {
        EFFECT_LOG_E("[GetHighestSaturationColor] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    // 调用原生方法获取饱和度最高的颜色
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetHighestSaturationColor(color);
    // 根据结果返回对应值
    if (errorCode != SUCCESS) {
        EFFECT_LOG_E("[GetHighestSaturationColor] Error3, failed to get highest saturation color (error code: %u)", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env); // 失败时返回 undefined
    }
    return BuildColor(env, color); // 创建并返回 Color 对象
}

ani_object AniColorPicker::GetAverageColor(ani_env *env, ani_object obj)
{
    EFFECT_LOG_E("[EFFECT_KIT_DEBUG] GetAverageColor called");
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[GetAverageColor] Error1, failed to retrieve ColorPicker wrapper.");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    if (!thisColorPicker->nativeColorPicker_) {
        EFFECT_LOG_E("[GetAverageColor] Error2, failed to retrieve native ColorPicker wrapper");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    // 调用原生方法计算平均颜色
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    ColorManager::Color color;
    errorCode = thisColorPicker->nativeColorPicker_->GetAverageColor(color);
    if (errorCode != SUCCESS) {
        EFFECT_LOG_E("GetAverageColor: Error3, GetAverageColor failed! errorCode=%u", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    return BuildColor(env, color);
}

ani_boolean AniColorPicker::IsBlackOrWhiteOrGrayColor(ani_env *env, ani_object obj, ani_double colorValue)
{
    AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
    if (!thisColorPicker) {
        EFFECT_LOG_E("[IsBlackOrWhiteOrGrayColor] Error1, failed to retrieve ColorPicker wrapper.");
        return false; 
    }
    if (!thisColorPicker->nativeColorPicker_)
    {
        EFFECT_LOG_E("[IsBlackOrWhiteOrGrayColor] Error2, failed to retrieve native ColorPicker wrapper");
        return false; 
    }

    uint32_t color = static_cast<uint32_t>(colorValue);
    ani_boolean rst = thisColorPicker->nativeColorPicker_->IsBlackOrWhiteOrGrayColor(color);
    
    return rst;
}

ani_object AniColorPicker::createColorPicker1(ani_env *env, ani_object para)
{
    EFFECT_LOG_E("[EFFECT_KIT_DEBUG] createColorPicker1 called");
    auto colorPicker = std::make_unique<AniColorPicker>();

    auto pixelMap = OHOS::Media::PixelMapTaiheAni::GetNativePixelMap(env, para);
    if (!pixelMap)
    {
        EFFECT_LOG_E("AniColorPicker: pixelMap is null");
        return AniEffectKitUtils::CreateAniUndefined(env); // 返回 undefined 给 ETS 层
    }
    colorPicker->srcPixelMap_ = pixelMap;
    // 创建底层的 ColorPicker 实例
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    colorPicker->nativeColorPicker_ = ColorPicker::CreateColorPicker(pixelMap, errorCode);
    
    // 检查 ColorPicker 创建是否成功
    if (colorPicker->nativeColorPicker_ == nullptr || errorCode != SUCCESS)
    {
        EFFECT_LOG_E("AniColorPicker: Failed to create ColorPicker, errorCode: %u", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    EFFECT_LOG_E("[EFFECT_KIT_DEBUG] createColorPicker1: pixelMap pointer");

    static const char *className = ANI_CLASS_COLOR_PICKER.c_str(); // 根据 ETS 层 className 填写
    const char *methodSig = "J:V";
    return AniEffectKitUtils::CreateAniObject(
        env, className, methodSig, reinterpret_cast<ani_long>(colorPicker.release()));
}

ani_object AniColorPicker::createColorPicker2(ani_env* env, ani_object para, ani_object region)
{
    auto colorPicker = std::make_unique<AniColorPicker>();
    auto pixelMap = OHOS::Media::PixelMapTaiheAni::GetNativePixelMap(env, para);
    if (!pixelMap) {
        EFFECT_LOG_E("AniColorPicker::createColorPicker2: pixelMap is null");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    colorPicker->srcPixelMap_ = pixelMap;

    ani_double length;
    if (ANI_OK != env->Object_GetPropertyByName_Double(region, "length", &length)) {
        EFFECT_LOG_E("get region length failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    if (int(length) >= REGION_COORDINATE_NUM) {
        for (int i = 0; i < REGION_COORDINATE_NUM; i++) {
            ani_ref doubleValueRef{}; 
            ani_status status = env->Object_CallMethodByName_Ref(region, "$_get", "i:C{std.core.Object}", &doubleValueRef, (ani_int)i);
            ani_double doubleValue;
            status = env->Object_CallMethodByName_Double(static_cast<ani_object>(doubleValueRef), "unboxed", ":d", &doubleValue);
            if (status != ANI_OK) {
                EFFECT_LOG_E("[CreateColorPicker] region Object_CallMethodByName_Double failed at i=%d", i);
                return AniEffectKitUtils::CreateAniUndefined(env);
            }
            colorPicker->coordinatesBuffer[i] = std::clamp<double>(doubleValue, 0.0, 1.0);
        }
    }

    bool coordinatesValid =
        (colorPicker->coordinatesBuffer[2] > colorPicker->coordinatesBuffer[0]) &&
        (colorPicker->coordinatesBuffer[3] > colorPicker->coordinatesBuffer[1]);
    if (!coordinatesValid) {
        EFFECT_LOG_E("AniColorPicker::createColorPicker2: region must satisfy right > left && bottom > top");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    // 新增：直接创建底层 ColorPicker 实例
    uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
    colorPicker->nativeColorPicker_ = ColorPicker::CreateColorPicker(
        pixelMap, colorPicker->coordinatesBuffer, errorCode);
    if (colorPicker->nativeColorPicker_ == nullptr || errorCode != SUCCESS) {
        EFFECT_LOG_E("AniColorPicker::createColorPicker2: Failed to create ColorPicker, errorCode: %u", errorCode);
        return AniEffectKitUtils::CreateAniUndefined(env);
    }

    static const char* className = ANI_CLASS_COLOR_PICKER.c_str();
    const char* methodSig = "J:V";
    return AniEffectKitUtils::CreateAniObject(env, className, methodSig,
        reinterpret_cast<ani_long>(colorPicker.release()));
}

ani_status AniColorPicker::Init(ani_env *env)
{
    static const char *className = ANI_CLASS_COLOR_PICKER.c_str();
    ani_class cls;
    if (env->FindClass(className, &cls) != ANI_OK)
    {
        EFFECT_LOG_E("Not found L@ohos/effectKit/effectKit/ColorPickerInternal");
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        /*ani_native_function{"getMainColorNative", ":L@ohos/effectKit/effectKit/Color;",
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetMainColor)},
        ani_native_function{"getMainColorSyncNative", ":L@ohos/effectKit/effectKit/Color;",
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetMainColorSync)},
        ani_native_function{"getLargestProportionColorNative", ":L@ohos/effectKit/effectKit/Color;",
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetLargestProportionColor)},
        ani_native_function{"getTopProportionColorsNative", "D:[Lstd/core/Object;",
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetTopProportionColors)},
        ani_native_function{"getHighestSaturationColorNative", ":L@ohos/effectKit/effectKit/Color;",
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetHighestSaturationColor)},
        ani_native_function{"getAverageColorNative", ":L@ohos/effectKit/effectKit/Color;",
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetAverageColor)},
        ani_native_function{"isBlackOrWhiteOrGrayColorNative", "D:Z",
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::IsBlackOrWhiteOrGrayColor)},
                            */
        ani_native_function{"getMainColorNative", nullptr,
                            reinterpret_cast<void *>(OHOS::Rosen::AniColorPicker::GetMainColor)},
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
    ani_status ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK)
    {
        EFFECT_LOG_I("Class_BindNativeMethods failed: %{public}d", ret);
        return ANI_ERROR;
    }
    return ANI_OK;
}

} // namespace Rosen
} // namespace OHOS
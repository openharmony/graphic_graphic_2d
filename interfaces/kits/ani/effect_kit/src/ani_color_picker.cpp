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

namespace OHOS
{
    namespace Rosen
    {

        static const std::string ANI_CLASS_COLOR_PICKER = "L@ohos/effectKit/effectKit/ColorPickerInternal;";

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

        static void GetMainColorExecute(ani_env *env, ani_status status, void *data);
        static void GetMainColorComplete(ani_env *env, ani_status status, void *data);
        static void CommonCallbackRoutine(ani_env *env, AniColorPickerAsyncContext *&asyncContext,
                                         const ani_object &valueParam);
        static ani_object BuildColor(ani_env *env, const ColorManager::Color &color);

        static void GetMainColorExecute(ani_env *env, ani_status status, void *data)
        {
            AniColorPickerAsyncContext *context = static_cast<AniColorPickerAsyncContext *>(data);

            if (context->errorMsg != nullptr) {
                context->status = ERROR;
                EFFECT_LOG_E("GetMainColorExecute mismatch args");
                return;
            }
            if (context->colorPicker == nullptr) {
                context->status = ERROR;
                EFFECT_LOG_E("GetMainColorExecute colorPicker is nullptr");
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
            if (context->errorMsg != nullptr) {
                context->status = ERROR;
                EFFECT_LOG_E("GetMainColorComplete mismatch args");
                return;
            }
            if (context->status == SUCCESS) {
                res = BuildColor(env, context->color);
            }
            CommonCallbackRoutine(env, context, res);
        }

        static void CommonCallbackRoutine(ani_env *env, AniColorPickerAsyncContext *&asyncContext,
                                         const ani_object &valueParam)
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

            if (asyncContext->status == SUCCESS) {
                result[1] = valueParam;
            } else if (asyncContext->errorMsg != nullptr) {
                ani_string str;
                env->String_NewUTF8("Internal error", 14U, &str);
                result[0] = static_cast<ani_object>(str);
            }

            if (asyncContext->resolver != nullptr) {
                if (asyncContext->status == SUCCESS) {
                    env->PromiseResolver_Resolve(asyncContext->resolver, result[1]);
                } else {
                    env->PromiseResolver_Reject(asyncContext->resolver, reinterpret_cast<ani_error>(result[0]));
                }
            } else {
                ani_wref wref;
                env->WeakReference_Create(asyncContext->callback, &wref);
                ani_boolean was_released = 0;
                ani_ref callbackRef = nullptr;
                env->WeakReference_GetReference(wref, &was_released, &callbackRef);
                ani_class cls;
                env->FindClass(ANI_CLASS_COLOR_PICKER.c_str(), &cls);
                ani_int resultCode = 2;
                env->Class_CallStaticMethod_Ref(cls, nullptr, &retVal, callbackRef, resultCode, result);
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

            static const char *className = ANI_CLASS_COLOR_PICKER.c_str();
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
            ani_long nativePtr;
            env->Object_GetFieldByName_Long(thisVar, "nativeColorPicker", &nativePtr);
            asyncContext->colorPicker = reinterpret_cast<AniColorPicker *>(nativePtr);
            // 设置 rColorPicker 指向实际的 ColorPicker 实例
            if (asyncContext->colorPicker != nullptr)
            {
                asyncContext->rColorPicker = asyncContext->colorPicker->nativeColorPicker_;
            }
            // 检查指针有效性
            if (asyncContext->colorPicker == nullptr || asyncContext->rColorPicker == nullptr) {
                EFFECT_LOG_E("GetMainColor: nativeColorPicker 为空");
                return result;
            }

            if (asyncContext->status != SUCCESS) {
                // 错误处理
                if (asyncContext->callback != nullptr) {
                    env->GlobalReference_Delete(asyncContext->callback);
                }
                if (asyncContext->resolver != nullptr) {
                    ani_string errorStr;
                    ani_int errorCode = 22;
                    env->String_NewUTF8("fail to get main color", errorCode, &errorStr);
                    ani_error error = reinterpret_cast<ani_error>(errorStr);
                    env->PromiseResolver_Reject(asyncContext->resolver, error);
                }
            }
            asyncContext.release();
            return result;
        }

        ani_object AniColorPicker::GetMainColorSync(ani_env *env, ani_object obj)
        {
            ani_object retVal{};
            AniColorPicker *colorPicker = nullptr;
            ani_long nativePtr;
            if (ANI_OK != env->Object_GetFieldByName_Long(obj, "nativeColorPicker", &nativePtr)) {
                EFFECT_LOG_E("GetMainColorSync: 获取 nativeColorPicker 失败");
                return retVal;
            }
            colorPicker = reinterpret_cast<AniColorPicker *>(nativePtr);

            if (colorPicker == nullptr) {
                EFFECT_LOG_E("GetMainColorSync: nativeColorPicker 为空");
                return retVal;
            }
            uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
            ColorManager::Color color;
            // 调用底层 ColorPicker 的 GetMainColor 方法
            if (colorPicker->nativeColorPicker_) {
                errorCode = colorPicker->nativeColorPicker_->GetMainColor(color);
            }
            if (errorCode == SUCCESS) {
                retVal = BuildColor(env, color);
            } else {
                {
                    ani_ref refRet;
                    env->GetUndefined(&refRet);
                    retVal = static_cast<ani_object>(refRet);
                }
            }

            return retVal;
        }

        ani_object AniColorPicker::GetLargestProportionColor(ani_env *env, ani_object obj)
        {
            // 从 ANI 对象中获取 C++ 实例指针
            AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
            if (!thisColorPicker) {
                EFFECT_LOG_E("Failed to retrieve native ColorPicker instance");
                return AniEffectKitUtils::CreateAniUndefined(env);
            }
            // 调用原生方法获取占比最大的颜色
            uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
            ColorManager::Color color;
            if (thisColorPicker->nativeColorPicker_) {
                errorCode = thisColorPicker->nativeColorPicker_->GetLargestProportionColor(color);
            }
            // 根据结果返回对应值
            if (errorCode == SUCCESS) {
                return BuildColor(env, color); // 创建并返回 Color 对象
            } else {
                EFFECT_LOG_E("Failed to get largest proportion color (error code: %u)", errorCode);
                return AniEffectKitUtils::CreateAniUndefined(env); // 失败时返回 undefined
            }
        }

        ani_object AniColorPicker::GetTopProportionColors(ani_env *env, ani_object obj, ani_double param)
        {
            // 获取实例（使用已实现的 GetColorPickerFromEnv）
            AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
            if (!thisColorPicker) {
                EFFECT_LOG_E("Failed to retrieve native ColorPicker instance");
                return nullptr;
            }
            // 处理颜色数量（限制范围）
            constexpr double PROPORTION_COLORS_NUM_LIMIT = 256.0;
            unsigned int colorsNum = static_cast<unsigned int>(std::clamp(param, 0.0, PROPORTION_COLORS_NUM_LIMIT));
            // 获取颜色列表 - 修正方法调用
            std::vector<ColorManager::Color> colors;
            if (thisColorPicker->nativeColorPicker_) {
                colors = thisColorPicker->nativeColorPicker_->GetTopProportionColors(colorsNum);
            }
            // 创建数组（允许空数组）
            ani_array_ref arrayValue = nullptr;
            ani_ref nullRef = nullptr;
            env->GetUndefined(&nullRef);
            if (env->Array_New_Ref(nullptr, static_cast<uint32_t>(colors.size()), nullRef, &arrayValue) != ANI_OK) {
                EFFECT_LOG_E("Failed to create array");
                return nullptr;
            }
            // 填充数组
            for (uint32_t i = 0; i < static_cast<uint32_t>(colors.size()); ++i) {
                ani_object colorValue = BuildColor(env, colors[i]);
                ani_ref colorRef = static_cast<ani_ref>(colorValue);
                if (env->Array_Set_Ref(arrayValue, i, colorRef) != ANI_OK) {
                    EFFECT_LOG_E("Failed to set array element at index %u", i);
                }
            }
            return static_cast<ani_object>(arrayValue);
        }

        ani_object AniColorPicker::GetHighestSaturationColor(ani_env *env, ani_object obj)
        {
            // 从 ANI 对象中获取 C++ 实例指针
            AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
            if (!thisColorPicker) {
                EFFECT_LOG_E("Failed to retrieve native ColorPicker instance");
                return AniEffectKitUtils::CreateAniUndefined(env);
            }
            // 调用原生方法获取饱和度最高的颜色
            uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
            ColorManager::Color color;
            if (thisColorPicker->nativeColorPicker_) {
                errorCode = thisColorPicker->nativeColorPicker_->GetHighestSaturationColor(color);
            }
            // 根据结果返回对应值
            if (errorCode == SUCCESS) {
                return BuildColor(env, color); // 创建并返回 Color 对象
            } else {
                EFFECT_LOG_E("Failed to get highest saturation color (error code: %u)", errorCode);
                return AniEffectKitUtils::CreateAniUndefined(env); // 失败时返回 undefined
            }
        }

        ani_object AniColorPicker::GetAverageColor(ani_env *env, ani_object obj)
        {
            // 从 ANI 对象中获取 C++ 实例指针
            AniColorPicker *thisColorPicker = AniEffectKitUtils::GetColorPickerFromEnv(env, obj);
            if (!thisColorPicker) {
                EFFECT_LOG_E("Failed to retrieve native ColorPicker instance");
                return AniEffectKitUtils::CreateAniUndefined(env);
            }
            // 调用原生方法计算平均颜色
            uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
            ColorManager::Color color;
            if (thisColorPicker->nativeColorPicker_) {
                errorCode = thisColorPicker->nativeColorPicker_->GetAverageColor(color);
            }
            // 根据结果返回对应值
            if (errorCode == SUCCESS) {
                return BuildColor(env, color); // 创建并返回 Color 对象
            } else {
                EFFECT_LOG_E("Failed to calculate average color (error code: %u)", errorCode);
                return AniEffectKitUtils::CreateAniUndefined(env); // 失败时返回 undefined
            }
        }

        ani_boolean AniColorPicker::IsBlackOrWhiteOrGrayColor(ani_env *env, ani_object obj, ani_double colorValue)
        {
            AniColorPicker *colorPicker{};
            ani_long nativePtr;
            if (ANI_OK != env->Object_GetFieldByName_Long(obj, "nativeColorPicker", &nativePtr)) {
                EFFECT_LOG_E("IsBlackOrWhiteOrGrayColor: 获取 nativeColorPicker 失败");
                ani_boolean res = false;
                return res;
            }
            colorPicker = reinterpret_cast<AniColorPicker *>(nativePtr);

            if (colorPicker == nullptr) {
                EFFECT_LOG_E("IsBlackOrWhiteOrGrayColor: nativeColorPicker 为空");
                ani_boolean res = false;
                return res;
            }
            uint32_t color = static_cast<uint32_t>(colorValue);

            ani_boolean rst = false;
            if (colorPicker->nativeColorPicker_) {
                rst = colorPicker->nativeColorPicker_->IsBlackOrWhiteOrGrayColor(color);
            }

            return rst;
        }

        ani_object AniColorPicker::createColorPicker1(ani_env *env, ani_object para)
        {
            auto colorPicker = std::make_unique<AniColorPicker>();
            auto pixelMap = OHOS::Media::PixelMapTaiheAni::GetNativePixelMap(env, para);
            if (!pixelMap) {
                EFFECT_LOG_E("AniColorPicker: pixelMap is null");
                return AniEffectKitUtils::CreateAniUndefined(env); // 返回 undefined 给 ETS 层
            }
            colorPicker->srcPixelMap_ = pixelMap;
            // 创建底层的 ColorPicker 实例
            uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
            colorPicker->nativeColorPicker_ = ColorPicker::CreateColorPicker(pixelMap, errorCode);
            static const char *className = ANI_CLASS_COLOR_PICKER.c_str(); // 根据 ETS 层 className 填写
            const char *methodSig = "J:V";
            return AniEffectKitUtils::CreateAniObject(
                env, className, methodSig, reinterpret_cast<ani_long>(colorPicker.release()));
        }

        ani_object AniColorPicker::createColorPicker2(ani_env *env, ani_object para, ani_object region)
        {
            auto colorPicker = std::make_unique<AniColorPicker>();
            auto pixelMap = OHOS::Media::PixelMapTaiheAni::GetNativePixelMap(env, para);
            if (!pixelMap) {
                EFFECT_LOG_E("AniColorPicker::createColorPicker2: pixelMap is null");
                return AniEffectKitUtils::CreateAniUndefined(env);
            }
            colorPicker->srcPixelMap_ = pixelMap;

            ani_double length = 0;
            if (ANI_OK != env->Object_GetPropertyByName_Double(region, "length", &length)) {
                EFFECT_LOG_E("AniColorPicker::createColorPicker2: get region length failed");
                return AniEffectKitUtils::CreateAniUndefined(env);
            }

            // 读取 region 数组值并赋值到 coordinatesBuffer
            int matrixLen = REGION_COORDINATE_NUM;
            std::vector<double> coordinatesBuffer(matrixLen);
            for (int i = 0; i < int(length) && i < matrixLen; ++i) {
                ani_float floatValue = 0.0;
                if (ANI_OK != env->Object_CallMethodByName_Float(region, "$__get", "I:F", &floatValue, (ani_int)i)) {
                    EFFECT_LOG_E("AniColorPicker::createColorPicker2: region[$_get] failed at index %d", i);
                    return AniEffectKitUtils::CreateAniUndefined(env);
                }
                coordinatesBuffer[i] = std::clamp<double>(floatValue, 0.0, 1.0); // 限制在合法范围
            }
            for (int regionIndex = 0; regionIndex < REGION_COORDINATE_NUM; regionIndex++) {
                colorPicker->coordinatesBuffer[regionIndex] = coordinatesBuffer[regionIndex];
            }

            bool coordinatesValid =
                (coordinatesBuffer[2] > coordinatesBuffer[0]) &&
                (coordinatesBuffer[3] > coordinatesBuffer[1]);
            if (!coordinatesValid) {
                EFFECT_LOG_E("AniColorPicker::createColorPicker2: region must satisfy right > left && bottom > top");
                return AniEffectKitUtils::CreateAniUndefined(env);
            }

            // 创建底层的 ColorPicker 实例，使用region参数
            uint32_t errorCode = ERR_EFFECT_INVALID_VALUE;
            colorPicker->nativeColorPicker_ = ColorPicker::CreateColorPicker(pixelMap, coordinatesBuffer.data(), errorCode);

            static const char *className = ANI_CLASS_COLOR_PICKER.c_str();
            const char *methodSig = "J:V";
            return AniEffectKitUtils::CreateAniObject(env, className, methodSig,
                                                      reinterpret_cast<ani_long>(colorPicker.release()));
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
                ani_native_function{"getMainColorNative", ":L@ohos/effectKit/effectKit/Color;",
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
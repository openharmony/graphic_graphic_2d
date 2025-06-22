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

#include "ani_uieffect.h"
namespace OHOS {
namespace Rosen {
ani_status AniEffect::BindVisualEffectMethod(ani_env* env)
{
    static const char* visualEffectClassName = "L@ohos/graphics/uiEffect/uiEffect/VisualEffectInternal;";
    ani_class visualEffectClass;
    if (env->FindClass(visualEffectClassName, &visualEffectClass) != ANI_OK) {
        UIEFFECT_LOG_E("Not found %{public}s", visualEffectClassName);
        return ANI_NOT_FOUND;
    };
    std::array visualEffectMethods = {
        ani_native_function { "backgroundColorBlenderNative",
            "L@ohos/graphics/uiEffect/uiEffect/BrightnessBlender;:L@ohos/graphics/uiEffect/uiEffect/VisualEffect;",
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::BackgroundColorBlender) },
    };
    ani_status ret = env->Class_BindNativeMethods(visualEffectClass,
        visualEffectMethods.data(), visualEffectMethods.size());
    if (ret != ANI_OK) {
        UIEFFECT_LOG_E("Class_BindNativeMethods failed: %{public}d", ret);
        return ANI_ERROR;
    };
    return ret;
}

ani_status AniEffect::BindFilterMethod(ani_env* env)
{
    static const char* filterClassName = "L@ohos/graphics/uiEffect/uiEffect/FilterInternal;";
    ani_class filterClass;
    if (env->FindClass(filterClassName, &filterClass) != ANI_OK) {
        UIEFFECT_LOG_E("Not found %{public}s", filterClassName);
        return ANI_NOT_FOUND;
    };
    std::array filterMethods = {
        ani_native_function { "pixelStretchNative", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::PixelStretch) },
        ani_native_function { "blurNative", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::Blur) },
        ani_native_function { "flyInFlyOutEffectNative", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::FlyInFlyOutEffect) },
        ani_native_function { "waterRippleNative", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::WaterRipple) },
        ani_native_function { "distortNative", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::Distort) },
    };
    ani_status ret = env->Class_BindNativeMethods(filterClass, filterMethods.data(), filterMethods.size());
    if (ret != ANI_OK) {
        UIEFFECT_LOG_E("Class_BindNativeMethods failed: %{public}d", ret);
        return ANI_ERROR;
    };
    return ret;
}

ani_object AniEffect::CreateAniObject(ani_env* env, std::string name, const char* signature, ani_long addr)
{
    ani_class cls;
    if (env->FindClass(name.c_str(), &cls) != ANI_OK) {
        UIEFFECT_LOG_E("not found '%{public}s'", name.c_str());
        return {};
    };
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", signature, &ctor) != ANI_OK) {
        UIEFFECT_LOG_E("get ctor failed '%{public}s'", name.c_str());
        return {};
    };
    ani_object obj = {};
    if (env->Object_New(cls, ctor, &obj, addr) != ANI_OK) {
        UIEFFECT_LOG_E("create object failed '%{public}s'", name.c_str());
        return obj;
    };
    UIEFFECT_LOG_E("create object success '%{public}s'", name.c_str());
    return obj;
}

bool CheckCreateBrightnessBlender(ani_env* env, ani_object para_obj, ani_object& blender_obj)
{
    ani_double cubicRateAni;
    ani_double quadraticRateAni;
    ani_double linearRateAni;
    ani_double degreeAni;
    ani_double saturationAni;
    ani_double fractionAni;
    ani_ref positiveCoefficientAni;
    ani_ref negativeCoefficientAni;
    if ((env->Object_GetPropertyByName_Double(para_obj, "cubicRate", &cubicRateAni) != ANI_OK) ||
        (env->Object_GetPropertyByName_Double(para_obj, "quadraticRate", &quadraticRateAni) != ANI_OK) ||
        (env->Object_GetPropertyByName_Double(para_obj, "linearRate", &linearRateAni) != ANI_OK) ||
        (env->Object_GetPropertyByName_Double(para_obj, "degree", &degreeAni) != ANI_OK) ||
        (env->Object_GetPropertyByName_Double(para_obj, "saturation", &saturationAni) != ANI_OK) ||
        (env->Object_GetPropertyByName_Double(para_obj, "fraction", &fractionAni) != ANI_OK) ||
        (env->Object_GetPropertyByName_Ref(para_obj, "positiveCoefficient", &positiveCoefficientAni) != ANI_OK) ||
        (env->Object_GetPropertyByName_Ref(para_obj, "negativeCoefficient", &negativeCoefficientAni) != ANI_OK)) {
        UIEFFECT_LOG_E("Input para is not BrightnessBlenderParam, some property cannot be found");
        return false;
    };
    if ((env->Object_SetPropertyByName_Double(blender_obj, "cubicRate", cubicRateAni) != ANI_OK) ||
        (env->Object_SetPropertyByName_Double(blender_obj, "quadraticRate", quadraticRateAni) != ANI_OK) ||
        (env->Object_SetPropertyByName_Double(blender_obj, "linearRate", linearRateAni) != ANI_OK) ||
        (env->Object_SetPropertyByName_Double(blender_obj, "degree", degreeAni) != ANI_OK) ||
        (env->Object_SetPropertyByName_Double(blender_obj, "saturation", saturationAni) != ANI_OK) ||
        (env->Object_SetPropertyByName_Double(blender_obj, "fraction", fractionAni) != ANI_OK) ||
        (env->Object_SetPropertyByName_Ref(blender_obj, "positiveCoefficient", positiveCoefficientAni) != ANI_OK) ||
        (env->Object_SetPropertyByName_Ref(blender_obj, "negativeCoefficient", negativeCoefficientAni) != ANI_OK)) {
        UIEFFECT_LOG_E("Cannot set all property for BrightnessBlender, some property cannot be found");
        return false;
    };
    return true;
}

void AniEffect::ParseBrightnessBlender(ani_env* env, ani_object para_obj, std::shared_ptr<BrightnessBlender>& blender)
{
    ani_double cubicRateAni;
    ani_double quadraticRateAni;
    ani_double linearRateAni;
    ani_double degreeAni;
    ani_double saturationAni;
    ani_double fractionAni;
    ani_ref positiveCoefficientAni;
    ani_ref negativeCoefficientAni;
    env->Object_GetPropertyByName_Double(para_obj, "cubicRate", &cubicRateAni);
    env->Object_GetPropertyByName_Double(para_obj, "quadraticRate", &quadraticRateAni);
    env->Object_GetPropertyByName_Double(para_obj, "linearRate", &linearRateAni);
    env->Object_GetPropertyByName_Double(para_obj, "degree", &degreeAni);
    env->Object_GetPropertyByName_Double(para_obj, "saturation", &saturationAni);
    env->Object_GetPropertyByName_Double(para_obj, "fraction", &fractionAni);
    env->Object_GetPropertyByName_Ref(para_obj, "positiveCoefficient", &positiveCoefficientAni);
    env->Object_GetPropertyByName_Ref(para_obj, "negativeCoefficient", &negativeCoefficientAni);
    const auto positiveCoefficientAniArray = reinterpret_cast<ani_array_double>(positiveCoefficientAni);
    const auto negativeCoefficientAniArray = reinterpret_cast<ani_array_double>(negativeCoefficientAni);
    ani_double posCoefNativeBuffer[3U] = { 0.0 };
    ani_double negCoefNativeBuffer[3U] = { 0.0 };
    const ani_size offset3 = 0;
    const ani_size len3 = 5;
    env->Array_GetRegion_Double(positiveCoefficientAniArray, offset3, len3, posCoefNativeBuffer);
    env->Array_GetRegion_Double(negativeCoefficientAniArray, offset3, len3, negCoefNativeBuffer);
    const int xIndex = 0;
    const int yIndex = 1;
    const int zIndex = 2;
    Vector3f posCoefNative(posCoefNativeBuffer[xIndex], posCoefNativeBuffer[yIndex], posCoefNativeBuffer[zIndex]);
    Vector3f negCoefNative(negCoefNativeBuffer[xIndex], negCoefNativeBuffer[yIndex], negCoefNativeBuffer[zIndex]);
    blender->SetCubicRate(static_cast<float>(cubicRateAni));
    blender->SetQuadRate(static_cast<float>(quadraticRateAni));
    blender->SetLinearRate(static_cast<float>(linearRateAni));
    blender->SetDegree(static_cast<float>(degreeAni));
    blender->SetSaturation(static_cast<float>(saturationAni));
    blender->SetFraction(static_cast<float>(fractionAni));
    blender->SetPositiveCoeff(posCoefNative);
    blender->SetNegativeCoeff(negCoefNative);
}

ani_object AniEffect::CreateEffect(ani_env* env)
{
    ani_object retVal {};
    auto effectObj = std::make_unique<VisualEffect>();
    retVal = CreateAniObject(env, ANI_UIEFFECT_VISUAL_EFFECT, nullptr, reinterpret_cast<ani_long>(effectObj.release()));
    return retVal;
}

ani_object AniEffect::CreateBrightnessBlender(ani_env* env, ani_object para)
{
    ani_object retVal {};
    ani_class cls;
    if (env->FindClass(ANI_UIEFFECT_BRIGHTNESS_BLENDER.c_str(), &cls) != ANI_OK) {
        UIEFFECT_LOG_E("not found '%{public}s'", ANI_UIEFFECT_BRIGHTNESS_BLENDER.c_str());
        return {};
    };
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor) != ANI_OK) {
        UIEFFECT_LOG_E("get ctor failed '%{public}s'", ANI_UIEFFECT_BRIGHTNESS_BLENDER.c_str());
        return {};
    };
    if (env->Object_New(cls, ctor, &retVal) != ANI_OK) {
        UIEFFECT_LOG_E("create object failed '%{public}s'", ANI_UIEFFECT_BRIGHTNESS_BLENDER.c_str());
        return {};
    };
    if (!CheckCreateBrightnessBlender(env, para, retVal)) {
        UIEFFECT_LOG_E("EffectNapi  CheckCreateBrightnessBlender failed.");
        return {};
    };
    return retVal;
}

ani_object AniEffect::BackgroundColorBlender(ani_env* env, ani_object obj, ani_object para)
{
    ani_object retVal {};
    auto blender = std::make_shared<BrightnessBlender>();
    ParseBrightnessBlender(env, para, blender);

    auto bgColorEffectPara = std::make_shared<BackgroundColorEffectPara>();
    bgColorEffectPara->SetBlender(blender);
    VisualEffect* effectObj = nullptr;
    ani_long nativeObj;
    if (env->Object_GetFieldByName_Long(obj, "visualEffectNativeObj", &nativeObj) != ANI_OK) {
        UIEFFECT_LOG_E("get generator visualEffectNativeObj failed");
        return retVal;
    };
    effectObj = reinterpret_cast<VisualEffect*>(nativeObj);
    effectObj->AddPara(bgColorEffectPara);
    retVal = CreateAniObject(env, ANI_UIEFFECT_VISUAL_EFFECT, nullptr, reinterpret_cast<ani_long>(effectObj));
    return retVal;
}

ani_object AniEffect::CreateFilter(ani_env* env)
{
    ani_object retVal {};
    auto filterObj = std::make_unique<Filter>();
    retVal = CreateAniObject(env, ANI_UIEFFECT_FILTER, nullptr, reinterpret_cast<ani_long>(filterObj.release()));
    return retVal;
}

ani_object AniEffect::FlyInFlyOutEffect(ani_env* env, ani_object obj, ani_double degree, ani_enum_item flyMode)
{
    ani_object retVal {};

    ani_size flyModeIndex;
    if (ANI_OK != env->EnumItem_GetIndex(flyMode, &flyModeIndex)) {
        UIEFFECT_LOG_E("get flyModeIndex failed");
        return retVal;
    }
    auto flyOutPara = std::make_shared<FlyOutPara>();

    if (degree < -3.4E+38 || degree > 3.4E+38) {
        UIEFFECT_LOG_E("degree out of float range");
        return retVal;
    }
    flyOutPara->SetDegree(static_cast<float>(degree));
    flyOutPara->SetFlyMode(static_cast<uint32_t>(flyModeIndex));

    ani_long nativeObj;
    if (ANI_OK != env->Object_GetFieldByName_Long(obj, "filterNativeObj", &nativeObj)) {
        UIEFFECT_LOG_E("get generator filterNativeObj failed");
        return retVal;
    }

    Filter* filterObj = reinterpret_cast<Filter*>(nativeObj);
    filterObj->AddPara(flyOutPara);
    retVal = CreateAniObject(env, ANI_UIEFFECT_FILTER, nullptr, reinterpret_cast<ani_long>(filterObj));

    return retVal;
}

ani_object AniEffect::WaterRipple(ani_env* env, ani_object obj, ani_object waterPara)
{
    ani_object retVal {};
    ani_double progress;
    ani_int waveCount;
    ani_double x;
    ani_double y;
    ani_ref rippleMode;

    if (ANI_OK != env->Object_GetPropertyByName_Double(waterPara, "progress", &progress)) {
        UIEFFECT_LOG_E("get progress failed");
        return retVal;
    }

    if (ANI_OK != env->Object_GetPropertyByName_Int(waterPara, "waveCount", &waveCount)) {
        UIEFFECT_LOG_E("get waveCount failed");
        return retVal;
    }

    if (ANI_OK != env->Object_GetPropertyByName_Double(waterPara, "x", &x)) {
        UIEFFECT_LOG_E("get x failed");
        return retVal;
    }

    if (ANI_OK != env->Object_GetPropertyByName_Double(waterPara, "y", &y)) {
        UIEFFECT_LOG_E("get y failed");
        return retVal;
    }

    if (ANI_OK != env->Object_GetPropertyByName_Ref(waterPara, "rippleMode", &rippleMode)) {
        UIEFFECT_LOG_E("get rippleMode failed");
        return retVal;
    }
    
    ani_enum_item rippleModeEnum = static_cast<ani_enum_item>(rippleMode);
    ani_size rippleModeIndex;
    if (ANI_OK != env->EnumItem_GetIndex(rippleModeEnum, &rippleModeIndex)) {
        UIEFFECT_LOG_E("get rippleModeIndex failed");
        return retVal;
    }
    auto waterRipplePara = std::make_shared<WaterRipplePara>();
    waterRipplePara->SetProgress(static_cast<float>(progress));
    waterRipplePara->SetWaveCount(static_cast<uint32_t>(waveCount));
    waterRipplePara->SetRippleCenterX(static_cast<float>(x));
    waterRipplePara->SetRippleCenterY(static_cast<float>(y));
    waterRipplePara->SetRippleMode(static_cast<uint32_t>(rippleModeIndex));

    ani_long nativeObj;
    if (ANI_OK != env->Object_GetFieldByName_Long(obj, "filterNativeObj", &nativeObj)) {
        UIEFFECT_LOG_E("get generator filterNativeObj failed");
        return retVal;
    }

    Filter* filterObj = reinterpret_cast<Filter*>(nativeObj);
    filterObj->AddPara(waterRipplePara);
    retVal = CreateAniObject(env, ANI_UIEFFECT_FILTER, nullptr, reinterpret_cast<ani_long>(filterObj));

    return retVal;
}

ani_object AniEffect::Distort(ani_env* env, ani_object obj, ani_double distortionK)
{
    ani_object retVal {};

    auto distortPara = std::make_shared<DistortPara>();
    distortPara->SetDistortionK(static_cast<float>(distortionK));

    ani_long nativeObj;
    if (ANI_OK != env->Object_GetFieldByName_Long(obj, "filterNativeObj", &nativeObj)) {
        UIEFFECT_LOG_E("get generator filterNativeObj failed");
        return retVal;
    }

    Filter* filterObj = reinterpret_cast<Filter*>(nativeObj);
    filterObj->AddPara(distortPara);
    retVal = CreateAniObject(env, ANI_UIEFFECT_FILTER, nullptr, reinterpret_cast<ani_long>(filterObj));

    return retVal;
}

ani_object AniEffect::PixelStretch(ani_env* env, ani_object obj, ani_object arrayObj, ani_enum_item enumItem)
{
    ani_object retVal {};

    ani_size enumIndex;
    env->EnumItem_GetIndex(enumItem, &enumIndex);
    Drawing::TileMode tileMode = static_cast<Drawing::TileMode>(enumIndex);

    auto pixelStretchPara = std::make_shared<PixelStretchPara>();
    pixelStretchPara->SetTileMode(tileMode);

    ani_double length;
    if (ANI_OK != env->Object_GetPropertyByName_Double(arrayObj, "length", &length)) {
        UIEFFECT_LOG_E("get stretchSizes length failed");
        return retVal;
    }

    Vector4f stretchPercent;
    // 4 mean Vector4f length
    int vectorLen = 4;
    for (int i = 0; i < int(length) && i < vectorLen; i++) {
        ani_float floatValue;
        if (ANI_OK != env->Object_CallMethodByName_Float(arrayObj, "$_get", "I:F", &floatValue, (ani_int)i)) {
            UIEFFECT_LOG_E("stretchSizes Object_CallMethodByName_Float_A failed");
            return retVal;
        }
        stretchPercent[i] = static_cast<float>(floatValue);
    }
    pixelStretchPara->SetStretchPercent(stretchPercent);

    ani_long nativeObj;
    if (ANI_OK != env->Object_GetFieldByName_Long(obj, "filterNativeObj", &nativeObj)) {
        UIEFFECT_LOG_E("get generator filterNativeObj failed");
        return retVal;
    }

    Filter* filterObj = reinterpret_cast<Filter*>(nativeObj);
    filterObj->AddPara(pixelStretchPara);
    retVal = CreateAniObject(env, ANI_UIEFFECT_FILTER, nullptr, reinterpret_cast<ani_long>(filterObj));

    return retVal;
}

ani_object AniEffect::Blur(ani_env* env, ani_object obj, ani_double radius)
{
    ani_object retVal {};
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(static_cast<float>(radius));

    ani_long nativeObj;
    if (ANI_OK != env->Object_GetFieldByName_Long(obj, "filterNativeObj", &nativeObj)) {
        UIEFFECT_LOG_E("get generator filterNativeObj failed");
        return retVal;
    }

    Filter* filterObj = reinterpret_cast<Filter*>(nativeObj);
    filterObj->AddPara(filterBlurPara);
    retVal = CreateAniObject(env, ANI_UIEFFECT_FILTER, nullptr, reinterpret_cast<ani_long>(filterObj));

    return retVal;
}
} // namespace Rosen
} // namespace OHOS

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        UIEFFECT_LOG_E("[ANI_Constructor] Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    };
    static const char* staticClassName = "L@ohos/graphics/uiEffect/uiEffect;";
    ani_namespace uiEffectNamespace;
    if (env->FindNamespace(staticClassName, &uiEffectNamespace) != ANI_OK) {
        UIEFFECT_LOG_E("[ANI_Constructor] FindNamespace failed");
        return ANI_ERROR;
    };
    std::array staticMethods = {
        ani_native_function { "createEffect", nullptr, reinterpret_cast<void*>(OHOS::Rosen::AniEffect::CreateEffect) },
        ani_native_function { "createBrightnessBlender", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::CreateBrightnessBlender) },
        ani_native_function { "createFilter", nullptr, reinterpret_cast<void*>(OHOS::Rosen::AniEffect::CreateFilter) },
    };
    if (env->Namespace_BindNativeFunctions(uiEffectNamespace, staticMethods.data(), staticMethods.size()) != ANI_OK) {
        UIEFFECT_LOG_E("[ANI_Constructor] Namespace_BindNativeFunctions failed");
        return ANI_ERROR;
    };

    ani_status status = OHOS::Rosen::AniEffect::BindVisualEffectMethod(env);
    if (status != ANI_OK) {
        return status;
    }

    status = OHOS::Rosen::AniEffect::BindFilterMethod(env);
    if (status != ANI_OK) {
        return status;
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}
}
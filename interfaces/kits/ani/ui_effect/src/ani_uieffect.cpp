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
    UIEFFECT_LOG_I("create object success '%{public}s'", name.c_str());
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
    const ani_size offset = 0;
    const ani_size len = 5;
    env->Array_GetRegion_Double(positiveCoefficientAniArray, offset, len, posCoefNativeBuffer);
    env->Array_GetRegion_Double(negativeCoefficientAniArray, offset, len, negCoefNativeBuffer);
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
    auto effectObj = std::make_unique<VisualEffect>();
    auto retVal = CreateAniObject(env, ANI_UIEFFECT_VISUAL_EFFECT, nullptr,
        reinterpret_cast<ani_long>(effectObj.release()));
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
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::CreateBrightnessBlender) }
    };
    if (env->Namespace_BindNativeFunctions(uiEffectNamespace, staticMethods.data(), staticMethods.size()) != ANI_OK) {
        UIEFFECT_LOG_E("[ANI_Constructor] Namespace_BindNativeFunctions failed");
        return ANI_ERROR;
    };
    static const std::string ani_class_VisualEffect = "L@ohos/graphics/uiEffect/uiEffect/VisualEffectInternal;";
    static const char* className = ani_class_VisualEffect.c_str();
    ani_class cls;
    if (env->FindClass(className, &cls) != ANI_OK) {
        UIEFFECT_LOG_E("Not found L@ohos/graphics/uiEffect/uiEffect/VisualEffectInternal;");
        return ANI_NOT_FOUND;
    };
    std::array methods = {
        ani_native_function { "backgroundColorBlenderNative",
            "L@ohos/graphics/uiEffect/uiEffect/BrightnessBlender;:L@ohos/graphics/uiEffect/uiEffect/VisualEffect;",
            reinterpret_cast<void*>(OHOS::Rosen::AniEffect::BackgroundColorBlender) }
    };
    ani_status ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        UIEFFECT_LOG_E("Class_BindNativeMethods failed: %{public}d", ret);
        return ANI_ERROR;
    };
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}
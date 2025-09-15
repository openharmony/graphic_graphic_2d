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

#include "ani_shadow_layer.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_SHADOW_LAYER_NAME = "@ohos.graphics.drawing.drawing.ShadowLayer";

ani_status AniShadowLayer::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_SHADOW_LAYER_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_SHADOW_LAYER_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "create", "dddC{@ohos.graphics.common2D.common2D.Color}:"
            "C{@ohos.graphics.drawing.drawing.ShadowLayer}", reinterpret_cast<void*>(CreateWithColor) },
        ani_native_function { "create", "dddX{C{@ohos.graphics.common2D.common2D.Color}C{std.core.Int}}:"
            "C{@ohos.graphics.drawing.drawing.ShadowLayer}", reinterpret_cast<void*>(Create) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_SHADOW_LAYER_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

ani_object AniShadowLayer::Create(ani_env* env, ani_object obj, ani_double blurRadius,
    ani_double x, ani_double y, ani_object colorObj)
{
    if (blurRadius <= 0) {
        ROSEN_LOGE("AniShadowLayer::Create Argv[0] is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. blurRadius must be greater than 0.");
        return CreateAniUndefined(env);
    }
    Drawing::ColorQuad color;
    if (!GetColorQuadFromParam(env, colorObj, color)) {
        ROSEN_LOGE("AniShadowLayer::Create Argv[3] is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
        return CreateAniUndefined(env);
    }
    std::shared_ptr<BlurDrawLooper> looper = BlurDrawLooper::CreateBlurDrawLooper(
        blurRadius, x, y, color);
    return CreateLooper(env, looper);
}

ani_object AniShadowLayer::CreateWithColor(ani_env* env, ani_object obj, ani_double blurRadius,
    ani_double x, ani_double y, ani_object aniColorObj)
{
    if (blurRadius <= 0) {
        ROSEN_LOGE("AniShadowLayer::Create blurRadius must be greater than 0. ");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
        return CreateAniUndefined(env);
    }
    Drawing::ColorQuad color;
    if (!GetColorQuadFromColorObj(env, aniColorObj, color)) {
        ROSEN_LOGE("AniShadowLayer::Create colorObj is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }
    std::shared_ptr<BlurDrawLooper> looper = BlurDrawLooper::CreateBlurDrawLooper(
        blurRadius, x, y, color);
    return CreateLooper(env, looper);
}

ani_object AniShadowLayer::CreateLooper(ani_env* env, const std::shared_ptr<BlurDrawLooper> blurDrawLooper)
{
    if (blurDrawLooper == nullptr) {
        ROSEN_LOGE("AniShadowLayer::CreateLooper blurDrawLooper is null");
        return CreateAniUndefined(env);
    }
    AniShadowLayer* aniShadowLayer = new AniShadowLayer(blurDrawLooper);
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_SHADOW_LAYER_NAME, aniShadowLayer);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete aniShadowLayer;
        ROSEN_LOGE("AniShadowLayer::CreateLooper failed cause aniObj is undefined");
    }
    return aniObj;
}

AniShadowLayer::~AniShadowLayer()
{
    blurDrawLooper_ = nullptr;
}

std::shared_ptr<BlurDrawLooper> AniShadowLayer::GetBlurDrawLooper()
{
    return blurDrawLooper_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
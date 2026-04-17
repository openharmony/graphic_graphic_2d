/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_EFFECT_SPATIAL_GLASS_EFFECT_PARA_H
#define UIEFFECT_EFFECT_SPATIAL_GLASS_EFFECT_PARA_H

#include "visual_effect_para.h"
#include <iostream>
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "ui_effect/mask/include/mask_para.h"

namespace OHOS {
namespace Rosen {
class SpatialGlassEffectPara : public VisualEffectPara {
public:
    SpatialGlassEffectPara()
    {
        this->type_ = VisualEffectPara::ParaType::SPATIAL_GLASS_EFFECT;
    }
    ~SpatialGlassEffectPara() override = default;

    void SetLeftTop(Vector3f& param)
    {
        leftTop_ = param;
    }

    const Vector3f GetLeftTop() const
    {
        return leftTop_;
    }

    void SetRightTop(Vector3f& param)
    {
        rightTop_ = param;
    }

    const Vector3f GetRightTop() const
    {
        return rightTop_;
    }

    void SetLeftBot(Vector3f& param)
    {
        leftBot_ = param;
    }

    const Vector3f GetLeftBot() const
    {
        return leftBot_;
    }

    void SetRightBot(Vector3f& param)
    {
        rightBot_ = param;
    }

    const Vector3f GetRightBot() const
    {
        return rightBot_;
    }

    void SetCameraPosition(Vector3f& param)
    {
        cameraPosition_ = param;
    }

    const Vector3f GetCameraPosition() const
    {
        return cameraPosition_;
    }

    void SetCameraInstrinsics(Vector3f& param)
    {
        cameraIntrinsics_ = param;
    }

    const Vector3f GetCameraInstrinsics() const
    {
        return cameraIntrinsics_;
    }

    void SetThickParams(Vector2f& param)
    {
        thickParams_ = param;
    }

    const Vector2f GetThickParams() const
    {
        return thickParams_;
    }

    void SetLightDir(Vector3f& param)
    {
        lightDir_ = param;
    }

    const Vector3f GetLightDir() const
    {
        return lightDir_;
    }

    void SetGlassBaseColor(Vector4f& param)
    {
        glassBaseColor_ = param;
    }

    const Vector4f GetGlassBaseColor() const
    {
        return glassBaseColor_;
    }

    void SetRefractCoef(Vector2f& param)
    {
        refractCoef_ = param;
    }

    const Vector2f GetRefractCoef() const
    {
        return refractCoef_;
    }

    void SetEdgeLightColor(Vector4f& param)
    {
        edgeLightColor_ = param;
    }

    const Vector4f GetEdgeLightColor() const
    {
        return edgeLightColor_;
    }

    void SetFresnelEnvColor(Vector4f& param)
    {
        fresnelEnvColor_ = param;
    }

    const Vector4f GetFresnelEnvColor() const
    {
        return fresnelEnvColor_;
    }

    void SetFresnelParams(Vector2f& param)
    {
        fresnelParams_ = param;
    }

    const Vector2f GetFresnelParams() const
    {
        return fresnelParams_;
    }

    void SetSpecularShiny(float param)
    {
        specularShiny_ = param;
    }

    float GetSpecularShiny() const
    {
        return specularShiny_;
    }

private:
    Vector3f leftTop_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f rightTop_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f leftBot_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f rightBot_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f cameraPosition_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f cameraIntrinsics_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector2f thickParams_ = Vector2f(0.0f, 0.0f);
    Vector3f lightDir_ = Vector3f(0.0f, 0.0f, 0.0f);
    
    Vector4f glassBaseColor_ = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    Vector2f refractCoef_ = Vector2f(0.0f, 0.0f);
    Vector4f edgeLightColor_ = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    Vector4f fresnelEnvColor_ = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    Vector2f fresnelParams_ = Vector2f(0.0f, 0.0f);
    float specularShiny_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_SPATIAL_GLASS_EFFECT_PARA_H
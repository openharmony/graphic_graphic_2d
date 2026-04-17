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

    void SetLeftTop(Vector3f& leftTop)
    {
        leftTop_ = leftTop;
    }

    const Vector3f GetLeftTop() const
    {
        return leftTop_;
    }

    void SetRightTop(Vector3f& rightTop)
    {
        rightTop_ = rightTop;
    }

    const Vector3f GetRightTop() const
    {
        return rightTop_;
    }

    void SetLeftBot(Vector3f& leftBot)
    {
        leftBot_ = leftBot;
    }

    const Vector3f GetLeftBot() const
    {
        return leftBot_;
    }

    void SetRightBot(Vector3f& rightBot)
    {
        rightBot_ = rightBot;
    }

    const Vector3f GetRightBot() const
    {
        return rightBot_;
    }

    void SetMameraPosition(Vector3f& cameraPosition)
    {
        cameraPosition_ = cameraPosition;
    }

    const Vector3f GetCameraPosition() const
    {
        return cameraPosition_;
    }

    void SetCameraInstrinsics(Vector3f& cameraIntrinsics)
    {
        cameraIntrinsics_ = cameraIntrinsics;
    }

    const Vector3f GetCameraInstrinsics() const
    {
        return cameraIntrinsics_;
    }

    void SetThickParams(Vector2f& thickParams)
    {
        thickParams_ = thickParams;
    }

    const Vector2f GetThickParams() const
    {
        return thickParams_;
    }

    void SetLightDir(Vector3f& lightDir)
    {
        lightDir_ = lightDir;
    }

    const Vector3f GetLightDir() const
    {
        return lightDir_;
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
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_SPATIAL_GLASS_EFFECT_PARA_H
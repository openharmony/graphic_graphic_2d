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
#ifndef UIEFFECT_FILTER_FROSTED_GLASS_PARA_H
#define UIEFFECT_FILTER_FROSTED_GLASS_PARA_H
#include <iostream>
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "filter_para.h"

namespace OHOS {
namespace Rosen {
class FrostedGlassPara : public FilterPara {
public:
    FrostedGlassPara()
    {
        this->type_ = FilterPara::ParaType::FROSTED_GLASS;
    }
    ~FrostedGlassPara() override = default;
    
    void SetBlurParams(Vector2f& blurParams)
    {
        blurParams_ = blurParams;
    }

    const Vector2f GetBlurParams() const
    {
        return blurParams_;
    }

    void SetWeightsEmboss(Vector2f& weightsEmboss)
    {
        weightsEmboss_ = weightsEmboss;
    }

    const Vector2f GetWeightsEmboss() const
    {
        return weightsEmboss_;
    }

    void SetWeightsEdl(Vector2f& weightsEdl)
    {
        weightsEdl_ = weightsEdl; 
    } 

    const Vector2f GetWeightsEdl() const
    { 
        return weightsEdl_;
    }

    void SetBgRates(Vector2f& bgRates)
    {
        bgRates_ = bgRates; 
    }

    const Vector2f GetBgRates() const
    { 
        return bgRates_;
    }

    void SetBgKBS(Vector3f& bgKBS)
    {
        bgKBS_ = bgKBS; 
    }

    const Vector3f GetBgKBS() const
    { 
        return bgKBS_;
    }

    void SetBgPos(Vector3f& bgPos)
    { 
        bgPos_ = bgPos;
    }

    const Vector3f GetBgPos() const
    { 
        return bgPos_;
    }

    void SetBgNeg(Vector3f& bgNeg)
    {
        bgNeg_ = bgNeg;
    }

    const Vector3f GetBgNeg() const
    {
        return bgNeg_;
    }

    void SetRefractParams(Vector3f& refractParams)
    {
        refractParams_ = refractParams;
    }

    const Vector3f GetRefractParams() const
    {
        return refractParams_;
    }

    void SetSdParams(Vector3f& sdParams)
    {
        sdParams_ = sdParams;
    }

    const Vector3f GetSdParams() const
    {
        return sdParams_;
    }

    void SetSdRates(Vector2f& sdRates)
    {
        sdRates_ = sdRates;
    }

    const Vector2f GetSdRates() const
    {
        return sdRates_;
    }

    void SetSdKBS(Vector3f& sdKBS)
    {
        sdKBS_ = sdKBS;
    }

    const Vector3f GetSdKBS() const
    {
        return sdKBS_;
    }

    void SetSdPos(Vector3f& sdPos)
    {
        sdPos_ = sdPos;
    }

    const Vector3f GetSdPos() const
    {
        return sdPos_;
    }

    void SetSdNeg(Vector3f& sdNeg)
    {
        sdNeg_ = sdNeg;
    }

    const Vector3f GetSdNeg() const
    {
        return sdNeg_;
    }

    void SetEnvLightParams(Vector3f& envLightParams)
    {
        envLightParams_ = envLightParams;
    }

    const Vector3f GetEnvLightParams() const
    {
        return envLightParams_;
    }

    void SetEnvLightRates(Vector2f& envLightRates)
    {
        envLightRates_ = envLightRates;
    }

    const Vector2f GetEnvLightRates() const
    {
        return envLightRates_;
    }

    void SetEnvLightKBS(Vector3f& envLightKBS)
    {
        envLightKBS_ = envLightKBS;
    }

    const Vector3f GetEnvLightKBS() const
    {
        return envLightKBS_;
    }

    void SetEnvLightPos(Vector3f& envLightPos)
    {
        envLightPos_ = envLightPos;
    }

    const Vector3f GetEnvLightPos() const
    {
        return envLightPos_;
    }

    void SetEnvLightNeg(Vector3f& envLightNeg)
    {
        envLightNeg_ = envLightNeg;
    }

    const Vector3f GetEnvLightNeg() const
    {
        return envLightNeg_;
    }
    
    void SetEdLightParams(Vector2f& edLightParams)
    {
        edLightParams_ = edLightParams;
    }

    const Vector2f GetEdLightParams() const
    { 
        return edLightParams_;
    }

    void SetEdLightAngles(Vector2f& edLightAngles)
    {
        edLightAngles_ = edLightAngles;
    }

    const Vector2f GetEdLightAngles() const
    {
        return edLightAngles_;
    }

    void SetEdLightDir(Vector2f& edLightDir)
    {
        edLightDir_ = edLightDir;
    }

    const Vector2f GetEdLightDir() const
    {
        return edLightDir_;
    }

    void SetEdLightRates(Vector2f& edLightRates)
    {
        edLightRates_ = edLightRates;
    }

    const Vector2f GetEdLightRates() const
    {
        return edLightRates_;
    }

    void SetEdLightKBS(Vector3f& edLightKBS)
    {
        edLightKBS_ = edLightKBS;
    }

    const Vector3f GetEdLightKBS() const
    {
        return edLightKBS_;
    }

    void SetEdLightPos(Vector3f& edLightPos)
    {
        edLightPos_ = edLightPos;
    }

    const Vector3f GetEdLightPos() const
    {
        return edLightPos_;
    }

    void SetEdLightNeg(Vector3f& edLightNeg)
    {
        edLightNeg_ = edLightNeg;
    }

    const Vector3f GetEdLightNeg() const
    {
        return edLightNeg_;
    }

    void SetBorderSize(Vector2f& borderSize)
    {
        borderSize_ = borderSize;
    }

    const Vector2f GetBorderSize() const
    {
        return borderSize_;
    }

    void SetCornerRadius(float& cornerRadius)
    {
        cornerRadius_ = cornerRadius;
    }

    const float& GetCornerRadius() const
    {
        return cornerRadius_;
    }

private:
    Vector2f blurParams_ = Vector2f(0.0f, 0.0f);
    Vector2f weightsEmboss_ = Vector2f(0.0f, 0.0f); // (envLight, sd)
    Vector2f weightsEdl_ = Vector2f(0.0f, 0.0f); // (envLight, sd)
    // Background darken parameters
    Vector2f bgRates_ = Vector2f(0.0f, 0.0f);
    Vector3f bgKBS_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f bgPos_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f bgNeg_ = Vector3f(0.0f, 0.0f, 0.0f);
    // Refraction parameters
    Vector3f refractParams_ = Vector3f(0.0f, 0.0f, 0.0f);
    // Inner shadow parameters
    Vector3f sdParams_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector2f sdRates_ = Vector2f(0.0f, 0.0f);
    Vector3f sdKBS_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f sdPos_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f sdNeg_ = Vector3f(0.0f, 0.0f, 0.0f);
    // Env refraction parameters
    Vector3f envLightParams_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector2f envLightRates_ = Vector2f(0.0f, 0.0f);
    Vector3f envLightKBS_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f envLightPos_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f envLightNeg_ = Vector3f(0.0f, 0.0f, 0.0f);
    // Edge highlights parameters
    Vector2f edLightParams_ = Vector2f(0.0f, 0.0f);
    Vector2f edLightAngles_ = Vector2f(0.0f, 30.0f);
    Vector2f edLightDir_ = Vector2f(0.0f, 0.0f);
    Vector2f edLightRates_ = Vector2f(0.0f, 0.0f);
    Vector3f edLightKBS_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f edLightPos_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f edLightNeg_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector2f borderSize_ = Vector2f(0.0f, 0.0f);
    float cornerRadius_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_FROSTED_GLASS_PARA_H
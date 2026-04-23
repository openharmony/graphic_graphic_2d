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
#ifndef UIEFFECT_EFFECT_HDR_DARKEN_BLENDER_H
#define UIEFFECT_EFFECT_HDR_DARKEN_BLENDER_H

#include "blender.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {

class HdrDarkenBlender : public Blender {
public:
    HdrDarkenBlender()
    {
        this->blenderType_ = Blender::HDR_DARKEN_BLENDER;
    }
    ~HdrDarkenBlender() override = default;

    void SetHdrBrightnessRatio(float hdrBrightnessRatio)
    {
        hdrBrightnessRatio_ = UIEffect::GetLimitedPara(hdrBrightnessRatio);
    }

    float GetHdrBrightnessRatio() const
    {
        return hdrBrightnessRatio_;
    }

    void SetGrayscaleFactor(const Vector3f& grayscaleFactor)
    {
        grayscaleFactor_ = UIEffect::GetLimitedPara(grayscaleFactor);
    }

    const Vector3f& GetGrayscaleFactor() const
    {
        return grayscaleFactor_;
    }

private:
    float hdrBrightnessRatio_ = 1.0f;
    Vector3f grayscaleFactor_;
};

} // namespace Rosen
} // namespace OHOS
#endif // #define UIEFFECT_EFFECT_HDR_DARKEN_BLENDER_H


/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_EFFECT_DISTORTION_COLLAPSE_EFFECT_PARA_H
#define UIEFFECT_EFFECT_DISTORTION_COLLAPSE_EFFECT_PARA_H

#include "visual_effect_para.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {

class DistortionCollapseEffectPara : public VisualEffectPara {
public:
    DistortionCollapseEffectPara()
    {
        type_ = ParaType::DISTORTION_COLLAPSE_EFFECT;
    }
    ~DistortionCollapseEffectPara() override = default;

    void SetLUCorner(const Vector2f& corner)
    {
        luCorner_ = corner;
    }

    const Vector2f& GetLUCorner() const
    {
        return luCorner_;
    }

    void SetRUCorner(const Vector2f& corner)
    {
        ruCorner_ = corner;
    }

    const Vector2f& GetRUCorner() const
    {
        return ruCorner_;
    }

    void SetLBCorner(const Vector2f& corner)
    {
        lbCorner_ = corner;
    }

    const Vector2f& GetLBCorner() const
    {
        return lbCorner_;
    }

    void SetRBCorner(const Vector2f& corner)
    {
        rbCorner_ = corner;
    }

    const Vector2f& GetRBCorner() const
    {
        return rbCorner_;
    }

    void SetBarrelDistortion(const Vector4f& barrelDistortion)
    {
        barrelDistortion_ = barrelDistortion;
    }

    const Vector4f& GetBarrelDistortion() const
    {
        return barrelDistortion_;
    }

    void SetDisabled(bool disabled)
    {
        disabled_ = disabled;
    }

    bool IsDisabled() const
    {
        return disabled_;
    }

private:
    Vector2f luCorner_;
    Vector2f ruCorner_;
    Vector2f lbCorner_;
    Vector2f rbCorner_;
    Vector4f barrelDistortion_;
    bool disabled_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // UIEFFECT_EFFECT_DISTORTION_COLLAPSE_EFFECT_PARA_H

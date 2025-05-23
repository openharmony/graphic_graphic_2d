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
#ifndef UIEFFECT_EFFECT_HDR_UI_BRIGHTNESS_PARA_H
#define UIEFFECT_EFFECT_HDR_UI_BRIGHTNESS_PARA_H

#include "visual_effect_para.h"

namespace OHOS {
namespace Rosen {
class HDRUIBrightnessPara : public VisualEffectPara {
public:
    HDRUIBrightnessPara()
    {
        this->type_ = VisualEffectPara::ParaType::HDR_UI_BRIGHTNESS;
    }
    ~HDRUIBrightnessPara() override = default;

    void SetHDRUIBrightness(float hdrUIBrightness)
    {
        hdrUIBrightness_ = hdrUIBrightness;
    }

    float GetHDRUIBrightness() const
    {
        return hdrUIBrightness_;
    }

private:
    float hdrUIBrightness_ = 1.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_HDR_UI_BRIGHTNESS_PARA_H

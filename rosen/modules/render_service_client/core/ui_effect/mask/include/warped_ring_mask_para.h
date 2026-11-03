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
#ifndef UIEFFECT_WARPED_RING_MASK_PARA_H
#define UIEFFECT_WARPED_RING_MASK_PARA_H

#include "ui_effect/mask/include/mask_para.h"

namespace OHOS {
namespace Rosen {
struct WarpedRingParam {
    float radius = 0.0f;
    float baseHalfWidth = 0.0f;
    float widthVariation = 0.0f;
    float rotateAngle = 0.0f;
    float rotate3DProgress = 0.0f;
    float noiseEvolution = 0.0f;
};

class WarpedRingMaskPara : public MaskPara {
public:
    WarpedRingMaskPara()
    {
        type_ = MaskPara::Type::WARPED_RING_MASK;
    }
    ~WarpedRingMaskPara() override = default;

    void SetProgress(float progress)
    {
        progress_ = progress;
    }

    float GetProgress() const
    {
        return progress_;
    }

    void SetRingParam(const WarpedRingParam& ringParam)
    {
        ringParam_ = ringParam;
    }

    const WarpedRingParam& GetRingParam() const
    {
        return ringParam_;
    }

private:
    float progress_ = 0.0f;
    WarpedRingParam ringParam_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_WARPED_RING_MASK_PARA_H
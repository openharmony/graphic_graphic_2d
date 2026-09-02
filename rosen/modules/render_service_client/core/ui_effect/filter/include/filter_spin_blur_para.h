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
#ifndef UIEFFECT_FILTER_SPIN_BLUR_PARA_H
#define UIEFFECT_FILTER_SPIN_BLUR_PARA_H

#include "common/rs_vector2.h"
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"

namespace OHOS {
namespace Rosen {
class SpinBlurPara : public FilterPara {
public:
    SpinBlurPara()
    {
        type_ = FilterPara::ParaType::SPIN_BLUR;
    }
    ~SpinBlurPara() override = default;

    void SetCenter(const Vector2f& center)
    {
        center_ = center;
    }

    const Vector2f& GetCenter() const
    {
        return center_;
    }

    void SetAngle(float angle)
    {
        angle_ = angle;
    }

    float GetAngle() const
    {
        return angle_;
    }

    void SetSamples(int32_t samples)
    {
        samples_ = samples;
    }

    int32_t GetSamples() const
    {
        return samples_;
    }

private:
    Vector2f center_ = Vector2f(0.5f, 0.5f);
    float angle_ = 0.0f;
    int32_t samples_ = 32;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_SPIN_BLUR_PARA_H
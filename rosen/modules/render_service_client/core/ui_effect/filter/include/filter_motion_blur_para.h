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
#ifndef UIEFFECT_FILTER_MOTION_BLUR_PARA_H
#define UIEFFECT_FILTER_MOTION_BLUR_PARA_H

#include "filter_para.h"
#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT MotionBlurPara : public FilterPara {
public:
    MotionBlurPara()
    {
        this->type_ = FilterPara::ParaType::MOTION_BLUR;
    }
    ~MotionBlurPara() override = default;

    void SetRadius(float radius)
    {
        radius_ = radius;
    }

    float GetRadius() const
    {
        return radius_;
    }

    void SetAnchor(const Vector2f& anchor)
    {
        anchor_ = anchor;
    }

    const Vector2f& GetAnchor() const
    {
        return anchor_;
    }

    void SetSampleCount(int32_t sampleCount)
    {
        sampleCount_ = sampleCount;
    }

    int32_t GetSampleCount() const
    {
        return sampleCount_;
    }

private:
    float radius_ = 0.0f;
    Vector2f anchor_ = Vector2f(0.5f, 0.5f); // 默认中心点 [0.5, 0.5]
    int32_t sampleCount_ = 8;
};

} // namespace Rosen
} // namespace OHOS

#endif // UIEFFECT_FILTER_MOTION_BLUR_PARA_H
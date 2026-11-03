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
#ifndef UIEFFECT_FILTER_HALO_BLOOM_PARA_H
#define UIEFFECT_FILTER_HALO_BLOOM_PARA_H

#include "common/rs_vector4.h"
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"

namespace OHOS {
namespace Rosen {
class HaloBloomPara : public FilterPara {
public:
    HaloBloomPara()
    {
        type_ = FilterPara::ParaType::HALO_BLOOM;
    }
    ~HaloBloomPara() override = default;

    void SetTintColor(const Vector4f& tintColor)
    {
        tintColor_ = tintColor;
    }

    const Vector4f& GetTintColor() const
    {
        return tintColor_;
    }

    void SetBloomFactor(float bloomFactor)
    {
        bloomFactor_ = bloomFactor;
    }

    float GetBloomFactor() const
    {
        return bloomFactor_;
    }

    void SetGlowExposure(float glowExposure)
    {
        glowExposure_ = glowExposure;
    }

    float GetGlowExposure() const
    {
        return glowExposure_;
    }

private:
    Vector4f tintColor_ = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    float bloomFactor_ = 0.0f;
    float glowExposure_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_HALO_BLOOM_PARA_H
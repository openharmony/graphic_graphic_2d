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

#ifndef UIEFFECT_FILTER_MASK_TRANSITION_PARA_H
#define UIEFFECT_FILTER_MASK_TRANSITION_PARA_H

#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT MaskTransitionPara : public FilterPara {
public:
    MaskTransitionPara()
    {
        this->type_ = FilterPara::ParaType::MASK_TRANSITION;
    }
    ~MaskTransitionPara() override = default;

    MaskTransitionPara(const MaskTransitionPara& other);

    void SetMask(std::shared_ptr<MaskPara> maskPara)
    {
        maskPara_ = maskPara;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

    void SetFactor(float factor)
    {
        factor_ = factor;
    }

    float GetFactor() const
    {
        return factor_;
    }

    void SetInverse(bool inverse)
    {
        inverse_ = inverse;
    }

    bool GetInverse() const
    {
        return inverse_;
    }

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val);

    std::shared_ptr<FilterPara> Clone() const override;

private:
    std::shared_ptr<MaskPara> maskPara_ = nullptr;
    float factor_ = 1.0f;
    bool inverse_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // UIEFFECT_FILTER_MASK_TRANSITION_PARA_H
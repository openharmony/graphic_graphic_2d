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
#ifndef UIEFFECT_FILTER_DISPERSION_PARA_H
#define UIEFFECT_FILTER_DISPERSION_PARA_H
#include "common/rs_vector2.h"
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT DispersionPara : public FilterPara {
public:
    DispersionPara()
    {
        this->type_ = FilterPara::ParaType::DISPERSION;
    }
    ~DispersionPara() override = default;

    DispersionPara(const DispersionPara& other);

    void SetMask(std::shared_ptr<MaskPara> maskPara)
    {
        maskPara_ = maskPara;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

    void SetOpacity(float opacity)
    {
        opacity_ = opacity;
    }

    float GetOpacity() const
    {
        return opacity_;
    }

    void SetRedOffset(const Vector2f& redOffset)
    {
        redOffset_ = redOffset;
    }

    const Vector2f& GetRedOffset() const
    {
        return redOffset_;
    }

    void SetGreenOffset(const Vector2f& greenOffset)
    {
        greenOffset_ = greenOffset;
    }

    const Vector2f& GetGreenOffset() const
    {
        return greenOffset_;
    }

    void SetBlueOffset(const Vector2f& blueOffset)
    {
        blueOffset_ = blueOffset;
    }

    const Vector2f& GetBlueOffset() const
    {
        return blueOffset_;
    }

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val);

    std::shared_ptr<FilterPara> Clone() const override;

private:
    std::shared_ptr<MaskPara> maskPara_ = nullptr;

    float opacity_ = 0.0f;
    Vector2f redOffset_;
    Vector2f greenOffset_;
    Vector2f blueOffset_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_EDGE_LIGHT_PARA_H

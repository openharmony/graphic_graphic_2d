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
#ifndef RENDER_RS_RENDER_PIXEL_MAP_MASK_H
#define RENDER_RS_RENDER_PIXEL_MAP_MASK_H

#include <memory>
#include "common/rs_vector4.h"
#include "image/image.h"
#include "render/rs_render_mask.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderPixelMapMaskPara : public RSRenderMaskPara {
public:
    RSRenderPixelMapMaskPara(PropertyId id) :
        RSRenderMaskPara(RSUIFilterType::PIXEL_MAP_MASK)
    {
        id_ = id;
    }
    virtual ~RSRenderPixelMapMaskPara() = default;

    static std::shared_ptr<RSRenderPropertyBase> CreateRenderProperty(RSUIFilterType type);

    template<class T>
    std::shared_ptr<RSRenderAnimatableProperty<T>> GetRenderAnimatableProperty(const RSUIFilterType type)
    {
        return std::static_pointer_cast<RSRenderAnimatableProperty<T>>(GetRenderProperty(type));
    }

    void GetDescription(std::string& out) const override;

    bool WriteToParcel(Parcel& parcel) override;

    bool ReadFromParcel(Parcel& parcel) override;

    virtual std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties() override;

    const std::shared_ptr<Drawing::Image> GetImage() const;

private:
    std::shared_ptr<Drawing::Image> cacheImage_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_RS_RENDER_PIXEL_MAP_MASK_H

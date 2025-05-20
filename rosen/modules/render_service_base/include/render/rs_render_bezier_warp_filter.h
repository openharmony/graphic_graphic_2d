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
#ifndef RENDER_BEZIER_WARP_FILTER_RENDER_PROPERTY_H
#define RENDER_BEZIER_WARP_FILTER_RENDER_PROPERTY_H

#include "render/rs_render_filter_base.h"
namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderBezierWarpFilterPara : public RSRenderFilterParaBase {
public:
    RSRenderBezierWarpFilterPara(PropertyId id) : RSRenderFilterParaBase(RSUIFilterType::BEZIER_WARP)
    {
        id_ = id;
    }

    virtual ~RSRenderBezierWarpFilterPara() = default;

    void GetDescription(std::string& out) const override;

    virtual bool WriteToParcel(Parcel& parcel) override;

    virtual bool ReadFromParcel(Parcel& parcel) override;

    static std::shared_ptr<RSRenderPropertyBase> CreateRenderProperty(RSUIFilterType type);

    virtual std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties() override;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_BEZIER_WARP_FILTER_RENDER_PROPERTY_H

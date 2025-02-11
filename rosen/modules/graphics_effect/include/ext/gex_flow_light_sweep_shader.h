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
#ifndef GRAPHICS_EFFECT_EXT_FLOW_LIGHT_SWEEP_SHADER_H
#define GRAPHICS_EFFECT_EXT_FLOW_LIGHT_SWEEP_SHADER_H

#include <parcel.h>

#include "draw/color.h"
#include "ge_shader.h"

namespace OHOS {
namespace Rosen {

struct GE_EXPORT GEXFlowLightSweepParams {
    std::vector<std::pair<Drawing::Color, float>> effectColors_;

    bool Marshalling(Parcel& parcel);
    bool Unmarshalling(Parcel& parcel);
};

class GE_EXPORT GEXFlowLightSweepShader : public GEShader {
public:
    GEXFlowLightSweepShader() = default;
    ~GEXFlowLightSweepShader() override = default;

    static std::shared_ptr<GEXFlowLightSweepShader> CreateDynamicImpl(
        std::vector<std::pair<Drawing::Color, float>>& param);

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override { }

    virtual const std::string GetDescription() const;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_EXT_FLOW_LIGHT_SWEEP_SHADER_H

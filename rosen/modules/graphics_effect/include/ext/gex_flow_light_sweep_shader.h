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

constexpr uint32_t SIZE_MAX_LIMIT = 100;  // 100 max length
struct GEXFlowLightSweepParams {
    std::vector<std::pair<Drawing::Color, float>> effectColors_;

    bool Marshalling(Parcel& parcel)
    {
        uint32_t effectColorsSize = static_cast<uint32_t>(effectColors_.size());
        if (effectColorsSize > SIZE_MAX_LIMIT) {
            return false;
        }

        if (!parcel.WriteUint32(effectColorsSize)) {
            return false;
        }

        for (size_t i = 0; i < effectColorsSize; i++) {
            if (!parcel.WriteUint32((uint32_t)effectColors_[i].first.CastToColorQuad())) {
                return false;
            }
            if (!parcel.WriteFloat(effectColors_[i].second)) {
                return false;
            }
        }

        return true;
    }
    
    bool Unmarshalling(Parcel& parcel)
    {
        uint32_t effectColorsSize = parcel.ReadUint32();
        if (effectColorsSize > SIZE_MAX_LIMIT) {
            return false;
        }

        for (size_t i = 0; i < effectColorsSize; i++) {
            std::pair<Drawing::Color, float> effectColor;
            effectColor.first = Drawing::Color(parcel.ReadUint32());
            effectColor.second = parcel.ReadFloat();

            effectColors_.emplace_back(effectColor);
        }

        return true;
    }
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

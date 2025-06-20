/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_MAGNIFIER_PARA_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_MAGNIFIER_PARA_H

#include "common/rs_color.h"
#include "common/rs_macros.h"
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSMagnifierParams {
public:
    explicit RSMagnifierParams() {}
    ~RSMagnifierParams() = default;

    float factor_ = 0.f;
    float width_ = 0.f;
    float height_ = 0.f;
    float cornerRadius_ = 0.f;
    float borderWidth_ = 0.f;
    float offsetX_ = 0.f;
    float offsetY_ = 0.f;

    float shadowOffsetX_ = 0.f;
    float shadowOffsetY_ = 0.f;
    float shadowSize_ = 0.f;
    float shadowStrength_ = 0.f;

    // rgba
    uint32_t gradientMaskColor1_ = 0x00000000;
    uint32_t gradientMaskColor2_ = 0x00000000;
    uint32_t outerContourColor1_ = 0x00000000;
    uint32_t outerContourColor2_ = 0x00000000;

    void Dump(std::string& out) const
    {
        out += "[factor:" + std::to_string(factor_) + " width:" + std::to_string(width_);
        out += " height:" + std::to_string(height_) + " cornerRadius:" + std::to_string(cornerRadius_);
        out += " borderWidth:" + std::to_string(borderWidth_) + " offsetX:" + std::to_string(offsetX_);
        out += " offsetY:" + std::to_string(offsetY_) + " shadowOffsetX:" + std::to_string(shadowOffsetX_);
        out += " shadowOffsetY:" + std::to_string(shadowOffsetY_) + " shadowSize:" + std::to_string(shadowSize_);
        out += " shadowStrength:" + std::to_string(shadowStrength_);
        RSColor color(gradientMaskColor1_);
        out += " gradientMaskColor1";
        color.Dump(out);
        color = RSColor(gradientMaskColor2_);
        out += " gradientMaskColor2";
        color.Dump(out);
        color = RSColor(outerContourColor1_);
        out += " outerContourColor1";
        color.Dump(out);
        color = RSColor(outerContourColor2_);
        out += " outerContourColor2";
        color.Dump(out);
        out += ']';
    }
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_MAGNIFIER_PARA_H

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

#include "ui_effect/property/include/rs_ui_shader_base.h"

namespace OHOS {
namespace Rosen {
using ShaderCreator = std::function<std::shared_ptr<RSNGShaderBase>()>;

static std::unordered_map<RSNGEffectType, ShaderCreator> creatorLUT = {
    {RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT, [] {
            return std::make_shared<RSNGContourDiagonalFlowLight>();
        }
    },
    {RSNGEffectType::WAVY_RIPPLE_LIGHT, [] {
            return std::make_shared<RSNGWavyRippleLight>();
        }
    },
    {RSNGEffectType::AURORA_NOISE, [] {
            return std::make_shared<RSNGAuroraNoise>();
        }
    },
    {RSNGEffectType::PARTICLE_CIRCULAR_HALO, [] {
            return std::make_shared<RSNGParticleCircularHalo>();
        }
    }
};

std::shared_ptr<RSNGShaderBase> RSNGShaderBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

} // namespace Rosen
} // namespace OHOS

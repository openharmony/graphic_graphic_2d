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

#include "effect/rs_render_shader_base.h"

#include <unordered_map>

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "effect/rs_render_mask_base.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"

namespace OHOS {
namespace Rosen {
using ShaderCreator = std::function<std::shared_ptr<RSNGRenderShaderBase>()>;

static std::unordered_map<RSNGEffectType, ShaderCreator> creatorLUT = {
    {RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT, [] {
            return std::make_shared<RSNGRenderContourDiagonalFlowLight>();
        }
    },
    {RSNGEffectType::WAVY_RIPPLE_LIGHT, [] {
            return std::make_shared<RSNGRenderWavyRippleLight>();
        }
    },
    {RSNGEffectType::AURORA_NOISE, [] {
            return std::make_shared<RSNGRenderAuroraNoise>();
        }
    },
    {RSNGEffectType::PARTICLE_CIRCULAR_HALO, [] {
            return std::make_shared<RSNGRenderParticleCircularHalo>();
        }
    }
};

std::shared_ptr<Drawing::GEVisualEffect> RSNGRenderShaderHelper::CreateGEFilter(RSNGEffectType type)
{
    return std::make_shared<Drawing::GEVisualEffect>(GetShaderTypeString(type), Drawing::DrawingPaintType::BRUSH);
}

void RSNGRenderShaderHelper::AppendToGEContainer(std::shared_ptr<Drawing::GEVisualEffectContainer>& ge,
    std::shared_ptr<Drawing::GEVisualEffect> geShader)
{
    if (ge == nullptr) {
        return;
    }
    ge->AddToChainedFilter(geShader);
}

void RSNGRenderShaderHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, float value)
{
    geFilter->SetParam(desc, value);
}

void RSNGRenderShaderHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, const Vector4f& value)
{
    geFilter->SetParam(desc, value);
}

void RSNGRenderShaderHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, const std::shared_ptr<RSPath> value)
{
    if (value == nullptr) {
        return;
    }
    geFilter->SetParam(desc, std::make_shared<Drawing::Path>(value->GetDrawingPath()));
}

void RSNGRenderShaderHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, const Vector2f& value)
{
    geFilter->SetParam(desc, std::make_pair(value.x_, value.y_));
}

void RSNGRenderShaderHelper::UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
    const std::string& desc, std::shared_ptr<RSNGRenderMaskBase> value)
{
    geFilter->SetParam(desc, value->GenerateGEShaderMask());
}

std::shared_ptr<RSNGRenderShaderBase> RSNGRenderShaderBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

[[nodiscard]] bool RSNGRenderShaderBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderShaderBase>& val)
{
    std::shared_ptr<RSNGRenderShaderBase> head = nullptr;
    auto current = head;
    for (size_t shaderCount = 0; shaderCount <= EFFECT_COUNT_LIMIT; ++shaderCount) {
        RSNGEffectTypeUnderlying type = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
            ROSEN_LOGE("RSNGRenderShaderBase: Unmarshalling type failed");
            return false;
        }

        if (type == END_OF_CHAIN) {
            val = head;
            return true;
        }

        auto shader = Create(static_cast<RSNGEffectType>(type));
        if (shader && !shader->OnUnmarshalling(parcel)) {
            ROSEN_LOGE("RSNGRenderShaderBase: Unmarshalling shader failed with type %{public}d", type);
            return false;
        }
        if (!current) {
            head = shader; // init head
        } else {
            current->nextEffect_ = shader;
        }
        current = shader;
    }

    ROSEN_LOGE("RSNGRenderShaderBase: Unmarshalling shader count arrive limit(%{public}zu)",
        EFFECT_COUNT_LIMIT);
    return false;
}

void RSNGRenderShaderBase::Dump(std::string& out) const
{
    std::string descStr = ": ";
    std::string splitStr = ", ";

    out += RSNGRenderShaderHelper::GetShaderTypeString(GetType());
    out += descStr;
    DumpProperty(out);
    if (nextEffect_) {
        out += splitStr;
        nextEffect_->Dump(out);
    }
}

} // namespace Rosen
} // namespace OHOS

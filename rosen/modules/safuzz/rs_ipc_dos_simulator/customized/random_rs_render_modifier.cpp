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

#include "customized/random_rs_render_modifier.h"

#include <memory>

#include "animation/rs_spring_model.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSRenderModifier> RandomRSRenderModifier::GetRandomRSRenderModifier()
{
    static const std::vector<std::function<std::shared_ptr<RSRenderModifier>()>> rsRenderModifierRandomGenerator = {
        RandomRSRenderModifier::GetRandomRSGeometryTransRenderModifier,
        RandomRSRenderModifier::GetRandomRSDrawCmdListRenderModifier,

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER, THRESHOLD_TYPE) \
    RandomRSRenderModifier::GetRandomRS##MODIFIER_NAME##RenderModifier,
#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER) \
    DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, Add, MODIFIER_TIER, ZERO)

        DECLARE_NOANIMATABLE_MODIFIER(Particles, RSRenderParticleVector, PARTICLE, Foreground)
#include "modifier/rs_modifiers_def.in"

#undef DECLARE_NOANIMATABLE_MODIFIER
#undef DECLARE_ANIMATABLE_MODIFIER

#define DECLARE_SAFUZZ_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE) \
    RandomRSRenderModifier::GetRandomRS##MODIFIER_NAME##RenderModifier,
#define DECLARE_SAFUZZ_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE) \
    DECLARE_SAFUZZ_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE)

#include "customized/rs_render_modifiers.in"

#undef DECLARE_SAFUZZ_NOANIMATABLE_MODIFIER
#undef DECLARE_SAFUZZ_ANIMATABLE_MODIFIER
    };

    int index = RandomEngine::GetRandomIndex(rsRenderModifierRandomGenerator.size() - 1);
    auto modifier = rsRenderModifierRandomGenerator[index]();
    return modifier;
}

std::shared_ptr<RSGeometryTransRenderModifier> RandomRSRenderModifier::GetRandomRSGeometryTransRenderModifier()
{
    auto renderProperty = std::make_shared<RSRenderProperty<Drawing::Matrix>>(
        RandomData::GetRandomDrawingMatrix(), RandomData::GetRandomUint64());
    return std::make_shared<RSGeometryTransRenderModifier>(renderProperty);
}

std::shared_ptr<RSDrawCmdListRenderModifier> RandomRSRenderModifier::GetRandomRSDrawCmdListRenderModifier()
{
    auto renderProperty = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(
        RandomDrawCmdList::GetRandomDrawCmdList(), RandomData::GetRandomUint64());
    return std::make_shared<RSDrawCmdListRenderModifier>(renderProperty);
}
} // namespace Rosen
} // namespace OHOS

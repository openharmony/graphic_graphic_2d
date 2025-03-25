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

#ifndef SAFUZZ_RANDOM_RS_RENDER_MODIFIER_H
#define SAFUZZ_RANDOM_RS_RENDER_MODIFIER_H

#include "customized/random_rs_render_property_base.h"
#include "modifier/rs_render_modifier.h"
#include "property/rs_properties_def.h"

namespace OHOS {
namespace Rosen {
class RandomRSRenderModifier {
public:
    static std::shared_ptr<RSRenderModifier> GetRandomRSRenderModifier();

private:
    static std::shared_ptr<RSGeometryTransRenderModifier> GetRandomRSGeometryTransRenderModifier();
    static std::shared_ptr<RSDrawCmdListRenderModifier> GetRandomRSDrawCmdListRenderModifier();

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER, THRESHOLD_TYPE) \
    static std::shared_ptr<RS##MODIFIER_NAME##RenderModifier> GetRandomRS##MODIFIER_NAME##RenderModifier()       \
    {                                                                                                            \
        return std::make_shared<RS##MODIFIER_NAME##RenderModifier>(                                              \
            RandomRSRenderPropertyBase::GetRandom<TYPE, true>());                                                \
    }
#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER)                         \
    static std::shared_ptr<RS##MODIFIER_NAME##RenderModifier> GetRandomRS##MODIFIER_NAME##RenderModifier()       \
    {                                                                                                            \
        return std::make_shared<RS##MODIFIER_NAME##RenderModifier>(                                              \
            RandomRSRenderPropertyBase::GetRandom<TYPE, false>());                                               \
    }

DECLARE_NOANIMATABLE_MODIFIER(Particles, RSRenderParticleVector, PARTICLE, Foreground)
#include "modifier/rs_modifiers_def.in"

#undef DECLARE_NOANIMATABLE_MODIFIER
#undef DECLARE_ANIMATABLE_MODIFIER

#define DECLARE_SAFUZZ_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE)                                                  \
    static std::shared_ptr<RS##MODIFIER_NAME##RenderModifier> GetRandomRS##MODIFIER_NAME##RenderModifier()       \
    {                                                                                                            \
        return std::make_shared<RS##MODIFIER_NAME##RenderModifier>(                                              \
            RandomRSRenderPropertyBase::GetRandom<TYPE, true>());                                                \
    }
#define DECLARE_SAFUZZ_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE)                                                \
    static std::shared_ptr<RS##MODIFIER_NAME##RenderModifier> GetRandomRS##MODIFIER_NAME##RenderModifier()       \
    {                                                                                                            \
        return std::make_shared<RS##MODIFIER_NAME##RenderModifier>(                                              \
            RandomRSRenderPropertyBase::GetRandom<TYPE, false>());                                               \
    }

#include "customized/rs_render_modifiers.in"

#undef DECLARE_SAFUZZ_NOANIMATABLE_MODIFIER
#undef DECLARE_SAFUZZ_ANIMATABLE_MODIFIER
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_RS_RENDER_MODIFIER_H

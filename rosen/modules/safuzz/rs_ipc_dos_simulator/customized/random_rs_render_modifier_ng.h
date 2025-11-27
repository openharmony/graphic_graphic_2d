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
#include "modifier_ng/rs_render_modifier_ng.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "property/rs_properties_def.h"

namespace OHOS {
namespace Rosen {
class RandomRSRenderModifier {
public:
    static std::shared_ptr<ModifierNG::RSRenderModifier> GetRandomRSRenderModifier();
    static ModifierNG::RSPropertyType GetRandomRSPropertyType();
    static ModifierNG::RSModifierType GetRandomRSModifierType();
};

enum class RandomRSPropertyType : uint16_t {
#define X(name) name,
#include "modifier_ng/rs_property_ng_type.in"
#undef X
    MAX,
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_RS_RENDER_MODIFIER_H

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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_MODIFIER_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_MODIFIER_MANAGER_H

#include <memory>
#include <unordered_map>

#include "common/rs_common_def.h"

namespace OHOS::Rosen {
namespace ModifierNG {
class RSModifier;

class RSC_EXPORT RSModifierManagerNG : public std::enable_shared_from_this<RSModifierManagerNG> {
public:
    RSModifierManagerNG() = default;
    virtual ~RSModifierManagerNG() = default;
    RSModifierManagerNG(const RSModifierManagerNG&) = delete;
    RSModifierManagerNG(const RSModifierManagerNG&&) = delete;
    RSModifierManagerNG& operator=(const RSModifierManagerNG&) = delete;
    RSModifierManagerNG& operator=(const RSModifierManagerNG&&) = delete;

    void addModifier(std::shared_ptr<ModifierNG::RSModifier> modifier);
    void removeModifier(std::shared_ptr<ModifierNG::RSModifier> modifier);

protected:
    std::unordered_map<ModifierId, std::shared_ptr<ModifierNG::RSModifier>> modifiers_;
};
} // namespace ModifierNG
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_MODIFIER_MANAGER_H
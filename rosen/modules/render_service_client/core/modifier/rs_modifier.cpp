
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "modifier/rs_modifier.h"

#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_modifier_manager_map.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
void RSModifier::AttachProperty(const std::shared_ptr<RSPropertyBase>& property)
{
#ifndef MODIFIER_NG
    if (property != nullptr && property_ != nullptr) {
        property->target_ = property_->target_;
        property->SetIsCustom(true);
        property->AttachModifier(shared_from_this());
        property->MarkModifierDirty();
    }
#endif
}

void RSModifier::SetDirty(const bool isDirty, const std::shared_ptr<RSModifierManager>& modifierManager)
{
#ifndef MODIFIER_NG
    if (!isDirty) {
        isDirty_ = isDirty;
        return;
    }

    if (isDirty_) {
        return;
    }
    isDirty_ = isDirty;

    if (modifierManager == nullptr) {
        ROSEN_LOGE("multi-instance, Modifier manager is null while mark modifier dirty Id: %{public}" PRIu64 "!",
            GetPropertyId());
        return;
    }

    modifierManager->AddModifier(shared_from_this());
#endif
}

void RSModifier::ResetRSNodeExtendModifierDirty()
{
#ifndef MODIFIER_NG
    if (property_ == nullptr) {
        return;
    }

    auto target = property_->target_.lock();
    if (target == nullptr) {
        return;
    }

    target->ResetExtendModifierDirty();
#endif
}
} // namespace Rosen
} // namespace OHOS

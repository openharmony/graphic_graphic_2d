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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_FACTORY_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_FACTORY_H

#include "boot_associative_display_strategy.h"
#include "boot_compatible_display_strategy.h"
#include "boot_independent_display_strategy.h"
#include "util.h"

namespace OHOS {
class BootAnimationFactory {
public:
    static std::shared_ptr<BootAnimationStrategy> GetBootStrategy(BootStrategyType type)
    {
        std::shared_ptr<BootAnimationStrategy> strategy;
        switch (type) {
            case BootStrategyType::ASSOCIATIVE:
                strategy = std::make_shared<BootAssociativeDisplayStrategy>();
                break;
            case BootStrategyType::COMPATIBLE:
                strategy = std::make_shared<BootCompatibleDisplayStrategy>();
                break;
            case BootStrategyType::INDEPENDENT:
                strategy = std::make_shared<BootIndependentDisplayStrategy>();
                break;
            default:
                strategy = nullptr;
                break;
        }
        return strategy;
    }
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_FACTORY_H

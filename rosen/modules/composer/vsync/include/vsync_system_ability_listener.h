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

#ifndef VSYNC_SYSTEM_ABILITY_LISTENER_H
#define VSYNC_SYSTEM_ABILITY_LISTENER_H

#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace Rosen {
class VSyncSystemAbilityListener : public SystemAbilityStatusChangeStub {
public:
    VSyncSystemAbilityListener(const std::string& threadName, const std::string& uid, const std::string& pid,
        const std::string& tid) : threadName_(threadName), uid_(uid), pid_(pid), tid_(tid) {}
    virtual ~VSyncSystemAbilityListener() = default;

    virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    virtual void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

private:
    std::string threadName_;
    std::string uid_;
    std::string pid_;
    std::string tid_;
};
} // namespace Rosen
} // namespace OHOS
#endif
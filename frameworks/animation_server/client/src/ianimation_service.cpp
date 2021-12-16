/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <ianimation_service.h>

#include <gslogger.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("IAnimationService");
} // namespace

GSError IAnimationService::Init()
{
    if (animationService != nullptr) {
        return GSERROR_OK;
    }

    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        GSLOG2HI(ERROR) << "SystemAbilityManager is nullptr";
        return GSERROR_CONNOT_CONNECT_SAMGR;
    }

    auto robj = sam->GetSystemAbility(ANIMATION_SERVER_SA_ID);
    if (robj == nullptr) {
        GSLOG2HI(ERROR) << "Remote Object is nullptr";
        return GSERROR_CONNOT_CONNECT_SERVER;
    }

    animationService = iface_cast<IAnimationService>(robj);
    if (animationService == nullptr) {
        GSLOG2HI(ERROR) << "Cannot find proxy";
        return GSERROR_PROXY_NOT_INCLUDE;
    }

    return GSERROR_OK;
}

sptr<IAnimationService> IAnimationService::Get()
{
    return animationService;
}
} // namespace OHOS

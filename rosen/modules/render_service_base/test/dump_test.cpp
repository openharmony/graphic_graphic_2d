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

#include <iostream>
#include <vector>
#include<unistd.h>

#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "platform/ohos/rs_irender_service.h"
#include "platform/ohos/rs_render_service_proxy.h"
#include "platform/common/rs_log.h"


using namespace OHOS;
using namespace OHOS::Rosen;
// dump
int main()
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIRenderService> renderService = iface_cast<RSRenderServiceProxy>(remoteObject);
    int sfd[2];
    if (pipe(sfd) != 0) {
        return 0;
    }
    std::vector<std::u16string> args;
    (void)remoteObject->Dump(sfd[1], args);
    char buf[1024];
    bzero(buf, sizeof(buf));
    while (read(sfd[0], buf, sizeof(buf)-1) > 0) {
        std::cout<< buf;
        bzero(buf, sizeof(buf));
    };
}

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

#include "service_dumper.h"
#include "system_ability_definition.h"

namespace {
    constexpr const int32_t DUMP_HELP_RET_CODE = 1;
}

using namespace OHOS;

// dump
int main(int argc, char *argv[])
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> renderService = samgr->GetSystemAbility(RENDER_SERVICE);
    if (renderService == nullptr) {
        fprintf(stderr, "Can't find render service!");
        return -1;
    }

    Detail::ServiceDumper dumper(renderService, "Render_Service", 10000); // timeout: 10000 ms.
    int retCode = dumper.PrintHelpInfo(argc, argv);
    if (retCode == DUMP_HELP_RET_CODE) {
        return retCode;
    }
    return dumper.Run(argc, argv);
}

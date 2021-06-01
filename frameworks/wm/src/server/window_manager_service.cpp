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
#include "window_manager_server.h"

#include <unistd.h>

#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <vsync_module.h>

#include "iwindow_manager_service.h"
#include "window_manager_hilog.h"

using namespace OHOS;

namespace {
constexpr int SLEEP_TIME = 500 * 1000;

int StartServer()
{
    WMLOG_I("wms start_server");

    int result = 0;
    while (1) {
        auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sm != nullptr) {
            sptr<IRemoteObject> service = new WindowManagerServer();
            result = sm->AddSystemAbility(WINDOW_MANAGER_ID, service);
            break;
        }

        WMLOG_I("wms start_server SystemAbilityManager is nullptr");
        usleep(SLEEP_TIME);
    }

    return result;
}
} // namespace

int main()
{
    int ret = StartServer();
    printf("StartServer: %d\n", ret);

    ret = VsyncModule::GetInstance()->Start();
    printf("VsyncModule->Start return %d\n", ret);
    if (ret < 0) {
        return ret;
    }

    IPCSkeleton::JoinWorkThread();
}

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

#include <event_handler.h>
#include <gslogger.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "animation_server.h"

using namespace OHOS;

namespace {
DEFINE_HILOG_LABEL("AnimationServer::Main");
} // namespace

int main(int argc, const char *argv[])
{
    GSLOG2HI(INFO) << "main start";
    auto server = new AnimationServer();
    auto ret = server->Init();
    if (ret != GSERROR_OK) {
        GSLOG2SE(ERROR) << "Server init failed: " << GSErrorStr(ret);
        return ret;
    }

    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam->AddSystemAbility(ANIMATION_SERVER_SA_ID, server) != ERR_OK) {
        GSLOG2HI(ERROR) << "AddSystemAbility failed: " << ANIMATION_SERVER_SA_ID;
        return 1;
    }

    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&AnimationServer::Init, server));
    runner->Run();
    GSLOG2HI(INFO) << "main stop";
    return 0;
}

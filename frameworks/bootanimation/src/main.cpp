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

#include <display_type.h>
#include <display_manager.h>
#include <vsync_helper.h>
#include "boot_animation.h"
#include "util.h"

using namespace OHOS;

int main(int argc, const char *argv[])
{
    LOGI("main enter");
    WaitRenderServiceInit();

    auto& dms = OHOS::Rosen::DisplayManager::GetInstance();
    auto displays = dms.GetAllDisplays();
    while (displays.empty()) {
        LOGI("displays is empty, retry to get displays");
        displays = dms.GetAllDisplays();
        sleep(1);
    }

    BootAnimation bootAnimation;
    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&BootAnimation::Init, &bootAnimation,
        displays[0]->GetWidth(), displays[0]->GetHeight(), handler, runner));
    handler->PostTask(std::bind(&BootAnimation::PlaySound, &bootAnimation));
    runner->Run();

    LOGI("main exit");
    return 0;
}

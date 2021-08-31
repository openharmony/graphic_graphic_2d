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

#include <vsync_module.h>

#include "vsync_module_c.h"

using namespace OHOS;

int VsyncModuleStart()
{
    return VsyncModule::GetInstance()->Start();
}

int VsyncModuleStop()
{
    return VsyncModule::GetInstance()->Stop();
}

int VsyncModuleTrigger()
{
    return VsyncModule::GetInstance()->Trigger();
}

int VsyncModuleIsRunning()
{
    return VsyncModule::GetInstance()->IsRunning() ? 1 : 0;
}

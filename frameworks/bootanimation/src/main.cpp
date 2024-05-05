/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "boot_animation_controller.h"
#include "log.h"

using namespace OHOS;

int main(int argc, const char *argv[])
{
    LOGI("main enter");
    std::shared_ptr<BootAnimationController> controller = std::make_shared<BootAnimationController>();
    controller->Start();
    LOGI("main exit");
    return 0;
}

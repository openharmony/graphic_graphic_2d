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

#include "ipc/graphic_dumper_info_listener_death_recipient.h"
#include "graphic_dumper_server.h"
#include "ipc/igraphic_dumper_command.h"
#include "graphic_common.h"
#include "graphic_dumper_hilog.h"
namespace OHOS {
void GraphicDumperInfoListenerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    if (object == nullptr) {
        return;
    }
    GraphicDumperServer::GetInstance()->RemoveInfoListener(object);
}
} // namespace OHOS

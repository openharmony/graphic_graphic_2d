/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_pipline_dump_manager.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
void RSPiplineDumpManager::CmdExec(std::unordered_set<std::u16string>& argSets, std::string &out, sptr<RSIDumpCallback> callback)
{
    out += "\nRSProcessDump pid:";
    out += std::to_string(pid_);
    out += " screenId:";
    out += std::to_string(screenId_);
    RSDumpManager::CmdExec(argSets, out);
    if (callback == nullptr) {
        RS_LOGW("CmdExec called with null RSIDumpCallback. Dump result discarded");
        return;
    }
    callback->OnDumpResult(out);
}

void RSPiplineDumpManager::SetPid(int pid)
{
    pid_ = pid;
}

void RSPiplineDumpManager::SetScreenId(unsigned long screenId)
{
    screenId_ = screenId;
}
}
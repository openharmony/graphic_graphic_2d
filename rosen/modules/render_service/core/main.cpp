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

#include <sched.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <hilog/log.h>

#include "pipeline/rs_render_service.h"

int main(int argc, const char *argv[])
{
    signal(SIGPIPE, SIG_IGN);

    setpriority(PRIO_PROCESS, 0, -8);

    struct sched_param param = {0};
    param.sched_priority = 2;
    if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
        OHOS::HiviewDFX::HiLog::Error(::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "renderserver"},
            "%{public}s: Couldn't set SCHED_FIFO.", __func__);
    } else {
        OHOS::HiviewDFX::HiLog::Error(::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "renderserver"},
            "%{public}s: set SCHED_FIFO succ.", __func__);
    }

    if (!OHOS::Rosen::RSRenderService::GetInstance().Init()) {
        OHOS::HiviewDFX::HiLog::Error(::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "renderserver"},
            "%{public}s: render service init failed.", __func__);
    } else {
        OHOS::Rosen::RSRenderService::GetInstance().Run();
    }

    return 0;
}

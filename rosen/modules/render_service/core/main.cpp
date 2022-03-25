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

#include "platform/common/rs_log.h"
#include "pipeline/rs_render_service.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, const char *argv[])
{
    signal(SIGPIPE, SIG_IGN);

    setpriority(PRIO_PROCESS, 0, -8);

    struct sched_param param = {0};
    param.sched_priority = 2;
    if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
        RS_LOGE("RSRenderService Couldn't set SCHED_FIFO.");
    } else {
        RS_LOGE("RSRenderService set SCHED_FIFO succeed.");
    }

    sptr<RSRenderService> renderService(new RSRenderService());
    if (!renderService->Init()) {
        RS_LOGE("RSRenderService init failed.");
        return -1;
    }

    renderService->Run();

    return 0;
}

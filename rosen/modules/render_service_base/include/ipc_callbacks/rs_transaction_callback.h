/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_H
#define ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_H

#include <vector>
#include <iremote_broker.h>
#include <queue>
#include <mutex>
#include <iremote_stub.h>
#include <iremote_proxy.h>
#include "common/rs_common_def.h"
#include "graphic_common.h"
#ifdef ROSEN_OHOS
#include "sync_fence.h"
#endif

namespace OHOS {
namespace Rosen {
struct OnCompletedRet {
    uint64_t nodeId;
    pid_t clientPid;
    uint64_t queueId;

#ifdef ROSEN_OHOS
    sptr<SyncFence> releaseFence;
#endif

    uint32_t bufferSeqNum;
    bool isActiveGame;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_H

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

#ifndef RS_CORE_FEATURE_HPAE_OFFLINE_RESULT_H
#define RS_CORE_FEATURE_HPAE_OFFLINE_RESULT_H
#include <cstdint>
#include <buffer_handle.h>
#include "common/rs_common_def.h"
#include "pipeline/rs_surface_handler.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
struct ProcessOfflineResult {
    sptr<IConsumerSurface> consumer = nullptr;
    Rect damageRect = {0, 0, 0, 0};
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    GraphicIRect bufferRect = {0, 0, 0, 0};
    bool taskSuccess = false;
};

struct ProcessOfflineFuture {
    bool done = false;
    bool timeout = false;
    ProcessOfflineResult result;
    std::mutex mtx;
    std::condition_variable cv;
};
}
}
#endif // RS_CORE_FEATURE_HPAE_OFFLINE_RESULT_H
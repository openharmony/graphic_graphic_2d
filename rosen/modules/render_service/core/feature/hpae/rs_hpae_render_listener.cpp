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

#include "rs_hpae_render_listener.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSHpaeRenderListener::~RSHpaeRenderListener() {}

RSHpaeRenderListener::RSHpaeRenderListener(std::weak_ptr<RSSurfaceHandler> surfaceHandler)
    : surfaceHandler_(surfaceHandler) {}

void RSHpaeRenderListener::OnBufferAvailable()
{
    auto surfaceHandler = surfaceHandler_.lock();
    if (surfaceHandler == nullptr) {
        RS_LOGE("RSHpaeRenderListener::OnBufferAvailable surfaceHandler is nullptr");
        return;
    }
    RS_LOGD("RSHpaeRenderListener::OnBufferAvailable node id:%{public}" PRIu64, surfaceHandler->GetNodeId());
    surfaceHandler->IncreaseAvailableBuffer();
}
}
}
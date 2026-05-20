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

#ifndef RS_TUNNEL_LAYER_STATE_HANDLER_H
#define RS_TUNNEL_LAYER_STATE_HANDLER_H

#include <surface_buffer.h>

#include "common/rs_common_def.h"

namespace OHOS::Rosen {
class RSTunnelLayerStateHandler {
public:
    virtual ~RSTunnelLayerStateHandler() = default;
    virtual void HandleLayerStateChanged(NodeId nodeId, LayerStateChange state, uint64_t tunnelLayerGeneration) = 0;
};
} // namespace OHOS::Rosen

#endif // RS_TUNNEL_LAYER_STATE_HANDLER_H

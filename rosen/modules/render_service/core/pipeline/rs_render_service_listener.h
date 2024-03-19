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
#ifndef RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_H
#define RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_H

#include <ibuffer_consumer_listener.h>
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

#include "rs_render_service.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceListener : public IBufferConsumerListener {
public:
    RSRenderServiceListener(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode);
    ~RSRenderServiceListener() override;
    void OnBufferAvailable() override;
    void OnTunnelHandleChange() override;
    void OnCleanCache() override;
    void OnGoBackground() override;
private:
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_H
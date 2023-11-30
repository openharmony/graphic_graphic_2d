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

#ifndef RENDER_SERVICE_CORE_RS_RCD_RENDER_LISTENER_H
#define RENDER_SERVICE_CORE_RS_RCD_RENDER_LISTENER_H

#include <ibuffer_consumer_listener.h>
#include "rs_rcd_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RSRcdRenderListener : public IBufferConsumerListener {
public:
    explicit RSRcdRenderListener(std::weak_ptr<RSRcdSurfaceRenderNode> surfaceRenderNode);
    ~RSRcdRenderListener() override;
    void OnBufferAvailable() override;
    void OnGoBackground() override;

private:
    std::weak_ptr<RSRcdSurfaceRenderNode> surfaceRenderNode_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_RCD_RENDER_LISTENER_H
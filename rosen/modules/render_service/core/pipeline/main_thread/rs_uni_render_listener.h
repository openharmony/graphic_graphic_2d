/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_LISTENER_H
#define RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_LISTENER_H

#include <memory>
#include <mutex>
#include <ibuffer_consumer_listener.h>
#include "common/rs_common_def.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_surface_handler.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderListener : public IBufferConsumerListener {
public:
    RSUniRenderListener(std::weak_ptr<RSSurfaceHandler> surfaceHandler);
    ~RSUniRenderListener() override;
    void OnBufferAvailable() override;

private:
    std::weak_ptr<RSSurfaceHandler> surfaceHandler_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_LISTENER_H

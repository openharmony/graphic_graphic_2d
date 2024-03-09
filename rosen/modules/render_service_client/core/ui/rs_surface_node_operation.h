/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_OPERATION_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_OPERATION_H

#include <string>

#include "common/rs_macros.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSSurfaceNodeOperation final {
public:
    static RSSurfaceNodeOperation& GetInstance();
    ~RSSurfaceNodeOperation() = default;
    void ProcessRSSurfaceNode(std::string xcomponentId,
        uint64_t nodeId, float x, float y, std::shared_ptr<RSSurfaceNode> surfaceNode);

private:
    RSSurfaceNodeOperation() = default;
    RSSurfaceNodeOperation(const RSSurfaceNodeOperation&) = delete;
    RSSurfaceNodeOperation& operator=(const RSSurfaceNodeOperation&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_OPERATION_H
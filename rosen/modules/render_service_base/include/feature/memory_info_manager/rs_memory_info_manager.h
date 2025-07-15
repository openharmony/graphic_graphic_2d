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

#ifndef RS_MEMORY_INFO_MANAGER_H
#define RS_MEMORY_INFO_MANAGER_H

#include <memory>

#include "pipeline/rs_surface_handler.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RSMemoryInfoManager {
public:
    RSMemoryInfoManager() = default;
    ~RSMemoryInfoManager() noexcept = default;
    
    static void SetSurfaceMemoryInfo(bool onTree, const std::shared_ptr<RSSurfaceHandler> handler);
    static void SetImageMemoryInfo(const std::shared_ptr<Media::PixelMap> pixelMap);
    static void RecordNodeOnTreeStatus(bool flag, NodeId nodeId, NodeId instanceRootNodeId);
    static void ResetRootNodeStatusChangeFlag(NodeId nodeId, NodeId instanceRootNodeId);
};
} // Rosen
} // OHOS
#endif // RS_MEMORY_INFO_MANAGER_H
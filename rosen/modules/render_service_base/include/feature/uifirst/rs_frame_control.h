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

#ifndef RS_FRAME_CONTROL_H
#define RS_FRAME_CONTROL_H

#include <set>
#include "common/rs_macros.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSFrameControlTool {
public:
    void SetAppWindowNodeId(NodeId id);
    bool CheckAppWindowNodeId(NodeId id);
    void SetNodeIdForFrameControl(RSSurfaceRenderNode& node);
    static RSFrameControlTool& Instance();
private:
    // refreshAppWindowNodeId_ cannot be operated concurrently
    NodeId refreshAppWindowNodeId_;
};
}
}
#endif
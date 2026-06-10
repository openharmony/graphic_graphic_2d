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


#ifndef RS_DIRECT_COMPOSITION_HELPER_H
#define RS_DIRECT_COMPOSITION_HELPER_H

#include <atomic>

namespace OHOS {
namespace Rosen {
struct RSDirectCompositionHelper {
    bool isLastFrameDirectComposition_ = false;
    // True only when the previous frame actually traversed the GPU composition
    // path (needTraverseNodeTree=true with rendering). Unlike
    // !isLastFrameDirectComposition_, this is not triggered by "nothing to
    // update" frames or buffer-not-updated frames, which pose no HdiOutput
    // conflict with the listener.
    bool lastFrameDidGpuRender_ = false;
    // Consecutive frames where DoDirectComposition actually ran and returned
    // true. GPU-composition / DoComp-failure frames reset it to 0; "nothing
    // to update" frames leave it unchanged. Tunnel can only enter ACTIVE when
    // this reaches TUNNEL_STABLE_THRESHOLD.
    std::atomic<uint32_t> consecutiveDoCompSuccessCount_ {0};
};

} // namespace Rosen
} // namespace OHOS
#endif
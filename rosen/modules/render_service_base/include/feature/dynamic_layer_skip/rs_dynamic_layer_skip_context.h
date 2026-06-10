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

#ifndef RENDER_SERVICE_BASE_FEATURE_RS_DYNAMIC_LAYER_SKIP_CONTEXT_H
#define RENDER_SERVICE_BASE_FEATURE_RS_DYNAMIC_LAYER_SKIP_CONTEXT_H

#include <cstddef>
#include <vector>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum LayerDrawContent : size_t {
    SELF = 0,       // whether the node itself has draw content
    SUBTREE = 1,    // whether the subtree has draw content, determined by all its descendants
    UPDATE = 2,     // whether the node has update content in current frame, used for dynamic layer skip optimization
    SUBTREE_UPDATE = 3, // cumulative flag: whether subtree has updates; consumed during layer validity detection
    MAX = 4
};

class RSDynamicLayerSkipController;
struct RSB_EXPORT LayerSkipContext {
    bool screenLayerInvalid_ = false;
    std::vector<NodeId> relevantSurfaceNodeIds_ = {};
    void SyncFrom(const RSDynamicLayerSkipController& controller);
    void Reset();
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_RS_DYNAMIC_LAYER_SKIP_CONTEXT_H
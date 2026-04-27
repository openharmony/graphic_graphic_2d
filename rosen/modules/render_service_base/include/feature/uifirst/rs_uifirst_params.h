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

#ifndef RS_UIFIRST_PARAMS_H
#define RS_UIFIRST_PARAMS_H

#include <atomic>
#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen {

/*
 * Memory alignment rule for adding new variables:
 * 16-byte -> 8-byte -> 4-byte -> 1-byte
 * Add new variables in the corresponding section to minimize padding.
 */
struct RSUIFirstRenderParams {
    // 16-byte types (RectI: 4 x int32)
    RectI childrenDirtyRect;
    RectI visibleFilterRect;

    // 8-byte types (NodeId, Vector2f)
    NodeId startingWindowId = INVALID_NODEID;
    Vector2f lastCacheSize = {0.f, 0.f};

    // 4-byte types (enums: MultiThreadCacheType, Gravity)
    MultiThreadCacheType cacheType = MultiThreadCacheType::NONE;
    Gravity frameGravity = Gravity::TOP_LEFT;

    // 1-byte types (bools)
    bool parentEnabled = false;
    bool leashAllEnabled = false;
    bool isPartialSynced = false;
};

/*
 * Memory alignment rule for adding new variables:
 * 8-byte -> 4-byte -> atomic(4-byte) -> 1-byte
 * Add new variables in the corresponding section to minimize padding.
 * Note: std::atomic<bool> requires 4-byte alignment.
 */
struct RSUIFirstNodeState {
    // 8-byte types (int64_t)
    int64_t uifirstStartTime = -1;

    // 4-byte types (enums)
    MultiThreadCacheType lastFrameCacheType = MultiThreadCacheType::NONE;
    RSUIFirstSwitch switchMode = RSUIFirstSwitch::NONE;

    // Atomic types (4-byte aligned, for thread safety)
    std::atomic<bool> isWaitFirstFrame {false};

    // 1-byte types (bools)
    bool isEnabled = false;
    bool hasAppWindow = false;
    bool forceUIFirst = false;
    bool forceStateChanged = false;
    bool forceUpdate = false;
    bool isForceMarked = false;
    bool forceDrawWithSkipped = false;
    bool contentDirty = false;
    bool selfAndParentShouldPaint = true;
    bool needSync = false;
    bool skipPartialSync = false;
    bool isTargetDfxEnabled = false;
    bool subThreadAssignable = false;
};

} // namespace OHOS::Rosen

#endif // RS_UIFIRST_PARAMS_H
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

#ifndef RS_BACKGROUND_REBUILD_PARAM_H
#define RS_BACKGROUND_REBUILD_PARAM_H

#include <cstdint>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
// GoStop node map handling mode: 0 do nothing, 1 only RemoveSurfaceNodeMap, 2 only DestroyTokenNode
enum class GoStopNodeMapMode : uint8_t {
    NONE = 0,
    REMOVE_SURFACE_NODE_MAP,
    DESTROY_TOKEN_NODE,
};

class RSB_EXPORT RSBackgroundRebuildParam {
public:
    static RSBackgroundRebuildParam& Instance();

    void SetBackgroundRebuildEnabled(bool isEnable);
    bool IsBackgroundRebuildEnabled() const;

    // Written once during feature config parsing at render service startup and read-only afterwards,
    // so the plain members need no synchronization.
    void SetGoStopNodeMapMode(GoStopNodeMapMode mode);
    GoStopNodeMapMode GetGoStopNodeMapMode() const;

private:
    RSBackgroundRebuildParam() = default;
    bool isBackgroundRebuildEnabled_ = false;
    // Keep consistent with the default of isBackgroundRebuildEnabled_ (disabled -> RemoveSurfaceNodeMap)
    GoStopNodeMapMode goStopNodeMapMode_ = GoStopNodeMapMode::REMOVE_SURFACE_NODE_MAP;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_BACKGROUND_REBUILD_PARAM_H
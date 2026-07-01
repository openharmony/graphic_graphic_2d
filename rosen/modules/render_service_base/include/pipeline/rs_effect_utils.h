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

#ifndef RENDER_SERVICE_BASE_CORE_PIPELINE_RS_EFFECT_UTILS_H
#define RENDER_SERVICE_BASE_CORE_PIPELINE_RS_EFFECT_UTILS_H

#include "effect/rs_render_effect_common_def.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"

namespace OHOS {

namespace Rosen {
class RSB_EXPORT RSEffectUtils {
public:
    static bool AccumulateFilterRenderContext(RSRenderNode& node,
        const RSRenderNode& rootNode, FilterRenderContext& context);

    // true：causes filter cache to be generated offscreen.
    static bool IsOffscreenForFilterCache(RSRenderNode& node);

    // true: has filter that only depends on background
    static bool HasBackgroundDependentFilter(const RSProperties& properties);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CORE_PIPELINE_RS_EFFECT_UTILS_H

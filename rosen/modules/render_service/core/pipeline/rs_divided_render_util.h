/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_RENDER_SERVICE_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_RENDER_SERVICE_UTIL_H

#include "rs_base_render_util.h"
#include "rs_composer_adapter.h"

namespace OHOS {
namespace Rosen {
class RSDividedRenderUtil {
public:
    static void InitEnableClient();
    static bool IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info);
    static void SetNeedClient(bool flag);
    static BufferDrawParam CreateBufferDrawParam(
        const RSSurfaceRenderNode& node,
        bool inLocalCoordinate = false,
        bool isClipHole = false,
        bool forceCPU = false,
        bool setColorFilter = true);
private:
    static bool IsForceClient();
    static void CalculateSurfaceNodeClipRects(
        const RSSurfaceRenderNode& node,
        const RectF& absBounds,
        const RectF& localBounds,
        bool inLocalCoordinate,
        BufferDrawParam& params);

    static bool enableClient;
};
} // Rosen
} // OHOS
#endif

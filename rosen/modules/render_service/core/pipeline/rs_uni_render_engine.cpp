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

#include "pipeline/rs_uni_render_engine.h"

namespace OHOS {
namespace Rosen {
void RSUniRenderEngine::DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
    BufferDrawParam& params, PreProcessFunc preProcess, PostProcessFunc postProcess)
{
    canvas.save();
    canvas.concat(params.matrix);
    if (!params.useCPU) {
        RegisterDeleteBufferListener(node.GetConsumer());
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
    canvas.restore();
}
} // namespace Rosen
} // namespace OHOS

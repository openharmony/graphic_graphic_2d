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

#ifndef RS_CORE_PIPELINE_UNI_RENDER_MIRROR_PROCESSOR_H
#define RS_CORE_PIPELINE_UNI_RENDER_MIRROR_PROCESSOR_H

#include "rs_processor.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderMirrorProcessor : public RSProcessor {
public:
    RSUniRenderMirrorProcessor() = default;
    ~RSUniRenderMirrorProcessor() noexcept override = default;

    bool Init(ScreenId id, int32_t offsetX, int32_t offsetY, ScreenId mirroredId) override;
    void ProcessSurface(RSSurfaceRenderNode& node) override;
    void ProcessDisplaySurface(RSDisplayRenderNode& node) override;
    void PostProcess() override;

    std::unique_ptr<RSPaintFilterCanvas> GetCanvas()
    {
        return std::move(canvas_);
    }
private:
    void SetBufferTimeStamp();

    std::unique_ptr<RSRenderFrame> renderFrame_;
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_UNI_RENDER_MIRROR_PROCESSOR_H

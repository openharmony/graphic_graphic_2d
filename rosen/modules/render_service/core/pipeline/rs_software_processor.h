/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RS_SOFTWARE_PROCESSOR_H
#define RS_SOFTWARE_PROCESSOR_H

#include "pipeline/rs_processor.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {

class RSSoftwareProcessor : public RSProcessor {
public:
    RSSoftwareProcessor();
    ~RSSoftwareProcessor() override;
    void ProcessSurface(RSSurfaceRenderNode& node) override;
    void Init(ScreenId id, int32_t offsetX, int32_t offsetY) override;
    void PostProcess() override;

private:
    sptr<Surface> producerSurface_;
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
    ScreenInfo currScreenInfo_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_SOFTWARE_PROCESSOR_H

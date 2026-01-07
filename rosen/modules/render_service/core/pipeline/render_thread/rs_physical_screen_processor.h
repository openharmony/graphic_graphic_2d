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

#ifndef RS_CORE_PIPELINE_PHYSICAL_SCREEN_PROCESSOR_H
#define RS_CORE_PIPELINE_PHYSICAL_SCREEN_PROCESSOR_H

#include "rs_composer_adapter.h"
#include "pipeline/rs_processor.h"

namespace OHOS {
namespace Rosen {
class RSPhysicalScreenProcessor : public RSProcessor {
public:
    static inline constexpr RSProcessorType Type = RSProcessorType::PHYSICAL_SCREEN_PROCESSOR;
    RSProcessorType GetType() const override
    {
        return Type;
    }
    
    RSPhysicalScreenProcessor(const std::shared_ptr<RSRenderComposerClient>& composerClient);
    ~RSPhysicalScreenProcessor() noexcept override;

    bool Init(RSScreenRenderNode& node, int32_t offsetX, int32_t offsetY,
              std::shared_ptr<RSBaseRenderEngine> renderEngine) override;
    void ProcessSurface(RSSurfaceRenderNode& node) override;
    void ProcessScreenSurface(RSScreenRenderNode& node) override;
    void ProcessRcdSurface(RSRcdSurfaceRenderNode& node) override;
    void PostProcess() override;
private:
    void Redraw(const sptr<Surface>& surface, const std::vector<RSLayerPtr>& layers);

    std::unique_ptr<RSComposerAdapter> composerAdapter_ = nullptr;
    std::vector<RSLayerPtr> layers_;
    std::shared_ptr<RSRenderComposerClient> composerClient_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_PHYSICAL_SCREEN_PROCESSOR_H

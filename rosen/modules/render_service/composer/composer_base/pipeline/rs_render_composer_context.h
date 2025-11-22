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

#ifndef RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_CONTEXT_H
#define RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_CONTEXT_H

#include "common/rs_common_def.h"
#include "rs_layer.h"
#include "rs_layer_transaction_data.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSRenderComposerContext {
public:
    RSRenderComposerContext() = default;
    ~RSRenderComposerContext() = default;

    std::vector<std::shared_ptr<RSLayer>> GetRSLayersVec();
    void SetRSLayersVec(std::vector<std::shared_ptr<RSLayer>>&& layersVec);
    void ClearAllRSLayers();

private:
    std::mutex mutex_;
    std::vector<std::shared_ptr<RSLayer>> layersVec_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_CONTEXT_H
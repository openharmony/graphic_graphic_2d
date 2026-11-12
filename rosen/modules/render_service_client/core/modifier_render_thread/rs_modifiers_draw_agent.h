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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_AGENT_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_AGENT_H

#include "modifier_render_thread/rs_modifiers_draw.h"

namespace OHOS::Rosen {
class RSUIContext;

class RSC_EXPORT RSModifiersDrawAgent final {
public:
    RSModifiersDrawAgent();
    ~RSModifiersDrawAgent();

    RSModifiersDrawAgent(const RSModifiersDrawAgent&) = delete;
    RSModifiersDrawAgent(const RSModifiersDrawAgent&&) = delete;
    RSModifiersDrawAgent& operator=(const RSModifiersDrawAgent&) = delete;
    RSModifiersDrawAgent& operator=(const RSModifiersDrawAgent&&) = delete;

private:
    void CommitTransaction(std::shared_ptr<RSUIContext> uiContext,
        std::shared_ptr<RSRenderPipelineClient> renderPiplineClient,
        std::unique_ptr<RSTransactionData>& transactionData, std::atomic<uint32_t>& transactionDataIndex,
        uint32_t commitIndex);

    void WaitAllTasksFinish();

    void Destroy();

    std::shared_ptr<RSModifiersDraw> modifiersDraw_ = nullptr;

    friend class RSUIContext;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_AGENT_H

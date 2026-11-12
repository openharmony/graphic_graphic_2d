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

#include "modifier_render_thread/rs_modifiers_draw_agent.h"

#include "ui/rs_ui_context.h"

namespace OHOS::Rosen {
RSModifiersDrawAgent::RSModifiersDrawAgent()
{
    modifiersDraw_ = std::make_shared<RSModifiersDraw>();
}

RSModifiersDrawAgent::~RSModifiersDrawAgent() = default;

void RSModifiersDrawAgent::CommitTransaction(std::shared_ptr<RSUIContext> uiContext,
    std::shared_ptr<RSRenderPipelineClient> renderPiplineClient, std::unique_ptr<RSTransactionData>& transactionData,
    std::atomic<uint32_t>& transactionDataIndex, uint32_t commitIndex)
{
    modifiersDraw_->ScheduleTask(
        [modifiersDraw = modifiersDraw_, uiContext, renderPiplineClient, data = std::move(transactionData),
            &transactionDataIndex, index = commitIndex]() mutable {
            RS_TRACE_NAME_FMT("Do CommitTransaction, index=%u", index);
            modifiersDraw->CommitTransaction(
                uiContext->GetCanvasModifiersDrawAgent(), renderPiplineClient, data, transactionDataIndex);
            // Move transactionData back to UI thread for destruction to avoid
            // releasing it on the ModifiersDraw thread.
            auto sharedData = std::shared_ptr<RSTransactionData>(std::move(data));
            uiContext->PostTask([sharedData]() {});
            uiContext->UnblockUIThread();
        });
}

void RSModifiersDrawAgent::WaitAllTasksFinish()
{
    modifiersDraw_->PostSyncTask([modifiersDraw = modifiersDraw_]() { modifiersDraw->ReleaseResource(); });
}

void RSModifiersDrawAgent::Destroy()
{
    modifiersDraw_->Destroy();
}
} // namespace OHOS::Rosen

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

#include "rs_render_to_composer_connection.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderToComposerConnection"
namespace OHOS {
namespace Rosen {
RSRenderToComposerConnection::RSRenderToComposerConnection(const std::string& name, uint64_t screenId,
    std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent)
    : screenId_(screenId), rsRenderComposerAgent_(rsRenderComposerAgent)
{}

bool RSRenderToComposerConnection::CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData)
{
    RS_TRACE_NAME_FMT("%s screenId:%" PRIu64, __func__, screenId_);
    if (transactionData.get() == nullptr) {
        RS_LOGE("%{public}s param nullptr", __func__);
        return false;
    }
    return rsRenderComposerAgent_->ComposerProcess(std::move(transactionData));
}

void RSRenderToComposerConnection::ClearFrameBuffers()
{
    RS_TRACE_NAME_FMT("%s screenId:" PRIu64, __func__, screenId_);
    if (rsRenderComposerAgent_ == nullptr) {
        RS_LOGE("%{public}s param illegal", __func__);
        return;
    }
    rsRenderComposerAgent_->ClearFrameBuffers();
}

void RSRenderToComposerConnection::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    RS_TRACE_NAME_FMT("%s screenId:" PRIu64, __func__, screenId_);
    if (surfaceId == 0 || rsRenderComposerAgent_ == nullptr) {
        RS_LOGE("%{public}s param illegal", __func__);
        return;
    }
    rsRenderComposerAgent_->CleanLayerBufferBySurfaceId(surfaceId);
}

void RSRenderToComposerConnection::OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp)
{
    RS_TRACE_NAME_FMT("%s screenId:" PRIu64, __func__, screenId);
    if (rsRenderComposerAgent_ == nullptr) {
        RS_LOGE("%{public}s param nullptr", __func__);
        return;
    }
    rsRenderComposerAgent_->OnScreenVBlankIdleCallback(screenId, timestamp);
}

void RSRenderToComposerConnection::ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds)
{
    RS_TRACE_NAME_FMT("%s screenId:%" PRIu64"", __func__, screenId_);
    if (rsRenderComposerAgent_ == nullptr) {
        RS_LOGE("%{public}s param illegal", __func__);
        return;
    }
    rsRenderComposerAgent_->ClearRedrawGPUCompositionCache(bufferIds);
}

void RSRenderToComposerConnection::SetScreenBacklight(uint32_t level)
{
    RS_TRACE_NAME_FMT("%s level:%" PRIu32"", __func__, level);
    if (rsRenderComposerAgent_ == nullptr) {
        RS_LOGE("%{public}s param illegal", __func__);
        return;
    }
    rsRenderComposerAgent_->SetScreenBacklight(level);
}

void RSRenderToComposerConnection::SetComposerToRenderConnection(
    const sptr<IRSComposerToRenderConnection>& composerToRenderConn)
{
    RS_TRACE_NAME_FMT("%s", __func__);
    if (rsRenderComposerAgent_ == nullptr) {
        RS_LOGE("%{public}s param illegal", __func__);
        return;
    }
    rsRenderComposerAgent_->SetComposerToRenderConnection(composerToRenderConn);
}

void RSRenderToComposerConnection::PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer)
{
    RS_TRACE_NAME_FMT("%s", __func__);
    if (rsRenderComposerAgent_ == nullptr) {
        RS_LOGE("%{public}s param illegal", __func__);
        return;
    }
    rsRenderComposerAgent_->PreAllocProtectedFrameBuffers(buffer);
}
} // namespace Rosen
} // namespace OHOS
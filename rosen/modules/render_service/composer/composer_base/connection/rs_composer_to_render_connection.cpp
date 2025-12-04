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

#include "rs_composer_to_render_connection.h"

#include "rs_render_composer_client.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "frame_report.h"

#undef LOG_TAG
#define LOG_TAG "RSComposerToRenderConnection"

namespace OHOS {
namespace Rosen {
int32_t RSComposerToRenderConnection::ReleaseLayerBuffers(ReleaseLayerBuffersInfo &releaseLayerInfo)
{
    uint64_t screenId = releaseLayerInfo.screenId;
    RS_LOGD("RSComposerToRenderConnection::ReleaseLayerBuffers, screenId:%{public}" PRIu64, screenId);
    auto uniRenderThread = &(RSUniRenderThread::Instance());
    std::shared_ptr<RSRenderComposerClient> composerClient = uniRenderThread->GetRSRenderComposerClient(screenId);
    if (composerClient == nullptr) {
        RS_LOGE("GetRSRenderComposerClient failed, screenId:%{public}" PRIu64, screenId);
        return COMPOSITOR_ERROR_NULLPTR;
    }
    composerClient->RegistOnBufferReleaseFunc([](uint64_t seqNum, sptr<SyncFence> fence) {
        RSUniRenderThread::OnComposedBufferCallBack(seqNum, fence);
    });
    composerClient->ReleaseLayerBuffers(screenId, releaseLayerInfo.timestampVec, releaseLayerInfo.releaseBufferFenceVec);
    uniRenderThread->NotifyScreenNodeBufferReleased();
    // 游戏大脑 打桩获取SwapBufferTime
    if (FrameReport::GetInstance().HasGameScene()) {
        RS_LOGD("[game_accelerate_schedule] RSComposerToRenderConnection lastSwapBufferTime %{public}lld",
            releaseLayerInfo.lastSwapBufferTime / 1000);
        FrameReport::GetInstance().SetLastSwapBufferTime(releaseLayerInfo.lastSwapBufferTime);
    }
    return COMPOSITOR_ERROR_OK;
}

int32_t RSComposerToRenderConnection::NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t>& lppNodeIds)
{
    RS_LOGD("RSComposerToRenderConnection NotifyLppLayerToRender");
    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr) {
        mainThread->JudgeLppLayer(vsyncId, lppNodeIds);
    }
    return COMPOSITOR_ERROR_OK;
}

} // namespace Rosen
} // namespace OHOS
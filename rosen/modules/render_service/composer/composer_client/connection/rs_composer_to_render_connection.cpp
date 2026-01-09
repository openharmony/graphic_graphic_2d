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
#include "frame_report.h"
#include "platform/common/rs_log.h"
#include "rs_render_composer_client.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSComposerToRenderConnection"

namespace OHOS {
namespace Rosen {
int32_t RSComposerToRenderConnection::ReleaseLayerBuffers(ReleaseLayerBuffersInfo& releaseLayerInfo)
{
    RS_LOGD("RSComposerToRenderConnection::ReleaseLayerBuffers, screenId:%{public}" PRIu64, releaseLayerInfo.screenId);
    if (releaseLayerBuffersCB_ != nullptr) {
        releaseLayerBuffersCB_(releaseLayerInfo);
    }
    // 游戏大脑 打桩获取SwapBufferTime
    if (FrameReport::GetInstance().HasGameScene()) {
        RS_LOGD("[game_accelerate_schedule] RSComposerToRenderConnection lastSwapBufferTime %{public}" PRId64,
            releaseLayerInfo.lastSwapBufferTime / 1000);
        FrameReport::GetInstance().SetLastSwapBufferTime(releaseLayerInfo.lastSwapBufferTime);
    }
    return COMPOSITOR_ERROR_OK;
}

void RSComposerToRenderConnection::RegisterReleaseLayerBuffersCB(ReleaseLayerBuffersCB callback)
{
    releaseLayerBuffersCB_ = std::move(callback);
}

int32_t RSComposerToRenderConnection::NotifyLppLayerToRender(
    uint64_t vsyncId, const std::unordered_set<uint64_t>& lppNodeIds)
{
    RS_LOGD("%s", __func__);
    if (judgeLppLayerCB_ != nullptr) {
        judgeLppLayerCB_(vsyncId, lppNodeIds);
    }
    return COMPOSITOR_ERROR_OK;
}


void RSComposerToRenderConnection::RegisterJudgeLppLayerCB(JudgeLppLayerCB callback)
{
    judgeLppLayerCB_ = std::move(callback);
}
} // namespace Rosen
} // namespace OHOS
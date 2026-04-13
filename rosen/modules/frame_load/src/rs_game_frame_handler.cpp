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

#include "rs_game_frame_handler.h"

#include "frame_report.h"
#include "pipeline/rs_uni_render_judgement.h"

namespace OHOS {
namespace Rosen {
constexpr int64_t UNI_RENDER_VSYNC_OFFSET = 5000000;             // ns
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = -3300000; // ns

RsGameFrameHandler::RsGameFrameHandler(
    const HandleGameSceneChangedFunc& handleGameSceneChangedFunc, const GetCoreInfoFunc& getCoreInfoFunc)
    : handleGameSceneChangedFunc_(handleGameSceneChangedFunc), getCoreInfoFunc_(getCoreInfoFunc) {}

void RsGameFrameHandler::HandleGameSceneChanged()
{
    bool isLtpoEnabled = true;
    bool isVsyncOffsetCustomized = false;
    bool isDelayMode = false;
    int64_t rsOffset = 0;
    int64_t appOffset = 0;
    getCoreInfoFunc_(isLtpoEnabled, isVsyncOffsetCustomized, isDelayMode, rsOffset, appOffset);
    if (isLtpoEnabled || !isVsyncOffsetCustomized) {
        return;
    }

    // ltpo is not enabled and vsync offset is configed in hgm_policy_config.xml, continue
    bool hasGameScene = FrameReport::GetInstance().HasGameScene();
    if (hasGameScene_.exchange(hasGameScene) == hasGameScene) {
        return;
    }

    // game scene changed, continue
    int64_t offset = 0;
    if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        offset = isDelayMode ? UNI_RENDER_VSYNC_OFFSET_DELAY_MODE : UNI_RENDER_VSYNC_OFFSET;
    }

    // if device is in game scene now, set offset to customized value, otherwise recover to normal offset
    handleGameSceneChangedFunc_(hasGameScene ? rsOffset : offset, hasGameScene ? appOffset : offset);
}
} // namespace Rosen
} // namespace OHOS

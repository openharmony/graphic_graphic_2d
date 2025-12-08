/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "feature/hyper_graphic_manager/rs_vblank_idle_corrector.h"

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t IDLE_FRAME_COUNT_THRESHOLD = 2;
};

void RSVBlankIdleCorrector::SetScreenVBlankIdle()
{
    bool isCorrectorEnabled = HgmCore::Instance().IsVBlankIdleCorrectEnabled();
    if (isCorrectorEnabled) {
        idleFrameCount_ = IDLE_FRAME_COUNT_THRESHOLD;
        isVBlankIdle_ = true;
    }
}

void RSVBlankIdleCorrector::ProcessScreenConstraint(ScreenId screenId, uint64_t timestamp,
    uint64_t constraintRelativeTime)
{
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("%{public}s, screenManager is null!", __func__);
        return;
    }

    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        RS_LOGE("%{public}s, frameRateMgr is null!", __func__);
        return;
    }

    bool isScreenNeedAdaptive = frameRateMgr->AdaptiveStatus() == SupportASStatus::SUPPORT_AS &&
        frameRateMgr->IsGameNodeOnTree();
    if (isScreenNeedAdaptive && screenId == frameRateMgr->GetCurScreenId()) {
        RS_TRACE_NAME("RSVBlankIdleCorrector::ProcessScreenConstraint set 3 in Adaptive Mode!");
        screenManager->SetScreenConstraint(screenId, 0, ScreenConstraintType::CONSTRAINT_ADAPTIVE);
        return;
    }

    bool isCorrectorEnabled = HgmCore::Instance().IsVBlankIdleCorrectEnabled();
    if (!isCorrectorEnabled) {
        idleFrameCount_ = 0;
        isVBlankIdle_ = false;
        screenManager->SetScreenConstraint(screenId, 0, ScreenConstraintType::CONSTRAINT_NONE);
        return;
    }

    if (isVBlankIdle_) {
        uint64_t pipelineOffset = static_cast<uint64_t>(HgmCore::Instance().GetPipelineOffset());
        if (idleFrameCount_ > 0) {
            uint64_t absoluteTime = timestamp + pipelineOffset;
            screenManager->SetScreenConstraint(screenId,
                absoluteTime, ScreenConstraintType::CONSTRAINT_ABSOLUTE);
        } else {
            screenManager->SetScreenConstraint(screenId, 0, ScreenConstraintType::CONSTRAINT_NONE);
        }
        idleFrameCount_--;
        if (idleFrameCount_ < 0) {
            idleFrameCount_ = 0;
            isVBlankIdle_ = false;
        }
    } else if (constraintRelativeTime > 0) {
        screenManager->SetScreenConstraint(screenId,
            constraintRelativeTime, ScreenConstraintType::CONSTRAINT_RELATIVE);
    } else {
        screenManager->SetScreenConstraint(screenId, 0, ScreenConstraintType::CONSTRAINT_NONE);
    }
}
}
}

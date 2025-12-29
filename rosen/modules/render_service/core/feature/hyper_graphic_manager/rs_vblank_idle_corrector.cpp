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
    RSScreenManager* scmFromHgm = HgmCore::Instance().GetScreenManager();
    if (scmFromHgm == nullptr) {
        HGM_LOGE("scmFromHgm is nullptr");
        return;
    }

    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        HGM_LOGE("frameRateMgr is null!");
        return;
    }

    ScreenId curScreenId = frameRateMgr->GetCurScreenId();
    if (screenId != curScreenId) {
        HGM_LOGW("screenId:%{public}" PRIu64 " curScreenId:%{public}" PRIu64, screenId, curScreenId);
        return;
    }

    bool isScreenNeedAdaptive = frameRateMgr->AdaptiveStatus() == SupportASStatus::SUPPORT_AS &&
        frameRateMgr->IsGameNodeOnTree();
    if (isScreenNeedAdaptive) {
        RS_TRACE_NAME_FMT("%s: set 3 in Adaptive Mode!", __func__);
        scmFromHgm->SetScreenConstraint(screenId, 0, ScreenConstraintType::CONSTRAINT_ADAPTIVE);
        return;
    }

    bool isCorrectorEnabled = HgmCore::Instance().IsVBlankIdleCorrectEnabled();
    if (!isCorrectorEnabled) {
        idleFrameCount_ = 0;
        isVBlankIdle_ = false;
        scmFromHgm->SetScreenConstraint(screenId, 0, ScreenConstraintType::CONSTRAINT_NONE);
        return;
    }

    if (isVBlankIdle_) {
        uint64_t pipelineOffset = static_cast<uint64_t>(HgmCore::Instance().GetPipelineOffset());
        if (idleFrameCount_ > 0) {
            uint64_t absoluteTime = timestamp + pipelineOffset;
            scmFromHgm->SetScreenConstraint(screenId, absoluteTime, ScreenConstraintType::CONSTRAINT_ABSOLUTE);
        } else {
            scmFromHgm->SetScreenConstraint(screenId, 0, ScreenConstraintType::CONSTRAINT_NONE);
        }
        idleFrameCount_--;
        if (idleFrameCount_ < 0) {
            idleFrameCount_ = 0;
            isVBlankIdle_ = false;
        }
    } else if (constraintRelativeTime > 0) {
        scmFromHgm->SetScreenConstraint(screenId, constraintRelativeTime, ScreenConstraintType::CONSTRAINT_RELATIVE);
    } else {
        scmFromHgm->SetScreenConstraint(screenId, 0, ScreenConstraintType::CONSTRAINT_NONE);
    }
}
}
}

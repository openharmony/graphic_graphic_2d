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

#include "native_display_soloist.h"
#include "hgm_log.h"
#include "feature/hyper_graphic_manager/rs_ui_display_soloist.h"
#include "transaction/rs_interfaces.h"
#include "vsync_receiver.h"

using namespace OHOS::Rosen;

struct OH_DisplaySoloist {
    std::shared_ptr<SoloistId> soloistId_;
};

OH_DisplaySoloist* OH_DisplaySoloist_Create(bool useExclusiveThread)
{
    std::shared_ptr<SoloistId> soloistId = SoloistId::Create();
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    soloistManager.InsertUseExclusiveThreadFlag(soloistId->GetId(), useExclusiveThread);

    OH_DisplaySoloist* displaySoloist = new OH_DisplaySoloist({ soloistId });
    HGM_LOGI("[DisplaySoloist] Create DisplaySoloist SoloistId: %{public}d", soloistId->GetId());
    return displaySoloist;
}

int32_t OH_DisplaySoloist_Destroy(OH_DisplaySoloist* displaySoloist)
{
    if (displaySoloist == nullptr) {
        HGM_LOGE("%{public}s displaySoloist is nullptr", __func__);
        return SOLOIST_ERROR;
    }

    if (displaySoloist->soloistId_ == nullptr) {
        HGM_LOGE("%{public}s soloistId_ is nullptr", __func__);
        return SOLOIST_ERROR;
    }

    SoloistIdType soloistId = displaySoloist->soloistId_->GetId();
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    soloistManager.RemoveSoloist(soloistId);
    HGM_LOGI("[DisplaySoloist] Destroy DisplaySoloist SoloistId: %{public}d", soloistId);

    delete displaySoloist;
    displaySoloist = nullptr;
    return EXEC_SUCCESS;
}

int32_t OH_DisplaySoloist_Start(OH_DisplaySoloist* displaySoloist,
    OH_DisplaySoloist_FrameCallback callback, void* data)
{
    if (displaySoloist == nullptr) {
        HGM_LOGE("%{public}s displaySoloist is nullptr", __func__);
        return SOLOIST_ERROR;
    }

    if (displaySoloist->soloistId_ == nullptr) {
        HGM_LOGE("%{public}s soloistId_ is nullptr", __func__);
        return SOLOIST_ERROR;
    }

    SoloistIdType soloistId = displaySoloist->soloistId_->GetId();
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    soloistManager.InsertOnVsyncCallback(soloistId, callback, data);
    soloistManager.Start(soloistId);
    HGM_LOGI("[DisplaySoloist] Start DisplaySoloist SoloistId: %{public}d", soloistId);
    return EXEC_SUCCESS;
}

int32_t OH_DisplaySoloist_Stop(OH_DisplaySoloist* displaySoloist)
{
    if (displaySoloist == nullptr) {
        HGM_LOGE("%{public}s displaySoloist is nullptr", __func__);
        return SOLOIST_ERROR;
    }

    if (displaySoloist->soloistId_ == nullptr) {
        HGM_LOGE("%{public}s soloistId_ is nullptr", __func__);
        return SOLOIST_ERROR;
    }

    SoloistIdType soloistId = displaySoloist->soloistId_->GetId();
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    soloistManager.Stop(soloistId);
    HGM_LOGI("[DisplaySoloist] Stop DisplaySoloist SoloistId: %{public}d", soloistId);
    return EXEC_SUCCESS;
}

int32_t OH_DisplaySoloist_SetExpectedFrameRateRange(OH_DisplaySoloist* displaySoloist,
    DisplaySoloist_ExpectedRateRange* range)
{
    if (displaySoloist == nullptr || range == nullptr) {
        HGM_LOGE("%{public}s displaySoloist or range is nullptr", __func__);
        return SOLOIST_ERROR;
    }

    if (displaySoloist->soloistId_ == nullptr) {
        HGM_LOGE("%{public}s soloistId_ is nullptr", __func__);
        return SOLOIST_ERROR;
    }

    FrameRateRange frameRateRange(range->min, range->max, range->expected);
    if (!frameRateRange.IsValid()) {
        HGM_LOGE("%{public}s ExpectedRateRange Error, please check.", __func__);
        return SOLOIST_ERROR;
    }

    SoloistIdType soloistId = displaySoloist->soloistId_->GetId();
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    soloistManager.InsertFrameRateRange(soloistId, frameRateRange);
    HGM_LOGI("[DisplaySoloist] SoloistId: %{public}d SetExpectedFrameRateRange{%{public}d, %{public}d, %{public}d}",
        soloistId, frameRateRange.min_, frameRateRange.max_, frameRateRange.preferred_);
    return EXEC_SUCCESS;
}

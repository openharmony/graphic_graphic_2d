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

#include "vsync_manager.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "rs_render_service.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t UNI_RENDER_VSYNC_OFFSET = 5000000; // ns
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = -3300000; // ns
}

RSVsyncManager::RSVsyncManager()
{

}

RSVsyncManager::~RSVsyncManager()
{

}

void RSVsyncManager::SetScreenManager(sptr<RSScreenManager> screenManager)
{
    screenManager_ = screenManager;
}

bool RSVsyncManager::init(sptr<RSScreenManager> screenManager)
{
    SetScreenManager(screenManager);
    return VsyncComponentInit();
}

bool RSVsyncManager::VsyncComponentInit()
{
    if (screenManager_ == nullptr){
        return false;
    }
    vsyncGenerator_ = CreateVSyncGenerator();
    // The offset needs to be set
    int64_t offset = 0;
    if (!HgmCore::Instance().GetLtpoEnabled()) {
        if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            offset = HgmCore::Instance().IsDelayMode() ? UNI_RENDER_VSYNC_OFFSET_DELAY_MODE : UNI_RENDER_VSYNC_OFFSET;
        }
        rsVSyncController_ = new VSyncController(vsyncGenerator_, offset);
        appVSyncController_ = new VSyncController(vsyncGenerator_, offset);
    } else {
        rsVSyncController_ = new VSyncController(vsyncGenerator_, 0);
        appVSyncController_ = new VSyncController(vsyncGenerator_, 0);
        vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    }
    vsyncSampler_ = CreateVSyncSampler();
    vsyncSampler_->RegUpdateVsyncEnabledScreenIdCallback([this](uint64_t vsyncEnabledScreenId) -> bool {
       return this->screenManager_->UpdateVsyncEnabledScreenId(vsyncEnabledScreenId);
    });

    vsyncSampler_->RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback([this](
        uint64_t screenId, ScreenPowerStatus status, uint64_t enabledScreenId) -> uint64_t {
        return this->screenManager_->JudgeVSyncEnabledScreenWhilePowerStatusChanged(screenId, status, enabledScreenId);
    });

    vsyncSampler_->RegUpdateFoldScreenConnectStatusLockedCallback([this](uint64_t screenId, bool connected) {
        return this->screenManager_->UpdateFoldScreenConnectStatusLocked(screenId, connected);
    });

    vsyncSampler_->RegSetScreenVsyncEnableByIdCallback([this](
        uint64_t vsyncEnabledScreenId, uint64_t screenId, bool enabled) {
        return this->screenManager_->SetScreenVsyncEnableById(vsyncEnabledScreenId, screenId, enabled);
    });

    vsyncSampler_->RegGetScreenVsyncEnableByIdCallback([this](uint64_t vsyncEnabledScreenId) {
        return this->screenManager_->GetScreenVsyncEnableById(vsyncEnabledScreenId);
    });

    auto dvsyncParam = InitDVSyncParams();
    rsVSyncDistributor_ = new VSyncDistributor(rsVSyncController_, "rs", dvsyncParam);
    appVSyncDistributor_ = new VSyncDistributor(appVSyncController_, "app", dvsyncParam);
    vsyncGenerator_->SetRSDistributor(rsVSyncDistributor_);
    vsyncGenerator_->SetAppDistributor(appVSyncDistributor_);
    rsVsyncManagerAgent_ = new RSVsyncManagerAgent(vsyncGenerator_, rsVSyncDistributor_, appVSyncDistributor_);
    return true;
}


VsyncError RSVsyncManager::AddRSVsyncConnection(const sptr<VSyncConnection>& connection)
{
    if (rsVSyncDistributor_ != nullptr) {
        return rsVSyncDistributor_->AddConnection(connection);
    }
    return VSYNC_ERROR_NULLPTR;
}

sptr<VSyncConnection> RSVsyncManager::CreateRSVSyncConnection(std::string name, const sptr<IRemoteObject>& token)
{
    auto conn = new VSyncConnection(rsVSyncDistributor_, name, token);
    return conn;
}

void RSVsyncManager::RegSetScreenVsyncEnabledCallbackForRenderService(ScreenId vsyncEnabledScreenId,
    std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    if (vsyncSampler_ != nullptr) {
        vsyncSampler_->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncEnabledScreenId, handler);
    }
}

uint64_t RSVsyncManager::JudgeVSyncEnabledScreenWhileHotPlug(ScreenId screenId, bool connected)
{
    if (vsyncSampler_ != nullptr) {
        return vsyncSampler_->JudgeVSyncEnabledScreenWhileHotPlug(screenId, connected);
    }
    return INVALID_SCREEN_ID;
}

sptr<RSVsyncManagerAgent> RSVsyncManager::GetVsyncManagerAgent()
{
    return rsVsyncManagerAgent_;
}


sptr<VSyncDistributor> RSVsyncManager::GetVSyncAppDistributor()
{
    return appVSyncDistributor_;
}


sptr<VSyncDistributor> RSVsyncManager::GetVSyncRSDistributor()
{
    return rsVSyncDistributor_;
}

sptr<VSyncGenerator> RSVsyncManager::GetVSyncGenerator()
{
    return vsyncGenerator_;
}

sptr<VSyncController> RSVsyncManager::GetVSyncRSController()
{
    return rsVSyncController_;
}

sptr<VSyncController> RSVsyncManager::GetVSyncAppController()
{
    return appVSyncController_;
}

sptr<VSyncSampler> RSVsyncManager::GetVSyncSampler()
{
    return vsyncSampler_;
}

DVSyncFeatureParam RSVsyncManager::InitDVSyncParams()
{
    std::vector<bool> switchParams = {
        DVSyncParam::IsDVSyncEnable(),
        DVSyncParam::IsUiDVSyncEnable(),
        DVSyncParam::IsNativeDVSyncEnable(),
        DVSyncParam::IsAdaptiveDVSyncEnable(),
    };
    std::vector<uint32_t> bufferCountParams = {
        DVSyncParam::GetUiBufferCount(),
        DVSyncParam::GetRsBufferCount(),
        DVSyncParam::GetNativeBufferCount(),
        DVSyncParam::GetWebBufferCount(),
    };
    auto adaptiveConfigs = DVSyncParam::GetAdaptiveConfig();
    DVSyncFeatureParam dvsyncParam = {
        std::move(switchParams),
        std::move(bufferCountParams),
        std::move(adaptiveConfigs)
    };
    return dvsyncParam;
}

} // namespace Rosen
} // namespace OHOS


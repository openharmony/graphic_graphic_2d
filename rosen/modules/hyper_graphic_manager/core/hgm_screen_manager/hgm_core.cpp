/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hgm_core.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unistd.h>

#include "hgm_frame_rate_manager.h"
#include "hgm_config_callback_manager.h"
#include "hgm_log.h"
#include "vsync_generator.h"
#include "platform/common/rs_system_properties.h"
#include "parameters.h"
#include "frame_rate_report.h"
#include "sandbox_utils.h"

namespace OHOS::Rosen {
static std::map<uint32_t, int64_t> IDEAL_PERIOD = {
    { 120, 8333333 },
    { 90, 11111111 },
    { 72, 13888888 },
    { 60, 16666666 }
};

HgmCore& HgmCore::Instance()
{
    static HgmCore instance;
    if (instance.IsInit()) {
        return instance;
    }
    if (instance.Init() == false) {
        HGM_LOGI("HgmCore initialization failed");
    }

    return instance;
}

HgmCore::HgmCore()
{
    HGM_LOGI("Construction of Hgmcore");
}

bool HgmCore::Init()
{
    if (isInit_) {
        return true;
    }

    if (!isEnabled_) {
        HGM_LOGE("HgmCore Hgm is desactivated");
        return false;
    }

    if (InitXmlConfig() != EXEC_SUCCESS) {
        HGM_LOGE("HgmCore falied to parse");
        return false;
    }

    // ensure that frameRateManager init after XML parsed.
    hgmFrameRateMgr_ = std::make_unique<HgmFrameRateManager>();

    auto newRateMode = static_cast<int32_t>(RSSystemProperties::GetHgmRefreshRateModesEnabled());
    if (newRateMode == 0) {
        HGM_LOGI("HgmCore No customer refreshrate mode found, set to xml default");
        if (mPolicyConfigData_ == nullptr) {
            HGM_LOGE("HgmCore failed to get parsed data");
        } else {
            customFrameRateMode_ = std::stoi(mPolicyConfigData_->defaultRefreshRateMode_);
        }
    } else {
        HGM_LOGI("HgmCore No customer refreshrate mode found: %{public}d", newRateMode);
        customFrameRateMode_ = newRateMode;
    }

    SetLtpoConfig();

    isInit_ = true;
    HGM_LOGI("HgmCore initialization success!!!");
    return isInit_;
}

int32_t HgmCore::InitXmlConfig()
{
    HGM_LOGD("HgmCore is parsing xml configuration");
    if (!mParser_) {
        mParser_ = std::make_unique<XMLParser>();
    }

    if (mParser_->LoadConfiguration(configFileProduct) != EXEC_SUCCESS) {
        HGM_LOGW("HgmCore failed to load prod xml configuration file");
    }
    if (mParser_->Parse() != EXEC_SUCCESS) {
        HGM_LOGW("HgmCore failed to parse prod xml configuration");
    }

    if (!mPolicyConfigData_) {
        mPolicyConfigData_ = mParser_->GetParsedData();
    }

    return EXEC_SUCCESS;
}

void HgmCore::SetLtpoConfig()
{
    if ((hgmFrameRateMgr_ == nullptr) || (mPolicyConfigData_ == nullptr)) {
        return;
    }
    auto curScreenStrategyId = hgmFrameRateMgr_->GetCurScreenStrategyId();
    auto curScreenSetting =
        mPolicyConfigData_->screenConfigs_[curScreenStrategyId][std::to_string(customFrameRateMode_)];
    if (curScreenSetting.ltpoConfig.find("switch") != curScreenSetting.ltpoConfig.end()) {
        ltpoEnabled_ = std::stoi(curScreenSetting.ltpoConfig["switch"]);
    } else {
        ltpoEnabled_ = 0;
        HGM_LOGW("HgmCore failed to find switch strategy for LTPO");
    }

    if (curScreenSetting.ltpoConfig.find("maxTE") != curScreenSetting.ltpoConfig.end()) {
        maxTE_ = std::stoul(curScreenSetting.ltpoConfig["maxTE"]);
    } else {
        maxTE_ = 0;
        HGM_LOGW("HgmCore failed to find TE strategy for LTPO");
    }

    if (curScreenSetting.ltpoConfig.find("alignRate") != curScreenSetting.ltpoConfig.end()) {
        alignRate_ = std::stoul(curScreenSetting.ltpoConfig["alignRate"]);
    } else {
        alignRate_ = 0;
        HGM_LOGW("HgmCore failed to find alignRate strategy for LTPO");
    }

    if (curScreenSetting.ltpoConfig.find("pipelineOffsetPulseNum") != curScreenSetting.ltpoConfig.end()) {
        pipelineOffsetPulseNum_ = std::stoi(curScreenSetting.ltpoConfig["pipelineOffsetPulseNum"]);
        CreateVSyncGenerator()->SetVSyncPhaseByPulseNum(pipelineOffsetPulseNum_);
    } else {
        pipelineOffsetPulseNum_ = 0;
        HGM_LOGW("HgmCore failed to find pipelineOffset strategy for LTPO");
    }

    HGM_LOGI("HgmCore LTPO strategy ltpoEnabled: %{public}d, maxTE: %{public}d, alignRate: %{public}d, " \
        "pipelineOffsetPulseNum: %{public}d", ltpoEnabled_, maxTE_, alignRate_, pipelineOffsetPulseNum_);
}

void HgmCore::RegisterRefreshRateModeChangeCallback(const RefreshRateModeChangeCallback& callback)
{
    refreshRateModeChangeCallback_ = callback;
    if (refreshRateModeChangeCallback_ != nullptr) {
        auto refreshRateModeName = GetCurrentRefreshRateModeName();
        refreshRateModeChangeCallback_(refreshRateModeName);
    }
}

int32_t HgmCore::SetCustomRateMode(int32_t mode)
{
    customFrameRateMode_ = mode;
    return EXEC_SUCCESS;
}

int32_t HgmCore::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    // set the screen to the desired refreshrate
    HGM_LOGD("HgmCore setting screen " PUBU64 " to the rate of %{public}d", id, rate);
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGW("HgmCore failed to get screen of : " PUBU64 "", id);
        return HGM_ERROR;
    }

    if (rate <= 0) {
        HGM_LOGW("HgmCore refuse an illegal framerate: %{public}d", rate);
        return HGM_ERROR;
    }
    sceneId = static_cast<int32_t>(screenSceneSet_.size());
    int32_t modeToSwitch = screen->SetActiveRefreshRate(sceneId, static_cast<uint32_t>(rate));
    if (modeToSwitch < 0) {
        return modeToSwitch;
    }

    std::lock_guard<std::mutex> lock(modeListMutex_);

    // the rate is accepted and passed to a list, will be applied by hardwarethread before sending the composition
    HGM_LOGI("HgmCore the rate of %{public}d is accepted, the target mode is %{public}d", rate, modeToSwitch);
    if (modeListToApply_ == nullptr) {
        HGM_LOGD("HgmCore modeListToApply_ is invalid, buiding a new mode list");
        modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
    }
    auto modeList = modeListToApply_.get();
    (*modeList)[id] = modeToSwitch;
    return modeToSwitch;
}

int32_t HgmCore::SetRateAndResolution(ScreenId id, int32_t sceneId, int32_t rate, int32_t width, int32_t height)
{
    // reserved
    return HGM_ERROR;
}

int32_t HgmCore::SetRefreshRateMode(int32_t refreshRateMode)
{
    // setting mode to xml modeid
    if (refreshRateMode != HGM_REFRESHRATE_MODE_AUTO
        && mPolicyConfigData_ != nullptr && mPolicyConfigData_->xmlCompatibleMode_) {
        refreshRateMode = mPolicyConfigData_->SettingModeId2XmlModeId(refreshRateMode);
    }
    HGM_LOGD("HgmCore set refreshrate mode to : %{public}d", refreshRateMode);
    // change refreshrate mode by setting application
    if (SetCustomRateMode(refreshRateMode) != EXEC_SUCCESS) {
        return HGM_ERROR;
    }

    hgmFrameRateMgr_->HandleRefreshRateMode(refreshRateMode);

    auto refreshRateModeName = GetCurrentRefreshRateModeName();
    if (refreshRateModeChangeCallback_ != nullptr) {
        refreshRateModeChangeCallback_(refreshRateModeName);
    }
    HgmConfigCallbackManager::GetInstance()->SyncRefreshRateModeChangeCallback(refreshRateModeName);
    return EXEC_SUCCESS;
}

void HgmCore::NotifyScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    hgmFrameRateMgr_->HandleScreenPowerStatus(id, status);

    if (refreshRateModeChangeCallback_ != nullptr) {
        auto refreshRateModeName = GetCurrentRefreshRateModeName();
        refreshRateModeChangeCallback_(refreshRateModeName);
    }
}

int32_t HgmCore::AddScreen(ScreenId id, int32_t defaultMode, ScreenSize& screenSize)
{
    // add a physical screen to hgm during hotplug event
    HGM_LOGI("HgmCore adding screen : " PUBI64 "", id);
    bool removeId = std::any_of(screenIds_.begin(), screenIds_.end(),
        [id](const ScreenId screen) { return screen == id; });
    if (removeId) {
        if (RemoveScreen(id) != EXEC_SUCCESS) {
            HGM_LOGW("HgmCore failed to remove the existing screen, not adding : " PUBI64 "", id);
            return HGM_BASE_REMOVE_FAILED;
        }
    }

    sptr<HgmScreen> newScreen = new HgmScreen(id, defaultMode, screenSize);

    std::lock_guard<std::mutex> lock(listMutex_);
    screenList_.push_back(newScreen);
    screenIds_.push_back(id);

    int32_t screenNum = GetScreenListSize();
    HGM_LOGI("HgmCore num of screen is %{public}d", screenNum);
    return EXEC_SUCCESS;
}

int32_t HgmCore::RemoveScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lock(listMutex_);
    // delete a screen during a hotplug event
    HGM_LOGD("HgmCore deleting the screen : " PUBU64 "", id);
    for (auto screen = screenIds_.begin(); screen != screenIds_.end(); ++screen) {
        if (*screen == id) {
            screenIds_.erase(screen);
            break;
        }
    }
    for (auto screen = screenList_.begin(); screen != screenList_.end(); ++screen) {
        if ((*screen)->GetId() == id) {
            screenList_.erase(screen);
            break;
        }
    }
    return EXEC_SUCCESS;
}

int32_t HgmCore::AddScreenInfo(ScreenId id, int32_t width, int32_t height, uint32_t rate, int32_t mode)
{
    // add a supported screen mode to the screen
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGW("HgmCore failed to get screen of : " PUBU64 "", id);
        return HGM_NO_SCREEN;
    }

    if (screen->AddScreenModeInfo(width, height, rate, mode) == EXEC_SUCCESS) {
        return EXEC_SUCCESS;
    }

    HGM_LOGW("HgmCore failed to add screen mode info of screen : " PUBU64 "", id);
    return HGM_SCREEN_PARAM_ERROR;
}

int32_t HgmCore::RefreshBundleName(const std::string& name)
{
    if (name == currentBundleName_) {
        return EXEC_SUCCESS;
    }

    currentBundleName_ = name;

    if (customFrameRateMode_ == HGM_REFRESHRATE_MODE_AUTO) {
        return EXEC_SUCCESS;
    }

    int resetResult = SetRefreshRateMode(customFrameRateMode_);
    if (resetResult == EXEC_SUCCESS) {
        HGM_LOGI("HgmCore reset current refreshrate mode: %{public}d due to bundlename: %{public}s",
            customFrameRateMode_, currentBundleName_.c_str());
    }
    return EXEC_SUCCESS;
}

uint32_t HgmCore::GetScreenCurrentRefreshRate(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGW("HgmCore failed to find screen " PUBU64 "", id);
        return static_cast<uint32_t>(EXEC_SUCCESS);
    }

    return screen->GetActiveRefreshRate();
}

int32_t HgmCore::GetCurrentRefreshRateMode() const
{
    if (customFrameRateMode_ != HGM_REFRESHRATE_MODE_AUTO
        && mPolicyConfigData_ != nullptr && mPolicyConfigData_->xmlCompatibleMode_) {
        return mPolicyConfigData_->XmlModeId2SettingModeId(customFrameRateMode_);
    }
    return customFrameRateMode_;
}

int32_t HgmCore::GetCurrentRefreshRateModeName() const
{
    if (mPolicyConfigData_ != nullptr && mPolicyConfigData_->xmlCompatibleMode_) {
        return mPolicyConfigData_->GetRefreshRateModeName(customFrameRateMode_);
    }
    std::map<int32_t, int32_t> modeIdRateMap = {{-1, -1}, {1, 60}, {2, 90}, {3, 120}};
    if (modeIdRateMap.find(customFrameRateMode_) != modeIdRateMap.end()) {
        return modeIdRateMap[customFrameRateMode_];
    }
    return customFrameRateMode_;
}

sptr<HgmScreen> HgmCore::GetScreen(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(listMutex_);
    for (auto screen = screenList_.begin(); screen != screenList_.end(); ++screen) {
        if ((*screen)->GetId() == id) {
            return *screen;
        }
    }

    return nullptr;
}

std::vector<uint32_t> HgmCore::GetScreenSupportedRefreshRates(ScreenId id)
{
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGW("HgmCore failed to find screen " PUBU64 "", id);
        return std::vector<uint32_t>(static_cast<uint32_t>(EXEC_SUCCESS));
    }

    auto supportedRates = screen->GetSupportedRates();
    std::vector<uint32_t> retVec;
    retVec.assign(supportedRates.begin(), supportedRates.end());
    return retVec;
}

std::vector<int32_t> HgmCore::GetScreenComponentRefreshRates(ScreenId id)
{
    if (!mPolicyConfigData_) {
        HGM_LOGW("HgmCore no parsed component data, returning default value");
        return std::vector<int32_t>(static_cast<uint32_t>(EXEC_SUCCESS));
    }

    std::vector<int32_t> retVec;
    for (const auto& [rate, _] : mPolicyConfigData_->refreshRateForSettings_) {
        retVec.emplace_back(rate);
        HGM_LOGE("HgmCore Adding component rate: %{public}d", rate);
    }
    return retVec;
}

std::unique_ptr<std::unordered_map<ScreenId, int32_t>> HgmCore::GetModesToApply()
{
    std::lock_guard<std::mutex> lock(modeListMutex_);
    return std::move(modeListToApply_);
}

void HgmCore::SetActiveScreenId(ScreenId id)
{
    activeScreenId_ = id;
}

sptr<HgmScreen> HgmCore::GetActiveScreen() const
{
    if (activeScreenId_ == INVALID_SCREEN_ID) {
        HGM_LOGE("HgmScreen activeScreenId_ noset");
        return nullptr;
    }
    return GetScreen(activeScreenId_);
}

int64_t HgmCore::GetIdealPeriod(uint32_t rate)
{
    if (IDEAL_PERIOD.count(rate)) {
        return IDEAL_PERIOD[rate];
    }
    return 0;
}
} // namespace OHOS::Rosen

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
#include "hgm_screen_info.h"
#include "syspara/parameter.h"

namespace OHOS::Rosen {
namespace {
std::map<uint32_t, int64_t> IDEAL_PERIOD = {
    { 144, 6944444 },
    { 120, 8333333 },
    { 90, 11111111 },
    { 80, 12500000 },
    { 72, 13888888 },
    { 60, 16666666 },
    { 48, 20833333 },
    { 45, 22222222 },
    { 40, 25000000 },
    { 36, 27777777 },
    { 30, 33333333 },
    { 24, 41666666 },
    { 20, 50000000 },
    { 15, 66666666 },
    { 10, 100000000 },
};
} // namespace

void HgmCore::SysModeChangeProcess(const char* key, const char* value, void* context)
{
    std::string mode(value);
    HgmTaskHandleThread::Instance().PostTask([mode] () {
        auto curMode = HgmCore::Instance().GetCurrentRefreshRateMode();
        HgmCore::Instance().GetPolicyConfigData()->UpdateRefreshRateForSettings(mode);
        HgmCore::Instance().SetRefreshRateMode(curMode);
        RSSystemProperties::SetHgmRefreshRateModesEnabled(std::to_string(curMode));
        HGM_LOGI("System mode changed to %{public}s, cur refresh mode is %{public}d", mode.c_str(), curMode);
    });
}

HgmCore& HgmCore::Instance()
{
    static HgmCore instance;
    instance.Init();
    return instance;
}

HgmCore::HgmCore()
{
    HGM_LOGI("Construction of Hgmcore");
}

void HgmCore::Init()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [this]() {
        if (InitXmlConfig() != EXEC_SUCCESS) {
            HGM_LOGE("HgmCore falied to parse");
            return;
        }

        // ensure that frameRateManager init after XML parsed.
        hgmFrameRateMgr_ = std::make_unique<HgmFrameRateManager>();

        auto newRateMode = static_cast<int32_t>(RSSystemProperties::GetHgmRefreshRateModesEnabled());
        if (newRateMode == 0) {
            HGM_LOGI("HgmCore No customer refreshrate mode found, set to xml default");
            if (mPolicyConfigData_ == nullptr || !XMLParser::IsNumber(mPolicyConfigData_->defaultRefreshRateMode_)) {
                HGM_LOGE("HgmCore failed to get parsed data");
            } else {
                customFrameRateMode_ = std::stoi(mPolicyConfigData_->defaultRefreshRateMode_);
            }
        } else {
            HGM_LOGI("HgmCore No customer refreshrate mode found: %{public}d", newRateMode);
            customFrameRateMode_ = newRateMode;
            if (customFrameRateMode_ != HGM_REFRESHRATE_MODE_AUTO &&
                mPolicyConfigData_ != nullptr && mPolicyConfigData_->xmlCompatibleMode_) {
                customFrameRateMode_ = mPolicyConfigData_->SettingModeId2XmlModeId(customFrameRateMode_);
            }
            CheckCustomFrameRateModeValid();
        }
        if (mPolicyConfigVisitor_ != nullptr) {
            mPolicyConfigVisitor_->SetXmlModeId(std::to_string(customFrameRateMode_));
        }
        (void)AddParamWatcher();
        SetLtpoConfig();
        HGM_LOGI("HgmCore initialization success!!!");
    });
}

int HgmCore::AddParamWatcher() const
{
    // SysModeChangeProcess will be called when first WatchParameter
    int ret = WatchParameter("persist.sys.mode", HgmCore::SysModeChangeProcess, nullptr);
    if (ret != SUCCESS) {
        HGM_LOGE("WatchParameter fail: %{public}d", ret);
    }
    return ret;
}

void HgmCore::CheckCustomFrameRateModeValid()
{
    if (hgmFrameRateMgr_ == nullptr || mPolicyConfigData_ == nullptr) {
        return;
    }

    auto curScreenStrategyId = hgmFrameRateMgr_->GetCurScreenStrategyId();
    auto& screenConfigs = mPolicyConfigData_->screenConfigs_;
    if (screenConfigs.find(curScreenStrategyId) == screenConfigs.end()) {
        return;
    }

    auto& screenConfig = screenConfigs[curScreenStrategyId];
    auto modeStr = std::to_string(customFrameRateMode_);
    if (screenConfig.find(modeStr) != screenConfig.end() || screenConfig.empty()) {
        return;
    }

    int32_t maxMode = HGM_REFRESHRATE_MODE_AUTO;
    for (auto& [modeStr, _] : screenConfig) {
        if (!XMLParser::IsNumber(modeStr)) {
            continue;
        }
        auto mode = std::stoi(modeStr);
        if (maxMode < mode) {
            maxMode = mode;
        }
    }
    HGM_LOGE("auto repair mode: %{public}d -> %{public}d", customFrameRateMode_, maxMode);
    customFrameRateMode_ = maxMode;
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
    if (mPolicyConfigData_ != nullptr) {
        mPolicyConfigVisitor_ = std::make_shared<PolicyConfigVisitorImpl>(*mPolicyConfigData_);
    }

    return EXEC_SUCCESS;
}

void HgmCore::SetASConfig(const PolicyConfigData::ScreenSetting& curScreenSetting)
{
    if (curScreenSetting.ltpoConfig.find("adaptiveSync") != curScreenSetting.ltpoConfig.end()) {
        std::string asConfig = curScreenSetting.ltpoConfig.at("adaptiveSync");
        if (asConfig == "1" || asConfig == "0") {
            adaptiveSync_ = std::stoi(asConfig);
        } else {
            adaptiveSync_ = 0;
        }
    } else {
        adaptiveSync_ = 0;
        HGM_LOGW("HgmCore failed to find adaptiveSync strategy for LTPO, then set to 0");
    }
}

void HgmCore::SetLtpoConfig()
{
    if ((hgmFrameRateMgr_ == nullptr) || (mPolicyConfigData_ == nullptr)) {
        return;
    }
    auto curScreenStrategyId = hgmFrameRateMgr_->GetCurScreenStrategyId();
    if (mPolicyConfigData_->screenConfigs_.count(curScreenStrategyId) == 0 ||
        mPolicyConfigData_->screenConfigs_[curScreenStrategyId].count(std::to_string(customFrameRateMode_)) == 0) {
        return;
    }
    auto curScreenSetting =
        mPolicyConfigData_->screenConfigs_[curScreenStrategyId][std::to_string(customFrameRateMode_)];
    const auto& ltpoConfig = curScreenSetting.ltpoConfig;
    if (ltpoConfig.find("switch") != ltpoConfig.end() && XMLParser::IsNumber(ltpoConfig.at("switch"))) {
        ltpoEnabled_ = std::stoi(ltpoConfig.at("switch"));
    } else {
        ltpoEnabled_ = 0;
        HGM_LOGW("HgmCore failed to find switch strategy for LTPO");
    }

    if (ltpoConfig.find("maxTE") != ltpoConfig.end() && XMLParser::IsNumber(ltpoConfig.at("maxTE"))) {
        maxTE_ = std::stoul(ltpoConfig.at("maxTE"));
        CreateVSyncGenerator()->SetVSyncMaxRefreshRate(maxTE_);
    } else {
        maxTE_ = 0;
        HGM_LOGW("HgmCore failed to find TE strategy for LTPO");
    }

    if (ltpoConfig.find("alignRate") != ltpoConfig.end() && XMLParser::IsNumber(ltpoConfig.at("alignRate"))) {
        alignRate_ = std::stoul(ltpoConfig.at("alignRate"));
    } else {
        alignRate_ = 0;
        HGM_LOGW("HgmCore failed to find alignRate strategy for LTPO");
    }

    if (ltpoConfig.count("pipelineOffsetPulseNum") != 0 &&
        XMLParser::IsNumber(ltpoConfig.at("pipelineOffsetPulseNum"))) {
        pipelineOffsetPulseNum_ = std::stoi(ltpoConfig.at("pipelineOffsetPulseNum"));
        CreateVSyncGenerator()->SetVSyncPhaseByPulseNum(pipelineOffsetPulseNum_);
    } else {
        pipelineOffsetPulseNum_ = 0;
        HGM_LOGW("HgmCore failed to find pipelineOffset strategy for LTPO");
    }
    SetIdealPipelineOffset(pipelineOffsetPulseNum_);
    SetASConfig(curScreenSetting);

    SetScreenConstraintConfig();
    SetPerformanceConfig();
    HGM_LOGI("HgmCore LTPO strategy ltpoEnabled: %{public}d, maxTE: %{public}d, alignRate: %{public}d, "
        "pipelineOffsetPulseNum: %{public}d, vBlankIdleCorrectSwitch: %{public}d, "
        "lowRateToHighQuickSwitch: %{public}d, pluseNum_: %{public}d, isDelayMode_: %{public}d",
        ltpoEnabled_, maxTE_, alignRate_, pipelineOffsetPulseNum_, vBlankIdleCorrectSwitch_.load(),
        lowRateToHighQuickSwitch_.load(), pluseNum_, isDelayMode_);
}

void HgmCore::SetScreenConstraintConfig()
{
    auto curScreenStrategyId = hgmFrameRateMgr_->GetCurScreenStrategyId();
    if (mPolicyConfigData_->screenConfigs_.count(curScreenStrategyId) == 0 ||
        mPolicyConfigData_->screenConfigs_[curScreenStrategyId].count(std::to_string(customFrameRateMode_)) == 0) {
        return;
    }
    const auto& curScreenSetting =
        mPolicyConfigData_->screenConfigs_[curScreenStrategyId][std::to_string(customFrameRateMode_)];
    if (curScreenSetting.ltpoConfig.find("vBlankIdleCorrectSwitch") != curScreenSetting.ltpoConfig.end() &&
        XMLParser::IsNumber(curScreenSetting.ltpoConfig.at("vBlankIdleCorrectSwitch"))) {
        vBlankIdleCorrectSwitch_.store(std::stoi(curScreenSetting.ltpoConfig.at("vBlankIdleCorrectSwitch")));
    } else {
        vBlankIdleCorrectSwitch_.store(false);
        HGM_LOGW("HgmCore failed to find vBlankIdleCorrectSwitch strategy for LTPO");
    }

    if (curScreenSetting.ltpoConfig.find("lowRateToHighQuickSwitch") != curScreenSetting.ltpoConfig.end() &&
        XMLParser::IsNumber(curScreenSetting.ltpoConfig.at("lowRateToHighQuickSwitch"))) {
        lowRateToHighQuickSwitch_.store(std::stoi(curScreenSetting.ltpoConfig.at("lowRateToHighQuickSwitch")));
    } else {
        lowRateToHighQuickSwitch_.store(false);
        HGM_LOGW("HgmCore failed to find lowRateToHighQuickSwitch strategy for LTPO");
    }
}

void HgmCore::SetPerformanceConfig()
{
    auto curScreenStrategyId = hgmFrameRateMgr_->GetCurScreenStrategyId();
    if (mPolicyConfigData_->screenConfigs_.count(curScreenStrategyId) == 0 ||
        mPolicyConfigData_->screenConfigs_[curScreenStrategyId].count(std::to_string(customFrameRateMode_)) == 0) {
        return;
    }
    const auto& curScreenSetting =
        mPolicyConfigData_->screenConfigs_[curScreenStrategyId][std::to_string(customFrameRateMode_)];
    if (curScreenSetting.performanceConfig.count("pluseNum") != 0 &&
        XMLParser::IsNumber(curScreenSetting.performanceConfig.at("pluseNum"))) {
        pluseNum_ = std::stoi(curScreenSetting.performanceConfig.at("pluseNum"));
    } else {
        pluseNum_ = -1;
        HGM_LOGW("HgmCore failed to find pluseNum_ strategy for LTPO");
    }
    if (curScreenSetting.performanceConfig.count("piplineDelayModeEnable") != 0) {
        isDelayMode_ = curScreenSetting.performanceConfig.at("piplineDelayModeEnable") != "0";
    } else {
        isDelayMode_ = true;
        HGM_LOGW("HgmCore failed to find piplineDelayModeEnable_ strategy for LTPO");
    }
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

void HgmCore::RegisterRefreshRateUpdateCallback(const RefreshRateUpdateCallback& callback)
{
    ScreenId screenId = HgmCore::Instance().GetActiveScreenId();
    uint32_t refreshRate = HgmCore::Instance().GetScreenCurrentRefreshRate(screenId);
    refreshRateUpdateCallback_ = callback;
    if (refreshRateUpdateCallback_ != nullptr) {
        refreshRateUpdateCallback_(refreshRate);
    }
}

int32_t HgmCore::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate, bool shouldSendCallback)
{
    if (!IsEnabled()) {
        HGM_LOGD("HgmCore is not enabled");
        return HGM_ERROR;
    }
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
    int32_t modeToSwitch = screen->SetActiveRefreshRate(sceneId, static_cast<uint32_t>(rate));
    if (modeToSwitch < 0) {
        return modeToSwitch;
    }

    std::lock_guard<std::mutex> lock(modeListMutex_);

    // the rate is accepted and passed to a list, will be applied by hardwarethread before sending the composition
    HGM_LOGD("HgmCore the rate of %{public}d is accepted, the target mode is %{public}d", rate, modeToSwitch);
    if (modeListToApply_ == nullptr) {
        HGM_LOGD("HgmCore modeListToApply_ is invalid, buiding a new mode list");
        modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
    }
    auto modeList = modeListToApply_.get();
    (*modeList)[id] = modeToSwitch;

    if (refreshRateUpdateCallback_ && shouldSendCallback) {
        refreshRateUpdateCallback_(rate);
        HGM_LOGD("refresh rate changed, notify to app");
    }
    return modeToSwitch;
}

int32_t HgmCore::SetRateAndResolution(ScreenId id, int32_t sceneId, int32_t rate, int32_t width, int32_t height)
{
    // reserved
    return HGM_ERROR;
}

int32_t HgmCore::SetRefreshRateMode(int32_t refreshRateMode)
{
    if (mPolicyConfigVisitor_ != nullptr) {
        mPolicyConfigVisitor_->SetSettingModeId(refreshRateMode);
    }
    // setting mode to xml modeid
    if (mPolicyConfigData_ != nullptr && mPolicyConfigData_->xmlCompatibleMode_) {
        refreshRateMode = mPolicyConfigData_->SettingModeId2XmlModeId(refreshRateMode);
    }
    HGM_LOGD("HgmCore set refreshrate mode to : %{public}d", refreshRateMode);
    // change refreshrate mode by setting application
    if (SetCustomRateMode(refreshRateMode) != EXEC_SUCCESS) {
        return HGM_ERROR;
    }

    hgmFrameRateMgr_->HandleRefreshRateMode(refreshRateMode);
    // sync vsync mode after refreshRate mode switching
    auto refreshRateModeName = GetCurrentRefreshRateModeName();
    if (refreshRateModeChangeCallback_ != nullptr) {
        refreshRateModeChangeCallback_(refreshRateModeName);
    }
    HgmConfigCallbackManager::GetInstance()->SyncRefreshRateModeChangeCallback(refreshRateModeName);
    return EXEC_SUCCESS;
}

void HgmCore::NotifyScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    if (hgmFrameRateMgr_ != nullptr) {
        hgmFrameRateMgr_->HandleScreenPowerStatus(id, status);
    }

    if (refreshRateModeChangeCallback_ != nullptr) {
        auto refreshRateModeName = GetCurrentRefreshRateModeName();
        refreshRateModeChangeCallback_(refreshRateModeName);
    }
}

void HgmCore::NotifyScreenRectFrameRateChange(ScreenId id, const GraphicIRect& activeRect)
{
    if (hgmFrameRateMgr_ != nullptr) {
        hgmFrameRateMgr_->HandleScreenRectFrameRate(id, activeRect);
    }

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
    auto configData = HgmCore::Instance().GetPolicyConfigData();
    if (configData != nullptr) {
        auto& hgmScreenInfo = HgmScreenInfo::GetInstance();
        auto isLtpo = hgmScreenInfo.IsLtpoType(hgmScreenInfo.GetScreenType(id));
        std::string curScreenName = "screen" + std::to_string(id) + "_" + (isLtpo ? "LTPO" : "LTPS");
        for (auto strategyConfig : configData->screenStrategyConfigs_) {
            if (strategyConfig.first.find(curScreenName) == 0) {
                newScreen->SetSelfOwnedScreenFlag(true);
                break;
            }
        }
    }

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

uint32_t HgmCore::GetScreenCurrentRefreshRate(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGD("HgmCore failed to find screen " PUBU64 "", id);
        return static_cast<uint32_t>(EXEC_SUCCESS);
    }

    return screen->GetActiveRefreshRate();
}

int32_t HgmCore::GetCurrentRefreshRateMode() const
{
    if (mPolicyConfigData_ != nullptr && mPolicyConfigData_->xmlCompatibleMode_) {
        auto ret = mPolicyConfigData_->XmlModeId2SettingModeId(std::to_string(customFrameRateMode_));
        HGM_LOGI("In GetCurrentRefreshRateMode, xmlid: %{public}d, setid: %{public}d", customFrameRateMode_, ret);
        return ret;
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
    HgmTaskHandleThread::Instance().DetectMultiThreadingCalls();
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
    activeScreenId_.store(id);
}

sptr<HgmScreen> HgmCore::GetActiveScreen() const
{
    auto activeScreenId = GetActiveScreenId();
    if (activeScreenId == INVALID_SCREEN_ID) {
        HGM_LOGE("HgmScreen activeScreenId_ noset");
        return nullptr;
    }
    return GetScreen(activeScreenId);
}

int64_t HgmCore::GetIdealPeriod(uint32_t rate)
{
    if (IDEAL_PERIOD.count(rate)) {
        return IDEAL_PERIOD[rate];
    }
    return 0;
}
} // namespace OHOS::Rosen

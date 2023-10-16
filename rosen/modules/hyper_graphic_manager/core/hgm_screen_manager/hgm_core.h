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

#ifndef HGM_CORE_H
#define HGM_CORE_H

#include <functional>
#include <cinttypes>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>
#include <unordered_set>

#include <event_handler.h>

#include "hgm_screen.h"
#include "xml_parser.h"

namespace OHOS::Rosen {
class HgmCore final {
public:
    static HgmCore& Instance();

    std::vector<ScreenId> GetScreenIds() const
    {
        return screenIds_;
    }

    bool IsEnabled() const
    {
        return isEnabled_;
    }

    bool IsInit() const
    {
        return isInit_;
    }

    int32_t GetScreenListSize() const
    {
        return screenList_.size();
    }

    ScreenId GetActiveScreenId() const
    {
        return activeScreenId_;
    }

    std::shared_ptr<ParsedConfigData> GetParsedConfigData() const
    {
        return mParsedConfigData_;
    }

    // set refresh rates
    int32_t SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);
    static int32_t SetRateAndResolution(ScreenId id, int32_t sceneId, int32_t rate, int32_t width, int32_t height);
    int32_t SetRefreshRateMode(RefreshRateMode refreshRateMode);

    // screen interface
    int32_t AddScreen(ScreenId id, int32_t defaultMode, ScreenSize& screenSize);
    int32_t RemoveScreen(ScreenId id);
    int32_t AddScreenInfo(ScreenId id, int32_t width, int32_t height, uint32_t rate, int32_t mode);
    int32_t RefreshBundleName(const std::string& name);
    uint32_t GetScreenCurrentRefreshRate(ScreenId id) const;
    int32_t GetCurrentRefreshRateMode() const;
    sptr<HgmScreen> GetScreen(ScreenId id) const;
    sptr<HgmScreen> GetActiveScreen() const;
    std::vector<uint32_t> GetScreenSupportedRefreshRates(ScreenId id);
    std::vector<int32_t> GetScreenComponentRefreshRates(ScreenId id);
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> GetModesToApply();
    void SetActiveScreenId(ScreenId id);
    void StartScreenScene(SceneType sceceType);
    void StopScreenScene(SceneType sceceType);
    int32_t GetScenePreferred() const;
    int32_t SetModeBySettingConfig();
private:
    HgmCore();
    ~HgmCore() = default;
    HgmCore(const HgmCore&) = delete;
    HgmCore(const HgmCore&&) = delete;
    HgmCore& operator=(const HgmCore&) = delete;
    HgmCore& operator=(const HgmCore&&) = delete;

    bool Init();
    int32_t InitXmlConfig();
    int32_t SetCustomRateMode(RefreshRateMode mode);
    int32_t RequestBundlePermission(int32_t rate);

    bool isEnabled_ = true;
    bool isInit_ = false;
    static constexpr char CONFIG_FILE[] = "/system/etc/graphic/hgm_policy_config.xml";
    std::unique_ptr<XMLParser> mParser_;
    std::shared_ptr<ParsedConfigData> mParsedConfigData_ = nullptr;

    RefreshRateMode customFrameRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    std::vector<ScreenId> screenIds_;
    std::vector<sptr<HgmScreen>> screenList_;
    mutable std::mutex listMutex_;

    mutable std::mutex modeListMutex_;
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> modeListToApply_ = nullptr;

    std::string currentBundleName_;
    ScreenId activeScreenId_ = INVALID_SCREEN_ID;
    std::unordered_set<SceneType> screenSceneSet_;
};
} // namespace OHOS::Rosen
#endif // HGM_CORE_H

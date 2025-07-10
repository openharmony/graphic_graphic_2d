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

#ifndef HGM_APP_PAGE_URL_STRATEGY_H
#define HGM_APP_PAGE_URL_STRATEGY_H

#include "hgm_command.h"

namespace OHOS {
namespace Rosen {

class HgmAppPageUrlStrategy final {
public:
    using PageUrlVoterCallback = std::function<void(pid_t pid, std::string strategy, const bool isAddVoter)>;

    void NotifyPageName(pid_t pid, const std::string& packageName, const std::string& pageName, bool isEnter);
    void SetPageUrlConfig(PolicyConfigData::PageUrlConfigMap pageUrlConfig);
    void RegisterPageUrlVoterCallback(const PageUrlVoterCallback& callback);
    void NotifyScreenSettingChange();
    void CleanPageUrlVote(pid_t pid);
private:
    struct VoterInfo {
        bool hasVoter = false;
        std::string packageName;
        std::string pageName;
    };
    using PageUrlVoterInfo = std::unordered_map<uint32_t, VoterInfo>;
    PageUrlVoterInfo pageUrlVoterInfo_;
    PolicyConfigData::PageUrlConfigMap pageUrlConfig_;
    PageUrlVoterCallback pageUrlVoterCallback_;

    void ProcessRemoveVoter(pid_t pid);
    void ProcessAddVoter(pid_t pid, std::string strategy);
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_APP_PAGE_URL_STRATEGY_H
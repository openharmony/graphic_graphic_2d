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

#include "hgm_app_page_url_strategy.h"
#include "hgm_log.h"

namespace OHOS {
namespace Rosen {

void HgmAppPageUrlStrategy::SetPageUrlConfig(PolicyConfigData::PageUrlConfigMap pageUrlConfig)
{
    pageUrlConfig_ = pageUrlConfig;
}

void HgmAppPageUrlStrategy::RegisterPageUrlVoterCallback(const PageUrlVoterCallback& callback)
{
    pageUrlVoterCallback_ = callback;
}

void HgmAppPageUrlStrategy::ProcessRemoveVoter(pid_t pid)
{
    HGM_LOGI("HgmAppPageUrlStrategy: Remove Voter pid = %{public}d", pid);
    if (pageUrlVoterCallback_ != nullptr) {
        pageUrlVoterCallback_(pid, "", false);
    }
}

void HgmAppPageUrlStrategy::ProcessAddVoter(pid_t pid, std::string strategy)
{
    HGM_LOGI("HgmAppPageUrlStrategy: Add Voter pid = %{public}d", pid);
    if (pageUrlVoterCallback_ != nullptr) {
        pageUrlVoterCallback_(pid, strategy, true);
    }
}

void HgmAppPageUrlStrategy::CleanPageUrlVote(pid_t pid)
{
    if (pageUrlVoterInfo_.count(pid) > 0) {
        HGM_LOGD("HgmAppPageUrlStrategy: clean pid = %{public}d", pid);
        pageUrlVoterInfo_.erase(pid);
    }
}

void HgmAppPageUrlStrategy::NotifyScreenSettingChange()
{
    if (pageUrlVoterInfo_.empty()) {
        return;
    }
    for (auto& pageUrlVoter : pageUrlVoterInfo_) {
        uint32_t pid = pageUrlVoter.first;
        VoterInfo voterInfo = pageUrlVoter.second;
        std::string pageName = voterInfo.pageName;
        if (!pageName.empty()) {
            std::string packageName = voterInfo.packageName;
            /* clean old config voter */
            if (voterInfo.hasVoter) {
                ProcessRemoveVoter(pid);
                pageUrlVoterInfo_[pid].hasVoter = false;
            }
            /* add new config voter if have pageName config */
            if (pageUrlConfig_.count(packageName) <= 0) {
                continue;
            }
            PolicyConfigData::PageUrlConfig pageUrlConfig = pageUrlConfig_[packageName];
            if (pageUrlConfig.count(pageName) <= 0) {
                continue;
            }
            auto strategy = pageUrlConfig[pageName];
            ProcessAddVoter(pid, strategy);
            pageUrlVoterInfo_[pid].hasVoter = true;
        }
    }
}

void HgmAppPageUrlStrategy::NotifyPageName(pid_t pid, const std::string& packageName,
    const std::string& pageName, bool isEnter)
{
    HGM_LOGD("HgmAppPageUrlStrategy: pid=%{public}d packageName=%{public}s pageName=%{public}s" \
        " isEnter=%{public}d", pid, packageName.c_str(), pageName.c_str(), isEnter);
    VoterInfo voterInfo = {false, packageName, pageName};
    if (pageUrlVoterInfo_.count(pid) <= 0) {
        pageUrlVoterInfo_[pid] = voterInfo;
    }
    pageUrlVoterInfo_[pid].pageName = isEnter ? pageName : "";
    /* not config record pageName */
    if (pageUrlConfig_.count(packageName) <= 0) {
        HGM_LOGD("HgmAppPageUrlStrategy: not packageName config");
        return;
    }

    PolicyConfigData::PageUrlConfig pageUrlConfig = pageUrlConfig_[packageName];
    if (pageUrlConfig.count(pageName) <= 0) {
        HGM_LOGD("HgmAppPageUrlStrategy: not pageUrl config");
        return;
    }

    auto strategy = pageUrlConfig[pageName];
    if (isEnter && !pageUrlVoterInfo_[pid].hasVoter) {
        ProcessAddVoter(pid, strategy);
        pageUrlVoterInfo_[pid].hasVoter = true;
    } else if (!isEnter && pageUrlVoterInfo_[pid].hasVoter) {
        ProcessRemoveVoter(pid);
        pageUrlVoterInfo_[pid].hasVoter = false;
    }
}
} // namespace Rosen
} // namespace OHOS
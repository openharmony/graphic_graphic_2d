/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MODE_CONFIG_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MODE_CONFIG_H

#include <string>
#include <unordered_map>
#include <vector>
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
using GroupId = uint8_t;
constexpr GroupId DEFAULT_RENDER_PROCESS = 0;

struct ScreenInfoConfig {
    std::string screenName = "";
    ScreenId screenId = INVALID_SCREEN_ID;
};

struct GroupInfoConfig {
    std::string groupName = "";
    std::vector<ScreenInfoConfig> screenInfos;
};

class RenderModeConfig {
private:
    friend class RenderModeConfigBuilder;

    RenderModeConfig(const bool isMultiProcessModeEnabled,
                     const GroupId defaultRenderProcess,
                     std::map<GroupId, GroupInfoConfig>&& groupInfoConfigs,
                     std::unordered_map<ScreenId, GroupId>&& screenIdToGroupId) :
        isMultiProcessModeEnabled_(isMultiProcessModeEnabled),
        defaultRenderProcess_(defaultRenderProcess),
        groupInfoConfigs_(std::move(groupInfoConfigs)),
        screenIdToGroupId_(std::move(screenIdToGroupId))
    {}

public:
    bool GetIsMultiProcessModeEnabled() const { return isMultiProcessModeEnabled_; }
    GroupId GetDefaultRenderProcess() const { return defaultRenderProcess_; }
    const std::map<GroupId, GroupInfoConfig>& GetGroupInfoConfigs() const { return groupInfoConfigs_; }
    const std::unordered_map<ScreenId, GroupId>& GetScreenIdToGroupId() const { return screenIdToGroupId_; }

private:
    const bool isMultiProcessModeEnabled_;
    const GroupId defaultRenderProcess_;
    const std::map<GroupId, GroupInfoConfig> groupInfoConfigs_;
    const std::unordered_map<ScreenId, GroupId> screenIdToGroupId_;
};

class RenderModeConfigBuilder {
public:
    RenderModeConfigBuilder() = default;

    RenderModeConfigBuilder SetIsMultiProcessModeEnabled(const bool isMultiProcessModeEnabled)
    {
        isMultiProcessModeEnabled_ = isMultiProcessModeEnabled;
        return *this;
    }

    RenderModeConfigBuilder SetDefaultRenderProcess(const GroupId defaultRenderProcess)
    {
        defaultRenderProcess_ = defaultRenderProcess;
        return *this;
    }

    RenderModeConfigBuilder& SetGroupInfoConfigs(GroupId groupId, GroupInfoConfig&& groupInfoConfigs)
    {
        groupInfoConfigs_.insert_or_assign(groupId, std::move(groupInfoConfigs));
        return *this;
    }

    RenderModeConfigBuilder& SetScreenIdToGroupId(ScreenId screenId, GroupId groupId)
    {
        screenIdToGroupId_.insert_or_assign(screenId, groupId);
        return *this;
    }

    std::shared_ptr<const RenderModeConfig> Build()
    {
        return std::make_shared<const RenderModeConfig>(
            RenderModeConfig(isMultiProcessModeEnabled_, defaultRenderProcess_,
                std::move(groupInfoConfigs_), std::move(screenIdToGroupId_)));
    }

private:
    bool isMultiProcessModeEnabled_ = false;
    GroupId defaultRenderProcess_ = DEFAULT_RENDER_PROCESS;
    std::map<GroupId, GroupInfoConfig> groupInfoConfigs_;
    std::unordered_map<ScreenId, GroupId> screenIdToGroupId_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MODE_CONFIG_H

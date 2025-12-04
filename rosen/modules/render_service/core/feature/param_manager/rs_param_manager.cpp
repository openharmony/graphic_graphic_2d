/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "rs_param_manager.h"

#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "parameters.h"
#include "platform/common/rs_log.h"
#include "sys/file.h"

namespace OHOS {
namespace Rosen {

namespace {
const std::string LOCAL_PATH = "/system/etc/SwitchOffList/";
const std::string CLOUD_PATH = "/data/service/el1/public/update/param_service/install/system/etc/SwitchOffList/";
const std::string TARGET_SWITCH = "disable_ddgr_5_1_1_a"; // the switch to disable DDGR
const std::string SUBTREE_SWITCH = "disable_subtree_6_0_a"; // the switch to disable Subtree
constexpr int32_t VERSION_LEN = 4; // the number of digits extracted from the fixed format string "x.x.x.x"
const std::string RECEIVE_UPDATE_PERMISSION = "ohos.permission.RECEIVE_UPDATE_MESSAGE"; // permission to get message
const std::string CONFIG_UPDATED_ACTION = "usual.event.DUE_SA_CFG_UPDATED"; // common event of parameter update
static constexpr int32_t RETRY_SUBSCRIBER = 10; // retry up to 10 times to start subscribing
}

RSParamManager::~RSParamManager()
{
    UnSubscribeEvent();
}

RSParamManager& RSParamManager::GetInstance()
{
    static RSParamManager instance;
    return instance;
}

// Search the target switch in the configration file
bool RSParamManager::IsCloudDisableDDGR()
{
    std::string filePath = GetHigherVersionPath() + "switch_off_list";
    RS_LOGD("RSParamManager::IsCloudDisableDDGR: The filepath is %{public}s", filePath.c_str());
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        RS_LOGE("RSParamManager::IsCloudDisableDDGR: Can not open the file.");
        return false;
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        Trim(line);
        if (line == TARGET_SWITCH) {
            RS_LOGD("RSParamManager::IsCloudDisableDDGR: Succeed to find switch %{public}s.", TARGET_SWITCH.c_str());
            inputFile.close();
            return true;
        }
    }
    RS_LOGE("RSParamManager::IsCloudDisableDDGR: Failed to find switch %{public}s.", TARGET_SWITCH.c_str());
    inputFile.close();
    return false;
}

bool RSParamManager::IsCloudDisableSubtree()
{
    std::string filePath = GetHigherVersionPath() + "switch_off_list";
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        RS_LOGE("RSParamManager::IsCloudDisableSubtree: Can not open the file.");
        return false;
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        Trim(line);
        if (line == SUBTREE_SWITCH) {
            RS_LOGI("RSParamManager::IsCloudDisableSubtree: Succeed to find switch %{public}s.",
                SUBTREE_SWITCH.c_str());
            inputFile.close();
            return true;
        }
    }
    RS_LOGE("RSParamManager::IsCloudDisableSubtree: Failed to find switch %{public}s.", SUBTREE_SWITCH.c_str());
    inputFile.close();
    return false;
}

// Get path which contains a higher version configration file
std::string RSParamManager::GetHigherVersionPath()
{
    std::string localVersionFile = LOCAL_PATH + "version.txt"; // local version path
    std::string cloudVersionFile = CLOUD_PATH + "version.txt"; // cloud version path
    std::vector<std::string> localVersionNums = GetVersionNums(localVersionFile);
    std::vector<std::string> cloudVersionNums = GetVersionNums(cloudVersionFile);
    if (CompareVersion(localVersionNums, cloudVersionNums)) {
        return CLOUD_PATH;
    }
    return LOCAL_PATH;
}

std::vector<std::string> RSParamManager::SplitString(const std::string& str, char pattern)
{
    std::stringstream iss(str);
    std::vector<std::string> result;
    std::string token;
    while (getline(iss, token, pattern)) {
        result.emplace_back(token);
    }
    return result;
}

std::vector<std::string> RSParamManager::GetVersionNums(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        RS_LOGD("RSParamManager::GetVersionNums: version file is not open.");
        return {};
    }
    std::string line;
    std::getline(file, line);
    file.close();
    if (line.empty()) {
        RS_LOGD("RSParamManager::GetVersionNums: version file is empty.");
        return {};
    }
    std::vector<std::string> versionStr = SplitString(line, '=');
    const size_t expectedSize = 2; // 2: expecte to divide the string into two parts base on "=".
    if (versionStr.size() != expectedSize) {
        RS_LOGD("RSParamManager::GetVersionNums: version num is not valid, expected format 'key=value'.");
        return {};
    }
    if (versionStr[1].empty()) { // index 1 indicates string representing the version number.
        RS_LOGD("RSParamManager::GetVersionNums: version value is empty.");
        return {};
    }
    Trim(versionStr[1]);
    std::vector<std::string> versionNum = SplitString(versionStr[1], '.');
    return versionNum;
}

bool RSParamManager::CompareVersion(
    const std::vector<std::string> &localVersion, const std::vector<std::string> &cloudVersion)
{
    if (localVersion.size() != VERSION_LEN || cloudVersion.size() != VERSION_LEN) {
        RS_LOGD("RSParamManager::CompareVersion: version num is not valid.");
        return false;
    }
    for (auto i = 0; i < VERSION_LEN; i++) {
        if (localVersion[i] != cloudVersion[i]) {
            if (!IsNumber(localVersion[i]) || !IsNumber(cloudVersion[i])) {
                RS_LOGD("RSParamManager::CompareVersion: version part contains non-numeric characters.");
                return false;
            }
            int localValue = std::atoi(localVersion[i].c_str());
            int cloudValue = std::atoi(cloudVersion[i].c_str());
            bool ret = localValue < cloudValue ? true : false;
            return ret;
        }
    }
    return false;
}

void RSParamManager::SubscribeEvent()
{
    std::lock_guard<std::mutex> lock(subscriberMutex_);
    handleEventFunc_["usual.event.DUE_SA_CFG_UPDATED"] = &RSParamManager::HandleParamUpdate;
    for (auto it = handleEventFunc_.begin(); it != handleEventFunc_.end(); ++it) {
        RS_LOGD("RSParamManager::SubscribeEvent: Add event: %{public}s", it->first.c_str());
        eventHandles_.emplace(it->first, std::bind(it->second, this, std::placeholders::_1));
    }
    if (subscriber_) {
        RS_LOGD("RSParamManager::SubscribeEvent: Common Event is already subscribered!");
        return;
    }
    EventFwk::MatchingSkills matchingSkills;
    for (auto &event : handleEventFunc_) {
        RS_LOGD("RSParamManager::SubscribeEvent: MatchingSkills add event: %{public}s", event.first.c_str());
        matchingSkills.AddEvent(event.first);
    }
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPermission(RECEIVE_UPDATE_PERMISSION);
    subscriber_ = std::make_shared<ParamCommonEventSubscriber>(subscribeInfo, *this);
    if (subscriber_ == nullptr) {
        RS_LOGD("RSParamManager::SubscribeEvent: subscriber is nullptr.");
        return;
    }
    int32_t retry = RETRY_SUBSCRIBER;
    do {
        bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
        if (subscribeResult) {
            RS_LOGD("RSParamManager::SubscribeEvent: SubscribeEvent successfully.");
            return;
        } else {
            RS_LOGD("RSParamManager::SubscribeEvent: SubscribeEvent failed, retry %{public}d", retry);
            retry--;
            sleep(1);
        }
    } while (retry);
    RS_LOGD("RSParamManager::SubscribeEvent: SubscribeEvent failed.");
}

void RSParamManager::UnSubscribeEvent()
{
    RS_LOGD("RSParamManager::UnSubscribeEvent: UnSubscribe event.");
    std::lock_guard<std::mutex> lock(subscriberMutex_);
    eventHandles_.clear();
    handleEventFunc_.clear();
    if (subscriber_) {
        bool subscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        subscriber_.reset();
    }
}

void RSParamManager::OnReceiveEvent(const AAFwk::Want &want)
{
    std::lock_guard<std::mutex> lock(subscriberMutex_);
    std::string action = want.GetAction();
    auto it = eventHandles_.find(action);
    if (it == eventHandles_.end()) {
        RS_LOGD("RSParamManager::OnReceiveEvent: Ignore event: %{public}s", action.c_str());
        return;
    }
    RS_LOGD("RSParamManager::OnReceiveEvent: Handle event: %{public}s", action.c_str());
    HandleParamUpdate(want);
}

void RSParamManager::HandleParamUpdate(const AAFwk::Want &want)
{
    std::string action = want.GetAction();
    if (action != CONFIG_UPDATED_ACTION) {
        RS_LOGD("RSParamManager::HandleParamUpdate: Invalid sensor param update info: %{public}s", action.c_str());
        return;
    }
    // "1" indicates parameter update has been detected but has not restarted, it will revert to "0" once restarted.
    bool setParamResult = system::SetParameter("debug.graphic.cloudpushrestart", "1");
    if (!setParamResult) {
        RS_LOGE("RSParamManager::HandleParamUpdate: Failed to set parameter 'debug.graphic.cloudpushrestart'");
        return;
    }
    if (IsCloudDisableSubtree()) {
        // "1" indicates succeed to find the switch to disable Subtree, while "0" means it was not.
        setParamResult = system::SetParameter("persist.rosen.subtree.disabled", "1");
        if (!setParamResult) {
            RS_LOGE("RSParamManager::HandleParamUpdate: Failed to set subtree cloudpush parameter to 1'");
        }
    } else {
        setParamResult = system::SetParameter("persist.rosen.subtree.disabled", "0");
        if (!setParamResult) {
            RS_LOGE("RSParamManager::HandleParamUpdate: Failed to set subtree cloudpush parameter to 0'");
        }
    }
    if (IsCloudDisableDDGR()) {
        // "1" indicates succeed to find the switch to disable DDGR, while "0" means it was not.
        setParamResult = system::SetParameter("persist.rosen.disableddgr.enabled", "1");
        if (!setParamResult) {
            RS_LOGE("RSParamManager::HandleParamUpdate: Failed to set cloudpush parameter to 1'");
            return;
        }
    } else {
        setParamResult = system::SetParameter("persist.rosen.disableddgr.enabled", "0");
        if (!setParamResult) {
            RS_LOGE("RSParamManager::HandleParamUpdate: Failed to set cloudpush parameter to 0'");
            return;
        }
    }
}

} // namespace Rosen
} // namespace OHOS
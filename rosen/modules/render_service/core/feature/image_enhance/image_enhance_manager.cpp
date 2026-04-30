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

#include "image_enhance_manager.h"
#include "common/rs_common_hook.h"
#include "image_enhance_param.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
ImageEnhanceManager::ImageEnhanceManager()
{
    mImageEnhanceScene_ = ImageEnhanceParam::GetImageEnhanceScene();
    {
        for (auto& pkgName : mImageEnhanceScene_) {
            RS_LOGI("ImageEnhanceManager ImageEnhance pkgName[%{public}s]", pkgName.c_str());
        }
    }
}

void ImageEnhanceManager::CheckPackageInConfigList(const std::vector<std::string>& pkgs)
{
    std::lock_guard<std::mutex> setMutex(mutexLock_);
    RS_LOGI("ImageEnhanceManager CheckPackageInConfigList start");
    // update pkgs
    if (pkgs_ == pkgs) {
        return;
    }
    pkgs_ = pkgs;

    std::unordered_set<pid_t> imageEnhancePidList = {};
    for (auto& param : pkgs_) {
        RS_LOGI("pkg update:%{public}s", param.c_str());
        auto [pkgName, pid, appType] = AnalyzePkgParam(param);
        CheckImageEnhanceList(pkgName, pid, imageEnhancePidList);
    }
    RsCommonHook::Instance().SetImageEnhancePidList(imageEnhancePidList);
}

std::tuple<std::string, pid_t, int32_t> ImageEnhanceManager::AnalyzePkgParam(const std::string& param)
{
    std::string pkgName = param;
    pid_t pid = 0;
    int32_t appType = -1;

    auto index = param.find(":");
    if (index == std::string::npos) {
        return { pkgName, pid, appType };
    }

    // split by : index
    pkgName = param.substr(0, index);
    auto pidAppType = param.substr(index + 1, param.size());
    index = pidAppType.find(":");
    if (index == std::string::npos) {
        if (IsNumber(pidAppType)) {
            pid = static_cast<pid_t>(std::stoi(pidAppType));
        }
        return { pkgName, pid, appType };
    }

    // split by : index
    auto pidStr = pidAppType.substr(0, index);
    if (IsNumber(pidStr)) {
        pid = static_cast<pid_t>(std::stoi(pidStr));
    }

    auto appTypeStr = pidAppType.substr(index + 1, pidAppType.size());
    if (IsNumber(appTypeStr)) {
        appType = std::stoi(appTypeStr);
    }
    return { pkgName, pid, appType };
}

void ImageEnhanceManager::CheckImageEnhanceList(
    const std::string& pkgName, const pid_t pid, std::unordered_set<pid_t>& imageEnhancePidList) const
{
    if (mImageEnhanceScene_.empty()) {
        return;
    }
    if (mImageEnhanceScene_.find(pkgName) != mImageEnhanceScene_.end()) {
        imageEnhancePidList.insert(pid);
    }
}

bool ImageEnhanceManager::IsNumber(const std::string& str)
{
    if (str.length() == 0 || str.length() > PKG_STRING_MAX_LENGTH) {
        return false;
    }
    auto number = static_cast<uint32_t>(std::count_if(str.begin(), str.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}
} // namespace OHOS::Rosen
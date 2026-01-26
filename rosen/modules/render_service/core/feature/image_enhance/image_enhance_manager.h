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

#ifndef RENDER_SERVICE_CORE_PIPELINE_IMAGE_ENHANCE_MANAGER_H
#define RENDER_SERVICE_CORE_PIPELINE_IMAGE_ENHANCE_MANAGER_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace OHOS::Rosen {
constexpr uint32_t PKG_STRING_MAX_LENGTH = 8;

class ImageEnhanceManager {
public:
    ImageEnhanceManager();
    ~ImageEnhanceManager() = default;

    ImageEnhanceManager(const ImageEnhanceManager&) = delete;
    ImageEnhanceManager(const ImageEnhanceManager&&) = delete;
    ImageEnhanceManager& operator=(const ImageEnhanceManager&) = delete;
    ImageEnhanceManager& operator=(const ImageEnhanceManager&&) = delete;

    void CheckPackageInConfigList(const std::vector<std::string>& pkgs);

private:
    std::tuple<std::string, pid_t, int32_t> AnalyzePkgParam(const std::string& param);
    bool IsNumber(const std::string& str);
    void CheckImageEnhanceList(
        const std::string& pkgName, const pid_t pid, std::unordered_set<pid_t>& imageEnhancePidList) const;

    mutable std::mutex mutexLock_{};
    std::vector<std::string> pkgs_;
    std::unordered_set<std::string> mImageEnhanceScene_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_CORE_PIPELINE_IMAGE_ENHANCE_MANAGER_H
/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_INFO_H
#define RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_INFO_H

#include "rs_hrp_service.h"

namespace OHOS {
namespace Rosen {
using OnRemoteDiedCallback = std::function<void()>;

struct DataBaseRs {
    int32_t appPid = -1;
    int32_t eventType = -1;
    int32_t versionCode = -1;
    int64_t uniqueId = 0;
    int64_t inputTime = 0;
    int64_t beginVsyncTime = 0;
    int64_t endVsyncTime = 0;
    bool isDisplayAnimator = false;
    std::string sceneId;
    std::string versionName;
    std::string bundleName;
    std::string processName;
    std::string abilityName;
    std::string pageUrl;
    std::string sourceType;
    std::string note;
};

struct AppInfo {
    int64_t startTime = 0;
    int64_t endTime = 0;
    int32_t pid = 0;
    std::string versionName;
    int32_t versionCode = -1;
    std::string bundleName;
    std::string processName;
};
struct GameStateData {
    int32_t pid = -1;
    int32_t uid = 0;
    int32_t state = 0;
    int32_t renderTid = -1;
    std::string bundleName;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_INFO_H
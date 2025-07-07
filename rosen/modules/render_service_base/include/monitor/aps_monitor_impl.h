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

#ifndef RENDER_SERVICE_BASE_APS_MONITOR_IMPL_H
#define RENDER_SERVICE_BASE_APS_MONITOR_IMPL_H

#include <map>
#include <string>
#include <set>

namespace OHOS {
namespace Rosen {

class ApsMonitorImpl {
public:
    void SetApsSurfaceBoundChange(std::string height, std::string width, std::string id);
    void SetApsSurfaceDestroyedInfo(std::string id);
    void SetApsSurfaceNodeTreeChange(bool onTree, std::string name);
    static ApsMonitorImpl& GetInstance();
    using SendApsEventFunc = void (*)(int32_t, std::map<std::string, std::string>);
    using SetBoundChangeFunc = void (*)(std::string height, std::string width, std::string id);
    using SetSurfaceDestroyedFunc = void (*)(std::string id);

private:
    ApsMonitorImpl() = default;
    ~ApsMonitorImpl();
    bool IsCloseAps();
    void LoadApsFuncsOnce();
    void* loadFileHandle_ = nullptr;
    SendApsEventFunc sendApsEventFunc_ = nullptr;
    SetBoundChangeFunc setBoundChangeFunc_ = nullptr;
    SetSurfaceDestroyedFunc setSurfaceDestroyedFunc_ = nullptr;
    bool isApsFuncsAvailable_ = true;
    bool isApsFuncsLoad_ = false;
};

} // namespace Rosen
} // namespace OHOS
#endif
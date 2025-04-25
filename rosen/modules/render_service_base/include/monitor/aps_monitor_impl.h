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

#include <string>
#include <set>

namespace OHOS {
namespace Rosen {
using SetBoundChangeFunc = void (*)(std::string pkgName, std::string sceneName, std::string state);
using SetSurfaceDestroyFunc = void (*)(std::string id);

class ApsMonitorImpl {
public:
    ApsMonitorImpl() = default;
    ~ApsMonitorImpl();
    void SetApsSurfaceBoundChange(std::string height, std::string width, std::string id);
    void SetApsSurfaceDestroyedInfo(std::string id);

private:
    void LoadApsFuncOnce();
    static const std::set<std::string> apsScenes;
    void* loadfilehandle_ = nullptr;
    SetBoundChangeFunc setBoundChangeFunc_ = nullptr;
    SetSurfaceDestroyFunc setSurfaceDestroyFunc_ = nullptr;
    bool isloadapsfunc_ = false;
};

} // namespace Rosen
} // namespace OHOS
#endif
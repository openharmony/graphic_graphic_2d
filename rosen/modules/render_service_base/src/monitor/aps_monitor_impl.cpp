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

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include "monitor/aps_monitor_impl.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string APS_CLIENT_SO = "libaps_client.z.so";
const std::string SURFACE_BOUND_CHANGE = "SURFACE_BOUND_CHANGE";
const std::string SURFACE_DESTROYED = "SURFACE_DESTROYED";
const std::string SURFACE_NODE_TREE_CHANGE = "SURFACE_NODE_TREE_CHANGE";
}

ApsMonitorImpl::~ApsMonitorImpl()
{
    if (loadFileHandle_ != nullptr) {
#ifdef _WIN32
        FreeLibrary((HMODULE)loadFileHandle_);
#else
        dlclose(loadFileHandle_);
#endif
        loadFileHandle_ = nullptr;
    }
}

ApsMonitorImpl& ApsMonitorImpl::GetInstance()
{
    static ApsMonitorImpl apsMonitorImpl;
    return apsMonitorImpl;
}

void ApsMonitorImpl::SetApsSurfaceBoundChange(std::string height, std::string width, std::string id)
{
    LoadApsFuncsOnce();
    if (sendApsEventFunc_ == nullptr) {
        return;
    }
    std::unordered_map<std::string, std::string> eventData;
    eventData.emplace("height", std::move(height));
    eventData.emplace("width", std::move(width));
    eventData.emplace("id", std::move(id));
    sendApsEventFunc_(SURFACE_BOUND_CHANGE, std::move(eventData));
}

void ApsMonitorImpl::SetApsSurfaceDestroyedInfo(std::string id)
{
    LoadApsFuncsOnce();
    if (sendApsEventFunc_ == nullptr) {
        return;
    }
    std::unordered_map<std::string, std::string> eventData;
    eventData.emplace("id", std::move(id));
    sendApsEventFunc_(SURFACE_DESTROYED, std::move(eventData));
}

void ApsMonitorImpl::SetApsSurfaceNodeTreeChange(bool onTree, std::string name)
{
    LoadApsFuncsOnce();
    if (sendApsEventFunc_ == nullptr) {
        return;
    }
    std::unordered_map<std::string, std::string> eventData;
    eventData.emplace("onTree", std::to_string(onTree));
    eventData.emplace("name", std::move(name));
    sendApsEventFunc_(SURFACE_NODE_TREE_CHANGE, std::move(eventData));
}

void ApsMonitorImpl::LoadApsFuncsOnce()
{
    if (!isApsFuncsAvailable_ || isApsFuncsLoad_) {
        return;
    }
#ifdef _WIN32
    loadFileHandle_ = (void*)LoadLibraryA(APS_CLIENT_SO.c_str());
#else
    loadFileHandle_ = dlopen(APS_CLIENT_SO.c_str(), RTLD_NOW);
#endif
    if (loadFileHandle_ == nullptr) {
        isApsFuncsAvailable_ = false;
        return;
    }
#ifdef _WIN32
    sendApsEventFunc_ = reinterpret_cast<SendApsEventFunc>(
        (void*)GetProcAddress((HMODULE)loadFileHandle_, "SendApsEvent"));
#else
    sendApsEventFunc_ = reinterpret_cast<SendApsEventFunc>(dlsym(loadFileHandle_, "SendApsEvent"));
#endif
    if (sendApsEventFunc_ == nullptr) {
#ifdef _WIN32
        FreeLibrary((HMODULE)loadFileHandle_);
#else
        dlclose(loadFileHandle_);
#endif
        isApsFuncsAvailable_ = false;
        return;
    }
    isApsFuncsLoad_ = true;
}
} // namespace Rosen
} // namespace OHOS
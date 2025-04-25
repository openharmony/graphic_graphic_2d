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

namespace OHOS {
namespace Rosen {
using namespace std;
namespace {
    const std::string APS_CLIENT_SO = "libaps_client.z.so";
}

ApsMonitorImpl::~ApsMonitorImpl()
{
    if (loadfilehandle_ != nullptr) {
#ifdef _WIN32
        FreeLibrary((HMODULE)loadfilehandle_);
#else
        dlclose(loadfilehandle_);
#endif
        loadfilehandle_ = nullptr;
    }
}

void ApsMonitorImpl::SetApsSurfaceBoundChange(std::string height, std::string width, std::string id)
{
    LoadApsFuncOnce();
    if (setBoundChangeFunc_ == nullptr) {
        return;
    }
    setBoundChangeFunc_(height, width, id);
    return;
}

void ApsMonitorImpl::SetApsSurfaceDestroyedInfo(std::string id)
{
    LoadApsFuncOnce();
    if (setSurfaceDestroyFunc_ == nullptr) {
        return;
    }
    setSurfaceDestroyFunc_(id);
}

void ApsMonitorImpl::LoadApsFuncOnce()
{
    if (isloadapsfunc_) {
        return;
    }
#ifdef _WIN32
    loadfilehandle_ = (void*)LoadLibraryA(APS_CLIENT_SO.c_str());
#else
    loadfilehandle_ = dlopen(APS_CLIENT_SO.c_str(), RTLD_NOW);
#endif
    if (loadfilehandle_ == nullptr) {
        return;
    }
#ifdef _WIN32
    setBoundChangeFunc_ = reinterpret_cast<SetBoundChangeFunc>(
        (void*)GetProcAddress((HMODULE)loadfilehandle_, "SetApsSurfaceBoundChange")
    );
    setSurfaceDestroyFunc_ = reinterpret_cast<SetSurfaceDestroyFunc>(
        (void*)GetProcAddress((HMODULE)loadfilehandle_, "SetApsSurfaceDestroyedInfo")
    );
#else
    setBoundChangeFunc_ =
        reinterpret_cast<SetBoundChangeFunc>(dlsym(loadfilehandle_, "SetApsSurfaceBoundChange"));
    setSurfaceDestroyFunc_ =
        reinterpret_cast<SetSurfaceDestroyFunc>(dlsym(loadfilehandle_, "SetApsSurfaceDestroyedInfo"));
#endif
    if (setBoundChangeFunc_ == nullptr || setSurfaceDestroyFunc_ == nullptr) {
#ifdef _WIN32
        FreeLibrary((HMODULE)loadfilehandle_);
#else
        dlclose(loadfilehandle_);
#endif
        return;
    }
    isloadapsfunc_ = true;
}
} // namespace Rosen
} // namespace OHOS
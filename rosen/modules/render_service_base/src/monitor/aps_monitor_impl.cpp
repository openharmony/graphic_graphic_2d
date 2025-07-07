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
const int32_t SURFACE_NODE_TREE_CHANGE = 1;
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

void ApsMonitorImpl::SetApsSurfaceBoundChange(std::string height, std::string width,
    std::string id)
{
    LoadApsFuncsOnce();
    if (setBoundChangeFunc_ == nullptr) {
        return;
    }
    setBoundChangeFunc_(std::move(height), std::move(width), std::move(id));
    return;
}

void ApsMonitorImpl::SetApsSurfaceDestroyedInfo(std::string id)
{
    LoadApsFuncsOnce();
    if (setSurfaceDestroyedFunc_ == nullptr) {
        return;
    }
    setSurfaceDestroyedFunc_(std::move(id));
}

void ApsMonitorImpl::SetApsSurfaceNodeTreeChange(bool onTree, std::string name)
{
    LoadApsFuncsOnce();
    if (sendApsEventFunc_ == nullptr) {
        return;
    }
    std::map<std::string, std::string> apsData;
    apsData["onTree"] = std::to_string(onTree);
    apsData["name"] = name;
    sendApsEventFunc_(SURFACE_NODE_TREE_CHANGE, apsData);
}

bool ApsMonitorImpl::IsCloseAps()
{
#ifdef _WIN32
    setBoundChangeFunc_ = reinterpret_cast<SetBoundChangeFunc>(
        (void*)GetProcAddress((HMODULE)loadFileHandle_, "SetApsSurfaceBoundChange"));
    setSurfaceDestroyedFunc_ = reinterpret_cast<SetSurfaceDestroyedFunc>(
        (void*)GetProcAddress((HMODULE)loadFileHandle_, "SetApsSurfaceDestroyedInfo"));
    sendApsEventFunc_ = reinterpret_cast<SendApsEventFunc>(
        (void*)GetProcAddress((HMODULE)loadFileHandle_, "SendApsEvent"));
#else
    setBoundChangeFunc_ =
        reinterpret_cast<SetBoundChangeFunc>(dlsym(loadFileHandle_, "SetApsSurfaceBoundChange"));
    setSurfaceDestroyedFunc_ =
        reinterpret_cast<SetSurfaceDestroyedFunc>(dlsym(loadFileHandle_, "SetApsSurfaceDestroyedInfo"));
    sendApsEventFunc_ = reinterpret_cast<SendApsEventFunc>(dlsym(loadFileHandle_, "SendApsEvent"));
#endif
    return setBoundChangeFunc_ == nullptr || setSurfaceDestroyedFunc_ == nullptr || sendApsEventFunc_ == nullptr;
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

    if (IsCloseAps()) {
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
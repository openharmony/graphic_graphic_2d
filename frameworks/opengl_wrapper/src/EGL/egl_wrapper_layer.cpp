/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "egl_wrapper_layer.h"

#include <dlfcn.h>
#include <cstdlib>
#include <parameter.h>
#include <parameters.h>
#include <sstream>

#include "wrapper_log.h"
#include "directory_ex.h"
#include "egl_bundle_mgr_helper.h"

namespace OHOS {
namespace {
#if (defined(__aarch64__) || defined(__x86_64__))
    constexpr const char *DEBUG_LAYERS_LIB_DIR = "/system/lib64/";
#else
    constexpr const char *DEBUG_LAYERS_LIB_DIR = "/system/lib/";
#endif
constexpr const char *DEBUG_LAYERS_PREFIX = "lib";
constexpr const char *DEBUG_LAYERS_SUFFIX = ".so";
constexpr const char *DEBUG_LAYERS_DELIMITER = ":";
constexpr const char *DEBUG_LAYER_INIT_FUNC = "DebugLayerInitialize";
constexpr const char *DEBUG_LAYER_GET_PROC_ADDR_FUNC = "DebugLayerGetProcAddr";
constexpr const char *DEBUG_LAYER_NAME = "debug.graphic.debug_layer";
constexpr const char *DEBUG_HAP_NAME = "debug.graphic.debug_hap";
constexpr const char *DEBUG_SANDBOX_DIR = "/data/storage/el1/bundle/";
}

static std::string g_strLayers;
static AppExecFwk::BundleInfo g_bundleInfo;

static void GetWrapperDebugLayers(const char *key, const char *value, void *context)
{
    WLOGD("");
    g_strLayers = std::string(value);
    WLOGD("g_strLayers is %{public}s", g_strLayers.c_str());
}

static void UpdateApiEntries(LayerSetupFunc func,
    EglWrapperFuncPointer *curr, char const * const *entries)
{
    WLOGD("");
    while (*entries) {
        char const *name = *entries;
        EglWrapperFuncPointer layerFunc = func(name, *curr);
        if (layerFunc == nullptr) {
            WLOGW("LayerSetupFunc(%{public}s) return nullptr.", name);
        } else if (layerFunc != *curr) {
            WLOGI("Update api entry for %{public}s", name);
            *curr = layerFunc;
        }
        curr++;
        entries++;
    }
}

static void SetupFuncMaps(FunctionTable &table,
    char const * const *entries, EglWrapperFuncPointer *curr)
{
    WLOGD("");
    while (*entries) {
        const char *name = *entries;
        if (table.find(name) == table.end()) {
            table[name] = *curr;
        } else {
            WLOGD("duplicate function name(%{public}s)", name);
        }

        entries++;
        curr++;
    }
}

static const void *GetNextLayerProcAddr(void *funcTable, const char *name)
{
    WLOGD("name = %{public}s", name);
    auto table = reinterpret_cast<FunctionTable *>(funcTable);
    if (table->find(name) == table->end()) {
        WLOGE("not found in function table, name = %{public}s", name);
        return nullptr;
    }

    EglWrapperFuncPointer val = (*table)[name];
    return reinterpret_cast<void *>(val);
}

static void SplitEnvString(std::string str, std::vector<std::string> *layers)
{
    WLOGD("");
    size_t pos1 = 0;
    size_t pos2 = str.find(DEBUG_LAYERS_DELIMITER);
    while (pos2 != std::string::npos) {
        layers->emplace_back(str.substr(pos1, pos2-pos1));
        pos1 = pos2 + 1;
        pos2 = str.find(DEBUG_LAYERS_DELIMITER, pos1);
    }

    if (pos1 != str.length()) {
        layers->emplace_back(str.substr(pos1));
    }
}

static std::vector<std::string> GetDebugLayers(void)
{
    WLOGD("");
    std::vector<std::string> layers;

    g_strLayers = system::GetParameter(DEBUG_LAYER_NAME, "");
    WLOGD("g_strLayers is %{public}s", g_strLayers.c_str());
    auto ret = WatchParameter(DEBUG_LAYER_NAME, GetWrapperDebugLayers, nullptr);
    if (ret) {
        WLOGD("WatchParameter faild.");
    }

    if (!g_strLayers.empty()) {
        SplitEnvString(g_strLayers, &layers);
    } else {
        WLOGD("OPENGL_WRAPPER_DEBUG_LAYERS is not set.");
    }

    for (auto i : layers) {
        WLOGD("OPENGL_WRAPPER_DEBUG_LAYERS layer = %{public}s\n", i.c_str());
    }

    return layers;
}

static std::vector<std::string> GetDebugLayerPaths()
{
    WLOGD("GetDebugLayerPaths");
    std::vector<std::string> layerPaths = {std::string(DEBUG_LAYERS_LIB_DIR)};
    std::string pathStr(DEBUG_SANDBOX_DIR);

    std::string appLibPath = g_bundleInfo.applicationInfo.nativeLibraryPath;
    if (!appLibPath.empty()) {
        layerPaths.push_back(pathStr + appLibPath + "/");
    }

    for (auto hapModuleInfo: g_bundleInfo.hapModuleInfos) {
        if (!hapModuleInfo.nativeLibraryPath.empty()) {
            layerPaths.push_back(hapModuleInfo.nativeLibraryPath + "/");
        }
    }
    return layerPaths;
}

static std::vector<std::string> GetSquidHapLayerPaths()
{
    WLOGD("GetSquidHapLayerPaths");
    std::vector<std::string> layerPaths = {};
    std::string pathStr(DEBUG_SANDBOX_DIR);

    std::string appLibPath = g_bundleInfo.applicationInfo.nativeLibraryPath;
    if (!appLibPath.empty()) {
        layerPaths.push_back(pathStr + appLibPath + "/");
    }

    for (auto hapModuleInfo: g_bundleInfo.hapModuleInfos) {
        if (!hapModuleInfo.nativeLibraryPath.empty()) {
            layerPaths.push_back(hapModuleInfo.nativeLibraryPath + "/");
        }
    }
    return layerPaths;
}

EglWrapperLayer& EglWrapperLayer::GetInstance()
{
    static EglWrapperLayer layer;
    return layer;
}

EglWrapperLayer::~EglWrapperLayer()
{
    WLOGD("");
    if (dlhandle_) {
        dlclose(dlhandle_);
        dlhandle_ = nullptr;
    }
}

bool EglWrapperLayer::Init(EglWrapperDispatchTable *table)
{
    WLOGD("");
    if (initialized_) {
        WLOGI("EglWrapperLayer is already loaded.");
        return true;
    }

    if (table == nullptr) {
        WLOGE("table is nullptr.");
        return false;
    }

    if (!LoadLayers()) {
        WLOGE("LoadLayers failed.");
        return false;
    }

    InitLayers(table);

    initialized_ = true;
    return true;
}

bool EglWrapperLayer::InitBundleInfo()
{
    std::string debugHap = system::GetParameter(DEBUG_HAP_NAME, "");
    if (debugHap.empty()) {
        WLOGD("The parameter for debug hap name is not set!");
        return false;
    }
    auto eglBundleMgrHelper = DelayedSingleton<AppExecFwk::EGLBundleMgrHelper>::GetInstance();
    if (eglBundleMgrHelper == nullptr) {
        WLOGE("eglBundleMgrHelper is null!");
        return false;
    }

    if (eglBundleMgrHelper->GetBundleInfoForSelf(
        AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION, g_bundleInfo) == ERR_OK) {
        if (g_bundleInfo.name == debugHap) {
            return true;
        } else {
            WLOGD("this hap is %{public}s, the debug hap is %{public}s",
                g_bundleInfo.name.c_str(), debugHap.c_str());
        }
    } else {
        WLOGE("Call GetBundleInfoForSelf func failed!");
    }
    return false;
}

void EglWrapperLayer::SetupLayerFuncTbl(EglWrapperDispatchTable *table)
{
    layerFuncTbl_.resize(layerSetup_.size() + 1);
    char const * const *entries;
    EglWrapperFuncPointer *curr;

    entries = gWrapperApiNames;
    curr = reinterpret_cast<EglWrapperFuncPointer*>(&table->wrapper);
    SetupFuncMaps(layerFuncTbl_[0], entries, curr);

    entries = gGlApiNames2;
    curr = reinterpret_cast<EglWrapperFuncPointer*>(&table->gl.table2);
    SetupFuncMaps(layerFuncTbl_[0], entries, curr);

    entries = gGlApiNames3;
    curr = reinterpret_cast<EglWrapperFuncPointer*>(&table->gl.table3);
    SetupFuncMaps(layerFuncTbl_[0], entries, curr);

    for (uint32_t i = 0; i < layerSetup_.size(); i++) {
        layerInit_[i](reinterpret_cast<void*>(&layerFuncTbl_[i]),
            reinterpret_cast<GetNextLayerAddr>(GetNextLayerProcAddr));

        entries = gWrapperApiNames;
        curr = reinterpret_cast<EglWrapperFuncPointer *>(&table->wrapper);
        UpdateApiEntries(layerSetup_[i], curr, entries);
        SetupFuncMaps(layerFuncTbl_[i + 1], entries, curr);

        entries = gGlApiNames2;
        curr = reinterpret_cast<EglWrapperFuncPointer*>(&table->gl.table2);
        UpdateApiEntries(layerSetup_[i], curr, entries);
        SetupFuncMaps(layerFuncTbl_[i + 1], entries, curr);

        entries = gGlApiNames3;
        curr = reinterpret_cast<EglWrapperFuncPointer*>(&table->gl.table3);
        UpdateApiEntries(layerSetup_[i], curr, entries);
        SetupFuncMaps(layerFuncTbl_[i + 1], entries, curr);
    }
}

bool EglWrapperLayer::LoadLayers()
{
    WLOGD("");
    std::vector<std::string> layers = GetDebugLayers();

    std::vector<std::string> systemLayers = eglSystemLayersManager_.GetSystemLayers();

#if USE_IGRAPHICS_EXTENDS_HOOKS
    std::string iGraphicsLibName("iGraphicsCore.z");
    for (const auto &i : systemLayers) {
        if (i == iGraphicsLibName) {
            iGraphicsLogic = true;
        } else {
            layers.push_back(i);
        }
    }
#else
    layers.insert(layers.end(), systemLayers.begin(), systemLayers.end());
#endif

    if (layers.empty()) {
        WLOGD("layers is empty");
        return false;
    }

    if (!InitBundleInfo()) {
        WLOGD("Get BundleInfo failed.");
    } else {
        WLOGD("Get BundleInfo ok.");
    }

    WLOGD("appProvisionType is %{public}s !", g_bundleInfo.applicationInfo.appProvisionType.c_str());
    if (std::find(layers.begin(), layers.end(), "squid") != layers.end() &&
        g_bundleInfo.applicationInfo.appProvisionType == "release") {
        WLOGD("appProvisionType is release.");
        return false;
    }
    return DoLoadLayers(layers);
}

bool EglWrapperLayer::LoadLayers(const std::string& libname, const std::vector<std::string> &layerPaths)
{
    bool success = false;
    for (auto layerPath : layerPaths) {
        layerPath += libname;
        std::string realLayerPath;
        WLOGD("LoadLayers layerPath=%{public}s", layerPath.c_str());
        if (!PathToRealPath(layerPath, realLayerPath)) {
            WLOGD("LoadLayers PathToRealPath is failed");
            continue;
        }
        WLOGD("LoadLayers realLayerPath=%{private}s", realLayerPath.c_str());
        if (!LoadLayerFuncs(realLayerPath)) {
            WLOGD("LoadLayers LoadLayerFuncs is failed");
            return false;
        } else {
            success = true;
        }
    }
    return success;
}

bool EglWrapperLayer::DoLoadLayers(const std::vector<std::string>& layers)
{
    for (size_t i = layers.size() - 1; i >= 0; i--) {
        std::string layerLib = std::string(DEBUG_LAYERS_PREFIX) + layers[i] + std::string(DEBUG_LAYERS_SUFFIX);
        std::vector<std::string> allLayerPaths;
        if (layerLib == "libsquid.so") {
            allLayerPaths = GetSquidHapLayerPaths();
            if (!LoadLayers(layerLib, allLayerPaths) && !LoadLayerFuncs(layerLib)) {
                WLOGD("LoadLayers squid is faild");
                return false;
            } else {
                WLOGD("LoadLayers squid is ok!");
            }
        } else {
            allLayerPaths = GetDebugLayerPaths();
            WLOGD("LoadLayerFuncs layerLib=%{public}s", layerLib.c_str());
            if (!LoadLayers(layerLib, allLayerPaths)) {
                return false;
            } else {
                WLOGD("LoadLayerFuncs is ok!");
            }
        }
    }
    return true;
}

bool EglWrapperLayer::LoadLayerFuncs(const std::string& realLayerPath)
{
    dlhandle_ = dlopen(realLayerPath.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (dlhandle_ == nullptr) {
        WLOGE("dlopen failed. error: %{public}s.", dlerror());
        return false;
    }

    LayerInitFunc initFunc = (LayerInitFunc)dlsym(dlhandle_, DEBUG_LAYER_INIT_FUNC);
    if (initFunc == nullptr) {
        WLOGE("can't find %{public}s in debug layer library.", DEBUG_LAYER_INIT_FUNC);
        dlclose(dlhandle_);
        dlhandle_ = nullptr;
        return false;
    }
    layerInit_.push_back(initFunc);

    LayerSetupFunc setupFunc = (LayerSetupFunc)dlsym(dlhandle_, DEBUG_LAYER_GET_PROC_ADDR_FUNC);
    if (setupFunc == nullptr) {
        WLOGE("can't find %{public}s in debug layer library.", DEBUG_LAYER_GET_PROC_ADDR_FUNC);
        dlclose(dlhandle_);
        dlhandle_ = nullptr;
        return false;
    }
    layerSetup_.push_back(setupFunc);
    return true;
}

void EglWrapperLayer::InitLayers(EglWrapperDispatchTable *table)
{
    WLOGD("");
    if (layerInit_.empty() || layerSetup_.empty()) {
        return;
    }

    SetupLayerFuncTbl(table);
#ifdef EGL_WRAPPER_DEBUG_ENABLE
    WLOGD("layer function table info: ");
    int count = 0;
    for (const auto &table : layerFuncTbl_) {
        WLOGD("===table%{public}d===: ", count++);
        for (const auto &item : table) {
            WLOGD("name: %{public}s : %{public}s", item.first.c_str(),
                (item.second != nullptr) ? "valid" : "invalid");
        }
    }
#endif
}
} // namespace OHOS

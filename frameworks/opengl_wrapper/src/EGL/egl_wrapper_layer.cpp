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
constexpr const char *DEBUG_LAYER_NAME = "debug_layer";
}

static std::string g_strLayers;

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
            WLOGW("duplicate function name(%{public}s)", name);
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

EglWrapperLayer& EglWrapperLayer::GetInstance()
{
    static EglWrapperLayer layer;
    return layer;
}

EglWrapperLayer::~EglWrapperLayer()
{
    WLOGD("");
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
    if (layers.empty()) {
        WLOGD("layers is empty");
        return false;
    }
    for (int32_t i = layers.size() - 1; i >= 0; i--) {
        std::string layerLib = std::string(DEBUG_LAYERS_PREFIX) + layers[i] + std::string(DEBUG_LAYERS_SUFFIX);
        std::string layerPath = std::string(DEBUG_LAYERS_LIB_DIR) + layerLib;
        WLOGD("layerPath = %{public}s", layerPath.c_str());

        void *dlhandle = dlopen(layerPath.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (dlhandle == nullptr) {
            WLOGE("dlopen failed. error: %{public}s.", dlerror());
            return false;
        }

        LayerInitFunc initFunc = (LayerInitFunc)dlsym(dlhandle, DEBUG_LAYER_INIT_FUNC);
        if (initFunc == nullptr) {
            WLOGE("can't find %{public}s in debug layer library.", DEBUG_LAYER_INIT_FUNC);
            return false;
        }
        layerInit_.push_back(initFunc);

        LayerSetupFunc setupFunc = (LayerSetupFunc)dlsym(dlhandle, DEBUG_LAYER_GET_PROC_ADDR_FUNC);
        if (setupFunc == nullptr) {
            WLOGE("can't find %{public}s in debug layer library.", DEBUG_LAYER_GET_PROC_ADDR_FUNC);
            return false;
        }
        layerSetup_.push_back(setupFunc);
    }
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

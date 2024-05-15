/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <string>

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <hilog/log.h>

#include "plugin_manager.h"
#include "../common/common.h"

PluginManager PluginManager::m_pluginManager;

PluginManager::~PluginManager()
{
    for (auto iter = m_nativeXComponentMap.begin(); iter != m_nativeXComponentMap.end(); ++iter) {
        if (iter->second != nullptr) {
            delete iter->second;
            iter->second = nullptr;
        }
    }
    m_nativeXComponentMap.clear();

    for (auto iter = m_pluginRenderMap.begin(); iter != m_pluginRenderMap.end(); ++iter) {
        if (iter->second != nullptr) {
            delete iter->second;
            iter->second = nullptr;
        }
    }
    m_pluginRenderMap.clear();
}

void PluginManager::Export(napi_env env, napi_value exports)
{
    if ((env == nullptr) || (exports == nullptr)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "Export: env or exports is null");
        return;
    }

    napi_value exportInstance = nullptr;
    if (napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "Export: napi_get_named_property fail");
        return;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "Export: napi_unwrap fail");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = { '\0' };
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager",
            "Export: OH_NativeXComponent_GetXComponentId fail");
        return;
    }

    std::string id(idStr);
    auto context = PluginManager::GetInstance();
    if ((context != nullptr) && (nativeXComponent != nullptr)) {
        context->SetNativeXComponent(id, nativeXComponent);
        auto render = context->GetRender(id);
        OH_NativeXComponent_RegisterCallback(nativeXComponent, &PluginRender::m_callback);
        if (render != nullptr) {
            render->Export(env, exports);
        }
    }
}

void PluginManager::SetNativeXComponent(std::string &id, OH_NativeXComponent *nativeXComponent)
{
    if (nativeXComponent == nullptr) {
        return;
    }

    if (m_nativeXComponentMap.find(id) == m_nativeXComponentMap.end()) {
        m_nativeXComponentMap[id] = nativeXComponent;
        return;
    }

    if (m_nativeXComponentMap[id] != nativeXComponent) {
        OH_NativeXComponent *tmp = m_nativeXComponentMap[id];
        delete tmp;
        tmp = nullptr;
        m_nativeXComponentMap[id] = nativeXComponent;
    }
}

PluginRender *PluginManager::GetRender(std::string &id)
{
    if (m_pluginRenderMap.find(id) == m_pluginRenderMap.end()) {
        PluginRender *instance = PluginRender::GetInstance(id);
        m_pluginRenderMap[id] = instance;
        return instance;
    }

    return m_pluginRenderMap[id];
}

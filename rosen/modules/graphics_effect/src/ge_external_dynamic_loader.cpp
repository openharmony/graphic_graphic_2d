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
#include "ge_external_dynamic_loader.h"
#include "ge_log.h"
#include "ge_system_properties.h"

#ifdef GE_PLATFORM_UNIX
#include <dlfcn.h>
#endif

namespace OHOS {
namespace Rosen {
namespace {
const std::string GRAPHICS_EFFECT_EXT_ENABLE = "rosen.graphic.gex.enable";
#ifdef GE_PLATFORM_UNIX
#if (defined(__aarch64__) || defined(__x86_64__))
const std::string GRAPHICS_EFFECT_EXT_LIB_PATH = "/system/lib64/libgraphics_effect_ext.z.so";
#else
const std::string GRAPHICS_EFFECT_EXT_LIB_PATH = "/system/lib/libgraphics_effect_ext.z.so";
#endif
const std::string GRAPHICS_EFFECT_EXT_INREFACE = "CreateGEXObjectByType";
#endif
}

GEExternalDynamicLoader::GEExternalDynamicLoader()
{
    LOGI("GEExternalDynamicLoader load");
#ifdef GE_PLATFORM_UNIX
    libHandle_ = dlopen(GRAPHICS_EFFECT_EXT_LIB_PATH.c_str(), RTLD_LAZY);
    if (!libHandle_) {
        LOGE("GEExternalDynamicLoader lib handle is null");
        return;
    }

    createObjectFunc_ = (CreateGEXObjectByTypeFunc)dlsym(libHandle_, GRAPHICS_EFFECT_EXT_INREFACE.c_str());
    if (!createObjectFunc_) {
        LOGE("GEExternalDynamicLoader CreateObjectFunc is null");
        return;
    }
    LOGI("GEExternalDynamicLoader load success");
#endif
}

GEExternalDynamicLoader::~GEExternalDynamicLoader()
{
    LOGI("GEExternalDynamicLoader unload");
#ifdef GE_PLATFORM_UNIX
    if (libHandle_) {
        dlclose(libHandle_);
    }
#endif
    libHandle_ = nullptr;
}

GEExternalDynamicLoader& GEExternalDynamicLoader::GetInstance()
{
    static GEExternalDynamicLoader instance;
    return instance;
}

void* GEExternalDynamicLoader::CreateGEXObjectByType(uint32_t type, uint32_t len, void* param)
{
    if (!createObjectFunc_) {
        LOGD("GEExternalDynamicLoader::CreateGEXObjectByType interface is null");
        return nullptr;
    }

#ifdef GE_OHOS
    auto enable = system::GetBoolParameter(GRAPHICS_EFFECT_EXT_ENABLE, true);
    if (enable) {
        return createObjectFunc_(type, len, param);
    }

    LOGW("GEExternalDynamicLoader::CreateGEXObjectByType dynamic load disabled");
    return nullptr;
#else
    return createObjectFunc_(type, len, param);
#endif
}

} // namespace Rosen
} // namespace OHOS

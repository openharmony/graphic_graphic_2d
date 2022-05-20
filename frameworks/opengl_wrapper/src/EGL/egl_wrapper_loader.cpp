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

#include "egl_wrapper_loader.h"

#include <dlfcn.h>
#include <string>

#include "egl_defs.h"
#include "../wrapper_log.h"

using namespace OHOS;
namespace OHOS {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "OpenGLWrapper" };
constexpr const char *VENDOR_LIB_PATH = "/system/lib/";
constexpr const char *LIB_EGL_NAME = "libEGL.so";
constexpr const char *LIB_GLESV1_NAME = "libGLESv1.so";
constexpr const char *LIB_GLESV2_NAME = "libGLESv2.so";
constexpr const char *LIB_GLESV3_NAME = "libGLESv3.so";
}

EglWrapperLoader& EglWrapperLoader::GetInstance()
{
    static EglWrapperLoader loader;
    return loader;
}

EglWrapperLoader::~EglWrapperLoader()
{
    WLOGD("");
}

using GetProcAddressType = FunctionPointerType (*)(const char *);
bool EglWrapperLoader::LoadEgl(const char *path, EglHookTable *table)
{
    WLOGD("");
    dlEglHandle_ = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (dlEglHandle_ == nullptr) {
        WLOGE("dlopen failed. error: %{public}s.", dlerror());
        return false;
    }

    GetProcAddressType getProcAddr =
        (GetProcAddressType)dlsym(dlEglHandle_, "eglGetProcAddress");
    if (getProcAddr == nullptr) {
        WLOGE("can't find eglGetProcAddress() in EGL driver library.");
        return false;
    }

    FunctionPointerType *current = (FunctionPointerType *)table;
    char const * const *api = gEglApiNames;
    while (*api) {
        char const *name = *api;
        FunctionPointerType func = (FunctionPointerType)dlsym(dlEglHandle_, name);
        if (func == nullptr) {
            WLOGD("try to getProcAddr %{public}s.", name);
            func = getProcAddr(name);
            if (func == nullptr) {
                WLOGD("couldn't find the entry-point: %{public}s.", name);
            }
        }

        *current++ = func;
        api++;
    }

    return true;
}

void *EglWrapperLoader::LoadGl(const char *path, char const * const *glName, FunctionPointerType *entry)
{
    WLOGD("");
    void *dlHandle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (dlHandle == nullptr) {
        WLOGE("dlopen failed. error: %{public}s.", dlerror());
        return nullptr;
    }

    FunctionPointerType *current = entry;
    char const * const *api = glName;
    while (*api) {
        char const *name = *api;
        FunctionPointerType func = (FunctionPointerType)dlsym(dlHandle, name);
        if (func == nullptr) {
            WLOGD("couldn't find the entry-point: %{public}s.", name);
        }
        *current++ = func;
        api++;
    }

    return dlHandle;
}

bool EglWrapperLoader::LoadVendorDriver(EglWrapperDispatchTable *table)
{
    WLOGD("EGL");
    std::string eglPath = std::string(VENDOR_LIB_PATH) + std::string(LIB_EGL_NAME);
    if (!LoadEgl(eglPath.c_str(), &table->egl)) {
        WLOGE("LoadEgl Failed.");
        return false;
    }

    WLOGD("GLESV1");
    std::string glPath = std::string(VENDOR_LIB_PATH) + std::string(LIB_GLESV1_NAME);
    dlGlHandle1_ = LoadGl(glPath.c_str(), gGlApiNames1, (FunctionPointerType *)&table->gl.table1);
    if (!dlEglHandle_) {
        WLOGE("LoadGl GLESV1 Failed.");
        return false;
    }

    WLOGD("GLESV2");
    glPath = std::string(VENDOR_LIB_PATH) + std::string(LIB_GLESV2_NAME);
    dlGlHandle2_ = LoadGl(glPath.c_str(), gGlApiNames2, (FunctionPointerType *)&table->gl.table2);
    if (!dlGlHandle2_) {
        WLOGE("LoadGl GLESV2 Failed.");
        return false;
    }

    WLOGD("GLESV3");
    glPath = std::string(VENDOR_LIB_PATH) + std::string(LIB_GLESV3_NAME);
    dlGlHandle3_ = LoadGl(glPath.c_str(), gGlApiNames3, (FunctionPointerType *)&table->gl.table3);
    if (!dlGlHandle3_) {
        WLOGE("LoadGl GLESV3 Failed.");
        return false;
    }

    return true;
}

bool EglWrapperLoader::Load(EglWrapperDispatchTable *table)
{
    WLOGD("");
    if (table == nullptr) {
        WLOGE("table is nullptr.");
        return false;
    }

    if (table->isLoad) {
        WLOGI("EglWrapperLoader has been already loaded.");
        return true;
    }

    if (!LoadVendorDriver(table)) {
        WLOGE("LoadVendorDriver Failed.");
        return false;
    }

    table->isLoad = true;
    return true;
}

bool EglWrapperLoader::Unload(EglWrapperDispatchTable *table)
{
    WLOGD("");
    if (table == nullptr) {
        WLOGE("table is nullptr.");
        return false;
    }
    if (!table->isLoad) {
        WLOGE("EglWrapperLoader is not loaded.");
        return false;
    }

    table->isLoad = false;

    if (dlEglHandle_) {
        dlclose(dlEglHandle_);
        dlEglHandle_ = nullptr;
    }
    if (dlGlHandle1_) {
        dlclose(dlGlHandle1_);
        dlGlHandle1_ = nullptr;
    }
    if (dlGlHandle2_) {
        dlclose(dlGlHandle2_);
        dlGlHandle2_ = nullptr;
    }
    if (dlGlHandle3_) {
        dlclose(dlGlHandle3_);
        dlGlHandle3_ = nullptr;
    }

    return true;
}

void *EglWrapperLoader::GetProcAddrFromDriver(const char *name)
{
    void *func = nullptr;

    if (dlEglHandle_) {
        func = dlsym(dlEglHandle_, name);
    }

    if (!func && dlGlHandle3_) {
        func = dlsym(dlGlHandle3_, name);
    }

    if (!func && dlGlHandle2_) {
        func = dlsym(dlGlHandle2_, name);
    }

    if (!func && dlGlHandle1_) {
        func = dlsym(dlGlHandle1_, name);
    }

    return func;
}
} // namespace OHOS

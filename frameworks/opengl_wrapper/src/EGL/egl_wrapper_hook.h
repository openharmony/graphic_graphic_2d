/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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


#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_HOOK_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_HOOK_H

#include <dlfcn.h>
#include <string>

#include "wrapper_log.h"
#include "egl_wrapper_layer.h"
#include "egl_core.h"

#if USE_IGRAPHICS_EXTENDS_HOOKS

namespace OHOS {

    // common layer loader
    using GetNextLayerAddr = void* (*)(void*, const char*);
    using LayerIsEnableFunc = bool (*)();

    struct LayerEntryFunc {
        LayerIsEnableFunc layerIsEnable = nullptr;
        LayerInitFunc layerInit = nullptr;
        LayerSetupFunc layerSetup = nullptr;
    };

    // GTX special
    using GTXInitHookTableFunc = void (*)(EglWrapperDispatchTable*,
        GlHookTable3*, GlHookTable3*, char const *const*);
    using GTXSetGlesVersionFunc = void (*)(int);
    using GTXGetSingleThreadStatusFunc = bool (*)();
    using EGLGetGlesVersionCallBackFunc = EGLint (*)(void);
    using EGLSwitchTableCallBackFunc = void (*)(bool);
    using EGLMakeCurrentAfterHookCallBackFunc = EGLBoolean (*)(EGLDisplay dpy,
        EGLSurface draw, EGLSurface read, EGLContext ctx);
    using GTXSetEGLCallBackFunc = void (*)(EGLGetGlesVersionCallBackFunc,
        EGLSwitchTableCallBackFunc, EGLMakeCurrentAfterHookCallBackFunc);
    using GTXCheckEglCompatibilityFunc = bool (*)(uint64_t*, uint32_t, uint64_t);

    struct GTXEntryFunc {
        GTXInitHookTableFunc gtxInitHookTable = nullptr;
        GTXSetGlesVersionFunc gtxSetGlesVersion = nullptr;
        GTXGetSingleThreadStatusFunc gtxGetSingleThreadStatus = nullptr;
        GTXSetEGLCallBackFunc gtxSetEGLCallBack = nullptr;
        GTXCheckEglCompatibilityFunc gtxCheckEglCompatibility = nullptr;
    };

class EglWrapperHook final {
public:
    LayerEntryFunc layerEntry;
    GTXEntryFunc gtx;

    static EglWrapperHook& GetInstance();
    bool Hook(EglWrapperDispatchTable* table);
    bool IsInit();

private:
    EglWrapperHook() : layerInited_(false) {}
    ~EglWrapperHook();

    bool LoadHookLayer(void);
    bool CheckCompatibility();
    bool InitHookLayer(EglWrapperDispatchTable* table);
    bool InitHookTable(EglWrapperDispatchTable* table);
    void LayerEntries(const LayerSetupFunc LayerSetup, EglWrapperFuncPointer *curr, char const * const * entries);
    template<typename Func = void*>
    bool LoadLayerSymbol(void* dlhandle, const char* name, Func &layerFunc)
    {
        layerFunc = reinterpret_cast<Func>(dlsym(dlhandle, name));
        if (layerFunc == nullptr) {
            WLOGE("Failed to dlsym %{public}s", name);
            dlclose(dlhandle_);
            dlhandle_ = nullptr;
            return false;
        }
        return true;
    }

    bool layerInited_;
    void *dlhandle_ = nullptr;
};

} // namespace OHOS

#endif

#endif
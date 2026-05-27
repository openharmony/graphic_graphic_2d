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

#include "egl_wrapper_hook.h"
#include "egl_wrapper_display.h"
#include "egl_wrapper_context.h"
#include "thread_private_data_ctl.h"

#if USE_IGRAPHICS_EXTENDS_HOOKS

namespace OHOS {
namespace {
#ifndef __aarch64__
    constexpr const char *HOOK_LAYERS_LIB_DIR = "/system/lib/";
#else
    constexpr const char *HOOK_LAYERS_LIB_DIR = "/system/lib64/";
#endif
}

static EglWrapperDisplay *ValidateDisplay(EGLDisplay dpy)
{
    EglWrapperDisplay *display = EglWrapperDisplay::GetWrapperDisplay(dpy);
    if (!display) {
        WLOGE("EGLDisplay is invalid.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_DISPLAY);
        return nullptr;
    }

    if (!display->IsReady()) {
        WLOGE("display is not ready.");
        ThreadPrivateDataCtl::SetError(EGL_NOT_INITIALIZED);
        return nullptr;
    }
    return display;
}

static EGLint EGLGetGlesVersion(void)
{
    EGLint version = -1;
    EGLContext ctx = ThreadPrivateDataCtl::GetContext();
    if (ctx) {
        EglWrapperContext *ctxPtr = EglWrapperContext::GetWrapperContext(ctx);
        version = ctxPtr ? ctxPtr->GetEglVersion() : -1;
    }
    return version;
}

static void EGLMakeSwitchHookTable(bool originMode)
{
    EGLContext ctx = ThreadPrivateDataCtl::GetContext();
    if (ctx) {
        EglWrapperContext *ctxPtr = EglWrapperContext::GetWrapperContext(ctx);
        if (originMode) {
            ThreadPrivateDataCtl::SetGlHookTable(&g_glHookSingle);
        } else {
            ThreadPrivateDataCtl::SetGlHookTable(&gWrapperHook.gl);
        }
        EglWrapperHook& hookLayer(EglWrapperHook::GetInstance());
        if (hookLayer.IsInit() && ctxPtr) {
            hookLayer.gtx.gtxSetGlesVersion(ctxPtr->GetEglVersion());
        }
    }
}

static EGLBoolean EGLMakeCurrentAfterHook(EGLDisplay dpy, EGLSurface draw,
    EGLSurface read, EGLContext ctx)
{
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->MakeCurrentAfterHook(draw, read, ctx);
}

EglWrapperHook& EglWrapperHook::GetInstance()
{
    static EglWrapperHook hookLayer;
    return hookLayer;
}

bool EglWrapperHook::Hook(EglWrapperDispatchTable *table)
{
    if (layerInited_) {
        return true;
    }

    if (!LoadHookLayer()) {
        return false;
    }

    if (!CheckCompatibility()) {
        return false;
    }

    if (!InitHookLayer(table)) {
        return false;
    }

    if (!InitHookTable(table)) {
        return false;
    }

    layerInited_ = true;
    return true;
}

bool EglWrapperHook::IsInit()
{
    return layerInited_;
}

bool EglWrapperHook::LoadHookLayer(void)
{
    std::string path = std::string(HOOK_LAYERS_LIB_DIR) + "libiGraphicsCore.z.so";
    dlhandle_ = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (dlhandle_ == nullptr) {
        WLOGE("Failed to dlopen %{public}s. error: %{public}s.", path.c_str(), dlerror());
        return false;
    }

    // load common layer entries
    if (!LoadLayerSymbol<LayerIsEnableFunc>(dlhandle_, "IsEnable", layerEntry.layerIsEnable)) {return false;}
    if (!LoadLayerSymbol<LayerInitFunc>(dlhandle_, "DebugLayerInitialize", layerEntry.layerInit)) {return false;}
    if (!LoadLayerSymbol<LayerSetupFunc>(dlhandle_, "DebugLayerGetProcAddr", layerEntry.layerSetup)) {return false;}

    // load gtx special api
    if (!LoadLayerSymbol<GTXInitHookTableFunc>(dlhandle_, "InitHooksTable", gtx.gtxInitHookTable)) {return false;}
    if (!LoadLayerSymbol<GTXSetGlesVersionFunc>(dlhandle_, "IGraphicsSetGlesVersion", gtx.gtxSetGlesVersion)) {
        return false;
    }
    if (!LoadLayerSymbol<GTXGetSingleThreadStatusFunc>(dlhandle_, "GetSingleThreadStatus",
        gtx.gtxGetSingleThreadStatus)) {
        return false;
    }
    if (!LoadLayerSymbol<GTXSetEGLCallBackFunc>(dlhandle_, "SetEGLCallback", gtx.gtxSetEGLCallBack)) {return false;}
    if (!LoadLayerSymbol<GTXCheckEglCompatibilityFunc>(dlhandle_, "CheckEglCompatibility",
        gtx.gtxCheckEglCompatibility)) {
        return false;
    }

    return true;
}

bool EglWrapperHook::CheckCompatibility()
{
    std::vector<uint64_t> eglHookTableSizes = {
        sizeof(WrapperHookTable),
        sizeof(EglHookTable),
        sizeof(GlHookTable1),
        sizeof(GlHookTable2),
        sizeof(GlHookTable3),
    };
    return gtx.gtxCheckEglCompatibility(eglHookTableSizes.data(), eglHookTableSizes.size(), ENTRIES_FILES_VERSION);
}

bool EglWrapperHook::InitHookLayer(EglWrapperDispatchTable *table)
{
    if (!layerEntry.layerIsEnable()) {
        return false;
    }

    layerEntry.layerInit(nullptr, nullptr);

    char const * const *entries;
    EglWrapperFuncPointer *curr;

    entries = gWrapperApiNames;
    curr = reinterpret_cast<EglWrapperFuncPointer *>(&table->wrapper);
    LayerEntries(layerEntry.layerSetup, curr, entries);

    entries = gGlApiNames3;
    curr = reinterpret_cast<EglWrapperFuncPointer *>(&table->gl.table3);
    LayerEntries(layerEntry.layerSetup, curr, entries);

    return true;
}

void EglWrapperHook::LayerEntries(const LayerSetupFunc LayerSetup,
    EglWrapperFuncPointer *curr, char const * const *entries)
{
    while (*entries) {
        char const *name = *entries;
        EglWrapperFuncPointer prev = *curr;
        *curr = LayerSetup(name, *curr);
        if (prev != *curr) {
            WLOGD("EglWrapperHook replace %{public}s org: %{public}p hook: %{public}p", name, prev, *curr);
        }
        curr++;
        entries++;
    }
}

bool EglWrapperHook::InitHookTable(EglWrapperDispatchTable *table)
{
    gtx.gtxInitHookTable(table, &g_glHookCSDR.table3, &g_glHookSingle.table3, gGlApiNames3);
    gtx.gtxSetEGLCallBack(EGLGetGlesVersion, EGLMakeSwitchHookTable, EGLMakeCurrentAfterHook);

    return true;
}

EglWrapperHook::~EglWrapperHook()
{
    if (dlhandle_) {
        dlclose(dlhandle_);
        dlhandle_ = nullptr;
    }
}
}

#endif
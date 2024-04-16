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
#include "egl_wrapper_entry.h"

#include <map>
#include <string>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <dlfcn.h>

#include "egl_defs.h"
#include "egl_wrapper_context.h"
#include "egl_wrapper_display.h"
#include "egl_wrapper_loader.h"
#include "thread_private_data_ctl.h"
#include "wrapper_log.h"
#include "egl_blob_cache.h"
#if USE_APS_IGAMESERVICE_FUNC
#include "egl_slice_report.h"
#include "aps_game_fps_controller.h"
#endif //USE_APS_IGAMESERVICE_FUNC
using namespace OHOS;
namespace OHOS {
static inline void ClearError()
{
    ThreadPrivateDataCtl::ClearError();
}

static EglWrapperDisplay *ValidateDisplay(EGLDisplay dpy)
{
    EglWrapperDisplay *display = EglWrapperDisplay::GetWrapperDisplay(dpy);
    if (!display) {
        WLOGE("EGLDislay is invalid.");
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

EGLBoolean EglChooseConfigImpl(EGLDisplay dpy, const EGLint *attribList,
    EGLConfig *configs, EGLint configSize, EGLint *numConfig)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (numConfig == nullptr) {
        WLOGE("EGLint *numConfig is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    *numConfig = 0;

    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglChooseConfig) {
        ret = table->egl.eglChooseConfig(display->GetEglDisplay(),
            attribList, configs, configSize, numConfig);
    } else {
        WLOGE("eglChooseConfig is invalid.");
    }

    return ret;
}

EGLBoolean EglCopyBuffersImpl(EGLDisplay dpy, EGLSurface surf, NativePixmapType target)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->CopyBuffers(surf, target);
}

EGLContext EglCreateContextImpl(EGLDisplay dpy, EGLConfig config,
    EGLContext shareList, const EGLint *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_CONTEXT;
    }

    return display->CreateEglContext(config, shareList, attribList);
}

EGLSurface EglCreatePbufferSurfaceImpl(EGLDisplay dpy, EGLConfig config,
    const EGLint* attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_CONTEXT;
    }

    return display->CreatePbufferSurface(config, attribList);
}

EGLSurface EglCreatePixmapSurfaceImpl(EGLDisplay dpy, EGLConfig config,
    EGLNativePixmapType pixmap, const EGLint* attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_CONTEXT;
    }

    return display->CreatePixmapSurface(config, pixmap, attribList);
}

EGLSurface EglCreateWindowSurfaceImpl(EGLDisplay dpy,
    EGLConfig config, NativeWindowType window, const EGLint* attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_SURFACE;
    }

    return display->CreateEglSurface(config, window, attribList);
}

EGLBoolean EglDestroyContextImpl(EGLDisplay dpy, EGLContext ctx)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->DestroyEglContext(ctx);
}

EGLBoolean EglDestroySurfaceImpl(EGLDisplay dpy, EGLSurface surf)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->DestroyEglSurface(surf);
}

EGLBoolean EglGetConfigAttribImpl(EGLDisplay dpy, EGLConfig config,
    EGLint attribute, EGLint *value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglGetConfigAttrib) {
        ret = table->egl.eglGetConfigAttrib(display->GetEglDisplay(), config, attribute, value);
    } else {
        WLOGE("eglGetConfigAttrib is not found.");
    }
    return ret;
}

EGLBoolean EglGetConfigsImpl(EGLDisplay dpy, EGLConfig *configs,
    EGLint configSize, EGLint *numConfig)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (numConfig == nullptr) {
        WLOGE("EGLint *numConfig is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglGetConfigs) {
        ret = table->egl.eglGetConfigs(display->GetEglDisplay(), configs, configSize, numConfig);
    } else {
        WLOGE("eglGetConfigs is not found.");
    }

    return ret;
}

EGLDisplay EglGetCurrentDisplayImpl(void)
{
    ClearError();
    EGLContext ctx = ThreadPrivateDataCtl::GetContext();
    if (ctx) {
        EglWrapperContext *ctxPtr = EglWrapperContext::GetWrapperContext(ctx);
        if (ctxPtr == nullptr) {
            WLOGE("current is bad context.");
            ThreadPrivateDataCtl::SetError(EGL_BAD_CONTEXT);
            return EGL_NO_DISPLAY;
        }
        return ctxPtr->GetDisplay();
    }
    return EGL_NO_DISPLAY;
}

EGLSurface EglGetCurrentSurfaceImpl(EGLint type)
{
    ClearError();
    EGLContext ctx = ThreadPrivateDataCtl::GetContext();
    if (ctx) {
        EglWrapperContext *ctxPtr = EglWrapperContext::GetWrapperContext(ctx);
        if (ctxPtr == nullptr) {
            WLOGE("current is bad context.");
            ThreadPrivateDataCtl::SetError(EGL_BAD_CONTEXT);
            return EGL_NO_SURFACE;
        }
        return ctxPtr->GetCurrentSurface(type);
    }
    return EGL_NO_SURFACE;
}

static EGLDisplay EglGetPlatformDisplayInternal(EGLenum platform,
    EGLNativeDisplayType type, const EGLAttrib *attribList)
{
    WLOGD("");
    if (type != EGL_DEFAULT_DISPLAY) {
        WLOGE("EGLNativeDisplayType is invalid.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_NO_DISPLAY;
    }

    if (platform != EGL_PLATFORM_OHOS_KHR) {
        WLOGE("EGLenum platform is not EGL_PLATFORM_OHOS_KHR.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_NO_DISPLAY;
    }

    return EglWrapperDisplay::GetEglDisplay(platform, type, attribList);
}

EGLDisplay EglGetDisplayImpl(EGLNativeDisplayType type)
{
    ClearError();
    WLOGD("");
    return EglGetPlatformDisplayInternal(EGL_PLATFORM_OHOS_KHR, type, nullptr);
}

EGLint EglGetErrorImpl(void)
{
    EGLint ret = EGL_SUCCESS;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglGetError) {
        ret = table->egl.eglGetError();
    } else {
        WLOGE("eglGetError is not found.");
    }

    if (ret == EGL_SUCCESS) {
        ret = ThreadPrivateDataCtl::GetError();
    }

    return ret;
}

static __eglMustCastToProperFunctionPointerType findBuiltinWrapper(const char* procname)
{
#if (defined(__aarch64__) || defined(__x86_64__))
    static void* dlglv3Handle = dlopen("/system/lib64/libGLESv3.so", RTLD_NOW | RTLD_LOCAL);
#else
    static void* dlglv3Handle = dlopen("/system/lib/platformsdk/libGLESv3.so", RTLD_NOW | RTLD_LOCAL);
#endif
    void* proc = dlsym(dlglv3Handle, procname);
    if (proc) {
        return (__eglMustCastToProperFunctionPointerType)proc;
    }
    return nullptr;
}

__eglMustCastToProperFunctionPointerType EglGetProcAddressImpl(const char *procname)
{
    ClearError();
    WLOGD("");
    if (gExtensionMap.find(procname) != gExtensionMap.end()) {
        return gExtensionMap.at(procname);
    }

    __eglMustCastToProperFunctionPointerType addr = findBuiltinWrapper(procname);
    if (addr) {
        return __eglMustCastToProperFunctionPointerType(addr);
    }

    EglWrapperLoader& loader(EglWrapperLoader::GetInstance());
    void *func = loader.GetProcAddrFromDriver(procname);

    if (!func) {
        EglWrapperDispatchTablePtr table = &gWrapperHook;
        if (table->isLoad && table->egl.eglGetProcAddress) {
            func = reinterpret_cast<void *>(table->egl.eglGetProcAddress(procname));
        }
    }

    if (func) {
        return __eglMustCastToProperFunctionPointerType(func);
    }

    WLOGW("FindEglExtApi did not find an entry for %{public}s", procname);
    return nullptr;
}

EGLBoolean EglInitializeImpl(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
    WLOGD("");
#if USE_APS_IGAMESERVICE_FUNC
    OHOS::GameService::EglSliceReport::GetInstance().InitSliceReport();
#endid
    EglWrapperDisplay *display = EglWrapperDisplay::GetWrapperDisplay(dpy);
    if (!display) {
        WLOGE("EGLDislay is invalid.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }

    return display->Init(major, minor);
}

EGLBoolean EglMakeCurrentImpl(EGLDisplay dpy, EGLSurface draw,
    EGLSurface read, EGLContext ctx)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->MakeCurrent(draw, read, ctx);
}

EGLBoolean EglQueryContextImpl(EGLDisplay dpy, EGLContext ctx,
    EGLint attribute, EGLint *value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (value == nullptr) {
        WLOGE("EGLint *value is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    return display->QueryContext(ctx, attribute, value);
}

const char *EglQueryStringImpl(EGLDisplay dpy, EGLint name)
{
    ClearError();
    WLOGD("");
    EGLDisplay actualDpy = EGL_NO_DISPLAY;
    if (dpy != EGL_NO_DISPLAY) {
        EglWrapperDisplay *display = ValidateDisplay(dpy);
        if (!display) {
            return nullptr;
        }
        actualDpy = display->GetEglDisplay();
    }

    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglQueryString) {
        return table->egl.eglQueryString(actualDpy, name);
    } else {
        WLOGE("eglQueryString is not found.");
    }

    return nullptr;
}

EGLBoolean EglQuerySurfaceImpl(EGLDisplay dpy, EGLSurface surf,
    EGLint attribute, EGLint* value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (value == nullptr) {
        WLOGE("EGLint *value is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    return display->QuerySurface(surf, attribute, value);
}

EGLBoolean EglSwapBuffersImpl(EGLDisplay dpy, EGLSurface surf)
{
    ClearError();
    WLOGD("");
#if USE_APS_IGAMESERVICE_FUNC
    OHOS::GameService::EglSliceReport::GetInstance().AddGraphicCount();
    OHOS::Rosen::ApsGameFpsController::GetInstance().PowerCtrllofEglswapbuffer();
#endif
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->SwapBuffers(surf);
}

EGLBoolean EglTerminateImpl(EGLDisplay dpy)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->Terminate();
}

EGLBoolean EglWaitGLImpl(void)
{
    ClearError();
    WLOGD("");
    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglWaitGL) {
        ret = table->egl.eglWaitGL();
    } else {
        WLOGE("eglWaitGL is not found.");
    }
    return ret;
}

EGLBoolean EglWaitNativeImpl(EGLint engine)
{
    ClearError();
    WLOGD("");
    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglWaitNative) {
        ret = table->egl.eglWaitNative(engine);
    } else {
        WLOGE("eglWaitNative is not found.");
    }
    return ret;
}

EGLBoolean EglBindTexImageImpl(EGLDisplay dpy, EGLSurface surf, EGLint buffer)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->BindTexImage(surf, buffer);
}

EGLBoolean EglReleaseTexImageImpl(EGLDisplay dpy, EGLSurface surf, EGLint buffer)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->ReleaseTexImage(surf, buffer);
}

EGLBoolean EglSurfaceAttribImpl(EGLDisplay dpy, EGLSurface surf,
    EGLint attribute, EGLint value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->SurfaceAttrib(surf, attribute, value);
}

EGLBoolean EglSwapIntervalImpl(EGLDisplay dpy, EGLint interval)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglSwapInterval) {
        ret = table->egl.eglSwapInterval(display->GetEglDisplay(), interval);
    } else {
        WLOGE("eglQueryString is not found.");
    }

    return ret;
}

EGLBoolean EglBindAPIImpl(EGLenum api)
{
    ClearError();
    WLOGD("");
    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglBindAPI) {
        ret = table->egl.eglBindAPI(api);
    } else {
        WLOGE("eglBindAPI is not found.");
    }
    return ret;
}

EGLBoolean EglQueryAPIImpl(void)
{
    ClearError();
    WLOGD("");
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglQueryAPI) {
        return table->egl.eglQueryAPI();
    } else {
        WLOGE("eglQueryAPI is not found.");
    }

    return EGL_OPENGL_ES_API;
}

EGLSurface EglCreatePbufferFromClientBufferImpl(EGLDisplay dpy,
    EGLenum buftype, EGLClientBuffer buffer,
    EGLConfig config, const EGLint *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_SURFACE;
    }

    return display->CreatePbufferFromClientBuffer(buftype, buffer, config, attribList);
}

EGLBoolean EglReleaseThreadImpl(void)
{
    ClearError();
    WLOGD("");
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglReleaseThread) {
        table->egl.eglReleaseThread();
    } else {
        WLOGE("eglReleaseThread is not found.");
    }

    ThreadPrivateDataCtl::ClearPrivateData();
    return EGL_TRUE;
}

EGLBoolean EglWaitClientImpl(void)
{
    ClearError();
    WLOGD("");
    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglWaitClient) {
        ret = table->egl.eglWaitClient();
    } else {
        WLOGE("eglWaitClient is not found.");
    }
    return ret;
}

EGLContext EglGetCurrentContextImpl(void)
{
    ClearError();
    EGLContext ctx = ThreadPrivateDataCtl::GetContext();
    return ctx;
}

EGLSync EglCreateSyncImpl(EGLDisplay dpy, EGLenum type, const EGLAttrib *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_SYNC;
    }

    EGLSyncKHR ret = EGL_NO_SYNC;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglCreateSync) {
        ret = table->egl.eglCreateSync(display->GetEglDisplay(), type, attribList);
    } else {
        WLOGE("eglCreateSync is not found.");
    }

    return ret;
}

EGLBoolean EglDestroySyncImpl(EGLDisplay dpy, EGLSync sync)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglDestroySync) {
        ret = table->egl.eglDestroySync(display->GetEglDisplay(), sync);
    } else {
        WLOGE("eglDestroySync is not found.");
    }

    return ret;
}

EGLint EglClientWaitSyncImpl(EGLDisplay dpy, EGLSync sync,
    EGLint flags, EGLTimeKHR timeout)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglClientWaitSync) {
        ret = table->egl.eglClientWaitSync(display->GetEglDisplay(),
            sync, flags, timeout);
    } else {
        WLOGE("eglClientWaitSync is not found.");
    }

    return ret;
}

EGLBoolean EglGetSyncAttribImpl(EGLDisplay dpy, EGLSync sync,
    EGLint attribute, EGLAttrib *value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (value == nullptr) {
        WLOGE("EGLAttrib *value is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglGetSyncAttrib) {
        ret = table->egl.eglGetSyncAttrib(display->GetEglDisplay(),
            sync, attribute, value);
    } else {
        WLOGE("eglGetSyncAttrib is not found.");
    }

    return ret;
}

EGLImage EglCreateImageImpl(EGLDisplay dpy, EGLContext ctx,
    EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_IMAGE;
    }

    return display->CreateImage(ctx, target, buffer, attribList);
}

EGLBoolean EglDestroyImageImpl(EGLDisplay dpy, EGLImage img)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->DestroyImage(img);
}

EGLDisplay EglGetPlatformDisplayImpl(EGLenum platform,
    void *nativeDisplay, const EGLAttrib *attribList)
{
    ClearError();
    return EglGetPlatformDisplayInternal(platform,
        static_cast<EGLNativeDisplayType>(nativeDisplay), attribList);
}

EGLSurface EglCreatePlatformWindowSurfaceImpl(EGLDisplay dpy,
    EGLConfig config, void *nativeWindow, const EGLAttrib *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_SURFACE;
    }

    return display->CreatePlatformWindowSurface(config, nativeWindow, attribList);
}


EGLSurface EglCreatePlatformPixmapSurfaceImpl(EGLDisplay dpy,
    EGLConfig config, void *nativePixmap, const EGLAttrib *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_SURFACE;
    }

    return display->CreatePlatformPixmapSurface(config, nativePixmap, attribList);
}

EGLBoolean EglWaitSyncImpl(EGLDisplay dpy, EGLSync sync, EGLint flags)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglWaitSync) {
        ret = table->egl.eglWaitSync(display->GetEglDisplay(), sync, flags);
    } else {
        WLOGE("eglWaitSync is not found.");
    }

    return ret;
}

EGLBoolean EglLockSurfaceKHRImpl(EGLDisplay dpy, EGLSurface surf,
    const EGLint *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->LockSurfaceKHR(surf, attribList);
}

EGLBoolean EglUnlockSurfaceKHRImpl(EGLDisplay dpy, EGLSurface surf)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->UnlockSurfaceKHR(surf);
}

EGLImageKHR EglCreateImageKHRImpl(EGLDisplay dpy, EGLContext ctx,
    EGLenum target, EGLClientBuffer buffer, const EGLint *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_IMAGE_KHR;
    }

    return display->CreateImageKHR(ctx, target, buffer, attribList);
}

EGLBoolean EglDestroyImageKHRImpl(EGLDisplay dpy, EGLImageKHR img)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->DestroyImageKHR(img);
}

EGLSyncKHR EglCreateSyncKHRImpl(EGLDisplay dpy, EGLenum type, const EGLint* attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_SYNC_KHR;
    }

    EGLSyncKHR ret = EGL_NO_SYNC_KHR;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglCreateSyncKHR) {
        ret = table->egl.eglCreateSyncKHR(display->GetEglDisplay(), type, attribList);
    } else {
        WLOGE("eglCreateSyncKHR is not found.");
    }

    return ret;
}

EGLBoolean EglDestroySyncKHRImpl(EGLDisplay dpy, EGLSyncKHR sync)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglDestroySyncKHR) {
        ret = table->egl.eglDestroySyncKHR(display->GetEglDisplay(), sync);
    } else {
        WLOGE("eglDestroySyncKHR is not found.");
    }

    return ret;
}

EGLint EglClientWaitSyncKHRImpl(EGLDisplay dpy, EGLSyncKHR sync,
    EGLint flags, EGLTimeKHR timeout)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglClientWaitSyncKHR) {
        ret = table->egl.eglClientWaitSyncKHR(display->GetEglDisplay(),
            sync, flags, timeout);
    } else {
        WLOGE("eglClientWaitSyncKHR is not found.");
    }

    return ret;
}

EGLBoolean EglGetSyncAttribKHRImpl(EGLDisplay dpy, EGLSyncKHR sync,
    EGLint attribute, EGLint *value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (value == nullptr) {
        WLOGE("EGLint *value is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglGetSyncAttribKHR) {
        ret = table->egl.eglGetSyncAttribKHR(display->GetEglDisplay(),
            sync, attribute, value);
    } else {
        WLOGE("eglGetSyncAttribKHR is not found.");
    }

    return ret;
}

EGLBoolean EglSignalSyncKHRImpl(EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglSignalSyncKHR) {
        ret = table->egl.eglSignalSyncKHR(display->GetEglDisplay(), sync, mode);
    } else {
        WLOGE("eglSignalSyncKHR is not found.");
    }

    return ret;
}

EGLStreamKHR EglCreateStreamKHRImpl(EGLDisplay dpy, const EGLint *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_STREAM_KHR;
    }

    EGLStreamKHR ret = EGL_NO_STREAM_KHR;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglCreateStreamKHR) {
        ret = table->egl.eglCreateStreamKHR(display->GetEglDisplay(), attribList);
    } else {
        WLOGE("eglCreateStreamKHR is not found.");
    }

    return ret;
}

EGLBoolean EglDestroyStreamKHRImpl(EGLDisplay dpy, EGLStreamKHR stream)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglDestroyStreamKHR) {
        ret = table->egl.eglDestroyStreamKHR(display->GetEglDisplay(), stream);
    } else {
        WLOGE("eglDestroyStreamKHR is not found.");
    }

    return ret;
}

EGLBoolean EglStreamAttribKHRImpl(EGLDisplay dpy, EGLStreamKHR stream,
    EGLenum attribute, EGLint value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglStreamAttribKHR) {
        ret = table->egl.eglStreamAttribKHR(display->GetEglDisplay(),
            stream, attribute, value);
    } else {
        WLOGE("eglStreamAttribKHR is not found.");
    }

    return ret;
}

EGLBoolean EglQueryStreamKHRImpl(EGLDisplay dpy, EGLStreamKHR stream,
    EGLenum attribute, EGLint *value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (value == nullptr) {
        WLOGE("EGLint *value is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglQueryStreamKHR) {
        ret = table->egl.eglQueryStreamKHR(display->GetEglDisplay(),
            stream, attribute, value);
    } else {
        WLOGE("eglQueryStreamKHR is not found.");
    }

    return ret;
}

EGLBoolean EglQueryStreamu64KHRImpl(EGLDisplay dpy, EGLStreamKHR stream,
    EGLenum attribute, EGLuint64KHR *value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (value == nullptr) {
        WLOGE("EGLint *value is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglQueryStreamu64KHR) {
        ret = table->egl.eglQueryStreamu64KHR(display->GetEglDisplay(),
            stream, attribute, value);
    } else {
        WLOGE("eglQueryStreamu64KHR is not found.");
    }

    return ret;
}

EGLBoolean EglStreamConsumerGLTextureExternalKHRImpl(EGLDisplay dpy,
    EGLStreamKHR stream)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglStreamConsumerGLTextureExternalKHR) {
        ret = table->egl.eglStreamConsumerGLTextureExternalKHR(
            display->GetEglDisplay(), stream);
    } else {
        WLOGE("eglStreamConsumerGLTextureExternalKHR is not found.");
    }

    return ret;
}

EGLBoolean EglStreamConsumerAcquireKHRImpl(EGLDisplay dpy, EGLStreamKHR stream)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglStreamConsumerAcquireKHR) {
        ret = table->egl.eglStreamConsumerAcquireKHR(
            display->GetEglDisplay(), stream);
    } else {
        WLOGE("eglStreamConsumerAcquireKHR is not found.");
    }

    return ret;
}

EGLBoolean EglStreamConsumerReleaseKHRImpl(EGLDisplay dpy, EGLStreamKHR stream)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglStreamConsumerReleaseKHR) {
        ret = table->egl.eglStreamConsumerReleaseKHR(
            display->GetEglDisplay(), stream);
    } else {
        WLOGE("eglStreamConsumerReleaseKHR is not found.");
    }

    return ret;
}

EGLSurface EglCreateStreamProducerSurfaceKHRImpl(EGLDisplay dpy, EGLConfig config,
    EGLStreamKHR stream, const EGLint *attribList)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_SURFACE;
    }

    return display->CreateStreamProducerSurfaceKHR(config, stream, attribList);
}

EGLBoolean EglQueryStreamTimeKHRImpl(EGLDisplay dpy, EGLStreamKHR stream,
    EGLenum attribute, EGLTimeKHR *value)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    if (value == nullptr) {
        WLOGE("EGLint *value is nullptr.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglQueryStreamTimeKHR) {
        ret = table->egl.eglQueryStreamTimeKHR(display->GetEglDisplay(),
            stream, attribute, value);
    } else {
        WLOGE("eglQueryStreamTimeKHR is not found.");
    }

    return ret;
}

EGLNativeFileDescriptorKHR EglGetStreamFileDescriptorKHRImpl(
    EGLDisplay dpy, EGLStreamKHR stream)
{
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_FILE_DESCRIPTOR_KHR;
    }

    EGLNativeFileDescriptorKHR ret = EGL_NO_FILE_DESCRIPTOR_KHR;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglGetStreamFileDescriptorKHR) {
        ret = table->egl.eglGetStreamFileDescriptorKHR(
            display->GetEglDisplay(), stream);
    } else {
        WLOGE("eglGetStreamFileDescriptorKHR is not found.");
    }

    return ret;
}

EGLStreamKHR EglCreateStreamFromFileDescriptorKHRImpl(
    EGLDisplay dpy, EGLNativeFileDescriptorKHR fd)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_NO_STREAM_KHR;
    }

    EGLStreamKHR ret = EGL_NO_STREAM_KHR;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglCreateStreamFromFileDescriptorKHR) {
        ret = table->egl.eglCreateStreamFromFileDescriptorKHR(
            display->GetEglDisplay(), fd);
    } else {
        WLOGE("eglCreateStreamFromFileDescriptorKHR is not found.");
    }

    return ret;
}

EGLBoolean EglWaitSyncKHRImpl(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    EGLBoolean ret = EGL_FALSE;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglWaitSyncKHR) {
        ret = table->egl.eglWaitSyncKHR(display->GetEglDisplay(), sync, flags);
    } else {
        WLOGE("eglWaitSyncKHR is not found.");
    }

    return ret;
}

EGLDisplay EglGetPlatformDisplayEXTImpl(EGLenum platform,
    void *nativeDisplay, const EGLint *attribList)
{
    WLOGD("");
    if (nativeDisplay != EGL_DEFAULT_DISPLAY) {
        WLOGE("nativeDisplay is invalid.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_NO_DISPLAY;
    }

    if (platform != EGL_PLATFORM_OHOS_KHR) {
        WLOGE("EGLenum platform is not EGL_PLATFORM_OHOS_KHR.");
        ThreadPrivateDataCtl::SetError(EGL_BAD_PARAMETER);
        return EGL_NO_DISPLAY;
    }

    return EglWrapperDisplay::GetEglDisplayExt(platform, nativeDisplay, attribList);
}

EGLBoolean EglSwapBuffersWithDamageKHRImpl(EGLDisplay dpy, EGLSurface draw,
    EGLint *rects, EGLint nRects)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->SwapBuffersWithDamageKHR(draw, rects, nRects);
}

EGLBoolean EglSetDamageRegionKHRImpl(EGLDisplay dpy, EGLSurface surf,
    EGLint *rects, EGLint nRects)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }

    return display->SetDamageRegionKHR(surf, rects, nRects);
}

EGLint EglDupNativeFenceFDANDROIDImpl(EGLDisplay dpy, EGLSyncKHR sync)
{
    ClearError();
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return EGL_FALSE;
    }
    EGLint ret = -1;
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglDupNativeFenceFDANDROID) {
        ret = table->egl.eglDupNativeFenceFDANDROID(display->GetEglDisplay(), sync);
    } else {
        WLOGE("EglDupNativeFenceFDANDROID platform is not found.");
    }
    return ret;
}

void EglSetBlobCacheFuncsANDROIDImpl(EGLDisplay dpy, EGLSetBlobFuncANDROID set, EGLGetBlobFuncANDROID get)
{
    WLOGD("");
    EglWrapperDisplay *display = ValidateDisplay(dpy);
    if (!display) {
        return;
    }

    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglSetBlobCacheFuncsANDROID) {
        table->egl.eglSetBlobCacheFuncsANDROID(display->GetEglDisplay(),
                                               BlobCache::setBlobFunc, BlobCache::getBlobFunc);
    } else {
        WLOGE("EglSetBlobCacheFuncsANDROIDImpl platform is not found.");
    }
}

static const std::map<std::string, EglWrapperFuncPointer> gEglWrapperMap = {
    /* EGL_VERSION_1_0 */
    { "eglChooseConfig", (EglWrapperFuncPointer)&EglChooseConfigImpl },
    { "eglCopyBuffers", (EglWrapperFuncPointer)&EglCopyBuffersImpl },
    { "eglCreateContext", (EglWrapperFuncPointer)&EglCreateContextImpl },
    { "eglCreatePbufferSurface", (EglWrapperFuncPointer)&EglCreatePbufferSurfaceImpl },
    { "eglCreatePixmapSurface", (EglWrapperFuncPointer)&EglCreatePixmapSurfaceImpl },
    { "eglCreateWindowSurface", (EglWrapperFuncPointer)&EglCreateWindowSurfaceImpl },
    { "eglDestroyContext", (EglWrapperFuncPointer)&EglDestroyContextImpl },
    { "eglDestroySurface", (EglWrapperFuncPointer)&EglDestroySurfaceImpl },
    { "eglGetConfigAttrib", (EglWrapperFuncPointer)&EglGetConfigAttribImpl },
    { "eglGetConfigs", (EglWrapperFuncPointer)&EglGetConfigsImpl },
    { "eglGetCurrentDisplay", (EglWrapperFuncPointer)&EglGetCurrentDisplayImpl },
    { "eglGetCurrentSurface", (EglWrapperFuncPointer)&EglGetCurrentSurfaceImpl },
    { "eglGetDisplay", (EglWrapperFuncPointer)&EglGetDisplayImpl },
    { "eglGetError", (EglWrapperFuncPointer)&EglGetErrorImpl },
    { "eglGetProcAddress", (EglWrapperFuncPointer)&EglGetProcAddressImpl },
    { "eglInitialize", (EglWrapperFuncPointer)&EglInitializeImpl },
    { "eglMakeCurrent", (EglWrapperFuncPointer)&EglMakeCurrentImpl },
    { "eglQueryContext", (EglWrapperFuncPointer)&EglQueryContextImpl },
    { "eglQueryString", (EglWrapperFuncPointer)&EglQueryStringImpl },
    { "eglQuerySurface", (EglWrapperFuncPointer)&EglQuerySurfaceImpl },
    { "eglSwapBuffers", (EglWrapperFuncPointer)&EglSwapBuffersImpl },
    { "eglTerminate", (EglWrapperFuncPointer)&EglTerminateImpl },
    { "eglWaitGL", (EglWrapperFuncPointer)&EglWaitGLImpl },
    { "eglWaitNative", (EglWrapperFuncPointer)&EglWaitNativeImpl },

    /* EGL_VERSION_1_1 */
    { "eglBindTexImage", (EglWrapperFuncPointer)&EglBindTexImageImpl },
    { "eglReleaseTexImage", (EglWrapperFuncPointer)&EglReleaseTexImageImpl },
    { "eglSurfaceAttrib", (EglWrapperFuncPointer)&EglSurfaceAttribImpl },
    { "eglSwapInterval", (EglWrapperFuncPointer)&EglSwapIntervalImpl },

    /* EGL_VERSION_1_2 */
    { "eglBindAPI", (EglWrapperFuncPointer)&EglBindAPIImpl },
    { "eglQueryAPI", (EglWrapperFuncPointer)&EglQueryAPIImpl },
    { "eglCreatePbufferFromClientBuffer", (EglWrapperFuncPointer)&EglCreatePbufferFromClientBufferImpl },
    { "eglReleaseThread", (EglWrapperFuncPointer)&EglReleaseThreadImpl },
    { "eglWaitClient", (EglWrapperFuncPointer)&EglWaitClientImpl },

    /* EGL_VERSION_1_3 */
    /* EGL_VERSION_1_4 */
    { "eglGetCurrentContext", (EglWrapperFuncPointer)&EglGetCurrentContextImpl },

    /* EGL_VERSION_1_5 */
    { "eglCreateSync", (EglWrapperFuncPointer)&EglCreateSyncImpl },
    { "eglDestroySync", (EglWrapperFuncPointer)&EglDestroySyncImpl },
    { "eglClientWaitSync", (EglWrapperFuncPointer)&EglClientWaitSyncImpl },
    { "eglGetSyncAttrib", (EglWrapperFuncPointer)&EglGetSyncAttribImpl },
    { "eglCreateImage", (EglWrapperFuncPointer)&EglCreateImageImpl },
    { "eglDestroyImage", (EglWrapperFuncPointer)&EglDestroyImageImpl },
    { "eglGetPlatformDisplay", (EglWrapperFuncPointer)&EglGetPlatformDisplayImpl },
    { "eglCreatePlatformWindowSurface", (EglWrapperFuncPointer)&EglCreatePlatformWindowSurfaceImpl },
    { "eglCreatePlatformPixmapSurface", (EglWrapperFuncPointer)&EglCreatePlatformPixmapSurfaceImpl },
    { "eglWaitSync", (EglWrapperFuncPointer)&EglWaitSyncImpl },

    /* EGL_EXTENTIONS */
    { "eglLockSurfaceKHR", (EglWrapperFuncPointer)&EglLockSurfaceKHRImpl },
    { "eglUnlockSurfaceKHR", (EglWrapperFuncPointer)&EglUnlockSurfaceKHRImpl },
    { "eglDupNativeFenceFDANDROID", (EglWrapperFuncPointer)&EglDupNativeFenceFDANDROIDImpl },

    { "eglCreateImageKHR", (EglWrapperFuncPointer)&EglCreateImageKHRImpl },
    { "eglDestroyImageKHR", (EglWrapperFuncPointer)&EglDestroyImageKHRImpl },

    { "eglCreateSyncKHR", (EglWrapperFuncPointer)&EglCreateSyncKHRImpl },
    { "eglDestroySyncKHR", (EglWrapperFuncPointer)&EglDestroySyncKHRImpl },
    { "eglClientWaitSyncKHR", (EglWrapperFuncPointer)&EglClientWaitSyncKHRImpl },
    { "eglGetSyncAttribKHR", (EglWrapperFuncPointer)&EglGetSyncAttribKHRImpl },

    { "eglSignalSyncKHR", (EglWrapperFuncPointer)&EglSignalSyncKHRImpl },

    { "eglCreateStreamKHR", (EglWrapperFuncPointer)&EglCreateStreamKHRImpl },
    { "eglDestroyStreamKHR", (EglWrapperFuncPointer)&EglDestroyStreamKHRImpl },
    { "eglStreamAttribKHR", (EglWrapperFuncPointer)&EglStreamAttribKHRImpl },
    { "eglQueryStreamKHR", (EglWrapperFuncPointer)&EglQueryStreamKHRImpl },
    { "eglQueryStreamu64KHR", (EglWrapperFuncPointer)&EglQueryStreamu64KHRImpl },

    { "eglStreamConsumerGLTextureExternalKHR", (EglWrapperFuncPointer)&EglStreamConsumerGLTextureExternalKHRImpl },
    { "eglStreamConsumerAcquireKHR", (EglWrapperFuncPointer)&EglStreamConsumerAcquireKHRImpl },
    { "eglStreamConsumerReleaseKHR", (EglWrapperFuncPointer)&EglStreamConsumerReleaseKHRImpl },

    { "eglCreateStreamProducerSurfaceKHR", (EglWrapperFuncPointer)&EglCreateStreamProducerSurfaceKHRImpl },

    { "eglQueryStreamTimeKHR", (EglWrapperFuncPointer)&EglQueryStreamTimeKHRImpl },

    { "eglGetStreamFileDescriptorKHR", (EglWrapperFuncPointer)&EglGetStreamFileDescriptorKHRImpl },
    { "eglCreateStreamFromFileDescriptorKHR", (EglWrapperFuncPointer)&EglCreateStreamFromFileDescriptorKHRImpl },

    { "eglWaitSyncKHR", (EglWrapperFuncPointer)&EglWaitSyncKHRImpl },

    { "eglGetPlatformDisplayEXT", (EglWrapperFuncPointer)&EglGetPlatformDisplayEXTImpl },

    { "eglSwapBuffersWithDamageKHR", (EglWrapperFuncPointer)&EglSwapBuffersWithDamageKHRImpl },
    { "eglSetDamageRegionKHR", (EglWrapperFuncPointer)&EglSetDamageRegionKHRImpl },
    { "eglSetBlobCacheFuncsANDROID", (EglWrapperFuncPointer)&EglSetBlobCacheFuncsANDROIDImpl },

};

EglWrapperFuncPointer FindEglWrapperApi(const std::string &name)
{
    if (gEglWrapperMap.find(name) != gEglWrapperMap.end()) {
        return gEglWrapperMap.at(name);
    }

    WLOGW("FindEglWrapperApi did not find an entry for %{public}s", name.c_str());
    return nullptr;
}

bool CheckEglWrapperApi(const std::string &name)
{
    if (gEglWrapperMap.find(name) != gEglWrapperMap.end()) {
        return true;
    }
    return false;
}
}; // namespace OHOS

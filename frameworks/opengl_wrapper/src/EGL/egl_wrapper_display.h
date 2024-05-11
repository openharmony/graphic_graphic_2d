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
#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_DISPLAY_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_DISPLAY_H

#include <mutex>
#include <unordered_set>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace OHOS {
class EglWrapperObject;
class EglWrapperContext;
class EglWrapperSurface;
#if USE_IGRAPHICS_EXTENDS_HOOKS
struct GlHookTable;
#endif

class EglWrapperDisplay {
public:
    EGLBoolean Init(EGLint *major, EGLint *minor);
    EGLBoolean Terminate();
    EGLBoolean MakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx);
#if USE_IGRAPHICS_EXTENDS_HOOKS
    EGLBoolean MakeCurrentAfterHook(EGLSurface draw, EGLSurface read, EGLContext ctx);
#endif
    static EglWrapperDisplay *GetWrapperDisplay(EGLDisplay display);
    static EGLDisplay GetEglDisplay(EGLenum platform, EGLNativeDisplayType disp, const EGLAttrib *attribList);
    static EGLDisplay GetEglDisplayExt(EGLenum platform, void *disp, const EGLint *attribList);
    static bool ValidateEglContext(EGLContext ctx);
    static bool ValidateEglSurface(EGLSurface surf);
    EGLContext CreateEglContext(EGLConfig config, EGLContext shareList, const EGLint *attribList);
    EGLSurface CreateEglSurface(EGLConfig config, NativeWindowType window, const EGLint *attribList);
    EGLBoolean DestroyEglContext(EGLContext context);
    EGLBoolean DestroyEglSurface(EGLSurface surf);

    void AddObject(EglWrapperObject *obj);
    void RemoveObject(EglWrapperObject *obj);

    EGLBoolean CopyBuffers(EGLSurface surf, NativePixmapType target);
    EGLSurface CreatePbufferSurface(EGLConfig config, const EGLint *attribList);
    EGLSurface CreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attribList);
    inline bool IsReady() const
    {
        return (refCnt_ > 0);
    };
    inline EGLDisplay GetEglDisplay() const
    {
        return disp_;
    };

    EGLBoolean QueryContext(EGLContext ctx, EGLint attribute, EGLint *value);
    EGLBoolean QuerySurface(EGLSurface surf, EGLint attribute, EGLint *value);
    EGLBoolean SwapBuffers(EGLSurface surf);
    EGLBoolean BindTexImage(EGLSurface surf, EGLint buffer);
    EGLBoolean ReleaseTexImage(EGLSurface surf, EGLint buffer);
    EGLBoolean SurfaceAttrib(EGLSurface surf, EGLint attribute, EGLint value);
    EGLSurface CreatePbufferFromClientBuffer(EGLenum buftype,
        EGLClientBuffer buffer, EGLConfig config, const EGLint *attribList);
    EGLImage CreateImage(EGLContext ctx, EGLenum target,
        EGLClientBuffer buffer, const EGLAttrib *attribList);
    EGLBoolean DestroyImage(EGLImage img);
    EGLSurface CreatePlatformWindowSurface(EGLConfig config,
        void *nativeWindow, const EGLAttrib *attribList);
    EGLSurface CreatePlatformPixmapSurface(EGLConfig config,
        void *nativePixmap, const EGLAttrib *attribList);
    EGLBoolean LockSurfaceKHR(EGLSurface surf, const EGLint *attribList);
    EGLBoolean UnlockSurfaceKHR(EGLSurface surf);

    EGLImageKHR CreateImageKHR(EGLContext ctx, EGLenum target,
        EGLClientBuffer buffer, const EGLint *attribList);
    EGLBoolean DestroyImageKHR(EGLImageKHR img);

    EGLSurface CreateStreamProducerSurfaceKHR(EGLConfig config,
        EGLStreamKHR stream, const EGLint *attribList);

    EGLBoolean SwapBuffersWithDamageKHR(EGLSurface draw, EGLint *rects, EGLint nRects);
    EGLBoolean SetDamageRegionKHR(EGLSurface surf, EGLint *rects, EGLint nRects);
    EGLBoolean GetCompositorTimingSupportedANDROID(EGLSurface surface, EGLint name);
    EGLBoolean GetFrameTimestampSupportedANDROID(EGLSurface surface, EGLint timestamp);
    EGLBoolean PresentationTimeANDROID(EGLSurface surface, EGLnsecsANDROID time);
    EGLSurface CreatePlatformWindowSurfaceEXT(EGLConfig config, void *nativeWindow, const EGLint *attribList);
    EGLSurface CreatePlatformPixmapSurfaceEXT(EGLConfig config, void *nativePixmap, const EGLint *attribList);
    EGLBoolean SwapBuffersWithDamageEXT(EGLSurface surface, const EGLint *rects, EGLint nRects);
private:
    EglWrapperDisplay() noexcept;
    ~EglWrapperDisplay();
    EGLDisplay GetEglNativeDisplay(EGLenum platform, EGLNativeDisplayType disp, const EGLAttrib *attribList);
    EGLDisplay GetEglNativeDisplayExt(EGLenum platform, void *disp, const EGLint *attribList);
    bool CheckObject(EglWrapperObject *obj);
    void ClearObjects();
    EGLBoolean InternalMakeCurrent(EglWrapperSurface *draw, EglWrapperSurface *read, EglWrapperContext *ctx,
        bool isAfterHook = false, EglWrapperContext *curCtx = nullptr);

#if USE_IGRAPHICS_EXTENDS_HOOKS
    void ChooseHookTable(bool isAfterHook, const EglWrapperContext *ctx, const EglWrapperContext *curCtx,
        GlHookTable **ppHookTable);
    static int ChooseGlesVersion(const EGLint *attribList);
#endif

    static EglWrapperDisplay wrapperDisp_;
    EGLDisplay  disp_;
    std::mutex  lockMutex_;
    std::recursive_mutex refLockMutex_;
    std::unordered_set<EglWrapperObject *> objects_;
    uint32_t    refCnt_;
};
} // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_DISPLAY_H

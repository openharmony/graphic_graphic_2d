/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "egl_manager.h"

#include <fcntl.h>
#include <scoped_bytrace.h>
#include <mutex>
#include <unistd.h>

#include "buffer_log.h"

namespace OHOS {
namespace {
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char *CHARACTER_STRING_WHITESPACE = " ";
constexpr const char *EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char *EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr const char *EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
constexpr const char *EGL_EXT_IMAGE_DMA_BUF_IMPORT = "EGL_EXT_image_dma_buf_import";
constexpr const char *EGL_KHR_SURFACELESS_CONTEXT = "EGL_KHR_surfaceless_context";
constexpr const char *EGL_KHR_NO_CONFIG_CONTEXT = "EGL_KHR_no_config_context";
constexpr const char *EGL_CREATE_IMAGE_KHR = "eglCreateImageKHR";
constexpr const char *EGL_DESTROY_IMAGE_KHR = "eglDestroyImageKHR";
constexpr const char *EGL_IMAGE_TARGET_TEXTURE2DOES = "glEGLImageTargetTexture2DOES";
constexpr const char *EGL_KHR_FENCE_SYNC = "EGL_KHR_fence_sync";
constexpr const char *EGL_CREATE_SYNC_KHR = "eglCreateSyncKHR";
constexpr const char *EGL_DESTROY_SYNC_KHR = "eglDestroySyncKHR";
constexpr const char *EGL_CLIENT_WAIT_SYNC_KHR = "eglClientWaitSyncKHR";
constexpr const char *EGL_DUP_NATIVE_FENCE_FD_ANDROID = "eglDupNativeFenceFDANDROID";
constexpr const char *EGL_KHR_WAIT_SYNC = "EGL_KHR_wait_sync";
constexpr const char *EGL_WAIT_SYNC_KHR = "eglWaitSyncKHR";
constexpr const char *GL_OES_EGL_IMAGE = "GL_OES_EGL_image";
} // namespace

EglManager::EglManager()
{
    BLOGD("EglManager");
}

EglManager::~EglManager()
{
    BLOGD("~EglManager");
    Deinit();
}

sptr<EglManager> EglManager::GetInstance()
{
    if (instance_ == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance_ == nullptr) {
            instance_ = new EglManager();
        }
    }
    return instance_;
}

bool EglManager::IsInit() const
{
    return initFlag_;
}

GSError EglManager::Init(EGLContext context)
{
    ScopedBytrace eGLManagerInit("EGLManagerInit");
    if (initFlag_) {
        BLOGW("already init, eglMakeCurrent");
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_);
        return GSERROR_OK;
    }

    if (GbmInit() != GSERROR_OK) {
        BLOGE("GbmInit failed.");
        return GSERROR_INTERNEL;
    }

    if (EglInit(context) != GSERROR_OK) {
        BLOGE("EglInit failed.");
        return GSERROR_INTERNEL;
    }

    initFlag_ = true;
    return GSERROR_OK;
}

GSError EglManager::GbmInit()
{
    device_ = EGL_DEFAULT_DISPLAY;
    return GSERROR_OK;
}

namespace {
static bool CheckEglExtension(const char *extensions, const char *extension)
{
    size_t extlen = strlen(extension);
    const char *end = extensions + strlen(extensions);

    while (extensions < end) {
        size_t n = 0;

        /* Skip whitespaces, if any */
        if (*extensions == CHARACTER_WHITESPACE) {
            extensions++;
            continue;
        }

        n = strcspn(extensions, CHARACTER_STRING_WHITESPACE);
        /* Compare strings */
        if (n == extlen && strncmp(extension, extensions, n) == 0) {
            return true; /* Found */
        }
        extensions += n;
    }
    /* Not found */
    return false;
}

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, EGLDisplay native_display, const EGLint *attrib_list)
{
    static GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

    if (!eglGetPlatformDisplayExt) {
        const char *extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions &&
            (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
             CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
            eglGetPlatformDisplayExt =
                (GetPlatformDisplayExt)eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT);
        }
    }

    if (eglGetPlatformDisplayExt) {
        return eglGetPlatformDisplayExt(platform, native_display, attrib_list);
    }
    return eglGetDisplay((EGLNativeDisplayType) native_display);
}
}

GSError EglManager::EglCheckExt()
{
    const char *eglExtensions = eglQueryString(display_, EGL_EXTENSIONS);
    if (eglExtensions == nullptr) {
        BLOGE("param is nullptr.");
        return GSERROR_INTERNEL;
    }

    if (!CheckEglExtension(eglExtensions, EGL_EXT_IMAGE_DMA_BUF_IMPORT)) {
        BLOGE("EGL_EXT_image_dma_buf_import not supported");
        return GSERROR_INTERNEL;
    }

    if (!CheckEglExtension(eglExtensions, EGL_KHR_SURFACELESS_CONTEXT)) {
        BLOGE("EGL_KHR_surfaceless_context not supported");
        return GSERROR_INTERNEL;
    }

    if (CheckEglExtension(eglExtensions, EGL_KHR_NO_CONFIG_CONTEXT)) {
        conf_ = EGL_NO_CONFIG_KHR;
    } else {
        BLOGW("EGL_KHR_no_config_context not supported");
        EGLint count;
        EGLint config_attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 1,
            EGL_GREEN_SIZE, 1,
            EGL_BLUE_SIZE, 1,
            EGL_ALPHA_SIZE, 1,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };
        EGLBoolean ret = eglChooseConfig(display_, config_attribs, &conf_, 1, &count);
        if (!(ret && count >= 1)) {
            BLOGE("Failed to eglChooseConfig");
            return GSERROR_INTERNEL;
        }
    }
    return GSERROR_OK;
}

GSError EglManager::EglFuncInit()
{
    const char *eglExtensions = eglQueryString(display_, EGL_EXTENSIONS);
    if (eglExtensions == nullptr) {
        BLOGE("param is nullptr.");
        return GSERROR_INTERNEL;
    }

    createImage_ = (EglCreateImageFunc)eglGetProcAddress(EGL_CREATE_IMAGE_KHR);
    if (createImage_ == nullptr) {
        BLOGE("param is nullptr.");
        return GSERROR_INTERNEL;
    }

    destroyImage_ = (EglDestroyImageFunc)eglGetProcAddress(EGL_DESTROY_IMAGE_KHR);
    if (destroyImage_ == nullptr) {
        BLOGE("param is nullptr.");
        return GSERROR_INTERNEL;
    }

    imageTargetTexture2d_ = (EglImageTargetTexture2DFunc)eglGetProcAddress(EGL_IMAGE_TARGET_TEXTURE2DOES);
    if (imageTargetTexture2d_ == nullptr) {
        BLOGE("param is nullptr.");
        return GSERROR_INTERNEL;
    }
    
    if (CheckEglExtension(eglExtensions, EGL_KHR_FENCE_SYNC)) {
        createSync_ = (EglCreateSyncFunc)eglGetProcAddress(EGL_CREATE_SYNC_KHR);
        if (createSync_ == nullptr) {
            BLOGE("param is nullptr.");
            return GSERROR_INTERNEL;
        }
        
        destroySync_ = (EglDestroySyncFunc)eglGetProcAddress(EGL_DESTROY_SYNC_KHR);
        if (destroySync_ == nullptr) {
            BLOGE("param is nullptr.");
            return GSERROR_INTERNEL;
        }

        clientWaitSync_ = (EglClientWaitSyncFunc)eglGetProcAddress(EGL_CLIENT_WAIT_SYNC_KHR);
        if (clientWaitSync_ == nullptr) {
            BLOGE("param is nullptr.");
            return GSERROR_INTERNEL;
        }

        dupNativeFenceFd_ = (EglDupNativeFenceFdFunc)eglGetProcAddress(EGL_DUP_NATIVE_FENCE_FD_ANDROID);
        if (dupNativeFenceFd_ == nullptr) {
            BLOGE("param is nullptr.");
            return GSERROR_INTERNEL;
        }
    }

    if (CheckEglExtension(eglExtensions, EGL_KHR_WAIT_SYNC)) {
        waitSync_ = (EglWaitSyncFunc)eglGetProcAddress(EGL_WAIT_SYNC_KHR);
        if (waitSync_ == nullptr) {
            BLOGE("param is nullptr.");
            return GSERROR_INTERNEL;
        }
    }
    return GSERROR_OK;
}

GSError EglManager::EglInit(EGLContext ctx)
{
    ScopedBytrace func(__func__);
    display_ = GetPlatformEglDisplay(EGL_PLATFORM_GBM_KHR, device_, NULL);
    if (display_ == EGL_NO_DISPLAY) {
        BLOGE("Failed to create EGLDisplay");
        return GSERROR_INTERNEL;
    }

    EGLint major, minor;
    if (eglInitialize(display_, &major, &minor) == EGL_FALSE) {
        BLOGE("Failed to initialize EGLDisplay");
        return GSERROR_INTERNEL;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        BLOGE("Failed to bind OpenGL ES API");
        return GSERROR_INTERNEL;
    }

    if (EglCheckExt() != GSERROR_OK) {
        BLOGE("EglCheckExt failed");
        return GSERROR_INTERNEL;
    }

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM,
        EGL_NONE
    };

    if (ctx == EGL_NO_CONTEXT) {
        context_ = eglCreateContext(display_, conf_, EGL_NO_CONTEXT, context_attribs);
        if (context_ == EGL_NO_CONTEXT) {
            BLOGE("Failed to create EGLContext");
            return GSERROR_INTERNEL;
        }
        ctxReleaseFlg_ = true;
    } else {
        context_ = ctx;
    }

    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_);

    const char *glExtensions = (const char *) glGetString(GL_EXTENSIONS);
    if (glExtensions == nullptr) {
        BLOGE("param is nullptr.");
        return GSERROR_INTERNEL;
    }

    if (!CheckEglExtension(glExtensions, GL_OES_EGL_IMAGE)) {
        BLOGE("GL_OES_EGL_image not supported");
        return GSERROR_INTERNEL;
    }

    if (EglFuncInit() != GSERROR_OK) {
        BLOGE("EglFuncInit failed");
        return GSERROR_INTERNEL;
    }

    return GSERROR_OK;
}

void EglManager::Deinit()
{
    initFlag_ = false;

    if (drmFd_ >= 0) {
        close(drmFd_);
    }

    if (context_ != EGL_NO_CONTEXT && ctxReleaseFlg_) {
        eglDestroyContext(display_, context_);
    }

    if (display_ != EGL_NO_DISPLAY) {
        eglTerminate(display_);
    }
}

EGLDisplay EglManager::GetEGLDisplay() const
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return EGL_NO_DISPLAY;
    }
    return display_;
}

EGLContext EglManager::GetEGLContext() const
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return EGL_NO_CONTEXT;
    }
    return context_;
}

EGLImageKHR EglManager::EglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attribList)
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return EGL_NO_IMAGE;
    }
    return createImage_(display_, ctx, target, buffer, attribList);
}

EGLImageKHR EglManager::EglCreateImage(EGLenum target, EGLClientBuffer buffer, const EGLint *attribList)
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return EGL_NO_IMAGE;
    }
    return createImage_(display_, context_, target, buffer, attribList);
}

EGLBoolean EglManager::EglDestroyImage(EGLImageKHR image)
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return EGL_FALSE;
    }
    return destroyImage_(display_, image);
}

void EglManager::EglImageTargetTexture2D(GLenum target, GLeglImageOES image)
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return;
    }
    return imageTargetTexture2d_(target, image);
}

EGLSyncKHR EglManager::EglCreateSync(EGLenum type, const EGLint *attribList)
{
    if (createSync_ == nullptr) {
        BLOGE("param is nullptr.");
        return EGL_NO_SYNC_KHR;
    }
    return createSync_(display_, type, attribList);
}

EGLint EglManager::EglWaitSync(EGLSyncKHR sync, EGLint flags)
{
    if (waitSync_ == nullptr) {
        BLOGE("param is nullptr.");
        return EGL_FALSE;
    }
    return waitSync_(display_, sync, flags);
}

EGLint EglManager::EglClientWaitSync(EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout)
{
    if (clientWaitSync_ == nullptr) {
        BLOGE("param is nullptr.");
        return EGL_FALSE;
    }
    return clientWaitSync_(display_, sync, flags, timeout);
}

EGLBoolean EglManager::EglDestroySync(EGLSyncKHR sync)
{
    if (destroySync_ == nullptr) {
        BLOGE("param is nullptr.");
        return EGL_FALSE;
    }
    return destroySync_(display_, sync);
}

EGLint EglManager::EglDupNativeFenceFd(EGLSyncKHR sync)
{
    if (dupNativeFenceFd_ == nullptr) {
        BLOGE("param is nullptr.");
        return EGL_DONT_CARE;
    }
    return dupNativeFenceFd_(display_, sync);
}

EGLBoolean EglManager::EglMakeCurrent()
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return EGL_FALSE;
    }
    return eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_);
}

EGLBoolean EglManager::EglMakeCurrent(EGLSurface draw, EGLSurface read)
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return EGL_FALSE;
    }
    return eglMakeCurrent(display_, draw, read, context_);
}

EGLBoolean EglManager::EglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    if (!initFlag_) {
        BLOGE("EglManager is not init.");
        return EGL_FALSE;
    }
    return eglMakeCurrent(display_, draw, read, ctx);
}
} // namespace OHOS

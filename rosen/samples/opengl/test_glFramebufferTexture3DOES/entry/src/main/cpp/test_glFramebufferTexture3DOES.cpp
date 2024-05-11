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

#include "napi/native_api.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <hilog/log.h>

static EGLDisplay g_Display = nullptr;
static EGLContext g_Context = nullptr;
static EGLSurface g_Surface = nullptr;
static const unsigned int LOG_PRINT_DOMAIN = 0xFF01;
static const int SUCCESS = 0;
static const int FAILED = -1;
static const int INT_INIT_VAL = 0;
static const int RGB_SIZE = 8;
static const int CREAT_NUM_ONE = 1;
static const int INIT_WIDTH = 480;
static const int INIT_HEIGHT = 800;
static const int INIT_EGL_VERSION = 3;
static const int MATRIX_2 = 2;
static const int MATRIX_8 = 8;
static const int MATRIX_15 = 15;
static const int MATRIX_16 = 16;

static napi_value GetError(const napi_env env, const GLenum glError)
{
    napi_value result = nullptr;
    if (GL_NO_ERROR == glError) {
        napi_create_int32(env, SUCCESS, &result);
    } else {
        napi_create_int32(env, FAILED, &result);
    }
    return result;
}

void InitGLES()
{
    g_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(g_Display, nullptr, nullptr);
    EGLint numConfigs = INT_INIT_VAL;
    const EGLint configAttribs[] = {EGL_RED_SIZE, RGB_SIZE, EGL_GREEN_SIZE, RGB_SIZE,
                                    EGL_BLUE_SIZE, RGB_SIZE, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                                    EGL_NONE};
    EGLConfig config = nullptr;
    eglChooseConfig(g_Display, configAttribs, &config, CREAT_NUM_ONE, &numConfigs);
    const EGLint surfaceAttribs[] = {EGL_WIDTH, INIT_WIDTH, EGL_HEIGHT, INIT_HEIGHT, EGL_NONE};
    g_Surface = eglCreatePbufferSurface(g_Display, config, surfaceAttribs);
    const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, INIT_EGL_VERSION, EGL_NONE};
    g_Context = eglCreateContext(g_Display, config, EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(g_Display, g_Surface, g_Surface, g_Context);
}

void DestroyGLES()
{
    eglDestroySurface(g_Display, g_Surface);
    eglDestroyContext(g_Display, g_Context);
    eglTerminate(g_Display);
    eglSwapBuffers(g_Display, g_Surface);
}

static napi_value GLCopyTexSubImage3D(napi_env env, napi_callback_info info)
{
    InitGLES();
    GLuint texture;
    glGenTextures(CREAT_NUM_ONE, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexImage3D(GL_TEXTURE_3D, INT_INIT_VAL, GL_R8, MATRIX_8, MATRIX_8, MATRIX_8, INT_INIT_VAL,
        GL_RED, GL_UNSIGNED_BYTE, nullptr);
    GLuint framebuffer;
    glGenFramebuffers(CREAT_NUM_ONE, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, INT_INIT_VAL, MATRIX_8);
    glGenerateMipmap(GL_TEXTURE_3D);
    glTexImage3D(GL_TEXTURE_3D, INT_INIT_VAL, GL_R8UI, MATRIX_16, MATRIX_16, MATRIX_16, INT_INIT_VAL,
        GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture3DOES(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, texture, 0, 0);
    glCopyTexSubImage3D(GL_TEXTURE_3D, INT_INIT_VAL, INT_INIT_VAL, INT_INIT_VAL, MATRIX_2, MATRIX_2,
        MATRIX_15, MATRIX_16, MATRIX_16);
    GLenum glError = glGetError();
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "EGLCore", "glError : %{public}d", glError);
    DestroyGLES();
    return GetError(env, glError);
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        { "testglFramebufferTexture3DOES", nullptr, GLCopyTexSubImage3D, nullptr, nullptr, nullptr,
            napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version =1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}

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
#include <iostream>
#include <vector>
#include <fstream>
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
static const int OFFSET = 16;
static const int DEFAULT_SIZE = 256;
static const int INDEX_NUM = 2;
static const int STRIDE = 4;

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

std::vector<char> ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "EGLCore", "Unable to open file: %{public}d", errno);
        return {};
    }
    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), fileSize);
    return buffer;
}

static int TestGlCompressedTexImage2D()
{
    auto data = ReadFile("/mnt/hmdfs/100/1.pkm");
    if (data.empty()) {
        return FAILED;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB8_ETC2, DEFAULT_SIZE, DEFAULT_SIZE, 0,
        data.size() - OFFSET, data.data() + OFFSET);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (textureID == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Failed to load texture");
        return FAILED;
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.5f, 1.0f
    };
    glVertexAttribPointer(0, INDEX_NUM, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, INDEX_NUM, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), vertices + INDEX_NUM);
    glEnableVertexAttribArray(1);
    glDeleteTextures(1, &textureID);
    return SUCCESS;
}

static int TestGlCompressedTexSubImage2D()
{
    auto data = ReadFile("/mnt/hmdfs/100/1.pkm");
    if (data.empty()) {
        return FAILED;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB8_ETC2, DEFAULT_SIZE, DEFAULT_SIZE, 0,
        data.size() - OFFSET, data.data() + OFFSET);
    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DEFAULT_SIZE, DEFAULT_SIZE, GL_COMPRESSED_RGB8_ETC2,
        data.size() - OFFSET, data.data() + OFFSET);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (textureID == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Failed to load texture");
        return FAILED;
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.5f, 1.0f
    };
    glVertexAttribPointer(0, INDEX_NUM, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, INDEX_NUM, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), vertices + INDEX_NUM);
    glEnableVertexAttribArray(1);
    glDeleteTextures(1, &textureID);
    return SUCCESS;
}

static int TestGlCompressedTexImage3D()
{
    auto data = ReadFile("/mnt/hmdfs/100/1.pkm");
    if (data.empty()) {
        return FAILED;
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glCompressedTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_COMPRESSED_RGB8_ETC2, DEFAULT_SIZE, DEFAULT_SIZE, 1, 0,
        data.size() - OFFSET, data.data() + OFFSET);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (textureID == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Failed to load texture");
        return FAILED;
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.5f, 1.0f
    };
    glVertexAttribPointer(0, INDEX_NUM, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, INDEX_NUM, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), vertices + INDEX_NUM);
    glEnableVertexAttribArray(1);
    glDeleteTextures(1, &textureID);
    return SUCCESS;
}

static int TestGlCompressedTexSubImage3D()
{
    auto data = ReadFile("/mnt/hmdfs/100/1.pkm");
    if (data.empty()) {
        return FAILED;
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glCompressedTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_COMPRESSED_RGB8_ETC2, DEFAULT_SIZE, DEFAULT_SIZE, 1, 0,
        data.size() - OFFSET, data.data() + OFFSET);
    glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, DEFAULT_SIZE, DEFAULT_SIZE, 1, GL_COMPRESSED_RGB8_ETC2,
        data.size() - OFFSET, data.data() + OFFSET);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (textureID == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Failed to load texture");
        return FAILED;
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.5f, 1.0f
    };
    glVertexAttribPointer(0, INDEX_NUM, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, INDEX_NUM, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), vertices + INDEX_NUM);
    glEnableVertexAttribArray(1);
    glDeleteTextures(1, &textureID);
    return SUCCESS;
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

static napi_value CreateResult(napi_env env, int status)
{
    napi_value result = nullptr;
    napi_create_int32(env, status, &result);
    return result;
}

static napi_value GLCompressApi(napi_env env, napi_callback_info info)
{
    InitGLES();
    if (TestGlCompressedTexImage2D() != SUCCESS) {
        DestroyGLES();
        return CreateResult(env, FAILED);
    }
    if (TestGlCompressedTexSubImage2D() != SUCCESS) {
        DestroyGLES();
        return CreateResult(env, FAILED);
    }
    if (TestGlCompressedTexImage3D() != SUCCESS) {
        DestroyGLES();
        return CreateResult(env, FAILED);
    }
    if (TestGlCompressedTexSubImage3D() != SUCCESS) {
        DestroyGLES();
        return CreateResult(env, FAILED);
    }

    GLenum glError = glGetError();
    DestroyGLES();
    return GetError(env, glError);
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        { "testGlCompressApi", nullptr, GLCompressApi, nullptr, nullptr, nullptr,
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

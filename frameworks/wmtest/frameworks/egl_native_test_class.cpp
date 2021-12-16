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

#include "egl_native_test_class.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <securec.h>
#include <sys/time.h>

#include <display_type.h>

#include "inative_test.h"
#include "util.h"

namespace OHOS {
sptr<EGLNativeTestSync> EGLNativeTestSync::CreateSync(EGLDrawFunc drawFunc,
    sptr<EglSurface> &peglsurface, uint32_t width, uint32_t height, void *data)
{
    if (drawFunc != nullptr && peglsurface != nullptr) {
        sptr<EGLNativeTestSync> nts = new EGLNativeTestSync();
        nts->draw = drawFunc;
        nts->eglsurface = peglsurface;
        nts->width_ = width;
        nts->height_ = height;
        RequestSync(std::bind(&EGLNativeTestSync::Sync, nts, SYNC_FUNC_ARG), data);
        return nts;
    }
    return nullptr;
}

void EGLNativeTestSync::Sync(int64_t, void *data)
{
    if (!GLContextInit()) {
        printf("GLContextInit failed.\n");
        return;
    }

    if (sret == SURFACE_ERROR_OK) {
        draw(&glCtx, eglsurface, width_, height_);
        count++;
    }

    sret = eglsurface->SwapBuffers();

    RequestSync(std::bind(&EGLNativeTestSync::Sync, this, SYNC_FUNC_ARG), data);
}

namespace {
const char *g_vertShaderText =
    "uniform float offset;\n"
    "attribute vec4 pos;\n"
    "attribute vec4 color;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
    "  gl_Position = pos + vec4(offset, offset, 0.0, 0.0);\n"
    "  v_color = color;\n"
    "}\n";

const char *g_fragShaderText =
    "precision mediump float;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
    "  gl_FragColor = v_color;\n"
    "}\n";

static GLuint CreateShader(const char *source, GLenum shaderType)
{
    GLuint shader;
    GLint status;

    shader = glCreateShader(shaderType);
    assert(shader != 0);

    glShaderSource(shader, 1, (const char **) &source, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        constexpr int32_t maxLogLength = 1000;
        char log[maxLogLength];
        GLsizei len;
        glGetShaderInfoLog(shader, maxLogLength, &len, log);
        fprintf(stderr, "Error: compiling %s: %.*s\n",
            shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", len, log);
        return 0;
    }

    return shader;
}

static GLuint CreateAndLinkProgram(GLuint vert, GLuint frag)
{
    GLint status;
    GLuint program = glCreateProgram();

    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        constexpr int32_t maxLogLength = 1000;
        char log[maxLogLength];
        GLsizei len;
        glGetProgramInfoLog(program, maxLogLength, &len, log);
        fprintf(stderr, "Error: linking:\n%.*s\n", len, log);
        return 0;
    }

    return program;
}
} // namespace

bool EGLNativeTestSync::GLContextInit()
{
    if (bInit) {
        return bInit;
    }

    if (eglsurface == nullptr) {
        printf("GLContextInit eglsurface is nullptr\n");
        return bInit;
    }

    if (eglsurface->InitContext() != SURFACE_ERROR_OK) {
        printf("GLContextInit InitContext failed\n");
        return bInit;
    }

    GLuint vert = CreateShader(g_vertShaderText, GL_VERTEX_SHADER);
    GLuint frag = CreateShader(g_fragShaderText, GL_FRAGMENT_SHADER);

    glCtx.program = CreateAndLinkProgram(vert, frag);

    glDeleteShader(vert);
    glDeleteShader(frag);

    glCtx.pos = glGetAttribLocation(glCtx.program, "pos");
    glCtx.color = glGetAttribLocation(glCtx.program, "color");

    glUseProgram(glCtx.program);

    glCtx.offsetUniform = glGetUniformLocation(glCtx.program, "offset");

    if (glCtx.program == 0) {
        printf("glCtx.program = 0.\n");
    } else {
        bInit = true;
    }
    return bInit;
}

void EGLNativeTestDraw::FlushDraw(GLContext *ctx, sptr<EglSurface> &eglsurface, uint32_t width, uint32_t height)
{
    /* Complete a movement iteration in 5000 ms. */
    static const GLfloat verts[][0x2] = {
        { -0.5, -0.5 },
        { -0.5,  0.5 },
        {  0.5, -0.5 },
        {  0.5,  0.5 }
    };
    static const GLfloat colors[][0x3] = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
        { 1, 1, 0 }
    };

    /* Split time_ms in repeating windows of [0, iterationMs) and map them
     * to offsets in the [-0.5, 0.5) range. */
    constexpr uint64_t iterationMs = 5000000000;
    GLfloat offset = (GetNowTime() % iterationMs) / static_cast<double>(iterationMs);

    glViewport(0, 0, width, height);

    glUniform1f(ctx->offsetUniform, offset - 0.5);

    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    constexpr int32_t sizePosVertexAttribute = 2;
    constexpr int32_t sizeColorsVertexAttribute = 3;
    glVertexAttribPointer(ctx->pos, sizePosVertexAttribute, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(ctx->color, sizeColorsVertexAttribute, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(ctx->pos);
    glEnableVertexAttribArray(ctx->color);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 0x4);

    glDisableVertexAttribArray(ctx->pos);
    glDisableVertexAttribArray(ctx->color);
}
} // namespace OHOS

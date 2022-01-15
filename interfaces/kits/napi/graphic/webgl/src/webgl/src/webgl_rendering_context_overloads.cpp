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

#include "../include/context/webgl_rendering_context_overloads.h"
#include "../../common/napi/n_func_arg.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/util/log.h"
#include "../include/webgl/webgl_uniform_location.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGLRenderingContextOverloads::BufferData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bufferData start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::bufferData target = %{public}u", target);
    bool usagesucc = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_number);
    LOGI("WebGL WebGLRenderContext::bufferData usagesucc = %{public}u", usagesucc);
    size_t size;
    void *data = nullptr;
    if (usagesucc) {
        tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return nullptr;
        }
    } else {
        if (funcArg[NARG_POS::SECOND] == nullptr) {
            return nullptr;
        }
        bool isdataview = false;
        napi_is_dataview(env, funcArg[NARG_POS::SECOND], &isdataview);
        LOGI("WebGL WebGLRenderContext::bufferData isdataview = %{public}u", isdataview);
        if (isdataview) {
            tie(succ, data, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToDataview();
            if (!succ) {
                return nullptr;
            }
        } else {
            bool isarraybuffer = false;
            napi_is_arraybuffer(env, funcArg[NARG_POS::SECOND], &isarraybuffer);
            LOGI("WebGL WebGLRenderContext::bufferData isarraybuffer = %{public}u", isarraybuffer);
            if (isarraybuffer) {
                tie(succ, data, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToArraybuffer();
                if (!succ) {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
        }
    }
    LOGI("WebGL WebGLRenderContext::bufferData size = %{public}u", size);
    LOGI("WebGL WebGLRenderContext::bufferData data = %{public}u", data);
    int32_t usage;
    tie(succ, usage) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::bufferData usage = %{public}u", usage);
    glBufferData(static_cast<GLenum>(target), static_cast<GLsizeiptr>(size),
        static_cast<void*>(data), static_cast<GLenum>(usage));
    LOGI("WebGL bufferData end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::BufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bufferSubData start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    LOGI("WebGL WebGLRenderContext::bufferSubData succ = %{public}u", succ);
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::bufferSubData target = %{public}u", target);
    int64_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::bufferSubData offset = %{public}u", offset);
    size_t size;
    void *data = nullptr;
    bool isdataview = false;
    napi_is_dataview(env, funcArg[NARG_POS::THIRD], &isdataview);
    LOGI("WebGL WebGLRenderContext::bufferSubData isdataview = %{public}u", isdataview);
    if (isdataview) {
        tie(succ, data, size) = NVal(env, funcArg[NARG_POS::THIRD]).ToDataview();
        if (!succ) {
            return nullptr;
        }
    } else {
        bool isarraybuffer = false;
        napi_is_arraybuffer(env, funcArg[NARG_POS::THIRD], &isarraybuffer);
        LOGI("WebGL WebGLRenderContext::bufferSubData isarraybuffer = %{public}u", isarraybuffer);
        if (isarraybuffer) {
            tie(succ, data, size) = NVal(env, funcArg[NARG_POS::THIRD]).ToArraybuffer();
            if (!succ) {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }
    LOGI("WebGL WebGLRenderContext::bufferSubData size = %{public}u", size);
    LOGI("WebGL WebGLRenderContext::bufferSubData data = %{public}u", data);
    glBufferSubData(static_cast<GLenum>(target), static_cast<GLintptr>(offset),
        static_cast<GLsizeiptr>(size), static_cast<void*>(data));
    LOGI("WebGL bufferSubData end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::CompressedTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL compressedTexImage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D target = %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D index = %{public}u", level);
    int64_t internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D level = %{public}u", internalformat);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D height = %{public}u", height);
    int64_t border;
    tie(succ, border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D border = %{public}u", border);
    size_t imageSize;
    void *data = nullptr;
    bool isdataview = false;
    napi_is_dataview(env, funcArg[NARG_POS::SEVENTH], &isdataview);
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D isdataview = %{public}u", isdataview);
    if (isdataview) {
        tie(succ, data, imageSize) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
    } else {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D imageSize = %{public}u", imageSize);
    LOGI("WebGL WebGLRenderContext::compressedTexImage2D data = %{public}u", data);
    glCompressedTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
        static_cast<GLenum>(internalformat), static_cast<GLsizei>(width),
        static_cast<GLsizei>(height), static_cast<GLint>(border),
        static_cast<GLsizei>(imageSize), static_cast<void*>(data));
    LOGI("WebGL compressedTexImage2D end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::CompressedTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL compressedTexSubImage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D target = %{public}u", target);
    int32_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D level = %{public}u", level);
    int64_t xoffset;
    tie(succ, xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D xoffset = %{public}u", xoffset);
    int64_t yoffset;
    tie(succ, yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D yoffset = %{public}u", yoffset);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D height = %{public}u", height);
    int64_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D format = %{public}u", format);
    size_t imageSize;
    void *data = nullptr;
    bool isdataview = false;
    napi_is_dataview(env, funcArg[NARG_POS::EIGHTH], &isdataview);
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D isdataview = %{public}u", isdataview);
    if (isdataview) {
        tie(succ, data, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
    } else {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D imageSize = %{public}u", imageSize);
    LOGI("WebGL WebGLRenderContext::compressedTexSubImage2D data = %{public}u", data);
    glCompressedTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xoffset),
        static_cast<GLint>(yoffset), static_cast<GLsizei>(width), static_cast<GLsizei>(height),
        static_cast<GLenum>(format), static_cast<GLsizei>(imageSize),
        static_cast<void*>(data));
    LOGI("WebGL compressedTexSubImage2D end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::ReadPixels(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL readPixels start");
    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::readPixels  = %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::readPixels y = %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::readPixels width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::readPixels height = %{public}u", height);
    int64_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::readPixels format = %{public}u", format);
    int64_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::readPixels type = %{public}u", type);
    if (funcArg[NARG_POS::SEVENTH] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::readPixels imageSize start");
    size_t imageSize;
    void *pixels = nullptr;
    tie(succ, pixels, imageSize) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToDataview();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::readPixels pixels = %{public}u", pixels);
    glReadPixels(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
        static_cast<GLsizei>(height), static_cast<GLenum>(format),
        static_cast<GLenum>(type), static_cast<void*>(pixels));
    LOGI("WebGL readPixels end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::TexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE) || funcArg.InitArgs(NARG_CNT::SIX))) {
        return nullptr;
    }

    bool usagesucc = NVal(env, funcArg[NARG_POS::SIXTH]).TypeIs(napi_number);
    if (usagesucc) {
        bool succ = false;
        LOGI("WebGL texImage2D start");
        int64_t target;
        tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D target = %{public}u", target);
        int64_t level;
        tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D level = %{public}u", level);
        int64_t internalformat;
        tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D internalformat = %{public}u", internalformat);

        int32_t width;
        tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D width = %{public}u", width);
        int64_t height;
        tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D height = %{public}u", height);
        int64_t border;
        tie(succ, border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D border = %{public}u", border);
        int32_t format;
        tie(succ, format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D format = %{public}u", format);
        int64_t type;
        tie(succ, type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D type = %{public}u", type);
        if (funcArg[NARG_POS::NINTH] == nullptr) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D imageSize statrt");
        size_t imageSize;
        void *pixels = nullptr;
        tie(succ, pixels, imageSize) = NVal(env, funcArg[NARG_POS::NINTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D pixels = %{public}u", pixels);
        glTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(internalformat),
            static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLint>(border),
            static_cast<GLenum>(format), static_cast<GLenum>(type), static_cast<void*>(pixels));
        LOGI("WebGL texImage2D end");
        return nullptr;
    } else {
        bool succ = false;
        LOGI("WebGL texImage2D start");
        int64_t target;
        tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D target = %{public}u", target);
        int64_t level;
        tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D level = %{public}u", level);
        int64_t internalformat;
        tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D internalformat = %{public}u", internalformat);
        int32_t format;
        tie(succ, format) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D format = %{public}u", format);
        int64_t type;
        tie(succ, type) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D type = %{public}u", type);
        napi_value teximagesource = funcArg[NARG_POS::SIXTH];
        napi_value resultobject = nullptr;
        napi_status statusobject = napi_coerce_to_object(env, teximagesource, &resultobject);
        if (statusobject != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D resultobject = %{public}u", resultobject);
        napi_value resultwidth = nullptr;
        napi_status widthstatus = napi_get_named_property(env, resultobject, "width", &resultwidth);
        if (widthstatus != napi_ok) {
            return nullptr;
        }
        int64_t width;
        tie(succ, width) = NVal(env, resultwidth).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D width = %{public}u", width);
        napi_value resultheight = nullptr;
        napi_status heightstatus = napi_get_named_property(env, resultobject, "height", &resultheight);
        if (heightstatus != napi_ok) {
            return nullptr;
        }
        int64_t height;
        tie(succ, height) = NVal(env, resultheight).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texImage2D height = %{public}u", height);
        napi_value resultdata = nullptr;
        napi_status datares = napi_get_named_property(env, resultobject, "data", &resultdata);
        if (datares != napi_ok) {
            return nullptr;
        }
        napi_value resultstr;
        napi_status rsuStatus = napi_coerce_to_string(env, resultdata, &resultstr);
        LOGI("WebGL WebGLRenderContext::texImage2D rsuStatus = %{public}u", rsuStatus);
        if (rsuStatus != napi_ok) {
            return nullptr;
        }
        unique_ptr<char[]> name;
        tie(succ, name, ignore) = NVal(env, resultstr).ToUTF8String();
        if (!succ) {
            return nullptr;
        }
        int64_t border = 0;
        glTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(internalformat),
            static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLint>(border),
            static_cast<GLenum>(format), static_cast<GLenum>(type), static_cast<void*>(name.get()));
        LOGI("WebGL texImage2D end");
    }
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::TexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE) || funcArg.InitArgs(NARG_CNT::SEVEN))) {
        return nullptr;
    }
    bool usagesucc = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (usagesucc) {
        bool succ = false;
        LOGI("WebGL texSubImage2D start");
        int64_t target;
        tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D target = %{public}u", target);
        int64_t level;
        tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D level = %{public}u", level);
        int64_t xoffset;
        tie(succ, xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D xoffset = %{public}u", xoffset);
        int64_t yoffset;
        tie(succ, yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D yoffset = %{public}u", yoffset);
        int64_t width;
        tie(succ, width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D width = %{public}u", width);
        int64_t height;
        tie(succ, height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D height = %{public}u", height);
        int64_t format;
        tie(succ, format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D format = %{public}u", format);
        int64_t type;
        tie(succ, type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D type = %{public}u", type);
        if (funcArg[NARG_POS::NINTH] == nullptr) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D texSubImage2D start");
        void  *pixels = nullptr;
        size_t imageSize;
        tie(succ, pixels, imageSize) = NVal(env, funcArg[NARG_POS::NINTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D texSubImage2D = %{public}u", pixels);
        glTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xoffset),
            static_cast<GLint>(yoffset), static_cast<GLsizei>(width), static_cast<GLsizei>(height),
            static_cast<GLenum>(format), static_cast<GLenum>(type), static_cast<void*>(pixels));
        LOGI("WebGL texSubImage2D end");
    } else {
        bool succ = false;
        LOGI("WebGL texSubImage2D start");
        int64_t target;
        tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D target = %{public}u", target);
        int64_t level;
        tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D level = %{public}u", level);
        int64_t xoffset;
        tie(succ, xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D xoffset = %{public}u", xoffset);
        int64_t yoffset;
        tie(succ, yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D yoffset = %{public}u", yoffset);
        int64_t format;
        tie(succ, format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D format = %{public}u", format);
        int64_t type;
        tie(succ, type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D type = %{public}u", type);
        napi_value teximagesource = funcArg[NARG_POS::SEVENTH];
        napi_value resultobject = nullptr;
        napi_status statusobject = napi_coerce_to_object(env, teximagesource, &resultobject);
        if (statusobject != napi_ok) {
            return nullptr;
        }
        napi_value resultwidth = nullptr;
        napi_status widthstatus = napi_get_named_property(env, resultobject, "width", &resultwidth);
        if (widthstatus != napi_ok) {
            return nullptr;
        }
        int64_t width;
        tie(succ, width) = NVal(env, resultwidth).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D width = %{public}u", width);
        napi_value resultheight = nullptr;
        napi_status heightstatus = napi_get_named_property(env, resultobject, "height", &resultheight);
        if (heightstatus != napi_ok) {
            return nullptr;
        }
        int64_t height;
        tie(succ, height) = NVal(env, resultheight).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderContext::texSubImage2D height = %{public}u", height);
        napi_value resultdata = nullptr;
        napi_status datares = napi_get_named_property(env, resultobject, "data", &resultdata);
        if (datares != napi_ok) {
            return nullptr;
        }
        napi_value resultstr;
        napi_status rsuStatus = napi_coerce_to_string(env, resultdata, &resultstr);
        LOGI("WebGL WebGLRenderContext::texSubImage2D rsuStatus = %{public}u", rsuStatus);
        if (rsuStatus != napi_ok) {
            return nullptr;
        }
        unique_ptr<char[]> name;
        tie(succ, name, ignore) = NVal(env, resultstr).ToUTF8String();
        if (!succ) {
            return nullptr;
        }
        glTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xoffset),
            static_cast<GLint>(yoffset), static_cast<GLsizei>(width), static_cast<GLsizei>(height),
            static_cast<GLenum>(format), static_cast<GLenum>(type), static_cast<void*>(name.get()));
        LOGI("WebGL texSubImage2D end");
    }
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform1fv start");
    WebGLUniformLocation *webGLUniformlocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformlocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformlocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv location = %{public}u", location);

    napi_value uniformarray = funcArg[NARG_POS::SECOND];

    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        float* a = nullptr;
        LOGI("WebGL uniform1fv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv count = %{public}u", count);
        float uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv element = %{public}u", element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv ele = %{public}f", (float)ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformfv[i] = (float)ele;
        }
        a = uniformfv;
        glUniform1fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            reinterpret_cast<GLfloat *>(uniformfv));
        LOGI("WebGL uniform1fv uniformarray end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::SECOND]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv count = %{public}u", count);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv type = %{public}u", type);
    if (type == napi_float32_array) {
        float *inputFloat32 = (float *)((float *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniform1fv input_float32_array = %{public}u", inputFloat32);
        glUniform1fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLfloat*>(inputFloat32));
    }
    LOGI("WebGL uniform1fv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform2fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv location = %{public}u", location);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    bool isUniformArray = false;
    float* a = nullptr;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        LOGI("WebGL uniform2fv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv count = %{public}u", count);
        float uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv ele = %{public}f", (float)ele);
            uniformfv[i] = (float)ele;
        }
        a = uniformfv;
        glUniform2fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            reinterpret_cast<GLfloat *>(uniformfv));
        LOGI("WebGL uniform2fv uniformarray end");
        return nullptr;
    }

    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::SECOND]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv count = %{public}u", count);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv type = %{public}u", type);
    if (type == napi_float32_array) {
        float *inputFloat32 = (float *)((float *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv input_float32_array = %{public}u", inputFloat32);
        glUniform2fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLfloat*>(inputFloat32));
    }
    LOGI("WebGL uniform2fv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform3fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3fv location = %{public}u", location);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        LOGI("WebGL uniform3fv is isUniformArray");
        float* a = nullptr;
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniform3fv count = %{public}u", count);
        float uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniform3fv ele = %{public}f", (float)ele);
            uniformfv[i] = (float)ele;
        }
        a = uniformfv;
        glUniform3fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            reinterpret_cast<GLfloat *>(uniformfv));
        LOGI("WebGL uniform3fv uniformarray end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3fv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::SECOND]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3fv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3fv count = %{public}u", count);
    if (type == napi_float32_array) {
        float *inputFloat32 = (float *)((float *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniform3fv input_float32_array = %{public}u", inputFloat32);
        glUniform3fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLfloat*>(inputFloat32));
    }
    LOGI("WebGL uniform3fv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
            return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform4fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform4fv location = %{public}u", location);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        LOGI("WebGL uniform4fv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniform4fv count = %{public}u", count);
        float* a = nullptr;
        float uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniform4fv ele = %{public}f", (float)ele);
            uniformfv[i] = (float)ele;
        }
        a = uniformfv;
        glUniform4fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            reinterpret_cast<GLfloat *>(uniformfv));
        LOGI("WebGL uniform4fv uniformarray end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform4fv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::SECOND]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniform4fv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform4fv count = %{public}u", count);
    if (type == napi_float32_array) {
        float *inputFloat32 = (float *)((float *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniform4fv input_float32_array = %{public}u", inputFloat32);
        glUniform4fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLfloat*>(inputFloat32));
    }
    LOGI("WebGL uniform4fv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform1iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform1iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1iv location = %{public}u", location);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        int* a = nullptr;
        LOGI("WebGL uniform1iv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniform1iv count = %{public}u", count);
        int uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status doubleStatus = napi_get_value_int64(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniform1iv ele = %{public}f", (float)ele);
            uniformfv[i] = (int)ele;
        }
        a = uniformfv;
        glUniform1iv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            reinterpret_cast<GLint *>(a));
        LOGI("WebGL uniform1iv uniformarray end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1iv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::SECOND]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1iv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform1iv count = %{public}u", count);
    if (type == napi_int32_array) {
        int *intInt32 = (int *)((int *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniform1iv int_int32_array = %{public}u", intInt32);
        glUniform1iv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLint*>(intInt32));
    }
    LOGI("WebGL uniform1iv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform2iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform2iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2iv location = %{public}u", location);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        LOGI("WebGL uniform2iv is isUniformArray");
        int* a = nullptr;
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniform2iv count = %{public}u", count);
        int uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status doubleStatus = napi_get_value_int64(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniform2iv ele = %{public}d", ele);
            uniformfv[i] = (int)ele;
        }
        a = uniformfv;
        glUniform2iv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            reinterpret_cast<GLint *>(a));
        LOGI("WebGL uniform2iv uniformarray end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2iv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::SECOND]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2iv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2iv count = %{public}u", count);
    if (type == napi_int32_array) {
        int *intInt32 = (int *)((int *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniform2iv int_int32_array = %{public}u", intInt32);
        glUniform2iv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLint*>(intInt32));
    }
    LOGI("WebGL uniform2iv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform3iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform3iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3iv location = %{public}u", location);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        int* a = nullptr;
        LOGI("WebGL uniform3iv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniform3iv count = %{public}u", count);
        int uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status doubleStatus = napi_get_value_int64(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniform3iv ele = %{public}f", (float)ele);
            uniformfv[i] = (int)ele;
        }
        a = uniformfv;
        glUniform3iv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            reinterpret_cast<GLint *>(a));
        LOGI("WebGL uniform3iv uniformarray end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3iv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::SECOND]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3iv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3iv count = %{public}u", count);
    if (type == napi_int32_array) {
        int *intInt32 = (int *)((int *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniform3iv int_int32_array = %{public}u", intInt32);
        glUniform3iv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLint*>(intInt32));
    }
    LOGI("WebGL uniform3iv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform4iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform4iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform4iv location = %{public}u", location);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        int* a = nullptr;
        LOGI("WebGL uniform4iv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniform4iv count = %{public}u", count);
        int uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status doubleStatus = napi_get_value_int64(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniform4iv ele = %{public}f", (float)ele);
            uniformfv[i] = (int)ele;
        }
        a = uniformfv;
        glUniform4iv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            reinterpret_cast<GLint *>(a));
        LOGI("WebGL uniform4iv uniformarray end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform4iv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::SECOND]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniform4iv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniform4iv count = %{public}u", count);
    if (type == napi_int32_array) {
        int *intInt32 = (int *)((int *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniform4iv int_int32_array = %{public}u", intInt32);
        glUniform4iv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLint*>(intInt32));
    }
    LOGI("WebGL uniform4iv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::UniformMatrix2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
    return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniformMatrix2fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv location = %{public}d", location);
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv succ = %{public}u", succ);
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv transpose = %{public}u", transpose);
    napi_value uniformarray = funcArg[NARG_POS::THIRD];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        float* a = nullptr;
        LOGI("WebGL uniformMatrix2fv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv count = %{public}u", count);
        float uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv ele = %{public}f", (float)ele);
            uniformfv[i] = (float)ele;
        }
        a = uniformfv;
        glUniformMatrix2fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLboolean>(transpose), static_cast<GLfloat*>(a));
        LOGI("WebGL uniformMatrix2fv uniformarray end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, uniformarray).IsTypeArray();
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv isTypedarray = %{public}u", isTypedarray);
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::THIRD]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv count = %{public}u", count);
    if (type == napi_float32_array) {
        float *inputFloat32 = (float *)((float *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix2fv aint32 = %{public}u", inputFloat32);
        glUniformMatrix2fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLboolean>(transpose), static_cast<GLfloat*>(inputFloat32));
    }
    LOGI("WebGL uniformMatrix2fv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::UniformMatrix3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniformMatrix3fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix3fv location = %{public}d", location);
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix3fv transpose = %{public}u", transpose);
    napi_value uniformarray = funcArg[NARG_POS::THIRD];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        float* a = nullptr;
        LOGI("WebGL uniformMatrix3fv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix3fv count = %{public}u", count);
        float uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix3fv ele = %{public}f", (float)ele);
            uniformfv[i] = (float)ele;
        }
        a = uniformfv;
        glUniformMatrix3fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLboolean>(transpose),  static_cast<GLfloat*>(a));
        LOGI("WebGL uniformMatrix3fv uniformarray end");
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::THIRD]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix3fv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix3fv count = %{public}u", count);
    if (type == napi_float32_array) {
        float *inputFloat32 = (float *)((float *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix3fv aint32 = %{public}u", inputFloat32);
        glUniformMatrix3fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLboolean>(transpose), static_cast<GLfloat*>(inputFloat32));
    }
    LOGI("WebGL uniformMatrix3fv end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::UniformMatrix4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniformMatrix4fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix4fv location = %{public}u", location);
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix4fv transpose = %{public}u", transpose);
    napi_value uniformarray = funcArg[NARG_POS::THIRD];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        float* a = nullptr;
        LOGI("WebGL glUniformMatrix4fv is isUniformArray");
        uint32_t count;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL WebGLRenderingContextOverloads::glUniformMatrix4fv count = %{public}u", count);
        float uniformfv[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL WebGLRenderingContextOverloads::glUniformMatrix4fv ele = %{public}f", (float)ele);
            uniformfv[i] = (float)ele;
        }
        a = uniformfv;
        glUniformMatrix4fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLboolean>(transpose), static_cast<GLfloat*>(a));
        LOGI("WebGL glUniformMatrix4fv uniformarray end");
        return nullptr;
    }
    void *value = nullptr;
    size_t count;
    size_t byteOffset;
    napi_typedarray_type type;
    tie(succ, value, count, byteOffset, type)  = NVal(env, funcArg[NARG_POS::THIRD]).ToTypedArrayInfo();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextOverloads::glUniformMatrix4fv value = %{public}u", value);
    LOGI("WebGL WebGLRenderingContextOverloads::glUniformMatrix4fv count = %{public}u", count);
    if (type == napi_float32_array) {
        float *inputFloat32 = (float *)((float *)value - byteOffset);
        LOGI("WebGL WebGLRenderingContextOverloads::uniformMatrix4fv aint32 = %{public}u", inputFloat32);
        glUniformMatrix4fv(static_cast<GLint>(location), static_cast<GLsizei>(count),
            static_cast<GLboolean>(transpose), static_cast<GLfloat*>(inputFloat32));
    }
    LOGI("WebGL uniformMatrix4fv end");
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
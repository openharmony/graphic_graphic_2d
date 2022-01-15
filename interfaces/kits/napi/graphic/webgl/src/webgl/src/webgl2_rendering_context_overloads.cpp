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
#include "../include/context/webgl2_rendering_context_overloads.h"
#include "../include/webgl/webgl_uniform_location.h"
#include "../../common/napi/n_func_arg.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/util/log.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGL2RenderingContextOverloads::BufferData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    LOGI("WebGL2 bufferData start");
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::bufferData target = %{public}u", target);
    GLvoid *data = nullptr;
    int64_t size = 0;
    bool usageSucc = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_number);
    if (usageSucc) {
        LOGI("WebGL2 WebGL2RenderContext::bufferData usageSucc");
        tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::bufferData size = %{public}u", size);
    } else {
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::FOURTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderingContextBase::bufferData srcOffset = %{public}u", srcOffset);
        }
        int64_t length;
        if (funcArg[NARG_POS::FIFTH] != nullptr) {
            tie(succ, length) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderingContextBase::bufferData length = %{public}u", length);
        }
        tie(succ, data, length) = NVal(env, funcArg[NARG_POS::SECOND]).ToArraybuffer();
        if (!succ) {
            tie(succ, data, length) = NVal(env, funcArg[NARG_POS::SECOND]).ToDataview();
            if (!succ) {
                return nullptr;
            }
        }
        char *srcDataBase = static_cast<char *>(data);
        data = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
        LOGI("WebGL2 WebGL2RenderContext::bufferData data = %{public}u", data);
    }
    int64_t usage;
    tie(succ, usage) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::bufferData usage = %{public}u", usage);
    glBufferData(static_cast<GLenum>(target), static_cast<GLsizeiptr>(size),
                 static_cast<GLvoid *>(data), static_cast<GLenum>(usage));
    LOGI("WebGL2 bufferData end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::BufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 bufferSubData start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::bufferSubData target = %{public}u", target);
    int64_t dstByteOffset;
    tie(succ, dstByteOffset) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::bufferSubData dstByteOffset = %{public}u", dstByteOffset);
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::bufferSubData srcOffset = %{public}u", srcOffset);
    }
    int64_t length;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::bufferSubData length = %{public}u", length);
    }
    GLvoid *srcData = nullptr;
    tie(succ, srcData, length) = NVal(env, funcArg[NARG_POS::THIRD]).ToArraybuffer();
    if (!succ) {
        tie(succ, srcData, length) = NVal(env, funcArg[NARG_POS::THIRD]).ToDataview();
        if (!succ) {
            return nullptr;
        }
    }
    char *srcDataBase = static_cast<char *>(srcData);
    srcData = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
    LOGI("WebGL2 WebGL2RenderContext::bufferSubData srcData = %{public}u", srcData);
    glBufferSubData(static_cast<GLenum>(target), static_cast<GLintptr>(dstByteOffset),
                    static_cast<GLsizeiptr>(length), reinterpret_cast<GLvoid *>(srcData));
    LOGI("WebGL2 bufferSubData end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::TexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE, NARG_CNT::TEN) || funcArg.InitArgs(NARG_CNT::SIX))) {
        return nullptr;
    }

    bool sixSucc = NVal(env, funcArg[NARG_POS::SIXTH]).TypeIs(napi_number);
    if (sixSucc) {
        LOGI("WebGL2 texImage2D start");
        bool succ = false;
        int64_t target;
        tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D target = %{public}u", target);
        int64_t level;
        tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D level = %{public}u", level);
        int64_t internalFormat;
        tie(succ, internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D internalFormat = %{public}u", internalFormat);
        int64_t width;
        tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D width = %{public}u", width);
        int64_t height;
        tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D height = %{public}u", height);
        int64_t border;
        tie(succ, border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D border = %{public}u", border);
        int64_t format;
        tie(succ, format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D format = %{public}u", format);
        int64_t type;
        tie(succ, type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D type = %{public}u", type);
        size_t size;
        GLvoid *srcData = nullptr;
        bool usageSucc = NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number);
        if (usageSucc) {
            int64_t pboOffset;
            tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::texImage2D pboOffset = %{public}u", pboOffset);
            char *srcDataBase = static_cast<char *>(srcData);
            srcData = reinterpret_cast<GLvoid *>((srcDataBase + pboOffset));
            LOGI("WebGL2 WebGL2RenderContext::texImage2D srcData = %{public}u", srcData);
        } else {
            tie(succ, srcData, size) = NVal(env, funcArg[NARG_POS::NINTH]).ToDataview();
            LOGI("WebGL2 WebGL2RenderContext::texImage2D succ = %{public}u", succ);
            if (!succ) {
                napi_value texImageSource = funcArg[NARG_POS::NINTH];
                napi_value resultObject = nullptr;
                napi_status statusObject = napi_coerce_to_object(env, texImageSource, &resultObject);
                if (statusObject != napi_ok) {
                    return nullptr;
                }
                napi_value resultWidth = nullptr;
                LOGI("WebGL2 WebGLRenderContext::texImage2D statusObject = %{public}u", resultObject);
                napi_status widthStatus = napi_get_named_property(env, resultObject, "width", &resultWidth);
                if (widthStatus != napi_ok) {
                    return nullptr;
                }
                int64_t srcWidth;
                tie(succ, srcWidth) = NVal(env, resultWidth).ToInt64();
                if (!succ) {
                    return nullptr;
                }
                LOGI("WebGL2 WebGLRenderContext::texImage2D srcWidth = %{public}u", srcWidth);
                napi_value resultHeight = nullptr;
                napi_status heightStatus = napi_get_named_property(env, resultObject, "height", &resultHeight);
                if (heightStatus != napi_ok) {
                    return nullptr;
                }
                int64_t srcHeight;
                tie(succ, srcHeight) = NVal(env, resultHeight).ToInt64();
                if (!succ) {
                    return nullptr;
                }
                LOGI("WebGL2 WebGLRenderContext::texImage2D srcHeight = %{public}u", srcHeight);
                napi_value resultData = nullptr;
                napi_status dataResult = napi_get_named_property(env, resultObject, "data", &resultData);
                if (dataResult != napi_ok) {
                    return nullptr;
                }
                napi_value resultStr;
                napi_status resultStatus = napi_coerce_to_string(env, resultData, &resultStr);
                LOGI("WebGL2 WebGLRenderContext::texImage2D resultStatus = %{public}u", resultStatus);
                if (resultStatus != napi_ok) {
                    return nullptr;
                }
                unique_ptr<char[]> name;
                tie(succ, name, ignore) = NVal(env, resultStr).ToUTF8String();
                if (!succ) {
                    return nullptr;
                }
                glTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                             static_cast<GLint>(internalFormat), static_cast<GLsizei>((width != 0) ? width : srcWidth),
                             static_cast<GLsizei>((height != 0) ? height : srcHeight), static_cast<GLint>(border),
                             static_cast<GLenum>(format), static_cast<GLenum>(type),
                             reinterpret_cast<GLvoid *>(name.get()));
                LOGI("WebGL2 texImage2D end");
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::texImage2D srcData = %{public}u", srcData);
            int64_t srcOffset = 0;
            if (funcArg[NARG_POS::TENTH] != nullptr) {
                LOGI("WebGL2 WebGL2RenderContext::texImage2D into 10");
                tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
                if (!succ) {
                    return nullptr;
                }
                LOGI("WebGL2 WebGL2RenderContext::texImage2D srcOffset = %{public}u", srcOffset);
            }
            char *srcDataBase = static_cast<char *>(srcData);
            srcData = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
            LOGI("WebGL2 WebGL2RenderContext::texImage2D srcData = %{public}u", srcData);
        }
        glTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                     static_cast<GLint>(internalFormat), static_cast<GLsizei>(width),
                     static_cast<GLsizei>(height), static_cast<GLint>(border),
                     static_cast<GLenum>(format), static_cast<GLenum>(type),
                     reinterpret_cast<GLvoid *>(srcData));
        LOGI("WebGL2 texImage2D end");
    } else {
        LOGI("WebGL2 texImage2D start");
        bool succ = false;
        int64_t target;
        tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D target = %{public}u", target);
        int64_t level;
        tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texImage2D level = %{public}u", level);
        int64_t internalformat;
        tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        if (funcArg.InitArgs(NARG_CNT::SIX)) {
            int64_t format;
            tie(succ, format) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::texImage2D format = %{public}u", format);
            int64_t type;
            tie(succ, type) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::texImage2D type = %{public}u", type);
            napi_value texImageSource = funcArg[NARG_POS::SIXTH];
            napi_value resultObject = nullptr;
            napi_status statusObject = napi_coerce_to_object(env, texImageSource, &resultObject);
            if (statusObject != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL2 WebGLRenderContext::texImage2D resultobject = %{public}u", resultObject);
            napi_value resultWidth = nullptr;
            napi_status widthStatus = napi_get_named_property(env, resultObject, "width", &resultWidth);
            if (widthStatus != napi_ok) {
                return nullptr;
            }
            int64_t width;
            tie(succ, width) = NVal(env, resultWidth).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGLRenderContext::texImage2D width = %{public}u", width);
            napi_value resultHeight = nullptr;
            napi_status heightStatus = napi_get_named_property(env, resultObject, "height", &resultHeight);
            if (heightStatus != napi_ok) {
                return nullptr;
            }
            int64_t height;
            tie(succ, height) = NVal(env, resultHeight).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGLRenderContext::texImage2D height = %{public}u", height);
            napi_value resultData = nullptr;
            napi_status dataResult = napi_get_named_property(env, resultObject, "data", &resultData);
            if (dataResult != napi_ok) {
                return nullptr;
            }
            napi_value resultStr;
            napi_status resultStatus = napi_coerce_to_string(env, resultData, &resultStr);
            LOGI("WebGL2 WebGLRenderContext::texImage2D resultStatus = %{public}u", resultStatus);
            if (resultStatus != napi_ok) {
                return nullptr;
            }
            unique_ptr<char[]> name;
            tie(succ, name, ignore) = NVal(env, resultStr).ToUTF8String();
            if (!succ) {
                return nullptr;
            }
            int64_t border = 0;
            glTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                         static_cast<GLint>(internalformat), static_cast<GLsizei>(width),
                         static_cast<GLsizei>(height), static_cast<GLint>(border),
                         static_cast<GLenum>(format), static_cast<GLenum>(type),
                         reinterpret_cast<GLvoid *>(name.get()));
            LOGI("WebGL2 texImage2D end");
        }
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::TexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE, NARG_CNT::TEN) || funcArg.InitArgs(NARG_CNT::SEVEN))) {
        return nullptr;
    }
    bool sevenSucc = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (sevenSucc) {
        LOGI("WebGL2 texSubImage2D start");
        bool succ = false;
        int64_t target;
        tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D target = %{public}u", target);
        int64_t level;
        tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D level = %{public}u", level);
        int64_t xOffset;
        tie(succ, xOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D xOffset = %{public}u", xOffset);
        int64_t yOffset;
        tie(succ, yOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D yOffset = %{public}u", yOffset);
        int64_t width;
        tie(succ, width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D width = %{public}u", width);
        int64_t height;
        tie(succ, height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D height = %{public}u", height);
        int64_t format;
        tie(succ, format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D format = %{public}u", format);
        int64_t type;
        tie(succ, type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D type = %{public}u", type);
        size_t size;
        void *srcData = nullptr;
        bool usageSucc = NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number);
        if (usageSucc) {
            int64_t pboOffset;
            tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::texSubImage2D pboOffset = %{public}u", pboOffset);
            char *srcDataBase = static_cast<char *>(srcData);
            srcData = reinterpret_cast<GLvoid *>((srcDataBase + pboOffset));
            LOGI("WebGL2 WebGL2RenderContext::texSubImage2D srcData = %{public}u", srcData);
        } else {
            tie(succ, srcData, size) = NVal(env, funcArg[NARG_POS::NINTH]).ToDataview();
            if (!succ) {
                napi_value texImageSource = funcArg[NARG_POS::NINTH];
                napi_value resultObject = nullptr;
                napi_status statusObject = napi_coerce_to_object(env, texImageSource, &resultObject);
                if (statusObject != napi_ok) {
                    return nullptr;
                }
                LOGI("WebGL2 WebGLRenderContext::texSubImage2D resultobject = %{public}u", resultObject);
                napi_value resultWidth = nullptr;
                napi_status widthStatus = napi_get_named_property(env, resultObject, "width", &resultWidth);
                if (widthStatus != napi_ok) {
                    return nullptr;
                }
                int64_t srcWidth;
                tie(succ, srcWidth) = NVal(env, resultWidth).ToInt64();
                if (!succ) {
                    return nullptr;
                }
                LOGI("WebGL2 WebGLRenderContext::texSubImage2D srcWidth = %{public}u", srcWidth);
                napi_value resultHeight = nullptr;
                napi_status heigthStatus = napi_get_named_property(env, resultObject, "height", &resultHeight);
                if (heigthStatus != napi_ok) {
                    return nullptr;
                }
                int64_t srcHeight;
                tie(succ, srcHeight) = NVal(env, resultHeight).ToInt64();
                if (!succ) {
                    return nullptr;
                }
                LOGI("WebGL2 WebGLRenderContext::texSubImage2D srcHeight = %{public}u", srcHeight);
                napi_value resultData = nullptr;
                napi_status dataResult = napi_get_named_property(env, resultObject, "data", &resultData);
                if (dataResult != napi_ok) {
                    return nullptr;
                }
                napi_value resultStr;
                napi_status resultStatus = napi_coerce_to_string(env, resultData, &resultStr);
                LOGI("WebGL2 WebGLRenderContext::texSubImage2D resultStatus = %{public}u", resultStatus);
                if (resultStatus != napi_ok) {
                    return nullptr;
                }
                unique_ptr<char[]> name;
                tie(succ, name, ignore) = NVal(env, resultStr).ToUTF8String();
                if (!succ) {
                    return nullptr;
                }
                glTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                                static_cast<GLint>(xOffset), static_cast<GLint>(yOffset),
                                static_cast<GLsizei>((width != 0) ? width : srcWidth),
                                static_cast<GLsizei>((height != 0) ? height : srcHeight),
                                static_cast<GLenum>(format), static_cast<GLenum>(type),
                                reinterpret_cast<GLvoid *>(name.get()));
                LOGI("WebGL2 texSubImage2D end");
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::texSubImage2D srcData = %{public}u", srcData);
            int64_t srcOffset = 0;
            if (funcArg[NARG_POS::TENTH] != nullptr) {
                tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
                if (!succ) {
                    return nullptr;
                }
                LOGI("WebGL2 WebGL2RenderContext::texSubImage2D srcOffset = %{public}u", srcOffset);
            }
            char *srcDataBase = static_cast<char *>(srcData);
            srcData = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
            LOGI("WebGL2 WebGL2RenderContext::texSubImage2D srcData = %{public}u", srcData);
        }
        glTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                        static_cast<GLint>(xOffset), static_cast<GLint>(yOffset),
                        static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                        static_cast<GLenum>(format), static_cast<GLenum>(type),
                        reinterpret_cast<GLvoid *>(srcData));
        LOGI("WebGL2 texSubImage2D end");
    } else {
        LOGI("WebGL2 texSubImage2D start");
        bool succ = false;
        int64_t target;
        tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D target = %{public}u", target);
        int64_t level;
        tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D level = %{public}u", level);
        int64_t xOffset;
        tie(succ, xOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D xOffset = %{public}u", xOffset);
        int64_t yOffset;
        tie(succ, yOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::texSubImage2D yOffset = %{public}u", yOffset);
        if (funcArg.InitArgs(NARG_CNT::SEVEN)) {
            int64_t format;
            tie(succ, format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::texSubImage2D format = %{public}u", format);
            int64_t type;
            tie(succ, type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::texSubImage2D type = %{public}u", type);
            napi_value texImageSource = funcArg[NARG_POS::SEVENTH];
            napi_value resultObject = nullptr;
            napi_status statusObject = napi_coerce_to_object(env, texImageSource, &resultObject);
            if (statusObject != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL2 WebGLRenderContext::texSubImage2D resultObject = %{public}u", resultObject);
            napi_value resultWidth = nullptr;
            napi_status widthStatus = napi_get_named_property(env, resultObject, "width", &resultWidth);
            if (widthStatus != napi_ok) {
                return nullptr;
            }
            int64_t width;
            tie(succ, width) = NVal(env, resultWidth).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGLRenderContext::texSubImage2D width = %{public}u", width);
            napi_value resultHeight = nullptr;
            napi_status heigthStatus = napi_get_named_property(env, resultObject, "height", &resultHeight);
            if (heigthStatus != napi_ok) {
                return nullptr;
            }
            int64_t height;
            tie(succ, height) = NVal(env, resultHeight).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGLRenderContext::texSubImage2D height = %{public}u", height);
            napi_value resultData = nullptr;
            napi_status dataResult = napi_get_named_property(env, resultObject, "data", &resultData);
            if (dataResult != napi_ok) {
                return nullptr;
            }
            napi_value resultStr;
            napi_status resultStatus = napi_coerce_to_string(env, resultData, &resultStr);
            LOGI("WebGL2 WebGLRenderContext::texSubImage2D resultStatus = %{public}u", resultStatus);
            if (resultStatus != napi_ok) {
                return nullptr;
            }
            unique_ptr<char[]> name;
            tie(succ, name, ignore) = NVal(env, resultStr).ToUTF8String();
            if (!succ) {
                return nullptr;
            }
            glTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                            static_cast<GLint>(xOffset), static_cast<GLint>(yOffset),
                            static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                            static_cast<GLenum>(format), static_cast<GLenum>(type),
                            reinterpret_cast<GLvoid *>(name.get()));
            LOGI("WebGL2 texSubImage2D end");
        }
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::CompressedTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN, NARG_CNT::NINE)) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexImage2D start");
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D target = %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D level = %{public}u", level);
    int64_t internalFormat;
    tie(succ, internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D internalFormat = %{public}u", internalFormat);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D height = %{public}u", height);
    int64_t border;
    tie(succ, border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D border = %{public}u", border);
    GLvoid *srcData = nullptr;
    int64_t imageSize = 0;
    bool usageSucc = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (usageSucc) {
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D imageSize = %{public}u", imageSize);
        int64_t offset;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D offset = %{public}u", offset);
        char *srcDataBase = static_cast<char *>(srcData);
        srcData = reinterpret_cast<GLvoid *>((srcDataBase + offset));
    } else {
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::NINTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderingContextOverloads::compressedTexSubImage2D srcOffset = %{public}u", srcOffset);
        }
        int64_t srcLengthOverride;
        if (funcArg[NARG_POS::TENTH] != nullptr) {
            tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("compressedTexSubImage2D srcLengthOverride = %{public}u", srcLengthOverride);
        }
        tie(succ, srcData, srcLengthOverride) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
        char *srcDataBase = static_cast<char *>(srcData);
        srcData = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
        LOGI("WebGL2 WebGL2RenderContext::compressedTexImage2D srcData = %{public}u", srcData);
    }
    glCompressedTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                           static_cast<GLenum>(internalFormat), static_cast<GLsizei>(width),
                           static_cast<GLsizei>(height), static_cast<GLint>(border),
                           static_cast<GLsizei>(imageSize), reinterpret_cast<GLvoid *>(srcData));
    LOGI("WebGL2 compressedTexImage2D end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::CompressedTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT, NARG_CNT::TEN)) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D start");
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D target = %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D level = %{public}u", level);
    int64_t xOffset;
    tie(succ, xOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D xOffset = %{public}u", xOffset);
    int64_t yOffset;
    tie(succ, yOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D yOffset = %{public}u", yOffset);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D height = %{public}u", height);
    int64_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D format = %{public}u", format);
    int64_t imageSize = 0;
    GLvoid *srcData = nullptr;
    bool usageSucc = NVal(env, funcArg[NARG_POS::EIGHTH]).TypeIs(napi_number);
    if (usageSucc) {
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D imageSize = %{public}u", imageSize);
        int64_t offset;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D offset = %{public}u", offset);
        char *srcDataBase = static_cast<char *>(srcData);
        srcData = reinterpret_cast<GLvoid *>((srcDataBase + offset));
        LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D srcData = %{public}u", srcData);
    } else {
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::NINTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderingContextOverloads::compressedTexSubImage2D srcOffset = %{public}u", srcOffset);
        }
        int64_t srcLength = 0;
        if (funcArg[NARG_POS::TENTH] != nullptr) {
            tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderingContextOverloads::compressedTexSubImage2D srcLength = %{public}u", srcLength);
        }
        tie(succ, srcData, srcLength) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
        char *srcDataBase = static_cast<char *>(srcData);
        srcData = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
        LOGI("WebGL2 WebGL2RenderContext::compressedTexSubImage2D srcData = %{public}u", srcData);
    }
    glCompressedTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                              static_cast<GLint>(xOffset), static_cast<GLint>(yOffset),
                              static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                              static_cast<GLenum>(format), static_cast<GLsizei>(imageSize),
                              reinterpret_cast<GLvoid *>(srcData));
    LOGI("WebGL2 compressedTexSubImage2D end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform1fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform1fv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform1fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform1fv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform1fv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniform1fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniform1fv[i] = (float)ele;
        }
        glUniform1fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLfloat*>(uniform1fv + srcOffset));
        LOGI("WebGL2 uniform1fv end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_float32_array) {
        float* inputFloat = (float*)((uint8_t*)(data) + srcOffset);
        glUniform1fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLfloat*>(inputFloat));
        LOGI("WebGL2 uniform1fv end");
        return nullptr;
    }
    LOGI("WebGL2 uniform1fv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform2fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform2fv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform2fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::length length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform2fv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniform2fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniform2fv[i] = (float)ele;
        }
        glUniform2fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLfloat*>(uniform2fv + srcOffset));
        LOGI("WebGL2 uniform2fv end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_float32_array) {
        float* inputFloat = (float*)((uint8_t*)(data) + srcOffset);
        glUniform2fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLfloat*>(inputFloat));
        LOGI("WebGL2 uniform2fv end");
        return nullptr;
    }
    LOGI("WebGL2 uniform2fv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform3fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform3fv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform3fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform3fv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform3fv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniform3fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniform3fv[i] = (float)ele;
        }
        glUniform3fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLfloat*>(uniform3fv + srcOffset));
        LOGI("WebGL2 uniform3fv end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_float32_array) {
        float* inputFloat = (float*)((uint8_t*)(data) + srcOffset);
        glUniform3fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLfloat*>(inputFloat));
        LOGI("WebGL2 uniform3fv end");
        return nullptr;
    }
    LOGI("WebGL2 uniform3fv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform4fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform4fv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform4fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform4fv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform4fv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniform4fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniform4fv[i] = (float)ele;
        }
        glUniform4fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLfloat*>(uniform4fv + srcOffset));
        LOGI("WebGL2 uniform4fv end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_float32_array) {
        float* inputFloat = (float*)((uint8_t*)(data) + srcOffset);
        glUniform4fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLfloat*>(inputFloat));
        LOGI("WebGL2 uniform4fv end");
        return nullptr;
    }
    LOGI("WebGL2 uniform4fv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform1iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform1iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform1iv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform1iv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform1iv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform1iv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        int64_t uniform1iv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status intStatus = napi_get_value_int64(env, element, &ele);
            if (intStatus != napi_ok) {
                return nullptr;
            }
            uniform1iv[i] = ele;
        }
        glUniform1iv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLint *>(uniform1iv + srcOffset));
        LOGI("WebGL2 uniform1iv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_int8_array) {
        int8_t* inputInt8 = (int8_t*)((uint8_t*)(data) + srcOffset);
        glUniform1iv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLint*>(inputInt8));
        LOGI("WebGL2 uniform1iv end");
        return nullptr;
    }
    LOGI("WebGL2 uniform1iv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform2iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform2iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform2iv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform2iv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform2iv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform2iv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        int64_t uniform2iv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status intStatus = napi_get_value_int64(env, element, &ele);
            if (intStatus != napi_ok) {
                return nullptr;
            }
            uniform2iv[i] = ele;
        }
        glUniform2iv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLint *>(uniform2iv + srcOffset));
        LOGI("WebGL2 uniform2iv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_int8_array) {
        int8_t* inputInt8 = (int8_t*)((uint8_t*)(data) + srcOffset);
        glUniform2iv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLint*>(inputInt8));
        LOGI("WebGL2 uniform2iv end");
        return nullptr;
    }
    LOGI("WebGL2 uniform2iv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform3iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform3iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform3iv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform3iv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform3iv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform3iv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        int64_t uniform3iv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status intStatus = napi_get_value_int64(env, element, &ele);
            if (intStatus != napi_ok) {
                return nullptr;
            }
            uniform3iv[i] = ele;
        }
        glUniform3iv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLint *>(uniform3iv + srcOffset));
        LOGI("WebGL2 uniform3iv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_int8_array) {
        int8_t* inputInt8 = (int8_t*)((uint8_t*)(data) + srcOffset);
        glUniform3iv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLint*>(inputInt8));
        LOGI("WebGL2 uniform3iv end");
        return nullptr;
    }
    LOGI("WebGL2 uniform3iv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform4iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform4iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform4iv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform4iv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform4iv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform4iv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        int64_t uniform4iv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status intStatus = napi_get_value_int64(env, element, &ele);
            if (intStatus != napi_ok) {
                return nullptr;
            }
            uniform4iv[i] = ele;
        }
        glUniform4iv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLint *>(uniform4iv + srcOffset));
        LOGI("WebGL2 uniform4iv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_int8_array) {
        int8_t* inputInt8 = (int8_t*)((uint8_t*)(data) + srcOffset);
        glUniform4iv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                     reinterpret_cast<GLint*>(inputInt8));
        LOGI("WebGL2 uniform4iv end");
        return nullptr;
    }
    LOGI("WebGL2 uniform4iv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::UniformMatrix2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniformMatrix2fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderContext::uniformMatrix2fv locationId = %{public}u", locationId);
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::uniformMatrix2fv transpose = %{public}u", transpose);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniformMatrix2fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniformMatrix2fv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix2fv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix2fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix2fv[i] = (float)ele;
        }
        glUniformMatrix2fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                           static_cast<GLboolean>(transpose),
                           reinterpret_cast<GLfloat *>(uniformMatrix2fv + srcOffset));
        LOGI("WebGL2 uniformMatrix2fv end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_float32_array) {
        float* inputFloat = (float*)((uint8_t*)(data) + srcOffset);
        glUniformMatrix2fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                           static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat *>(inputFloat));
        LOGI("WebGL2 uniformMatrix2fv end");
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix2fv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::UniformMatrix3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniformMatrix3fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderContext::uniformMatrix3fv locationId = %{public}u", locationId);
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::uniformMatrix3fv transpose = %{public}u", transpose);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniformMatrix3fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniformMatrix3fv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix3fv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix3fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix3fv[i] = (float)ele;
        }
        glUniformMatrix3fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                           static_cast<GLboolean>(transpose),
                           reinterpret_cast<GLfloat *>(uniformMatrix3fv + srcOffset));
        LOGI("WebGL2 uniformMatrix3fv end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_float32_array) {
        float* inputFloat = (float*)((uint8_t*)(data) + srcOffset);
        glUniformMatrix3fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                           static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat *>(inputFloat));
        LOGI("WebGL2 uniformMatrix3fv end");
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix3fv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::UniformMatrix4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniformMatrix4fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderContext::uniformMatrix4fv locationId = %{public}u", locationId);
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::uniformMatrix4fv transpose = %{public}u", transpose);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniformMatrix4fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderingContextOverloads::uniformMatrix4fv length = %{public}u", length);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix4fv is Array");
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix4fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element = nullptr;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix4fv[i] = (float)ele;
        }
        glUniformMatrix4fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                           static_cast<GLboolean>(transpose),
                           reinterpret_cast<GLfloat *>(uniformMatrix4fv + srcOffset));
        LOGI("WebGL2 uniformMatrix4fv end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (type == napi_float32_array) {
        float* inputFloat = (float*)((uint8_t*)(data) + srcOffset);
        glUniformMatrix4fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
                           static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat *>(inputFloat));
        LOGI("WebGL2 uniformMatrix4fv end");
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix4fv end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::ReadPixels(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN, NARG_CNT::EIGHT)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 readPixels start");
    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::readPixels x = %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::readPixels y = %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::readPixels width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::readPixels height = %{public}u", height);
    int64_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::readPixels format = %{public}u", format);
    int64_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 WebGL2RenderContext::readPixels type = %{public}u", type);
    GLvoid *data = nullptr;
    bool usageSucc = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (usageSucc) {
        int64_t offset;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 WebGL2RenderContext::readPixels offset = %{public}u", offset);
        char *srcDataBase = static_cast<char *>(data);
        data = reinterpret_cast<GLvoid *>((srcDataBase + offset));
    } else {
        size_t size;
        tie(succ, data, size) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
        int64_t dstOffset = 0;
        if (funcArg[NARG_POS::EIGHTH] != nullptr) {
            tie(succ, dstOffset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderContext::readPixels dstOffset = %{public}u", dstOffset);
        }
        char *srcDataBase = static_cast<char *>(data);
        data = reinterpret_cast<GLvoid *>((srcDataBase + dstOffset));
        LOGI("WebGL2 WebGL2RenderContext::readPixels data = %{public}u", data);
    }
    glReadPixels(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height), static_cast<GLenum>(format),
                 static_cast<GLenum>(type), reinterpret_cast<GLvoid *>(data));
    LOGI("WebGL2 readPixels end");
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
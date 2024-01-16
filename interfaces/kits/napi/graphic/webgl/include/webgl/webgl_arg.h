/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef WEBGL_ARG_H
#define WEBGL_ARG_H

#include <climits>
#include "securec.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "napi/n_val.h"
#include "util/log.h"
#include "image_source.h"

namespace OHOS {
namespace Rosen {
using BufferDataType = enum {
    BUFFER_DATA_INT_8 = napi_int8_array,
    BUFFER_DATA_UINT_8 = napi_uint8_array,
    BUFFER_DATA_UINT_8_CLAMPED = napi_uint8_clamped_array,
    BUFFER_DATA_INT_16 = napi_int16_array,
    BUFFER_DATA_UINT_16 = napi_uint16_array,
    BUFFER_DATA_INT_32 = napi_int32_array,
    BUFFER_DATA_UINT_32 = napi_uint32_array,
    BUFFER_DATA_FLOAT_32 = napi_float32_array,
    BUFFER_DATA_FLOAT_64 = napi_float64_array,
    BUFFER_DATA_BIGINT_64 = napi_bigint64_array,
    BUFFER_DATA_BIGUINT_64 = napi_biguint64_array,
    BUFFER_DATA_BOOLEAN,
    BUFFER_DATA_GLENUM,
    BUFFER_DATA_INVALID
};

using BufferType = enum {
    BUFFER_DATA_VIEW,
    BUFFER_ARRAY_BUFFER,
    BUFFER_TYPED_ARRAY,
    BUFFER_ARRAY,
};

using WebGLArgType = enum : int {
    ARG_FLOAT,
    ARG_UINT,
    ARG_INT,
    ARG_ENUM,
    ARG_INTPTR,
    ARG_SIZE,
    ARG_BOOL,
    ARG_SIZEPTR,
};

union WebGLArgValue {
    GLboolean glBool;
    GLfloat glFloat;
    GLuint glUint;
    GLint glInt;
    GLenum glEnum;
    GLintptr glIntptr;
    GLsizei glSizei;
    GLsizeiptr glSizeptr;
};

struct WebGLArgInfo {
    WebGLArgType argType;
    uint32_t argIndex;
    const char* argName;
    WebGLArgInfo(uint32_t index, WebGLArgType type, const char* name)
        : argType(type), argIndex(index), argName(name) {}
    ~WebGLArgInfo() {}
};

struct WebGLImageOption {
    GLenum format;
    GLenum type;
    GLsizei width;
    GLsizei height;
    GLsizei depth;

    WebGLImageOption() : format(0), type(0), width(0), height(0) {}
    WebGLImageOption(GLenum format, GLenum type, GLsizei width, GLsizei height)
    {
        this->format = format;
        this->type = type;
        this->width = width;
        this->height = height;
        this->depth = 1;
    }
    WebGLImageOption(GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth)
    {
        this->format = format;
        this->type = type;
        this->width = width;
        this->height = height;
        this->depth = depth;
    }
    void Assign(const WebGLImageOption& opt)
    {
        this->format = opt.format;
        this->type = opt.type;
        this->width = opt.width;
        this->height = opt.height;
        this->depth = opt.depth;
    }
    void Dump();
};

using ImageDecodeFunc = enum {
    DECODE_RGBA_UBYTE,
    DECODE_RGB_UBYTE,
    DECODE_RGBA_USHORT_4444,
    DECODE_RGBA_USHORT_5551,
    DECODE_RGB_USHORT_565,
    DECODE_LUMINANCE_ALPHA_UBYTE,
    DECODE_LUMINANCE_UBYTE,
    DECODE_ALPHA_UBYTE
};

struct WebGLFormatMap {
    GLenum format;
    GLenum type;
    uint8_t channels;
    uint8_t bytesPrePixel;
    uint16_t decodeFunc;
    BufferDataType dataType;
    WebGLFormatMap(GLenum format, GLenum type, uint8_t channels, uint8_t bytesPrePixel, BufferDataType dataType,
        uint16_t decodeFunc)
    {
        this->format = format;
        this->type = type;
        this->channels = channels;
        this->bytesPrePixel = bytesPrePixel;
        this->dataType = dataType;
        this->decodeFunc = decodeFunc;
    }
};

union ColorParam {
#if BIG_ENDIANNESS
    struct {
        uint8_t alpha;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } argb;
#else
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } argb;
#endif
    uint32_t value;
};

union ColorParam_4_4_4_4 {
#if BIG_ENDIANNESS
    struct {
        uint8_t alpha : 4;
        uint8_t blue : 4;
        uint8_t green : 4;
        uint8_t red : 4;
    } rgba;
#else
    struct {
        uint8_t red : 4;
        uint8_t green : 4;
        uint8_t blue : 4;
        uint8_t alpha : 4;
    } rgba;
#endif
    uint16_t value;
};

union ColorParam_5_5_5_1 {
#if BIG_ENDIANNESS
    struct {
        uint8_t alpha : 1;
        uint8_t blue : 5;
        uint8_t green : 5;
        uint8_t red : 5;
    } rgba;
#else
    struct {
        uint8_t red : 5;
        uint8_t green : 5;
        uint8_t blue : 5;
        uint8_t alpha : 1;
    } rgba;
#endif
    uint16_t value;
};

union ColorParam_5_6_5 {
#if BIG_ENDIANNESS
    struct {
        uint8_t blue : 5;
        uint8_t green : 6;
        uint8_t red : 5;
    } rgb;
#else
    struct {
        uint8_t red : 5;
        uint8_t green : 6;
        uint8_t blue : 5;
    } rgb;
#endif
    uint16_t value;
};

struct TexImageArg {
    int func;
    GLenum target;
    GLint level;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    GLsizei width;
    GLsizei height;
    GLsizei border;
    GLsizei depth;

    TexImageArg()
        : func(0), target(0), level(0), internalFormat(0),
        format(0), type(0), width(0), height(0), border(0), depth(0) {}

    TexImageArg(const TexImageArg& arg)
    {
        func = arg.func;
        target = arg.target;
        level = arg.level;
        internalFormat = arg.internalFormat;
        format = arg.format;
        type = arg.type;
        width = arg.width;
        height = arg.height;
        border = arg.border;
        depth = arg.depth;
    }

    TexImageArg(
        GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum type)
    {
        this->target = target;
        this->level = level;
        this->internalFormat = internalFormat;
        this->type = type;
        this->width = width;
        this->height = height;
        this->depth = depth;
        this->border = 0;
        this->func = 0;
        this->format = 0;
    }
    virtual void Dump(const std::string& info) const;
};

struct TexStorageArg {
    int func;
    GLenum target;
    GLsizei levels;
    GLenum internalFormat;
    GLsizei width;
    GLsizei height;
    GLsizei depth;
    TexStorageArg() : func(0), target(0), levels(0), internalFormat(0), width(0), height(0), depth(0) {}
    TexStorageArg(const TexStorageArg& arg)
    {
        func = arg.func;
        target = arg.target;
        levels = arg.levels;
        internalFormat = arg.internalFormat;
        width = arg.width;
        height = arg.height;
        depth = arg.depth;
    }
    void Dump(const std::string& info) const;
};

struct TexSubImage2DArg : public TexImageArg {
    GLint xOffset;
    GLint yOffset;
    TexSubImage2DArg() : TexImageArg(), xOffset(0), yOffset{0} {}
    TexSubImage2DArg(const TexSubImage2DArg& arg) : TexImageArg(arg)
    {
        xOffset = arg.xOffset;
        yOffset = arg.yOffset;
    }
    void Dump(const std::string& info) const override;
};

struct TexSubImage3DArg : public TexSubImage2DArg {
    GLint zOffset;
    TexSubImage3DArg() : TexSubImage2DArg(), zOffset(0) {}
    TexSubImage3DArg(const TexSubImage3DArg& arg) : TexSubImage2DArg(arg)
    {
        zOffset = arg.zOffset;
    }
    void Dump(const std::string& info) const override;
};

struct CopyTexImage2DArg : public TexImageArg {
    GLint x;
    GLint y;
    CopyTexImage2DArg() : TexImageArg(), x(0), y{0} {}
};

struct CopyTexSubImageArg : public TexImageArg {
    GLint xOffset;
    GLint yOffset;
    GLint x;
    GLint y;
    CopyTexSubImageArg() : TexImageArg(), xOffset(0), yOffset{0}, x(0), y(0) {}
};

struct CopyTexSubImage3DArg : public CopyTexSubImageArg {
    GLint zOffset;
    CopyTexSubImage3DArg() : CopyTexSubImageArg(), zOffset(0) {}
};

struct TextureLayerArg {
    GLint level;
    GLint layer;
};

struct BufferBaseArg {
    GLenum target;
    GLuint index;
};

struct VertexAttribArg {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    GLintptr offset;
    void Dump(const std::string& info) const;
};

struct PixelsArg {
    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLenum type;
    void Dump(const std::string& info) const;
};

struct UniformExtInfo {
    GLuint dimension;
    GLuint elemCount;
    GLuint srcOffset;
    GLuint srcLength;
    UniformExtInfo(GLuint dimension) : dimension(dimension), elemCount(dimension), srcOffset(0), srcLength(0) {}
    UniformExtInfo(GLuint dimension, GLuint real)
        : dimension(dimension), elemCount(real), srcOffset(0), srcLength(0) {}
    bool GetUniformExtInfo(napi_env env, const NFuncArg& funcArg, int32_t start);
};

struct UniformTypeMap {
    GLenum type;
    GLenum baseType;
    GLsizei length;
    BufferDataType srcType;
    BufferDataType dstType;
};

// getVertexAttrib
struct VertexAttribDesc {
    bool enabled;
    bool normalized;
    // VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    GLuint boundBufferId;
    // VERTEX_ATTRIB_ARRAY_NORMALIZED
    GLsizei bytesPerElement;
    // VERTEX_ATTRIB_ARRAY_SIZE
    GLint size;
    // VERTEX_ATTRIB_ARRAY_TYPE
    GLenum type;
    // VERTEX_ATTRIB_ARRAY_STRIDE
    GLsizei stride;
    GLsizei originalStride;
    GLintptr offset;
    // by this func VertexAttribDivisor
    GLuint divisor;
};

struct VertexAttribInfo {
    BufferDataType type;
};

struct BufferPosition {
    GLint x;
    GLint y;
};

struct BufferSize {
    GLsizei width;
    GLsizei height;
};

struct DrawElementArg {
    GLenum mode;
    GLsizei count;
    GLenum type;
    int64_t offset;
};

struct BufferExt {
    GLuint offset;
    GLuint length;
    BufferExt() : offset(0), length(0) {}
    bool GetBufferExt(napi_env env, napi_value offsetArg, napi_value lenArg);
};

class WebGLArg {
public:
    static const GLsizei MATRIX_2X2_REQUIRE_MIN_SIZE = 4;
    static const GLsizei MATRIX_3X3_REQUIRE_MIN_SIZE = 9;
    static const GLsizei MATRIX_4X4_REQUIRE_MIN_SIZE = 16;

    static const GLsizei MATRIX_3X2_REQUIRE_MIN_SIZE = 100 + 6;
    static const GLsizei MATRIX_4X2_REQUIRE_MIN_SIZE = 100 + 8;
    static const GLsizei MATRIX_2X3_REQUIRE_MIN_SIZE = 200 + 6;
    static const GLsizei MATRIX_4X3_REQUIRE_MIN_SIZE = 200 + 12;
    static const GLsizei MATRIX_2X4_REQUIRE_MIN_SIZE = 300 + 8;
    static const GLsizei MATRIX_3X4_REQUIRE_MIN_SIZE = 300 + 12;

    static const GLsizei UNIFORM_1V_REQUIRE_MIN_SIZE = 1;
    static const GLsizei UNIFORM_2V_REQUIRE_MIN_SIZE = 2;
    static const GLsizei UNIFORM_3V_REQUIRE_MIN_SIZE = 3;
    static const GLsizei UNIFORM_4V_REQUIRE_MIN_SIZE = 4;

    WebGLArg(napi_env env, napi_value thisVar) : env_(env) {}
    virtual ~WebGLArg() {}

    static bool GetStringList(napi_env env, napi_value array, std::vector<char *>& list);
    static void FreeStringList(std::vector<char *>& list);
    static std::tuple<GLenum, GLintptr> ToGLintptr(napi_env env, napi_value data);
    static uint32_t GetWebGLDataSize(GLenum type);
    template<class T1, class T2>
    static bool CheckOverflow(T1 arg1, T2 arg2)
    {
        int64_t t = static_cast<int64_t>(arg1) + static_cast<int64_t>(arg2);
        if (t >= std::numeric_limits<T1>::max()) {
            return true;
        }
        return false;
    }

    template<class T>
    static bool CheckNoneNegInt(int64_t data)
    {
        if (data < 0) {
            return false;
        }
        if (data > static_cast<int64_t>(std::numeric_limits<T>::max())) {
            return false;
        }
        return true;
    }

    static napi_value GetUint32Parameter(napi_env env, GLenum pname);
    static napi_value GetInt32Parameter(napi_env env, GLenum pname);
    static napi_value GetInt64Parameter(napi_env env, GLenum pname);
    static napi_value GetFloatParameter(napi_env env, GLenum pname);
    static napi_value GetBoolParameter(napi_env env, GLenum pname);
    static bool CheckString(const std::string& str);
    static bool CheckReservedPrefix(const std::string& name);
private:
    napi_env env_;
    bool GetWebGLArg(napi_value data, WebGLArgValue& args, const WebGLArgInfo& func);
    WebGLArg(const WebGLArg&) = delete;
    WebGLArg& operator=(const WebGLArg&) = delete;
};

class WebGLCommBuffer {
public:
    const static int32_t MAX_DUMP = 12;
    WebGLCommBuffer(napi_env env) : env_(env) {}
    ~WebGLCommBuffer() {}

    size_t GetBufferDataSize() const;
    void DumpBuffer(BufferDataType destDataType) const;
    BufferDataType GetBufferDataType() const
    {
        return type_;
    }
    BufferType GetBufferType() const
    {
        return bufferType_;
    }
    size_t GetBufferLength() const
    {
        return dataLen_;
    }
protected:
    WebGLCommBuffer(const WebGLCommBuffer&) = delete;
    WebGLCommBuffer& operator=(const WebGLCommBuffer&) = delete;

    template<class T>
    void DumpDataBuffer(const char* format) const;
    napi_env env_;
    uint8_t* data_ = { nullptr };
    BufferDataType type_ { BUFFER_DATA_UINT_8 };
    size_t dataLen_ { 0 };
    BufferType bufferType_ { BUFFER_ARRAY };
    std::vector<uint8_t> buffer_ {};
};

class WebGLReadBufferArg : public WebGLCommBuffer {
public:
    WebGLReadBufferArg(napi_env env) : WebGLCommBuffer(env) {}
    ~WebGLReadBufferArg() {}

    napi_status GenBufferData(napi_value data, BufferDataType destDataType = BUFFER_DATA_FLOAT_32);
    uint8_t* GetBuffer() const
    {
        return data_;
    }
private:
    WebGLReadBufferArg(const WebGLReadBufferArg&) = delete;
    WebGLReadBufferArg& operator=(const WebGLReadBufferArg&) = delete;

    template<class srcT, class dstT>
    napi_status GetArrayElement(napi_value data, dstT defaultValue,
        napi_status (*getElementValue)(napi_env env, napi_value element, srcT* value));
    napi_status GetArrayData(napi_value data, BufferDataType destDataType);
};

class WebGLWriteBufferArg : public WebGLCommBuffer {
public:
    const static int32_t MAX_ALLOC_BUFFER_SIZE = 1024 * 1024;
    explicit WebGLWriteBufferArg(napi_env env) : WebGLCommBuffer(env) {}
    ~WebGLWriteBufferArg()
    {
        if (data_ != nullptr) {
            delete[] data_;
        }
    }

    uint8_t* AllocBuffer(uint32_t size)
    {
        if (size > MAX_ALLOC_BUFFER_SIZE) {
            return nullptr;
        }
        if (data_ != nullptr) {
            delete[] data_;
            data_ = nullptr;
        }
        dataLen_ = size;
        data_ = new uint8_t[dataLen_];
        return data_;
    }

    napi_value ToExternalArray(BufferDataType dstDataType);
    napi_value ToNormalArray(BufferDataType srcDataType, BufferDataType dstDataType);

private:
    template<class dstT>
    napi_value GenExternalArray();
    template<class srcT, class dstT>
    napi_value GenNormalArray(napi_status (*getNApiValue)(napi_env env, dstT value, napi_value* element));

    WebGLWriteBufferArg(const WebGLWriteBufferArg&) = delete;
    WebGLWriteBufferArg& operator=(const WebGLWriteBufferArg&) = delete;
};

class WebGLImageSource {
public:
    WebGLImageSource(napi_env env, int version, bool unpackFlipY, bool unpackPremultiplyAlpha)
        : webGLVersion_(version), env_(env), unpackFlipY_(unpackFlipY),
        unpackPremultiplyAlpha_(unpackPremultiplyAlpha) {}
    ~WebGLImageSource();

    GLsizei GetWidth() const
    {
        return imageOption_.width;
    }
    GLsizei GetHeight() const
    {
        return imageOption_.height;
    }
    const WebGLImageOption& GetWebGLImageOption() const
    {
        return imageOption_;
    }

    WebGLReadBufferArg* GetWebGLReadBuffer() const;
    GLvoid* GetImageSourceData() const;
    GLenum GenImageSource(const WebGLImageOption& opt, napi_value pixels, GLuint srcOffset);
    GLenum GenImageSource(const WebGLImageOption& opt, napi_value sourceImage);

    static std::tuple<size_t, BufferDataType>  GetBytesPrePixel(GLenum type);
    inline static ColorParam FromARGB(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
    {
        ColorParam colorValue {
    #if BIG_ENDIANNESS
            .argb = { .alpha = alpha, .red = red, .green = green, .blue = blue }
    #else
            .argb = { .blue = blue, .green = green, .red = red, .alpha = alpha }
    #endif
        };
        return colorValue;
    }

    inline static ColorParam FromRGBO(uint8_t red, uint8_t green, uint8_t blue, double opacity)
    {
        return FromARGB(static_cast<uint8_t>(round(opacity * 0xff)) & 0xff, red, green, blue);
    }

    inline static ColorParam FromRGB(uint8_t red, uint8_t green, uint8_t blue)
    {
        return FromARGB(0xff, red, green, blue);
    }
    static const WebGLFormatMap* GetWebGLFormatMap(GLenum type, GLenum format);
private:
    void DecodeData(const WebGLFormatMap* map, uint8_t* array);
    void DecodeDataForRGB_UBYTE(const WebGLFormatMap* formatMap, uint8_t* array);
    void DecodeDataForRGBA_USHORT_4444(const WebGLFormatMap* formatMap, uint8_t* array);
    void DecodeDataForRGBA_USHORT_5551(const WebGLFormatMap* formatMap, uint8_t* array);
    void DecodeDataForRGB_USHORT_565(const WebGLFormatMap* formatMap, uint8_t* array);
    bool DecodeImageData(const WebGLFormatMap* formatMap, const WebGLReadBufferArg* bufferDataArg, GLuint srcOffset);

    template<class T>
    std::tuple<bool, T> GetObjectIntField(napi_value resultObject, const std::string& name);
    bool HandleImageSourceData(napi_value resultData, napi_valuetype valueType);

    int webGLVersion_ { 0 };
    napi_env env_ { nullptr };
    bool unpackFlipY_ { false };
    bool unpackPremultiplyAlpha_ { false };
    GLuint srcOffset_ { 0 };
    WebGLImageOption imageOption_ {};
    std::vector<uint32_t> imageData_ {};
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap_ { nullptr };
    std::unique_ptr<WebGLReadBufferArg> readBuffer_ { nullptr };

    WebGLImageSource(const WebGLImageSource&) = delete;
    WebGLImageSource& operator=(const WebGLImageSource&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_ARG_H

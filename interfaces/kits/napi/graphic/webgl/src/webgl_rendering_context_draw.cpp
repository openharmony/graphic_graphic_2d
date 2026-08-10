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
#include "context/webgl_rendering_context_base_impl.h"

#include <array>
#include <cstdint>
#include <limits>
#include "context/webgl_rendering_context_base.h"
#include "context/webgl2_rendering_context_base.h"
#include "napi/n_class.h"
#include "util/log.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
namespace {
constexpr size_t MAX_ATTRIBUTE_NAME_LENGTH = 1025;

bool CheckedAdd(size_t a, size_t b, size_t& result)
{
    if (b > SIZE_MAX - a) {
        return false;
    }
    result = a + b;
    return true;
}

size_t GetAttribLocationCount(GLenum type, GLint arraySize)
{
    size_t columns = 1;
    switch (type) {
        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT2x3:
        case GL_FLOAT_MAT2x4:
            columns = 2;
            break;
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT3x2:
        case GL_FLOAT_MAT3x4:
            columns = 3;
            break;
        case GL_FLOAT_MAT4:
        case GL_FLOAT_MAT4x2:
        case GL_FLOAT_MAT4x3:
            columns = 4;
            break;
        default:
            break;
    }
    if (arraySize <= 0 || static_cast<size_t>(arraySize) > SIZE_MAX / columns) {
        return 0;
    }
    return columns * static_cast<size_t>(arraySize);
}

bool IsSignedIntegerAttribType(GLenum type)
{
    return type == GL_BYTE || type == GL_SHORT || type == GL_INT;
}

bool IsUnsignedIntegerAttribType(GLenum type)
{
    return type == GL_UNSIGNED_BYTE || type == GL_UNSIGNED_SHORT || type == GL_UNSIGNED_INT;
}

bool GetPixelBytes(GLenum type, uint32_t components, size_t& bytes)
{
    switch (type) {
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_5_5_5_1:
            bytes = sizeof(uint16_t);
            return true;
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
        case GL_UNSIGNED_INT_5_9_9_9_REV:
        case GL_UNSIGNED_INT_24_8:
            bytes = sizeof(uint32_t);
            return true;
        case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
            bytes = sizeof(uint64_t);
            return true;
        default:
            break;
    }
    size_t componentSize = 0;
    switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            componentSize = sizeof(uint8_t);
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_HALF_FLOAT:
            componentSize = sizeof(uint16_t);
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
            componentSize = sizeof(uint32_t);
            break;
        default:
            return false;
    }
    if (components == 0 || static_cast<size_t>(components) > SIZE_MAX / componentSize) {
        return false;
    }
    bytes = componentSize * static_cast<size_t>(components);
    return true;
}

size_t GetPixelElementSize(GLenum type)
{
    switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return sizeof(uint8_t);
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_HALF_FLOAT:
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_5_5_5_1:
            return sizeof(uint16_t);
        case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
            return sizeof(uint64_t);
        default:
            return sizeof(uint32_t);
    }
}

struct CompressedBlockFormat {
    size_t blockWidth;
    size_t blockHeight;
    size_t blockSize;
};

struct CompressedPvrtcFormat {
    size_t minWidth;
    size_t minHeight;
    size_t bpp;
};

bool CheckedMultiply(size_t a, size_t b, size_t& result)
{
    if (a != 0 && b > SIZE_MAX / a) {
        return false;
    }
    result = a * b;
    return true;
}

constexpr size_t ZERO_BUFFER_CHUNK_SIZE = 64 * 1024;
constexpr GLsizeiptr MAX_WEBGL_BUFFER_DATA_SIZE = static_cast<GLsizeiptr>(1024) * 1024 * 1024;
constexpr uint64_t MAX_WEBGL_TOTAL_BUFFER_DATA_SIZE = static_cast<uint64_t>(MAX_WEBGL_BUFFER_DATA_SIZE);
const std::array<uint8_t, ZERO_BUFFER_CHUNK_SIZE> ZERO_BUFFER_DATA {};

GLenum ClearBufferData(GLenum target, GLsizeiptr size, GLenum usage)
{
    GLsizeiptr offset = 0;
    while (offset < size) {
        const GLsizeiptr remaining = size - offset;
        const GLsizeiptr chunkSize = std::min(
            remaining, static_cast<GLsizeiptr>(ZERO_BUFFER_DATA.size()));
        glBufferSubData(target, offset, chunkSize, ZERO_BUFFER_DATA.data());
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            glBufferData(target, 0, nullptr, usage);
            (void)glGetError();
            return error;
        }
        offset += chunkSize;
    }
    return GL_NO_ERROR;
}

bool ComputeBlockBytes(size_t width, size_t height, const CompressedBlockFormat& fmt, size_t& out)
{
    if (fmt.blockWidth == 0 || fmt.blockHeight == 0) {
        return false;
    }
    size_t roundedWidth = 0;
    size_t roundedHeight = 0;
    if (!CheckedAdd(width, fmt.blockWidth - 1, roundedWidth) ||
        !CheckedAdd(height, fmt.blockHeight - 1, roundedHeight)) {
        return false;
    }
    size_t across = roundedWidth / fmt.blockWidth;
    size_t down = roundedHeight / fmt.blockHeight;
    size_t blocks = 0;
    if (!CheckedMultiply(across, down, blocks) || !CheckedMultiply(blocks, fmt.blockSize, out)) {
        return false;
    }
    return true;
}

bool ComputePvrtcBytes(size_t width, size_t height, const CompressedPvrtcFormat& fmt, size_t& out)
{
    const size_t kBitsPerByte = 8;
    size_t w = max(width, fmt.minWidth);
    size_t h = max(height, fmt.minHeight);
    size_t pixels = 0;
    size_t total = 0;
    if (!CheckedMultiply(w, h, pixels) || !CheckedMultiply(pixels, fmt.bpp, total)) {
        return false;
    }
    if (total > SIZE_MAX - (kBitsPerByte - 1)) {
        return false;
    }
    out = (total + (kBitsPerByte - 1)) / kBitsPerByte;
    return true;
}

bool ComputeCompressedSliceBytes(const TexImageArg& imgArg, size_t& out, GLenum& err)
{
    const CompressedBlockFormat kDxt1 { 4, 4, 8 };
    const CompressedBlockFormat kDxt35 { 4, 4, 16 };
    const CompressedPvrtcFormat kPvrtc4 { 8, 8, 4 };
    const CompressedPvrtcFormat kPvrtc2 { 16, 8, 2 };
    size_t width = static_cast<size_t>(imgArg.width);
    size_t height = static_cast<size_t>(imgArg.height);
    err = WebGLRenderingContextBase::NO_ERROR;
    GLenum format = imgArg.internalFormat == 0 ? imgArg.format : imgArg.internalFormat;
    switch (format) {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_ETC1_RGB8_OES:
            return ComputeBlockBytes(width, height, kDxt1, out);
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return ComputeBlockBytes(width, height, kDxt35, out);
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            return ComputePvrtcBytes(width, height, kPvrtc4, out);
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
            return ComputePvrtcBytes(width, height, kPvrtc2, out);
        default:
            err = WebGLRenderingContextBase::INVALID_ENUM;
            return false;
    }
}

bool ComputeCompressedBytesRequired(const TexImageArg& imgArg, size_t& out, GLenum& err)
{
    size_t sliceBytes = 0;
    if (!ComputeCompressedSliceBytes(imgArg, sliceBytes, err)) {
        return false;
    }
    bool is3D = imgArg.target == GL_TEXTURE_3D || imgArg.target == GL_TEXTURE_2D_ARRAY;
    size_t depth = is3D ? static_cast<size_t>(imgArg.depth) : 1;
    return CheckedMultiply(sliceBytes, depth, out);
}

struct PixelUnpackLayout {
    size_t width;
    size_t height;
    size_t depth;
    size_t rowStride;
    size_t imageStride;
    size_t bytesPerPixel;
    size_t skipPixels;
    size_t skipRows;
    size_t skipImages;
};

struct PixelUnpackState {
    GLint rowLength;
    GLint imageHeight;
    GLint skipPixels;
    GLint skipRows;
    GLint skipImages;
    GLint alignment;
};

GLenum BuildPixelUnpackLayout(const TexImageArg& imgArg, uint32_t componentCount,
    const PixelUnpackState& state, PixelUnpackLayout& layout)
{
    layout.width = static_cast<size_t>(imgArg.width);
    layout.height = static_cast<size_t>(imgArg.height);
    bool is3D = imgArg.target == GL_TEXTURE_3D || imgArg.target == GL_TEXTURE_2D_ARRAY;
    layout.depth = is3D ? static_cast<size_t>(imgArg.depth) : 1;
    size_t rowLength = state.rowLength == 0 ? layout.width : static_cast<size_t>(state.rowLength);
    size_t imageHeight = state.imageHeight == 0 ? layout.height : static_cast<size_t>(state.imageHeight);
    layout.skipPixels = static_cast<size_t>(state.skipPixels);
    layout.skipRows = static_cast<size_t>(state.skipRows);
    layout.skipImages = is3D ? static_cast<size_t>(state.skipImages) : 0;
    size_t end = 0;
    if (!CheckedAdd(layout.skipPixels, layout.width, end) || end > rowLength ||
        !CheckedAdd(layout.skipRows, layout.height, end) || end > imageHeight) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    if (!GetPixelBytes(imgArg.type, componentCount, layout.bytesPerPixel)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    size_t rowBytes = 0;
    size_t alignment = static_cast<size_t>(state.alignment);
    if (!CheckedMultiply(rowLength, layout.bytesPerPixel, rowBytes) ||
        !CheckedAdd(rowBytes, alignment - 1, layout.rowStride)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    layout.rowStride = layout.rowStride / alignment * alignment;
    if (!CheckedMultiply(layout.rowStride, imageHeight, layout.imageStride)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool ComputePixelUnpackSkipBytes(const PixelUnpackLayout& layout, size_t& required)
{
    size_t part = 0;
    return CheckedMultiply(layout.skipImages, layout.imageStride, required) &&
        CheckedMultiply(layout.skipRows, layout.rowStride, part) && CheckedAdd(required, part, required) &&
        CheckedMultiply(layout.skipPixels, layout.bytesPerPixel, part) && CheckedAdd(required, part, required);
}

bool AddPixelUnpackDataBytes(const PixelUnpackLayout& layout, size_t& required)
{
    if (layout.width == 0 || layout.height == 0 || layout.depth == 0) {
        return true;
    }
    size_t part = 0;
    return CheckedMultiply(layout.depth - 1, layout.imageStride, part) && CheckedAdd(required, part, required) &&
        CheckedMultiply(layout.height - 1, layout.rowStride, part) && CheckedAdd(required, part, required) &&
        CheckedMultiply(layout.width, layout.bytesPerPixel, part) && CheckedAdd(required, part, required);
}

bool ComputeVertexAttribRequiredEnd(
    const VertexAttribInfo& info, uint64_t maxVertex, GLsizei instanceCount, size_t& requiredEnd)
{
    size_t componentSize = WebGLArg::GetWebGLDataSize(info.glType);
    size_t attributeSize = 0;
    if (!CheckedMultiply(static_cast<size_t>(info.size), componentSize, attributeSize)) {
        return false;
    }
    size_t element = info.divisor == 0 ? static_cast<size_t>(maxVertex) :
        static_cast<size_t>(instanceCount - 1) / static_cast<size_t>(info.divisor);
    size_t stride = info.stride == 0 ? attributeSize : static_cast<size_t>(info.stride);
    size_t elementOffset = 0;
    return CheckedMultiply(element, stride, elementOffset) &&
        CheckedAdd(static_cast<size_t>(info.offset), elementOffset, requiredEnd) &&
        CheckedAdd(requiredEnd, attributeSize, requiredEnd);
}
} // namespace

GLenum WebGLRenderingContextBaseImpl::CheckPixelUnpackBufferRange(
    napi_env env, GLintptr offset, GLsizeiptr size)
{
    if (offset < 0 || size < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    GLuint bufferId = boundBufferIds_[BoundBufferType::PIXEL_UNPACK_BUFFER];
    if (bufferId == 0) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    WebGLBuffer* buffer = GetObjectInstance<WebGLBuffer>(env, bufferId);
    if (buffer == nullptr) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    uint64_t bufferSize = static_cast<uint64_t>(buffer->GetBufferSize());
    uint64_t offsetBytes = static_cast<uint64_t>(offset);
    uint64_t requiredBytes = static_cast<uint64_t>(size);
    if (offsetBytes > bufferSize || requiredBytes > bufferSize - offsetBytes) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckPixelUnpackData(
    napi_env env, const TexImageArg& imgArg, GLintptr offset)
{
    if (imgArg.width < 0 || imgArg.height < 0 || imgArg.depth < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    size_t elementSize = GetPixelElementSize(imgArg.type);
    if (offset < 0 || static_cast<uint64_t>(offset) % elementSize != 0) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    if (unpackFlipY_ || unpackPremultiplyAlpha_) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    PixelUnpackState state { unpackRowLength_, unpackImageHeight_, unpackSkipPixels_, unpackSkipRows_,
        unpackSkipImages_, unpackAlignment_ };
    PixelUnpackLayout layout {};
    GLenum result = BuildPixelUnpackLayout(imgArg, GetFormatComponentCount(imgArg.format), state, layout);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    size_t required = 0;
    if (!ComputePixelUnpackSkipBytes(layout, required) || !AddPixelUnpackDataBytes(layout, required) ||
        required > static_cast<size_t>(std::numeric_limits<GLsizeiptr>::max())) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return CheckPixelUnpackBufferRange(env, offset, static_cast<GLsizeiptr>(required));
}

void WebGLRenderingContextBaseImpl::TexImage2D_(
    const TexImageArg& imgArg, WebGLTexture* texture, const void* pixels, bool changeUnpackAlignment)
{
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    glTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.border,
        imgArg.format, imgArg.type, pixels);
    texture->SetTextureLevel(imgArg);

    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
    LOGD("WebGL TexImage2D_ target %{public}u result %{public}u", imgArg.target, GetError_());
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(
    napi_env env, const TexImageArg& imgArg, napi_value pixels, GLuint srcOffset)
{
    imgArg.Dump("WebGL texImage2D");
    LOGD("WebGL texImage2D srcOffset %{public}u", srcOffset);
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "texture is nullptr");
        return NVal::CreateNull(env).val_;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "CheckTexImage failed");
        return NVal::CreateNull(env).val_;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "CheckImmutable failed");
        return NVal::CreateNull(env).val_;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "CheckNPOT failed");
        return NVal::CreateNull(env).val_;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        error =imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height }, pixels, srcOffset);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D invalid pixels");
            return NVal::CreateNull(env).val_;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData();
    }
    TexImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg& info, napi_value source)
{
    TexImageArg imgArg(info);
    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    if (!NVal(env, source).IsNull()) {
        GLenum error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, source);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texImage2D Image source invalid");
            return NVal::CreateNull(env).val_;
        }
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }
    imgArg.Dump("WebGL texImage2D");

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "Can not find texture");
        return NVal::CreateNull(env).val_;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texImage2D checkTexImage failed");
        return NVal::CreateNull(env).val_;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "WebGL texImage2D checkImmutable failed");
        return NVal::CreateNull(env).val_;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL texImage2D checkNPOT failed");
        return NVal::CreateNull(env).val_;
    }
    if (imgArg.type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg.type = GL_FLOAT;
    }

    TexImage2D_(imgArg, texture, data, unpackAlignment_ != 1);
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg& imgArg, GLintptr pbOffset)
{
    imgArg.Dump("WebGL texImage2D");
    if (boundBufferIds_[BoundBufferType::PIXEL_UNPACK_BUFFER] == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "no PIXEL_UNPACK_BUFFER bound");
        return NVal::CreateNull(env).val_;
    }
    if (pbOffset < 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "pbOffset is negative");
        return NVal::CreateNull(env).val_;
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "Can not find texture");
        return NVal::CreateNull(env).val_;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texImage2D error");
        return NVal::CreateNull(env).val_;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    error = CheckPixelUnpackData(env, imgArg, pbOffset);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "texImage2D PBO range is invalid");
        return NVal::CreateNull(env).val_;
    }
    glTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.border,
        imgArg.format, imgArg.type, reinterpret_cast<GLvoid*>(pbOffset));
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return NVal::CreateNull(env).val_;
}

void WebGLRenderingContextBaseImpl::TexSubImage2D_(
    const TexSubImage2DArg& imgArg, WebGLTexture* texture, const void* pixels, bool changeUnpackAlignment)
{
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    glTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, imgArg.type, pixels);
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(napi_env env, const TexSubImage2DArg& imgArg, GLintptr pbOffset)
{
    imgArg.Dump("WebGL texSubImage2D");
    if (boundBufferIds_[BoundBufferType::PIXEL_UNPACK_BUFFER] == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "no PIXEL_UNPACK_BUFFER bound");
        return NVal::CreateNull(env).val_;
    }
    if (pbOffset < 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "pbOffset is negative");
        return NVal::CreateNull(env).val_;
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "texture is nullptr");
        return NVal::CreateNull(env).val_;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "CheckTexImage failed");
        return NVal::CreateNull(env).val_;
    }
    error = CheckPixelUnpackData(env, imgArg, pbOffset);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "texSubImage2D PBO range is invalid");
        return NVal::CreateNull(env).val_;
    }
    glTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, imgArg.type, reinterpret_cast<void *>(pbOffset));
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg& info, napi_value pixels, GLuint srcOffset)
{
    TexSubImage2DArg& imgArg = const_cast<TexSubImage2DArg&>(info);
    imgArg.Dump("WebGL texSubImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "texture is nullptr");
        return NVal::CreateNull(env).val_;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D error");
        return NVal::CreateNull(env).val_;
    }

    if (!texture->CheckValid(imgArg.target, imgArg.level)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, " invalid texture level");
        return NVal::CreateNull(env).val_;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        error = imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height }, pixels, srcOffset);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D invalid pixels");
            return NVal::CreateNull(env).val_;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    } else {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "pixels is null");
        return NVal::CreateNull(env).val_;
    }

    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D error");
        return NVal::CreateNull(env).val_;
    }

    if (imgArg.type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg.type = GL_FLOAT;
    }
    TexSubImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg& info, napi_value imageData)
{
    TexSubImage2DArg imgArg(info);
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "Can not find texture");
        return NVal::CreateNull(env).val_;
    }
    GLvoid* data = nullptr;
    GLenum error = 0;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    if (!NVal(env, imageData).IsNull()) {
        error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, imageData);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D Image source invalid");
            return NVal::CreateNull(env).val_;
        }
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }
    imgArg.Dump("WebGL texSubImage2D");
    error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D CheckTexImage failed");
        return NVal::CreateNull(env).val_;
    }
    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D CheckTexSubImage2D failed");
        return NVal::CreateNull(env).val_;
    }
    TexSubImage2D_(imgArg, texture, data, unpackAlignment_);
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::DrawElements(
    napi_env env, GLenum mode, GLsizei count, GLenum type, GLintptr offset)
{
    LOGD("WebGL drawElements mode %{public}u %{public}d %{public}u", mode, count, type);
    GLenum result = CheckDrawElements(env, mode, count, type, static_cast<int64_t>(offset));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "WebGL drawElements failed");
        return NVal::CreateNull(env).val_;
    }
    glDrawElements(mode, count, type, reinterpret_cast<GLvoid*>(offset));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::DrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count)
{
    LOGD("WebGL drawArrays mode %{public}u %{public}d %{public}d error %{public}u", mode, first, count, GetError_());
    GLenum result = CheckDrawArrays(env, mode, first, count);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }
    glDrawArrays(mode, first, count);
    LOGD("WebGL drawArrays result %{public}u", GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(napi_env env, const PixelsArg& arg, GLintptr offset)
{
    arg.Dump("WebGL readPixels");
    if (!IsHighWebGL()) {
        return NVal::CreateNull(env).val_;
    }

    WebGLBuffer* buffer = GetBoundBuffer(env, WebGL2RenderingContextBase::PIXEL_PACK_BUFFER);
    if (buffer == nullptr || buffer->GetBufferSize() == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
            "buffer is nullptr or GetBufferSize failed");
        return NVal::CreateNull(env).val_;
    }
    uint64_t bufferSize = static_cast<uint64_t>(buffer->GetBufferSize());
    uint64_t dstOffset = static_cast<uint64_t>(offset);

    // Pass full buffer size and offset separately to CheckReadPixelsArg
    GLenum result = CheckReadPixelsArg(env, arg, bufferSize, dstOffset);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }
    glReadPixels(arg.x, arg.y, arg.width, arg.height, arg.format, arg.type, reinterpret_cast<void*>(offset));
    LOGD("WebGL readPixels result %{public}u", GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(
    napi_env env, const PixelsArg& arg, napi_value buffer, GLuint dstOffset)
{
    arg.Dump("WebGL readPixels");
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }

    // dstOffset is element offset, convert to byte offset
    uint64_t elemSize = static_cast<uint64_t>(bufferData.GetBufferDataSize());
    uint64_t dstOffsetBytes = static_cast<uint64_t>(dstOffset) * elemSize;
    uint64_t bufferBytes = static_cast<uint64_t>(bufferData.GetBufferLength());

    GLenum result = CheckReadPixelsArg(env, arg, bufferBytes, dstOffsetBytes);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }

    uint8_t* destination = bufferData.GetBuffer();
    if (destination == nullptr && bufferBytes > 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "readPixels destination is nullptr");
        return NVal::CreateNull(env).val_;
    }
    void* destinationStart = (destination == nullptr) ? nullptr : destination + dstOffsetBytes;
    glReadPixels(arg.x, arg.y, arg.width, arg.height, arg.format, arg.type, destinationStart);
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    LOGD("WebGL readPixels dstOffsetBytes %{public}llu result %{public}u",
        static_cast<unsigned long long>(dstOffsetBytes), GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::BufferData_(
    napi_env env, GLenum target, GLsizeiptr size, GLenum usage, const uint8_t* bufferData)
{
    LOGD("WebGL bufferData target %{public}u, usage %{public}u", target, usage);
    if (size < 0 || size > MAX_WEBGL_BUFFER_DATA_SIZE) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "size is outside the supported range");
        return NVal::CreateNull(env).val_;
    }
    uint32_t index = 0;
    if (!CheckBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "CheckBufferTarget failed");
        return NVal::CreateNull(env).val_;
    }
    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[index]);
    if (webGLBuffer == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "webGLBuffer is nullptr");
        return NVal::CreateNull(env).val_;
    }
    if (!CheckBufferDataUsage(env, usage)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "CheckBufferDataUsage failed");
        return NVal::CreateNull(env).val_;
    }

    if (webGLBuffer->GetTarget() != target) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
            "webGLBuffer->GetTarget %{public}u target %{public}u", webGLBuffer->GetTarget(), target);
        return NVal::CreateNull(env).val_;
    }
    const uint64_t previousBufferSize = static_cast<uint64_t>(webGLBuffer->GetBufferSize());
    const uint64_t retainedBufferBytes = allocatedBufferBytes_ >= previousBufferSize ?
        allocatedBufferBytes_ - previousBufferSize : 0;
    const uint64_t requestedBufferSize = static_cast<uint64_t>(size);
    if (retainedBufferBytes > MAX_WEBGL_TOTAL_BUFFER_DATA_SIZE ||
        requestedBufferSize > MAX_WEBGL_TOTAL_BUFFER_DATA_SIZE - retainedBufferBytes) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::OUT_OF_MEMORY, "buffer allocation exceeds the context quota");
        return NVal::CreateNull(env).val_;
    }
    glBufferData(target, size, bufferData, usage);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "glBufferData failed");
        return NVal::CreateNull(env).val_;
    }
    if (bufferData == nullptr && size > 0) {
        error = ClearBufferData(target, size, usage);
        if (error != GL_NO_ERROR) {
            webGLBuffer->SetBuffer(0, nullptr);
            SET_ERROR_WITH_LOG(error, "bufferData zero initialization failed");
            return NVal::CreateNull(env).val_;
        }
    }
    allocatedBufferBytes_ = retainedBufferBytes + requestedBufferSize;
    webGLBuffer->SetBuffer(size, bufferData);
    webGLBuffer->SetTarget(target);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(napi_env env, GLenum target, int64_t size, GLenum usage)
{
    LOGD("WebGL bufferData target %{public}u, usage %{public}u size %{public}" PRIi64, target, usage, size);
    BufferData_(env, target, static_cast<GLsizeiptr>(size), usage, nullptr);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(
    napi_env env, GLenum target, napi_value data, GLenum usage, const BufferExt& ext)
{
    WebGLReadBufferArg bufferData(env);
    if (NVal(env, data).IsNull()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "data is nullptr");
        return NVal::CreateNull(env).val_;
    }
    bool succ = bufferData.GenBufferData(data, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ || bufferData.GetBufferType() == BUFFER_ARRAY) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    // change
    size_t bufferLen = bufferData.GetBufferLength();
    size_t elemSize = bufferData.GetBufferDataSize();
    uint64_t offsetBytes = static_cast<uint64_t>(ext.offset) * elemSize;
    if (offsetBytes > bufferLen) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "srcOffset out of bounds");
        return NVal::CreateNull(env).val_;
    }
    size_t maxLength = bufferLen - static_cast<size_t>(offsetBytes);
    uint64_t selectedLength = 0;
    if (ext.length == 0) {
        selectedLength = static_cast<uint64_t>(maxLength);
    } else {
        uint64_t lengthBytes = static_cast<uint64_t>(ext.length) * elemSize;
        if (lengthBytes > maxLength) {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "length out of bounds");
            return NVal::CreateNull(env).val_;
        }
        selectedLength = lengthBytes;
    }
    if (selectedLength > static_cast<uint64_t>(MAX_WEBGL_BUFFER_DATA_SIZE)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "buffer data exceeds the supported limit");
        return NVal::CreateNull(env).val_;
    }
    GLsizeiptr length = static_cast<GLsizeiptr>(selectedLength);
    const uint8_t* source = bufferData.GetBuffer();
    if (length > 0 && source == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "buffer data is nullptr");
        return NVal::CreateNull(env).val_;
    }
    const uint8_t* sourceStart = (length == 0) ? nullptr : source + static_cast<size_t>(offsetBytes);
    BufferData_(env, target, length, usage, sourceStart);
    LOGD("WebGL bufferData buffer usage %{public}u size %{public}zu target %{public}u, result %{public}u ",
        usage, bufferData.GetBufferLength(), target, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::BufferSubData(
    napi_env env, GLenum target, GLintptr offset, napi_value buffer, const BufferExt& ext)
{
    uint32_t index = 0;
    if (!CheckBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "bufferSubData invalid target");
        return NVal::CreateNull(env).val_;
    }
    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[index]);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferSize() == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "bufferSubData Can not find bound buffer");
        return NVal::CreateNull(env).val_;
    }

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL bufferSubData invalid buffer data");
        return NVal::CreateNull(env).val_;
    }
    // check sub buffer
    if (offset < 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL bufferSubData negative offset");
        return NVal::CreateNull(env).val_;
    }
    const uint64_t offsetBytes = static_cast<uint64_t>(offset);
    const uint64_t bufferSize = static_cast<uint64_t>(webGLBuffer->GetBufferSize());
    const uint64_t dataSize = static_cast<uint64_t>(bufferData.GetBufferLength());
    if (offsetBytes > bufferSize || dataSize > bufferSize - offsetBytes) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL bufferSubData invalid buffer size %{public}zu offset %{public}zu ",
            bufferData.GetBufferLength(), webGLBuffer->GetBufferSize());
        return NVal::CreateNull(env).val_;
    }

    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    glBufferSubData(target, offset, static_cast<GLsizeiptr>(bufferData.GetBufferLength()),
        static_cast<uint8_t*>(bufferData.GetBuffer()));
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "glBufferSubData failed");
        return NVal::CreateNull(env).val_;
    }
    if (!webGLBuffer->UpdateBuffer(
        static_cast<size_t>(offsetBytes), bufferData.GetBuffer(), static_cast<size_t>(dataSize))) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "index buffer shadow update failed");
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL bufferSubData offset %{public}u target %{public}u size %{public}zu result %{public}u ",
         static_cast<unsigned int>(offset), static_cast<unsigned int>(target),
         bufferData.GetBufferLength(), static_cast<unsigned int>(error));
    return NVal::CreateNull(env).val_;
}

// CC-OFFNXT(huge_cyclomatic_complexity,G.FUN.01-CPP) WebGL pname dispatch maps directly to specification states.
napi_value WebGLRenderingContextBaseImpl::PixelStorei(napi_env env, GLenum pname, GLint param)
{
    switch (pname) {
        case WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL:
            unpackFlipY_ = (param == 1);
            return NVal::CreateNull(env).val_;
        case WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL:
            unpackPremultiplyAlpha_ = (param == 1);
            return NVal::CreateNull(env).val_;
        case WebGLRenderingContextBase::PACK_ALIGNMENT:
        case WebGLRenderingContextBase::UNPACK_ALIGNMENT: {
            if (param == 1 || param == 2 || param == 4 || param == 8) { // 2,4,8 ALIGNMENT
                if (pname == WebGLRenderingContextBase::PACK_ALIGNMENT) {
                    packAlignment_ = param;
                    webGLRenderingContext_->SetPackAlignment(param);
                } else {
                    unpackAlignment_ = param;
                }
            } else {
                SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
                return NVal::CreateNull(env).val_;
            }
            break;
        }
        case GL_UNPACK_ROW_LENGTH:
        case GL_UNPACK_SKIP_ROWS:
        case GL_UNPACK_SKIP_PIXELS:
        case GL_UNPACK_IMAGE_HEIGHT:
        case GL_UNPACK_SKIP_IMAGES: {
            if (!IsHighWebGL()) {
                SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
                return NVal::CreateNull(env).val_;
            }
            if (param < 0) {
                SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
                return NVal::CreateNull(env).val_;
            }
            switch (pname) {
                case GL_UNPACK_ROW_LENGTH:
                    unpackRowLength_ = param;
                    break;
                case GL_UNPACK_SKIP_ROWS:
                    unpackSkipRows_ = param;
                    break;
                case GL_UNPACK_SKIP_PIXELS:
                    unpackSkipPixels_ = param;
                    break;
                case GL_UNPACK_IMAGE_HEIGHT:
                    unpackImageHeight_ = param;
                    break;
                default:
                    unpackSkipImages_ = param;
                    break;
            }
            break;
        }
        case WebGLRenderingContextBase::UNPACK_COLORSPACE_CONVERSION_WEBGL: {
            if (static_cast<GLenum>(param) == WebGLRenderingContextBase::BROWSER_DEFAULT_WEBGL || param == GL_NONE) {
                unpackColorspaceConversion_ = static_cast<GLenum>(param);
            } else {
                SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
                    "WebGL pixelStorei invalid parameter for UNPACK_COLORSPACE_CONVERSION_WEBGL %{public}u", pname);
                return NVal::CreateNull(env).val_;
            }
            break;
        }
        default:
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM,
                "WebGL pixelStorei invalid pname  %{public}u", pname);
            return NVal::CreateNull(env).val_;
    }
    glPixelStorei(pname, param);
    LOGD("WebGL pixelStorei pname %{public}u param %{public}d result %{public}u ", pname, param, GetError_());
    return NVal::CreateNull(env).val_;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexImage2D(
    napi_env env, const TexImageArg& imgArg, size_t imageSize)
{
    if (imgArg.border) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("Invalid target or level target %{public}u %{public}d", imgArg.target, imgArg.level);
        return result;
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr || texture->CheckImmutable()) {
        LOGE("Invalid texture target %{public}u ", imgArg.target);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    if (!CheckTexImageInternalFormat(env, imgArg.func, imgArg.internalFormat)) {
        LOGE("Invalid internalFormat target %{public}u %{public}u", imgArg.target, imgArg.internalFormat);
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    result = CheckCompressedTexDimensions(imgArg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("Invalid internalFormat %{public}u ", imgArg.internalFormat);
        return result;
    }
    
    result = CheckCompressedTexData(imgArg, imageSize);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("Invalid tex data %{public}u ", result);
        return result;
    }

    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(
    napi_env env, const TexImageArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg.Dump("WebGL compressedTexImage2D");
    if (boundBufferIds_[BoundBufferType::PIXEL_UNPACK_BUFFER] == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "no PIXEL_UNPACK_BUFFER bound");
        return NVal::CreateNull(env).val_;
    }
    if (offset < 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "offset is negative");
        return NVal::CreateNull(env).val_;
    }
    if (imageSize < 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "imageSize is negative");
        return NVal::CreateNull(env).val_;
    }
    WebGLBuffer* pboBuf = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::PIXEL_UNPACK_BUFFER]);
    if (pboBuf == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "PBO is null");
        return NVal::CreateNull(env).val_;
    }
    uint64_t pboSize = static_cast<uint64_t>(pboBuf->GetBufferSize());
    uint64_t offset64 = static_cast<uint64_t>(offset);
    uint64_t imageSize64 = static_cast<uint64_t>(imageSize);
    if (offset64 > pboSize || imageSize64 > pboSize - offset64) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "offset + imageSize exceeds PBO size");
        return NVal::CreateNull(env).val_;
    }
    GLenum result = CheckCompressedTexImage2D(env, imgArg, static_cast<size_t>(imageSize));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }

    glCompressedTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.border, imageSize, reinterpret_cast<void*>(offset));
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture != nullptr) {
        texture->SetTextureLevel(
            { imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, 1, GL_UNSIGNED_BYTE });
    }
    LOGD("WebGL compressedTexImage2D %{public}u", GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(
    napi_env env, const TexImageArg& info, napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride)
{
    TexImageArg& imgArg = const_cast<TexImageArg&>(info);
    imgArg.Dump("WebGL compressedTexImage2D");

    WebGLReadBufferArg bufferData(env);
    bool succ = bufferData.GenBufferData(srcData, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "GenBufferData failed");
        return NVal::CreateNull(env).val_;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());

    size_t bufferLen = bufferData.GetBufferLength();
    size_t elemSize = bufferData.GetBufferDataSize();
    if (elemSize == 0 || static_cast<size_t>(srcOffset) > SIZE_MAX / elemSize) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "srcOffset out of bounds");
        return NVal::CreateNull(env).val_;
    }
    size_t offsetBytes = static_cast<size_t>(srcOffset) * elemSize;
    if (offsetBytes > bufferLen) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "srcOffset out of bounds");
        return NVal::CreateNull(env).val_;
    }
    size_t elementCount = bufferLen / elemSize;
    size_t availableElements = elementCount - srcOffset;
    size_t selectedElements = srcLengthOverride == 0 ? availableElements : srcLengthOverride;
    if (selectedElements > availableElements || selectedElements > SIZE_MAX / elemSize) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "source length out of bounds");
        return NVal::CreateNull(env).val_;
    }
    size_t length = selectedElements * elemSize;

    GLenum result = CheckCompressedTexImage2D(env, imgArg, length);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckCompressedTexImage2D failed");
        return NVal::CreateNull(env).val_;
    }
    if (length > static_cast<size_t>(std::numeric_limits<GLsizei>::max())) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "compressedTexImage2D length too large");
        return NVal::CreateNull(env).val_;
    }
    uint8_t* source = bufferData.GetBuffer();
    if (source == nullptr && length > 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "compressed texture source is nullptr");
        return NVal::CreateNull(env).val_;
    }
    GLvoid* data = (source == nullptr) ? nullptr : static_cast<GLvoid*>(source + offsetBytes);
    glCompressedTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.border, static_cast<GLsizei>(length), data);

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture != nullptr) {
        texture->SetTextureLevel(
            { imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, 1, GL_UNSIGNED_BYTE });
    }
    LOGD("WebGL compressedTexImage2D %{public}u", GetError_());
    return NVal::CreateNull(env).val_;
}

bool WebGLRenderingContextBaseImpl::CheckCompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, size_t imageSize)
{
    if (imgArg.border) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "imgArg.border %{public}d", imgArg.border);
        return false;
    }
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckTextureLevel failed");
        return false;
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr || texture->CheckImmutable()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
            "texture is nullptr or CheckImmutable failed");
        return false;
    }
    if (!CheckTexImageInternalFormat(env, imgArg.func, imgArg.format)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "CheckTexImageInternalFormat failed");
        return false;
    }
    result = CheckCompressedTexData(imgArg, imageSize);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckCompressedTexData failed");
        return false;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "imgArg.level %{public}d", imgArg.level);
        return false;
    }
    result = CheckCompressedTexSubDimensions(imgArg, texture);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckCompressedTexSubDimensions failed");
        return false;
    }
    return true;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg.Dump("WebGL compressedTexSubImage2D");
    WebGLReadBufferArg bufferData(env);
    GLvoid* data = nullptr;
    size_t length = 0;
    if (!NVal(env, srcData).IsNull()) {
        bool succ = bufferData.GenBufferData(srcData, BUFFER_DATA_FLOAT_32) == napi_ok;
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        bufferData.DumpBuffer(bufferData.GetBufferDataType());
        size_t bufferLen = bufferData.GetBufferLength();
        size_t elemSize = bufferData.GetBufferDataSize();
        if (elemSize == 0 || static_cast<size_t>(srcOffset) > SIZE_MAX / elemSize) {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "srcOffset out of bounds");
            return NVal::CreateNull(env).val_;
        }
        size_t offsetBytes = static_cast<size_t>(srcOffset) * elemSize;
        if (offsetBytes > bufferLen) {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "srcOffset out of bounds");
            return NVal::CreateNull(env).val_;
        }
        size_t elementCount = bufferLen / elemSize;
        size_t availableElements = elementCount - srcOffset;
        size_t selectedElements = srcLengthOverride == 0 ? availableElements : srcLengthOverride;
        if (selectedElements > availableElements || selectedElements > SIZE_MAX / elemSize) {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "source length out of bounds");
            return NVal::CreateNull(env).val_;
        }
        length = selectedElements * elemSize;
        uint8_t* source = bufferData.GetBuffer();
        if (source == nullptr && length > 0) {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "compressed texture source is nullptr");
            return NVal::CreateNull(env).val_;
        }
        data = (source == nullptr) ? nullptr : static_cast<void*>(source + offsetBytes);
    }
    bool succ = CheckCompressedTexSubImage2D(env, imgArg, length);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    if (length > static_cast<size_t>(std::numeric_limits<GLsizei>::max())) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "compressedTexSubImage2D length too large");
        return NVal::CreateNull(env).val_;
    }
    glCompressedTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, static_cast<GLsizei>(length), data);
    LOGD("WebGL compressedTexSubImage2D result: %{public}u", GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg.Dump("WebGL compressedTexSubImage2D");
    if (boundBufferIds_[BoundBufferType::PIXEL_UNPACK_BUFFER] == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "no PIXEL_UNPACK_BUFFER bound");
        return NVal::CreateNull(env).val_;
    }
    if (offset < 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "offset is negative");
        return NVal::CreateNull(env).val_;
    }
    if (imageSize < 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "imageSize is negative");
        return NVal::CreateNull(env).val_;
    }
    WebGLBuffer* pboBuf = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::PIXEL_UNPACK_BUFFER]);
    if (pboBuf == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "PBO is null");
        return NVal::CreateNull(env).val_;
    }
    uint64_t pboSize = static_cast<uint64_t>(pboBuf->GetBufferSize());
    uint64_t offset64 = static_cast<uint64_t>(offset);
    uint64_t imageSize64 = static_cast<uint64_t>(imageSize);
    if (offset64 > pboSize || imageSize64 > pboSize - offset64) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "offset + imageSize exceeds PBO size");
        return NVal::CreateNull(env).val_;
    }
    bool succ = CheckCompressedTexSubImage2D(env, imgArg, imageSize);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    glCompressedTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, imageSize, reinterpret_cast<void*>(offset));
    LOGD("WebGL compressedTexSubImage2D result %{public}u", GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::CopyTexImage2D(napi_env env, const CopyTexImage2DArg& imgArg)
{
    if (imgArg.border) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "imgArg.border %{public}d", imgArg.border);
        return NVal::CreateNull(env).val_;
    }
    imgArg.Dump("WebGL copyTexImage2D");
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckTextureLevel failed");
        return NVal::CreateNull(env).val_;
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture || texture->CheckImmutable()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
            "texture is nullptr or CheckImmutable failed");
        return NVal::CreateNull(env).val_;
    }
    result = CheckTextureFormatAndType(env,
        imgArg.internalFormat, imgArg.internalFormat, GL_UNSIGNED_BYTE, imgArg.level);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckTextureFormatAndType failed");
        return NVal::CreateNull(env).val_;
    }
    result = CheckTexFuncDimensions(imgArg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckTexFuncDimensions failed");
        return NVal::CreateNull(env).val_;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "imgArg.level %{public}d %{public}d", imgArg.level, IsHighWebGL());
        return NVal::CreateNull(env).val_;
    }
    GLuint frameBufferId = 0;
    result = CheckReadBufferAndGetInfo(env, &frameBufferId, nullptr, nullptr);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckReadBufferAndGetInfo failed");
        return NVal::CreateNull(env).val_;
    }

    glCopyTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.x, imgArg.y, imgArg.width,
        imgArg.height, imgArg.border);

    texture->SetTextureLevel({ imgArg.target, imgArg.level, imgArg.internalFormat,
        imgArg.width, imgArg.height, 1, 1 });
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::CopyTexSubImage2D(napi_env env, const CopyTexSubImageArg& imgArg)
{
    imgArg.Dump("WebGL copyTexSubImage2D");
    GLenum result = CheckCopyTexSubImage(env, imgArg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckCopyTexSubImage failed");
        return NVal::CreateNull(env).val_;
    }

    GLuint frameBufferId = 0;
    result = CheckReadBufferAndGetInfo(env, &frameBufferId, nullptr, nullptr);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "CheckReadBufferAndGetInfo failed");
        return NVal::CreateNull(env).val_;
    }
    glCopyTexSubImage2D(
        imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.x, imgArg.y, imgArg.width, imgArg.height);
    return NVal::CreateNull(env).val_;
}

GLenum WebGLRenderingContextBaseImpl::CheckTextureLevel(GLenum target, GLint level)
{
    GLint max = WebGLTexture::GetMaxTextureLevelForTarget(target, IsHighWebGL());
    if (max <= 0) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if ((level < 0) || level > max) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckTexImage(napi_env env, const TexImageArg& imgArg, WebGLTexture* texture)
{
    if (imgArg.border) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    GLenum internalFormat = imgArg.internalFormat;
    if (imgArg.internalFormat == 0) {
        internalFormat = texture->GetInternalFormat(imgArg.target, imgArg.level);
    }
    if (internalFormat == 0) {
        LOGE("Invalid internalFormat %{public}u", internalFormat);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    result = CheckTextureFormatAndType(env, internalFormat, imgArg.format, imgArg.type, imgArg.level);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    result = CheckTexFuncDimensions(imgArg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("Invalid texture dimension or type %{public}u", result);
        return result;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, WebGLTexture* texture)
{
    if (imgArg.xOffset < 0 || imgArg.yOffset < 0) {
        LOGE("WebGL CheckTexSubImage2D invalid xOffset %{public}d", imgArg.xOffset);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (!WebGLTexture::CheckTextureSize(
            imgArg.xOffset, imgArg.width, texture->GetWidth(imgArg.target, imgArg.level)) ||
        !WebGLTexture::CheckTextureSize(
            imgArg.yOffset, imgArg.height, texture->GetHeight(imgArg.target, imgArg.level))) {
        LOGE("WebGL invalid CheckTexSubImage2D GetWidth %{public}d, GetHeight %{public}d",
            texture->GetWidth(imgArg.target, imgArg.level), texture->GetHeight(imgArg.target, imgArg.level));
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (!IsHighWebGL() && texture->GetType(imgArg.target, imgArg.level) != imgArg.type) {
        LOGE("WebGL invalid CheckTexSubImage2D type %{public}u", imgArg.type);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::CheckTexImageInternalFormat(napi_env env, int32_t func, GLenum internalFormat)
{
    return CheckInList(internalFormat, WebGLRenderingContextBaseImpl::GetTexImageInternalFormat());
}

GLenum WebGLRenderingContextBaseImpl::CheckTexFuncDimensions(const TexImageArg& imgArg)
{
    if (imgArg.width < 0 || imgArg.height < 0 || imgArg.depth < 0) {
        LOGE("Invalid offset or size ");
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    switch (imgArg.target) {
        case GL_TEXTURE_2D:
            if (static_cast<GLuint>(imgArg.width) > (static_cast<GLuint>(maxTextureSize_) >> imgArg.level) ||
                static_cast<GLuint>(imgArg.height) > (static_cast<GLuint>(maxTextureSize_) >> imgArg.level)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            if (imgArg.func != IMAGE_TEX_SUB_IMAGE_2D && imgArg.width != imgArg.height) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            if (static_cast<uint32_t>(imgArg.width) > (static_cast<uint32_t>(maxCubeMapTextureSize_) >>
                static_cast<uint32_t>(imgArg.level))) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexDimensions(const TexImageArg& imgArg)
{
    GLenum result = CheckTexFuncDimensions(imgArg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("Invalid tex dimensions %{public}u %{public}d", imgArg.target, imgArg.level);
        return result;
    }
    if (CheckInList(imgArg.internalFormat, GetExtentionAstcTexImageInternal())) {
        return WebGLRenderingContextBase::NO_ERROR;
    }
    bool widthValid = true;
    bool heightValid = true;
    switch (imgArg.internalFormat) {
#ifdef GC3D_COMPRESSED_ATC_RGB_AMD
        case GC3D_COMPRESSED_ATC_RGB_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
#endif
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: {
            widthValid = (imgArg.level && imgArg.width == 1) || (imgArg.level && imgArg.width == 2) ||
                !(imgArg.width % 4); // 1 2 4 ALIGNMENT
            heightValid = (imgArg.level && imgArg.height == 1) || (imgArg.level && imgArg.height == 2) ||
                !(imgArg.height % 4); // 1 2 4 ALIGNMENT
            break;
        }
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: {
            widthValid = imgArg.width > 0 &&
                (static_cast<uint32_t>(imgArg.width) & static_cast<uint32_t>(imgArg.width - 1)) == 0;
            heightValid = imgArg.height > 0 &&
                (static_cast<uint32_t>(imgArg.height) & static_cast<uint32_t>(imgArg.height - 1)) == 0;
            break;
        }
        default:
            return WebGLRenderingContextBase::NO_ERROR;
    }

    if (!widthValid || !heightValid) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexData(const TexImageArg& imgArg, size_t dataLen)
{
    if (imgArg.width < 0 || imgArg.height < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    size_t bytesRequired = 0;
    GLenum err = WebGLRenderingContextBase::NO_ERROR;
    if (!ComputeCompressedBytesRequired(imgArg, bytesRequired, err)) {
        return err;
    }
    LOGD("CheckCompressedTexData bytesRequired %{public}zu", bytesRequired);
    if (dataLen != bytesRequired) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexSubDimensions(
    const TexSubImage2DArg& imgArg, WebGLTexture* texture)
{
    if (imgArg.xOffset < 0 || imgArg.yOffset < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    switch (imgArg.format) {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: {
            const int32_t kBlockWidth = 4;
            const int32_t kBlockHeight = 4;
            if ((imgArg.xOffset % kBlockWidth) || (imgArg.yOffset % kBlockHeight)) {
                return WebGLRenderingContextBase::INVALID_OPERATION;
            }
            if (WebGLArg::CheckOverflow<GLint, GLint>(imgArg.xOffset, imgArg.width) ||
                WebGLArg::CheckOverflow<GLint, GLint>(imgArg.yOffset, imgArg.height)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }

            if ((imgArg.xOffset + imgArg.width) > texture->GetWidth(imgArg.target, imgArg.level) ||
                (imgArg.yOffset + imgArg.height) > texture->GetHeight(imgArg.target, imgArg.level)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            return CheckCompressedTexDimensions(imgArg);
        }
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: {
            if ((imgArg.xOffset != 0) || (imgArg.yOffset != 0)) {
                return WebGLRenderingContextBase::INVALID_OPERATION;
            }
            if (imgArg.width != texture->GetWidth(imgArg.target, imgArg.level) ||
                imgArg.height != texture->GetHeight(imgArg.target, imgArg.level)) {
                return WebGLRenderingContextBase::INVALID_OPERATION;
            }
            return CheckCompressedTexDimensions(imgArg);
        }
        case GL_ETC1_RGB8_OES: {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckTextureDataBuffer(
    const TexImageArg& arg, const WebGLReadBufferArg* bufferData)
{
    if (!bufferData) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (WebGLTexture::ChangeToBufferDataType(arg.type) != bufferData->GetBufferDataType()) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCopyTexSubImage(napi_env env, const CopyTexSubImageArg& arg)
{
    GLenum result = CheckTextureLevel(arg.target, arg.level);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }

    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (texture == nullptr) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (!WebGLTexture::CheckTextureSize(arg.xOffset, arg.width, texture->GetWidth(arg.target, arg.level)) ||
        !WebGLTexture::CheckTextureSize(arg.yOffset, arg.height, texture->GetHeight(arg.target, arg.level))) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (arg.func == IMAGE_COPY_TEX_SUB_IMAGE_3D) {
        const CopyTexSubImage3DArg* img3D = reinterpret_cast<const CopyTexSubImage3DArg*>(&arg);
        if (!WebGLTexture::CheckTextureSize(img3D->zOffset, img3D->depth, texture->GetDepth(arg.target, arg.level))) {
            return WebGLRenderingContextBase::INVALID_VALUE;
        }
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckDrawState(napi_env env)
{
    return WebGLRenderingContextBase::NO_ERROR;
}

WebGLRenderingContextBaseImpl::AttribRequirement WebGLRenderingContextBaseImpl::GetAttribRequirement(GLenum type)
{
    switch (type) {
        case GL_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
            return AttribRequirement::SIGNED_INTEGER;
        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_INT_VEC2:
        case GL_UNSIGNED_INT_VEC3:
        case GL_UNSIGNED_INT_VEC4:
            return AttribRequirement::UNSIGNED_INTEGER;
        default:
            return AttribRequirement::FLOAT;
    }
}

bool WebGLRenderingContextBaseImpl::CheckVertexAttribType(
    const VertexAttribInfo& info, AttribRequirement requirement)
{
    bool typeMatches = requirement == AttribRequirement::FLOAT ? !info.integer : info.integer;
    if (requirement == AttribRequirement::SIGNED_INTEGER) {
        return typeMatches && IsSignedIntegerAttribType(info.glType);
    }
    if (requirement == AttribRequirement::UNSIGNED_INTEGER) {
        return typeMatches && IsUnsignedIntegerAttribType(info.glType);
    }
    return typeMatches;
}

GLenum WebGLRenderingContextBaseImpl::BuildActiveAttribRequirements(
    std::vector<AttribRequirement>& requirements)
{
    if (arrayVertexAttribs_.size() < maxVertexAttribs_) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    requirements.assign(maxVertexAttribs_, AttribRequirement::NONE);
    GLint activeCount = 0;
    GLint maxNameLength = 0;
    glGetProgramiv(currentProgramId_, GL_ACTIVE_ATTRIBUTES, &activeCount);
    glGetProgramiv(currentProgramId_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);
    if (activeCount < 0 || (activeCount > 0 &&
        (maxNameLength <= 0 || maxNameLength > static_cast<GLint>(MAX_ATTRIBUTE_NAME_LENGTH)))) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    std::array<GLchar, MAX_ATTRIBUTE_NAME_LENGTH> name {};
    for (GLint index = 0; index < activeCount; ++index) {
        GLsizei nameLength = 0;
        GLint arraySize = 0;
        GLenum type = 0;
        glGetActiveAttrib(currentProgramId_, static_cast<GLuint>(index), maxNameLength,
            &nameLength, &arraySize, &type, name.data());
        if (nameLength <= 0 || nameLength >= maxNameLength) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        name[static_cast<size_t>(nameLength)] = '\0';
        GLint location = glGetAttribLocation(currentProgramId_, name.data());
        size_t locationCount = GetAttribLocationCount(type, arraySize);
        if (location < 0 || locationCount == 0 || static_cast<size_t>(location) > requirements.size() ||
            locationCount > requirements.size() - static_cast<size_t>(location)) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        std::fill_n(requirements.begin() + location, locationCount, GetAttribRequirement(type));
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckVertexAttribBufferRange(napi_env env, size_t index,
    AttribRequirement requirement, uint64_t maxVertex, bool hasVertices, GLsizei instanceCount)
{
    const VertexAttribInfo& info = arrayVertexAttribs_[index];
    if (!info.enabled) {
        if ((requirement == AttribRequirement::SIGNED_INTEGER && info.type != BUFFER_DATA_INT_32) ||
            (requirement == AttribRequirement::UNSIGNED_INTEGER && info.type != BUFFER_DATA_UINT_32)) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        return WebGLRenderingContextBase::NO_ERROR;
    }
    if (!CheckVertexAttribType(info, requirement) || info.bufferId == 0 || info.size <= 0 || info.glType == 0 ||
        info.offset < 0) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    if (!hasVertices || (info.divisor != 0 && instanceCount == 0)) {
        return WebGLRenderingContextBase::NO_ERROR;
    }
    WebGLBuffer* buffer = GetObjectInstance<WebGLBuffer>(env, info.bufferId);
    if (buffer == nullptr) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    size_t requiredEnd = 0;
    if (!ComputeVertexAttribRequiredEnd(info, maxVertex, instanceCount, requiredEnd) ||
        requiredEnd > buffer->GetBufferSize()) {
        LOGE("WebGL draw vertex attribute %{public}zu exceeds buffer", index);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckVertexAttribBufferRanges(napi_env env,
    const std::vector<AttribRequirement>& requirements, uint64_t maxVertex, bool hasVertices,
    GLsizei instanceCount)
{
    for (size_t index = 0; index < requirements.size(); ++index) {
        if (requirements[index] == AttribRequirement::NONE) {
            continue;
        }
        GLenum result = CheckVertexAttribBufferRange(
            env, index, requirements[index], maxVertex, hasVertices, instanceCount);
        if (result != WebGLRenderingContextBase::NO_ERROR) {
            return result;
        }
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckVertexAttribBuffers(
    napi_env env, uint64_t maxVertex, bool hasVertices, GLsizei instanceCount)
{
    std::vector<AttribRequirement> requirements;
    GLenum result = BuildActiveAttribRequirements(requirements);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    return CheckVertexAttribBufferRanges(env, requirements, maxVertex, hasVertices, instanceCount);
}

GLenum WebGLRenderingContextBaseImpl::CheckDrawArrays(
    napi_env env, GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
    if (!CheckDrawMode(env, mode)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (!CheckStencil(env)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (first < 0 || count < 0 || instanceCount < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (!currentProgramId_) {
        LOGE("WebGL drawArrays no valid shader program in use");
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    bool hasVertices = count > 0 && instanceCount > 0;
    uint64_t maxVertex = hasVertices ?
        static_cast<uint64_t>(first) + static_cast<uint64_t>(count) - 1 : 0;
    GLenum result = CheckVertexAttribBuffers(env, maxVertex, hasVertices, instanceCount);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    result = CheckDrawState(env);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    return CheckFrameBufferBoundComplete(env);
}

GLenum WebGLRenderingContextBaseImpl::CheckDrawElementsArgs(napi_env env, const DrawElementArg& arg,
    GLsizei instanceCount, uint32_t& indexSize, WebGLBuffer*& indexBuffer)
{
    if (!CheckDrawMode(env, arg.mode)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (!CheckStencil(env)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    indexSize = 1;
    switch (arg.type) {
        case WebGLRenderingContextBase::UNSIGNED_BYTE:
            break;
        case WebGLRenderingContextBase::UNSIGNED_SHORT:
            indexSize = sizeof(short);
            break;
        case WebGLRenderingContextBase::UNSIGNED_INT: {
            indexSize = sizeof(int);
            if (IsHighWebGL()) {
                break;
            }
            [[fallthrough]];
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (arg.count < 0 || arg.offset < 0 || instanceCount < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if ((arg.offset % static_cast<int64_t>(indexSize)) != 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    indexBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::ELEMENT_ARRAY_BUFFER]);
    if (indexBuffer == nullptr || indexBuffer->GetBufferSize() == 0) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    // check count
    uint64_t requiredBytes = static_cast<uint64_t>(indexSize) * static_cast<uint64_t>(arg.count);
    uint64_t bufferSize = static_cast<uint64_t>(indexBuffer->GetBufferSize());
    if (requiredBytes > bufferSize || static_cast<uint64_t>(arg.offset) > bufferSize - requiredBytes) {
        LOGE("WebGL drawElements Insufficient buffer size %{public}d", arg.count);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (!currentProgramId_) {
        LOGE("WebGL drawArrays no valid shader program in use");
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::GetMaxReferencedVertex(const WebGLBuffer* indexBuffer,
    const DrawElementArg& arg, uint32_t indexSize, uint64_t& maxVertex, bool& hasReferencedVertex)
{
    const uint8_t* indexData = indexBuffer->GetIndexData();
    if (arg.count > 0 && indexData == nullptr) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    const uint64_t restartIndex = arg.type == WebGLRenderingContextBase::UNSIGNED_BYTE ?
        static_cast<uint64_t>(UINT8_MAX) :
        (arg.type == WebGLRenderingContextBase::UNSIGNED_SHORT ? static_cast<uint64_t>(UINT16_MAX) :
        static_cast<uint64_t>(UINT32_MAX));
    for (GLsizei index = 0; index < arg.count; ++index) {
        uint64_t value = 0;
        const uint8_t* source =
            indexData + static_cast<size_t>(arg.offset) + static_cast<size_t>(index) * indexSize;
        if (indexSize == sizeof(uint8_t)) {
            value = *source;
        } else if (indexSize == sizeof(uint16_t)) {
            uint16_t item = 0;
            if (memcpy_s(&item, sizeof(item), source, sizeof(item)) != EOK) {
                return WebGLRenderingContextBase::INVALID_OPERATION;
            }
            value = item;
        } else {
            uint32_t item = 0;
            if (memcpy_s(&item, sizeof(item), source, sizeof(item)) != EOK) {
                return WebGLRenderingContextBase::INVALID_OPERATION;
            }
            value = item;
        }
        if (IsHighWebGL() && value == restartIndex) {
            continue;
        }
        hasReferencedVertex = true;
        maxVertex = std::max(maxVertex, value);
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckDrawElements(
    napi_env env, GLenum mode, GLsizei count, GLenum type, int64_t offset, GLsizei instanceCount)
{
    uint32_t indexSize = 0;
    WebGLBuffer* indexBuffer = nullptr;
    DrawElementArg arg { mode, count, type, offset };
    GLenum result = CheckDrawElementsArgs(env, arg, instanceCount, indexSize, indexBuffer);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    bool hasVertices = count > 0 && instanceCount > 0;
    bool hasReferencedVertex = false;
    uint64_t maxVertex = 0;
    result = GetMaxReferencedVertex(indexBuffer, arg, indexSize, maxVertex, hasReferencedVertex);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    if (IsHighWebGL() && count > 0) {
        GLint64 maxElementIndex = 0;
        glGetInteger64v(GL_MAX_ELEMENT_INDEX, &maxElementIndex);
        if (maxElementIndex < 0 || maxVertex > static_cast<uint64_t>(maxElementIndex)) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
    }
    hasVertices = hasVertices && hasReferencedVertex;
    result = CheckVertexAttribBuffers(env, maxVertex, hasVertices, instanceCount);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    result = CheckDrawState(env);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        return result;
    }
    return CheckFrameBufferBoundComplete(env);
}

GLenum WebGLRenderingContextBaseImpl::CheckReadBufferAndGetInfo(
    napi_env env, GLuint* frameBufferId, GLenum* format, GLenum* type)
{
    GLenum target = IsHighWebGL() ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER;
    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, target);
    if (frameBuffer) {
        LOGD("CheckReadBufferAndGetInfo frameBuffer %{public}u", frameBuffer->GetFramebuffer());
        if (frameBuffer->CheckStatus(env, this) != WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE) {
            LOGE("CheckStatus not FRAMEBUFFER_COMPLETE");
            return WebGLRenderingContextBase::INVALID_FRAMEBUFFER_OPERATION;
        }
        if (!GetReadBufferFormatAndType(env, frameBuffer, format, type)) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        *frameBufferId = frameBuffer->GetFramebuffer();
    } else {
        if (defaultReadBufferMode_ == GL_NONE) {
            LOGE("defaultReadBufferMode_ %{public}u", defaultReadBufferMode_);
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        if (format) {
            *format = GL_RGBA;
        }
        if (type) {
            *type = GL_UNSIGNED_BYTE;
        }
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::GetReadBufferFormatAndType(
    napi_env env, const WebGLFramebuffer* frameBuffer, GLenum* format, GLenum* type)
{
    GLenum mode = frameBuffer->GetReadBufferMode();
    LOGD("GetReadBufferFormatAndType mode %{public}u", mode);
    if (mode == GL_NONE) {
        return false;
    }
    WebGLAttachment* attachedObject = frameBuffer->GetAttachment(mode);
    if (!attachedObject) {
        LOGE("GetReadBufferFormatAndType no attachment %{public}u", mode);
        return false;
    }
    WebGLAttachmentInfo info = {};
    if (!frameBuffer->GetWebGLAttachmentInfo(env, this, attachedObject, info)) {
        LOGE("GetReadBufferFormatAndType no attachment info %{public}u", mode);
        return false;
    }
    if (format) {
        *format = info.format;
    }
    if (type) {
        *type = info.type;
    }
    return true;
}

GLenum WebGLRenderingContextBaseImpl::CheckTextureFormatAndType(
    napi_env env, GLenum internalFormat, GLenum format, GLenum type, GLint level)
{
    LOGD("internalFormat %{public}u format %{public}u type %{public}u %{public}d",
        internalFormat, format, type, level);
    if (!CheckInList(internalFormat, WebGLTexture::GetSupportedInternalFormats())) {
        LOGE("Invalid internalFormat %{public}u ", internalFormat);
        return GL_INVALID_ENUM;
    }
    if (!CheckInList(format, WebGLTexture::GetSupportedFormats())) {
        LOGE("Invalid format %{public}u ", format);
        return GL_INVALID_ENUM;
    }
    if (!CheckInList(type, WebGLTexture::GetSupportedTypes())) {
        LOGE("Invalid type %{public}u ", type);
        return GL_INVALID_ENUM;
    }

    TextureFormatTypeMap map = { internalFormat, format, type };
    if (WebGLTexture::GetSupportedFormatTypeMaps().find(map) == WebGLTexture::GetSupportedFormatTypeMaps().end()) {
        LOGE("Invalid format type ");
        return GL_INVALID_OPERATION;
    }

    if ((format == GL_DEPTH_COMPONENT || format == GL_DEPTH_STENCIL_OES) && level > 0 && !IsHighWebGL()) {
        return GL_INVALID_OPERATION;
    }
    return 0;
}
} // namespace Impl
} // namespace Rosen
} // namespace OHOS

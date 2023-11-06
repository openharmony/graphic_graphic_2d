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

#include "context/webgl_rendering_context_base.h"
#include "context/webgl2_rendering_context_base.h"
#include "napi/n_class.h"
#include "util/log.h"
#include "util/object_source.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
#define SET_ERROR(error) \
    do { \
        LOGE("WebGL set error code %{public}u", error); \
        SetError(error); \
    } while (0)

#define SET_ERROR_WITH_LOG(error, info, ...) \
    do {                      \
        LOGE("WebGL error code %{public}u " info, error, ##__VA_ARGS__); \
        SetError(error); \
    } while (0)

void WebGLRenderingContextBaseImpl::TexImage2D_(
    const TexImageArg& imgArg, WebGLTexture* texture, const void* pixels, bool changeUnpackAlignment)
{
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    LOGD("WebGL TexImage2D_ target %{public}u result %{public}u", imgArg.target, GetError_());
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
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(error);
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        error =imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height }, pixels, srcOffset);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D invalid pixels");
            return nullptr;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData();
    }
    TexImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
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
            return nullptr;
        }
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }
    imgArg.Dump("WebGL texImage2D");

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "Can not find texture");
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texImage2D checkTexImage failed");
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "WebGL texImage2D checkImmutable failed");
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL texImage2D checkNPOT failed");
        return nullptr;
    }
    if (imgArg.type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg.type = GL_FLOAT;
    }

    TexImage2D_(imgArg, texture, data, unpackAlignment_ != 1);
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg& imgArg, GLintptr pbOffset)
{
    imgArg.Dump("WebGL texImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "Can not find texture");
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texImage2D error");
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.border,
        imgArg.format, imgArg.type, reinterpret_cast<GLvoid*>(pbOffset));
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
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
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(error);
        return nullptr;
    }
    glTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, imgArg.type, reinterpret_cast<void *>(pbOffset));
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg& info, napi_value pixels, GLuint srcOffset)
{
    TexSubImage2DArg& imgArg = const_cast<TexSubImage2DArg&>(info);
    imgArg.Dump("WebGL texSubImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D error");
        return nullptr;
    }

    if (!texture->CheckValid(imgArg.target, imgArg.level)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, " invalid texture level");
        return nullptr;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        error = imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height }, pixels, srcOffset);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D invalid pixels");
            return nullptr;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }

    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D error");
        return nullptr;
    }

    if (imgArg.type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg.type = GL_FLOAT;
    }
    TexSubImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg& info, napi_value imageData)
{
    TexSubImage2DArg imgArg(info);
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "Can not find texture");
        return nullptr;
    }
    GLvoid* data = nullptr;
    GLenum error = 0;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    if (!NVal(env, imageData).IsNull()) {
        error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, imageData);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D Image source invalid");
            return nullptr;
        }
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }
    imgArg.Dump("WebGL texSubImage2D");
    error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D CheckTexImage failed");
        return nullptr;
    }
    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D CheckTexSubImage2D failed");
        return nullptr;
    }
    TexSubImage2D_(imgArg, texture, data, unpackAlignment_);
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawElements(
    napi_env env, GLenum mode, GLsizei count, GLenum type, GLintptr offset)
{
    LOGD("WebGL drawElements mode %{public}u %{public}u %{public}u %{public}u", mode, count, type, offset);
    GLenum result = CheckDrawElements(env, mode, count, type, static_cast<int64_t>(offset));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "WebGL drawElements failed");
        return nullptr;
    }
    glDrawElements(mode, count, type, reinterpret_cast<GLvoid*>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count)
{
    LOGD("WebGL drawArrays mode %{public}u %{public}u %{public}u error %{public}u", mode, first, count, GetError_());
    GLenum result = CheckDrawArrays(env, mode, first, count);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    glDrawArrays(mode, first, count);
    LOGD("WebGL drawArrays result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(napi_env env, const PixelsArg& arg, GLintptr offset)
{
    arg.Dump("WebGL readPixels");
    if (!IsHighWebGL()) {
        return nullptr;
    }

    WebGLBuffer* buffer = GetBoundBuffer(env, WebGL2RenderingContextBase::PIXEL_PACK_BUFFER);
    if (buffer == nullptr || buffer->GetBufferSize() == 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    uint64_t size = static_cast<uint64_t>(buffer->GetBufferSize());
    if (size < offset) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    size = size - offset;

    GLenum result = CheckReadPixelsArg(env, arg, size);
    if (!result) {
        SET_ERROR(result);
        return nullptr;
    }
    glReadPixels(arg.x, arg.y, arg.width, arg.height, arg.format, arg.type, reinterpret_cast<void*>(offset));
    LOGD("WebGL readPixels result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(
    napi_env env, const PixelsArg& arg, napi_value buffer, GLuint dstOffset)
{
    arg.Dump("WebGL readPixels");
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLenum result = CheckReadPixelsArg(env, arg, static_cast<int64_t>(bufferData.GetBufferLength()));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }

    LOGD("WebGL readPixels pixels %{public}u", dstOffset);
    glReadPixels(arg.x, arg.y, arg.width, arg.height, arg.format, arg.type,
        static_cast<void*>(bufferData.GetBuffer() + dstOffset));
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    LOGD("WebGL readPixels result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData_(
    napi_env env, GLenum target, GLsizeiptr size, GLenum usage, const uint8_t* bufferData)
{
    LOGD("WebGL bufferData target %{public}u, usage %{public}u size %{public}u", target, usage, size);
    uint32_t index = 0;
    if (!CheckBufferTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[index]);
    if (webGLBuffer == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!CheckBufferDataUsage(env, usage)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (webGLBuffer->GetTarget() != target) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    webGLBuffer->SetBuffer(size, nullptr);
    if (bufferData != nullptr) {
        webGLBuffer->SetBuffer(size, bufferData);
        glBufferData(target, size, bufferData, usage);
    } else {
        glBufferData(target, size, nullptr, usage);
    }
    webGLBuffer->SetTarget(target);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(napi_env env, GLenum target, int64_t size, GLenum usage)
{
    LOGD("WebGL bufferData target %{public}u, usage %{public}u size %{public}u", target, usage, size);
    BufferData_(env, target, static_cast<GLsizeiptr>(size), usage, nullptr);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(
    napi_env env, GLenum target, napi_value data, GLenum usage, const BufferExt& ext)
{
    LOGD("WebGL bufferData target %{public}u, usage %{public}u ext: %{public}u %{public}u",
        target, usage, ext.offset, ext.length);
    WebGLReadBufferArg bufferData(env);
    if (NVal(env, data).IsNull()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bool succ = bufferData.GenBufferData(data, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ || bufferData.GetBufferType() == BUFFER_ARRAY) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGD("WebGL bufferData buffer size %{public}zu", bufferData.GetBufferLength());
    // change
    GLuint srcOffset = static_cast<GLuint>(ext.offset * bufferData.GetBufferDataSize());
    GLsizeiptr length = (ext.length == 0) ? static_cast<GLsizeiptr>(bufferData.GetBufferLength())
                        : static_cast<GLsizeiptr>(ext.length * bufferData.GetBufferDataSize());
    BufferData_(env, target, length, usage, bufferData.GetBuffer() + srcOffset);
    LOGD("WebGL bufferData target %{public}u, result %{public}u ", target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferSubData(
    napi_env env, GLenum target, GLintptr offset, napi_value buffer, const BufferExt& ext)
{
    LOGD("WebGL bufferSubData target %{public}u offset %{public}u %{public}u %{public}u",
        target, offset, ext.offset, ext.length);

    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::ARRAY_BUFFER]);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferSize() == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "bufferSubData Can not find bound buffer");
        return nullptr;
    }

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL bufferSubData invalid buffer data");
        return nullptr;
    }
    // check sub buffer
    if ((static_cast<size_t>(offset) + bufferData.GetBufferLength()) > webGLBuffer->GetBufferSize()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL bufferSubData invalid buffer size %{public}zu offset %{public}zu ",
            bufferData.GetBufferLength(), webGLBuffer->GetBufferSize());
        return nullptr;
    }

    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    LOGD("WebGL bufferSubData size %{public}zu", bufferData.GetBufferLength());

    glBufferSubData(target, offset, static_cast<GLsizeiptr>(bufferData.GetBufferLength()),
        static_cast<uint8_t*>(bufferData.GetBuffer()));
    LOGD("WebGL bufferSubData target %{public}u, result %{public}u ", target, GetError_());
    return nullptr;
}

} // namespace Impl
} // namespace Rosen
} // namespace OHOS
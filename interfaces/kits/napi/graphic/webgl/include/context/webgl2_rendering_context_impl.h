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

#ifndef WEBGL2_RENDERING_CONTEXT_IMPL_H
#define WEBGL2_RENDERING_CONTEXT_IMPL_H

#include "napi/n_exporter.h"
#include "webgl/webgl_texture.h"
#include "webgl_rendering_context_base_impl.h"

namespace OHOS {
namespace Rosen {
namespace Impl {

enum BoundQueryType : uint32_t {
    ANY_SAMPLES_PASSED,
    TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
    QUERY_MAX
};

class WebGL2RenderingContextImpl : public WebGLRenderingContextBaseImpl {
public:
    explicit WebGL2RenderingContextImpl(int32_t version, WebGLRenderingContextBasicBase *base)
        : WebGLRenderingContextBaseImpl(version, base) {}
    ~WebGL2RenderingContextImpl() override {}

    void Init() override;

    napi_value CreateQuery(napi_env env);
    napi_value DeleteQuery(napi_env env, napi_value object);
    napi_value IsQuery(napi_env env, napi_value object);
    napi_value GetQuery(napi_env env, GLenum target, GLenum pName);
    napi_value BeginQuery(napi_env env, GLenum target, napi_value object);
    napi_value EndQuery(napi_env env, GLenum target);
    napi_value GetQueryParameter(napi_env env, napi_value queryObj, GLenum pName);

    napi_value CreateSampler(napi_env env);
    napi_value DeleteSampler(napi_env env, napi_value samplerObj);
    napi_value IsSampler(napi_env env, napi_value samplerObj);
    napi_value BindSampler(napi_env env, GLuint unit, napi_value samplerObj);
    napi_value SamplerParameter(napi_env env, napi_value samplerObj, GLenum pname, bool isFloat, void* param);
    napi_value GetSamplerParameter(napi_env env, napi_value samplerObj, GLenum pname);

    napi_value CreateVertexArray(napi_env env);
    napi_value DeleteVertexArray(napi_env env, napi_value object);
    napi_value IsVertexArray(napi_env env, napi_value object);
    napi_value BindVertexArray(napi_env env, napi_value object);

    napi_value FenceSync(napi_env env, GLenum condition, GLbitfield flags);
    napi_value IsSync(napi_env env, napi_value syncObj);
    napi_value DeleteSync(napi_env env, napi_value syncObj);
    napi_value GetSyncParameter(napi_env env, napi_value syncObj, GLenum pname);
    napi_value WaitSync(napi_env env, napi_value syncObj, GLbitfield flags, GLint64 timeout);
    napi_value ClientWaitSync(napi_env env, napi_value syncObj, GLbitfield flags, GLint64 timeout);

    napi_value CreateTransformFeedback(napi_env env);
    napi_value DeleteTransformFeedback(napi_env env, napi_value obj);
    napi_value IsTransformFeedback(napi_env env, napi_value obj);
    napi_value BindTransformFeedback(napi_env env, napi_value obj, GLenum target);
    napi_value BeginTransformFeedback(napi_env env, GLenum primitiveMode);
    napi_value EndTransformFeedback(napi_env env);
    napi_value GetTransformFeedbackVarying(napi_env env, napi_value programObj, GLuint index);

    napi_value TexStorage2D(napi_env env, const TexStorageArg& arg);
    napi_value TexStorage3D(napi_env env, const TexStorageArg& arg);
    napi_value TexImage3D(napi_env env, const TexImageArg& imgArg, napi_value imageSource);
    napi_value TexImage3D(napi_env env, const TexImageArg& imgArg, napi_value data, GLuint srcOffset);
    napi_value TexImage3D(napi_env env, const TexImageArg& imgArg, GLintptr pboOffset);
    napi_value TexSubImage3D(napi_env env, const TexSubImage3DArg& imgArg, napi_value imageSource);
    napi_value TexSubImage3D(napi_env env, const TexSubImage3DArg& imgArg, napi_value data, GLuint srcOffset);
    napi_value TexSubImage3D(napi_env env, const TexSubImage3DArg& imgArg, GLintptr pboOffset);
    napi_value CopyTexSubImage3D(napi_env env, const CopyTexSubImage3DArg& imgArg);
    napi_value CompressedTexImage3D(napi_env env, const TexImageArg& imgArg, GLsizei imageSize, GLintptr offset);
    napi_value CompressedTexImage3D(
        napi_env env, const TexImageArg& imgArg, napi_value data, GLuint srcOffset, GLuint srcLengthOverride);
    napi_value CompressedTexSubImage3D(
        napi_env env, const TexSubImage3DArg& imgArg, GLsizei imageSize, GLintptr offset);
    napi_value CompressedTexSubImage3D(
        napi_env env, const TexSubImage3DArg& imgArg, napi_value data, GLuint srcOffset, GLuint srcLengthOverride);
    napi_value ClearBufferV(
        napi_env env, GLenum buffer, GLint drawBuffer, napi_value value, int64_t srcOffset, BufferDataType type);
    napi_value ClearBufferfi(napi_env env, GLenum buffer, GLint drawBuffer, GLfloat depth, GLint stencil);
    napi_value GetIndexedParameter(napi_env env, GLenum target, GLuint index);
    napi_value GetFragDataLocation(napi_env env, napi_value programObj, const std::string& name);

    napi_value VertexAttribI4i(napi_env env, GLuint index, GLint* data);
    napi_value VertexAttribI4ui(napi_env env, GLuint index, GLuint* data);
    napi_value VertexAttribI4iv(napi_env env, GLuint index, napi_value data);
    napi_value VertexAttribI4uiv(napi_env env, GLuint index, napi_value data);
    napi_value VertexAttribIPointer(napi_env env, const VertexAttribArg& vertexInfo);
    napi_value VertexAttribDivisor(napi_env env, GLuint index, GLuint divisor);

    napi_value DrawBuffers(napi_env env, napi_value dataObj);
    napi_value DrawArraysInstanced(napi_env env, GLenum mode, GLint first, GLsizei count, GLsizei instanceCount);
    napi_value DrawElementsInstanced(napi_env env, const DrawElementArg& arg, GLsizei instanceCount);
    napi_value DrawRangeElements(napi_env env, const DrawElementArg& arg, GLuint start, GLuint end);

    napi_value CopyBufferSubData(napi_env env,
        GLenum targets[2], int64_t readOffset, int64_t writeOffset, int64_t size); // 2 read write
    napi_value GetBufferSubData(napi_env env, GLenum target, int64_t offset, napi_value data, const BufferExt& ext);
    napi_value BlitFrameBuffer(napi_env env, GLint data[8], GLbitfield mask, GLenum filter);
    napi_value FrameBufferTextureLayer(
        napi_env env, GLenum target, GLenum attachment, napi_value textureObj, const TextureLayerArg& layer);
    napi_value ReadBuffer(napi_env env, GLenum mode);
    napi_value RenderBufferStorageMultiSample(napi_env env, const TexStorageArg& arg, GLsizei samples);
    napi_value BindBufferBase(napi_env env, const BufferBaseArg& arg, napi_value bufferObj);
    napi_value BindBufferRange(
        napi_env env, const BufferBaseArg& arg, napi_value bufferObj, GLintptr offset, GLsizeiptr size);
    napi_value InvalidateFrameBuffer(napi_env env, GLenum target, napi_value data);
    napi_value InvalidateSubFrameBuffer(
        napi_env env, GLenum target, napi_value data, const BufferPosition& position, const BufferSize& size);
    napi_value GetInternalFormatParameter(napi_env env, GLenum target, GLenum internalFormat, GLenum pname);

    napi_value GetUniformBlockIndex(napi_env env, napi_value programObj, const std::string& uniformBlockName);
    napi_value UniformBlockBinding(
        napi_env env, napi_value programObj, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
    napi_value TransformFeedbackVaryings(napi_env env, napi_value programObj, napi_value data, GLenum bufferMode);
    napi_value GetUniformIndices(napi_env env, napi_value programObj, napi_value data);
    napi_value GetActiveUniforms(napi_env env, napi_value programObj, napi_value data, GLenum pname);
    napi_value GetActiveUniformBlockParameter(
        napi_env env, napi_value programObj, GLuint uniformBlockIndex, GLenum pname);
    napi_value GetActiveUniformBlockName(napi_env env, napi_value programObj, GLuint uniformBlockIndex);

    napi_value BindBuffer(napi_env env, GLenum target, napi_value object) override;
    napi_value DeleteBuffer(napi_env env, napi_value object) override;
    napi_value GetParameter(napi_env env, GLenum pname) override;
    napi_value GetTexParameter(napi_env env, GLenum target, GLenum pname) override;
    napi_value GetFrameBufferAttachmentParameter(napi_env env, GLenum target, GLenum attachment, GLenum pname) override;
    void DoObjectDelete(int32_t type, WebGLObject *obj) override;
private:
    WebGL2RenderingContextImpl(const WebGL2RenderingContextImpl&) = delete;
    WebGL2RenderingContextImpl& operator=(const WebGL2RenderingContextImpl&) = delete;

    napi_value GetFrameBufferAttachmentParameterForDefault(
        napi_env env, GLenum target, GLenum attachment, GLenum pname);
    GLenum CheckTexSubImage3D(napi_env env, const TexSubImage3DArg& arg);
    GLenum CheckTexImage3D(napi_env env, const TexImageArg& info);
    GLenum CheckTexStorage(napi_env env, const TexStorageArg& arg);
    GLenum CheckClearBuffer(napi_env env, GLenum buffer, const WebGLReadBufferArg& bufferData);

    bool CheckGetFrameBufferAttachmentParameter(
        napi_env env, GLenum target, GLenum attachment, const WebGLFramebuffer* frameBuffer);
    bool UpdateBaseTargetBoundBuffer(napi_env env, GLenum target, GLuint index, GLuint bufferId);
    bool CheckBufferTargetCompatibility(napi_env env, GLenum target, WebGLBuffer* buffer);
    bool CheckBufferBindTarget(GLenum target);
    bool CheckQueryTarget(napi_env env, GLenum target, uint32_t& index);
    bool CheckStorageInternalFormat(napi_env env, GLenum internalFormat);
    bool CheckTransformFeedbackBuffer(GLenum target, WebGLBuffer* buffer);
    napi_value HandleFrameBufferPname(
        napi_env env, GLenum target, GLenum attachment, GLenum pname, WebGLAttachment* attachmentObject);

    GLuint currentQuery_[BoundQueryType::QUERY_MAX] { 0 };
    GLuint boundReadFrameBuffer_ { 0 };
    GLuint boundTransformFeedback_ { 0 };
    GLuint boundVertexArrayId_ { 0 };

    // TRANSFORM_FEEDBACK_BUFFER
    std::map<GLint, GLuint> boundIndexedTransformFeedbackBuffers_ {};
    GLuint maxBoundTransformFeedbackBufferIndex_ { 0 };
    // UNIFORM_BUFFER
    std::map<GLint, GLuint> boundIndexedUniformBuffers_ {};
    GLuint maxBoundUniformBufferIndex_ { 0 };

    std::vector<GLuint> samplerUnits_ {};
    GLuint maxSamplerUnit_ {};
};
} // namespace Impl
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL2_RENDERING_CONTEXT_IMPL_H

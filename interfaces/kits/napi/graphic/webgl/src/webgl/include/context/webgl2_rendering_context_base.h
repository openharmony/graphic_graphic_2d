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

#ifndef ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT_BASE
#define ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT_BASE

#include "webgl_rendering_context_basic_base.h"
#include <GLES3/gl3.h>
#include <map>
#include "../../../common/napi/n_exporter.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class WebGL2RenderingContextBase : public WebGLRenderingContextBasicBase {
public:
    static const GLenum READ_BUFFER = 0x0C02;
    static const GLenum UNPACK_ROW_LENGTH = 0x0CF2;
    static const GLenum UNPACK_SKIP_ROWS = 0x0CF3;
    static const GLenum UNPACK_SKIP_PIXELS = 0x0CF4;
    static const GLenum PACK_ROW_LENGTH = 0x0D02;
    static const GLenum PACK_SKIP_ROWS = 0x0D03;
    static const GLenum PACK_SKIP_PIXELS = 0x0D04;
    static const GLenum COLOR = 0x1800;
    static const GLenum DEPTH = 0x1801;
    static const GLenum STENCIL = 0x1802;
    static const GLenum RED = 0x1903;
    static const GLenum RGB8 = 0x8051;
    static const GLenum RGBA8 = 0x8058;
    static const GLenum RGB10_A2 = 0x8059;
    static const GLenum TEXTURE_BINDING_3D = 0x806A;
    static const GLenum UNPACK_SKIP_IMAGES = 0x806D;
    static const GLenum UNPACK_IMAGE_HEIGHT = 0x806E;
    static const GLenum TEXTURE_3D = 0x806F;
    static const GLenum TEXTURE_WRAP_R = 0x8072;
    static const GLenum MAX_3D_TEXTURE_SIZE = 0x8073;
    static const GLenum UNSIGNED_INT_2_10_10_10_REV = 0x8368;
    static const GLenum MAX_ELEMENTS_VERTICES = 0x80E8;
    static const GLenum MAX_ELEMENTS_INDICES = 0x80E9;
    static const GLenum TEXTURE_MIN_LOD = 0x813A;
    static const GLenum TEXTURE_MAX_LOD = 0x813B;
    static const GLenum TEXTURE_BASE_LEVEL = 0x813C;
    static const GLenum TEXTURE_MAX_LEVEL = 0x813D;
    static const GLenum MIN = 0x8007;
    static const GLenum MAX = 0x8008;
    static const GLenum DEPTH_COMPONENT24 = 0x81A6;
    static const GLenum MAX_TEXTURE_LOD_BIAS = 0x84FD;
    static const GLenum TEXTURE_COMPARE_MODE = 0x884C;
    static const GLenum TEXTURE_COMPARE_FUNC = 0x884D;
    static const GLenum CURRENT_QUERY = 0x8865;
    static const GLenum QUERY_RESULT = 0x8866;
    static const GLenum QUERY_RESULT_AVAILABLE = 0x8867;
    static const GLenum STREAM_READ = 0x88E1;
    static const GLenum STREAM_COPY = 0x88E2;
    static const GLenum STATIC_READ = 0x88E5;
    static const GLenum STATIC_COPY = 0x88E6;
    static const GLenum DYNAMIC_READ = 0x88E9;
    static const GLenum DYNAMIC_COPY = 0x88EA;
    static const GLenum MAX_DRAW_BUFFERS = 0x8824;
    static const GLenum DRAW_BUFFER0 = 0x8825;
    static const GLenum DRAW_BUFFER1 = 0x8826;
    static const GLenum DRAW_BUFFER2 = 0x8827;
    static const GLenum DRAW_BUFFER3 = 0x8828;
    static const GLenum DRAW_BUFFER4 = 0x8829;
    static const GLenum DRAW_BUFFER5 = 0x882A;
    static const GLenum DRAW_BUFFER6 = 0x882B;
    static const GLenum DRAW_BUFFER7 = 0x882C;
    static const GLenum DRAW_BUFFER8 = 0x882D;
    static const GLenum DRAW_BUFFER9 = 0x882E;
    static const GLenum DRAW_BUFFER10 = 0x882F;
    static const GLenum DRAW_BUFFER11 = 0x8830;
    static const GLenum DRAW_BUFFER12 = 0x8831;
    static const GLenum DRAW_BUFFER13 = 0x8832;
    static const GLenum DRAW_BUFFER14 = 0x8833;
    static const GLenum DRAW_BUFFER15 = 0x8834;
    static const GLenum MAX_FRAGMENT_UNIFORM_COMPONENTS = 0x8B49;
    static const GLenum MAX_VERTEX_UNIFORM_COMPONENTS = 0x8B4A;
    static const GLenum SAMPLER_3D = 0x8B5F;
    static const GLenum SAMPLER_2D_SHADOW = 0x8B62;
    static const GLenum FRAGMENT_SHADER_DERIVATIVE_HINT = 0x8B8B;
    static const GLenum PIXEL_PACK_BUFFER = 0x88EB;
    static const GLenum PIXEL_UNPACK_BUFFER = 0x88EC;
    static const GLenum PIXEL_PACK_BUFFER_BINDING = 0x88ED;
    static const GLenum PIXEL_UNPACK_BUFFER_BINDING = 0x88EF;
    static const GLenum FLOAT_MAT2x3 = 0x8B65;
    static const GLenum FLOAT_MAT2x4 = 0x8B66;
    static const GLenum FLOAT_MAT3x2 = 0x8B67;
    static const GLenum FLOAT_MAT3x4 = 0x8B68;
    static const GLenum FLOAT_MAT4x2 = 0x8B69;
    static const GLenum FLOAT_MAT4x3 = 0x8B6A;
    static const GLenum SRGB = 0x8C40;
    static const GLenum SRGB8 = 0x8C41;
    static const GLenum SRGB8_ALPHA8 = 0x8C43;
    static const GLenum COMPARE_REF_TO_TEXTURE = 0x884E;
    static const GLenum RGBA32F = 0x8814;
    static const GLenum RGB32F = 0x8815;
    static const GLenum RGBA16F = 0x881A;
    static const GLenum RGB16F = 0x881B;
    static const GLenum VERTEX_ATTRIB_ARRAY_INTEGER = 0x88FD;
    static const GLenum MAX_ARRAY_TEXTURE_LAYERS = 0x88FF;
    static const GLenum MIN_PROGRAM_TEXEL_OFFSET = 0x8904;
    static const GLenum MAX_PROGRAM_TEXEL_OFFSET = 0x8905;
    static const GLenum MAX_VARYING_COMPONENTS = 0x8B4B;
    static const GLenum TEXTURE_2D_ARRAY = 0x8C1A;
    static const GLenum TEXTURE_BINDING_2D_ARRAY = 0x8C1D;
    static const GLenum R11F_G11F_B10F = 0x8C3A;
    static const GLenum UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B;
    static const GLenum RGB9_E5 = 0x8C3D;
    static const GLenum UNSIGNED_INT_5_9_9_9_REV = 0x8C3E;
    static const GLenum TRANSFORM_FEEDBACK_BUFFER_MODE = 0x8C7F;
    static const GLenum MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS = 0x8C80;
    static const GLenum TRANSFORM_FEEDBACK_VARYINGS = 0x8C83;
    static const GLenum TRANSFORM_FEEDBACK_BUFFER_START = 0x8C84;
    static const GLenum TRANSFORM_FEEDBACK_BUFFER_SIZE = 0x8C85;
    static const GLenum TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = 0x8C88;
    static const GLenum RASTERIZER_DISCARD = 0x8C89;
    static const GLenum MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS = 0x8C8A;
    static const GLenum MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS = 0x8C8B;
    static const GLenum INTERLEAVED_ATTRIBS = 0x8C8C;
    static const GLenum SEPARATE_ATTRIBS = 0x8C8D;
    static const GLenum TRANSFORM_FEEDBACK_BUFFER = 0x8C8E;
    static const GLenum TRANSFORM_FEEDBACK_BUFFER_BINDING = 0x8C8F;
    static const GLenum RGBA32UI = 0x8D70;
    static const GLenum RGB32UI = 0x8D71;
    static const GLenum RGBA16UI = 0x8D76;
    static const GLenum RGB16UI = 0x8D77;
    static const GLenum RGBA8UI = 0x8D7C;
    static const GLenum RGB8UI = 0x8D7D;
    static const GLenum RGBA32I = 0x8D82;
    static const GLenum RGB32I = 0x8D83;
    static const GLenum RGBA16I = 0x8D88;
    static const GLenum RGB16I = 0x8D89;
    static const GLenum RGBA8I = 0x8D8E;
    static const GLenum RGB8I = 0x8D8F;
    static const GLenum RED_INTEGER = 0x8D94;
    static const GLenum RGB_INTEGER = 0x8D98;
    static const GLenum RGBA_INTEGER = 0x8D99;
    static const GLenum SAMPLER_2D_ARRAY = 0x8DC1;
    static const GLenum SAMPLER_2D_ARRAY_SHADOW = 0x8DC4;
    static const GLenum SAMPLER_CUBE_SHADOW = 0x8DC5;
    static const GLenum UNSIGNED_INT_VEC2 = 0x8DC6;
    static const GLenum UNSIGNED_INT_VEC3 = 0x8DC7;
    static const GLenum UNSIGNED_INT_VEC4 = 0x8DC8;
    static const GLenum INT_SAMPLER_2D = 0x8DCA;
    static const GLenum INT_SAMPLER_3D = 0x8DCB;
    static const GLenum INT_SAMPLER_CUBE = 0x8DCC;
    static const GLenum INT_SAMPLER_2D_ARRAY = 0x8DCF;
    static const GLenum UNSIGNED_INT_SAMPLER_2D = 0x8DD2;
    static const GLenum UNSIGNED_INT_SAMPLER_3D = 0x8DD3;
    static const GLenum UNSIGNED_INT_SAMPLER_CUBE = 0x8DD4;
    static const GLenum UNSIGNED_INT_SAMPLER_2D_ARRAY = 0x8DD7;
    static const GLenum DEPTH_COMPONENT32F = 0x8CAC;
    static const GLenum DEPTH32F_STENCIL8 = 0x8CAD;
    static const GLenum FLOAT_32_UNSIGNED_INT_24_8_REV = 0x8DAD;
    static const GLenum FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING = 0x8210;
    static const GLenum FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE = 0x8211;
    static const GLenum FRAMEBUFFER_ATTACHMENT_RED_SIZE = 0x8212;
    static const GLenum FRAMEBUFFER_ATTACHMENT_GREEN_SIZE = 0x8213;
    static const GLenum FRAMEBUFFER_ATTACHMENT_BLUE_SIZE = 0x8214;
    static const GLenum FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE = 0x8215;
    static const GLenum FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE = 0x8216;
    static const GLenum FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE = 0x8217;
    static const GLenum FRAMEBUFFER_DEFAULT = 0x8218;
    static const GLenum UNSIGNED_INT_24_8 = 0x84FA;
    static const GLenum DEPTH24_STENCIL8 = 0x88F0;
    static const GLenum UNSIGNED_NORMALIZED = 0x8C17;
    static const GLenum DRAW_FRAMEBUFFER_BINDING = 0x8CA6;
    static const GLenum READ_FRAMEBUFFER = 0x8CA8;
    static const GLenum DRAW_FRAMEBUFFER = 0x8CA9;
    static const GLenum READ_FRAMEBUFFER_BINDING = 0x8CAA;
    static const GLenum RENDERBUFFER_SAMPLES = 0x8CAB;
    static const GLenum FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER = 0x8CD4;
    static const GLenum MAX_COLOR_ATTACHMENTS = 0x8CDF;
    static const GLenum COLOR_ATTACHMENT1 = 0x8CE1;
    static const GLenum COLOR_ATTACHMENT2 = 0x8CE2;
    static const GLenum COLOR_ATTACHMENT3 = 0x8CE3;
    static const GLenum COLOR_ATTACHMENT4 = 0x8CE4;
    static const GLenum COLOR_ATTACHMENT5 = 0x8CE5;
    static const GLenum COLOR_ATTACHMENT6 = 0x8CE6;
    static const GLenum COLOR_ATTACHMENT7 = 0x8CE7;
    static const GLenum COLOR_ATTACHMENT8 = 0x8CE8;
    static const GLenum COLOR_ATTACHMENT9 = 0x8CE9;
    static const GLenum COLOR_ATTACHMENT10 = 0x8CEA;
    static const GLenum COLOR_ATTACHMENT11 = 0x8CEB;
    static const GLenum COLOR_ATTACHMENT12 = 0x8CEC;
    static const GLenum COLOR_ATTACHMENT13 = 0x8CED;
    static const GLenum COLOR_ATTACHMENT14 = 0x8CEE;
    static const GLenum COLOR_ATTACHMENT15 = 0x8CEF;
    static const GLenum FRAMEBUFFER_INCOMPLETE_MULTISAMPLE = 0x8D56;
    static const GLenum MAX_SAMPLES = 0x8D57;
    static const GLenum HALF_FLOAT = 0x140B;
    static const GLenum RG = 0x8227;
    static const GLenum RG_INTEGER = 0x8228;
    static const GLenum R8 = 0x8229;
    static const GLenum RG8 = 0x822B;
    static const GLenum R16F = 0x822D;
    static const GLenum R32F = 0x822E;
    static const GLenum RG16F = 0x822F;
    static const GLenum RG32F = 0x8230;
    static const GLenum R8I = 0x8231;
    static const GLenum R8UI = 0x8232;
    static const GLenum R16I = 0x8233;
    static const GLenum R16UI = 0x8234;
    static const GLenum R32I = 0x8235;
    static const GLenum R32UI = 0x8236;
    static const GLenum RG8I = 0x8237;
    static const GLenum RG8UI = 0x8238;
    static const GLenum RG16I = 0x8239;
    static const GLenum RG16UI = 0x823A;
    static const GLenum RG32I = 0x823B;
    static const GLenum RG32UI = 0x823C;
    static const GLenum VERTEX_ARRAY_BINDING = 0x85B5;
    static const GLenum R8_SNORM = 0x8F94;
    static const GLenum RG8_SNORM = 0x8F95;
    static const GLenum RGB8_SNORM = 0x8F96;
    static const GLenum RGBA8_SNORM = 0x8F97;
    static const GLenum SIGNED_NORMALIZED = 0x8F9C;
    static const GLenum COPY_READ_BUFFER = 0x8F36;
    static const GLenum COPY_WRITE_BUFFER = 0x8F37;
    static const GLenum COPY_READ_BUFFER_BINDING = 0x8F36;
    static const GLenum COPY_WRITE_BUFFER_BINDING = 0x8F37;
    static const GLenum UNIFORM_BUFFER = 0x8A11;
    static const GLenum UNIFORM_BUFFER_BINDING = 0x8A28;
    static const GLenum UNIFORM_BUFFER_START = 0x8A29;
    static const GLenum UNIFORM_BUFFER_SIZE = 0x8A2A;
    static const GLenum MAX_VERTEX_UNIFORM_BLOCKS = 0x8A2B;
    static const GLenum MAX_FRAGMENT_UNIFORM_BLOCKS = 0x8A2D;
    static const GLenum MAX_COMBINED_UNIFORM_BLOCKS = 0x8A2E;
    static const GLenum MAX_UNIFORM_BUFFER_BINDINGS = 0x8A2F;
    static const GLenum MAX_UNIFORM_BLOCK_SIZE = 0x8A30;
    static const GLenum MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS = 0x8A31;
    static const GLenum MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS = 0x8A33;
    static const GLenum UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0x8A34;
    static const GLenum ACTIVE_UNIFORM_BLOCKS = 0x8A36;
    static const GLenum UNIFORM_TYPE = 0x8A37;
    static const GLenum UNIFORM_SIZE = 0x8A38;
    static const GLenum UNIFORM_BLOCK_INDEX = 0x8A3A;
    static const GLenum UNIFORM_OFFSET = 0x8A3B;
    static const GLenum UNIFORM_ARRAY_STRIDE = 0x8A3C;
    static const GLenum UNIFORM_MATRIX_STRIDE = 0x8A3D;
    static const GLenum UNIFORM_IS_ROW_MAJOR = 0x8A3E;
    static const GLenum UNIFORM_BLOCK_BINDING = 0x8A3F;
    static const GLenum UNIFORM_BLOCK_DATA_SIZE = 0x8A40;
    static const GLenum UNIFORM_BLOCK_ACTIVE_UNIFORMS = 0x8A42;
    static const GLenum UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES = 0x8A43;
    static const GLenum UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER = 0x8A44;
    static const GLenum UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER = 0x8A46;
    static const GLenum INVALID_INDEX = 0xFFFFFFFF;
    static const GLenum MAX_VERTEX_OUTPUT_COMPONENTS = 0x9122;
    static const GLenum MAX_FRAGMENT_INPUT_COMPONENTS = 0x9125;
    static const GLenum MAX_SERVER_WAIT_TIMEOUT = 0x9111;
    static const GLenum OBJECT_TYPE = 0x9112;
    static const GLenum SYNC_CONDITION = 0x9113;
    static const GLenum SYNC_STATUS = 0x9114;
    static const GLenum SYNC_FLAGS = 0x9115;
    static const GLenum SYNC_FENCE = 0x9116;
    static const GLenum SYNC_GPU_COMMANDS_COMPLETE = 0x9117;
    static const GLenum UNSIGNALED = 0x9118;
    static const GLenum SIGNALED = 0x9119;
    static const GLenum ALREADY_SIGNALED = 0x911A;
    static const GLenum TIMEOUT_EXPIRED = 0x911B;
    static const GLenum CONDITION_SATISFIED = 0x911C;
    static const GLenum WAIT_FAILED = 0x911D;
    static const GLenum SYNC_FLUSH_COMMANDS_BIT = 0x00000001;
    static const GLenum VERTEX_ATTRIB_ARRAY_DIVISOR = 0x88FE;
    static const GLenum ANY_SAMPLES_PASSED = 0x8C2F;
    static const GLenum ANY_SAMPLES_PASSED_CONSERVATIVE = 0x8D6A;
    static const GLenum SAMPLER_BINDING = 0x8919;
    static const GLenum RGB10_A2UI = 0x906F;
    static const GLenum INT_2_10_10_10_REV = 0x8D9F;
    static const GLenum TRANSFORM_FEEDBACK = 0x8E22;
    static const GLenum TRANSFORM_FEEDBACK_PAUSED = 0x8E23;
    static const GLenum TRANSFORM_FEEDBACK_ACTIVE = 0x8E24;
    static const GLenum TRANSFORM_FEEDBACK_BINDING = 0x8E25;
    static const GLenum TEXTURE_IMMUTABLE_FORMAT = 0x912F;
    static const GLenum MAX_ELEMENT_INDEX = 0x8D6B;
    static const GLenum TEXTURE_IMMUTABLE_LEVELS = 0x82DF;
    static const GLint64 TIMEOUT_IGNORED = -1;
    static const GLenum MAX_CLIENT_WAIT_TIMEOUT_WEBGL = 0x9247;

    static napi_value DrawBuffers(napi_env env, napi_callback_info info);

    static napi_value ClearBufferfv(napi_env env, napi_callback_info info);

    static napi_value ClearBufferiv(napi_env env, napi_callback_info info);

    static napi_value ClearBufferuiv(napi_env env, napi_callback_info info);

    static napi_value ClearBufferfi(napi_env env, napi_callback_info info);

    static napi_value CreateQuery(napi_env env, napi_callback_info info);

    static napi_value DeleteQuery(napi_env env, napi_callback_info info);

    static napi_value IsQuery(napi_env env, napi_callback_info info);

    static napi_value BeginQuery(napi_env env, napi_callback_info info);

    static napi_value EndQuery(napi_env env, napi_callback_info info);

    static napi_value GetQuery(napi_env env, napi_callback_info info);

    static napi_value GetQueryParameter(napi_env env, napi_callback_info info);

    static napi_value CreateSampler(napi_env env, napi_callback_info info);

    static napi_value DeleteSampler(napi_env env, napi_callback_info info);

    static napi_value IsSampler(napi_env env, napi_callback_info info);

    static napi_value BindSampler(napi_env env, napi_callback_info info);

    static napi_value SamplerParameteri(napi_env env, napi_callback_info info);

    static napi_value SamplerParameterf(napi_env env, napi_callback_info info);

    static napi_value GetSamplerParameter(napi_env env, napi_callback_info info);

    static napi_value FenceSync(napi_env env, napi_callback_info info);

    static napi_value IsSync(napi_env env, napi_callback_info info);

    static napi_value DeleteSync(napi_env env, napi_callback_info info);

    static napi_value ClientWaitSync(napi_env env, napi_callback_info info);

    static napi_value WaitSync(napi_env env, napi_callback_info info);

    static napi_value GetSyncParameter(napi_env env, napi_callback_info info);

    static napi_value CreateTransformFeedback(napi_env env, napi_callback_info info);

    static napi_value DeleteTransformFeedback(napi_env env, napi_callback_info info);

    static napi_value IsTransformFeedback(napi_env env, napi_callback_info info);

    static napi_value BindTransformFeedback(napi_env env, napi_callback_info info);

    static napi_value BeginTransformFeedback(napi_env env, napi_callback_info info);

    static napi_value EndTransformFeedback(napi_env env, napi_callback_info info);

    static napi_value TexStorage3D(napi_env env, napi_callback_info info);

    static napi_value TexImage3D(napi_env env, napi_callback_info info);

    static napi_value TexSubImage3D(napi_env env, napi_callback_info info);

    static napi_value CopyTexSubImage3D(napi_env env, napi_callback_info info);

    static napi_value CompressedTexImage3D(napi_env env, napi_callback_info info);

    static napi_value CompressedTexSubImage3D(napi_env env, napi_callback_info info);

    static napi_value GetFragDataLocation(napi_env env, napi_callback_info info);

    static napi_value Uniform1ui(napi_env env, napi_callback_info info);

    static napi_value Uniform2ui(napi_env env, napi_callback_info info);

    static napi_value Uniform3ui(napi_env env, napi_callback_info info);

    static napi_value Uniform4ui(napi_env env, napi_callback_info info);

    static napi_value VertexAttribI4i(napi_env env, napi_callback_info info);

    static napi_value VertexAttribI4ui(napi_env env, napi_callback_info info);

    static napi_value VertexAttribIPointer(napi_env env, napi_callback_info info);

    static napi_value VertexAttribDivisor(napi_env env, napi_callback_info info);

    static napi_value DrawArraysInstanced(napi_env env, napi_callback_info info);

    static napi_value DrawElementsInstanced(napi_env env, napi_callback_info info);

    static napi_value DrawRangeElements(napi_env env, napi_callback_info info);

    static napi_value CopyBufferSubData(napi_env env, napi_callback_info info);

    static napi_value GetBufferSubData(napi_env env, napi_callback_info info);

    static napi_value BlitFramebuffer(napi_env env, napi_callback_info info);

    static napi_value FramebufferTextureLayer(napi_env env, napi_callback_info info);

    static napi_value ReadBuffer(napi_env env, napi_callback_info info);

    static napi_value RenderbufferStorageMultisample(napi_env env, napi_callback_info info);

    static napi_value TexStorage2D(napi_env env, napi_callback_info info);

    static napi_value GetTransformFeedbackVarying(napi_env env, napi_callback_info info);

    static napi_value PauseTransformFeedback(napi_env env, napi_callback_info info);

    static napi_value ResumeTransformFeedback(napi_env env, napi_callback_info info);

    static napi_value BindBufferBase(napi_env env, napi_callback_info info);

    static napi_value BindBufferRange(napi_env env, napi_callback_info info);

    static napi_value GetIndexedParameter(napi_env env, napi_callback_info info);

    static napi_value GetUniformBlockIndex(napi_env env, napi_callback_info info);

    static napi_value GetActiveUniformBlockParameter(napi_env env, napi_callback_info info);

    static napi_value GetActiveUniformBlockName(napi_env env, napi_callback_info info);

    static napi_value UniformBlockBinding(napi_env env, napi_callback_info info);

    static napi_value CreateVertexArray(napi_env env, napi_callback_info info);

    static napi_value DeleteVertexArray(napi_env env, napi_callback_info info);

    static napi_value IsVertexArray(napi_env env, napi_callback_info info);

    static napi_value BindVertexArray(napi_env env, napi_callback_info info);

    static napi_value Uniform1uiv(napi_env env, napi_callback_info info);

    static napi_value Uniform2uiv(napi_env env, napi_callback_info info);

    static napi_value Uniform3uiv(napi_env env, napi_callback_info info);

    static napi_value Uniform4uiv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix3x2fv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix4x2fv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix2x3fv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix4x3fv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix2x4fv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix3x4fv(napi_env env, napi_callback_info info);

    static napi_value VertexAttribI4iv(napi_env env, napi_callback_info info);

    static napi_value VertexAttribI4uiv(napi_env env, napi_callback_info info);

    static napi_value InvalidateFramebuffer(napi_env env, napi_callback_info info);

    static napi_value InvalidateSubFramebuffer(napi_env env, napi_callback_info info);

    static napi_value GetInternalformatParameter(napi_env env, napi_callback_info info);

    static napi_value TransformFeedbackVaryings(napi_env env, napi_callback_info info);

    static napi_value GetUniformIndices(napi_env env, napi_callback_info info);

    static napi_value GetActiveUniforms(napi_env env, napi_callback_info info);

public:
    std::map<GLenum, GLuint> queryMaps;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT_BASE

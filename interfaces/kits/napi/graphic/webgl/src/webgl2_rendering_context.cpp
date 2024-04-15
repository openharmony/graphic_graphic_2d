/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "context/webgl2_rendering_context.h"

#include "context/webgl_rendering_context_overloads.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/egl_manager.h"
#include "util/log.h"
#include "util/object_manager.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace OHOS {
namespace Rosen {
using namespace std;
static napi_value GetReadBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::READ_BUFFER).val_;
}

static napi_value GetUnpackRowLength(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNPACK_ROW_LENGTH).val_;
}

static napi_value GetUnpackSkipRows(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNPACK_SKIP_ROWS).val_;
}

static napi_value GetUnpackSkipPixels(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNPACK_SKIP_PIXELS).val_;
}

static napi_value GetPackRowLength(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::PACK_ROW_LENGTH).val_;
}

static napi_value GetPackSkipRows(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::PACK_SKIP_ROWS).val_;
}

static napi_value GetPackSkipPixels(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::PACK_SKIP_PIXELS).val_;
}

static napi_value GetColor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR).val_;
}

static napi_value GetDepth(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DEPTH).val_;
}

static napi_value GetStencil(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::STENCIL).val_;
}

static napi_value GetRed(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RED).val_;
}

static napi_value GetRgb8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB8).val_;
}

static napi_value GetRgba8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA8).val_;
}

static napi_value GetRgb10A2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB10_A2).val_;
}

static napi_value GetTextureBinding3D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_BINDING_3D).val_;
}

static napi_value GetUnpackSkipImages(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNPACK_SKIP_IMAGES).val_;
}

static napi_value GetUnpackIamgeHeight(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNPACK_IMAGE_HEIGHT).val_;
}

static napi_value GetTexture3D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_3D).val_;
}

static napi_value GetTextureWrapR(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_WRAP_R).val_;
}

static napi_value GetMax3DTextueSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_3D_TEXTURE_SIZE).val_;
}

static napi_value GetUnsgnedInt2101010Rev(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_2_10_10_10_REV).val_;
}

static napi_value GeMaxElementVertices(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_ELEMENTS_VERTICES).val_;
}

static napi_value GetElementIndices(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_ELEMENTS_INDICES).val_;
}

static napi_value GetTextureMinLod(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_MIN_LOD).val_;
}

static napi_value GetTextureMaxLod(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_MAX_LOD).val_;
}

static napi_value GetTextureBaseLevel(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_BASE_LEVEL).val_;
}

static napi_value GetTextureMaxLevel(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_MAX_LEVEL).val_;
}

static napi_value GetMin(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MIN).val_;
}

static napi_value GetMax(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX).val_;
}

static napi_value GetDepthComponent24(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DEPTH_COMPONENT24).val_;
}

static napi_value GetMaxTextureLodBias(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_TEXTURE_LOD_BIAS).val_;
}

static napi_value GetTextureCompareMode(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_COMPARE_MODE).val_;
}

static napi_value GetTextureCompareFunc(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_COMPARE_FUNC).val_;
}

static napi_value GetCurrentQuery(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::CURRENT_QUERY).val_;
}

static napi_value GetQueryResult(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::QUERY_RESULT).val_;
}

static napi_value GetQueryResultAvailable(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::QUERY_RESULT_AVAILABLE).val_;
}

static napi_value GetStreamRead(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::STREAM_READ).val_;
}

static napi_value GetStreamCopy(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::STREAM_COPY).val_;
}

static napi_value GetStaticRead(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::STATIC_READ).val_;
}

static napi_value GetStaticCopy(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::STATIC_COPY).val_;
}

static napi_value GetDynamicRead(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DYNAMIC_READ).val_;
}

static napi_value GetDynamicCopy(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DYNAMIC_COPY).val_;
}

static napi_value GetMaxDrawBuffers(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_DRAW_BUFFERS).val_;
}

static napi_value GetDrawBuffer0(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER0).val_;
}

static napi_value GetDrawBuffer1(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER1).val_;
}

static napi_value GetDrawBuffer2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER2).val_;
}

static napi_value GetDrawBuffer3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER3).val_;
}

static napi_value GetDrawBuffer4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER4).val_;
}

static napi_value GetDrawBuffer5(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER5).val_;
}

static napi_value GetDrawBuffer6(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER6).val_;
}

static napi_value GetDrawBuffer7(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER7).val_;
}

static napi_value GetDrawBuffer8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER8).val_;
}

static napi_value GetDrawBuffer9(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER9).val_;
}

static napi_value GetDrawBuffer10(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER10).val_;
}

static napi_value GetDrawBuffer11(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER11).val_;
}

static napi_value GetDrawBuffer12(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER12).val_;
}

static napi_value GetDrawBuffer13(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER13).val_;
}

static napi_value GetDrawBuffer14(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER14).val_;
}

static napi_value GetDrawBuffer15(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_BUFFER15).val_;
}

static napi_value GetMaxFragmentUniformComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_FRAGMENT_UNIFORM_COMPONENTS).val_;
}

static napi_value GetMaxVertexUniformComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_VERTEX_UNIFORM_COMPONENTS).val_;
}

static napi_value GetSampler3D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SAMPLER_3D).val_;
}

static napi_value GetSample2DShadow(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SAMPLER_2D_SHADOW).val_;
}

static napi_value GetFragmentShaderDerivativeHint(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAGMENT_SHADER_DERIVATIVE_HINT).val_;
}

static napi_value GetPixelPackBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::PIXEL_PACK_BUFFER).val_;
}

static napi_value GetPixelUnpackBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::PIXEL_UNPACK_BUFFER).val_;
}

static napi_value GetPixelPackBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::PIXEL_PACK_BUFFER_BINDING).val_;
}

static napi_value GetPixelUnpackBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::PIXEL_UNPACK_BUFFER_BINDING).val_;
}

static napi_value GetFloatMat2x3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FLOAT_MAT2x3).val_;
}

static napi_value GetFloatMat2x4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FLOAT_MAT2x4).val_;
}

static napi_value GetFloatMat3x2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FLOAT_MAT3x2).val_;
}

static napi_value GetFloatMat3x4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FLOAT_MAT3x4).val_;
}

static napi_value GetFloatMat4x2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FLOAT_MAT4x2).val_;
}

static napi_value GetFloatMat4x3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FLOAT_MAT4x3).val_;
}

static napi_value GetSrgb(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SRGB).val_;
}

static napi_value GetSrgb8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SRGB8).val_;
}

static napi_value GetSrgb8Alpha8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SRGB8_ALPHA8).val_;
}

static napi_value GetCompareRefToTexture(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COMPARE_REF_TO_TEXTURE).val_;
}

static napi_value GetRgba32f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA32F).val_;
}

static napi_value GetRgb32f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB32F).val_;
}

static napi_value GetRgba16f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA16F).val_;
}

static napi_value GetRgb16f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB16F).val_;
}

static napi_value GetVertexAttribArrayInteger(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::VERTEX_ATTRIB_ARRAY_INTEGER).val_;
}

static napi_value GetMaxArrayTextureLayers(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_ARRAY_TEXTURE_LAYERS).val_;
}

static napi_value GetMinProgramTexelOffset(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MIN_PROGRAM_TEXEL_OFFSET).val_;
}

static napi_value GetMaxProgramTexelOffset(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_PROGRAM_TEXEL_OFFSET).val_;
}

static napi_value GetMaxVaryingComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_VARYING_COMPONENTS).val_;
}

static napi_value GetTexture2DArray(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_2D_ARRAY).val_;
}

static napi_value GetTextureBinding2DArray(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_BINDING_2D_ARRAY).val_;
}

static napi_value GetR11fG11fB10f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R11F_G11F_B10F).val_;
}

static napi_value GetUnsignedInt10f11f11fRev(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_10F_11F_11F_REV).val_;
}

static napi_value GetRgb9E5(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB9_E5).val_;
}

static napi_value GetUnsignedInt5999Rev(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_5_9_9_9_REV).val_;
}

static napi_value GetTransformFeedbackBufferMode(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER_MODE).val_;
}

static napi_value GeMaxTransformFeedbackSeparateComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS).val_;
}

static napi_value GetTransformFeedbackVaryings(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_VARYINGS).val_;
}

static napi_value GetTransformFeedbackBufferStart(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER_START).val_;
}

static napi_value GetTransformFeedbackBufferSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER_SIZE).val_;
}

static napi_value GetTransformFeedbackPrimitivesWritten(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN).val_;
}

static napi_value GetRasterizerDiscard(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RASTERIZER_DISCARD).val_;
}

static napi_value GetMaxTransformFeedbackInterLeavedComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS).val_;
}

static napi_value GetMaxTransformFeedbackSeparateAttribs(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS).val_;
}

static napi_value GetInterleavedAttribs(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::INTERLEAVED_ATTRIBS).val_;
}

static napi_value GetSeparateAttribs(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SEPARATE_ATTRIBS).val_;
}

static napi_value GetTransformFeedbackBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER).val_;
}

static napi_value GetTransformFeedbackBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER_BINDING).val_;
}

static napi_value GetRgba32Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA32UI).val_;
}

static napi_value GetRgb32Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB32UI).val_;
}

static napi_value GetRgba16Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA16UI).val_;
}

static napi_value GetRgb16Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB16UI).val_;
}

static napi_value GetRgba8Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA8UI).val_;
}

static napi_value GetRgb8Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB8UI).val_;
}

static napi_value GetRgba32i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA32I).val_;
}

static napi_value GetRgb32i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB32I).val_;
}

static napi_value GetRgba16i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA16I).val_;
}

static napi_value GetRgb16i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB16I).val_;
}

static napi_value GetRgba8i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA8I).val_;
}

static napi_value GetRgb8i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB8I).val_;
}

static napi_value GetReadInteger(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RED_INTEGER).val_;
}

static napi_value GetRgbInteger(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB_INTEGER).val_;
}

static napi_value GetRgbaInteger(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA_INTEGER).val_;
}

static napi_value GetSample2DArray(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SAMPLER_2D_ARRAY).val_;
}

static napi_value GetSample2DArrayShadow(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SAMPLER_2D_ARRAY_SHADOW).val_;
}

static napi_value GetSampleCubeShadow(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SAMPLER_CUBE_SHADOW).val_;
}

static napi_value GetUnsignedIntVec2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_VEC2).val_;
}

static napi_value GetUnsignedIntVec3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_VEC3).val_;
}

static napi_value GetUnsignedIntVec4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_VEC4).val_;
}

static napi_value GetIntSample2D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::INT_SAMPLER_2D).val_;
}

static napi_value GetIntSample3D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::INT_SAMPLER_3D).val_;
}

static napi_value GetIntSampleCube(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::INT_SAMPLER_CUBE).val_;
}

static napi_value GetIntSample2DArray(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::INT_SAMPLER_2D_ARRAY).val_;
}

static napi_value GetUnsignedIntSample2D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_SAMPLER_2D).val_;
}

static napi_value GetUnsignedIntSample3D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_SAMPLER_3D).val_;
}

static napi_value GetUnsignedIntSampleCube(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_SAMPLER_CUBE).val_;
}

static napi_value GetUnsignedIntSample2DArray(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_SAMPLER_2D_ARRAY).val_;
}

static napi_value GetDepthComponent32f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DEPTH_COMPONENT32F).val_;
}

static napi_value GetDepth32FStencil8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DEPTH32F_STENCIL8).val_;
}

static napi_value GeFloat32UnsignedInt248Rev(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FLOAT_32_UNSIGNED_INT_24_8_REV).val_;
}

static napi_value GtFramebufferAttachmentColorEncoding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING).val_;
}

static napi_value GtFramebufferAttachmentComponentType(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE).val_;
}

static napi_value GtFramebufferAttachmentRedSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_RED_SIZE).val_;
}

static napi_value GtFramebufferAttachmentGreenSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_GREEN_SIZE).val_;
}

static napi_value GtFramebufferAttachmentBlueSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_BLUE_SIZE).val_;
}

static napi_value GtFramebufferAttachmentAlphaSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE).val_;
}

static napi_value GtFramebufferAttachmentDepthSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE).val_;
}

static napi_value GtFramebufferAttachmentStencilSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE).val_;
}

static napi_value GetFramebufferDefault(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_DEFAULT).val_;
}

static napi_value GetUnsignedInt248(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_INT_24_8).val_;
}

static napi_value GetDepth24Stencil8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DEPTH24_STENCIL8).val_;
}

static napi_value GetUnsignedNormalized(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNED_NORMALIZED).val_;
}

static napi_value GetDrawFramebufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_FRAMEBUFFER_BINDING).val_;
}

static napi_value GetReadFramebuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER).val_;
}

static napi_value GetDrawFramebuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::DRAW_FRAMEBUFFER).val_;
}

static napi_value GetFrameBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER_BINDING).val_;
}

static napi_value GetRenderbufferSamples(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RENDERBUFFER_SAMPLES).val_;
}

static napi_value GetFramebufferAttachmentTexturelLayer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER).val_;
}

static napi_value GetMaxColorAttachments(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_COLOR_ATTACHMENTS).val_;
}

static napi_value GetColorAttachment1(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT1).val_;
}

static napi_value GetColorAttachment2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT2).val_;
}

static napi_value GetColorAttachment3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT3).val_;
}

static napi_value GetColorAttachment4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT4).val_;
}

static napi_value GetColorAttachment5(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT5).val_;
}

static napi_value GetColorAttachment6(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT6).val_;
}

static napi_value GetColorAttachment7(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT7).val_;
}

static napi_value GetColorAttachment8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT8).val_;
}

static napi_value GetColorAttachment9(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT9).val_;
}

static napi_value GetColorAttachment10(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT10).val_;
}

static napi_value GetColorAttachment11(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT11).val_;
}

static napi_value GetColorAttachment12(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT12).val_;
}

static napi_value GetColorAttachment13(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT13).val_;
}

static napi_value GetColorAttachment14(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT14).val_;
}

static napi_value GetColorAttachment15(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COLOR_ATTACHMENT15).val_;
}

static napi_value GetFramebufferIncompleteMultiSample(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::FRAMEBUFFER_INCOMPLETE_MULTISAMPLE).val_;
}

static napi_value GetMaxSamples(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_SAMPLES).val_;
}

static napi_value GetHalfFloat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::HALF_FLOAT).val_;
}

static napi_value GetRg(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG).val_;
}

static napi_value GetRgInteger(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG_INTEGER).val_;
}

static napi_value GetR8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R8).val_;
}

static napi_value GetRg8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG8).val_;
}

static napi_value GetR16f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R16F).val_;
}

static napi_value GetR32f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R32F).val_;
}

static napi_value GetRg16f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG16F).val_;
}

static napi_value GetRg32f(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG32F).val_;
}

static napi_value GetR8i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R8I).val_;
}

static napi_value GetR8Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R8UI).val_;
}

static napi_value GetR16i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R16I).val_;
}

static napi_value GetR16Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R16UI).val_;
}

static napi_value GetR32i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R32I).val_;
}

static napi_value GetR32Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R32UI).val_;
}

static napi_value GetRg8i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG8I).val_;
}

static napi_value GetRg8ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG8UI).val_;
}

static napi_value GetRg16i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG16I).val_;
}

static napi_value GetRg16Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG16UI).val_;
}

static napi_value GetRg32i(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG32I).val_;
}

static napi_value GetRg32Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG32UI).val_;
}

static napi_value GetVertexArrayBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::VERTEX_ARRAY_BINDING).val_;
}

static napi_value GetR8Snorm(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::R8_SNORM).val_;
}

static napi_value GetRg8Snorm(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RG8_SNORM).val_;
}

static napi_value GetRgb8Snorm(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB8_SNORM).val_;
}

static napi_value GetRgba8Snorm(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGBA8_SNORM).val_;
}

static napi_value GetSignedNormalized(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SIGNED_NORMALIZED).val_;
}

static napi_value GetCopyReadBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COPY_READ_BUFFER).val_;
}

static napi_value GetCopyWriteBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COPY_WRITE_BUFFER).val_;
}

static napi_value GetCopyWriteReadBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COPY_READ_BUFFER_BINDING).val_;
}

static napi_value GetCopyWriteBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::COPY_WRITE_BUFFER_BINDING).val_;
}

static napi_value GetUniformBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BUFFER).val_;
}

static napi_value GetUniformBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BUFFER_BINDING).val_;
}

static napi_value GtUniformBufferStart(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BUFFER_START).val_;
}

static napi_value GetUniformBufferSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BUFFER_SIZE).val_;
}

static napi_value GetMaxVertexUniformBlocks(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_VERTEX_UNIFORM_BLOCKS).val_;
}

static napi_value GetMaxFragmentUniformBlocks(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_FRAGMENT_UNIFORM_BLOCKS).val_;
}

static napi_value GetMaxCombinedUniformBlocks(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_COMBINED_UNIFORM_BLOCKS).val_;
}

static napi_value GetUniformBufferBinfings(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_UNIFORM_BUFFER_BINDINGS).val_;
}

static napi_value GetUniformBlockSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_UNIFORM_BLOCK_SIZE).val_;
}

static napi_value GetMaxCombinedVertexUniformComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS).val_;
}

static napi_value GetMaxCombinedFragmentUniformComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS).val_;
}

static napi_value GetUniformBufferOffsetAlignment(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BUFFER_OFFSET_ALIGNMENT).val_;
}

static napi_value GetActiveUniformBlocks(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::ACTIVE_UNIFORM_BLOCKS).val_;
}

static napi_value GetUniformType(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_TYPE).val_;
}

static napi_value GetUniformSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_SIZE).val_;
}

static napi_value GetUniformIndex(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BLOCK_INDEX).val_;
}

static napi_value GetUniformOffset(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_OFFSET).val_;
}

static napi_value GetUniformArrayStride(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_ARRAY_STRIDE).val_;
}

static napi_value GetUniformMatrixStride(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_MATRIX_STRIDE).val_;
}

static napi_value GetUniformIsRowMajor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_IS_ROW_MAJOR).val_;
}

static napi_value GetUniformBlockBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BLOCK_BINDING).val_;
}

static napi_value GetUniformBlockDataSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BLOCK_DATA_SIZE).val_;
}

static napi_value GetUniformBlockActiveUniforms(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BLOCK_ACTIVE_UNIFORMS).val_;
}

static napi_value GetUniformBlockActiveUniformIndices(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES).val_;
}

static napi_value GetUniformBlockReferencedByVertexShader(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER).val_;
}

static napi_value GetUniformBlockReferencedByFragmentShader(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER).val_;
}

static napi_value GetInvalidIndex(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::INVALID_INDEX).val_;
}

static napi_value GetMaxVertexOutputComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_VERTEX_OUTPUT_COMPONENTS).val_;
}

static napi_value GetMaxFragmentInputComponents(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_FRAGMENT_INPUT_COMPONENTS).val_;
}

static napi_value GetMaxServerWaitTimeout(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_SERVER_WAIT_TIMEOUT).val_;
}

static napi_value GetObjectType(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::OBJECT_TYPE).val_;
}

static napi_value GetSyncCondition(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SYNC_CONDITION).val_;
}

static napi_value GetSyncStatus(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SYNC_STATUS).val_;
}

static napi_value GetSyncFlags(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SYNC_FLAGS).val_;
}

static napi_value GetSyncFence(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SYNC_FENCE).val_;
}

static napi_value GetSyncGpuCommandsComplete(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SYNC_GPU_COMMANDS_COMPLETE).val_;
}

static napi_value GetUnsignaled(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::UNSIGNALED).val_;
}

static napi_value GetSignaled(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SIGNALED).val_;
}

static napi_value GetAlreadySignaled(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::ALREADY_SIGNALED).val_;
}

static napi_value GetTimeOutExpired(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TIMEOUT_EXPIRED).val_;
}

static napi_value GetConditionSatisfied(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::CONDITION_SATISFIED).val_;
}

static napi_value GetWaitFailed(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
}

static napi_value GetSyncFlushCommandsBit(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SYNC_FLUSH_COMMANDS_BIT).val_;
}

static napi_value GetVertexAttribArrayDivisor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::VERTEX_ATTRIB_ARRAY_DIVISOR).val_;
}

static napi_value GetAnySamplePassed(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::ANY_SAMPLES_PASSED).val_;
}

static napi_value GetAnySamplePassedConservative(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::ANY_SAMPLES_PASSED_CONSERVATIVE).val_;
}

static napi_value GetSampleBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::SAMPLER_BINDING).val_;
}

static napi_value GetRgb10A2Ui(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::RGB10_A2UI).val_;
}

static napi_value GetInt2101010Rev(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::INT_2_10_10_10_REV).val_;
}

static napi_value GetTransformFeedback(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK).val_;
}

static napi_value GetTransformFeedbackPaused(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_PAUSED).val_;
}

static napi_value GetTransformFeedbackActive(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_ACTIVE).val_;
}

static napi_value GetTransformFeedbackBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BINDING).val_;
}

static napi_value GetTextureImmutableFormat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_IMMUTABLE_FORMAT).val_;
}

static napi_value GetMaxElementIndex(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_ELEMENT_INDEX).val_;
}

static napi_value GetTextureImmutableLevels(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TEXTURE_IMMUTABLE_LEVELS).val_;
}

static napi_value GetTimeoutIgnored(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::TIMEOUT_IGNORED).val_;
}

static napi_value GetMaxClientWaitTimeoutWebgl(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGL2RenderingContextBase::MAX_CLIENT_WAIT_TIMEOUT_WEBGL).val_;
}

bool WebGL2RenderingContext::Export(napi_env env, napi_value exports)
{
    LOGD("WebGL WebGL2RenderingContext::Export env %{public}p mContextRef %{public}p", env, contextRef_);
    napi_value instanceValue = GetContextInstance(env, GetClassName(),
        [](napi_env env, napi_callback_info info) -> napi_value {
            napi_value thisVar = nullptr;
            napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
            LOGD("WebGL WebGL2RenderingContext::create");
            return thisVar;
        },
        [](napi_env env, void* data, void* hint) {
            auto entity = static_cast<WebGL2RenderingContext*>(data);
            LOGD("WebGL WebGL2RenderingContext::delete");
            if (entity->contextRef_) {
                napi_delete_reference(env, entity->contextRef_);
            }
            delete entity;
        }
    );
    if (instanceValue == nullptr) {
        LOGE("Failed to create instance");
        return false;
    }
    LOGD("WebGL WebGL2RenderingContext::Export instanceValue %{public}p", instanceValue);

    std::vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("drawBuffers", WebGL2RenderingContextBase::DrawBuffers),
        NVal::DeclareNapiFunction("clearBufferfv", WebGL2RenderingContextBase::ClearBufferfv),
        NVal::DeclareNapiFunction("clearBufferiv", WebGL2RenderingContextBase::ClearBufferiv),
        NVal::DeclareNapiFunction("clearBufferuiv", WebGL2RenderingContextBase::ClearBufferuiv),
        NVal::DeclareNapiFunction("clearBufferfi", WebGL2RenderingContextBase::ClearBufferfi),
        NVal::DeclareNapiFunction("createQuery", WebGL2RenderingContextBase::CreateQuery),
        NVal::DeclareNapiFunction("deleteQuery", WebGL2RenderingContextBase::DeleteQuery),
        NVal::DeclareNapiFunction("isQuery", WebGL2RenderingContextBase::IsQuery),
        NVal::DeclareNapiFunction("beginQuery", WebGL2RenderingContextBase::BeginQuery),
        NVal::DeclareNapiFunction("endQuery", WebGL2RenderingContextBase::EndQuery),
        NVal::DeclareNapiFunction("getQuery", WebGL2RenderingContextBase::GetQuery),
        NVal::DeclareNapiFunction("getQueryParameter", WebGL2RenderingContextBase::GetQueryParameter),
        NVal::DeclareNapiFunction("createSampler", WebGL2RenderingContextBase::CreateSampler),
        NVal::DeclareNapiFunction("deleteSampler", WebGL2RenderingContextBase::DeleteSampler),
        NVal::DeclareNapiFunction("isSampler", WebGL2RenderingContextBase::IsSampler),
        NVal::DeclareNapiFunction("bindSampler", WebGL2RenderingContextBase::BindSampler),
        NVal::DeclareNapiFunction("samplerParameteri", WebGL2RenderingContextBase::SamplerParameteri),
        NVal::DeclareNapiFunction("samplerParameterf", WebGL2RenderingContextBase::SamplerParameterf),
        NVal::DeclareNapiFunction("getSamplerParameter", WebGL2RenderingContextBase::GetSamplerParameter),
        NVal::DeclareNapiFunction("fenceSync", WebGL2RenderingContextBase::FenceSync),
        NVal::DeclareNapiFunction("isSync", WebGL2RenderingContextBase::IsSync),
        NVal::DeclareNapiFunction("deleteSync", WebGL2RenderingContextBase::DeleteSync),
        NVal::DeclareNapiFunction("clientWaitSync", WebGL2RenderingContextBase::ClientWaitSync),
        NVal::DeclareNapiFunction("waitSync", WebGL2RenderingContextBase::WaitSync),
        NVal::DeclareNapiFunction("getSyncParameter", WebGL2RenderingContextBase::GetSyncParameter),
        NVal::DeclareNapiFunction("createTransformFeedback", WebGL2RenderingContextBase::CreateTransformFeedback),
        NVal::DeclareNapiFunction("deleteTransformFeedback", WebGL2RenderingContextBase::DeleteTransformFeedback),
        NVal::DeclareNapiFunction("isTransformFeedback", WebGL2RenderingContextBase::IsTransformFeedback),
        NVal::DeclareNapiFunction("bindTransformFeedback", WebGL2RenderingContextBase::BindTransformFeedback),
        NVal::DeclareNapiFunction("beginTransformFeedback", WebGL2RenderingContextBase::BeginTransformFeedback),
        NVal::DeclareNapiFunction("endTransformFeedback", WebGL2RenderingContextBase::EndTransformFeedback),
        NVal::DeclareNapiFunction("texStorage3D", WebGL2RenderingContextBase::TexStorage3D),
        NVal::DeclareNapiFunction("texImage3D", WebGL2RenderingContextBase::TexImage3D),
        NVal::DeclareNapiFunction("texSubImage3D", WebGL2RenderingContextBase::TexSubImage3D),
        NVal::DeclareNapiFunction("copyTexSubImage3D", WebGL2RenderingContextBase::CopyTexSubImage3D),
        NVal::DeclareNapiFunction("compressedTexImage3D", WebGL2RenderingContextBase::CompressedTexImage3D),
        NVal::DeclareNapiFunction("compressedTexSubImage3D", WebGL2RenderingContextBase::CompressedTexSubImage3D),
        NVal::DeclareNapiFunction("getFragDataLocation", WebGL2RenderingContextBase::GetFragDataLocation),
        NVal::DeclareNapiFunction("uniform1ui", WebGL2RenderingContextBase::Uniform1ui),
        NVal::DeclareNapiFunction("uniform2ui", WebGL2RenderingContextBase::Uniform2ui),
        NVal::DeclareNapiFunction("uniform3ui", WebGL2RenderingContextBase::Uniform3ui),
        NVal::DeclareNapiFunction("uniform4ui", WebGL2RenderingContextBase::Uniform4ui),
        NVal::DeclareNapiFunction("vertexAttribI4i", WebGL2RenderingContextBase::VertexAttribI4i),
        NVal::DeclareNapiFunction("vertexAttribI4ui", WebGL2RenderingContextBase::VertexAttribI4ui),
        NVal::DeclareNapiFunction("vertexAttribIPointer", WebGL2RenderingContextBase::VertexAttribIPointer),
        NVal::DeclareNapiFunction("vertexAttribDivisor", WebGL2RenderingContextBase::VertexAttribDivisor),
        NVal::DeclareNapiFunction("drawArraysInstanced", WebGL2RenderingContextBase::DrawArraysInstanced),
        NVal::DeclareNapiFunction("drawElementsInstanced", WebGL2RenderingContextBase::DrawElementsInstanced),
        NVal::DeclareNapiFunction("drawRangeElements", WebGL2RenderingContextBase::DrawRangeElements),
        NVal::DeclareNapiFunction("copyBufferSubData", WebGL2RenderingContextBase::CopyBufferSubData),
        NVal::DeclareNapiFunction("getBufferSubData", WebGL2RenderingContextBase::GetBufferSubData),
        NVal::DeclareNapiFunction("blitFramebuffer", WebGL2RenderingContextBase::BlitFramebuffer),
        NVal::DeclareNapiFunction("framebufferTextureLayer", WebGL2RenderingContextBase::FramebufferTextureLayer),
        NVal::DeclareNapiFunction("readBuffer", WebGL2RenderingContextBase::ReadBuffer),
        NVal::DeclareNapiFunction("renderbufferStorageMultisample",
            WebGL2RenderingContextBase::RenderbufferStorageMultisample),
        NVal::DeclareNapiFunction("texStorage2D", WebGL2RenderingContextBase::TexStorage2D),
        NVal::DeclareNapiFunction("getTransformFeedbackVarying",
            WebGL2RenderingContextBase::GetTransformFeedbackVarying),
        NVal::DeclareNapiFunction("pauseTransformFeedback", WebGL2RenderingContextBase::PauseTransformFeedback),
        NVal::DeclareNapiFunction("resumeTransformFeedback", ResumeTransformFeedback),
        NVal::DeclareNapiFunction("bindBufferBase", WebGL2RenderingContextBase::BindBufferBase),
        NVal::DeclareNapiFunction("bindBufferRange", WebGL2RenderingContextBase::BindBufferRange),
        NVal::DeclareNapiFunction("getIndexedParameter", WebGL2RenderingContextBase::GetIndexedParameter),
        NVal::DeclareNapiFunction("uniform1uiv", WebGL2RenderingContextBase::Uniform1uiv),
        NVal::DeclareNapiFunction("uniform2uiv", WebGL2RenderingContextBase::Uniform2uiv),
        NVal::DeclareNapiFunction("uniform3uiv", WebGL2RenderingContextBase::Uniform3uiv),
        NVal::DeclareNapiFunction("uniform4uiv", WebGL2RenderingContextBase::Uniform4uiv),
        NVal::DeclareNapiFunction("uniformMatrix3x2fv", WebGL2RenderingContextBase::UniformMatrix3x2fv),
        NVal::DeclareNapiFunction("uniformMatrix4x2fv", WebGL2RenderingContextBase::UniformMatrix4x2fv),
        NVal::DeclareNapiFunction("uniformMatrix2x3fv", WebGL2RenderingContextBase::UniformMatrix2x3fv),
        NVal::DeclareNapiFunction("uniformMatrix4x3fv", WebGL2RenderingContextBase::UniformMatrix4x3fv),
        NVal::DeclareNapiFunction("uniformMatrix2x4fv", WebGL2RenderingContextBase::UniformMatrix2x4fv),
        NVal::DeclareNapiFunction("uniformMatrix3x4fv", WebGL2RenderingContextBase::UniformMatrix3x4fv),
        NVal::DeclareNapiFunction("vertexAttribI4iv", WebGL2RenderingContextBase::VertexAttribI4iv),
        NVal::DeclareNapiFunction("vertexAttribI4uiv", WebGL2RenderingContextBase::VertexAttribI4uiv),
        NVal::DeclareNapiFunction("getUniformBlockIndex", WebGL2RenderingContextBase::GetUniformBlockIndex),
        NVal::DeclareNapiFunction("uniformBlockBinding", WebGL2RenderingContextBase::UniformBlockBinding),
        NVal::DeclareNapiFunction("createVertexArray", WebGL2RenderingContextBase::CreateVertexArray),
        NVal::DeclareNapiFunction("deleteVertexArray", WebGL2RenderingContextBase::DeleteVertexArray),
        NVal::DeclareNapiFunction("isVertexArray", WebGL2RenderingContextBase::IsVertexArray),
        NVal::DeclareNapiFunction("bindVertexArray", WebGL2RenderingContextBase::BindVertexArray),
        NVal::DeclareNapiFunction("getActiveUniformBlockParameter",
            WebGL2RenderingContextBase::GetActiveUniformBlockParameter),
        NVal::DeclareNapiFunction("bufferData", WebGLRenderingContextOverloads::BufferData),
        NVal::DeclareNapiFunction("bufferSubData", WebGLRenderingContextOverloads::BufferSubData),
        NVal::DeclareNapiFunction("texImage2D", WebGLRenderingContextOverloads::TexImage2D),
        NVal::DeclareNapiFunction("texSubImage2D", WebGLRenderingContextOverloads::TexSubImage2D),
        NVal::DeclareNapiFunction("compressedTexImage2D", WebGLRenderingContextOverloads::CompressedTexImage2D),
        NVal::DeclareNapiFunction("compressedTexSubImage2D", WebGLRenderingContextOverloads::CompressedTexSubImage2D),
        NVal::DeclareNapiFunction("uniform1fv", WebGLRenderingContextOverloads::Uniform1fv),
        NVal::DeclareNapiFunction("uniform2fv", WebGLRenderingContextOverloads::Uniform2fv),
        NVal::DeclareNapiFunction("uniform3fv", WebGLRenderingContextOverloads::Uniform3fv),
        NVal::DeclareNapiFunction("uniform4fv", WebGLRenderingContextOverloads::Uniform4fv),
        NVal::DeclareNapiFunction("uniform1iv", WebGLRenderingContextOverloads::Uniform1iv),
        NVal::DeclareNapiFunction("uniform2iv", WebGLRenderingContextOverloads::Uniform2iv),
        NVal::DeclareNapiFunction("uniform3iv", WebGLRenderingContextOverloads::Uniform3iv),
        NVal::DeclareNapiFunction("uniform4iv", WebGLRenderingContextOverloads::Uniform4iv),
        NVal::DeclareNapiFunction("uniformMatrix2fv", WebGLRenderingContextOverloads::UniformMatrix2fv),
        NVal::DeclareNapiFunction("uniformMatrix3fv", WebGLRenderingContextOverloads::UniformMatrix3fv),
        NVal::DeclareNapiFunction("uniformMatrix4fv", WebGLRenderingContextOverloads::UniformMatrix4fv),
        NVal::DeclareNapiFunction("readPixels", WebGLRenderingContextOverloads::ReadPixels),
        NVal::DeclareNapiFunction("invalidateFramebuffer", WebGL2RenderingContextBase::InvalidateFramebuffer),
        NVal::DeclareNapiFunction("invalidateSubFramebuffer", WebGL2RenderingContextBase::InvalidateSubFramebuffer),
        NVal::DeclareNapiFunction("getInternalformatParameter", WebGL2RenderingContextBase::GetInternalformatParameter),
        NVal::DeclareNapiFunction("transformFeedbackVaryings", WebGL2RenderingContextBase::TransformFeedbackVaryings),
        NVal::DeclareNapiFunction("getUniformIndices", WebGL2RenderingContextBase::GetUniformIndices),
        NVal::DeclareNapiFunction("getActiveUniforms", WebGL2RenderingContextBase::GetActiveUniforms),
        NVal::DeclareNapiFunction("getActiveUniformBlockName", WebGL2RenderingContextBase::GetActiveUniformBlockName),
        NVal::DeclareNapiGetter("READ_BUFFER", GetReadBuffer),
        NVal::DeclareNapiGetter("UNPACK_ROW_LENGTH", GetUnpackRowLength),
        NVal::DeclareNapiGetter("UNPACK_SKIP_ROWS", GetUnpackSkipRows),
        NVal::DeclareNapiGetter("UNPACK_SKIP_PIXELS", GetUnpackSkipPixels),
        NVal::DeclareNapiGetter("PACK_ROW_LENGTH", GetPackRowLength),
        NVal::DeclareNapiGetter("PACK_SKIP_ROWS", GetPackSkipRows),
        NVal::DeclareNapiGetter("PACK_SKIP_PIXELS", GetPackSkipPixels),
        NVal::DeclareNapiGetter("COLOR", GetColor),
        NVal::DeclareNapiGetter("DEPTH", GetDepth),
        NVal::DeclareNapiGetter("STENCIL", GetStencil),
        NVal::DeclareNapiGetter("RED", GetRed),
        NVal::DeclareNapiGetter("RGB8", GetRgb8),
        NVal::DeclareNapiGetter("RGBA8", GetRgba8),
        NVal::DeclareNapiGetter("RGB10_A2", GetRgb10A2),
        NVal::DeclareNapiGetter("TEXTURE_BINDING_3D", GetTextureBinding3D),
        NVal::DeclareNapiGetter("UNPACK_SKIP_IMAGES", GetUnpackSkipImages),
        NVal::DeclareNapiGetter("UNPACK_IMAGE_HEIGHT", GetUnpackIamgeHeight),
        NVal::DeclareNapiGetter("TEXTURE_3D", GetTexture3D),
        NVal::DeclareNapiGetter("TEXTURE_WRAP_R", GetTextureWrapR),
        NVal::DeclareNapiGetter("MAX_3D_TEXTURE_SIZE", GetMax3DTextueSize),
        NVal::DeclareNapiGetter("UNSIGNED_INT_2_10_10_10_REV", GetUnsgnedInt2101010Rev),
        NVal::DeclareNapiGetter("MAX_ELEMENTS_VERTICES", GeMaxElementVertices),
        NVal::DeclareNapiGetter("MAX_ELEMENTS_INDICES", GetElementIndices),
        NVal::DeclareNapiGetter("TEXTURE_MIN_LOD", GetTextureMinLod),
        NVal::DeclareNapiGetter("TEXTURE_MAX_LOD", GetTextureMaxLod),
        NVal::DeclareNapiGetter("TEXTURE_BASE_LEVEL", GetTextureBaseLevel),
        NVal::DeclareNapiGetter("TEXTURE_MAX_LEVEL", GetTextureMaxLevel),
        NVal::DeclareNapiGetter("MIN", GetMin),
        NVal::DeclareNapiGetter("MAX", GetMax),
        NVal::DeclareNapiGetter("DEPTH_COMPONENT24", GetDepthComponent24),
        NVal::DeclareNapiGetter("MAX_TEXTURE_LOD_BIAS", GetMaxTextureLodBias),
        NVal::DeclareNapiGetter("TEXTURE_COMPARE_MODE", GetTextureCompareMode),
        NVal::DeclareNapiGetter("TEXTURE_COMPARE_FUNC", GetTextureCompareFunc),
        NVal::DeclareNapiGetter("CURRENT_QUERY", GetCurrentQuery),
        NVal::DeclareNapiGetter("QUERY_RESULT", GetQueryResult),
        NVal::DeclareNapiGetter("QUERY_RESULT_AVAILABLE", GetQueryResultAvailable),
        NVal::DeclareNapiGetter("STREAM_READ", GetStreamRead),
        NVal::DeclareNapiGetter("STREAM_COPY", GetStreamCopy),
        NVal::DeclareNapiGetter("STATIC_READ", GetStaticRead),
        NVal::DeclareNapiGetter("STATIC_COPY", GetStaticCopy),
        NVal::DeclareNapiGetter("DYNAMIC_READ", GetDynamicRead),
        NVal::DeclareNapiGetter("DYNAMIC_COPY", GetDynamicCopy),
        NVal::DeclareNapiGetter("MAX_DRAW_BUFFERS", GetMaxDrawBuffers),
        NVal::DeclareNapiGetter("DRAW_BUFFER0", GetDrawBuffer0),
        NVal::DeclareNapiGetter("DRAW_BUFFER1", GetDrawBuffer1),
        NVal::DeclareNapiGetter("DRAW_BUFFER2", GetDrawBuffer2),
        NVal::DeclareNapiGetter("DRAW_BUFFER3", GetDrawBuffer3),
        NVal::DeclareNapiGetter("DRAW_BUFFER4", GetDrawBuffer4),
        NVal::DeclareNapiGetter("DRAW_BUFFER5", GetDrawBuffer5),
        NVal::DeclareNapiGetter("DRAW_BUFFER6", GetDrawBuffer6),
        NVal::DeclareNapiGetter("DRAW_BUFFER7", GetDrawBuffer7),
        NVal::DeclareNapiGetter("DRAW_BUFFER8", GetDrawBuffer8),
        NVal::DeclareNapiGetter("DRAW_BUFFER9", GetDrawBuffer9),
        NVal::DeclareNapiGetter("DRAW_BUFFER10", GetDrawBuffer10),
        NVal::DeclareNapiGetter("DRAW_BUFFER11", GetDrawBuffer11),
        NVal::DeclareNapiGetter("DRAW_BUFFER12", GetDrawBuffer12),
        NVal::DeclareNapiGetter("DRAW_BUFFER13", GetDrawBuffer13),
        NVal::DeclareNapiGetter("DRAW_BUFFER14", GetDrawBuffer14),
        NVal::DeclareNapiGetter("DRAW_BUFFER15", GetDrawBuffer15),
        NVal::DeclareNapiGetter("MAX_FRAGMENT_UNIFORM_COMPONENTS", GetMaxFragmentUniformComponents),
        NVal::DeclareNapiGetter("MAX_VERTEX_UNIFORM_COMPONENTS", GetMaxVertexUniformComponents),
        NVal::DeclareNapiGetter("SAMPLER_3D", GetSampler3D),
        NVal::DeclareNapiGetter("SAMPLER_2D_SHADOW", GetSample2DShadow),
        NVal::DeclareNapiGetter("FRAGMENT_SHADER_DERIVATIVE_HINT", GetFragmentShaderDerivativeHint),
        NVal::DeclareNapiGetter("PIXEL_PACK_BUFFER", GetPixelPackBuffer),
        NVal::DeclareNapiGetter("PIXEL_UNPACK_BUFFER", GetPixelUnpackBuffer),
        NVal::DeclareNapiGetter("PIXEL_PACK_BUFFER_BINDING", GetPixelPackBufferBinding),
        NVal::DeclareNapiGetter("PIXEL_UNPACK_BUFFER_BINDING", GetPixelUnpackBufferBinding),
        NVal::DeclareNapiGetter("FLOAT_MAT2x3", GetFloatMat2x3),
        NVal::DeclareNapiGetter("FLOAT_MAT2x4", GetFloatMat2x4),
        NVal::DeclareNapiGetter("FLOAT_MAT3x2", GetFloatMat3x2),
        NVal::DeclareNapiGetter("FLOAT_MAT3x4", GetFloatMat3x4),
        NVal::DeclareNapiGetter("FLOAT_MAT4x2", GetFloatMat4x2),
        NVal::DeclareNapiGetter("FLOAT_MAT4x3", GetFloatMat4x3),
        NVal::DeclareNapiGetter("SRGB", GetSrgb),
        NVal::DeclareNapiGetter("SRGB8", GetSrgb8),
        NVal::DeclareNapiGetter("SRGB8_ALPHA8", GetSrgb8Alpha8),
        NVal::DeclareNapiGetter("COMPARE_REF_TO_TEXTURE", GetCompareRefToTexture),
        NVal::DeclareNapiGetter("RGBA32F", GetRgba32f),
        NVal::DeclareNapiGetter("RGB32F", GetRgb32f),
        NVal::DeclareNapiGetter("RGBA16F", GetRgba16f),
        NVal::DeclareNapiGetter("RGB16F", GetRgb16f),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_INTEGER", GetVertexAttribArrayInteger),
        NVal::DeclareNapiGetter("MAX_ARRAY_TEXTURE_LAYERS", GetMaxArrayTextureLayers),
        NVal::DeclareNapiGetter("MIN_PROGRAM_TEXEL_OFFSET", GetMinProgramTexelOffset),
        NVal::DeclareNapiGetter("MAX_PROGRAM_TEXEL_OFFSET", GetMaxProgramTexelOffset),
        NVal::DeclareNapiGetter("MAX_VARYING_COMPONENTS", GetMaxVaryingComponents),
        NVal::DeclareNapiGetter("TEXTURE_2D_ARRAY", GetTexture2DArray),
        NVal::DeclareNapiGetter("TEXTURE_BINDING_2D_ARRAY", GetTextureBinding2DArray),
        NVal::DeclareNapiGetter("R11F_G11F_B10F", GetR11fG11fB10f),
        NVal::DeclareNapiGetter("UNSIGNED_INT_10F_11F_11F_REV", GetUnsignedInt10f11f11fRev),
        NVal::DeclareNapiGetter("RGB9_E5", GetRgb9E5),
        NVal::DeclareNapiGetter("UNSIGNED_INT_5_9_9_9_REV", GetUnsignedInt5999Rev),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_BUFFER_MODE", GetTransformFeedbackBufferMode),
        NVal::DeclareNapiGetter("MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS",
            GeMaxTransformFeedbackSeparateComponents),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_VARYINGS", GetTransformFeedbackVaryings),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_BUFFER_START", GetTransformFeedbackBufferStart),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_BUFFER_SIZE", GetTransformFeedbackBufferSize),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN", GetTransformFeedbackPrimitivesWritten),
        NVal::DeclareNapiGetter("RASTERIZER_DISCARD", GetRasterizerDiscard),
        NVal::DeclareNapiGetter("MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS",
            GetMaxTransformFeedbackInterLeavedComponents),
        NVal::DeclareNapiGetter("MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS",
            GetMaxTransformFeedbackSeparateAttribs),
        NVal::DeclareNapiGetter("INTERLEAVED_ATTRIBS", GetInterleavedAttribs),
        NVal::DeclareNapiGetter("SEPARATE_ATTRIBS", GetSeparateAttribs),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_BUFFER", GetTransformFeedbackBuffer),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_BUFFER_BINDING", GetTransformFeedbackBufferBinding),
        NVal::DeclareNapiGetter("RGBA32UI", GetRgba32Ui),
        NVal::DeclareNapiGetter("RGB32UI", GetRgb32Ui),
        NVal::DeclareNapiGetter("RGBA16UI", GetRgba16Ui),
        NVal::DeclareNapiGetter("RGB16UI", GetRgb16Ui),
        NVal::DeclareNapiGetter("RGBA8UI", GetRgba8Ui),
        NVal::DeclareNapiGetter("RGB8UI", GetRgb8Ui),
        NVal::DeclareNapiGetter("RGBA32I", GetRgba32i),
        NVal::DeclareNapiGetter("RGB32I", GetRgb32i),
        NVal::DeclareNapiGetter("RGBA16I", GetRgba16i),
        NVal::DeclareNapiGetter("RGB16I", GetRgb16i),
        NVal::DeclareNapiGetter("RGBA8I", GetRgba8i),
        NVal::DeclareNapiGetter("RGB8I", GetRgb8i),
        NVal::DeclareNapiGetter("RED_INTEGER", GetReadInteger),
        NVal::DeclareNapiGetter("RGB_INTEGER", GetRgbInteger),
        NVal::DeclareNapiGetter("RGBA_INTEGER", GetRgbaInteger),
        NVal::DeclareNapiGetter("SAMPLER_2D_ARRAY", GetSample2DArray),
        NVal::DeclareNapiGetter("SAMPLER_2D_ARRAY_SHADOW", GetSample2DArrayShadow),
        NVal::DeclareNapiGetter("SAMPLER_CUBE_SHADOW", GetSampleCubeShadow),
        NVal::DeclareNapiGetter("UNSIGNED_INT_VEC2", GetUnsignedIntVec2),
        NVal::DeclareNapiGetter("UNSIGNED_INT_VEC3", GetUnsignedIntVec3),
        NVal::DeclareNapiGetter("UNSIGNED_INT_VEC4", GetUnsignedIntVec4),
        NVal::DeclareNapiGetter("INT_SAMPLER_2D", GetIntSample2D),
        NVal::DeclareNapiGetter("INT_SAMPLER_3D", GetIntSample3D),
        NVal::DeclareNapiGetter("INT_SAMPLER_CUBE", GetIntSampleCube),
        NVal::DeclareNapiGetter("INT_SAMPLER_2D_ARRAY", GetIntSample2DArray),
        NVal::DeclareNapiGetter("UNSIGNED_INT_SAMPLER_2D", GetUnsignedIntSample2D),
        NVal::DeclareNapiGetter("UNSIGNED_INT_SAMPLER_3D", GetUnsignedIntSample3D),
        NVal::DeclareNapiGetter("UNSIGNED_INT_SAMPLER_CUBE", GetUnsignedIntSampleCube),
        NVal::DeclareNapiGetter("UNSIGNED_INT_SAMPLER_2D_ARRAY", GetUnsignedIntSample2DArray),
        NVal::DeclareNapiGetter("DEPTH_COMPONENT32F", GetDepthComponent32f),
        NVal::DeclareNapiGetter("DEPTH32F_STENCIL8", GetDepth32FStencil8),
        NVal::DeclareNapiGetter("FLOAT_32_UNSIGNED_INT_24_8_REV", GeFloat32UnsignedInt248Rev),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING", GtFramebufferAttachmentColorEncoding),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE", GtFramebufferAttachmentComponentType),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_RED_SIZE", GtFramebufferAttachmentRedSize),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_GREEN_SIZE", GtFramebufferAttachmentGreenSize),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_BLUE_SIZE", GtFramebufferAttachmentBlueSize),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE", GtFramebufferAttachmentAlphaSize),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE", GtFramebufferAttachmentDepthSize),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE", GtFramebufferAttachmentStencilSize),
        NVal::DeclareNapiGetter("FRAMEBUFFER_DEFAULT", GetFramebufferDefault),
        NVal::DeclareNapiGetter("UNSIGNED_INT_24_8", GetUnsignedInt248),
        NVal::DeclareNapiGetter("DEPTH24_STENCIL8", GetDepth24Stencil8),
        NVal::DeclareNapiGetter("UNSIGNED_NORMALIZED", GetUnsignedNormalized),
        NVal::DeclareNapiGetter("DRAW_FRAMEBUFFER_BINDING", GetDrawFramebufferBinding),
        NVal::DeclareNapiGetter("READ_FRAMEBUFFER", GetReadFramebuffer),
        NVal::DeclareNapiGetter("DRAW_FRAMEBUFFER", GetDrawFramebuffer),
        NVal::DeclareNapiGetter("READ_FRAMEBUFFER_BINDING", GetFrameBufferBinding),
        NVal::DeclareNapiGetter("RENDERBUFFER_SAMPLES", GetRenderbufferSamples),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER", GetFramebufferAttachmentTexturelLayer),
        NVal::DeclareNapiGetter("MAX_COLOR_ATTACHMENTS", GetMaxColorAttachments),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT1", GetColorAttachment1),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT2", GetColorAttachment2),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT3", GetColorAttachment3),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT4", GetColorAttachment4),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT5", GetColorAttachment5),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT6", GetColorAttachment6),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT7", GetColorAttachment7),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT8", GetColorAttachment8),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT9", GetColorAttachment9),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT10", GetColorAttachment10),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT11", GetColorAttachment11),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT12", GetColorAttachment12),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT13", GetColorAttachment13),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT14", GetColorAttachment14),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT15", GetColorAttachment15),
        NVal::DeclareNapiGetter("FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", GetFramebufferIncompleteMultiSample),
        NVal::DeclareNapiGetter("MAX_SAMPLES", GetMaxSamples),
        NVal::DeclareNapiGetter("HALF_FLOAT", GetHalfFloat),
        NVal::DeclareNapiGetter("RG", GetRg),
        NVal::DeclareNapiGetter("RG_INTEGER", GetRgInteger),
        NVal::DeclareNapiGetter("R8", GetR8),
        NVal::DeclareNapiGetter("RG8", GetRg8),
        NVal::DeclareNapiGetter("R16F", GetR16f),
        NVal::DeclareNapiGetter("R32F", GetR32f),
        NVal::DeclareNapiGetter("RG16F", GetRg16f),
        NVal::DeclareNapiGetter("RG32F", GetRg32f),
        NVal::DeclareNapiGetter("R8I", GetR8i),
        NVal::DeclareNapiGetter("R8UI", GetR8Ui),
        NVal::DeclareNapiGetter("R16I", GetR16i),
        NVal::DeclareNapiGetter("R16UI", GetR16Ui),
        NVal::DeclareNapiGetter("R32I", GetR32i),
        NVal::DeclareNapiGetter("R32UI", GetR32Ui),
        NVal::DeclareNapiGetter("RG8I", GetRg8i),
        NVal::DeclareNapiGetter("RG8UI", GetRg8ui),
        NVal::DeclareNapiGetter("RG16I", GetRg16i),
        NVal::DeclareNapiGetter("RG16UI", GetRg16Ui),
        NVal::DeclareNapiGetter("RG32I", GetRg32i),
        NVal::DeclareNapiGetter("RG32UI", GetRg32Ui),
        NVal::DeclareNapiGetter("VERTEX_ARRAY_BINDING", GetVertexArrayBinding),
        NVal::DeclareNapiGetter("R8_SNORM", GetR8Snorm),
        NVal::DeclareNapiGetter("RG8_SNORM", GetRg8Snorm),
        NVal::DeclareNapiGetter("RGB8_SNORM", GetRgb8Snorm),
        NVal::DeclareNapiGetter("RGBA8_SNORM", GetRgba8Snorm),
        NVal::DeclareNapiGetter("SIGNED_NORMALIZED", GetSignedNormalized),
        NVal::DeclareNapiGetter("COPY_READ_BUFFER", GetCopyReadBuffer),
        NVal::DeclareNapiGetter("COPY_WRITE_BUFFER", GetCopyWriteBuffer),
        NVal::DeclareNapiGetter("COPY_READ_BUFFER_BINDING", GetCopyWriteReadBufferBinding),
        NVal::DeclareNapiGetter("COPY_WRITE_BUFFER_BINDING", GetCopyWriteBufferBinding),
        NVal::DeclareNapiGetter("UNIFORM_BUFFER", GetUniformBuffer),
        NVal::DeclareNapiGetter("UNIFORM_BUFFER_BINDING", GetUniformBufferBinding),
        NVal::DeclareNapiGetter("UNIFORM_BUFFER_START", GtUniformBufferStart),
        NVal::DeclareNapiGetter("UNIFORM_BUFFER_SIZE", GetUniformBufferSize),
        NVal::DeclareNapiGetter("MAX_VERTEX_UNIFORM_BLOCKS", GetMaxVertexUniformBlocks),
        NVal::DeclareNapiGetter("MAX_FRAGMENT_UNIFORM_BLOCKS", GetMaxFragmentUniformBlocks),
        NVal::DeclareNapiGetter("MAX_COMBINED_UNIFORM_BLOCKS", GetMaxCombinedUniformBlocks),
        NVal::DeclareNapiGetter("MAX_UNIFORM_BUFFER_BINDINGS", GetUniformBufferBinfings),
        NVal::DeclareNapiGetter("MAX_UNIFORM_BLOCK_SIZE", GetUniformBlockSize),
        NVal::DeclareNapiGetter("MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", GetMaxCombinedVertexUniformComponents),
        NVal::DeclareNapiGetter("MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS",
            GetMaxCombinedFragmentUniformComponents),
        NVal::DeclareNapiGetter("UNIFORM_BUFFER_OFFSET_ALIGNMENT", GetUniformBufferOffsetAlignment),
        NVal::DeclareNapiGetter("ACTIVE_UNIFORM_BLOCKS", GetActiveUniformBlocks),
        NVal::DeclareNapiGetter("UNIFORM_TYPE", GetUniformType),
        NVal::DeclareNapiGetter("UNIFORM_SIZE", GetUniformSize),
        NVal::DeclareNapiGetter("UNIFORM_BLOCK_INDEX", GetUniformIndex),
        NVal::DeclareNapiGetter("UNIFORM_OFFSET", GetUniformOffset),
        NVal::DeclareNapiGetter("UNIFORM_ARRAY_STRIDE", GetUniformArrayStride),
        NVal::DeclareNapiGetter("UNIFORM_MATRIX_STRIDE", GetUniformMatrixStride),
        NVal::DeclareNapiGetter("UNIFORM_IS_ROW_MAJOR", GetUniformIsRowMajor),
        NVal::DeclareNapiGetter("UNIFORM_BLOCK_BINDING", GetUniformBlockBinding),
        NVal::DeclareNapiGetter("UNIFORM_BLOCK_DATA_SIZE", GetUniformBlockDataSize),
        NVal::DeclareNapiGetter("UNIFORM_BLOCK_ACTIVE_UNIFORMS", GetUniformBlockActiveUniforms),
        NVal::DeclareNapiGetter("UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES", GetUniformBlockActiveUniformIndices),
        NVal::DeclareNapiGetter("UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER",
            GetUniformBlockReferencedByVertexShader),
        NVal::DeclareNapiGetter("UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER",
            GetUniformBlockReferencedByFragmentShader),
        NVal::DeclareNapiGetter("INVALID_INDEX", GetInvalidIndex),
        NVal::DeclareNapiGetter("MAX_VERTEX_OUTPUT_COMPONENTS", GetMaxVertexOutputComponents),
        NVal::DeclareNapiGetter("MAX_FRAGMENT_INPUT_COMPONENTS", GetMaxFragmentInputComponents),
        NVal::DeclareNapiGetter("MAX_SERVER_WAIT_TIMEOUT", GetMaxServerWaitTimeout),
        NVal::DeclareNapiGetter("OBJECT_TYPE", GetObjectType),
        NVal::DeclareNapiGetter("SYNC_CONDITION", GetSyncCondition),
        NVal::DeclareNapiGetter("SYNC_STATUS", GetSyncStatus),
        NVal::DeclareNapiGetter("SYNC_FLAGS", GetSyncFlags),
        NVal::DeclareNapiGetter("SYNC_FENCE", GetSyncFence),
        NVal::DeclareNapiGetter("SYNC_GPU_COMMANDS_COMPLETE", GetSyncGpuCommandsComplete),
        NVal::DeclareNapiGetter("UNSIGNALED", GetUnsignaled),
        NVal::DeclareNapiGetter("SIGNALED", GetSignaled),
        NVal::DeclareNapiGetter("ALREADY_SIGNALED", GetAlreadySignaled),
        NVal::DeclareNapiGetter("TIMEOUT_EXPIRED", GetTimeOutExpired),
        NVal::DeclareNapiGetter("CONDITION_SATISFIED", GetConditionSatisfied),
        NVal::DeclareNapiGetter("WAIT_FAILED", GetWaitFailed),
        NVal::DeclareNapiGetter("SYNC_FLUSH_COMMANDS_BIT", GetSyncFlushCommandsBit),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_DIVISOR", GetVertexAttribArrayDivisor),
        NVal::DeclareNapiGetter("ANY_SAMPLES_PASSED", GetAnySamplePassed),
        NVal::DeclareNapiGetter("ANY_SAMPLES_PASSED_CONSERVATIVE", GetAnySamplePassedConservative),
        NVal::DeclareNapiGetter("SAMPLER_BINDING", GetSampleBinding),
        NVal::DeclareNapiGetter("RGB10_A2UI", GetRgb10A2Ui),
        NVal::DeclareNapiGetter("INT_2_10_10_10_REV", GetInt2101010Rev),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK", GetTransformFeedback),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_PAUSED", GetTransformFeedbackPaused),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_ACTIVE", GetTransformFeedbackActive),
        NVal::DeclareNapiGetter("TRANSFORM_FEEDBACK_BINDING", GetTransformFeedbackBinding),
        NVal::DeclareNapiGetter("TEXTURE_IMMUTABLE_FORMAT", GetTextureImmutableFormat),
        NVal::DeclareNapiGetter("MAX_ELEMENT_INDEX", GetMaxElementIndex),
        NVal::DeclareNapiGetter("TEXTURE_IMMUTABLE_LEVELS", GetTextureImmutableLevels),
        NVal::DeclareNapiGetter("TIMEOUT_IGNORED", GetTimeoutIgnored),
        NVal::DeclareNapiGetter("MAX_CLIENT_WAIT_TIMEOUT_WEBGL", GetMaxClientWaitTimeoutWebgl),
        // override
        NVal::DeclareNapiFunction("bindBuffer", WebGL2RenderingContextBase::BindBuffer),
        NVal::DeclareNapiFunction("deleteBuffer", WebGL2RenderingContextBase::DeleteBuffer),
        NVal::DeclareNapiFunction("renderbufferStorage", WebGL2RenderingContextBase::RenderbufferStorage),
        NVal::DeclareNapiFunction("getParameter", WebGL2RenderingContextBase::GetParameter),
        NVal::DeclareNapiFunction("getTexParameter", WebGL2RenderingContextBase::GetTexParameter),
        NVal::DeclareNapiFunction("getFramebufferAttachmentParameter",
            WebGL2RenderingContextBase::GetFramebufferAttachmentParameter),
        NVal::DeclareNapiProperty("WebGLRenderingContext", instanceValue),
    };
    std::vector<napi_property_descriptor> properties = {};
    WebGLRenderingContextBase::GetRenderingContextBasePropertyDesc(properties);
    properties.insert(properties.end(), props.begin(), props.end());
    LOGD("WebGLRenderingContext properties %{public}d", static_cast<int>(properties.size()));
    napi_status status = napi_define_properties(env, exports, properties.size(), properties.data());
    if (status != napi_ok) {
        return false;
    }
    return true;
}

string WebGL2RenderingContext::GetClassName()
{
    return WebGL2RenderingContext::className;
}

WebGL2RenderingContext::~WebGL2RenderingContext()
{
    LOGD("WebGL2RenderingContext::~WebGL2RenderingContext");
    ObjectManager::GetInstance().DeleteWebGLObject(true, this);
}
} // namespace Rosen
} // namespace OHOS
#ifdef __cplusplus
}
#endif

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

#include "../include/context/webgl_rendering_context.h"

#include "../include/util/object_manager.h"
#include "../include/util/log.h"
#include "../../common/napi/n_class.h"
#include "../../common/napi/n_func_arg.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;

static napi_value GetDepthBufferBit(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_BUFFER_BIT).val_;
}

static napi_value GetStencilBufferBit(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BUFFER_BIT).val_;
}

static napi_value GetColorBufferBit(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::COLOR_BUFFER_BIT).val_;
}

static napi_value GetPoints(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::POINTS).val_;
}

static napi_value GetLines(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LINES).val_;
}

static napi_value GetLineLoop(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LINE_LOOP).val_;
}

static napi_value GetLineStrip(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LINE_STRIP).val_;
}

static napi_value GetTriangles(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TRIANGLES).val_;
}

static napi_value GetTrianglesStrip(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TRIANGLE_STRIP).val_;
}

static napi_value GetTrianglesStripFan(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TRIANGLE_FAN).val_;
}

static napi_value GetZero(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ZERO).val_;
}

static napi_value GetOne(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ONE).val_;
}

static napi_value GetSrcColor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SRC_COLOR).val_;
}

static napi_value GetOneMinusSrcColor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ONE_MINUS_SRC_COLOR).val_;
}

static napi_value GetSrcAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SRC_ALPHA).val_;
}

static napi_value GetOneMinusSrcAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ONE_MINUS_SRC_ALPHA).val_;
}

static napi_value GetDstAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DST_ALPHA).val_;
}

static napi_value GetOneMinusDstAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ONE_MINUS_DST_ALPHA).val_;
}

static napi_value GetDstColor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DST_COLOR).val_;
}

static napi_value GetOneMinusDstColor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ONE_MINUS_DST_COLOR).val_;
}

static napi_value GetSrcAlphaSaturate(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SRC_ALPHA_SATURATE).val_;
}

static napi_value GetFuncAdd(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FUNC_ADD).val_;
}

static napi_value GetBlendEquation(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND_EQUATION).val_;
}

static napi_value GetBlendEquationRgb(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND_EQUATION_RGB).val_;
}

static napi_value GetBlendEquationAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND_EQUATION_ALPHA).val_;
}

static napi_value GetFuncSubtract(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FUNC_SUBTRACT).val_;
}

static napi_value GetFuncReverseSubtract(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FUNC_REVERSE_SUBTRACT).val_;
}

static napi_value GetBlendDstRgb(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND_DST_RGB).val_;
}

static napi_value GetBlendSrcRgb(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND_SRC_RGB).val_;
}

static napi_value GetBlendDstAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND_DST_ALPHA).val_;
}

static napi_value GetBlendSrcAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND_SRC_ALPHA).val_;
}

static napi_value GetConstantColor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CONSTANT_COLOR).val_;
}

static napi_value GetOneMinusConstantColor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ONE_MINUS_CONSTANT_COLOR).val_;
}

static napi_value GetConstantAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CONSTANT_ALPHA).val_;
}

static napi_value GetOneMinusCostantAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ONE_MINUS_CONSTANT_ALPHA).val_;
}

static napi_value GetBlendColor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND_COLOR).val_;
}

static napi_value GetArrayBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ARRAY_BUFFER).val_;
}

static napi_value GetElementArrayBuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ELEMENT_ARRAY_BUFFER).val_;
}

static napi_value GetArrayBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ARRAY_BUFFER_BINDING).val_;
}

static napi_value GetElementArrayBufferBinding(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ELEMENT_ARRAY_BUFFER_BINDING).val_;
}

static napi_value GetStreamDraw(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STREAM_DRAW).val_;
}

static napi_value GetStaticDraw(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STATIC_DRAW).val_;
}

static napi_value GetDynamicDraw(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DYNAMIC_DRAW).val_;
}

static napi_value GetBufferSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BUFFER_SIZE).val_;
}

static napi_value GetBufferUsage(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BUFFER_USAGE).val_;
}

static napi_value GetCurrentVertexAttrib(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CURRENT_VERTEX_ATTRIB).val_;
}

static napi_value GetFront(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRONT).val_;
}

static napi_value GetBack(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BACK).val_;
}

static napi_value GetFrontAndBack(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRONT_AND_BACK).val_;
}

static napi_value GetCullFace(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CULL_FACE).val_;
}

static napi_value GetBlend(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLEND).val_;
}

static napi_value GetDither(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DITHER).val_;
}

static napi_value GetStencilTest(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_TEST).val_;
}

static napi_value GetDepthTest(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_TEST).val_;
}

static napi_value GetScissorTest(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SCISSOR_TEST).val_;
}

static napi_value GetPolygonOffsetFill(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::POLYGON_OFFSET_FILL).val_;
}

static napi_value GetSampleAlphaToCoverage(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SAMPLE_ALPHA_TO_COVERAGE).val_;
}

static napi_value GetSampleCoverage(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SAMPLE_COVERAGE).val_;
}

static napi_value GetNoError(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::NO_ERROR).val_;
}

static napi_value GetInvalidEnum(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INVALID_ENUM).val_;
}

static napi_value GetInvalidValue(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INVALID_VALUE).val_;
}

static napi_value GetInvalidOperation(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INVALID_OPERATION).val_;
}

static napi_value GetOutOfMemory(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::OUT_OF_MEMORY).val_;
}

static napi_value GetCw(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CW).val_;
}

static napi_value GetCcw(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CCW).val_;
}

static napi_value GetLineWidth(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LINE_WIDTH).val_;
}

static napi_value GetAliasedPointSizeRange(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ALIASED_POINT_SIZE_RANGE).val_;
}

static napi_value GetAliasedLineWidthRange(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ALIASED_LINE_WIDTH_RANGE).val_;
}

static napi_value GetCullFaceMode(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CULL_FACE_MODE).val_;
}

static napi_value GetFrontFace(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRONT_FACE).val_;
}

static napi_value GetDepthRange(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_RANGE).val_;
}

static napi_value GetDepthWritemask(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_WRITEMASK).val_;
}

static napi_value GetDepthClearValue(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_CLEAR_VALUE).val_;
}

static napi_value GetDepthFunc(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_FUNC).val_;
}

static napi_value GetStencilClearValue(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_CLEAR_VALUE).val_;
}

static napi_value GetStencilFunc(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_FUNC).val_;
}

static napi_value GetStencilFail(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_FAIL).val_;
}

static napi_value GetStencilPassDepthFail(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_PASS_DEPTH_FAIL).val_;
}

static napi_value GetStencilPassDepthPass(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_PASS_DEPTH_PASS).val_;
}

static napi_value GetStencilRef(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_REF).val_;
}

static napi_value GetStencilValueMask(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_VALUE_MASK).val_;
}

static napi_value GetStencilWriteMask(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_WRITEMASK).val_;
}

static napi_value GetStencilBackFunc(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BACK_FUNC).val_;
}

static napi_value GetStencilBackFail(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BACK_FAIL).val_;
}

static napi_value GetStencilBackPassDepthFail(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BACK_PASS_DEPTH_FAIL).val_;
}

static napi_value GetStencilBackPassDepthPass(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BACK_PASS_DEPTH_PASS).val_;
}

static napi_value GetStencilBackRef(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BACK_REF).val_;
}

static napi_value GetStencilBackValueMask(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BACK_VALUE_MASK).val_;
}

static napi_value GetStencilBackWriteMask(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BACK_WRITEMASK).val_;
}

static napi_value GetViewport(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VIEWPORT).val_;
}

static napi_value GetScissorBox(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SCISSOR_BOX).val_;
}

static napi_value GetColorClearValue(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::COLOR_CLEAR_VALUE).val_;
}

static napi_value GetColorWriteMask(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::COLOR_WRITEMASK).val_;
}

static napi_value GetUnpackAlignment(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNPACK_ALIGNMENT).val_;
}

static napi_value GetPackAlignment(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::PACK_ALIGNMENT).val_;
}

static napi_value GetMaxTextureSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_TEXTURE_SIZE).val_;
}

static napi_value GetMaxViewportDims(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_VIEWPORT_DIMS).val_;
}

static napi_value GetSubpixelBits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SUBPIXEL_BITS).val_;
}

static napi_value GetRedBits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RED_BITS).val_;
}

static napi_value GetGreenBits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::GREEN_BITS).val_;
}

static napi_value GetBuleBits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BLUE_BITS).val_;
}

static napi_value GetAlphaBits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ALPHA_BITS).val_;
}

static napi_value GetDepthBits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_BITS).val_;
}

static napi_value GetStencilBits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_BITS).val_;
}

static napi_value GetPolygonOffsetUnits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::POLYGON_OFFSET_UNITS).val_;
}

static napi_value GetPolyonOffsetFactor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::POLYGON_OFFSET_FACTOR).val_;
}

static napi_value GetTextureBinding2D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_BINDING_2D).val_;
}

static napi_value GetSampleBuffers(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SAMPLE_BUFFERS).val_;
}

static napi_value GetSample(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SAMPLES).val_;
}

static napi_value GetSampleCoverageValue(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SAMPLE_COVERAGE_VALUE).val_;
}

static napi_value GetSampleCoverageInvert(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SAMPLE_COVERAGE_INVERT).val_;
}

static napi_value GetCompressedTextureFormats(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::COMPRESSED_TEXTURE_FORMATS).val_;
}

static napi_value GetDoneCare(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DONT_CARE).val_;
}

static napi_value GetFastest(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FASTEST).val_;
}

static napi_value GetIncest(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::NICEST).val_;
}

static napi_value GetGenerateMipMapHint(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::GENERATE_MIPMAP_HINT).val_;
}

static napi_value GetByte(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BYTE).val_;
}

static napi_value GetUnsignedByte(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNSIGNED_BYTE).val_;
}

static napi_value GetShort(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SHORT).val_;
}

static napi_value GetUnsignedShort(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNSIGNED_SHORT).val_;
}

static napi_value GetInt(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INT).val_;
}

static napi_value GetUnsignedInt(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNSIGNED_INT).val_;
}

static napi_value GetFloat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FLOAT).val_;
}

static napi_value GetDepthComponent(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_COMPONENT).val_;
}

static napi_value GetAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ALPHA).val_;
}

static napi_value GetRgb(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RGB).val_;
}

static napi_value GetRgba(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RGBA).val_;
}

static napi_value GetLuminance(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LUMINANCE).val_;
}

static napi_value GetLuminanceAlpha(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LUMINANCE_ALPHA).val_;
}

static napi_value GetUnsignedShort4444(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNSIGNED_SHORT_4_4_4_4).val_;
}

static napi_value GetUnsignedShort5551(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNSIGNED_SHORT_5_5_5_1).val_;
}

static napi_value GetUnsignedShort565(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNSIGNED_SHORT_5_6_5).val_;
}

static napi_value GetFragmentShader(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAGMENT_SHADER).val_;
}

static napi_value GetVertexShader(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERTEX_SHADER).val_;
}

static napi_value GetMaxVertexAttribs(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_VERTEX_ATTRIBS).val_;
}

static napi_value GetMaxVertexUniformVectors(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_VERTEX_UNIFORM_VECTORS).val_;
}

static napi_value GetMaxVaryingVectors(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_VARYING_VECTORS).val_;
}

static napi_value GetMaxCombinedTextureImageUnits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_COMBINED_TEXTURE_IMAGE_UNITS).val_;
}

static napi_value GetMaxVertexTextureImageUnits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_VERTEX_TEXTURE_IMAGE_UNITS).val_;
}

static napi_value GetMaxTextureImageUnits(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_TEXTURE_IMAGE_UNITS).val_;
}

static napi_value GeMaxFragmentUniformVectors(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_FRAGMENT_UNIFORM_VECTORS).val_;
}

static napi_value GetShaderType(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SHADER_TYPE).val_;
}

static napi_value GetDeleteStatus(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DELETE_STATUS).val_;
}

static napi_value GetLinkStatus(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LINK_STATUS).val_;
}

static napi_value GetValidateStatus(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VALIDATE_STATUS).val_;
}

static napi_value GetAttachedShader(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ATTACHED_SHADERS).val_;
}

static napi_value GetActiveUniforms(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ACTIVE_UNIFORMS).val_;
}

static napi_value GetActiveAttributes(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ACTIVE_ATTRIBUTES).val_;
}

static napi_value GetShadingLanguageVersion(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SHADING_LANGUAGE_VERSION).val_;
}

static napi_value GetCurrentProgram(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CURRENT_PROGRAM).val_;
}

static napi_value GetNever(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::NEVER).val_;
}

static napi_value GetLess(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LESS).val_;
}

static napi_value GetEqual(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::EQUAL).val_;
}

static napi_value GetLequal(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LEQUAL).val_;
}

static napi_value GetGreater(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::GREATER).val_;
}

static napi_value GetNotequal(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::NOTEQUAL).val_;
}

static napi_value GetGequal(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::GEQUAL).val_;
}

static napi_value GetAlways(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ALWAYS).val_;
}

static napi_value GetKeep(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::KEEP).val_;
}

static napi_value GetReplace(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::REPLACE).val_;
}

static napi_value GetIncr(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INCR).val_;
}

static napi_value GetDecr(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DECR).val_;
}

static napi_value GetInvert(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INVERT).val_;
}

static napi_value GetIncrWrap(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INCR_WRAP).val_;
}

static napi_value GetDecrWrap(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DECR_WRAP).val_;
}

static napi_value GetVendor(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VENDOR).val_;
}

static napi_value GetRenderer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERER).val_;
}

static napi_value GetVersion(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERSION).val_;
}

static napi_value GetNearest(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::NEAREST).val_;
}

static napi_value GetLinear(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LINEAR).val_;
}

static napi_value GetNearestMipmapNearest(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::NEAREST_MIPMAP_NEAREST).val_;
}

static napi_value GetLinearMipmapNearest(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LINEAR_MIPMAP_NEAREST).val_;
}

static napi_value GetNearestMipMapLinear(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::NEAREST_MIPMAP_LINEAR).val_;
}

static napi_value GetLinearMipmapLinear(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LINEAR_MIPMAP_LINEAR).val_;
}

static napi_value GetTextureMagFilter(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_MAG_FILTER).val_;
}

static napi_value GetTextureMinFilter(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_MIN_FILTER).val_;
}

static napi_value GetTextureWrapS(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_WRAP_S).val_;
}

static napi_value GetTextureWrapT(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_WRAP_T).val_;
}

static napi_value GetTexture2D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_2D).val_;
}

static napi_value GetTexture(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE).val_;
}

static napi_value GetTextureCubeMap(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_CUBE_MAP).val_;
}

static napi_value GetTextureBindingCubeMap(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_BINDING_CUBE_MAP).val_;
}

static napi_value GetTextureCubeMapPositiveX(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_X).val_;
}

static napi_value GetTextureCubeMapNegativeX(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_X).val_;
}

static napi_value GetTextureCubeMapPositiveY(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Y).val_;
}

static napi_value GetTextureCubeMapNegativeY(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Y).val_;
}

static napi_value GetTextureCubeMapPositiveZ(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Z).val_;
}

static napi_value GetTextureCubeMapNegativeZ(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Z).val_;
}

static napi_value GetMaxCubeMapTextureSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_CUBE_MAP_TEXTURE_SIZE).val_;
}

static napi_value GetTexture0(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE0).val_;
}

static napi_value GetTexture1(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE1).val_;
}

static napi_value GetTexture2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE2).val_;
}

static napi_value GetTexture3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE3).val_;
}

static napi_value GetTexture4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE4).val_;
}

static napi_value GetTexture5(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE5).val_;
}

static napi_value GetTexture6(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE6).val_;
}

static napi_value GetTexture7(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE7).val_;
}

static napi_value GetTexture8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE8).val_;
}

static napi_value GetTexture9(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE9).val_;
}

static napi_value GetTexture10(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE10).val_;
}

static napi_value GetTexture11(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE11).val_;
}

static napi_value GetTexture12(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE12).val_;
}

static napi_value GetTexture13(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE13).val_;
}

static napi_value GetTexture14(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE14).val_;
}

static napi_value GetTexture15(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE15).val_;
}

static napi_value GetTexture16(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE16).val_;
}

static napi_value GetTexture17(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE17).val_;
}

static napi_value GetTexture18(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE18).val_;
}

static napi_value GetTexture19(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE19).val_;
}

static napi_value GetTexture20(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE20).val_;
}

static napi_value GetTexture21(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE21).val_;
}

static napi_value GetTexture22(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE22).val_;
}

static napi_value GetTexture23(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE23).val_;
}

static napi_value GetTexture24(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE24).val_;
}

static napi_value GetTexture25(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE25).val_;
}

static napi_value GetTexture26(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE26).val_;
}

static napi_value GetTexture27(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE27).val_;
}

static napi_value GetTexture28(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE28).val_;
}

static napi_value GetTexture29(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE29).val_;
}

static napi_value GetTexture30(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE30).val_;
}

static napi_value GetTexture31(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::TEXTURE31).val_;
}

static napi_value GetActiveTexture(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::ACTIVE_TEXTURE).val_;
}

static napi_value GetRepeat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::REPEAT).val_;
}

static napi_value GetClampToEdge(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CLAMP_TO_EDGE).val_;
}

static napi_value GetMirroredRepeat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MIRRORED_REPEAT).val_;
}

static napi_value GetFloatVec2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FLOAT_VEC2).val_;
}

static napi_value GetFloatVec3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FLOAT_VEC3).val_;
}

static napi_value GetFloatVec4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FLOAT_VEC4).val_;
}

static napi_value GetIntVec2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INT_VEC2).val_;
}

static napi_value GetIntVec3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INT_VEC3).val_;
}

static napi_value GetIntVec4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INT_VEC4).val_;
}

static napi_value GetBool(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BOOL).val_;
}

static napi_value GetBoolVec2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BOOL_VEC2).val_;
}

static napi_value GetBoolVec3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BOOL_VEC3).val_;
}

static napi_value GetBoolVec4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BOOL_VEC4).val_;
}

static napi_value GetFloatMat2(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FLOAT_MAT2).val_;
}

static napi_value GetFloatMat3(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FLOAT_MAT3).val_;
}

static napi_value GetFloatMat4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FLOAT_MAT4).val_;
}

static napi_value GetSample2D(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SAMPLER_2D).val_;
}

static napi_value GetSamplerCube(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::SAMPLER_CUBE).val_;
}

static napi_value GetVertexAttribArrayEnabled(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_ENABLED).val_;
}

static napi_value GetVertexAttribArraySize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_SIZE).val_;
}

static napi_value GetVertexAttribArrayStride(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_STRIDE).val_;
}

static napi_value GetVertexAttribArrayType(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_TYPE).val_;
}

static napi_value GetVertexAttribArrayNormalized(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_NORMALIZED).val_;
}

static napi_value GetVertexAttribArrayPointer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_POINTER).val_;
}

static napi_value GetVertexAttribArrayBufferBinging(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_BUFFER_BINDING).val_;
}

static napi_value GetImplementationColorReadType(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::IMPLEMENTATION_COLOR_READ_TYPE).val_;
}

static napi_value GetImplementationColorReadFormat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::IMPLEMENTATION_COLOR_READ_FORMAT).val_;
}

static napi_value GetCompileStatus(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::COMPILE_STATUS).val_;
}

static napi_value GetLowFloat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LOW_FLOAT).val_;
}

static napi_value GetMediumFloat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MEDIUM_FLOAT).val_;
}

static napi_value GetHighFloat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::HIGH_FLOAT).val_;
}

static napi_value GetLowInt(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::LOW_INT).val_;
}

static napi_value GetMediumInt(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MEDIUM_INT).val_;
}

static napi_value GetHighInt(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::HIGH_INT).val_;
}

static napi_value Getframebuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER).val_;
}

static napi_value GetRenderbuffer(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER).val_;
}

static napi_value GetRgba4(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RGBA4).val_;
}

static napi_value GetRgb5A1(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RGB5_A1).val_;
}

static napi_value GetRgb565(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RGB565).val_;
}

static napi_value GetDepthComponent16(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_COMPONENT16).val_;
}

static napi_value GetStencilIndex8(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_INDEX8).val_;
}

static napi_value GetDepthStencil(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_STENCIL).val_;
}

static napi_value GetRenderbufferWidth(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_WIDTH).val_;
}

static napi_value GetRenderbufferHeight(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_HEIGHT).val_;
}

static napi_value GetRenderbufferInterNalFormat(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_INTERNAL_FORMAT).val_;
}

static napi_value GetRenderbufferRedSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_RED_SIZE).val_;
}

static napi_value GetRenderbufferGreenSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_GREEN_SIZE).val_;
}

static napi_value GetRenderbufferBlueSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_BLUE_SIZE).val_;
}

static napi_value GetRenderbufferAlphaSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_ALPHA_SIZE).val_;
}

static napi_value GetRenderbufferDepthSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_DEPTH_SIZE).val_;
}

static napi_value GetRenderbufferStencilSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_STENCIL_SIZE).val_;
}

static napi_value GetFramebufferAttachmentObjectType(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE).val_;
}

static napi_value GetFramebufferAttachmentObjectName(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_OBJECT_NAME).val_;
}

static napi_value GetFramebufferAttachmentTextureLevel(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL).val_;
}

static napi_value GetFramebufferTexureCubeMapFace(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE).val_;
}

static napi_value GetColorAttachment0(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::COLOR_ATTACHMENT0).val_;
}

static napi_value GetDepthAttachment(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_ATTACHMENT).val_;
}

static napi_value GetStencilAttachment(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::STENCIL_ATTACHMENT).val_;
}

static napi_value GetDepthStencilAttachment(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT).val_;
}

static napi_value GetNone(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::NONE).val_;
}

static napi_value GetFramebufferComplete(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE).val_;
}

static napi_value GetFramebufferIncompleteAttachment(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_INCOMPLETE_ATTACHMENT).val_;
}

static napi_value GetFramebufferIncompleteMissingAttachment(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT).val_;
}

static napi_value GetFramebufferIncompleteDimensions(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_INCOMPLETE_DIMENSIONS).val_;
}

static napi_value GetFramebufferUnsupported(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_UNSUPPORTED).val_;
}

static napi_value GetFramebufferBinging(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_BINDING).val_;
}

static napi_value GetRenderBufferBinging(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::RENDERBUFFER_BINDING).val_;
}

static napi_value GetMaxRenderBufferSize(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::MAX_RENDERBUFFER_SIZE).val_;
}

static napi_value GetInvalidFramebufferOperation(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::INVALID_FRAMEBUFFER_OPERATION).val_;
}

static napi_value GetUnpackFileYWebgl(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL).val_;
}

static napi_value GetUnpackPremultiplyAlphaWebgl(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL).val_;
}

static napi_value GetContextLostWebgl(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::CONTEXT_LOST_WEBGL).val_;
}

static napi_value GetUnpackColorSpaceConversionWebgl(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::UNPACK_COLORSPACE_CONVERSION_WEBGL).val_;
}

static napi_value GetBrowserDefaultWebgl(napi_env env, napi_callback_info info)
{
    return NVal::CreateInt64(env, WebGLRenderingContextBase::BROWSER_DEFAULT_WEBGL).val_;
}

bool WebGLRenderingContext::Export(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value contextClass = nullptr;
    napi_define_class(
        env, "WebGLRenderingContext", NAPI_AUTO_LENGTH,
        [](napi_env env, napi_callback_info info) -> napi_value {
            napi_value thisVar = nullptr;
            napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

            return thisVar;
        },
        nullptr, 0, nullptr, &contextClass);

    napi_value instanceValue = nullptr;
    napi_new_instance(env, contextClass, 0, nullptr, &instanceValue);
    status = napi_wrap(env, instanceValue, (void*)this, [](napi_env env, void* data, void* hint) {}, nullptr, nullptr);
    if (status != napi_ok) {
        return false;
    }
    napi_property_descriptor props[] = {
        NVal::DeclareNapiFunction("getContextAttributes", WebGLRenderingContextBase::GetContextAttributes),
        NVal::DeclareNapiFunction("isContextLost", WebGLRenderingContextBase::IsContextLost),
        NVal::DeclareNapiFunction("getSupportedExtensions", WebGLRenderingContextBase::GetSupportedExtensions),
        NVal::DeclareNapiFunction("getExtension", WebGLRenderingContextBase::GetExtension),
        NVal::DeclareNapiFunction("activeTexture", WebGLRenderingContextBase::ActiveTexture),
        NVal::DeclareNapiFunction("attachShader", WebGLRenderingContextBase::AttachShader),
        NVal::DeclareNapiFunction("bindAttribLocation", WebGLRenderingContextBase::BindAttribLocation),
        NVal::DeclareNapiFunction("bindBuffer", WebGLRenderingContextBase::BindBuffer),
        NVal::DeclareNapiFunction("bindFramebuffer", WebGLRenderingContextBase::BindFramebuffer),
        NVal::DeclareNapiFunction("bindRenderbuffer", WebGLRenderingContextBase::BindRenderbuffer),
        NVal::DeclareNapiFunction("bindTexture", WebGLRenderingContextBase::BindTexture),
        NVal::DeclareNapiFunction("blendColor", WebGLRenderingContextBase::BlendColor),
        NVal::DeclareNapiFunction("blendEquation", WebGLRenderingContextBase::BlendEquation),
        NVal::DeclareNapiFunction("blendEquationSeparate", WebGLRenderingContextBase::BlendEquationSeparate),
        NVal::DeclareNapiFunction("blendFunc", WebGLRenderingContextBase::BlendFunc),
        NVal::DeclareNapiFunction("blendFuncSeparate", WebGLRenderingContextBase::BlendFuncSeparate),
        NVal::DeclareNapiFunction("checkFramebufferStatus", WebGLRenderingContextBase::CheckFramebufferStatus),
        NVal::DeclareNapiFunction("clear", WebGLRenderingContextBase::Clear),
        NVal::DeclareNapiFunction("clearColor", WebGLRenderingContextBase::ClearColor),
        NVal::DeclareNapiFunction("clearDepth", WebGLRenderingContextBase::ClearDepth),
        NVal::DeclareNapiFunction("clearStencil", WebGLRenderingContextBase::ClearStencil),
        NVal::DeclareNapiFunction("colorMask", WebGLRenderingContextBase::ColorMask),
        NVal::DeclareNapiFunction("compileShader", WebGLRenderingContextBase::CompileShader),
        NVal::DeclareNapiFunction("copyTexImage2D", WebGLRenderingContextBase::CopyTexImage2D),
        NVal::DeclareNapiFunction("copyTexSubImage2D", WebGLRenderingContextBase::CopyTexSubImage2D),
        NVal::DeclareNapiFunction("createBuffer", WebGLRenderingContextBase::CreateBuffer),
        NVal::DeclareNapiFunction("createFramebuffer", WebGLRenderingContextBase::CreateFramebuffer),
        NVal::DeclareNapiFunction("createProgram", WebGLRenderingContextBase::CreateProgram),
        NVal::DeclareNapiFunction("createRenderbuffer", WebGLRenderingContextBase::CreateRenderbuffer),
        NVal::DeclareNapiFunction("createShader", WebGLRenderingContextBase::CreateShader),
        NVal::DeclareNapiFunction("createTexture", WebGLRenderingContextBase::CreateTexture),
        NVal::DeclareNapiFunction("cullFace", WebGLRenderingContextBase::CullFace),
        NVal::DeclareNapiFunction("deleteBuffer", WebGLRenderingContextBase::DeleteBuffer),
        NVal::DeclareNapiFunction("deleteFramebuffer", WebGLRenderingContextBase::DeleteFramebuffer),
        NVal::DeclareNapiFunction("deleteProgram", WebGLRenderingContextBase::DeleteProgram),
        NVal::DeclareNapiFunction("deleteRenderbuffer", WebGLRenderingContextBase::DeleteRenderbuffer),
        NVal::DeclareNapiFunction("deleteShader", WebGLRenderingContextBase::DeleteShader),
        NVal::DeclareNapiFunction("deleteTexture", WebGLRenderingContextBase::DeleteTexture),
        NVal::DeclareNapiFunction("depthFunc", WebGLRenderingContextBase::DepthFunc),
        NVal::DeclareNapiFunction("depthMask", WebGLRenderingContextBase::DepthMask),
        NVal::DeclareNapiFunction("depthRange", WebGLRenderingContextBase::DepthRange),
        NVal::DeclareNapiFunction("detachShader", WebGLRenderingContextBase::DetachShader),
        NVal::DeclareNapiFunction("disable", WebGLRenderingContextBase::Disable),
        NVal::DeclareNapiFunction("disableVertexAttribArray", WebGLRenderingContextBase::DisableVertexAttribArray),
        NVal::DeclareNapiFunction("drawArrays", WebGLRenderingContextBase::DrawArrays),
        NVal::DeclareNapiFunction("drawElements", WebGLRenderingContextBase::DrawElements),
        NVal::DeclareNapiFunction("enable", WebGLRenderingContextBase::Enable),
        NVal::DeclareNapiFunction("enableVertexAttribArray", WebGLRenderingContextBase::EnableVertexAttribArray),
        NVal::DeclareNapiFunction("finish", WebGLRenderingContextBase::Finish),
        NVal::DeclareNapiFunction("flush", WebGLRenderingContextBase::Flush),
        NVal::DeclareNapiFunction("framebufferRenderbuffer", WebGLRenderingContextBase::FramebufferRenderbuffer),
        NVal::DeclareNapiFunction("framebufferTexture2D", WebGLRenderingContextBase::FramebufferTexture2D),
        NVal::DeclareNapiFunction("viewport", WebGLRenderingContextBase::Viewport),
        NVal::DeclareNapiFunction("getUniformLocation", WebGLRenderingContextBase::GetUniformLocation),
        NVal::DeclareNapiFunction("getVertexAttribOffset", WebGLRenderingContextBase::GetVertexAttribOffset),
        NVal::DeclareNapiFunction("hint", WebGLRenderingContextBase::Hint),
        NVal::DeclareNapiFunction("isBuffer", WebGLRenderingContextBase::IsBuffer),
        NVal::DeclareNapiFunction("isEnabled", WebGLRenderingContextBase::IsEnabled),
        NVal::DeclareNapiFunction("renderbufferStorage", WebGLRenderingContextBase::RenderbufferStorage),
        NVal::DeclareNapiFunction("sampleCoverage", WebGLRenderingContextBase::SampleCoverage),
        NVal::DeclareNapiFunction("scissor", WebGLRenderingContextBase::Scissor),
        NVal::DeclareNapiFunction("shaderSource", WebGLRenderingContextBase::ShaderSource),
        NVal::DeclareNapiFunction("stencilFunc", WebGLRenderingContextBase::StencilFunc),
        NVal::DeclareNapiFunction("stencilFuncSeparate", WebGLRenderingContextBase::StencilFuncSeparate),
        NVal::DeclareNapiFunction("stencilMask", WebGLRenderingContextBase::StencilMask),
        NVal::DeclareNapiFunction("stencilMaskSeparate", WebGLRenderingContextBase::StencilMaskSeparate),
        NVal::DeclareNapiFunction("stencilOp", WebGLRenderingContextBase::StencilOp),
        NVal::DeclareNapiFunction("stencilOpSeparate", WebGLRenderingContextBase::StencilOpSeparate),
        NVal::DeclareNapiFunction("texParameterf", WebGLRenderingContextBase::TexParameterf),
        NVal::DeclareNapiFunction("texParameteri", WebGLRenderingContextBase::TexParameteri),
        NVal::DeclareNapiFunction("uniform1f", WebGLRenderingContextBase::Uniform1f),
        NVal::DeclareNapiFunction("uniform2f", WebGLRenderingContextBase::Uniform2f),
        NVal::DeclareNapiFunction("uniform3f", WebGLRenderingContextBase::Uniform3f),
        NVal::DeclareNapiFunction("uniform4f", WebGLRenderingContextBase::Uniform4f),
        NVal::DeclareNapiFunction("useProgram", WebGLRenderingContextBase::UseProgram),
        NVal::DeclareNapiFunction("uniform1i", WebGLRenderingContextBase::Uniform1i),
        NVal::DeclareNapiFunction("uniform2i", WebGLRenderingContextBase::Uniform2i),
        NVal::DeclareNapiFunction("uniform3i", WebGLRenderingContextBase::Uniform3i),
        NVal::DeclareNapiFunction("uniform4i", WebGLRenderingContextBase::Uniform4i),
        NVal::DeclareNapiFunction("validateProgram", WebGLRenderingContextBase::ValidateProgram),
        NVal::DeclareNapiFunction("vertexAttrib1f", WebGLRenderingContextBase::VertexAttrib1f),
        NVal::DeclareNapiFunction("vertexAttrib2f", WebGLRenderingContextBase::VertexAttrib2f),
        NVal::DeclareNapiFunction("vertexAttrib3f", WebGLRenderingContextBase::VertexAttrib3f),
        NVal::DeclareNapiFunction("vertexAttrib4f", WebGLRenderingContextBase::VertexAttrib4f),
        NVal::DeclareNapiFunction("vertexAttribPointer", WebGLRenderingContextBase::VertexAttribPointer),
        NVal::DeclareNapiFunction("frontFace", WebGLRenderingContextBase::FrontFace),
        NVal::DeclareNapiFunction("generateMipmap", WebGLRenderingContextBase::GenerateMipmap),
        NVal::DeclareNapiFunction("getActiveAttrib", WebGLRenderingContextBase::GetActiveAttrib),
        NVal::DeclareNapiFunction("getActiveUniform", WebGLRenderingContextBase::GetActiveUniform),
        NVal::DeclareNapiFunction("getAttribLocation", WebGLRenderingContextBase::GetAttribLocation),
        NVal::DeclareNapiFunction("getBufferParameter", WebGLRenderingContextBase::GetBufferParameter),
        NVal::DeclareNapiFunction("getError", WebGLRenderingContextBase::GetError),
        NVal::DeclareNapiFunction("getFramebufferAttachmentParameter",
                                  WebGLRenderingContextBase::GetFramebufferAttachmentParameter),
        NVal::DeclareNapiFunction("getProgramParameter", WebGLRenderingContextBase::GetProgramParameter),
        NVal::DeclareNapiFunction("getRenderbufferParameter", WebGLRenderingContextBase::GetRenderbufferParameter),
        NVal::DeclareNapiFunction("getShaderParameter", WebGLRenderingContextBase::GetShaderParameter),
        NVal::DeclareNapiFunction("getTexParameter", WebGLRenderingContextBase::GetTexParameter),
        NVal::DeclareNapiFunction("isFramebuffer", WebGLRenderingContextBase::IsFramebuffer),
        NVal::DeclareNapiFunction("isProgram", WebGLRenderingContextBase::IsProgram),
        NVal::DeclareNapiFunction("isRenderbuffer", WebGLRenderingContextBase::IsRenderbuffer),
        NVal::DeclareNapiFunction("isShader", WebGLRenderingContextBase::IsShader),
        NVal::DeclareNapiFunction("isTexture", WebGLRenderingContextBase::IsTexture),
        NVal::DeclareNapiFunction("lineWidth", WebGLRenderingContextBase::LineWidth),
        NVal::DeclareNapiFunction("linkProgram", WebGLRenderingContextBase::LinkProgram),
        NVal::DeclareNapiFunction("pixelStorei", WebGLRenderingContextBase::PixelStorei),
        NVal::DeclareNapiFunction("polygonOffset", WebGLRenderingContextBase::PolygonOffset),
        NVal::DeclareNapiFunction("getParameter", WebGLRenderingContextBase::GetParameter),
        NVal::DeclareNapiFunction("getAttachedShaders", WebGLRenderingContextBase::GetAttachedShaders),
        NVal::DeclareNapiFunction("getShaderPrecisionFormat", WebGLRenderingContextBase::GetShaderPrecisionFormat),
        NVal::DeclareNapiFunction("getShaderInfoLog", WebGLRenderingContextBase::GetShaderInfoLog),
        NVal::DeclareNapiFunction("getProgramInfoLog", WebGLRenderingContextBase::GetProgramInfoLog),
        NVal::DeclareNapiFunction("getShaderSource", WebGLRenderingContextBase::GetShaderSource),
        NVal::DeclareNapiFunction("getUniform", WebGLRenderingContextBase::GetUniform),
        NVal::DeclareNapiFunction("bufferData", WebGLRenderingContextOverloads::BufferData),
        NVal::DeclareNapiFunction("bufferSubData", WebGLRenderingContextOverloads::BufferSubData),
        NVal::DeclareNapiFunction("compressedTexImage2D", WebGLRenderingContextOverloads::CompressedTexImage2D),
        NVal::DeclareNapiFunction("compressedTexSubImage2D", WebGLRenderingContextOverloads::CompressedTexSubImage2D),
        NVal::DeclareNapiFunction("readPixels", WebGLRenderingContextOverloads::ReadPixels),
        NVal::DeclareNapiFunction("texImage2D", WebGLRenderingContextOverloads::TexImage2D),
        NVal::DeclareNapiFunction("texSubImage2D", WebGLRenderingContextOverloads::TexSubImage2D),
        NVal::DeclareNapiFunction("vertexAttrib1fv", WebGLRenderingContextBase::VertexAttrib1fv),
        NVal::DeclareNapiFunction("vertexAttrib2fv", WebGLRenderingContextBase::VertexAttrib2fv),
        NVal::DeclareNapiFunction("vertexAttrib3fv", WebGLRenderingContextBase::VertexAttrib3fv),
        NVal::DeclareNapiFunction("vertexAttrib4fv", WebGLRenderingContextBase::VertexAttrib4fv),
        NVal::DeclareNapiFunction("getVertexAttrib", WebGLRenderingContextBase::GetVertexAttrib),
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

        NVal::DeclareNapiGetter("DEPTH_BUFFER_BIT", GetDepthBufferBit),
        NVal::DeclareNapiGetter("STENCIL_BUFFER_BIT", GetStencilBufferBit),
        NVal::DeclareNapiGetter("COLOR_BUFFER_BIT", GetColorBufferBit),
        NVal::DeclareNapiGetter("POINTS", GetPoints),
        NVal::DeclareNapiGetter("LINES", GetLines),
        NVal::DeclareNapiGetter("LINE_LOOP", GetLineLoop),
        NVal::DeclareNapiGetter("LINE_STRIP", GetLineStrip),
        NVal::DeclareNapiGetter("TRIANGLES", GetTriangles),
        NVal::DeclareNapiGetter("TRIANGLE_STRIP", GetTrianglesStrip),
        NVal::DeclareNapiGetter("TRIANGLE_FAN", GetTrianglesStripFan),
        NVal::DeclareNapiGetter("ZERO", GetZero),
        NVal::DeclareNapiGetter("ONE", GetOne),
        NVal::DeclareNapiGetter("SRC_COLOR", GetSrcColor),
        NVal::DeclareNapiGetter("ONE_MINUS_SRC_COLOR", GetOneMinusSrcColor),
        NVal::DeclareNapiGetter("SRC_ALPHA", GetSrcAlpha),
        NVal::DeclareNapiGetter("ONE_MINUS_SRC_ALPHA", GetOneMinusSrcAlpha),
        NVal::DeclareNapiGetter("DST_ALPHA", GetDstAlpha),
        NVal::DeclareNapiGetter("ONE_MINUS_DST_ALPHA", GetOneMinusDstAlpha),
        NVal::DeclareNapiGetter("DST_COLOR", GetDstColor),
        NVal::DeclareNapiGetter("ONE_MINUS_DST_COLOR", GetOneMinusDstColor),
        NVal::DeclareNapiGetter("SRC_ALPHA_SATURATE", GetSrcAlphaSaturate),
        NVal::DeclareNapiGetter("FUNC_ADD", GetFuncAdd),
        NVal::DeclareNapiGetter("BLEND_EQUATION", GetBlendEquation),
        NVal::DeclareNapiGetter("BLEND_EQUATION_RGB", GetBlendEquationRgb),
        NVal::DeclareNapiGetter("BLEND_EQUATION_ALPHA", GetBlendEquationAlpha),
        NVal::DeclareNapiGetter("FUNC_SUBTRACT", GetFuncSubtract),
        NVal::DeclareNapiGetter("FUNC_REVERSE_SUBTRACT", GetFuncReverseSubtract),
        NVal::DeclareNapiGetter("BLEND_DST_RGB", GetBlendDstRgb),
        NVal::DeclareNapiGetter("BLEND_SRC_RGB", GetBlendSrcRgb),
        NVal::DeclareNapiGetter("BLEND_DST_ALPHA", GetBlendDstAlpha),
        NVal::DeclareNapiGetter("BLEND_SRC_ALPHA", GetBlendSrcAlpha),
        NVal::DeclareNapiGetter("CONSTANT_COLOR", GetConstantColor),
        NVal::DeclareNapiGetter("ONE_MINUS_CONSTANT_COLOR", GetOneMinusConstantColor),
        NVal::DeclareNapiGetter("CONSTANT_ALPHA", GetConstantAlpha),
        NVal::DeclareNapiGetter("ONE_MINUS_CONSTANT_ALPHA", GetOneMinusCostantAlpha),
        NVal::DeclareNapiGetter("BLEND_COLOR", GetBlendColor),
        NVal::DeclareNapiGetter("ARRAY_BUFFER", GetArrayBuffer),
        NVal::DeclareNapiGetter("ELEMENT_ARRAY_BUFFER", GetElementArrayBuffer),
        NVal::DeclareNapiGetter("ARRAY_BUFFER_BINDING", GetArrayBufferBinding),
        NVal::DeclareNapiGetter("ELEMENT_ARRAY_BUFFER_BINDING", GetElementArrayBufferBinding),
        NVal::DeclareNapiGetter("STREAM_DRAW", GetStreamDraw),
        NVal::DeclareNapiGetter("STATIC_DRAW", GetStaticDraw),
        NVal::DeclareNapiGetter("DYNAMIC_DRAW", GetDynamicDraw),
        NVal::DeclareNapiGetter("BUFFER_SIZE", GetBufferSize),
        NVal::DeclareNapiGetter("BUFFER_USAGE", GetBufferUsage),
        NVal::DeclareNapiGetter("CURRENT_VERTEX_ATTRIB", GetCurrentVertexAttrib),
        NVal::DeclareNapiGetter("FRONT", GetFront),
        NVal::DeclareNapiGetter("BACK", GetBack),
        NVal::DeclareNapiGetter("FRONT_AND_BACK", GetFrontAndBack),
        NVal::DeclareNapiGetter("CULL_FACE", GetCullFace),
        NVal::DeclareNapiGetter("BLEND", GetBlend),
        NVal::DeclareNapiGetter("DITHER", GetDither),
        NVal::DeclareNapiGetter("STENCIL_TEST", GetStencilTest),
        NVal::DeclareNapiGetter("DEPTH_TEST", GetDepthTest),
        NVal::DeclareNapiGetter("SCISSOR_TEST", GetScissorTest),
        NVal::DeclareNapiGetter("POLYGON_OFFSET_FILL", GetPolygonOffsetFill),
        NVal::DeclareNapiGetter("SAMPLE_ALPHA_TO_COVERAGE", GetSampleAlphaToCoverage),
        NVal::DeclareNapiGetter("SAMPLE_COVERAGE", GetSampleCoverage),
        NVal::DeclareNapiGetter("NO_ERROR", GetNoError),
        NVal::DeclareNapiGetter("INVALID_ENUM", GetInvalidEnum),
        NVal::DeclareNapiGetter("INVALID_VALUE", GetInvalidValue),
        NVal::DeclareNapiGetter("INVALID_OPERATION", GetInvalidOperation),
        NVal::DeclareNapiGetter("OUT_OF_MEMORY", GetOutOfMemory),
        NVal::DeclareNapiGetter("CW", GetCw),
        NVal::DeclareNapiGetter("CCW", GetCcw),
        NVal::DeclareNapiGetter("LINE_WIDTH", GetLineWidth),
        NVal::DeclareNapiGetter("ALIASED_POINT_SIZE_RANGE", GetAliasedPointSizeRange),
        NVal::DeclareNapiGetter("ALIASED_LINE_WIDTH_RANGE", GetAliasedLineWidthRange),
        NVal::DeclareNapiGetter("CULL_FACE_MODE", GetCullFaceMode),
        NVal::DeclareNapiGetter("FRONT_FACE", GetFrontFace),
        NVal::DeclareNapiGetter("DEPTH_RANGE", GetDepthRange),
        NVal::DeclareNapiGetter("DEPTH_WRITEMASK", GetDepthWritemask),
        NVal::DeclareNapiGetter("DEPTH_CLEAR_VALUE", GetDepthClearValue),
        NVal::DeclareNapiGetter("DEPTH_FUNC", GetDepthFunc),
        NVal::DeclareNapiGetter("STENCIL_CLEAR_VALUE", GetStencilClearValue),
        NVal::DeclareNapiGetter("STENCIL_FUNC", GetStencilFunc),
        NVal::DeclareNapiGetter("STENCIL_FAIL", GetStencilFail),
        NVal::DeclareNapiGetter("STENCIL_PASS_DEPTH_FAIL", GetStencilPassDepthFail),
        NVal::DeclareNapiGetter("STENCIL_PASS_DEPTH_PASS", GetStencilPassDepthPass),
        NVal::DeclareNapiGetter("STENCIL_REF", GetStencilRef),
        NVal::DeclareNapiGetter("STENCIL_VALUE_MASK", GetStencilValueMask),
        NVal::DeclareNapiGetter("STENCIL_WRITEMASK", GetStencilWriteMask),
        NVal::DeclareNapiGetter("STENCIL_BACK_FUNC", GetStencilBackFunc),
        NVal::DeclareNapiGetter("STENCIL_BACK_FAIL", GetStencilBackFail),
        NVal::DeclareNapiGetter("STENCIL_BACK_PASS_DEPTH_FAIL", GetStencilBackPassDepthFail),
        NVal::DeclareNapiGetter("STENCIL_BACK_PASS_DEPTH_PASS", GetStencilBackPassDepthPass),
        NVal::DeclareNapiGetter("STENCIL_BACK_REF", GetStencilBackRef),
        NVal::DeclareNapiGetter("STENCIL_BACK_VALUE_MASK", GetStencilBackValueMask),
        NVal::DeclareNapiGetter("STENCIL_BACK_WRITEMASK", GetStencilBackWriteMask),
        NVal::DeclareNapiGetter("VIEWPORT", GetViewport),
        NVal::DeclareNapiGetter("SCISSOR_BOX", GetScissorBox),
        NVal::DeclareNapiGetter("COLOR_CLEAR_VALUE", GetColorClearValue),
        NVal::DeclareNapiGetter("COLOR_WRITEMASK", GetColorWriteMask),
        NVal::DeclareNapiGetter("UNPACK_ALIGNMENT", GetUnpackAlignment),
        NVal::DeclareNapiGetter("PACK_ALIGNMENT", GetPackAlignment),
        NVal::DeclareNapiGetter("MAX_TEXTURE_SIZE", GetMaxTextureSize),
        NVal::DeclareNapiGetter("MAX_VIEWPORT_DIMS", GetMaxViewportDims),
        NVal::DeclareNapiGetter("SUBPIXEL_BITS", GetSubpixelBits),
        NVal::DeclareNapiGetter("RED_BITS", GetRedBits),
        NVal::DeclareNapiGetter("GREEN_BITS", GetGreenBits),
        NVal::DeclareNapiGetter("BLUE_BITS", GetBuleBits),
        NVal::DeclareNapiGetter("ALPHA_BITS", GetAlphaBits),
        NVal::DeclareNapiGetter("DEPTH_BITS", GetDepthBits),
        NVal::DeclareNapiGetter("STENCIL_BITS", GetStencilBits),
        NVal::DeclareNapiGetter("POLYGON_OFFSET_UNITS", GetPolygonOffsetUnits),
        NVal::DeclareNapiGetter("POLYGON_OFFSET_FACTOR", GetPolyonOffsetFactor),
        NVal::DeclareNapiGetter("TEXTURE_BINDING_2D", GetTextureBinding2D),
        NVal::DeclareNapiGetter("SAMPLE_BUFFERS", GetSampleBuffers),
        NVal::DeclareNapiGetter("SAMPLES", GetSample),
        NVal::DeclareNapiGetter("SAMPLE_COVERAGE_VALUE", GetSampleCoverageValue),
        NVal::DeclareNapiGetter("SAMPLE_COVERAGE_INVERT", GetSampleCoverageInvert),
        NVal::DeclareNapiGetter("COMPRESSED_TEXTURE_FORMATS", GetCompressedTextureFormats),
        NVal::DeclareNapiGetter("DONT_CARE", GetDoneCare),
        NVal::DeclareNapiGetter("FASTEST", GetFastest),
        NVal::DeclareNapiGetter("NICEST", GetIncest),
        NVal::DeclareNapiGetter("GENERATE_MIPMAP_HINT", GetGenerateMipMapHint),
        NVal::DeclareNapiGetter("BYTE", GetByte),
        NVal::DeclareNapiGetter("UNSIGNED_BYTE", GetUnsignedByte),
        NVal::DeclareNapiGetter("SHORT", GetShort),
        NVal::DeclareNapiGetter("UNSIGNED_SHORT", GetUnsignedShort),
        NVal::DeclareNapiGetter("INT", GetInt),
        NVal::DeclareNapiGetter("UNSIGNED_INT", GetUnsignedInt),
        NVal::DeclareNapiGetter("FLOAT", GetFloat),
        NVal::DeclareNapiGetter("DEPTH_COMPONENT", GetDepthComponent),
        NVal::DeclareNapiGetter("ALPHA", GetAlpha),
        NVal::DeclareNapiGetter("RGB", GetRgb),
        NVal::DeclareNapiGetter("RGBA", GetRgba),
        NVal::DeclareNapiGetter("LUMINANCE", GetLuminance),
        NVal::DeclareNapiGetter("LUMINANCE_ALPHA", GetLuminanceAlpha),
        NVal::DeclareNapiGetter("UNSIGNED_SHORT_4_4_4_4", GetUnsignedShort4444),
        NVal::DeclareNapiGetter("UNSIGNED_SHORT_5_5_5_1", GetUnsignedShort5551),
        NVal::DeclareNapiGetter("UNSIGNED_SHORT_5_6_5", GetUnsignedShort565),
        NVal::DeclareNapiGetter("FRAGMENT_SHADER", GetFragmentShader),
        NVal::DeclareNapiGetter("VERTEX_SHADER", GetVertexShader),
        NVal::DeclareNapiGetter("MAX_VERTEX_ATTRIBS", GetMaxVertexAttribs),
        NVal::DeclareNapiGetter("MAX_VERTEX_UNIFORM_VECTORS", GetMaxVertexUniformVectors),
        NVal::DeclareNapiGetter("MAX_VARYING_VECTORS", GetMaxVaryingVectors),
        NVal::DeclareNapiGetter("MAX_COMBINED_TEXTURE_IMAGE_UNITS", GetMaxCombinedTextureImageUnits),
        NVal::DeclareNapiGetter("MAX_VERTEX_TEXTURE_IMAGE_UNITS", GetMaxVertexTextureImageUnits),
        NVal::DeclareNapiGetter("MAX_TEXTURE_IMAGE_UNITS", GetMaxTextureImageUnits),
        NVal::DeclareNapiGetter("MAX_FRAGMENT_UNIFORM_VECTORS", GeMaxFragmentUniformVectors),
        NVal::DeclareNapiGetter("SHADER_TYPE", GetShaderType),
        NVal::DeclareNapiGetter("DELETE_STATUS", GetDeleteStatus),
        NVal::DeclareNapiGetter("LINK_STATUS", GetLinkStatus),
        NVal::DeclareNapiGetter("VALIDATE_STATUS", GetValidateStatus),
        NVal::DeclareNapiGetter("ATTACHED_SHADERS", GetAttachedShader),
        NVal::DeclareNapiGetter("ACTIVE_UNIFORMS", GetActiveUniforms),
        NVal::DeclareNapiGetter("ACTIVE_ATTRIBUTES", GetActiveAttributes),
        NVal::DeclareNapiGetter("SHADING_LANGUAGE_VERSION", GetShadingLanguageVersion),
        NVal::DeclareNapiGetter("CURRENT_PROGRAM", GetCurrentProgram),
        NVal::DeclareNapiGetter("NEVER", GetNever),
        NVal::DeclareNapiGetter("LESS", GetLess),
        NVal::DeclareNapiGetter("EQUAL", GetEqual),
        NVal::DeclareNapiGetter("LEQUAL", GetLequal),
        NVal::DeclareNapiGetter("GREATER", GetGreater),
        NVal::DeclareNapiGetter("NOTEQUAL", GetNotequal),
        NVal::DeclareNapiGetter("GEQUAL", GetGequal),
        NVal::DeclareNapiGetter("ALWAYS", GetAlways),
        NVal::DeclareNapiGetter("KEEP", GetKeep),
        NVal::DeclareNapiGetter("REPLACE", GetReplace),
        NVal::DeclareNapiGetter("INCR", GetIncr),
        NVal::DeclareNapiGetter("DECR", GetDecr),
        NVal::DeclareNapiGetter("INVERT", GetInvert),
        NVal::DeclareNapiGetter("INCR_WRAP", GetIncrWrap),
        NVal::DeclareNapiGetter("DECR_WRAP", GetDecrWrap),
        NVal::DeclareNapiGetter("VENDOR", GetVendor),
        NVal::DeclareNapiGetter("RENDERER", GetRenderer),
        NVal::DeclareNapiGetter("VERSION", GetVersion),
        NVal::DeclareNapiGetter("NEAREST", GetNearest),
        NVal::DeclareNapiGetter("LINEAR", GetLinear),
        NVal::DeclareNapiGetter("NEAREST_MIPMAP_NEAREST", GetNearestMipmapNearest),
        NVal::DeclareNapiGetter("LINEAR_MIPMAP_NEAREST", GetLinearMipmapNearest),
        NVal::DeclareNapiGetter("NEAREST_MIPMAP_LINEAR", GetNearestMipMapLinear),
        NVal::DeclareNapiGetter("LINEAR_MIPMAP_LINEAR", GetLinearMipmapLinear),
        NVal::DeclareNapiGetter("TEXTURE_MAG_FILTER", GetTextureMagFilter),
        NVal::DeclareNapiGetter("TEXTURE_MIN_FILTER", GetTextureMinFilter),
        NVal::DeclareNapiGetter("TEXTURE_WRAP_S", GetTextureWrapS),
        NVal::DeclareNapiGetter("TEXTURE_WRAP_T", GetTextureWrapT),
        NVal::DeclareNapiGetter("TEXTURE_2D", GetTexture2D),
        NVal::DeclareNapiGetter("TEXTURE", GetTexture),
        NVal::DeclareNapiGetter("TEXTURE_CUBE_MAP", GetTextureCubeMap),
        NVal::DeclareNapiGetter("TEXTURE_BINDING_CUBE_MAP", GetTextureBindingCubeMap),
        NVal::DeclareNapiGetter("TEXTURE_CUBE_MAP_POSITIVE_X", GetTextureCubeMapPositiveX),
        NVal::DeclareNapiGetter("TEXTURE_CUBE_MAP_NEGATIVE_X", GetTextureCubeMapNegativeX),
        NVal::DeclareNapiGetter("TEXTURE_CUBE_MAP_POSITIVE_Y", GetTextureCubeMapPositiveY),
        NVal::DeclareNapiGetter("TEXTURE_CUBE_MAP_NEGATIVE_Y", GetTextureCubeMapNegativeY),
        NVal::DeclareNapiGetter("TEXTURE_CUBE_MAP_POSITIVE_Z", GetTextureCubeMapPositiveZ),
        NVal::DeclareNapiGetter("TEXTURE_CUBE_MAP_NEGATIVE_Z", GetTextureCubeMapNegativeZ),
        NVal::DeclareNapiGetter("MAX_CUBE_MAP_TEXTURE_SIZE", GetMaxCubeMapTextureSize),
        NVal::DeclareNapiGetter("TEXTURE0", GetTexture0),
        NVal::DeclareNapiGetter("TEXTURE1", GetTexture1),
        NVal::DeclareNapiGetter("TEXTURE2", GetTexture2),
        NVal::DeclareNapiGetter("TEXTURE3", GetTexture3),
        NVal::DeclareNapiGetter("TEXTURE4", GetTexture4),
        NVal::DeclareNapiGetter("TEXTURE5", GetTexture5),
        NVal::DeclareNapiGetter("TEXTURE6", GetTexture6),
        NVal::DeclareNapiGetter("TEXTURE7", GetTexture7),
        NVal::DeclareNapiGetter("TEXTURE8", GetTexture8),
        NVal::DeclareNapiGetter("TEXTURE9", GetTexture9),
        NVal::DeclareNapiGetter("TEXTURE10", GetTexture10),
        NVal::DeclareNapiGetter("TEXTURE11", GetTexture11),
        NVal::DeclareNapiGetter("TEXTURE12", GetTexture12),
        NVal::DeclareNapiGetter("TEXTURE13", GetTexture13),
        NVal::DeclareNapiGetter("TEXTURE14", GetTexture14),
        NVal::DeclareNapiGetter("TEXTURE15", GetTexture15),
        NVal::DeclareNapiGetter("TEXTURE16", GetTexture16),
        NVal::DeclareNapiGetter("TEXTURE17", GetTexture17),
        NVal::DeclareNapiGetter("TEXTURE18", GetTexture18),
        NVal::DeclareNapiGetter("TEXTURE19", GetTexture19),
        NVal::DeclareNapiGetter("TEXTURE20", GetTexture20),
        NVal::DeclareNapiGetter("TEXTURE21", GetTexture21),
        NVal::DeclareNapiGetter("TEXTURE22", GetTexture22),
        NVal::DeclareNapiGetter("TEXTURE23", GetTexture23),
        NVal::DeclareNapiGetter("TEXTURE24", GetTexture24),
        NVal::DeclareNapiGetter("TEXTURE25", GetTexture25),
        NVal::DeclareNapiGetter("TEXTURE26", GetTexture26),
        NVal::DeclareNapiGetter("TEXTURE27", GetTexture27),
        NVal::DeclareNapiGetter("TEXTURE28", GetTexture28),
        NVal::DeclareNapiGetter("TEXTURE29", GetTexture29),
        NVal::DeclareNapiGetter("TEXTURE30", GetTexture30),
        NVal::DeclareNapiGetter("TEXTURE31", GetTexture31),
        NVal::DeclareNapiGetter("ACTIVE_TEXTURE", GetActiveTexture),
        NVal::DeclareNapiGetter("REPEAT", GetRepeat),
        NVal::DeclareNapiGetter("CLAMP_TO_EDGE", GetClampToEdge),
        NVal::DeclareNapiGetter("MIRRORED_REPEAT", GetMirroredRepeat),
        NVal::DeclareNapiGetter("FLOAT_VEC2", GetFloatVec2),
        NVal::DeclareNapiGetter("FLOAT_VEC3", GetFloatVec3),
        NVal::DeclareNapiGetter("FLOAT_VEC4", GetFloatVec4),
        NVal::DeclareNapiGetter("INT_VEC2", GetIntVec2),
        NVal::DeclareNapiGetter("INT_VEC3", GetIntVec3),
        NVal::DeclareNapiGetter("INT_VEC4", GetIntVec4),
        NVal::DeclareNapiGetter("BOOL", GetBool),
        NVal::DeclareNapiGetter("BOOL_VEC2", GetBoolVec2),
        NVal::DeclareNapiGetter("BOOL_VEC3", GetBoolVec3),
        NVal::DeclareNapiGetter("BOOL_VEC4", GetBoolVec4),
        NVal::DeclareNapiGetter("FLOAT_MAT2", GetFloatMat2),
        NVal::DeclareNapiGetter("FLOAT_MAT3", GetFloatMat3),
        NVal::DeclareNapiGetter("FLOAT_MAT4", GetFloatMat4),
        NVal::DeclareNapiGetter("SAMPLER_2D", GetSample2D),
        NVal::DeclareNapiGetter("SAMPLER_CUBE", GetSamplerCube),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_ENABLED", GetVertexAttribArrayEnabled),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_SIZE", GetVertexAttribArraySize),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_STRIDE", GetVertexAttribArrayStride),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_TYPE", GetVertexAttribArrayType),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_NORMALIZED", GetVertexAttribArrayNormalized),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_POINTER", GetVertexAttribArrayPointer),
        NVal::DeclareNapiGetter("VERTEX_ATTRIB_ARRAY_BUFFER_BINDING", GetVertexAttribArrayBufferBinging),
        NVal::DeclareNapiGetter("IMPLEMENTATION_COLOR_READ_TYPE", GetImplementationColorReadType),
        NVal::DeclareNapiGetter("IMPLEMENTATION_COLOR_READ_FORMAT", GetImplementationColorReadFormat),
        NVal::DeclareNapiGetter("COMPILE_STATUS", GetCompileStatus),
        NVal::DeclareNapiGetter("LOW_FLOAT", GetLowFloat),
        NVal::DeclareNapiGetter("MEDIUM_FLOAT", GetMediumFloat),
        NVal::DeclareNapiGetter("HIGH_FLOAT", GetHighFloat),
        NVal::DeclareNapiGetter("LOW_INT", GetLowInt),
        NVal::DeclareNapiGetter("MEDIUM_INT", GetMediumInt),
        NVal::DeclareNapiGetter("HIGH_INT", GetHighInt),
        NVal::DeclareNapiGetter("FRAMEBUFFER", Getframebuffer),
        NVal::DeclareNapiGetter("RENDERBUFFER", GetRenderbuffer),
        NVal::DeclareNapiGetter("RGBA4", GetRgba4),
        NVal::DeclareNapiGetter("RGB5_A1", GetRgb5A1),
        NVal::DeclareNapiGetter("RGB565", GetRgb565),
        NVal::DeclareNapiGetter("DEPTH_COMPONENT16", GetDepthComponent16),
        NVal::DeclareNapiGetter("STENCIL_INDEX8", GetStencilIndex8),
        NVal::DeclareNapiGetter("DEPTH_STENCIL", GetDepthStencil),
        NVal::DeclareNapiGetter("RENDERBUFFER_WIDTH", GetRenderbufferWidth),
        NVal::DeclareNapiGetter("RENDERBUFFER_HEIGHT", GetRenderbufferHeight),
        NVal::DeclareNapiGetter("RENDERBUFFER_INTERNAL_FORMAT", GetRenderbufferInterNalFormat),
        NVal::DeclareNapiGetter("RENDERBUFFER_RED_SIZE", GetRenderbufferRedSize),
        NVal::DeclareNapiGetter("RENDERBUFFER_GREEN_SIZE", GetRenderbufferGreenSize),
        NVal::DeclareNapiGetter("RENDERBUFFER_BLUE_SIZE", GetRenderbufferBlueSize),
        NVal::DeclareNapiGetter("RENDERBUFFER_ALPHA_SIZE", GetRenderbufferAlphaSize),
        NVal::DeclareNapiGetter("RENDERBUFFER_DEPTH_SIZE", GetRenderbufferDepthSize),
        NVal::DeclareNapiGetter("RENDERBUFFER_STENCIL_SIZE", GetRenderbufferStencilSize),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE", GetFramebufferAttachmentObjectType),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_OBJECT_NAME", GetFramebufferAttachmentObjectName),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL", GetFramebufferAttachmentTextureLevel),
        NVal::DeclareNapiGetter("FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE", GetFramebufferTexureCubeMapFace),
        NVal::DeclareNapiGetter("COLOR_ATTACHMENT0", GetColorAttachment0),
        NVal::DeclareNapiGetter("DEPTH_ATTACHMENT", GetDepthAttachment),
        NVal::DeclareNapiGetter("STENCIL_ATTACHMENT", GetStencilAttachment),
        NVal::DeclareNapiGetter("DEPTH_STENCIL_ATTACHMENT", GetDepthStencilAttachment),
        NVal::DeclareNapiGetter("NONE", GetNone),
        NVal::DeclareNapiGetter("FRAMEBUFFER_COMPLETE", GetFramebufferComplete),
        NVal::DeclareNapiGetter("FRAMEBUFFER_INCOMPLETE_ATTACHMENT", GetFramebufferIncompleteAttachment),
        NVal::DeclareNapiGetter("FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT",
            GetFramebufferIncompleteMissingAttachment),
        NVal::DeclareNapiGetter("FRAMEBUFFER_INCOMPLETE_DIMENSIONS", GetFramebufferIncompleteDimensions),
        NVal::DeclareNapiGetter("FRAMEBUFFER_UNSUPPORTED", GetFramebufferUnsupported),
        NVal::DeclareNapiGetter("FRAMEBUFFER_BINDING", GetFramebufferBinging),
        NVal::DeclareNapiGetter("RENDERBUFFER_BINDING", GetRenderBufferBinging),
        NVal::DeclareNapiGetter("MAX_RENDERBUFFER_SIZE", GetMaxRenderBufferSize),
        NVal::DeclareNapiGetter("INVALID_FRAMEBUFFER_OPERATION", GetInvalidFramebufferOperation),
        NVal::DeclareNapiGetter("UNPACK_FLIP_Y_WEBGL", GetUnpackFileYWebgl),
        NVal::DeclareNapiGetter("UNPACK_PREMULTIPLY_ALPHA_WEBGL", GetUnpackPremultiplyAlphaWebgl),
        NVal::DeclareNapiGetter("CONTEXT_LOST_WEBGL", GetContextLostWebgl),
        NVal::DeclareNapiGetter("UNPACK_COLORSPACE_CONVERSION_WEBGL", GetUnpackColorSpaceConversionWebgl),
        NVal::DeclareNapiGetter("BROWSER_DEFAULT_WEBGL", GetBrowserDefaultWebgl),
        NVal::DeclareNapiProperty("WebGLRenderingContext", instanceValue),
    };

    status = napi_define_properties(env, exports, sizeof(props) / sizeof(props[0]), props);
    if (status != napi_ok) {
        return false;
    }
    return true;
}

string WebGLRenderingContext::GetClassName()
{
    return WebGLRenderingContext::className;
}

WebGLRenderingContext::~WebGLRenderingContext()
{
    LOGI("WebGLRenderingContext::~WebGLRenderingContext");
    auto& webgl1Objects = ObjectManager::GetInstance().GetWebgl1ObjectMap();
    for (auto iter = webgl1Objects.begin(); iter != webgl1Objects.end(); iter++) {
        if (iter->second == this) {
            webgl1Objects.erase(iter);
            break;
        }
    }
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
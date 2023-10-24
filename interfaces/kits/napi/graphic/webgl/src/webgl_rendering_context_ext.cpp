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
#include <algorithm>

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_rendering_context_base.h"
#include "context/webgl_rendering_context_base_impl.h"
#include "napi/n_class.h"
#include "util/log.h"
#include "util/object_source.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
static void AddSupportElement(std::vector<GLenum>& supports, GLenum ele)
{
    if (std::find(supports.begin(), supports.end(), ele) == supports.end()) {
        supports.emplace_back(ele);
    }
}

const std::vector<GLenum>& WebGLRenderingContextBaseImpl::GetIntegerParaName()
{
    static std::vector<GLenum> integerParaNames = { GL_ACTIVE_TEXTURE, GL_ALPHA_BITS, GL_BLEND_DST_ALPHA,
        GL_BLEND_DST_RGB, GL_BLEND_EQUATION_ALPHA, GL_BLEND_EQUATION_RGB, GL_BLEND_SRC_ALPHA, GL_BLEND_SRC_RGB,
        GL_BLUE_BITS, GL_CULL_FACE_MODE, GL_DEPTH_BITS, GL_DEPTH_FUNC, GL_FRONT_FACE, GL_GENERATE_MIPMAP_HINT,
        GL_GREEN_BITS, GL_IMPLEMENTATION_COLOR_READ_FORMAT, GL_IMPLEMENTATION_COLOR_READ_TYPE,
        GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, GL_MAX_CUBE_MAP_TEXTURE_SIZE, GL_MAX_FRAGMENT_UNIFORM_VECTORS,
        GL_MAX_RENDERBUFFER_SIZE, GL_MAX_TEXTURE_IMAGE_UNITS, GL_MAX_TEXTURE_SIZE, GL_MAX_VARYING_VECTORS,
        GL_MAX_VERTEX_ATTRIBS, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, GL_MAX_VERTEX_UNIFORM_VECTORS, GL_PACK_ALIGNMENT,
        GL_RED_BITS, GL_SAMPLE_BUFFERS, GL_SAMPLES, GL_STENCIL_BACK_FAIL, GL_STENCIL_BACK_FUNC,
        GL_STENCIL_BACK_PASS_DEPTH_FAIL, GL_STENCIL_BACK_PASS_DEPTH_PASS, GL_STENCIL_BACK_REF,
        GL_STENCIL_BACK_VALUE_MASK, GL_STENCIL_BACK_WRITEMASK, GL_STENCIL_BITS, GL_STENCIL_CLEAR_VALUE, GL_STENCIL_FAIL,
        GL_STENCIL_FUNC, GL_STENCIL_PASS_DEPTH_FAIL, GL_STENCIL_PASS_DEPTH_PASS, GL_STENCIL_REF, GL_STENCIL_VALUE_MASK,
        GL_STENCIL_WRITEMASK, GL_SUBPIXEL_BITS, GL_UNPACK_ALIGNMENT };
    return integerParaNames;
}

const std::vector<GLenum>& WebGLRenderingContextBaseImpl::GetBoolParaName()
{
    static std::vector<GLenum> boolParaName = { GL_BLEND, GL_CULL_FACE, GL_DEPTH_TEST, GL_DEPTH_WRITEMASK, GL_DITHER,
        GL_POLYGON_OFFSET_FILL, GL_SAMPLE_ALPHA_TO_COVERAGE, GL_SAMPLE_COVERAGE, GL_SAMPLE_COVERAGE_INVERT,
        GL_SCISSOR_TEST, GL_STENCIL_TEST };
    return boolParaName;
}

const std::vector<GLenum>& WebGLRenderingContextBaseImpl::GetExtentionAstcTexImageInternal()
{
    static std::vector<GLenum> extentionAstcSupportInternalFormats {};
    static bool initd = false;
    if (!initd) {
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_4x4_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_5x4_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_5x5_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_6x5_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_6x6_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_8x5_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_8x6_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_8x8_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_10x5_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_10x6_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_10x10_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_12x10_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_RGBA_ASTC_12x12_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR);
        AddSupportElement(extentionAstcSupportInternalFormats, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR);
        initd = true;
    };
    return extentionAstcSupportInternalFormats;
}
} // namespace Impl
} // namespace Rosen
} // namespace OHOS
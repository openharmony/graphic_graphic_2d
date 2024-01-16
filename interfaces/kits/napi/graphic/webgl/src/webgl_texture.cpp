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

#include "webgl/webgl_texture.h"

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_rendering_context_base.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/util.h"
#include "webgl/webgl_arg.h"

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGLTexture::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }

    unique_ptr<WebGLTexture> webGlTexture = make_unique<WebGLTexture>();
    if (!NClass::SetEntityFor<WebGLTexture>(env, funcArg.GetThisVar(), move(webGlTexture))) {
        return NVal::CreateNull(env).val_;
    }
    return funcArg.GetThisVar();
}

bool WebGLTexture::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLTexture::Constructor, std::move(props));
    if (!succ) {
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLTexture::GetClassName()
{
    return WebGLTexture::className;
}

int64_t WebGLTexture::ComputeTextureLevel(int64_t width, int64_t height, int64_t depth)
{
    int64_t max = std::max(std::max(width, height), depth);
    if (max <= 0) {
        return 0;
    }
    double result = log2(max);
    return static_cast<int64_t>(ceil(result));
}

GLint WebGLTexture::GetMaxTextureLevelForTarget(GLenum target, bool highWebGL)
{
    static GLint maxTextureLevel = 0;
    static GLint maxCubeMapTextureLevel = 0;
    static GLint max3DTextureLevel = 0;
    GLint max = 0;
    switch (target) {
        case GL_TEXTURE_2D_ARRAY: // webgl 2
            if (!highWebGL) {
                break;
            }
            [[fallthrough]];
        case GL_TEXTURE_2D: {
            if (maxTextureLevel != 0) {
                return maxTextureLevel;
            }
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
            maxTextureLevel = ComputeTextureLevel(max, max, 1);
            return maxTextureLevel;
        }
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: {
            if (maxCubeMapTextureLevel != 0) {
                return maxCubeMapTextureLevel;
            }
            glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &max);
            maxCubeMapTextureLevel = ComputeTextureLevel(max, max, 1);
            return maxCubeMapTextureLevel;
        }
        case GL_TEXTURE_3D: {
            if (!highWebGL) {
                break;
            }
            if (max3DTextureLevel != 0) {
                return max3DTextureLevel;
            }
            glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max);
            max3DTextureLevel = ComputeTextureLevel(max, max, 1);
            return max3DTextureLevel;
        }
        default:
            break;
    }
    return 0;
}

WebGLTexture::~WebGLTexture()
{
    textureLevelInfos_.clear();
}

bool WebGLTexture::SetTarget(GLenum target)
{
    if (target_) {
        return true;
    }
    target_ = target;
    int32_t maxTextureLevelCtrl = 1;
    switch (target) {
        case GL_TEXTURE_2D:
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_3D: {
            target_ = target;
            maxTextureLevelCtrl = 1;
            break;
        }
        case GL_TEXTURE_CUBE_MAP: {
            target_ = target;
            maxTextureLevelCtrl = 6; // 6 for GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
            break;
        }
        default:
            return false;
    }
    textureLevelInfos_.resize(maxTextureLevelCtrl);
    for (int32_t i = 0; i < maxTextureLevelCtrl; ++i) {
        textureLevelInfos_[i].Init(target);
    }
    return true;
}

const TextureLevelInfo* WebGLTexture::GetTextureLevel(GLenum target, GLint level) const
{
    int32_t index = -1;
    switch (target_) {
        case GL_TEXTURE_CUBE_MAP:
            index = static_cast<int32_t>(target - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
            break;
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_2D:
        case GL_TEXTURE_3D:
            index = static_cast<int32_t>(target - target_);
            break;
        default:
            break;
    }
    if (index < 0 || index >= static_cast<int32_t>(textureLevelInfos_.size())) {
        return nullptr;
    }
    return textureLevelInfos_[index].GetTextureLevel(level);
}

bool WebGLTexture::SetTextureLevel(const TexImageArg& arg)
{
    TextureLevelInfo* levelInfo = const_cast<TextureLevelInfo*>(GetTextureLevel(arg.target, arg.level));
    if (levelInfo) {
        levelInfo->valid = true;
        levelInfo->internalFormat = arg.internalFormat;
        levelInfo->width = arg.width;
        levelInfo->height = arg.height;
        levelInfo->depth = arg.depth;
        levelInfo->type = arg.type;
        levelInfo->Dump("SetTextureLevel", arg.target, arg.level);
    }
    return true;
}

bool WebGLTexture::SetTexStorageInfo(const TexStorageArg* arg)
{
    GLenum type = GetTypeFromInternalFormat(arg->internalFormat);
    if (type == GL_NONE) {
        return false;
    }

    for (size_t i = 0; i < textureLevelInfos_.size(); ++i) {
        GLsizei levelWidth = arg->width;
        GLsizei levelHeight = arg->height;
        GLsizei levelDepth = arg->depth;
        for (GLint level = 0; level < arg->levels; ++level) {
            TextureLevelInfo* levelInfo = const_cast<TextureLevelInfo*>(GetTextureLevel(arg->target, level));
            if (levelInfo != nullptr) {
                levelInfo->internalFormat = arg->internalFormat;
                levelInfo->width = levelWidth;
                levelInfo->height = levelHeight;
                levelInfo->depth = levelDepth;
                levelInfo->type = type;
                levelInfo->valid = true;
                levelInfo->Dump("SetTexStorageInfo", arg->target, level);
            }
            levelWidth = std::max(1, levelWidth / 2); // 2 is half the width
            levelHeight = std::max(1, levelHeight / 2); // 2 is half the height
            levelDepth = std::max(1, levelDepth / 2); // 2 is half the depth
        }
    }
    immutable_ = true;
    return true;
}

GLenum WebGLTexture::GetTypeFromInternalFormat(GLenum internalFormat)
{
    switch (internalFormat) {
        case GL_RGBA4:
            return GL_UNSIGNED_SHORT_4_4_4_4;
        case GL_RGB565:
            return GL_UNSIGNED_SHORT_5_6_5;
        case GL_RGB5_A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;
        case GL_DEPTH_COMPONENT16:
            return GL_UNSIGNED_SHORT;
        case GL_R8:
            return GL_UNSIGNED_BYTE;
        case GL_R8_SNORM:
            return GL_BYTE;
        case GL_R16F:
            return GL_HALF_FLOAT;
        case GL_R32F:
            return GL_FLOAT;
        case GL_R8UI:
            return GL_UNSIGNED_BYTE;
        case GL_R8I:
            return GL_BYTE;
        case GL_R16UI:
            return GL_UNSIGNED_SHORT;
        case GL_R16I:
            return GL_SHORT;
        case GL_R32UI:
            return GL_UNSIGNED_INT;
        case GL_R32I:
            return GL_INT;
        case GL_RG8:
            return GL_UNSIGNED_BYTE;
        case GL_RG8_SNORM:
            return GL_BYTE;
        case GL_RG16F:
            return GL_HALF_FLOAT;
        case GL_RG32F:
            return GL_FLOAT;
        case GL_RG8UI:
            return GL_UNSIGNED_BYTE;
        case GL_RG8I:
            return GL_BYTE;
        case GL_RG16UI:
            return GL_UNSIGNED_SHORT;
        case GL_RG16I:
            return GL_SHORT;
        case GL_RG32UI:
            return GL_UNSIGNED_INT;
        case GL_RG32I:
            return GL_INT;
        case GL_RGB8:
            return GL_UNSIGNED_BYTE;
        case GL_SRGB8:
            return GL_UNSIGNED_BYTE;
        case GL_RGB8_SNORM:
            return GL_BYTE;
        case GL_R11F_G11F_B10F:
            return GL_UNSIGNED_INT_10F_11F_11F_REV;
        case GL_RGB9_E5:
            return GL_UNSIGNED_INT_5_9_9_9_REV;
        case GL_RGB16F:
            return GL_HALF_FLOAT;
        case GL_RGB32F:
            return GL_FLOAT;
        case GL_RGB8UI:
            return GL_UNSIGNED_BYTE;
        case GL_RGB8I:
            return GL_BYTE;
        case GL_RGB16UI:
            return GL_UNSIGNED_SHORT;
        case GL_RGB16I:
            return GL_SHORT;
        case GL_RGB32UI:
            return GL_UNSIGNED_INT;
        case GL_RGB32I:
            return GL_INT;
        case GL_RGBA8:
            return GL_UNSIGNED_BYTE;
        case GL_SRGB8_ALPHA8:
            return GL_UNSIGNED_BYTE;
        case GL_RGBA8_SNORM:
            return GL_BYTE;
        case GL_RGB10_A2:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case GL_RGBA16F:
            return GL_HALF_FLOAT;
        case GL_RGBA32F:
            return GL_FLOAT;
        case GL_RGBA8UI:
            return GL_UNSIGNED_BYTE;
        case GL_RGBA8I:
            return GL_BYTE;
        case GL_RGB10_A2UI:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case GL_RGBA16UI:
            return GL_UNSIGNED_SHORT;
        case GL_RGBA16I:
            return GL_SHORT;
        case GL_RGBA32I:
            return GL_INT;
        case GL_RGBA32UI:
            return GL_UNSIGNED_INT;
        case GL_DEPTH_COMPONENT24:
            return GL_UNSIGNED_INT;
        case GL_DEPTH_COMPONENT32F:
            return GL_FLOAT;
        case GL_DEPTH24_STENCIL8:
            return GL_UNSIGNED_INT_24_8;
        case GL_DEPTH32F_STENCIL8:
            return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        // Compressed types.
        case GL_ATC_RGB_AMD:
        case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return GL_UNSIGNED_BYTE;
        default:
            return GL_NONE;
    }
}

GLenum WebGLTexture::GetInternalFormat(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->internalFormat;
    }
    return 0;
}

GLenum WebGLTexture::GetType(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->type;
    }
    return 0;
}

GLsizei WebGLTexture::GetWidth(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->width;
    }
    return 0;
}

GLsizei WebGLTexture::GetHeight(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->height;
    }
    return 0;
}

GLsizei WebGLTexture::GetDepth(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->depth;
    }
    return 0;
}

bool WebGLTexture::CheckValid(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        levelInfo->Dump("CheckValid", target, level);
        return levelInfo->valid;
    }
    return 0;
}

void TextureLevelCtrl::Init(GLenum target)
{
    GLint maxLevel = WebGLTexture::GetMaxTextureLevelForTarget(target, true);
    if (maxLevel > 0) {
        textureInfos_.resize(maxLevel);
    }
}

bool WebGLTexture::CheckTextureSize(GLsizei offset, GLsizei w, GLsizei real)
{
    if (offset < 0 || w < 0) {
        return false;
    }

    if (WebGLArg::CheckOverflow<GLsizei, GLsizei>(offset, w)) {
        return false;
    }
    if ((offset + w) > real) {
        return false;
    }
    return true;
}

const std::vector<GLenum>& WebGLTexture::GetSupportInternalFormatGroup1()
{
    const static std::vector<GLenum> support1 = { WebGL2RenderingContextBase::R8UI, WebGL2RenderingContextBase::R8I,
        WebGL2RenderingContextBase::R16UI, WebGL2RenderingContextBase::R16I, WebGL2RenderingContextBase::R32UI,
        WebGL2RenderingContextBase::R32I, WebGL2RenderingContextBase::RG8UI, WebGL2RenderingContextBase::RG8I,
        WebGL2RenderingContextBase::RG16UI, WebGL2RenderingContextBase::RG16I, WebGL2RenderingContextBase::RG32UI,
        WebGL2RenderingContextBase::RG32I, WebGL2RenderingContextBase::RGBA8UI, WebGL2RenderingContextBase::RGBA8I,
        WebGL2RenderingContextBase::RGB10_A2UI, WebGL2RenderingContextBase::RGBA16UI,
        WebGL2RenderingContextBase::RGBA16I, WebGL2RenderingContextBase::RGBA32I,
        WebGL2RenderingContextBase::RGBA32UI };
    return support1;
}

const std::vector<GLenum>& WebGLTexture::GetSupportInternalFormatGroup2()
{
    const static std::vector<GLenum> support2 = { WebGL2RenderingContextBase::R8, WebGL2RenderingContextBase::RG8,
        WebGL2RenderingContextBase::RGB8, WebGLRenderingContextBase::RGB565, WebGL2RenderingContextBase::RGBA8,
        WebGL2RenderingContextBase::SRGB8_ALPHA8, WebGLRenderingContextBase::RGB5_A1, WebGLRenderingContextBase::RGBA4,
        WebGL2RenderingContextBase::RGB10_A2, WebGLRenderingContextBase::DEPTH_COMPONENT16,
        WebGL2RenderingContextBase::DEPTH_COMPONENT24, WebGL2RenderingContextBase::DEPTH_COMPONENT32F,
        WebGL2RenderingContextBase::DEPTH24_STENCIL8, WebGL2RenderingContextBase::DEPTH32F_STENCIL8,
        WebGLRenderingContextBase::STENCIL_INDEX8 };
    return support2;
}

void TextureLevelInfo::Dump(const std::string& info, GLenum target, GLint level) const
{
    LOGD("%{public}s [%{public}u %{public}d] %{public}u %{public}u [%{public}d %{public}d %{public}d]", info.c_str(),
        target, level, internalFormat, type, width, height, depth);
}

const std::vector<GLenum>& WebGLTexture::GetSupportedInternalFormats()
{
    static std::vector<GLenum> supportedInternalFormats = {
        GL_R8,
        GL_R8_SNORM,
        GL_R16F,
        GL_R32F,
        GL_R8UI,
        GL_R8I,
        GL_R16UI,
        GL_R16I,
        GL_R32UI,
        GL_R32I,
        GL_RG8,
        GL_RG8_SNORM,
        GL_RG16F,
        GL_RG32F,
        GL_RG8UI,
        GL_RG8I,
        GL_RG16UI,
        GL_RG16I,
        GL_RG32UI,
        GL_RG32I,
        GL_RGB8,
        GL_SRGB8,
        GL_RGB565,
        GL_RGB8_SNORM,
        GL_R11F_G11F_B10F,
        GL_RGB9_E5,
        GL_RGB16F,
        GL_RGB32F,
        GL_RGB8UI,
        GL_RGB8I,
        GL_RGB16UI,
        GL_RGB16I,
        GL_RGB32UI,
        GL_RGB32I,
        GL_RGBA8,
        GL_SRGB8_ALPHA8,
        GL_RGBA8_SNORM,
        GL_RGB5_A1,
        GL_RGBA4,
        GL_RGB10_A2,
        GL_RGBA16F,
        GL_RGBA32F,
        GL_RGBA8UI,
        GL_RGBA8I,
        GL_RGB10_A2UI,
        GL_RGBA16UI,
        GL_RGBA16I,
        GL_RGBA32I,
        GL_RGBA32UI,
        GL_DEPTH_COMPONENT16,
        GL_DEPTH_COMPONENT24,
        GL_DEPTH_COMPONENT32F,
        GL_DEPTH24_STENCIL8,
    };
    // for es2
    supportedInternalFormats.emplace_back(GL_RGB);
    supportedInternalFormats.emplace_back(GL_RGBA);
    supportedInternalFormats.emplace_back(GL_LUMINANCE_ALPHA);
    supportedInternalFormats.emplace_back(GL_LUMINANCE);
    supportedInternalFormats.emplace_back(GL_ALPHA);
    return supportedInternalFormats;
}

const std::vector<GLenum>& WebGLTexture::GetSupportedFormats()
{
    static std::vector<GLenum> supportedFormats = { GL_RED, GL_RED_INTEGER, GL_RG, GL_RG_INTEGER, GL_RGB,
        GL_RGB_INTEGER, GL_RGBA, GL_RGBA_INTEGER, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL };
    // for es2
    supportedFormats.emplace_back(GL_LUMINANCE_ALPHA);
    supportedFormats.emplace_back(GL_LUMINANCE);
    supportedFormats.emplace_back(GL_ALPHA);
    return supportedFormats;
}

const std::vector<GLenum>& WebGLTexture::GetSupportedTypes()
{
    static std::vector<GLenum> supportedTypes = {
        GL_BYTE,
        GL_UNSIGNED_SHORT,
        GL_SHORT,
        GL_UNSIGNED_INT,
        GL_INT,
        GL_HALF_FLOAT,
        GL_HALF_FLOAT_OES,
        GL_FLOAT,
        GL_UNSIGNED_INT_2_10_10_10_REV,
        GL_UNSIGNED_INT_10F_11F_11F_REV,
        GL_UNSIGNED_INT_5_9_9_9_REV,
        GL_UNSIGNED_INT_24_8,
    };

    // for es2
    supportedTypes.emplace_back(GL_UNSIGNED_BYTE);
    supportedTypes.emplace_back(GL_UNSIGNED_SHORT_5_6_5);
    supportedTypes.emplace_back(GL_UNSIGNED_SHORT_4_4_4_4);
    supportedTypes.emplace_back(GL_UNSIGNED_SHORT_5_5_5_1);
    return supportedTypes;
}

BufferDataType WebGLTexture::ChangeToBufferDataType(GLenum type)
{
    switch (type) {
        case GL_BYTE:
            return BUFFER_DATA_INT_8;
        case GL_UNSIGNED_BYTE:
            return BUFFER_DATA_UINT_8;
        case GL_SHORT:
            return BUFFER_DATA_INT_16;
        case GL_UNSIGNED_SHORT:
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_5_5_5_1:
            return BUFFER_DATA_UINT_16;
        case GL_INT:
            return BUFFER_DATA_INT_32;
        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
        case GL_UNSIGNED_INT_5_9_9_9_REV:
        case GL_UNSIGNED_INT_24_8:
            return BUFFER_DATA_UINT_32;
        case GL_FLOAT: // OES_texture_float
            return BUFFER_DATA_FLOAT_32;
        case GL_HALF_FLOAT:
        case GL_HALF_FLOAT_OES:
            return BUFFER_DATA_UINT_16;
        default:
            break;
    }
    return BUFFER_DATA_INVALID;
}

const std::set<TextureFormatTypeMap, TextureFormatTypeMapCompare>& WebGLTexture::GetSupportedFormatTypeMaps()
{
    static std::set<TextureFormatTypeMap, TextureFormatTypeMapCompare> supportedFormatTypeMaps = {
        { GL_R8, GL_RED, GL_UNSIGNED_BYTE },
        { GL_R8_SNORM, GL_RED, GL_BYTE },
        { GL_R16F, GL_RED, GL_HALF_FLOAT },
        { GL_R16F, GL_RED, GL_FLOAT },
        { GL_R32F, GL_RED, GL_FLOAT },
        { GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE },
        { GL_R8I, GL_RED_INTEGER, GL_BYTE },
        { GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT },
        { GL_R16I, GL_RED_INTEGER, GL_SHORT },
        { GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT },
        { GL_R32I, GL_RED_INTEGER, GL_INT },
        { GL_RG8, GL_RG, GL_UNSIGNED_BYTE },
        { GL_RG8_SNORM, GL_RG, GL_BYTE },
        { GL_RG16F, GL_RG, GL_HALF_FLOAT },
        { GL_RG16F, GL_RG, GL_FLOAT },
        { GL_RG32F, GL_RG, GL_FLOAT },
        { GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE },
        { GL_RG8I, GL_RG_INTEGER, GL_BYTE },
        { GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT },
        { GL_RG16I, GL_RG_INTEGER, GL_SHORT },
        { GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT },
        { GL_RG32I, GL_RG_INTEGER, GL_INT },
        { GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE },
        { GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE },
        { GL_RGB565, GL_RGB, GL_UNSIGNED_BYTE },
        { GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },
        { GL_RGB8_SNORM, GL_RGB, GL_BYTE },
        { GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV },
        { GL_R11F_G11F_B10F, GL_RGB, GL_HALF_FLOAT },
        { GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT },
        { GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV },
        { GL_RGB9_E5, GL_RGB, GL_HALF_FLOAT },
        { GL_RGB9_E5, GL_RGB, GL_FLOAT },
        { GL_RGB16F, GL_RGB, GL_HALF_FLOAT },
        { GL_RGB16F, GL_RGB, GL_FLOAT },
        { GL_RGB32F, GL_RGB, GL_FLOAT },
        { GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_BYTE },
        { GL_RGB8I, GL_RGB_INTEGER, GL_BYTE },
        { GL_RGB16UI, GL_RGB_INTEGER, GL_UNSIGNED_SHORT },
        { GL_RGB16I, GL_RGB_INTEGER, GL_SHORT },
        { GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT },
        { GL_RGB32I, GL_RGB_INTEGER, GL_INT },
        { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE },
        { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE },
        { GL_RGBA8_SNORM, GL_RGBA, GL_BYTE },
        { GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_BYTE },
        { GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1 },
        { GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV },
        { GL_RGBA4, GL_RGBA, GL_UNSIGNED_BYTE },
        { GL_RGBA4, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },
        { GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV },
        { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT },
        { GL_RGBA16F, GL_RGBA, GL_FLOAT },
        { GL_RGBA32F, GL_RGBA, GL_FLOAT },
        { GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE },
        { GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE },
        { GL_RGB10_A2UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV },
        { GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT },
        { GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT },
        { GL_RGBA32I, GL_RGBA_INTEGER, GL_INT },
        { GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT },
        { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT },
        { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },
        { GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },
        { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT },
        { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 },

        // es2
        { GL_RGB, GL_RGB, GL_UNSIGNED_BYTE },
        { GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },
        { GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
        { GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },
        { GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1 },
        { GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE },
        { GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE },
        { GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE },
    };
    return supportedFormatTypeMaps;
}

} // namespace Rosen
} // namespace OHOS

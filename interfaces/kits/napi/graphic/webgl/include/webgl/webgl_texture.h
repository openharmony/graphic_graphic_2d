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

#ifndef WEBGL_TEXTRUE_H
#define WEBGL_TEXTRUE_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>

#include "napi/n_exporter.h"
#include "webgl_arg.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
struct TexImageArg;
struct TexStorageArg;
class TextureLevelCtrl;
struct TextureLevelInfo;
struct TextureFormatTypeMap;
struct TextureFormatTypeMapCompare;

class WebGLTexture final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLTexture";
    inline static const int objectType = WEBGL_OBJECT_TEXTURE;
    inline static const int DEFAULT_TEXTURE = 0;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLTexture** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLTexture>(env, instance);
    }

    void SetTexture(uint32_t texture)
    {
        texture_ = texture;
    }

    uint32_t GetTexture() const
    {
        return texture_;
    }

    bool SetTarget(GLenum target);
    GLenum GetTarget() const
    {
        return target_;
    }

    explicit WebGLTexture() : texture_(0) {};

    WebGLTexture(napi_env env, napi_value exports) : NExporter(env, exports), texture_(0) {};

    ~WebGLTexture();

    static WebGLTexture* GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLTexture>(env, obj);
    }

    bool SetTextureLevel(const TexImageArg& arg);
    bool SetTexStorageInfo(const TexStorageArg* arg);

    GLenum GetInternalFormat(GLenum target, GLint level) const;
    GLenum GetType(GLenum target, GLint level) const;
    GLsizei GetWidth(GLenum target, GLint level) const;
    GLsizei GetHeight(GLenum target, GLint level) const;
    GLsizei GetDepth(GLenum target, GLint level) const;
    bool CheckImmutable() const
    {
        return immutable_;
    }
    bool CheckValid(GLenum target, GLint level) const;

    static int64_t ComputeTextureLevel(int64_t width, int64_t height, int64_t depth);
    static GLint GetMaxTextureLevelForTarget(GLenum target, bool highWebGL);
    static GLenum GetTypeFromInternalFormat(GLenum internalFormat);
    static bool CheckNPOT(GLsizei width, GLsizei height)
    {
        if (!width || !height) {
            return false;
        }
        if ((static_cast<uint32_t>(width) & static_cast<uint32_t>(width - 1)) ||
            (static_cast<uint32_t>(height) & static_cast<uint32_t>(height - 1))) {
            return true;
        }
        return false;
    }
    static bool CheckTextureSize(GLsizei offset, GLsizei w, GLsizei real);
    static const std::vector<GLenum> &GetSupportInternalFormatGroup1();
    static const std::vector<GLenum> &GetSupportInternalFormatGroup2();
    static const std::vector<GLenum>& GetSupportedInternalFormats();
    static const std::vector<GLenum>& GetSupportedFormats();
    static const std::vector<GLenum>& GetSupportedTypes();
    static const std::set<TextureFormatTypeMap, TextureFormatTypeMapCompare>& GetSupportedFormatTypeMaps();
    static BufferDataType ChangeToBufferDataType(GLenum type);
private:
    const TextureLevelInfo* GetTextureLevel(GLenum target, GLint level) const;
    GLenum target_ { 0 };
    uint32_t texture_;
    bool immutable_ { false };
    std::vector<TextureLevelCtrl> textureLevelInfos_ {};
};

class TextureLevelCtrl {
public:
    explicit TextureLevelCtrl() {}
    ~TextureLevelCtrl()
    {
        textureInfos_.clear();
    }

    void Init(GLenum target);
    const TextureLevelInfo* GetTextureLevel(GLint level) const
    {
        if (textureInfos_.size() <= static_cast<size_t>(level)) {
            return nullptr;
        }
        return &textureInfos_[level];
    }

private:
    std::vector<TextureLevelInfo> textureInfos_ = {};
};

struct TextureLevelInfo {
    bool valid { false };
    GLenum internalFormat { 0 };
    GLsizei width { 0 };
    GLsizei height { 0 };
    GLsizei depth { 0 };
    GLenum type { 0 };
    void Dump(const std::string &info, GLenum target, GLint level) const;
};

struct TextureFormatTypeMap {
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    TextureFormatTypeMap(GLenum arg1, GLenum arg2, GLenum arg3) : internalFormat(arg1), format(arg2), type(arg3) {}
};

struct TextureFormatTypeMapCompare {
    bool operator() (const TextureFormatTypeMap& lhs, const TextureFormatTypeMap& rhs) const
    {
        return (lhs.internalFormat < rhs.internalFormat ||
            ((lhs.internalFormat == rhs.internalFormat) && (lhs.format < rhs.format)) ||
            ((lhs.internalFormat == rhs.internalFormat) && (lhs.format == rhs.format) && (lhs.type < rhs.type)));
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_TEXTRUE_H

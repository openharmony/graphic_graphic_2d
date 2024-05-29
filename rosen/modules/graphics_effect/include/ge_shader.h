/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef GRAPHICS_EFFECT_GE_SHADER_H
#define GRAPHICS_EFFECT_GE_SHADER_H
#include "effect/runtime_effect.h"
#include <unordered_map>
#include <string>
#include <memory>
namespace OHOS {
namespace Rosen {
typedef enum {
    SHADER_AIBAR,
    SHADER_GREY,
    SHADER_BLUR,
    SHADER_BLURAF,
    SHADER_MIX,
    SHADER_SIMPLE,
    SHADER_HORIBLUR,
    SHADER_VERTBLUR,
    SHADER_MASKBLUR
} ShaderIndex;
// Union Shader Feature
typedef enum {
    TYPE_NO_REFERENCE,
    TYPE_REFERENCE_ROW,
    TYPE_REFERENCE_COLUME,
    TYPE_REFERENCE_AROUND
} ShaderType;
typedef enum {
    GE_ERROR_NONE = 0,
    GE_ERROR_NOT_ACCEPTED_CONNECT,
    GE_ERROR_SHADER_STRING_INVALID,
    GE_ERROR_SHADER_COMPILE_FAILED
} GE_Error;

class GEShader {
public:
    GEShader(const std::string& name, ShaderType type,
        const std::string& shaderStr, const Drawing::RuntimeEffectOptions* opt = nullptr);
    GEShader(const std::shared_ptr<GEShader> rth);
    ~GEShader();
    std::string GetName() const;
    ShaderType GetType() const;
    GEShader& String(const std::string& shaderStr);
    GEShader& Type(ShaderType type);
    GEShader& Name(const std::string& name);
    Drawing::RuntimeEffectOptions* GetOptions() const;
    bool Combine(const GEShader& rth);
    std::shared_ptr<Drawing::RuntimeEffect> GetShader() const;
    GE_Error Compile(const Drawing::RuntimeEffectOptions* ops = nullptr);
private:
    std::string GetString() const;
    GE_Error Connect(const GEShader& subShader) ;
    static GE_Error CombineShaderString(std::string& cbStr,
        const std::string& shaderStrFirst, const std::string& shaderStrSecond);

    // Union Shader Feature
    ShaderType type_ = TYPE_NO_REFERENCE;
    std::string name_ = {};
    std::string shaderStr_ = {};
    std::shared_ptr<Drawing::RuntimeEffect> shader_ = nullptr;
    Drawing::RuntimeEffectOptions* opt_ = nullptr;
};

class GEShaderStore {
public:
    static std::shared_ptr<GEShaderStore> GetInstance();
    std::shared_ptr<GEShader> GetShader(ShaderIndex which);
    std::shared_ptr<GEShader> GetShader(const std::vector<ShaderIndex>& which);
private:
    void Initialize();
    std::unordered_map<std::string, std::shared_ptr<GEShader>> shaderMap_ = {};
    std::unordered_map<ShaderIndex, std::shared_ptr<GEShader>> shaderObjMap_ = {};
};

#define REGISTER_SHADER(key, name, type, shader, opt) do { \
        auto sh = std::make_shared<GEShader>((name), (type), (shader), (opt)); \
        shaderObjMap_[(key)] = sh; \
    } while (0)

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_H
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

#include "ge_shader.h"
#include "ge_shader_string.h"
#include "ge_log.h"
#include <regex>
#include <sstream>
#include <random>

namespace OHOS {
namespace Rosen {

static std::shared_ptr<GEShaderStore> g_ShaderStoreInstance = nullptr;
std::shared_ptr<GEShaderStore> GEShaderStore::GetInstance()
{
    if (g_ShaderStoreInstance == nullptr) {
        g_ShaderStoreInstance = std::make_shared<GEShaderStore>();
        g_ShaderStoreInstance->Initialize();
    }
    return g_ShaderStoreInstance;
}

void GEShaderStore::RegisterShader(ShaderIndex key, const std::string& name, ShaderType type, const std::string& shader,
    const Drawing::RuntimeEffectOptions* opt)
{
    auto sh = std::make_shared<GEShader>(name, type, shader, opt);
    shaderObjMap_[key] = sh;
}

void GEShaderStore::Initialize()
{
    LOGD("GEShaderStore::Initialize");
    RegisterShader(SHADER_AIBAR, "AIBAR", TYPE_NO_REFERENCE, g_aibarString, nullptr);
    RegisterShader(SHADER_GREY, "Grey", TYPE_NO_REFERENCE, g_greyGradationString, nullptr);
    RegisterShader(SHADER_BLUR, "blur", TYPE_REFERENCE_AROUND, g_blurString, nullptr);
    Drawing::RuntimeEffectOptions opt;
    opt.useAF = true;
    RegisterShader(SHADER_BLURAF, "blurAf", TYPE_REFERENCE_AROUND, g_blurStringAF, &opt);
    RegisterShader(SHADER_MIX, "mix", TYPE_REFERENCE_AROUND, g_mixString, nullptr);
    RegisterShader(SHADER_HORIBLUR, "horizontal", TYPE_REFERENCE_AROUND, g_hBlurString, nullptr);
    RegisterShader(SHADER_VERTBLUR, "vertical", TYPE_REFERENCE_AROUND, g_vBlurString, nullptr);
    RegisterShader(SHADER_MASKBLUR, "maskblur", TYPE_REFERENCE_AROUND, g_maskBlurString, nullptr);
    RegisterShader(SHADER_SIMPLE, "Simple", TYPE_NO_REFERENCE, g_simpleString, nullptr);
}

static std::string GetKey(const std::vector<ShaderIndex>& which)
{
    std::string key = {};
    for (auto w : which) {
        key += std::to_string(w);
        key += "_";
    }
    return key;
}

std::shared_ptr<GEShader> GEShaderStore::GetShader(ShaderIndex which)
{
    LOGD("GEShaderStore::GetShader %{public}d ", which);
    std::string key = std::to_string(which) + "_";
    if (shaderMap_.count(key)) {
        LOGD("GetShader %{public}d is exist, total %{public}zu", which, shaderMap_.size());
        return shaderMap_[key];
    }

    if (!shaderObjMap_.count(which)) {
        LOGD("GetShader %{public}d is NOT registered", which);
        return nullptr;
    }
    // copy a new obj because if combine failed, we don't need to recover the obj in shaderobjmap_
    std::shared_ptr<GEShader> shaderObj = std::make_shared<GEShader>(shaderObjMap_[which]);

    if (shaderObj->Compile(shaderObj->GetOptions()) != GE_ERROR_NONE) {
        // else if failed, can be combined but can not be compiled, not accepted
        LOGD("GetShader %{public}d, shader %{public}s compile failed", which, shaderObj->GetName().c_str());
        return nullptr;
    }

    shaderMap_[key] = shaderObj;
    return shaderObj;
}

std::shared_ptr<GEShader> GEShaderStore::GetShader(const std::vector<ShaderIndex>& which)
{
    if (which.empty()) {
        LOGD("GetShader but input is empty");
        return nullptr;
    }

    std::string key = GetKey(which);
    LOGD("GEShaderStore::GetShader %{public}s ", key.c_str());
    if (shaderMap_.count(key)) {
        LOGD("GetShader %{public}s is exist, total %{public}zu", key.c_str(), shaderMap_.size());
        return shaderMap_[key];
    }

    for (auto w : which) {
        if (!shaderObjMap_.count(w)) {
            LOGD("GetShader %{public}d is NOT registered", w);
            return nullptr;
        }
    }

    std::shared_ptr<GEShader> shaderObj = std::make_shared<GEShader>(shaderObjMap_[which[0]]);
    size_t curWhich = 1;
    for (; curWhich < which.size(); curWhich++) {
        auto rth = shaderObjMap_[which[curWhich]];
        if (!shaderObj->Combine(*rth)) {
            LOGD("GetShader %{public}s, %{public}s combine %{public}s failed",
                key.c_str(), shaderObj->GetName().c_str(), rth->GetName().c_str());
            return nullptr;
        }
    }

    if (shaderObj->Compile(shaderObj->GetOptions()) != GE_ERROR_NONE) {
        LOGD("GetShader %{public}s, shader %{public}s compile filed", key.c_str(), shaderObj->GetName().c_str());
        return nullptr;
    }

    shaderMap_[key] = shaderObj;
    return shaderObj;
}

GEShader::GEShader(
    const std::string& name, ShaderType type, const std::string& shaderStr, const Drawing::RuntimeEffectOptions* opt)
    : type_(type), name_(name), shaderStr_(shaderStr)
{
    if (opt != nullptr) {
        opt_ = new Drawing::RuntimeEffectOptions;
        opt_->forceNoInline = opt->forceNoInline;
        opt_->useAF = opt->useAF;
    }
}

GEShader::GEShader(const std::shared_ptr<GEShader> rth)
{
    if (rth != nullptr) {
        name_ = rth->GetName();
        type_ = rth->GetType();
        shaderStr_ = rth->GetString();
        auto opt = rth->GetOptions();
        if (opt != nullptr) {
            opt_ = new Drawing::RuntimeEffectOptions;
            opt_->forceNoInline = opt->forceNoInline;
            opt_->useAF = opt->useAF;
        }
    }
}

GEShader::~GEShader()
{
    if (opt_ != nullptr) {
        delete opt_;
        opt_ = nullptr;
    }
}

std::string GEShader::GetName() const
{
    return name_;
}

ShaderType GEShader::GetType() const
{
    return type_;
}

std::string GEShader::GetString() const
{
    return shaderStr_;
}

GEShader& GEShader::String(const std::string& shaderStr)
{
    shaderStr_ = shaderStr;
    return *this;
}

GEShader& GEShader::Type(ShaderType type)
{
    type_ = type;
    return *this;
}

GEShader& GEShader::Name(const std::string& name)
{
    name_ = name;
    return *this;
}

Drawing::RuntimeEffectOptions* GEShader::GetOptions() const
{
    return opt_;
}

// if failed, nothing will be changed
bool GEShader::Combine(const GEShader& rth)
{
    auto ret = Connect(rth);
    if (ret != GE_ERROR_NONE) {
        LOGW("combine shader failed, cause = %{public}d", ret);
        return false;
    }
    return true;
}

std::shared_ptr<Drawing::RuntimeEffect> GEShader::GetShader() const
{
    return shader_;
}

GE_Error GEShader::Compile(const Drawing::RuntimeEffectOptions* ops)
{
    std::shared_ptr<Drawing::RuntimeEffect> shader;
    if (ops != nullptr) {
        shader = Drawing::RuntimeEffect::CreateForShader(shaderStr_, *ops);
    } else {
        shader = Drawing::RuntimeEffect::CreateForShader(shaderStr_);
    }

    if (shader == nullptr) {
        LOGE("GEShader::Compile %{public}s failed", name_.c_str());
        return GE_ERROR_SHADER_COMPILE_FAILED;
    }
    shader_ = shader;
    return GE_ERROR_NONE;
}

GE_Error GEShader::Connect(const GEShader& subShader)
{
    std::string newName = name_ + "_" + subShader.GetName();
    if (subShader.GetType() != TYPE_NO_REFERENCE) {
        return GE_ERROR_NOT_ACCEPTED_CONNECT;
    }

    std::string targetStr = {};
    if (CombineShaderString(targetStr, shaderStr_, subShader.GetString()) != GE_ERROR_NONE) {
        return GE_ERROR_SHADER_STRING_INVALID;
    }

    name_ = newName;
    shaderStr_ = targetStr;

    auto rthOpt = subShader.GetOptions();
    if (rthOpt != nullptr) {
        if (opt_ != nullptr) {
            opt_->useAF |= rthOpt->useAF;
            opt_->forceNoInline |= rthOpt->forceNoInline;
        } else {
            opt_ = new Drawing::RuntimeEffectOptions;
            opt_->forceNoInline = rthOpt->forceNoInline;
            opt_->useAF = rthOpt->useAF;
        }
    }
    return GE_ERROR_NONE;
}

static std::string PrepareFirstString(
    const std::string& sourceStr, const std::string& oldStr, const std::string& newStr)
{
    std::istringstream iss(sourceStr);
    std::ostringstream oss;
    std::string line;
    while (std::getline(iss, line)) {
        size_t pos = 0;
        while ((pos = line.find(oldStr, pos)) != std::string::npos) {
            // find "return ", maybe a sub path
            line.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
        oss << line << '\n';
    }
    return oss.str();
}

static std::string PrepareSecondString(
    const std::string& sourceStr, const std::string& oldStr, const std::string& newStr)
{
    std::istringstream iss(sourceStr);
    std::ostringstream oss;
    std::string line;
    bool first = true;
    // Replace subStr with newStr in the line
    std::string reg(R"(\\.[\s\S]*[a-zA-Z0-9]*\\([a-zA-Z0-9]*\\))");
    std::regex from(oldStr + reg);
    std::string to = newStr;

    while (std::getline(iss, line)) {
        if (first && line.find(oldStr) != std::string::npos) {
            // Skip the first line that contains subStr to remove duplicate imageShader
            first = false;
            continue;
        }
        line = std::regex_replace(line, from, to);
        oss << line << '\n';
    }
    return oss.str();
}

static std::string GenSimpleUUIDString()
{
    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static constexpr int maxNumberRange = 99; // the max random number
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> charDistribution(0, characters.size() - 1);
    std::uniform_int_distribution<> numDistribution(0, maxNumberRange);

    std::string randomString = "";
    randomString += characters[charDistribution(generator)];
    randomString += std::to_string(numDistribution(generator));

    return randomString;
}
/*
the two shaders should follow the rules as below:
1. import image param's name should be imageShader (casesentense)
2. param of main function should be float2 type and the name should be "coord", and only one param
*/
GE_Error GEShader::CombineShaderString(std::string& cbStr, const std::string& shaderA, const std::string& shaderB)
{
    // each valid shader string should be have 5 matches;
    static constexpr int validMatchCount = 5;
    static constexpr int sectionParam = 1; // 1 : the string before main
    static constexpr int sectionMainTitle = 2; // 2 : the string of main(...)
    static constexpr int sectionMainBody = 3; // 3 : the string in main
    static constexpr int sectionMainOthers = 4; // 4: the rest of shader string
    std::string var = GenSimpleUUIDString();
    auto strShaderB = PrepareSecondString(shaderB, "imageShader", var);

    std::regex re(R"(([\s\S]*)(half4\s+main\s*\(\s*float2\s+coord\s*\)\s*\{)([\s\S]*)(\}))");

    std::smatch matchA;
    std::regex_search(shaderA, matchA, re);
    if (matchA.size() < validMatchCount) {
        LOGW("GEShader::CombineShaderString, invalid first shader string");
        return GE_ERROR_SHADER_STRING_INVALID;
    }
    std::smatch matchB;
    std::regex_search(strShaderB, matchB, re);
    if (matchB.size() < validMatchCount) {
        LOGW("GEShader::CombineShaderString, invalid second shader string");
        return GE_ERROR_SHADER_STRING_INVALID;
    }
    // if have multi return , need to jump to next section
    std::string shaderABody = PrepareFirstString(matchA[sectionMainBody].str(), "return ", var +" = ");
    
    std::string resultVar("vec4 " + var + "=vec4(1.0);\n");
    cbStr = matchA[sectionParam].str() + "\n" + matchB[sectionParam].str() + "\n" + resultVar +
        matchA[sectionMainTitle].str() + "\n{" + shaderABody + "}\n{" + matchB[sectionMainBody].str() + "}\n" +
        matchA[sectionMainOthers].str();

    return GE_ERROR_NONE;
}

} // namespace Rosen
} // namespace OHOS
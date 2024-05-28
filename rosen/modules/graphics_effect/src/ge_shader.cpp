#include "ge_shader.h"
#include "ge_shader_string.h"
#include "ge_log.h"
#include <regex>
#include <sstream>
#include <random>
namespace OHOS{
    namespace Rosen{
        
        static std::shared_ptr<GEShaderStore> g_ShaderStoreInstance = nullptr;
        std::shared_ptr<GEShaderStore> GEShaderStore::GetInstance() {
            if (g_ShaderStoreInstance == nullptr) {
                g_ShaderStoreInstance = std::make_shared<GEShaderStore>();
                g_ShaderStoreInstance->Initialize();
            }
            return g_ShaderStoreInstance;
        }
        void GEShaderStore::Initialize() {
            LOGD("GEShaderStore::Initialize");
            REGISTER_SHADER(SHADER_AIBAR, "AIBAR", TYPE_NO_REFERENCE, aibarShader, nullptr);
            REGISTER_SHADER(SHADER_GREY, "Grey", TYPE_NO_REFERENCE, GreyGradationString, nullptr);
            REGISTER_SHADER(SHADER_BLUR, "blur", TYPE_REFERENCE_AROUND, blurString, nullptr);
            Drawing::RuntimeEffectOptions opt;
            opt.useAF = true;
            REGISTER_SHADER(SHADER_BLURAF, "blurAf", TYPE_REFERENCE_AROUND, blurStringAF, &opt);
            REGISTER_SHADER(SHADER_MIX, "mix", TYPE_REFERENCE_AROUND, mixString, nullptr);
            REGISTER_SHADER(SHADER_HORIBLUR, "horizontal", TYPE_REFERENCE_AROUND, HorizontalBlurString, nullptr);
            REGISTER_SHADER(SHADER_VERTBLUR, "vertical", TYPE_REFERENCE_AROUND, VerticalBlurString, nullptr);
            REGISTER_SHADER(SHADER_MASKBLUR, "maskblur", TYPE_REFERENCE_AROUND, maskBlurString, nullptr);
            REGISTER_SHADER(SHADER_SIMPLE, "Simple", TYPE_NO_REFERENCE, simpleShader, nullptr);
        }
        static std::string GetKey(const std::vector<ShaderIndex>& which) {
            std::string key = {};
            for (auto w : which) {
                key += std::to_string(w);
                key += "_";
            }
            return key;
        }
        std::shared_ptr<GEShader> GEShaderStore::GetShader(ShaderIndex which) {
            LOGD("GEShaderStore::GetShader %{public}d ", which);
            std::string key = std::to_string(which) + "_";
            if (shaderMap_.count(key)) {
                // support max 64 shaders union
                LOGD("GEShaderStore::GetShader the shader of %{public}d is already compiled, total %{public}zu", which, shaderMap_.size());
                return shaderMap_[key];
            }

            if (!shaderObjMap_.count(which))
            {
                // this key is not found 
                LOGD("GEShaderStore::GetShader the shader of %{public}d is NOT registered", which);
                return nullptr;
            }
            // copy a new obj because if combine failed, we don't need to recover the obj in shaderobjmap_
            std::shared_ptr<GEShader> shaderObj = std::make_shared<GEShader>(shaderObjMap_[which]);
            
            LOGD("GEShaderStore::GetShader %{public}d, shader %{public}s going to compile", which, shaderObj->GetName().c_str());
            if (shaderObj->Compile(shaderObj->GetOptions()) != GE_ERROR_NONE) {
                // else if failed, can be combined but can not be compiled, not accepted
                LOGD("GEShaderStore::GetShader %{public}d, shader %{public}s compile failed", which, shaderObj->GetName().c_str());
                return nullptr;
            }
            
            shaderMap_[key] = shaderObj;
            return shaderObj;
        }
        std::shared_ptr<GEShader> GEShaderStore::GetShader(const std::vector<ShaderIndex>& which) {
            if (which.empty()) {
                // no input , return empty vector
                LOGD("GEShaderStore::GetShader but input is empty");
                return nullptr;
            }

            std::string key = GetKey(which);
            LOGD("GEShaderStore::GetShader %{public}s ", key.c_str());
            if (shaderMap_.count(key)) {
                // support max 64 shaders union
                LOGD("GEShaderStore::GetShader the shader of %{public}s is already compiled, total %{public}zu", key.c_str(), shaderMap_.size());
                return shaderMap_[key];
            }

            for (auto w : which) {
                if (!shaderObjMap_.count(w))
                {
                    // this key is not found 
                    LOGD("GEShaderStore::GetShader the shader %{public}d is NOT registered", w);
                    return nullptr;
                }
            }

            std::shared_ptr<GEShader> shaderObj = std::make_shared<GEShader>(shaderObjMap_[which[0]]);
            size_t curWhich = 1;
            for (; curWhich < which.size(); curWhich++) {
                auto rth = shaderObjMap_[which[curWhich]];
                if (!shaderObj->Combine(*rth))
                {
                    // can not combine, return the shader compiled and the reset,
                    LOGD("GEShaderStore::GetShader %{public}s, %{public}s combine %{public}s failed", key.c_str(), shaderObj->GetName().c_str(), rth->GetName().c_str());
                    return nullptr;
                }
            }
           LOGD("GEShaderStore::GetShader %{public}s, shader %{public}s going to compile", key.c_str(), shaderObj->GetName().c_str());
            if (shaderObj->Compile(shaderObj->GetOptions()) != GE_ERROR_NONE) {
                LOGD("GEShaderStore::GetShader %{public}s, shader %{public}s compile filed", key.c_str(), shaderObj->GetName().c_str());
                return nullptr;
            }

            shaderMap_[key] = shaderObj;
            return shaderObj;
        }
        
        GEShader::GEShader(const std::string& name, ShaderType type, const std::string& shaderStr, const Drawing::RuntimeEffectOptions* opt) 
        : type_(type), name_(name), shaderStr_(shaderStr) {
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
        GEShader::~GEShader() {
            if (opt_ != nullptr) {
                delete opt_;
                opt_ = nullptr;
            }
        }
        std::string GEShader::GetName() const {
            return name_;
        }
        ShaderType GEShader::GetType() const {
            return type_;
        }
        std::string GEShader::GetString() const {
            return shaderStr_;
        }
        GEShader& GEShader::String(const std::string& shaderStr){
            shaderStr_ = shaderStr;
            return *this;
        }
        GEShader& GEShader::Type(ShaderType type){
            type_ = type;
            return *this;
        }
        GEShader& GEShader::Name(const std::string& name){
            name_ = name;
            return *this;
        }
        Drawing::RuntimeEffectOptions* GEShader::GetOptions() const{
            return opt_;
        }
        // if failed, everything will not be changed
        bool GEShader::Combine(const GEShader& rth){
            auto ret = Connect(rth);
            if (ret != GE_ERROR_NONE) {
                LOGW("combine shader failed, cause = %{public}d", ret);
                return false;
            }
            return true;
        }
        std::shared_ptr<Drawing::RuntimeEffect> GEShader::GetShader() const{
            return shader_;
        }
        GE_Error GEShader::Compile(const Drawing::RuntimeEffectOptions* ops) {
            std::shared_ptr<Drawing::RuntimeEffect> shader;
            if (ops != nullptr) {
                shader = Drawing::RuntimeEffect::CreateForShader(shaderStr_, *ops);
            }
            else {
                shader = Drawing::RuntimeEffect::CreateForShader(shaderStr_);
            } 
            if (shader == nullptr) {
                LOGE("GEShader::Compile %{public}s failed", name_.c_str());
                return GE_ERROR_SHADER_COMPILE_FAILED;
            }
            shader_ = shader;
            return GE_ERROR_NONE;
        }
        GE_Error GEShader::Connect(const GEShader& subShader) {
            // create a new name
            std::string newName = name_ + "_" + subShader.GetName();
            if (subShader.GetType() != TYPE_NO_REFERENCE) {
                // not accepted 
                return GE_ERROR_NOT_ACCEPTED_CONNECT;
            }

            // combine shader string
            std::string targetStr = {};
            if (CombineShaderString(targetStr, shaderStr_, subShader.GetString()) != GE_ERROR_NONE) {
                return GE_ERROR_SHADER_STRING_INVALID;
            }

            // update all items
            name_ = newName;
            shaderStr_ = targetStr;
            // type will not change
            // opt will change, if both have options, combine into one
            auto rthOpt = subShader.GetOptions();
            if (rthOpt != nullptr){
                if (opt_ != nullptr) {
                    opt_->useAF |= rthOpt->useAF;
                    opt_->forceNoInline |= rthOpt->forceNoInline;
                }
                else {
                    opt_ = new Drawing::RuntimeEffectOptions;
                    opt_->forceNoInline = rthOpt->forceNoInline;
                    opt_->useAF = rthOpt->useAF;
                }
            }

            return GE_ERROR_NONE;
        }
        static std::string PrepareFirstString(const std::string& sourcestr, const std::string& oldStr, const std::string& newStr) {
            std::istringstream iss(sourcestr);
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
        static std::string PrepareSecondString(const std::string& sourcestr, const std::string& oldStr, const std::string& newStr) {
            std::istringstream iss(sourcestr);
            std::ostringstream oss;
            std::string line;
            bool first = true;
            // Replace subStr with newStr in the line
            std::string reg(R"(\\.[\s\S]*[a-zA-Z0-9]*\\([a-zA-Z0-9]*\\))");
            std::regex from(oldStr + reg);
            std::string to = newStr;
            
            while (std::getline(iss, line)) {
                // remove imageShader define
                if (first && line.find(oldStr) != std::string::npos) {
                    // Skip the first line that contains subStr
                    first = false;
                    continue;
                }
                line = std::regex_replace(line, from, to);

                oss << line << '\n';
            }

            return oss.str();
        }
        static std::string GenSimpleUUIDString() {
            const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

            std::random_device random_device;
            std::mt19937 generator(random_device());
            std::uniform_int_distribution<> char_distribution(0, CHARACTERS.size() - 1);
            std::uniform_int_distribution<> num_distribution(0, 99);

            std::string random_string = "";
            random_string += CHARACTERS[char_distribution(generator)];
            random_string += std::to_string(num_distribution(generator));

            return random_string;
        }
        /*
        the two shaders should follow the rules as below:
        1. import image param's name should be imageShader (casesentense)
        2. param of main function should be float2 type and the name should be "coord", and only one param
        */
        GE_Error GEShader::CombineShaderString(std::string& cbStr, const std::string& shaderStrA, const std::string& shaderStrB) {
            // each valid shader string should be have 5 matches;
            const uint32_t VALID_MATCH_COUNT = 5; 
            std::string phase0Varname = GenSimpleUUIDString();
            // prepare shaderStrB, 
            auto strShaderB = PrepareSecondString(shaderStrB, "imageShader", phase0Varname);

            std::regex re(R"(([\s\S]*)(half4\s+main\s*\(\s*float2\s+coord\s*\)\s*\{)([\s\S]*)(\}))");

            std::smatch matchA;
            std::regex_search(shaderStrA, matchA, re);
            if (matchA.size() < VALID_MATCH_COUNT) {
                LOGW("GEShader::CombineShaderString, invalid first shader string");
                return GE_ERROR_SHADER_STRING_INVALID;
            }
            std::smatch matchB;
            std::regex_search(strShaderB, matchB, re);
            if (matchB.size() < VALID_MATCH_COUNT) {
                LOGW("GEShader::CombineShaderString, invalid second shader string");
                return GE_ERROR_SHADER_STRING_INVALID;
            }
            // if have multi return , need to jump to next section
            std::string phase0Body = PrepareFirstString(matchA[3].str(), "return ", phase0Varname +" = ");
            
            std::string phase0Section("vec4 " + phase0Varname + "=vec4(1.0);\n");
            cbStr = matchA[1].str() + "\n" + matchB[1].str() + "\n" + phase0Section +
            matchA[2].str() + "\n" +
            "    {" + phase0Body + "}\n" +
            "    {" + matchB[3].str() + "}\n" +
            matchA[4].str();

            return GE_ERROR_NONE;
        }

    }
}
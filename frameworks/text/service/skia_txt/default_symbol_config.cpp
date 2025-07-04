/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "default_symbol_config.h"

#ifdef _WIN32
#include "cstdlib"
#else
#include <climits>
#include <cstddef>
#endif
#include <fstream>

#include "src/ports/skia_ohos/HmSymbolConfig_ohos.h"
#include "symbol_resource/symbol_config_parser.h"
#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace Symbol {

namespace {
class SymbolAutoRegister {
public:
    SymbolAutoRegister() { RegisterSymbolConfigCallBack(); }
    ~SymbolAutoRegister() { UnregisterSymbolConfigCallBack(); }

private:
    void RegisterSymbolConfigCallBack()
    {
        static std::once_flag flag;
        std::call_once(flag, []() {
            std::function<int(const char*)> loadSymbolConfigFunc = [](const char* filePath) -> int {
                return DefaultSymbolConfig::GetInstance()->ParseConfigOfHmSymbol(filePath);
            };
            skia::text::HmSymbolConfig_OHOS::SetLoadSymbolConfig(loadSymbolConfigFunc);
        });
    }

    void UnregisterSymbolConfigCallBack() { skia::text::HmSymbolConfig_OHOS::ClearLoadSymbolConfig(); }
};

SymbolAutoRegister g_symbolAutoRegister;

const uint32_t ONE_BYTE_BITS_LEN = 8;
const int NO_ERROR = 0;                          // no error
const int ERROR_CONFIG_NOT_FOUND = 1;            // the configuration document is not found
const int ERROR_CONFIG_FORMAT_NOT_SUPPORTED = 2; // the formation of configuration is not supported

/* To get the data of font configuration file
 * \param fname the full name of the font configuration file
 * \param[out] size the size of data returned to the caller
 * \return The pointer of content of the file
 * \note The returned pointer should be freed by the caller
 */
std::unique_ptr<char[]> GetDataFromFile(const char* fname, std::streamsize& size)
{
    char tmpPath[PATH_MAX] = {0};
    if (strlen(fname) > PATH_MAX) {
        TEXT_LOGE("File path is too long, file: %{public}s", fname);
        return nullptr;
    }
#ifdef _WIN32
    auto canonicalFilePath = _fullpath(tmpPath, fname, sizeof(tmpPath));
#else
    auto canonicalFilePath = realpath(fname, tmpPath);
#endif
    if (canonicalFilePath == nullptr) {
        TEXT_LOGE("Invalid file path, file: %{public}s", fname);
        return nullptr;
    }

    std::ifstream file(tmpPath, std::ios::binary);
    if (!file || !file.good()) {
        TEXT_LOGE("Invalid file, unable to open file, file: %{public}s", fname);
        size = 0;
        return nullptr;
    }

    file.seekg(0, std::ios::end);
    size = file.tellg();
    if (size <= 0) {
        TEXT_LOGE("Empty File, file: %{public}s", fname);
        size = 0;
        return nullptr;
    }

    file.seekg(0, std::ios::beg);
    auto data = std::make_unique<char[]>(size);
    file.read(data.get(), size);

    if (file.gcount() != size) {
        TEXT_LOGE("Inconsistent file size verification, file: %{public}s", fname);
        size = 0;
        return nullptr;
    }
    return data;
}
} // namespace

DefaultSymbolConfig* DefaultSymbolConfig::GetInstance()
{
    static DefaultSymbolConfig singleton;
    return &singleton;
}

/* check the system font configuration document
 * \param fname the full name of the font configuration document
 * \return NO_ERROR successful
 * \return ERROR_CONFIG_NOT_FOUND config document is not found
 * \return ERROR_CONFIG_FORMAT_NOT_SUPPORTED config document format is not supported
 */
int DefaultSymbolConfig::CheckConfigFile(const char* fname, Json::Value& root)
{
    std::streamsize size = 0;
    auto data = GetDataFromFile(fname, size);
    if (data == nullptr) {
        TEXT_LOGE("Config file is not found : %{public}s", fname);
        return ERROR_CONFIG_NOT_FOUND;
    }
    JSONCPP_STRING errs;
    Json::CharReaderBuilder charReaderBuilder;
    std::unique_ptr<Json::CharReader> jsonReader(charReaderBuilder.newCharReader());
    bool isJson = jsonReader->parse(data.get(), data.get() + size, &root, &errs);
    if (!isJson) {
        if (!errs.empty()) {
            TEXT_LOGE("Error in config file:%{public}s", errs.c_str());
        } else {
            TEXT_LOGE("The format of config file is not supported:%{public}s", fname);
        }
        return ERROR_CONFIG_FORMAT_NOT_SUPPORTED;
    }
    if (!errs.empty()) {
        TEXT_LOGW("Warning in config file:%{public}s", errs.c_str());
    }
    return NO_ERROR;
}

int DefaultSymbolConfig::ParseConfigOfHmSymbol(const char* filePath)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (GetInit()) {
        return NO_ERROR;
    }
    if (filePath == nullptr || strlen(filePath) == 0) {
        return ERROR_CONFIG_NOT_FOUND;
    }
    Clear();
    Json::Value root;
    int err = CheckConfigFile(filePath, root);
    if (err != NO_ERROR) {
        root.clear();
        return err;
    }

    if (!SymbolConfigParser::ParseSymbolConfig(root, hmSymbolConfig_, animationInfos_)) {
        root.clear();
        TEXT_LOGW("Parse symbol config file failed");
        return ERROR_CONFIG_FORMAT_NOT_SUPPORTED;
    }

    root.clear();
    SetInit(true);
    return NO_ERROR;
}

OHOS::Rosen::Drawing::DrawingSymbolLayersGroups DefaultSymbolConfig::GetSymbolLayersGroups(uint16_t glyphId)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    OHOS::Rosen::Drawing::DrawingSymbolLayersGroups symbolLayersGroups;
    auto iter = hmSymbolConfig_.find(glyphId);
    if (iter != hmSymbolConfig_.end()) {
        symbolLayersGroups = iter->second;
    }
    return symbolLayersGroups;
}

uint32_t DefaultSymbolConfig::EncodeAnimationAttribute(
    uint16_t groupSum, uint16_t animationMode, OHOS::Rosen::Drawing::DrawingCommonSubType commonSubType)
{
    // Ensure that the values do not exceed 0xFF
    groupSum = std::min(groupSum, static_cast<uint16_t>(0xFF));
    animationMode = std::min(animationMode, static_cast<uint16_t>(0xFF));

    uint32_t result = static_cast<uint32_t>(groupSum);
    result = (result << ONE_BYTE_BITS_LEN) + static_cast<uint32_t>(animationMode);
    result = (result << ONE_BYTE_BITS_LEN) + static_cast<uint32_t>(commonSubType);
    return result;
}

std::vector<std::vector<OHOS::Rosen::Drawing::DrawingPiecewiseParameter>> DefaultSymbolConfig::GetGroupParameters(
    OHOS::Rosen::Drawing::DrawingAnimationType type, uint16_t groupSum, uint16_t animationMode,
    OHOS::Rosen::Drawing::DrawingCommonSubType commonSubType)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    std::vector<std::vector<OHOS::Rosen::Drawing::DrawingPiecewiseParameter>> groupParametersOut;
    if (animationInfos_.empty()) {
        return groupParametersOut;
    }
    std::unordered_map<OHOS::Rosen::Drawing::DrawingAnimationType, OHOS::Rosen::Drawing::DrawingAnimationInfo>::iterator
        iter = animationInfos_.find(type);
    if (iter == animationInfos_.end()) {
        return groupParametersOut;
    }
    auto key = EncodeAnimationAttribute(groupSum, animationMode, commonSubType);
    auto para = iter->second.animationParas.find(key);
    if (para != iter->second.animationParas.end()) {
        groupParametersOut = para->second.groupParameters;
    }
    return groupParametersOut;
}
} // namespace Symbol
} // namespace Rosen
} // namespace OHOS

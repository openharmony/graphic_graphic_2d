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

#include "custom_symbol_config.h"

#include <fstream>
#include <json/json.h>

#include "symbol_resource/symbol_config_parser.h"

namespace OHOS {
namespace Rosen {

CustomSymbolConfig* CustomSymbolConfig::GetInstance()
{
    static CustomSymbolConfig singleton;
    return &singleton;
}

LoadSymbolErrorCode CustomSymbolConfig::ParseConfig(const std::string &familyName,
    const uint8_t *data, size_t datalen)
{
    if (data == nullptr) {
        return LoadSymbolErrorCode::LOAD_FAILED;
    }
    
    if (symbolConfig_.find(familyName) != symbolConfig_.end()) {
        return LoadSymbolErrorCode::SUCCESS;
    }

    std::string srcString(data, data + datalen);
    Json::Value root;
    Json::Reader jsonReader;
    bool isJson = jsonReader.parse(srcString, root);
    if (!isJson) {
        return LoadSymbolErrorCode::JSON_ERROR;
    }

    std::unique_lock<std::shared_mutex> lock(mutex_);
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfigGroup;
    LoadSymbolErrorCode result = LoadSymbolErrorCode::JSON_ERROR;
    if (SymbolConfigParser::ParseSymbolLayersGrouping(root, symbolConfigGroup)) {
        symbolConfig_.emplace(familyName, symbolConfigGroup);
        result = LoadSymbolErrorCode::SUCCESS;
    }
    root.clear();
    return result;
}

std::optional<RSSymbolLayersGroups> CustomSymbolConfig::GetSymbolLayersGroups(const std::string &familyName,
    uint16_t glyphId)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = symbolConfig_.find(familyName);
    if (it == symbolConfig_.end()) {
        return std::nullopt;
    }

    std::unordered_map<uint16_t, RSSymbolLayersGroups> &layersInfoMap = it->second;
    auto infoIter = layersInfoMap.find(glyphId);
    if (infoIter == layersInfoMap.end()) {
        return std::nullopt;
    }
    return infoIter->second;
}

} // namespace Rosen
} // namespace OHOS
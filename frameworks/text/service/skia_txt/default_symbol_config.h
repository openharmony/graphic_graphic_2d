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

#ifndef DEFAULT_SYMBOL_CONFIG_H
#define DEFAULT_SYMBOL_CONFIG_H

#include <json/json.h>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "common/rs_macros.h"
#include "text/hm_symbol.h"

namespace OHOS {
namespace Rosen {
namespace Symbol {
class RS_EXPORT DefaultSymbolConfig {
public:
    static DefaultSymbolConfig* GetInstance();
    ~DefaultSymbolConfig() = default;

    OHOS::Rosen::Drawing::DrawingSymbolLayersGroups GetSymbolLayersGroups(uint16_t glyphId);

    std::vector<std::vector<OHOS::Rosen::Drawing::DrawingPiecewiseParameter>> GetGroupParameters(
        OHOS::Rosen::Drawing::DrawingAnimationType type, uint16_t groupSum, uint16_t animationMode = 0,
        OHOS::Rosen::Drawing::DrawingCommonSubType commonSubType = OHOS::Rosen::Drawing::DrawingCommonSubType::DOWN);

    int ParseConfigOfHmSymbol(const char* filePath);

    void Clear()
    {
        hmSymbolConfig_.clear();
        animationInfos_.clear();
        isInit_ = false;
    }

private:
    DefaultSymbolConfig() {}
    DefaultSymbolConfig(const DefaultSymbolConfig&) = delete;
    DefaultSymbolConfig& operator=(const DefaultSymbolConfig&) = delete;
    DefaultSymbolConfig(DefaultSymbolConfig&&) = delete;
    DefaultSymbolConfig& operator=(DefaultSymbolConfig&&) = delete;

    bool isInit_ = false;
    bool GetInit() const { return isInit_; }
    void SetInit(const bool init) { isInit_ = init; }

    std::shared_mutex mutex_;
    std::unordered_map<uint16_t, OHOS::Rosen::Drawing::DrawingSymbolLayersGroups> hmSymbolConfig_;
    std::unordered_map<OHOS::Rosen::Drawing::DrawingAnimationType, OHOS::Rosen::Drawing::DrawingAnimationInfo>
        animationInfos_;

    int CheckConfigFile(const char* fname, Json::Value& root);
    uint32_t EncodeAnimationAttribute(
        uint16_t groupSum, uint16_t animationMode, OHOS::Rosen::Drawing::DrawingCommonSubType commonSubType);
};
} // namespace Symbol
} // namespace Rosen
} // namespace OHOS
#endif // DEFAULT_SYMBOL_CONFIG_H
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

#ifndef CUSTOM_SYMBOL_CONFIG_H
#define CUSTOM_SYMBOL_CONFIG_H

#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "symbol_engine/drawing.h"
#include "text/hm_symbol.h"
#include "symbol_constants.h"

namespace OHOS {
namespace Rosen {
namespace Symbol {
class CustomSymbolConfig {
public:
    static CustomSymbolConfig* GetInstance();
    ~CustomSymbolConfig() = default;

    std::optional<RSSymbolLayersGroups> GetSymbolLayersGroups(const std::string &familyName, uint16_t glyphId);
    LoadSymbolErrorCode ParseConfig(const std::string &familyName, const uint8_t *data, size_t datalen);

private:
    CustomSymbolConfig() {}
    CustomSymbolConfig(const CustomSymbolConfig&) = delete;
    CustomSymbolConfig& operator=(const CustomSymbolConfig&) = delete;
    CustomSymbolConfig(CustomSymbolConfig&&) = delete;
    CustomSymbolConfig& operator=(CustomSymbolConfig&&) = delete;

    std::unordered_map<std::string, std::unordered_map<uint16_t, RSSymbolLayersGroups>> symbolConfig_;
    std::shared_mutex mutex_;
};
} // namespace Symbol
} // namespace Rosen
} // namespace OHOS
#endif // CUSTOM_SYMBOL_CONFIG_H
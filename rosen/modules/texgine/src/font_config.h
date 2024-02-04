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

#ifndef FONT_CONFIG_H
#define FONT_CONFIG_H

#include <json/json.h>
#include <string>
#include <vector>
#include <include/core/SkString.h>
#include <map>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class FontConfig {
public:
    explicit FontConfig(const char* fname = nullptr);
    virtual ~FontConfig() = default;
    virtual void Dump() const;
    std::vector<std::string> GetFontSet() const;

protected:
    int ParseConfig(const char* fname);
    int ParseFont(const Json::Value& root);
    int CheckConfigFile(const char* fname, Json::Value& root) const;
    static char* GetFileData(const char* fname, int& size);

private:
    std::vector<std::string> fontSet_;
};

typedef struct AdjustInfo {
    int origValue;
    int newValue;
} AdjustInfo;
using AdjustSet = std::vector<AdjustInfo>;

typedef struct AliasInfo {
    std::string familyName;
    int weight;
} AliasInfo;
using AliasSet = std::vector<AliasInfo>;

typedef struct FallbackInfo {
    std::string familyName;
    std::string font;
} FallbackInfo;
using FallbackInfoSet = std::vector<FallbackInfo>;

typedef struct FallbackGroup {
    std::string groupName;
    FallbackInfoSet fallbackInfoSet;
} FallbackGroup;
using FallbackGroupSet = std::vector<FallbackGroup>;

typedef struct FontGenericInfo {
    std::string familyName;
    AliasSet aliasSet;
    AdjustSet adjustSet;
} FontGenericInfo;
using GenericSet = std::vector<FontGenericInfo>;

typedef struct FontConfigJsonInfo {
    std::vector<std::string> fontDirSet;
    GenericSet genericSet;
    FallbackGroupSet fallbackGroupSet;
} FontConfigJsonInfo;

class FontConfigJson : public FontConfig {
public:
    FontConfigJson() = default;
    int ParseFile(const char* fname = nullptr);
    std::shared_ptr<FontConfigJsonInfo> GetFontConfigJsonInfo()
    {
        return fontPtr;
    }

    // for test
    void Dump() const override;

private:
    int ParseConfigList(const char* fname);
    int ParseFontDir(const Json::Value& root);
    int ParseGeneric(const Json::Value& root);
    int ParseAlias(const Json::Value& root, FontGenericInfo &genericInfo);
    int ParseAdjust(const Json::Value& root, FontGenericInfo &genericInfo);
    int ParseFallback(const Json::Value& root);
    int ParseFallbackItem(const Json::Value& root, FallbackInfo &fallbackInfo);
    int ParseDir(const Json::Value &root);
    int ParseFallbakArr(const Json::Value& arr, int i, const char* key);
    int CheckGeneric(const Json::Value& root, const char* key);
    int ParseAliasArr(const Json::Value& arr, const char* key, FontGenericInfo &genericInfo);
    void DumpFontDir() const;
    void DumpGeneric() const;
    void DumpForbak() const;
    void DumpAlias(const AliasSet &aliasSet) const;
    void DumpAjdust(const AdjustSet &adjustSet) const;

    std::shared_ptr<FontConfigJsonInfo> fontPtr = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
#endif /* FONT_CONFIG_H */

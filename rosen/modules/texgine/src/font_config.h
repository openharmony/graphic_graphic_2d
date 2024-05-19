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

#include <string>
#include <vector>
#include <include/core/SkString.h>
#include <map>

struct cJSON;
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
    int ParseFont(const cJSON* root);
    cJSON* CheckConfigFile(const char* fname) const;
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

using FontFileMap = std::map<std::string, std::string>;

class FontConfigJson : public FontConfig {
public:
    FontConfigJson() = default;
    int ParseFile(const char* fname = nullptr);
    int ParseFontFileMap(const char* fname = nullptr);
    std::shared_ptr<FontConfigJsonInfo> GetFontConfigJsonInfo()
    {
        return fontPtr;
    }
    std::shared_ptr<FontFileMap> GetFontFileMap()
    {
        return fontFileMap;
    }
    // for test
    void Dump() const override;

private:
    int ParseConfigList(const char* fname);
    int ParseConfigListPath(const char* fname);
    int ParseGeneric(const cJSON* root, const char* key);
    int ParseAlias(const cJSON* root, FontGenericInfo &genericInfo);
    int ParseAdjust(const cJSON* root, FontGenericInfo &genericInfo);
    int ParseFallback(const cJSON* root, const char* key);
    int ParseFontMap(const cJSON* root, const char* key);
    int ParseDir(const cJSON* root);
    void AnalyseFontDir(const cJSON* root);
    int ParseAdjustArr(const cJSON* arr, FontGenericInfo &genericInfo);
    int ParseAliasArr(const cJSON* arr, FontGenericInfo &genericInfo);
    void DumpFontDir() const;
    void DumpGeneric() const;
    void DumpForbak() const;
    void DumpAlias(const AliasSet &aliasSet) const;
    void DumpAjdust(const AdjustSet &adjustSet) const;
    void DumpFontFileMap() const;

    std::shared_ptr<FontConfigJsonInfo> fontPtr = nullptr;
    std::shared_ptr<FontFileMap> fontFileMap = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
#endif /* FONT_CONFIG_H */

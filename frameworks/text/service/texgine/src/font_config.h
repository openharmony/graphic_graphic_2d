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

#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
    static cJSON* CheckConfigFile(const char* fname);
    static char* GetFileData(const char* fname, int& size);

private:
    std::vector<std::string> fontSet_;
    std::string rootPath_;
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

using FontFileMap = std::unordered_map<std::string, std::string>;

class FontConfigJson : public FontConfig {
public:
    FontConfigJson() = default;
    int ParseFile(const char* fname = nullptr);
    int ParseFontFileMap(const char* fname = nullptr);
    template<typename T>
    static int ParseInstallConfig(const char* fontPath, T& fontPathList);
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
    int ParseFontMap(const cJSON* root, const char* key);
    int ParseDir(const cJSON* root);
    int ParseFonts(const cJSON* root);
    void AnalyseFont(const cJSON* root);
    static int ParseInstallFont(const cJSON* root, std::vector<std::string>& fontPathList);
    static int ParseInstallFont(const cJSON* root, FontFileMap& fontPathList);
    static void ParseFullName(const cJSON* root, std::vector<std::string>& fullNameList);
    void DumpFontDir() const;
    void DumpGeneric() const;
    void DumpForbak() const;
    void DumpAlias(const AliasSet &aliasSet) const;
    void DumpAjdust(const AdjustSet &adjustSet) const;
    void DumpFontFileMap() const;
    struct FontJson {
        int type = 0;
        int weight = 0;
        std::string alias;
        std::string family;
        std::string lang;
    };
    void EmplaceFontJson(const FontJson &fontJson);

    std::shared_ptr<FontConfigJsonInfo> fontPtr = nullptr;
    std::shared_ptr<FontFileMap> fontFileMap = nullptr;
    // because of some memory bugs, this place use shared ptr
    std::shared_ptr<std::unordered_map<std::string, size_t>> indexMap = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
#endif /* FONT_CONFIG_H */

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

#include "font_config.h"

#include <dirent.h>
#include <fstream>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef BUILD_NON_SDK_VER
#include "securec.h"
#endif
#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define SUCCESSED 0
#define FAILED 1

const std::string DEFAULT_DIR = "/system/fonts/";
const std::string FONT_DEFAULT_CONFIG = "/system/etc/fontconfig.json";

FontConfig::FontConfig(const char* fname)
{
    int err = ParseConfig(fname);
    if (err != 0) {
        LOGSO_FUNC_LINE(ERROR) << "parse config err";
    }
}

char* FontConfig::GetFileData(const char* fname, int& size)
{
#ifdef BUILD_NON_SDK_VER
    char realPath[PATH_MAX] = {0};
    if (fname == nullptr || realpath(fname, realPath) == NULL) {
        LOGSO_FUNC_LINE(ERROR) << "path or realPath is NULL";
        return nullptr;
    }
#endif
    std::ifstream file(fname);
    if (file.good()) {
        FILE* fp = fopen(fname, "r");
        if (fp == nullptr) {
            return nullptr;
        }
        fseek(fp, 0L, SEEK_END);
        size = ftell(fp) + 1;
        rewind(fp);
        char* data = static_cast<char*>(malloc(size));
        if (data == nullptr) {
            fclose(fp);
            return nullptr;
        }
#ifdef BUILD_NON_SDK_VER
        if (memset_s(data, size, 0, size) != EOK) {
            LOGSO_FUNC_LINE(ERROR) << "memset failed";
            free(data);
            data = nullptr;
            fclose(fp);
            return nullptr;
        }
#else
            memset(data, 0, size);
#endif
        (void)fread(data, size, 1, fp);
        fclose(fp);
        return data;
    }

    return nullptr;
}

int FontConfig::CheckConfigFile(const char* fname, Json::Value& root) const
{
    int size = 0;
    char* data = GetFileData(fname, size);
    if (data == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "data is null";
        return FAILED;
    }
    JSONCPP_STRING errs;
    Json::CharReaderBuilder charReaderBuilder;
    std::unique_ptr<Json::CharReader> jsonReader(charReaderBuilder.newCharReader());
    bool isJson = jsonReader->parse(data, data + size, &root, &errs);
    free(data);
    data = nullptr;

    if (!isJson || !errs.empty()) {
        LOGSO_FUNC_LINE(ERROR) << "not json or errs no empty";
        return FAILED;
    }
    return SUCCESSED;
}

int FontConfig::ParseFont(const Json::Value& root)
{
    for (unsigned int i = 0; i < root.size(); ++i) {
        if (root[i].isString()) {
            fontSet_.emplace_back(DEFAULT_DIR + root[i].asString());
        }
    }
    return SUCCESSED;
}

int FontConfig::ParseConfig(const char* fname)
{
    if (fname == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "fname is null";
        return FAILED;
    }
    Json::Value root;
    int err = CheckConfigFile(fname, root);
    if (err != 0) {
        LOGSO_FUNC_LINE(ERROR) << "check config file failed";
        return err;
    }
    const char* tag = "font";
    if (root.isMember(tag)) {
        if (root[tag].isArray()) {
            ParseFont(root[tag]);
        } else {
            LOGSO_FUNC_LINE(ERROR) << "not array";
            return FAILED;
        }
    } else {
        LOGSO_FUNC_LINE(ERROR) << "not member";
        return FAILED;
    }

    return SUCCESSED;
}

void FontConfig::Dump() const
{
    for (auto it : fontSet_) {
        LOGSO_FUNC_LINE(INFO) << "fname:" << it;
    }
}

std::vector<std::string> FontConfig::GetFontSet() const
{
    return fontSet_;
}

int FontConfigJson::ParseFile(const char* fname)
{
    if (fname == nullptr) {
        LOGSO_FUNC_LINE(DEBUG) << "ParseFile fname is nullptr";
        fname = FONT_DEFAULT_CONFIG.c_str();
    }

    LOGSO_FUNC_LINE(INFO) << "ParseFile fname is: " << fname;
    fontPtr = std::make_shared<FontConfigJsonInfo>();
    int err = ParseConfigList(fname);
    if (err != 0) {
        LOGSO_FUNC_LINE(ERROR) << "ParseFile ParseConfigList failed";
        return err;
    }
    return SUCCESSED;
}

int FontConfigJson::ParseDir(const Json::Value &root)
{
    // "fontdir" - directory
    const char* key = "fontdir";
    if (root.isMember(key)) {
        if (root[key].isArray()) {
            int ret = ParseFontDir(root[key]);
            if (ret != SUCCESSED) {
                LOGSO_FUNC_LINE(ERROR) << "ParseConfigList ParseFontDir failed";
                return ret;
            }
        } else {
            LOGSO_FUNC_LINE(ERROR) << "ParseConfigList root[fontdir] is not array";
            return FAILED;
        }
    }
    return SUCCESSED;
}

int FontConfigJson::ParseConfigList(const char* fname)
{
    if (fname == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "ParseConfigList fname is nullptr";
        return FAILED;
    }
    Json::Value root;
    int ret = CheckConfigFile(fname, root);
    if (ret != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "ParseConfigList CheckConfigFile failed";
        return ret;
    }
    ret = ParseDir(root);
    if (ret) {
        return ret;
    }
    // "generic", "fallback" - font attribute
    const char* keys[] = {"generic", "fallback", nullptr};
    int index = 0;
    while (true) {
        if (keys[index] == nullptr) {
            break;
        }
        const char* key = keys[index++];
        if (!root.isMember(key)) {
            LOGSO_FUNC_LINE(ERROR) << "ParseConfigList root[" << key << "] is not member";
            return FAILED;
        } else if (!root[key].isArray()) {
            LOGSO_FUNC_LINE(ERROR) << "ParseConfigList root[" << key << "] is not array";
            return FAILED;
        }
        const Json::Value& arr = root[key];
        for (unsigned int i = 0; i < arr.size(); i++) {
            ret = ParseFallbakArr(arr, i, key);
            if (ret != SUCCESSED) {
                return ret;
            }
        }
    }
    root.clear();
    return SUCCESSED;
}

int FontConfigJson::ParseFallbakArr(const Json::Value& arr, int i, const char* key)
{
    int ret;
    if (arr[i].isObject()) {
        if (!strcmp(key, "generic")) {
            if ((ret = ParseGeneric(arr[i])) != SUCCESSED) {
                LOGSO_FUNC_LINE(ERROR) << "ParseConfigList ParseGeneric failed";
                return ret;
            }
        } else if (!strcmp(key, "fallback")) {
            if ((ret = ParseFallback(arr[i])) != SUCCESSED) {
                LOGSO_FUNC_LINE(ERROR) << "ParseConfigList ParseFallback failed";
                return ret;
            }
        }
    } else {
        LOGSO_FUNC_LINE(ERROR) << "ParseConfigList arr is not object :" << i;
        return FAILED;
    }
    return SUCCESSED;
}

int FontConfigJson::ParseFontDir(const Json::Value& root)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (root[i].isString()) {
            const char* dir = root[i].asCString();
            fontPtr->fontDirSet.emplace_back(std::string(dir));
        } else {
            LOGSO_FUNC_LINE(ERROR) << "ParseFontDir root is not string :" << i;
            return FAILED;
        }
    }
    return SUCCESSED;
}

int FontConfigJson::CheckGeneric(const Json::Value& root, const char* key)
{
    if (!root.isMember(key)) {
        LOGSO_FUNC_LINE(ERROR) << "ParseGeneric root is not member";
        return FAILED;
    } else if (!root[key].isString()) {
        LOGSO_FUNC_LINE(ERROR) << "ParseGeneric root is not string";
        return FAILED;
    }
    // "alias" - necessary, the data type should be Array
    if (!root.isMember("alias")) {
        LOGSO_FUNC_LINE(ERROR) << "ParseGeneric root has no alias member";
        return FAILED;
    }
    return SUCCESSED;
}

int FontConfigJson::ParseAliasArr(const Json::Value& arr, const char* key,
    FontGenericInfo &genericInfo)
{
    for (unsigned int j = 0; j < arr.size(); j++) {
        if (arr[j].isObject()) {
            if (!strcmp(key, "alias")) {
                ParseAlias(arr[j], genericInfo);
            } else if (!strcmp(key, "adjust")) {
                ParseAdjust(arr[j], genericInfo);
            } else {
                LOGSO_FUNC_LINE(ERROR) << "ParseGeneric not support";
            }
        } else {
            LOGSO_FUNC_LINE(ERROR) << "ParseGeneric arr[j:" << j << "] is not object";
            return FAILED;
        }
    }
    return SUCCESSED;
}

int FontConfigJson::ParseGeneric(const Json::Value& root)
{
    // "family" - necessary, the data type should be String
    const char* key = "family";
    std::string familyName = root[key].asCString();
    int ret = CheckGeneric(root, key);
    if (ret) {
        return ret;
    }
    // "adjust", "variation" - optional
    std::vector<std::string> tags = {"alias", "adjust"};
    unsigned int tagsCont = 2; // tags size
    FontGenericInfo genericInfo;
    genericInfo.familyName = familyName;
    for (unsigned int i = 0; i < tags.size(); i++) {
        key= tags[i].c_str();
        if (!root.isMember(key)) {
            continue;
        }
        if (root[key].isArray()) {
            const Json::Value& arr = root[key];
            ret = ParseAliasArr(arr, key, genericInfo);
            if (ret) return ret;
        } else {
            LOGSO_FUNC_LINE(ERROR) << "ParseGeneric root[key:" << key << "] is not array";
            return FAILED;
        }
        if (root.size() == tagsCont) {
            break;
        }
    }
    fontPtr->genericSet.push_back(genericInfo);
    return SUCCESSED;
}

int FontConfigJson::ParseAlias(const Json::Value& root, FontGenericInfo &genericInfo)
{
    if (root.empty()) {
        LOGSO_FUNC_LINE(ERROR) << "ParseAlias root is empty";
        return FAILED;
    }
    Json::Value::Members members = root.getMemberNames();
    const char* key = members[0].c_str();
    if (!root[key].isInt()) {
        LOGSO_FUNC_LINE(ERROR) << "ParseAlias root[key:" << key << "] is not int";
        return FAILED;
    }

    std::string aliasName = std::string(key);
    int weight = root[key].asInt();
    AliasInfo info = {aliasName, weight};
    genericInfo.aliasSet.emplace_back(std::move(info));
    return SUCCESSED;
}

int FontConfigJson::ParseAdjust(const Json::Value& root, FontGenericInfo &genericInfo)
{
    std::vector<std::string> tags = {"weight", "to"};
    int values[2]; // value[0] - to save 'weight', value[1] - to save 'to'
    for (unsigned int i = 0; i < tags.size(); i++) {
        const char* key  = tags[i].c_str();
        if (!root.isMember(key)) {
            LOGSO_FUNC_LINE(ERROR) << "ParseAdjust root[key:" << key << "] is not member";
            return FAILED;
        } else if (!root[key].isInt()) {
            LOGSO_FUNC_LINE(ERROR) << "ParseAdjust root[key:" << key << "] is not int";
            return FAILED;
        } else {
            values[i] = root[key].asInt();
        }
    }
    AdjustInfo info = {values[0], values[1]};
    genericInfo.adjustSet.emplace_back(std::move(info));
    return SUCCESSED;
}

int FontConfigJson::ParseFallback(const Json::Value& root)
{
    if (root.empty()) {
        LOGSO_FUNC_LINE(ERROR) << "ParseFallback root is empty";
        return FAILED;
    }
    Json::Value::Members members = root.getMemberNames();
    const char* key = members[0].c_str();
    if (!root[key].isArray()) {
        LOGSO_FUNC_LINE(ERROR) << "ParseFallback root[key:" << key << "] is not array";
        return FAILED;
    }
    FallbackGroup fallbackGroup;
    fallbackGroup.groupName = std::string(key);
    const Json::Value& fallbackArr = root[key];
    for (unsigned int i = 0; i < fallbackArr.size(); i++) {
        if (!fallbackArr[i].isObject()) {
            continue;
        }
        FallbackInfo fallbackInfo;
        ParseFallbackItem(fallbackArr[i], fallbackInfo);
        fallbackGroup.fallbackInfoSet.emplace_back(std::move(fallbackInfo));
    }
    fontPtr->fallbackGroupSet.emplace_back(std::move(fallbackGroup));
    return SUCCESSED;
}

int FontConfigJson::ParseFallbackItem(const Json::Value& root, FallbackInfo &fallbackInfo)
{
    if (root.empty()) {
        LOGSO_FUNC_LINE(ERROR) << "ParseFallbackItem root is empty";
        return FAILED;
    }
    Json::Value::Members members = root.getMemberNames();
    const char* key = nullptr;
    for (unsigned int i = 0; i < members.size(); i++) {
        if (members[i] != "variations" && members[i] != "index") {
            key = members[i].c_str();
        } else continue;
    }
    if (key == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "ParseFallbackItem key is nullptr";
        return FAILED;
    }
    if (!root[key].isString()) {
        LOGSO_FUNC_LINE(ERROR) << "ParseFallbackItem root[key:" << key << "] is not string";
        return FAILED;
    }
    std::string lang = std::string(key);
    std::string familyName = root[key].asCString();

    fallbackInfo.familyName = familyName;
    fallbackInfo.font = lang;
    return SUCCESSED;
}

void FontConfigJson::DumpAlias(const AliasSet &aliasSet) const
{
    if (!aliasSet.empty()) {
        const std::string space = "    ";
        LOGSO_FUNC_LINE(INFO) << "  \"alias\": [";
        for (auto it : aliasSet) {
            LOGSO_FUNC_LINE(INFO) << "  {";
            LOGSO_FUNC_LINE(INFO) << space << "  \"" <<
                it.familyName << "\" : " << it.weight;
            LOGSO_FUNC_LINE(INFO) << "   },";
        }
        LOGSO_FUNC_LINE(INFO) << "  ],";
    }
}

void FontConfigJson::DumpAjdust(const AdjustSet &adjustSet) const
{
    if (!adjustSet.empty()) {
        LOGSO_FUNC_LINE(INFO) << "  \"adjust\": [";
        const std::string space = "    ";
        for (auto it : adjustSet) {
            LOGSO_FUNC_LINE(INFO) << "   {";
            LOGSO_FUNC_LINE(INFO) << space << "  \"weght\" :" <<
                it.origValue << " , " << "\"to\" :" << it.newValue;
            LOGSO_FUNC_LINE(INFO) << "   },";
        }
        LOGSO_FUNC_LINE(INFO) << "  ],";
    }
}

void FontConfigJson::DumpGeneric() const
{
    LOGSO_FUNC_LINE(INFO) << "generic : [";
    if (!fontPtr->genericSet.empty()) {
        for (auto it : fontPtr->genericSet) {
            LOGSO_FUNC_LINE(INFO) << "  \"family\": [\""<< it.familyName << "\"],";
            DumpAlias(it.aliasSet);
            DumpAjdust(it.adjustSet);
        }
    }
    LOGSO_FUNC_LINE(INFO) << "]";
}

void FontConfigJson::DumpForbak() const
{
    if (!fontPtr->fallbackGroupSet.empty()) {
        LOGSO_FUNC_LINE(INFO) << "\"fallback\": [";
        LOGSO_FUNC_LINE(INFO) << "{";
        for (auto group : fontPtr->fallbackGroupSet) {
            LOGSO_FUNC_LINE(INFO) << " \"" << group.groupName << "\" : [";
            if (group.fallbackInfoSet.empty()) continue;
            const std::string space = "    ";
            for (auto it : group.fallbackInfoSet) {
                LOGSO_FUNC_LINE(INFO) << "  {";
                LOGSO_FUNC_LINE(INFO) << space << it.font << "\" : \""
                    << it.familyName << "\"";
                LOGSO_FUNC_LINE(INFO) << "   },";
            }
            LOGSO_FUNC_LINE(INFO) << " ]";
        }
        LOGSO_FUNC_LINE(INFO) << "}";
        LOGSO_FUNC_LINE(INFO) << "]";
    }
}

void FontConfigJson::DumpFontDir() const
{
    LOGSO_FUNC_LINE(INFO) << "fontdir : [";
    if (!fontPtr->fontDirSet.empty()) {
        for (auto it : fontPtr->fontDirSet) {
            LOGSO_FUNC_LINE(INFO) << "\""<< it << "\",";
        }
    }
    LOGSO_FUNC_LINE(INFO) << "]";
}

void FontConfigJson::Dump() const
{
    LOGSO_FUNC_LINE(INFO) << "font config dump in";
    if (fontPtr == nullptr) {
        LOGSO_FUNC_LINE(INFO) << "adapter is nullptr";
        return;
    }

    DumpFontDir();
    DumpGeneric();
    DumpForbak();

    LOGSO_FUNC_LINE(INFO) << "dump out";
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
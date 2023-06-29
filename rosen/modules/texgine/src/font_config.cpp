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

#include "securec.h"
#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define SUCCESSED 0
#define FAILED 1

const std::string DEFAULT_DIR = "/system/fonts/";

FontConfig::FontConfig(const char* fname)
{
    int err = ParseConfig(fname);
    if (err != 0) {
        LOGSO_FUNC_LINE(ERROR) << "parse config err";
    }
}

char* FontConfig::GetFileData(const char* fname, int& size) const
{
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
        if (memset_s(data, size, 0, size) != EOK) {
            LOGSO_FUNC_LINE(ERROR) << "memset failed";
            free(data);
            data = nullptr;
            fclose(fp);
            return nullptr;
        }
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
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
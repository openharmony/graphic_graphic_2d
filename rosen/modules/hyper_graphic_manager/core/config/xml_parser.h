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

#ifndef HGM_XML_PARSER_H
#define HGM_XML_PARSER_H

#include <string>
#include <unordered_map>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "hgm_command.h"
#include "hgm_log.h"

namespace OHOS::Rosen {
class XMLParser {
public:
    int32_t LoadConfiguration(const char* fileDir);
    int32_t Parse();
    void Destroy();

    explicit XMLParser() : xmlDocument_(nullptr) {}

    ~XMLParser()
    {
        Destroy();
    }

    std::unique_ptr<PolicyConfigData> GetParsedData()
    {
        return std::move(mParsedData_);
    }

    static bool IsNumber(const std::string& str);

private:
    static int32_t GetHgmXmlNodeAsInt(xmlNode &node);
    bool ParseInternal(xmlNode &node);
    int32_t ParseParam(xmlNode &node);
    int32_t ParseParams(xmlNode &node);
    int32_t ParseStrategyConfig(xmlNode &node);
    int32_t ParseScreenConfig(xmlNode &node);
    int32_t ParseSimplex(xmlNode &node, std::unordered_map<std::string, std::string> &config,
                         const std::string &valueName = "value", const std::string &keyName = "name");
    int32_t ParserDynamicSetting(xmlNode &node, PolicyConfigData::DynamicSettingMap &dynamicSettingMap);
    int32_t ParseSceneList(xmlNode &node, PolicyConfigData::SceneConfigMap &sceneList);
    int32_t ParseMultiAppStrategy(xmlNode &node, PolicyConfigData::ScreenSetting& screenSetting);
    std::string ExtractPropertyValue(const std::string &propName, xmlNode &node);

    xmlDoc *xmlDocument_;
    std::unique_ptr<PolicyConfigData> mParsedData_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // HGM_XML_PARSER_H
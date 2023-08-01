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
    int32_t LoadConfiguration();
    int32_t Parse();
    void Destroy();

    explicit XMLParser() : xmlDocument_(nullptr) {}

    ~XMLParser()
    {
        Destroy();
    }

    std::unique_ptr<ParsedConfigData> GetParsedData()
    {
        return std::move(mParsedData_);
    }

private:
    int32_t GetHgmXmlNodeAsInt(xmlNode &node) const;
    bool ParseInternal(xmlNode &node);
    int32_t ParseParam(xmlNode &node);
    int32_t ParseParams(xmlNode &node);
    int32_t ParseSetting(xmlNode &node, std::unordered_map<std::string, std::string> &config);
    int32_t ParserAnimationDynamicSetting(xmlNode &node);
    int32_t ParseStrat(xmlNode &node);
    std::string ExtractPropertyValue(const std::string &propName, xmlNode &node);
    static bool IsNumber(const std::string &str);

    xmlDoc *xmlDocument_;
    static constexpr char CONFIG_FILE[] = "/system/etc/graphic/hgm_policy_config.xml";
    std::unordered_map<std::string, int> hgmXmlLabel_;
    std::unique_ptr<ParsedConfigData> mParsedData_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // HGM_XML_PARSER_H
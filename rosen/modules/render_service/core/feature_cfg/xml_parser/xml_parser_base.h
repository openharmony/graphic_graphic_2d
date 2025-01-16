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

#ifndef XML_PARSER_BASE_H
#define XML_PARSER_BASE_H

#include <string>
#include <unordered_map>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "platform/common/rs_log.h"
#include "feature_param.h"

namespace OHOS::Rosen {
using FeatureParamMapType = std::unordered_map<std::string, std::shared_ptr<FeatureParam>>;

enum ParseXmlNode {
    PARSE_XML_UNDEFINED = 0,
    PARSE_XML_FEATURE_SWITCH,
    PARSE_XML_FEATURE_SINGLEPARAM,
    PARSE_XML_FEATURE_MULTIPARAM,
};

class XMLParserBase {
public:
    virtual ~XMLParserBase() = default;

    virtual int32_t ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
    {
        return 0;
    };

    int32_t LoadGraphicConfiguration(const char* fileDir);
    int32_t Parse();
    bool ParseFeatureSwitch(std::string val);
    std::string ParseFeatureMultiParam(std::string type, std::string val);

    std::string ExtractPropertyValue(const std::string &propName, xmlNode &node);
    int32_t GetXmlNodeAsInt(xmlNode &node);
    static bool IsNumber(const std::string& str);

private:
    bool ParseInternal(xmlNode &node);
    xmlDoc *xmlDocument_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // XML_PARSER_BASE_H
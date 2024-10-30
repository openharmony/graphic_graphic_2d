/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_2D_XML_PARSER_H
#define GRAPHIC_2D_XML_PARSER_H

#include <string>
#include <unordered_map>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "graphic_2d_configure_commad.h"

namespace OHOS::Rosen {
class Graphic2dXmlParser {
public:
    int32_t LoadConfiguration(const char* fileDir);
    int32_t Parse();
    void Destroy();

    explicit Graphic2dXmlParser() : xmlDocument_(nullptr) {}

    ~Graphic2dXmlParser()
    {
        Destroy();
    }

    std::unique_ptr<Graphic2dConfigData> GetParsedData()
    {
        return std::move(mParsedData_);
    }
    
private:
    static int32_t GetG2cXmlNodeAsInt(xmlNode &node);
    bool ParseInternal(xmlNode &node);
    int32_t ParseParams(xmlNode &node);
    int32_t ParseSimplex(xmlNode &node, std::unordered_map<std::string, std::string> &config,
                         const std::string &valueName = "value", const std::string &keyName = "name");
    xmlDoc *xmlDocument_ = nullptr;
    std::string ExtractPropertyValue(const std::string &propName, xmlNode &node);
    std::unique_ptr<Graphic2dConfigData> mParsedData_ = nullptr;
};
}
#endif // GRAPHIC_2D_XML_PARSER_H
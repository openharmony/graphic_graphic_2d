/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ddgr_config_param_parse.h"

namespace OHOS::Rosen {
int32_t DDGRConfigParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("DDGRConfigParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("DDGRConfigParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseDDGRConfigInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("DDGRConfigParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    RS_LOGI("DDGRConfigParamParse end");

    return PARSE_EXEC_SUCCESS;
}

int32_t DDGRConfigParamParse::ParseDDGRConfigInternal(xmlNode &node)
{
    std::string name = ExtractPropertyValue("name", node);
    std::string val = ExtractPropertyValue("value", node);
    if (name.empty() || val.empty()) {
        RS_LOGD("DDGRConfigParamParse: skip unnamed or unvalued param");
        return PARSE_EXEC_SUCCESS;
    }

    DDGRConfigParam::SetParam(name, val);
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen

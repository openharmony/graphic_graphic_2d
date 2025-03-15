/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "hard_cursor_param_parse.h"

namespace OHOS::Rosen {

int32_t HardCursorParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGE("ParseFeatureParam stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParsePrevalidateInternal(featureMap, *currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("ParsePrevalidateInternal stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t HardCursorParamParse::ParsePrevalidateInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    auto iter = featureMap.find(FEATURE_CONFIGS[HARDCURSOR]);
    if (iter != featureMap.end()) {
        HardCursorParam_ = std::static_pointer_cast<HardCursorParam>(iter->second);
    } else {
        RS_LOGE("ParsePrevalidateInternal stop parsing, no initializing param map");
        return PARSE_NO_PARAM;
    }

    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH && name == "HardCursorEnabled") {
        bool isEnabled = ParseFeatureSwitch(val);
        HardCursorParam_->SetHardCursorEnable(isEnabled);
    }

    return PARSE_EXEC_SUCCESS;
}
}
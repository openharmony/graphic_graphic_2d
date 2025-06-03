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

#include <sstream>
#include "rotateoffscreen_param_parse.h"

namespace OHOS::Rosen {
int32_t RotateOffScreenParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGE("RotateOffScreenParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseRotateOffScreenInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("RotateOffScreenParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t RotateOffScreenParamParse::ParseRotateOffScreenInternal(xmlNode &node)
{
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "RotateOffScreenDisplayNodeEnabled") {
            RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(isEnabled);
            RS_LOGI("RotateOffScreenParamParse parse RotateOffScreenDisplayNodeEnable %{public}d", isEnabled);
        } else if (name == "RotateOffScreenSurfaceNodeEnabled") {
            RotateOffScreenParam::SetRotateOffScreenSurfaceNodeEnable(isEnabled);
            RS_LOGI("RotateOffScreenParamParse parse RotateOffScreenSurfaceNodeEnable %{public}d", isEnabled);
        } else if (name == "RotateOffScreenDowngradeEnabled") {
            RotateOffScreenParam::SetRotateOffScreenDowngradeEnable(isEnabled);
            RS_LOGI("RotateOffScreenParamParse parse RotateOffScreenDowngradeEnabled %{public}d", isEnabled);
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
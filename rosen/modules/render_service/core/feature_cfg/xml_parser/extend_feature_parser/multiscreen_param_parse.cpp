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

#include "multiscreen_param_parse.h"

namespace OHOS::Rosen {

int32_t MultiScreenParamParse::ParseFeatureParam([[maybe_unused]] FeatureParamMapType& featureMap, xmlNode& node)
{
    RS_LOGI("MultiScreenParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGW("MultiScreenParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseMultiScreenInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGW("MultiScreenParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    RS_LOGI("MultiScreenParamParse end, isExternalScreenSecure: %{public}d, isSlrScaleEnabled: %{public}d,"
        " isRsReportHwcDead: %{public}d, isRsSetScreenPowerStatus: %{public}d, isMirrorDisplayCloseP3: %{public}d",
        MultiScreenParam::IsExternalScreenSecure(), MultiScreenParam::IsSlrScaleEnabled(),
        MultiScreenParam::IsRsReportHwcDead(), MultiScreenParam::IsRsSetScreenPowerStatus(),
        MultiScreenParam::IsMirrorDisplayCloseP3());
    return PARSE_EXEC_SUCCESS;
}

int32_t MultiScreenParamParse::ParseMultiScreenInternal(xmlNode& node)
{
    xmlNode *currNode = &node;

    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "IsExternalScreenSecure") {
            MultiScreenParam::SetExternalScreenSecure(isEnabled);
        } else if (name == "IsSlrScaleEnabled") {
            MultiScreenParam::SetSlrScaleEnabled(isEnabled);
        } else if (name == "IsRsReportHwcDead") {
            MultiScreenParam::SetRsReportHwcDead(isEnabled);
        } else if (name == "IsRsSetScreenPowerStatus") {
            MultiScreenParam::SetRsSetScreenPowerStatus(isEnabled);
        } else if (name == "IsMirrorDisplayCloseP3") {
            MultiScreenParam::SetMirrorDisplayCloseP3(isEnabled);
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
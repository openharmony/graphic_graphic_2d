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

#include "hdr_param_parse.h"

namespace OHOS::Rosen {

int32_t HDRParamParse::ParseFeatureParam(FeatureParamMapType featureMap, xmlNode &node)
{
    RS_LOGI("HDRParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("HDRParamParse stop parsing, no children nodes");
        return CCM_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseHdrInternal(featureMap, *currNode) != CCM_EXEC_SUCCESS) {
            RS_LOGE("HDRParamParse stop parsing, parse internal fail");
            return CCM_PARSE_INTERNAL_FAIL;
        }
    }

    return CCM_EXEC_SUCCESS;
}

int32_t HDRParamParse::ParseHdrInternal(FeatureParamMapType featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;

    auto iter = featureMap.find(paramVec[featureParamCode::HDR]);
    if (iter != featureMap.end()) {
        hdrParam_ = std::static_pointer_cast<HDRParam>(iter->second);
    } else {
        RS_LOGD("HDRParamParse stop parsing, no initializing param map");
        return CCM_NO_PARAM;
    }

    // Start Parse Feature Params
    int xmlParamType = GetCcmXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == CCM_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "HdrVideoEnabled") {
            hdrParam_->SetHdrVideoEnable(isEnabled);
            RS_LOGD("HDRParamParse parse HdrVideoEnabled %{public}d", hdrParam_->IsHdrVideoEnable());
        } else if (name == "HdrImageEnabled") {
            hdrParam_->SetHdrImageEnable(isEnabled);
            RS_LOGD("HDRParamParse parse HdrImageEnabled %{public}d", hdrParam_->IsHdrImageEnable());
        }
    }

    return CCM_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
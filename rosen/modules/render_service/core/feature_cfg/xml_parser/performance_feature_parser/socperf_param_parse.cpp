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

#include "socperf_param_parse.h"

namespace OHOS::Rosen {

int32_t SOCPerfParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("SOCPerfParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("SOCPerfParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseSOCPerfInternal(featureMap, *currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("SOCPerfParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t SOCPerfParamParse::ParseSOCPerfInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;

    auto iter = featureMap.find(FEATURE_CONFIGS[SOC_PERF]);
    if (iter == featureMap.end()) {
        RS_LOGD("SOCPerfParamParse stop parsing, no initializing param map");
        return PARSE_NO_PARAM;
    }
    socPerfParam_ = std::static_pointer_cast<SOCPerfParam>(iter->second);

    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "MultilayersSocperfEnabled") {
            socPerfParam_->SetMultilayersSOCPerfEnable(isEnabled);
            RS_LOGI("SOCPerfParamParse parse MultilayersSocperfEnabled %{public}d", 
                socPerfParam_->IsMultilayersSOCPerfEnable());
        } else if (name == "UnlockSocperfEnabled") {
            socPerfParam_->SetUnlockSOCPerfEnable(isEnabled);
            RS_LOGI("SOCPerfParamParse parse UnlockSocperfEnabled %{public}d", 
                socPerfParam_->IsUnlockSOCPerfEnable());
        } else if (name == "BlurSocperfEnabled") {
            socPerfParam_->SetBlurSOCPerfEnable(isEnabled);
            RS_LOGI("SOCPerfParamParse parse BlurSocperfEnabled %{public}d", 
                socPerfParam_->IsBlurSOCPerfEnable());
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
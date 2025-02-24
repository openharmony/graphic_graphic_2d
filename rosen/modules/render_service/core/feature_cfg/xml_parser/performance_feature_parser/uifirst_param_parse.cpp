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
#include "uifirst_param_parse.h"

namespace OHOS::Rosen {

UIFirstSwitchType UIFirstParamParse::GetUIFirstSwitchType(const std::string& input)
{
    static const std::map<std::string, UIFirstSwitchType> uifirstSwitchTypeMap = {
        {"UIFirstEnabled", UIFirstSwitchType::UIFIRST_ENABLED},
        {"CardUIFirstEnabled", UIFirstSwitchType::CARD_UIFIRST_ENABLED},
        {"CacheOptimizeRotateEnabled", UIFirstSwitchType::ROTATE_ENABLED},
        {"FreeMultiWindowEnabled", UIFirstSwitchType::FREE_MULTI_WINDOW_ENABLED}
    };

    auto it = uifirstSwitchTypeMap.find(input);
    if (it != uifirstSwitchTypeMap.end()) {
        return it->second;
    }
    return UIFirstSwitchType::UNKNOWN;
}

int32_t UIFirstParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("UIFirstParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("UIFirstParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseUIFirstInternal(featureMap, *currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("UIFirstParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t UIFirstParamParse::ParseUIFirstInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;

    auto iter = featureMap.find(FEATURE_CONFIGS[UIFirst]);
    if (iter != featureMap.end()) {
        uifirstParam_ = std::static_pointer_cast<UIFirstParam>(iter->second);
    } else {
        RS_LOGD("UIFirstParamParse stop parsing, no initializing param map");
    }

    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        switch (GetUIFirstSwitchType(name)) {
            case UIFirstSwitchType::UIFIRST_ENABLED:
                uifirstParam_->SetUIFirstEnable(isEnabled);
                RS_LOGI("UIFirstParamParse parse UIFirstEnabled %{public}d", uifirstParam_->IsUIFirstEnable());
                break;
            case UIFirstSwitchType::CARD_UIFIRST_ENABLED:
                uifirstParam_->SetCardUIFirstEnable(isEnabled);
                RS_LOGI("UIFirstParamParse parse CardUIFirstEnabled %{public}d",
                    uifirstParam_->IsCardUIFirstEnable());
                break;
            case UIFirstSwitchType::ROTATE_ENABLED:
                uifirstParam_->SetCacheOptimizeRotateEnable(isEnabled);
                RS_LOGI("UIFirstParamParse parse CacheOptimizeRotateEnabled %{public}d",
                    uifirstParam_->IsCacheOptimizeRotateEnable());
                break;
            case UIFirstSwitchType::FREE_MULTI_WINDOW_ENABLED:
                uifirstParam_->SetFreeMultiWindowEnable(isEnabled);
                RS_LOGI("UIFirstParamParse parse FreeMultiWindowEnabled %{public}d",
                    uifirstParam_->IsFreeMultiWindowEnable());
                break;
            default:
                RS_LOGI("UIFirstParamParse parse %{public}s is not support!", name.c_str());
                break;
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        if (name == "UIFirstEnableWindowThreshold" && IsNumber(val)) {
            int num;
            std::istringstream iss(val);
            if (iss >> num) {
                uifirstParam_->SetUIFirstEnableWindowThreshold(num);
                RS_LOGI("UIFirstParamParse parse UIFirstEnableWindowThreshold %{public}d",
                    uifirstParam_->GetUIFirstEnableWindowThreshold());
            }
        }
    }

    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
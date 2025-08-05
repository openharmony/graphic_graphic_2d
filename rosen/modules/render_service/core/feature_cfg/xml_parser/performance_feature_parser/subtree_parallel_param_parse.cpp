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
#include "subtree_parallel_param_parse.h"

namespace OHOS::Rosen {

SubtreeSwitchType SubtreeParallelParamParse::GetSubtreeSwitchType(const std::string &input)
{
    static const std::map<std::string, SubtreeSwitchType> subtreeSwitchTypeMap = {
        {"SubtreeEnabled", SubtreeSwitchType::SUBTREE_ENABLED},
        {"MutliWinPolicyEnabled", SubtreeSwitchType::MULTIWIN_POLICY_ENABLED},
        {"MutliWinPolicySurfaceNumber", SubtreeSwitchType::MULTIWIN_POLICY_SURFACE_NUMBER},
        {"RBPolicyEnabled", SubtreeSwitchType::RB_POLICY_ENABLED},
        {"RBPolicyChildrenWeight", SubtreeSwitchType::RB_POLICY_CHILDREN_WEIGHT},
        {"RBPolicySubtreeWeight", SubtreeSwitchType::RB_POLICY_SUBTREE_WEIGHT},
        {"SubtreeEnableScene", SubtreeSwitchType::SUBTREE_PARALLEL_ENABLE_SCENE},
    };

    auto it = subtreeSwitchTypeMap.find(input);
    if (it != subtreeSwitchTypeMap.end()) {
        return it->second;
    }
    return SubtreeSwitchType::UNKNOWN;
}

int32_t SubtreeParallelParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGE("SubtreeParallelParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseSubtreeInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("SubtreeParallelParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t SubtreeParallelParamParse::ParseFeatureSingleParam(std::string name, std::string val)
{
    int num = -1;
    if (!IsNumber(val)) {
        RS_LOGE("SubtreeParallelParamParse failed, val is not number, name[%{public}s], value[%{public}s]",
            name.c_str(), val.c_str());
        return PARSE_ERROR;
    }
    num = std::stoi(val);
    switch (GetSubtreeSwitchType(name)) {
        case SubtreeSwitchType::MULTIWIN_POLICY_SURFACE_NUMBER:
            SubtreeParallelParam::SetMutliWinSurfaceNum(num);
            RS_LOGI("SubtreeParallelParamParse parse SetMutliWinSurfaceNum %{public}d", num);
            break;
        case SubtreeSwitchType::RB_POLICY_CHILDREN_WEIGHT:
            SubtreeParallelParam::SetRBChildrenWeight(num);
            RS_LOGI("SubtreeParallelParamParse parse SetRBChildrenWeight %{public}d", num);
            break;
        case SubtreeSwitchType::RB_POLICY_SUBTREE_WEIGHT:
            SubtreeParallelParam::SetRBSubtreeWeight(num);
            RS_LOGI("SubtreeParallelParamParse parse SetRBSubtreeWeight %{public}d", num);
            break;
        default:
            RS_LOGE("SubtreeParallelParamParse parse FeatureSingleParam failed name[%{public}s]", name.c_str());
            return PARSE_ERROR;
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t SubtreeParallelParamParse::ParseFeatureMultiParamForApp(xmlNode &node, std::string &name)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto appName = ExtractPropertyValue("name", *currNode);
        auto val = ExtractPropertyValue("value", *currNode);
        if (!IsNumber(val)) {
            return PARSE_ERROR;
        }
        if (GetSubtreeSwitchType(name) == SubtreeSwitchType::SUBTREE_PARALLEL_ENABLE_SCENE) {
            SubtreeParallelParam::SetSubtreeScene(appName, val);
            RS_LOGI("SubtreeParallelParamParse SetSubtreeScene appName: %{public}s, val: %{public}s",
                appName.c_str(), val.c_str());
        } else {
            RS_LOGD("SubtreeParallelParamParse cannot find name[%{public}s], appName[%{public}s], value[%{public}s]",
                name.c_str(), appName.c_str(), val.c_str());
            return PARSE_NO_PARAM;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t SubtreeParallelParamParse::ParseSubtreeInternal(xmlNode &node)
{
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        switch (GetSubtreeSwitchType(name)) {
            case SubtreeSwitchType::SUBTREE_ENABLED:
                SubtreeParallelParam::SetSubtreeEnable(isEnabled);
                RS_LOGI("SubtreeParallelParamParse SubtreeEnabled %{public}d", isEnabled);
                break;
            case SubtreeSwitchType::RB_POLICY_ENABLED:
                SubtreeParallelParam::SetRBPolicyEnabled(isEnabled);
                RS_LOGI("SubtreeParallelParamParse SetRBPolicyEnabled %{public}d", isEnabled);
                break;
            case SubtreeSwitchType::MULTIWIN_POLICY_ENABLED:
                SubtreeParallelParam::SetMultiWinPolicyEnabled(isEnabled);
                RS_LOGI("SubtreeParallelParamParse SetMultiWinPolicyEnabled %{public}d", isEnabled);
                break;
            default:
                RS_LOGE("SubtreeParallelParamParse %{public}s is not support!", name.c_str());
                break;
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        if (ParseFeatureSingleParam(name, val) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("SubtreeParallelParamParse parse SingleParam fail");
            return PARSE_INTERNAL_FAIL;
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_MULTIPARAM) {
        xmlNode *currNode = &node;
        if (ParseFeatureMultiParamForApp(*currNode, name) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("SubtreeParallelParamParse parse MultiParam fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
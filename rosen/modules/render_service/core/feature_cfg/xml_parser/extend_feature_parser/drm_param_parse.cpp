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

#include "drm_param_parse.h"

namespace OHOS::Rosen {
constexpr const int LIST_SIZE_LIMIT = 100;
constexpr const int APP_WINDOW_NAME_LIMIT = 100;

int32_t DRMParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("DRMParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("DRMParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseDrmInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("DRMParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t DRMParamParse::ParseDrmInternal(xmlNode &node)
{
    xmlNode *currNode = &node;
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "DrmEnabled") {
            DRMParam::SetDrmEnable(isEnabled);
            RS_LOGI("DRMParamParse parse DrmEnabled %{public}d", DRMParam::IsDrmEnable());
        } else if (name == "DrmMarkAllParentBlurEnabled") {
            DRMParam::SetDrmMarkAllParentBlurEnable(isEnabled);
            RS_LOGI("DRMParamParse parse DrmMaskBlurEnabled %{public}d", DRMParam::IsDrmMarkAllParentBlurEnable());
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_MULTIPARAM) {
        if (ParseFeatureMultiParam(*currNode, name) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("parse MultiParam fail");
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t DRMParamParse::ParseFeatureMultiParam(xmlNode &node, std::string &name)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGE("DRMParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    for (int i = 0; currNode && i < LIST_SIZE_LIMIT ; currNode = currNode->next, ++i) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto paramName = ExtractPropertyValue("name", *currNode);
        if (paramName.size() > APP_WINDOW_NAME_LIMIT) {
            continue;
        }
        auto val = ExtractPropertyValue("value", *currNode);
        if (!IsNumber(val)) {
            return PARSE_ERROR;
        }
        if (val == "1") {
            DRMParam::AddWhiteList(paramName);
        } else if (val == "0") {
            DRMParam::AddBlackList(paramName);
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
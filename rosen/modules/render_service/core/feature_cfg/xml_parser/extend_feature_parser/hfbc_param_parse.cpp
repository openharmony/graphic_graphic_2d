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

#include "hfbc_param_parse.h"

#include "hgm_core.h"

namespace OHOS::Rosen {

int32_t HFBCParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("HFBCParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGE("HFBCParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseHfbcInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("HFBCParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t HFBCParamParse::ParseHfbcInternal(xmlNode &node)
{
    xmlNode *currNode = &node;

    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_MULTIPARAM) {
        if (ParseFeatureMultiParamForApp(*currNode, name) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("HFBCParamParse parse MultiParam fail");
        }
        if (name == "HfbcDisable") {
            HgmTaskHandleThread::Instance().PostTask([val] () {
                HgmHfbcConfig& hfbcConfig = HgmCore::Instance().GetHfbcConfig();
                RS_LOGI("HFBCParamParse postTask about hfbcConfig");
                hfbcConfig.SetHfbcConfigMap(HFBCParam::GetHfbcConfigMap());
                // val 0: enable list mode, other: disable list mode
                hfbcConfig.SetHfbcControlMode(val != "0");
            });
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t HFBCParamParse::ParseFeatureMultiParamForApp(xmlNode &node, std::string &name)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGE("HFBCParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto property = ExtractPropertyValue("name", *currNode);
        auto val = ExtractPropertyValue("value", *currNode);
        if (!IsNumber(val)) {
            return PARSE_ERROR;
        }
        RS_LOGI("HFBCParamParse %{public}s: property:%{public}s, value:%{public}s",
            __func__, property.c_str(), val.c_str());
        if (name == "HfbcDisable") {
            HFBCParam::SetHfbcConfigForApp(property, val);
        } else {
            RS_LOGE("HFBCParamParse ParseFeatureMultiParam cannot find name:%s", name.c_str());
            return PARSE_NO_PARAM;
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
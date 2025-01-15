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

#include <algorithm>

#include "xml_parser_base.h"
#include "graphic_ccm_feature_param_manager.h"

namespace OHOS::Rosen {

int32_t XMLParserBase::LoadGraphicConfiguration(const char* fileDir)
{
    RS_LOGD("XMLParserBase opening xml file");
    xmlDocument_ = xmlReadFile(fileDir, nullptr, 0);
    if (!xmlDocument_) {
        RS_LOGE("XMLParser xmlReadFile failed");
        return CCM_FILE_LOAD_FAIL;
    }

    return CCM_EXEC_SUCCESS;
}

int32_t XMLParserBase::Parse()
{
    RS_LOGI("XMLParserBase Parse Start");
    if (!xmlDocument_) {
        RS_LOGE("XMLParser xmlDocument_ is empty, should do LoadGraphicConfiguration first");
        return CCM_FILE_LOAD_FAIL;
    }
    xmlNode *root = xmlDocGetRootElement(xmlDocument_);
    if (root == nullptr) {
        RS_LOGE("XMLParser xmlDocGetRootElement failed");
        return CCM_GET_ROOT_FAIL;
    }

    if (ParseInternal(*root) == false) {
        return CCM_PARSE_INTERNAL_FAIL;
    }
    return CCM_EXEC_SUCCESS;
}

bool XMLParserBase::ParseInternal(xmlNode &node)
{
    RS_LOGI("XMLParserBase ParseInternal Start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("XMLParserBase stop parsing internal, no children nodes");
        return false;
    }
    currNode = currNode->xmlChildrenNode;
    int32_t parseSuccess = CCM_EXEC_SUCCESS;

    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (parseSuccess != CCM_EXEC_SUCCESS) {
            return false;
        }

        std::string featureName = ExtractPropertyValue("name", *currNode);
        RS_LOGI("XMLParserBase featureName is: %{public}s", featureName.c_str());
        if (featureName = "") {
            return false;
        }
        auto parseMap = GraphicCcmFeatureParamManager::GetInstance().featureParseMap_;
        auto featureMap = GraphicCcmFeatureParamManager::GetInstance().featureParamMap_;
        auto iter = parseMap.find(featureName);
        if (iter != parseMap.end()) {
            auto featureObj = iter->second;
            parseSuccess = featureObj->ParseFeatureParam(featureMap, *currNode);
        } else {
            RS_LOGE("XMLParserBase featureMap cannot find feature %{public}s", featureName.c_str());
        }
    }
    return true;
}

std::string XMLParserBase::ExtractPropertyValue(const std::string &propName, xmlNode &node)
{
    RS_LOGD("XMLParserBase extracting value : %{public}s", propName.c_str());
    std::string propValue = "";
    xmlChar *tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        RS_LOGD("XMLParser not a empty tempValue");
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }

    return propValue;
}

int32_t XMLParserBase::GetCcmXmlNodeAsInt(xmlNode &node)
{
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureSwicth"))) {
        return CCM_XML_FEATURE_SWITCH;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureSingleParam"))) {
        return CCM_XML_FEATURE_SINGLEPARAM;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureMultiParam"))) {
        return CCM_XML_FEATURE_MULTIPARAM;
    }
    RS_LOGD("XMLParserBase failed to identify a xml node : %{public}s", node.name);
    return CCM_XML_UNDEFINED;
}

bool XMLParserBase::ParseFeatureSwitch(std::string val)
{
    return val == "true";
}

int32_t XMLParserBase::ParseFeatureSingleParam(std::string val)
{
    return std::atoi(val.c_str());
}
} // namespace OHOS::Rosen
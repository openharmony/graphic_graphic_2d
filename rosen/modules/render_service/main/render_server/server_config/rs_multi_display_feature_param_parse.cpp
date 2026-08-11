/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_multi_display_feature_param_parse.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t XML_STRING_MAX_LENGTH = 20;
}

SplitScreenFeature RSMultiDisplayFeatureParamParse::ParseSplitScreenFeature(xmlNode &featureNode)
{
    RS_LOGI("RSMultiDisplayFeature %{public}s XML parse SplitScreenFeature", __func__);
    xmlNodePtr featureNodePtr = &featureNode;
    SplitScreenFeature featureConfig;
    // Parse Feature Switch
    auto name = ExtractPropertyValue("name", *featureNodePtr);
    auto value = ExtractPropertyValue("value", *featureNodePtr);
    if (name == "switch") {
        featureConfig.enabled = (value == "true") ? true : false;
    }
    // Parse Feature Params
    for (xmlNodePtr node = featureNodePtr->xmlChildrenNode; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("params")) == 0) {
            auto mainDisplayId = ExtractPropertyValue("mainDisplayId", *node);
            auto subDisplayId = ExtractPropertyValue("subDisplayId", *node);
            SplitScreenParams param;
            if (IsNumber(mainDisplayId) && IsNumber(subDisplayId)) {
                param.mainDisplayId = std::stoull(mainDisplayId);
                param.subDisplayId = std::stoull(subDisplayId);
                featureConfig.params[param.mainDisplayId] = param;
            } else {
                RS_LOGW("RSMultiDisplayFeature %{public}s SplitScreenFeature param is invalid.", __func__);
            }
        }
    }
    return featureConfig;
}

InterpolationFeature RSMultiDisplayFeatureParamParse::ParseInterpolationFeature(xmlNode &featureNode)
{
    RS_LOGI("RSMultiDisplayFeature %{public}s XML parse InterpolationFeature", __func__);
    xmlNodePtr featureNodePtr = &featureNode;
    InterpolationFeature featureConfig;
    // Parse Feature Switch
    auto name = ExtractPropertyValue("name", *featureNodePtr);
    auto value = ExtractPropertyValue("value", *featureNodePtr);
    if (name == "switch") {
        featureConfig.enabled = (value == "true") ? true : false;
    }
    // Parse Feature Params
    for (xmlNodePtr node = featureNodePtr->xmlChildrenNode; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("params")) == 0) {
            auto displayId = ExtractPropertyValue("displayId", *node);
            auto realWidth = ExtractPropertyValue("realWidth", *node);
            auto realHeight = ExtractPropertyValue("realHeight", *node);
            auto paramA = ExtractPropertyValue("paramA", *node);
            auto paramN = ExtractPropertyValue("paramN", *node);
            InterpolationParams param;
            if (IsNumber(displayId) && IsNumber(realWidth) && IsNumber(realHeight) &&
                IsNumber(paramA) && IsNumber(paramN)) {
                param.displayId = std::stoull(displayId);
                param.realWidth = std::stoul(realWidth);
                param.realHeight = std::stoul(realHeight);
                param.paramA = std::stoul(paramA);
                param.paramN = std::stoul(paramN);
                featureConfig.params[param.displayId] = param;
            } else {
                RS_LOGW("RSMultiDisplayFeature %{public}s InterpolationFeature param is invalid.", __func__);
            }
        }
    }
    return featureConfig;
}

CrossDomainFeature RSMultiDisplayFeatureParamParse::ParseCrossDomainFeature(xmlNode &featureNode)
{
    RS_LOGI("RSMultiDisplayFeature %{public}s XML parse CrossDomainFeature", __func__);
    xmlNodePtr featureNodePtr = &featureNode;
    CrossDomainFeature featureConfig;
    // Parse Feature Switch
    auto name = ExtractPropertyValue("name", *featureNodePtr);
    auto value = ExtractPropertyValue("value", *featureNodePtr);
    if (name == "switch") {
        featureConfig.enabled = (value == "true") ? true : false;
    }
    // Parse Feature Params
    for (xmlNodePtr node = featureNodePtr->xmlChildrenNode; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("params")) == 0) {
            auto displayId = ExtractPropertyValue("displayId", *node);
            if (IsNumber(displayId)) {
                featureConfig.params.insert(std::stoull(displayId));
            } else {
                RS_LOGW("RSMultiDisplayFeature %{public}s CrossDomainFeature param is invalid.", __func__);
            }
        }
    }
    return featureConfig;
}

std::string RSMultiDisplayFeatureParamParse::ExtractPropertyValue(const std::string &propName, xmlNode &node)
{
    RS_LOGD("RSMultiDisplayFeature extracting value : %{public}s", propName.c_str());
    std::string propValue = "";
    xmlChar *tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        RS_LOGD("RSMultiDisplayFeature not a empty tempValue");
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }

    return propValue;
}

bool RSMultiDisplayFeatureParamParse::IsNumber(const std::string& str)
{
    if (str.length() == 0 || str.length() > XML_STRING_MAX_LENGTH) {
        return false;
    }
    auto number = static_cast<uint32_t>(std::count_if(str.begin(), str.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}
} // namespace OHOS::Rosen
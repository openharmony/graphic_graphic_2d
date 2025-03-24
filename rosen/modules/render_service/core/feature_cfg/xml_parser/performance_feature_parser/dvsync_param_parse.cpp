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

#include "dvsync_param_parse.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

int32_t DVSyncParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("DVSyncParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseDVSyncInternal(featureMap, *currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("DVSyncParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t DVSyncParamParse::ParseDVSyncInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;

    auto iter = featureMap.find(FEATURE_CONFIGS[DVSYNC]);
    if (iter != featureMap.end()) {
        dvsyncParam_ = std::static_pointer_cast<DVSyncParam>(iter->second);
    } else {
        RS_LOGD("stop parsing, no initializing param map");
        return PARSE_NO_PARAM;
    }
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "DVSyncEnabled") {
            dvsyncParam_->SetDVSyncEnable(isEnabled);
            RS_LOGI("DVSyncParamParse parse DVSyncEnabled %{public}d", dvsyncParam_->IsDVSyncEnable());
        } else if (name == "UiDVSyncEnabled") {
            dvsyncParam_->SetUiDVSyncEnable(isEnabled);
            RS_LOGI("DVSyncParamParse parse UiDVSyncEnabled %{public}d", dvsyncParam_->IsUiDVSyncEnable());
        } else if (name == "NativeDVSyncEnabled") {
            dvsyncParam_->SetNativeDVSyncEnable(isEnabled);
            RS_LOGI("DVSyncParamParse parse NativeDVSyncEnabled %{public}d", dvsyncParam_->IsNativeDVSyncEnable());
        } else if (name == "AdaptiveDVSyncEnabled") {
            dvsyncParam_->SetAdaptiveDVSyncEnable(isEnabled);
            RS_LOGI("DVSyncParamParse parse AdaptiveDVSyncEnabled %{public}d", dvsyncParam_->IsAdaptiveDVSyncEnable());
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        if (name == "DVSyncRsBufferCount") {
            dvsyncParam_->SetRsBufferCount(std::atoi(val.c_str()));
            RS_LOGI("DVSyncParamParse parse DVSyncRsBufferCount %{public}d", dvsyncParam_->GetRsBufferCount());
        } else if (name == "DVSyncUiBufferCount") {
            dvsyncParam_->SetUiBufferCount(std::atoi(val.c_str()));
            RS_LOGI("DVSyncParamParse parse DVSyncUiBufferCount %{public}d", dvsyncParam_->GetUiBufferCount());
        } else if (name == "DVSyncNativeBufferCount") {
            dvsyncParam_->SetNativeBufferCount(std::atoi(val.c_str()));
            RS_LOGI("DVSyncParamParse parse DVSyncNativeBufferCount %{public}d", dvsyncParam_->GetNativeBufferCount());
        } else if (name == "DVSyncWebBufferCount") {
            dvsyncParam_->SetWebBufferCount(std::atoi(val.c_str()));
            RS_LOGI("DVSyncParamParse parse DVSyncWebBufferCount %{public}d", dvsyncParam_->GetWebBufferCount());
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_MULTIPARAM) {
        if (ParseFeatureMultiParam(*currNode, name) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("parse MultiParam fail");
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t DVSyncParamParse::ParseFeatureMultiParam(xmlNode &node, std::string &name)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("DVSyncParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto paramName = ExtractPropertyValue("name", *currNode);
        auto val = ExtractPropertyValue("value", *currNode);
        if (!IsNumber(val)) {
            return PARSE_ERROR;
        }
        dvsyncParam_->SetAdaptiveConfig(paramName, val);
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
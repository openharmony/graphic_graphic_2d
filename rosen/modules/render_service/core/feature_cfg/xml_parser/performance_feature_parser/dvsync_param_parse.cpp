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

        if (ParseDVSyncInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("DVSyncParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t DVSyncParamParse::ParseDVSyncInternal(xmlNode &node)
{
    xmlNode *currNode = &node;
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "DVSyncEnabled") {
            DVSyncParam::SetDVSyncEnable(isEnabled);
            RS_LOGI("DVSyncParamParse parse DVSyncEnabled %{public}d", DVSyncParam::IsDVSyncEnable());
        } else if (name == "UiDVSyncEnabled") {
            DVSyncParam::SetUiDVSyncEnable(isEnabled);
            RS_LOGI("DVSyncParamParse parse UiDVSyncEnabled %{public}d", DVSyncParam::IsUiDVSyncEnable());
        } else if (name == "NativeDVSyncEnabled") {
            DVSyncParam::SetNativeDVSyncEnable(isEnabled);
            RS_LOGI("DVSyncParamParse parse NativeDVSyncEnabled %{public}d", DVSyncParam::IsNativeDVSyncEnable());
        } else if (name == "AdaptiveDVSyncEnabled") {
            DVSyncParam::SetAdaptiveDVSyncEnable(isEnabled);
            RS_LOGI("DVSyncParamParse parse AdaptiveDVSyncEnabled %{public}d", DVSyncParam::IsAdaptiveDVSyncEnable());
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        if (name == "DVSyncRsBufferCount") {
            DVSyncParam::SetRsBufferCount(std::atoi(val.c_str()));
            RS_LOGI("DVSyncParamParse parse DVSyncRsBufferCount %{public}d", DVSyncParam::GetRsBufferCount());
        } else if (name == "DVSyncUiBufferCount") {
            DVSyncParam::SetUiBufferCount(std::atoi(val.c_str()));
            RS_LOGI("DVSyncParamParse parse DVSyncUiBufferCount %{public}d", DVSyncParam::GetUiBufferCount());
        } else if (name == "DVSyncNativeBufferCount") {
            DVSyncParam::SetNativeBufferCount(std::atoi(val.c_str()));
            RS_LOGI("DVSyncParamParse parse DVSyncNativeBufferCount %{public}d", DVSyncParam::GetNativeBufferCount());
        } else if (name == "DVSyncWebBufferCount") {
            DVSyncParam::SetWebBufferCount(std::atoi(val.c_str()));
            RS_LOGI("DVSyncParamParse parse DVSyncWebBufferCount %{public}d", DVSyncParam::GetWebBufferCount());
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
        if (name == "ForceRsDVsyncConfig") {
            DVSyncParam::SetForceRsDVsyncConfig(paramName, val);
        } else {
            DVSyncParam::SetAdaptiveConfig(paramName, val);
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
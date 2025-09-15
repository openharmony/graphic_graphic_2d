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

#include "image_enhance_param_parse.h"

#include "hgm_core.h"

namespace OHOS::Rosen {
int32_t ImageEnhanceParamParse::ParseFeatureParam(FeatureParamMapType& featureMap, xmlNode& node)
{
    RS_LOGI("ImageEnhanceParamParse start");
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGE("ImageEnhanceParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (ParseImageEnhanceInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("ImageEnhanceParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t ImageEnhanceParamParse::ParseImageEnhanceInternal(xmlNode& node)
{
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    if (xmlParamType != PARSE_XML_FEATURE_MULTIPARAM || name != "ImageEnhance" ||
        ParseFeatureMultiParamForApp(node) != PARSE_EXEC_SUCCESS) {
        return PARSE_INTERNAL_FAIL;
    }
    HgmTaskHandleThread::Instance().PostTask([]() {
        auto& hgmCore = HgmCore::Instance();
        hgmCore.SetImageEnhanceScene(ImageEnhanceParam::GetImageEnhanceScene());
        RS_LOGI("ImageEnhanceParamParse postTask about ImageEnhanceScene");
    });
    return PARSE_EXEC_SUCCESS;
}

int32_t ImageEnhanceParamParse::ParseFeatureMultiParamForApp(xmlNode& node)
{
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        return PARSE_GET_CHILD_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        std::string appName = ExtractPropertyValue("name", *currNode);
        RS_LOGI("ImageEnhanceParamParse ImageEnhanceApp: appName:%{public}s", appName.c_str());
        if (!appName.empty()) {
            ImageEnhanceParam::SetImageEnhanceScene(appName);
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen
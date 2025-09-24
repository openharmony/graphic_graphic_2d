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
    std::unordered_map<std::string, std::function<int32_t(xmlNode&)>> parserMap = {
        { "ImageEnhance", [this] (xmlNode& node) {return ParseImageEnhanceInternal(node);} },
        { "ImageEnhanceParam", [this] (xmlNode& node) {return ParseImageEnhanceParamInternal(node);} },
        { "SLR", [this] (xmlNode& node) {return ParseImageEnhanceAlgoInternal(node);} },
        { "ESR", [this] (xmlNode& node) {return ParseImageEnhanceAlgoInternal(node);} },
    };
    currNode = currNode->xmlChildrenNode;
    size_t successCount = 0;
    for (; currNode != nullptr; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        std::string nodeName = ExtractPropertyValue("name", *currNode);
        if (parserMap.find(nodeName) == parserMap.end()) {
            RS_LOGE("ImageEnhanceParamParse stop parsing, invalid node: %{public}s.", nodeName.c_str());
            return PARSE_INTERNAL_FAIL;
        }
        if (parserMap[nodeName](*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("ImageEnhanceParamParse stop parsing, parse %{public}s fail!", nodeName.c_str());
            return PARSE_INTERNAL_FAIL;
        } else {
            successCount++;
        }
    }
    if (successCount != parserMap.size()) {
        RS_LOGE("ImageEnhanceParamParse parsing fail, %{public}zu success, %{public}zu expected",
            successCount, parserMap.size());
        return PARSE_INTERNAL_FAIL;
    }
    RsCommonHook::Instance().SetImageEnhanceParams(params_);
    RsCommonHook::Instance().SetImageEnhanceAlgoParams(algoParams_);
    return PARSE_EXEC_SUCCESS;
}

int32_t ImageEnhanceParamParse::ParseImageEnhanceInternal(xmlNode& node)
{
    int xmlParamType = GetXmlNodeAsInt(node);
    if (xmlParamType != PARSE_XML_FEATURE_MULTIPARAM ||
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
    for (; currNode != nullptr; currNode = currNode->next) {
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

int32_t ImageEnhanceParamParse::ParseImageEnhanceParamInternal(xmlNode& node)
{
    int xmlParamType = GetXmlNodeAsInt(node);
    if (xmlParamType != PARSE_XML_FEATURE_MULTIPARAM ||
        ParseImageEnhanceParam(node) != PARSE_EXEC_SUCCESS) {
        return PARSE_INTERNAL_FAIL;
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t ImageEnhanceParamParse::ParseImageEnhanceParam(xmlNode& node)
{
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        return PARSE_GET_CHILD_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    const std::unordered_map<std::string, std::function<bool(xmlNode&)>> handlers = {
        { "MinSize", [this] (xmlNode& node) {
                return ExtractValue<int>(node, "%d", params_.minSize);
            }
        },
        { "MaxSize", [this] (xmlNode& node) {
                return ExtractValue<int>(node, "%d", params_.maxSize);
            }
        },
        { "MinScaleRatio", [this] (xmlNode& node) {
            return ExtractValue<float>(node, "%f", params_.minScaleRatio);
            }
        },
        { "MaxScaleRatio", [this] (xmlNode& node) {
                return ExtractValue<float>(node, "%f", params_.maxScaleRatio);
            }
        },
    };
    for (; currNode != nullptr; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto it = handlers.find(ExtractPropertyValue("name", *currNode));
        if (it == handlers.end()) {
            return PARSE_INTERNAL_FAIL;
        }
        if (!it->second(*currNode)) {
            return PARSE_INTERNAL_FAIL;
        }
    }
    if (!CheckParams()) {
        return PARSE_INTERNAL_FAIL;
    }
    return PARSE_EXEC_SUCCESS;
}

bool ImageEnhanceParamParse::CheckParams()
{
    if (params_.minSize >= params_.maxSize || params_.minScaleRatio >= params_.maxScaleRatio) {
        return false;
    }
    params_.isValid = true;
    return true;
}

int32_t ImageEnhanceParamParse::ParseImageEnhanceAlgoInternal(xmlNode& node)
{
    int xmlParamType = GetXmlNodeAsInt(node);
    if (xmlParamType != PARSE_XML_FEATURE_MULTIPARAM ||
        ParseImageEnhanceAlgoParam(node) != PARSE_EXEC_SUCCESS) {
        return PARSE_INTERNAL_FAIL;
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t ImageEnhanceParamParse::ParseImageEnhanceAlgoParam(xmlNode& node)
{
    std::string name = ExtractPropertyValue("name", node);
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        return PARSE_GET_CHILD_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    RSImageDetailEnhanceAlgoParams algoParams;
    RSImageDetailEnhanceRangeParams params;
    const std::unordered_map<std::string, std::function<bool(xmlNode&)>> handlers = {
        { "RangeMin",
            [this, &params] (xmlNode& node) { return ExtractValue<float>(node, "%f", params.rangeMin); }
        },
        { "RangeMax",
            [this, &params] (xmlNode& node) { return ExtractValue<float>(node, "%f", params.rangeMax); }
        },
        { "param", [this, &params, &algoParams] (xmlNode& node) {
                if (!ExtractValue<float>(node, "%f", params.effectParam)) {
                    return false;
                }
                algoParams.rangeParams.push_back(params);
                return true;
            }
        },
        { "MinSize",
            [this, &algoParams] (xmlNode& node) { return ExtractValue<int>(node, "%d", algoParams.minSize); }
        },
        { "MaxSize",
            [this, &algoParams] (xmlNode& node) { return ExtractValue<int>(node, "%d", algoParams.maxSize); }
        },
    };
    for (; currNode != nullptr; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto it = handlers.find(ExtractPropertyValue("name", *currNode));
        if (it == handlers.end()) {
            return PARSE_INTERNAL_FAIL;
        }
        if (!it->second(*currNode)) {
            return PARSE_INTERNAL_FAIL;
        }
    }
    if (!CheckAlgoParams(algoParams)) {
        return PARSE_INTERNAL_FAIL;
    }
    algoParams_[name] = algoParams;
    return PARSE_EXEC_SUCCESS;
}

bool ImageEnhanceParamParse::CheckAlgoParams(RSImageDetailEnhanceAlgoParams& algoParams)
{
    if (algoParams.minSize >= algoParams.maxSize || algoParams.rangeParams.size() == 0) {
        return false;
    }
    float lastRangeMax = algoParams.rangeParams[0].rangeMin;
    for (auto& rangeParam : algoParams.rangeParams) {
        if (rangeParam.rangeMin >= rangeParam.rangeMax || rangeParam.rangeMin < lastRangeMax) {
            return false;
        }
        lastRangeMax = rangeParam.rangeMax;
    }
    algoParams.isValid = true;
    return true;
}

} // namespace OHOS::Rosen
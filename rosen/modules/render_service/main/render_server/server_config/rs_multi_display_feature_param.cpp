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

#include "rs_multi_display_feature_param.h"

#include "platform/common/rs_log.h"
#include "rs_multi_display_feature_param_parse.h"

namespace OHOS::Rosen {
namespace {
const std::string PRODUCT_FILE = "/sys_prod/etc/graphic/graphic_render_server_config.xml";
}

bool RSMultiDisplayFeatureParam::Load(const std::string& filePath)
{
    RS_LOGI("RSMultiDisplayFeature %{public}s opening xml file", __func__);
    xmlDocPtr docPtr = nullptr;

    if (filePath.empty()) {
        docPtr = xmlReadFile(PRODUCT_FILE.c_str(), nullptr, 0);
    } else {
        docPtr = xmlReadFile(filePath.c_str(), nullptr, 0);
    }
    if (!docPtr) {
        RS_LOGW("RSMultiDisplayFeature %{public}s xmlReadFile failed.", __func__);
        return false;
    }
    return Parse(*docPtr);
}

bool RSMultiDisplayFeatureParam::Parse(xmlDoc& doc)
{
    RS_LOGI("RSMultiDisplayFeature %{public}s XML parse", __func__);
    xmlDocPtr docPtr = &doc;
    xmlNodePtr root = nullptr;
    root = xmlDocGetRootElement(docPtr);
    if (!root || xmlStrcmp(root->name, reinterpret_cast<const xmlChar*>("Configs")) != 0) {
        RS_LOGW("RSMultiDisplayFeature %{public}s xmlDocGetRootElement failed", __func__);
        xmlFreeDoc(docPtr);
        return false;
    }
    for (xmlNodePtr node = root->xmlChildrenNode; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("multi_display_features")) == 0) {
            ParseFeatureParam(*node);
            break;
        }
    }
    xmlFreeDoc(docPtr);
    return true;
}

void RSMultiDisplayFeatureParam::ParseFeatureParam(xmlNode& featuresNode)
{
    xmlNodePtr featuresNodePtr = &featuresNode;
    for (xmlNodePtr node = featuresNodePtr->xmlChildrenNode; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("split_screen_feature")) == 0) {
            splitFeature_ = RSMultiDisplayFeatureParamParse::ParseSplitScreenFeature(*node);
        } else if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("interp_screen_feature")) == 0) {
            interpolationFeature_ = RSMultiDisplayFeatureParamParse::ParseInterpolationFeature(*node);
        } else if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("cross_domain_feature")) == 0) {
            crossDomainFeature_ = RSMultiDisplayFeatureParamParse::ParseCrossDomainFeature(*node);
        }
    }
}

bool RSMultiDisplayFeatureParam::IsSplitScreenFeatureEnable()
{
    return splitFeature_.enabled;
}

bool RSMultiDisplayFeatureParam::IsInterpolationFeatureEnable()
{
    if (splitFeature_.enabled) {
        return interpolationFeature_.enabled;
    }
    return false;
}

bool RSMultiDisplayFeatureParam::IsCrossDomainFeatureEnable()
{
    return crossDomainFeature_.enabled;
}

std::optional<SplitScreenParams> RSMultiDisplayFeatureParam::GetSplitScreenParams(ScreenId screenId)
{
    if (splitFeature_.params.find(screenId) != splitFeature_.params.end()) {
        return splitFeature_.params[screenId];
    } else {
        return std::nullopt;
    }
}

std::optional<InterpolationParams> RSMultiDisplayFeatureParam::GetInterpolationParams(ScreenId screenId)
{
    if (interpolationFeature_.params.find(screenId) != interpolationFeature_.params.end()) {
        return interpolationFeature_.params[screenId];
    } else {
        return std::nullopt;
    }
}

bool RSMultiDisplayFeatureParam::IsScreenInCrossDomain(ScreenId screenId)
{
    return crossDomainFeature_.params.find(screenId) != crossDomainFeature_.params.end();
}
} // namespace OHOS::Rosen
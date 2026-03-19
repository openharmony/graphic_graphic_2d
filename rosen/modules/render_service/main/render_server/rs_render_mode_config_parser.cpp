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

#include "rs_render_mode_config_parser.h"

#include <algorithm>
#include <cstdint>
#include <fstream>

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderModeConfigParser"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* RENDER_MODE_CONFIG_PATH { "etc/graphic/render_mode_config.xml" };

xmlDoc* LoadConfiguration(const char* fileDir, const std::vector<std::string>& paths, bool isSys)
{
    for (const std::string& configPath : paths) {
        std::string graphicFilePath { configPath + fileDir };
        xmlDoc* doc { xmlReadFile(graphicFilePath.c_str(), nullptr, 0) };
        if (doc != nullptr) {
            RS_LOGD("%{public}s: success to get %{public}s graphic config: %{public}s", __func__,
                isSys ? "sys" : "prod", graphicFilePath.c_str());
            return doc;
        }
    }
    RS_LOGE("%{public}s: %{public}s xmlReadFile failed", __func__, isSys ? "sys" : "prod");
    return nullptr;
}

bool IsNumber(const std::string& str)
{
    if (str.empty()) {
        return false;
    }
    auto number { static_cast<uint32_t>(
        std::count_if(str.begin(), str.end(), [](unsigned char c) { return std::isdigit(c); })) };
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}
} // namespace

RSRenderModeConfigParser::~RSRenderModeConfigParser()
{
    if (xmlDocument_ != nullptr) {
        xmlFreeDoc(xmlDocument_);
        xmlDocument_ = nullptr;
    }
}

std::shared_ptr<const RenderModeConfig> RSRenderModeConfigParser::BuildRenderConfig()
{
    RS_LOGI("%{public}s: builder init", __func__);
    if (Parse() != RENDER_MODE_PARSE_EXEC_SUCCESS) {
        RS_LOGE("%{public}s: multiprocess parse failed", __func__);
        isMultiProcessModeEnabled_ = false;
    }
    RS_LOGI("%{public}s: parse successfully", __func__);
    builder.SetIsMultiProcessModeEnabled(isMultiProcessModeEnabled_);
    RS_LOGI("%{public}s: builder set successfully", __func__);
    return builder.Build();
}

int32_t RSRenderModeConfigParser::LoadConfigurations()
{
    xmlDoc* sysDoc { LoadConfiguration(RENDER_MODE_CONFIG_PATH, sysPath_, true) };
    if (!sysDoc) {
        return RENDER_MODE_PARSE_FILE_LOAD_FAIL;
    }
    xmlDocument_ = sysDoc;

    // product config must work with system config
    xmlDoc* prodDoc { LoadConfiguration(RENDER_MODE_CONFIG_PATH, { prodPath_ }, false) };
    if (prodDoc) {
        xmlFreeDoc(xmlDocument_);
        xmlDocument_ = prodDoc;
    }
    return RENDER_MODE_PARSE_EXEC_SUCCESS;
}

int32_t RSRenderModeConfigParser::Parse()
{
    RS_LOGD("%{public}s", __func__);
    if (LoadConfigurations() != RENDER_MODE_PARSE_EXEC_SUCCESS) {
        RS_LOGE("%{public}s: LoadConfiguration failed", __func__);
        return RENDER_MODE_PARSE_FILE_LOAD_FAIL;
    }

    xmlNode* root { xmlDocGetRootElement(xmlDocument_) };
    if (!root) {
        RS_LOGE("%{public}s: xmlDocGetRootElement failed", __func__);
        return RENDER_MODE_PARSE_GET_ROOT_FAIL;
    }

    if (!ParseInternal(*root)) {
        return RENDER_MODE_PARSE_INTERNAL_FAIL;
    }

    return RENDER_MODE_PARSE_EXEC_SUCCESS;
}

bool RSRenderModeConfigParser::ParseInternal(xmlNode& node)
{
    RS_LOGD("%{public}s: parsing an internal node", __func__);
    xmlNode* currNode { &node };

    std::string valueStr { ExtractPropertyValue("value", *currNode) };
    if (!IsNumber(valueStr)) {
        RS_LOGE("%{public}s: ParseInternal failed. valueStr[%{public}s]", __func__, valueStr.c_str());
        return false;
    }
    const auto value { std::stoi(valueStr) };
    isMultiProcessModeEnabled_ = (value == 0 || value == 1) ? value : 0;

    std::string defaultGroupStr { ExtractPropertyValue("default_group", *currNode) };
    bool hasDefaultGroup = IsNumber(defaultGroupStr);
    GroupId defaultGroupId { hasDefaultGroup ? std::stoi(defaultGroupStr) : DEFAULT_RENDER_PROCESS };

    bool isFirstGroup { true };
    for (currNode = currNode->xmlChildrenNode; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("render_process"))) {
            continue;
        }

        std::string groupIdStr { ExtractPropertyValue("id", *currNode) };
        GroupId groupId {};
        if (!IsNumber(groupIdStr)) {
            groupId = defaultGroupId;
        } else {
            groupId = std::stoi(groupIdStr);
            if (isFirstGroup && !hasDefaultGroup) {
                defaultGroupId = groupId;
            }
        }

        if (ParseGroup(*currNode, groupId) != RENDER_MODE_PARSE_EXEC_SUCCESS) {
            return false;
        }

        isFirstGroup = false;
    }

    builder.SetDefaultRenderProcess(defaultGroupId);
    return true;
}

std::string RSRenderModeConfigParser::ExtractPropertyValue(const std::string& propName, xmlNode& node)
{
    RS_LOGD("%{public}s: extracting value %{public}s", __func__, propName.c_str());
    std::string propValue {};
    xmlChar* tempValue { nullptr };
    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }
    if (tempValue != nullptr) {
        RS_LOGD("%{public}s: not a empty tempValue", __func__);
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }
    return propValue;
}

int32_t RSRenderModeConfigParser::ParseGroup(xmlNode& node, GroupId groupId)
{
    RS_LOGD("%{public}s: parsing render_process", __func__);
    xmlNode* currNode { &node };

    std::string groupName { ExtractPropertyValue("name", *currNode) };
    GroupInfoConfig groupInfo { .groupName = std::move(groupName) };

    for (currNode = currNode->xmlChildrenNode; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        std::string screenName { ExtractPropertyValue("name", *currNode) };
        std::string screenIdStr { ExtractPropertyValue("id", *currNode) };
        if (!IsNumber(screenIdStr)) {
            RS_LOGE("%{public}s: screenIdStr[%{public}s] is not a valid number", __func__, screenIdStr.c_str());
            return RENDER_MODE_PARSE_ERROR;
        }
        int32_t screenId { std::stoi(screenIdStr) };
        builder.SetScreenIdToGroupId(screenId, groupId);
        ScreenInfoConfig screenInfo { std::move(screenName), screenId };
        groupInfo.screenInfos.emplace_back(std::move(screenInfo));
    }
    builder.SetGroupInfoConfigs(groupId, std::move(groupInfo));
    return RENDER_MODE_PARSE_EXEC_SUCCESS;
}
} // namespace Rosen
} // namespace OHOS
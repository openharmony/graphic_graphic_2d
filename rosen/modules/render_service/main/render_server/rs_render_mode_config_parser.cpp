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
#include <fstream>
#include <cstdint>

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderModeConfigParser"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* RENDER_MODE_CONFIG_PATH = "etc/graphic/render_mode_config.xml";

xmlDoc* LoadConfiguration(const char* fileDir, const std::vector<std::string>& paths, bool isSys)
{
    for (const std::string& configPath : paths) {
        std::string graphicFilePath = configPath + fileDir;
        xmlDoc* doc = xmlReadFile(graphicFilePath.c_str(), nullptr, 0);
        if (doc != nullptr) {
            RS_LOGD("dmulti_process %{public}s: Success to get %{public}s graphic config: %{public}s",
                    __func__, isSys ? "sys" : "prod", graphicFilePath.c_str());
            return doc;
        }
    }
    RS_LOGE("dmulti_process %{public}s: %{public}s xmlReadFile failed", __func__, isSys ? "sys" : "prod");
    return nullptr;
}

bool IsNumber(const std::string& str)
{
    auto number = static_cast<uint32_t>(std::count_if(str.begin(), str.end(), [](unsigned char c) {
        return std::isdigit(c); 
    }));
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}
}

RSRenderModeConfigParser::~RSRenderModeConfigParser()
{
    if (xmlDocument_ != nullptr) {
        xmlFreeDoc(xmlDocument_);
        xmlDocument_ = nullptr;
    }
}

std::shared_ptr<const RenderModeConfig> RSRenderModeConfigParser::BuildRenderConfig()
{
    RenderModeConfigBuilder builder;
    RS_LOGI("dmulti_process %{public}s: builder init", __func__);
    if (Parse(builder) != RENDER_MODE_PARSE_EXEC_SUCCESS) {
        RS_LOGE("dmulti_process %{public}s: render_mode_config parse failed", __func__);
        isMultiProcessModeEnabled_ = false;
    }
    RS_LOGI("dmulti_process %{public}s: parse successfully", __func__);
    builder.SetIsMultiProcessModeEnabled(isMultiProcessModeEnabled_);
    RS_LOGI("dmulti_process %{public}s: builder set successfully", __func__);
    return builder.Build();
}

int32_t RSRenderModeConfigParser::LoadConfigurations()
{
    xmlDoc* prodDoc = LoadConfiguration(RENDER_MODE_CONFIG_PATH, { prodPath_ }, false);
    xmlDoc* sysDoc = LoadConfiguration(RENDER_MODE_CONFIG_PATH, sysPath_, true);

    if (prodDoc) {
        xmlDocument_ = prodDoc;
        return sysDoc ? RENDER_MODE_PARSE_EXEC_SUCCESS : RENDER_MODE_PARSE_FILE_LOAD_FAIL;
    }
    if (sysDoc) {
        xmlDocument_ = sysDoc;
        return RENDER_MODE_PARSE_EXEC_SUCCESS;
    }
    RS_LOGE("dmulti_process %{public}s: neither production nor system config exists", __func__);
    return RENDER_MODE_PARSE_FILE_LOAD_FAIL;
}

int32_t RSRenderModeConfigParser::Parse(RenderModeConfigBuilder& builder)
{
    RS_LOGD("dmulti_process %{public}s: Parse", __func__);
    if (LoadConfigurations() != RENDER_MODE_PARSE_EXEC_SUCCESS) {
        RS_LOGE("dmulti_process %{public}s: LoadConfiguration failed", __func__);
        return RENDER_MODE_PARSE_FILE_LOAD_FAIL;
    }
    if (!xmlDocument_) {
        RS_LOGE("dmulti_process %{public}s: xmlDocument_ is empty, should do LoadConfiguration first", __func__);
        return RENDER_MODE_PARSE_FILE_LOAD_FAIL;
    }
    xmlNode* root = xmlDocGetRootElement(xmlDocument_);
    if (root == nullptr) {
        RS_LOGE("dmulti_process %{public}s: xmlDocGetRootElement failed", __func__);
        return RENDER_MODE_PARSE_GET_ROOT_FAIL;
    }

    if (!ParseInternal(*root, builder)) {
        return RENDER_MODE_PARSE_INTERNAL_FAIL;
    }

    return RENDER_MODE_PARSE_EXEC_SUCCESS;
}

bool RSRenderModeConfigParser::ParseInternal(xmlNode& node, RenderModeConfigBuilder& builder)
{
    RS_LOGD("dmulti_process %{public}s: parsing an internal node", __func__);
    xmlNode* currNode = &node;
    std::string valueStr = ExtractPropertyValue("value", *currNode);
    if (valueStr.empty() || !IsNumber(valueStr)) {
        RS_LOGE("dmulti_process %{public}s: ParseInternal failed. valueStr[%{public}s]", __func__, valueStr.c_str());
        return false;
    }
    const auto value = std::stoi(valueStr);
    isMultiProcessModeEnabled_ = (value == 0 || value == 1) ? value : 0;
    std::string defaultGroupStr = ExtractPropertyValue("default_group", *currNode);
    auto defaultGroupId = (defaultGroupStr.empty() || !IsNumber(defaultGroupStr)) ?
        DEFAULT_RENDER_PROCESS : std::stoi(defaultGroupStr);
    builder.SetDefaultRenderProcess(defaultGroupId);

    int32_t parseSuccess = RENDER_MODE_PARSE_EXEC_SUCCESS;
    for (currNode = currNode->xmlChildrenNode; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("render_process"))) {
            parseSuccess = ParseGroup(*currNode, builder, defaultGroupId);
        }
        if (parseSuccess != RENDER_MODE_PARSE_EXEC_SUCCESS) {
            return false;
        }
    }
    return true;
}

std::string RSRenderModeConfigParser::ExtractPropertyValue(const std::string& propName, xmlNode& node)
{
    RS_LOGD("dmulti_process %{public}s: extracting value %{public}s", __func__, propName.c_str());
    std::string propValue = "";
    xmlChar* tempValue = nullptr;
    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }
    if (tempValue != nullptr) {
        RS_LOGD("dmulti_process %{public}s: not a empty tempValue", __func__);
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }
    return propValue;
}

int32_t RSRenderModeConfigParser::ParseGroup(xmlNode& node, RenderModeConfigBuilder& builder, GroupId defaultGroupId)
{
    RS_LOGD("dmulti_process %{public}s: parsing render_process", __func__);
    xmlNode* currNode = &node;
    std::string groupName = ExtractPropertyValue("name", *currNode);
    if (groupName.empty()) {
        RS_LOGE("dmulti_process %{public}s: groupName is empty", __func__);
        return false;
    }
    std::string groupIdStr = ExtractPropertyValue("id", *currNode);
    GroupId groupId = (groupIdStr.empty() || !IsNumber(groupIdStr)) ?
        defaultGroupId : std::stoi(groupIdStr);

    GroupInfoConfig groupInfo = { .groupName = std::move(groupName) };
    for (currNode = currNode->xmlChildrenNode; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        std::string screenName = ExtractPropertyValue("name", *currNode);
        std::string screenIdStr = ExtractPropertyValue("id", *currNode);
        if (screenName.empty() || screenIdStr.empty() || !IsNumber(screenIdStr)) {
            RS_LOGE("dmulti_process %{public}s: screenIdStr[%{public}s] is not a valid number",
                __func__, screenIdStr.c_str());
            return RENDER_MODE_PARSE_ERROR;
        }
        int32_t screenId = std::stoi(screenIdStr);
        builder.SetScreenIdToGroupId(screenId, groupId);
        ScreenInfoConfig screenInfo = {std::move(screenName), screenId};
        groupInfo.screenInfos.emplace_back(std::move(screenInfo));
    }
    builder.SetGroupInfoConfigs(groupId, std::move(groupInfo));
    return RENDER_MODE_PARSE_EXEC_SUCCESS;
}
} // namespace Rosen
} // namespace OHOS

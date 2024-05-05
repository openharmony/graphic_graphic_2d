/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "property/rs_property_trace.h"

#include <climits>
#include <cstdarg>
#include <fstream>
#include <regex>
#include <securec.h>
#include <sys/time.h>
#include "directory_ex.h"

#include "rs_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
const std::string ANIMATION_LOG_PATH = "/etc/rosen/";
const std::string CONFIG_FILE_NAME = "property.config";
const std::string COLON_SEPARATOR = ":";
const std::string COMMA_SEPARATOR = ",";
const std::string SEMICOLON_SEPARATOR = ";";
const std::string NODE_ID_TAG = "ID";
const std::string NODE_NAME_TAG = "NODE_NAME:";
const std::string PROPERTY_ID_TAG = "PROPERTY";
const std::string ALL_NEED_TAG = "all";
const std::string CORNER_PROPERTY_TAG = "corner";
const std::string ALPHA_PROPERTY_TAG = "alpha";
const std::string SHADOW_PROPERTY_TAG = "shadow";
static constexpr int MIN_INFO_SIZE = 2;
RSPropertyTrace RSPropertyTrace::instance_;

void RSPropertyTrace::PropertiesDisplayByTrace(const NodeId& id, const RSProperties& properties)
{
    if (IsNeedPropertyTrace(id)) {
        auto rectI = (properties.GetBoundsGeometry())->GetAbsRect();
        std::string str = std::to_string(id) + " Geometry Rect: " + rectI.ToString();
        for (auto property : propertySet_) {
            if (property == CORNER_PROPERTY_TAG) {
                auto corner = properties.GetCornerRadius();
                str = str + " Corner:(" + std::to_string(corner.x_) +
                    "," + std::to_string(corner.y_) +
                    "," + std::to_string(corner.z_) +
                    "," + std::to_string(corner.w_) + ")";
            } else if (property == ALPHA_PROPERTY_TAG) {
                auto alpha = properties.GetAlpha();
                str = str + " Alpha:" + std::to_string(alpha);
            } else if (property == SHADOW_PROPERTY_TAG) {
                auto shadowColor = properties.GetShadowColor();
                auto shadowAlpha = properties.GetShadowAlpha();
                auto shadowOffsetX = properties.GetShadowOffsetX();
                auto shadowOffsetY = properties.GetShadowOffsetY();
                auto shadowRadius = properties.GetShadowRadius();
                auto shadowElevation = properties.GetShadowElevation();
                str = str + " Shadow:{color:(" + std::to_string(shadowColor.GetRed()) +
                    "," + std::to_string(shadowColor.GetGreen()) +
                    "," + std::to_string(shadowColor.GetBlue()) +
                    "),offset:(" + std::to_string(shadowOffsetX) +
                    "," + std::to_string(shadowOffsetY) +
                    "),radius:" + std::to_string(shadowRadius) +
                    ",elevation:" + std::to_string(shadowElevation) +
                    ",alpha:" + std::to_string(shadowAlpha) + "}";
            }
        }
        AddTraceFlag(str);
    }
}

void RSPropertyTrace::TracePropertiesByNodeName(const NodeId& id, const std::string& nodeName,
    const RSProperties& properties)
{
    if (!IsNeedPropertyTrace(nodeName)) {
        return;
    }
    std::string content = "NodeId:[" + std::to_string(id) + "], NodeName:[" + nodeName + "]" +
        " Geometry Rect: " + (properties.GetBoundsGeometry())->GetAbsRect().ToString() +
        " Bounds Rect: " + properties.GetBoundsRect().ToString() +
        " Frame Rect: " + properties.GetFrameRect().ToString() +
        " Alpha: " + std::to_string(properties.GetAlpha()) +
        " Visible: " + std::to_string(properties.GetVisible());
    AddTraceFlag(content);
}

void RSPropertyTrace::RefreshNodeTraceInfo()
{
    if (IsNeedRefreshConfig()) {
        ClearNodeAndPropertyInfo();
        InitNodeAndPropertyInfo();
    }
}

void RSPropertyTrace::AddTraceFlag(const std::string& str)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, str.c_str());
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

std::vector<std::string> SplitStringBySeparator(const std::string& str,
    const std::string& separator)
{
    std::regex re(separator);
    std::sregex_token_iterator pos(str.begin(), str.end(), re, -1);
    std::sregex_token_iterator endPos;
    std::vector<std::string> result;
    while (pos != endPos) {
        result.emplace_back(*pos++);
    }
    return result;
}

void RSPropertyTrace::InitNodeAndPropertyInfo()
{
    std::string configFilePath = ANIMATION_LOG_PATH + CONFIG_FILE_NAME;
    std::string newpath;
    if (!PathToRealPath(configFilePath, newpath)) {
        ROSEN_LOGE("Render node trace config file is nullptr!");
        return;
    }
    std::ifstream configFile(newpath);
    if (!configFile.is_open()) {
        ROSEN_LOGE("Open render node trace config file failed!");
        return;
    }

    std::string info;
    while (std::getline(configFile, info)) {
        if (DealNodeNameConfig(info)) {
            continue;
        }
        std::vector<std::string> infos = SplitStringBySeparator(info, SEMICOLON_SEPARATOR);
        if (infos.empty()) {
            continue;
        }
        DealConfigInputInfo(infos.front());
    }
    configFile.close();
}

void RSPropertyTrace::DealConfigInputInfo(const std::string& info)
{
    std::vector<std::string> splitResult = SplitStringBySeparator(info, COLON_SEPARATOR);
    if (splitResult.size() != MIN_INFO_SIZE) {
        ROSEN_LOGE("Render node trace config file size error!");
        return;
    }

    std::string tag = splitResult.front();
    if (tag == NODE_ID_TAG) {
        std::vector<std::string> nodeIds =
            SplitStringBySeparator(splitResult.back(), COMMA_SEPARATOR);
        for (std::string nodeId : nodeIds) {
            if (nodeId == ALL_NEED_TAG) {
                needWriteAllNode_ = true;
                return;
            }
            auto id = atoll(nodeId.c_str());
            nodeIdSet_.insert(id);
        }
    } else if (tag == PROPERTY_ID_TAG) {
        std::vector<std::string> propertys =
            SplitStringBySeparator(splitResult.back(), COMMA_SEPARATOR);
        for (std::string property : propertys) {
            propertySet_.insert(property);
        }
    }
}

bool RSPropertyTrace::DealNodeNameConfig(const std::string& info)
{
    if (info.compare(0, NODE_NAME_TAG.size(), NODE_NAME_TAG) != 0) {
        return false;
    }

    if (info == NODE_NAME_TAG) {
        needTraceAllNode_ = true;
        return true;
    }

    needTraceAllNode_ = false;
    std::vector<std::string> nodeNames = SplitStringBySeparator(info.substr(NODE_NAME_TAG.size()), COMMA_SEPARATOR);
    for (std::string nodeName : nodeNames) {
        nodeNameSet_.insert(nodeName);
    }
    return true;
}

void RSPropertyTrace::ClearNodeAndPropertyInfo()
{
    nodeIdSet_.clear();
    nodeNameSet_.clear();
    propertySet_.clear();
}

bool RSPropertyTrace::IsNeedRefreshConfig()
{
    struct stat configFileStatus = {};
    std::string configFilePath = ANIMATION_LOG_PATH + CONFIG_FILE_NAME;
    if (stat(configFilePath.c_str(), &configFileStatus)) {
        return false;
    }

    std::string curent(ctime(&configFileStatus.st_mtime));
    if (curent != propertyFileLastModifyTime) {
        propertyFileLastModifyTime = curent;
        return true;
    }
    return false;
}

bool RSPropertyTrace::IsNeedPropertyTrace(const NodeId& id)
{
    auto itrn = nodeIdSet_.find(id);
    if (itrn == nodeIdSet_.end() && !needWriteAllNode_) {
        return false;
    }
    return true;
}

bool RSPropertyTrace::IsNeedPropertyTrace(const std::string& nodeName)
{
    if (nodeName == "") {
        return false;
    }
    return needTraceAllNode_ || (nodeNameSet_.find(nodeName) != nodeNameSet_.end());
}
} // namespace Rosen
} // namespace OHOS

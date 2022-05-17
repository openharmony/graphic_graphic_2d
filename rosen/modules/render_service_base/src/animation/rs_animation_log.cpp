/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "animation/rs_animation_log.h"

#include <cstdarg>
#include <regex>
#include <securec.h>
#include <sys/time.h>

#include "render/rs_blur_filter.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
const std::string ANIMATION_LOG_PATH = "/etc/rosen/";
const std::string ANIMATION_LOG_FILE_NAME = "Animation";
const std::string ANIMATION_LOG_FILE_TYPE = ".log";
const std::string CONFIG_FILE_NAME = "property.config";
const std::string COLON_SEPARATOR = ":";
const std::string COMMA_SEPARATOR = ",";
const std::string SEMICOLON_SEPARATOR = ";";
const std::string PROPERTY_TAG = "PROPERTY";
const std::string NODE_ID_TAG = "ID";
const std::string ALL_NEED_TAG = "all";
static constexpr int MIN_INFO_SIZE = 2;
static constexpr int DATA_INDEX_ZERO = 0;
static constexpr int DATA_INDEX_ONE = 1;
static constexpr int DATA_INDEX_TWO = 2;
static constexpr int DATA_INDEX_THREE = 3;
static constexpr int DATA_INDEX_FOUR = 4;
static constexpr int DATA_INDEX_FIVE = 5;
static constexpr int DATA_INDEX_SIX = 6;
static constexpr int DATA_INDEX_SEVEN = 7;
static constexpr int DATA_INDEX_EIGHT = 8;
static constexpr int MAX_LOG_LENGTH = 2048;
static constexpr size_t LOG_FILE_MAX_SIZE = 10485760;

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

RSAnimationLog::RSAnimationLog()
{
    std::string logFilePath = ANIMATION_LOG_PATH + ANIMATION_LOG_FILE_NAME + ANIMATION_LOG_FILE_TYPE;
    PreProcessLogFile(logFilePath);
    logFile_ = std::ofstream(logFilePath, std::ios::app);
    if (!logFile_.is_open()) {
        ROSEN_LOGI("Open file error:[%s]", logFilePath.c_str());
        logFile_.close();
        return;
    }

    InitNodeAndPropertyInfo();
}

RSAnimationLog::~RSAnimationLog()
{
    logFile_.close();
}

void RSAnimationLog::PreProcessLogFile(const std::string& logFilePath)
{
    std::ifstream logFile = std::ifstream(logFilePath.c_str(), std::ios::binary);
    if (!logFile) {
        ROSEN_LOGI("Open file err:[%s]", logFilePath.c_str());
        logFile.close();
        return;
    }

    logFile.seekg(0, logFile.end);
    size_t logFileSize = logFile.tellg();
    if (logFileSize >= LOG_FILE_MAX_SIZE) {
        std::string timestampPath = ANIMATION_LOG_PATH + ANIMATION_LOG_FILE_NAME + std::to_string(GetNowTime())
            + ANIMATION_LOG_FILE_TYPE;
        std::rename(logFilePath.c_str(), timestampPath.c_str());
    }
    logFile.close();
}

void RSAnimationLog::InitNodeAndPropertyInfo()
{
    std::string configFilePath = ANIMATION_LOG_PATH + CONFIG_FILE_NAME;
    std::ifstream configFile(configFilePath.c_str());
    if (!configFile.is_open()) {
        return;
    }

    std::string info;
    while (std::getline(configFile, info)) {
        std::vector<std::string> infos = SplitStringBySeparator(info, SEMICOLON_SEPARATOR);
        if (infos.empty()) {
            continue;
        }
        DealConfigInputInfo(infos.front());
    }
    configFile.close();
}

void RSAnimationLog::DealConfigInputInfo(const std::string& info)
{
    std::vector<std::string> splitResult = SplitStringBySeparator(info, COLON_SEPARATOR);
    if (splitResult.size() != MIN_INFO_SIZE) {
        return;
    }

    std::string tag = splitResult.front();
    if (tag == PROPERTY_TAG) {
        std::vector<std::string> props =
            SplitStringBySeparator(splitResult.back(), COMMA_SEPARATOR);
        for (std::string prop: props) {
            if (prop == ALL_NEED_TAG) {
                needWriteAllProperty_ = true;
                return;
            }
            RSAnimatableProperty property =
                static_cast<RSAnimatableProperty>(strtoull(prop.c_str(), NULL, 10));
            propertySet_.insert(property);
        }
    }

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
    }
}

void RSAnimationLog::ClearNodeAndPropertyInfo()
{
    propertySet_.clear();
    nodeIdSet_.clear();
}

bool RSAnimationLog::IsNeedWriteLog(const RSAnimatableProperty& property, const NodeId& id)
{
    auto itrp = propertySet_.find(property);
    if (itrp == propertySet_.end() && !needWriteAllProperty_) {
        return false;
    }

    auto itrn = nodeIdSet_.find(id);
    if (itrn == nodeIdSet_.end() && !needWriteAllNode_) {
        return false;
    }

    return true;
}

int64_t RSAnimationLog::GetNowTime()
{
    struct timeval start = {};
    gettimeofday(&start, nullptr);
    constexpr uint32_t secToUsec = 1000 * 1000;
    return static_cast<uint64_t>(start.tv_sec) * secToUsec + start.tv_usec;
}

void RSAnimationLog::WriteString(const std::string& log)
{
    if (logFile_.is_open()) {
        logFile_.write(log.c_str(), log.size());
        logFile_.flush();
    }
}

int RSAnimationLog::WriteLog(const char* format, ...)
{
    char logStr[MAX_LOG_LENGTH] = {0};
    va_list args;
    va_start(args, format);
    int ret = vsprintf_s(logStr, MAX_LOG_LENGTH, format, args);
    if (ret == -1) {
        va_end(args);
        return -1;
    }

    WriteString(logStr);
    va_end(args);
    return 0;
}

#define WRITE_LOG(format, ...)                \
    do {                                      \
        WriteLog(format, ##__VA_ARGS__);      \
    } while (0)

template<>
void RSAnimationLog::WriteAnimationValueToLog(const int& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} value:{%d}\n",
        id, GetNowTime(), property, value);
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const int& startValue, const int& endValue)
{
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%d} endValue:{%d}\n", id, GetNowTime(), property, startValue, endValue);
}

template<>
void RSAnimationLog::WriteAnimationValueToLog(const float& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} value:{%f}\n",
        id, GetNowTime(), property, value);
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const float& startValue, const float& endValue)
{
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%f} endValue:{%f}\n", id, GetNowTime(), property, startValue, endValue);
}

template<>
void RSAnimationLog::WriteAnimationValueToLog(const Color& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} " \
        "value:{%d %d %d %d}\n", id, GetNowTime(), property, value.GetRed(),
        value.GetGreen(), value.GetBlue(), value.GetAlpha());
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const Color& startValue, const Color& endValue)
{
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%d %d %d %d} endValue:{%d %d %d %d}\n", id, GetNowTime(), property,
        startValue.GetRed(), startValue.GetGreen(), startValue.GetBlue(),
        startValue.GetAlpha(), endValue.GetRed(), endValue.GetGreen(), endValue.GetBlue(),
        endValue.GetAlpha());
}

template<>
void RSAnimationLog::WriteAnimationValueToLog(const Matrix3f& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    auto data = value.GetConstData();
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} " \
        "value:{%d %d %d %d %d %d %d %d %d}\n", id, GetNowTime(), property,
        data[DATA_INDEX_ZERO], data[DATA_INDEX_ONE], data[DATA_INDEX_TWO],
        data[DATA_INDEX_THREE], data[DATA_INDEX_FOUR], data[DATA_INDEX_FIVE],
        data[DATA_INDEX_SIX], data[DATA_INDEX_SEVEN], data[DATA_INDEX_EIGHT]);
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const Matrix3f& startValue, const Matrix3f& endValue)
{
    auto startData = startValue.GetConstData();
    auto endData = endValue.GetConstData();
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%d %d %d %d %d %d %d %d %d} endValue:{%d %d %d %d %d %d %d %d %d}\n",
        id, GetNowTime(), property, startData[DATA_INDEX_ZERO], startData[DATA_INDEX_ONE],
        startData[DATA_INDEX_TWO], startData[DATA_INDEX_THREE], startData[DATA_INDEX_FOUR],
        startData[DATA_INDEX_FIVE], startData[DATA_INDEX_SIX], startData[DATA_INDEX_SEVEN],
        startData[DATA_INDEX_EIGHT], endData[DATA_INDEX_ZERO], endData[DATA_INDEX_ONE],
        endData[DATA_INDEX_TWO], endData[DATA_INDEX_THREE], endData[DATA_INDEX_FOUR],
        endData[DATA_INDEX_FIVE], endData[DATA_INDEX_SIX], endData[DATA_INDEX_SEVEN],
        endData[DATA_INDEX_EIGHT]);
}

template<>
void RSAnimationLog::WriteAnimationValueToLog(const Vector2f& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} " \
        "value:{%f %f}\n", id, GetNowTime(), property, value.x_, value.y_);
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const Vector2f& startValue, const Vector2f& endValue)
{
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%f %f} endValue:{%f %f}\n", id, GetNowTime(), property,
        startValue.x_, startValue.y_, endValue.x_, endValue.y_);
}

template<>
void RSAnimationLog::WriteAnimationValueToLog(const Vector4f& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    auto data = value;
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} " \
        "value:{%f %f %f %f}\n", id, GetNowTime(), property, data[DATA_INDEX_ZERO],
        data[DATA_INDEX_ONE], data[DATA_INDEX_TWO], data[DATA_INDEX_THREE]);
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const Vector4f& startValue, const Vector4f& endValue)
{
    auto startData = startValue;
    auto endData = endValue;
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%f %f %f %f} endValue:{%f %f %f %f}\n", id, GetNowTime(), property,
        startData[DATA_INDEX_ZERO], startData[DATA_INDEX_ONE], startData[DATA_INDEX_TWO],
        startData[DATA_INDEX_THREE], endData[DATA_INDEX_ZERO], endData[DATA_INDEX_ONE],
        endData[DATA_INDEX_TWO], endData[DATA_INDEX_THREE]);
}

template<>
void RSAnimationLog::WriteAnimationValueToLog(const Quaternion& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    auto data = value;
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} " \
        "value:{%f %f %f %f}\n", id, GetNowTime(), property, data[DATA_INDEX_ZERO],
        data[DATA_INDEX_ONE], data[DATA_INDEX_TWO], data[DATA_INDEX_THREE]);
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const Quaternion& startValue, const Quaternion& endValue)
{
    auto startData = startValue;
    auto endData = endValue;
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%f %f %f %f} endValue:{%f %f %f %f}\n", id, GetNowTime(), property,
        startData[DATA_INDEX_ZERO], startData[DATA_INDEX_ONE], startData[DATA_INDEX_TWO],
        startData[DATA_INDEX_THREE], endData[DATA_INDEX_ZERO], endData[DATA_INDEX_ONE],
        endData[DATA_INDEX_TWO], endData[DATA_INDEX_THREE]);
}

template<>
void RSAnimationLog::WriteAnimationValueToLog(const std::shared_ptr<RSFilter>& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    auto filter = std::static_pointer_cast<RSBlurFilter>(value);
    if (filter == nullptr) {
        WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} " \
            "value:{nullptr}\n", id, GetNowTime(), property);
        return;
    }
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} value:{%f %f}\n",
        id, GetNowTime(), property, filter->GetBlurRadiusX(), filter->GetBlurRadiusY());
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const std::shared_ptr<RSFilter>& startValue,
    const std::shared_ptr<RSFilter>& endValue)
{
    auto startFilter = std::static_pointer_cast<RSBlurFilter>(startValue);
    auto endFilter = std::static_pointer_cast<RSBlurFilter>(endValue);
    if (startFilter == nullptr || endFilter == nullptr) {
        WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} " \
            "startValue:{nullptr} endValue:{nullptr}\n", id, GetNowTime(), property);
        return;
    }
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%f %f} endValue:{%f %f}\n", id, GetNowTime(), property,
        startFilter->GetBlurRadiusX(), startFilter->GetBlurRadiusY(),
        endFilter->GetBlurRadiusX(), endFilter->GetBlurRadiusY());
}

template<>
void RSAnimationLog::WriteAnimationValueToLog(const Vector4<Color>& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    auto data = value;
    WRITE_LOG("RSAnimationValueLog NodeId:{%llu} time:{%lld} property:{%llu} " \
        "value:{%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d}\n", id, GetNowTime(),
        property, data[DATA_INDEX_ZERO].GetRed(), data[DATA_INDEX_ZERO].GetGreen(),
        data[DATA_INDEX_ZERO].GetBlue(), data[DATA_INDEX_ZERO].GetAlpha(),
        data[DATA_INDEX_ONE].GetRed(), data[DATA_INDEX_ONE].GetGreen(),
        data[DATA_INDEX_ONE].GetBlue(), data[DATA_INDEX_ONE].GetAlpha(),
        data[DATA_INDEX_TWO].GetRed(), data[DATA_INDEX_TWO].GetGreen(),
        data[DATA_INDEX_TWO].GetBlue(), data[DATA_INDEX_TWO].GetAlpha(),
        data[DATA_INDEX_THREE].GetRed(), data[DATA_INDEX_THREE].GetGreen(),
        data[DATA_INDEX_THREE].GetBlue(), data[DATA_INDEX_THREE].GetAlpha());
}

template<>
void RSAnimationLog::WriteAnimationInfoToLog(const RSAnimatableProperty& property,
    const AnimationId& id, const Vector4<Color>& startValue, const Vector4<Color>& endValue)
{
    auto startData = startValue;
    auto endData = endValue;
    WRITE_LOG("RSAnimationInfoLog AnimationId:{%llu} time:{%lld} property:{%llu} " \
        "startValue:{%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d} " \
        "endValue:{%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d}\n", id, GetNowTime(), property,
        startData[DATA_INDEX_ZERO].GetRed(), startData[DATA_INDEX_ZERO].GetGreen(),
        startData[DATA_INDEX_ZERO].GetBlue(), startData[DATA_INDEX_ZERO].GetAlpha(),
        startData[DATA_INDEX_ONE].GetRed(), startData[DATA_INDEX_ONE].GetGreen(),
        startData[DATA_INDEX_ONE].GetBlue(), startData[DATA_INDEX_ONE].GetAlpha(),
        startData[DATA_INDEX_TWO].GetRed(), startData[DATA_INDEX_TWO].GetGreen(),
        startData[DATA_INDEX_TWO].GetBlue(), startData[DATA_INDEX_TWO].GetAlpha(),
        startData[DATA_INDEX_THREE].GetRed(), startData[DATA_INDEX_THREE].GetGreen(),
        startData[DATA_INDEX_THREE].GetBlue(), startData[DATA_INDEX_THREE].GetAlpha(),
        endData[DATA_INDEX_ZERO].GetRed(), endData[DATA_INDEX_ZERO].GetGreen(),
        endData[DATA_INDEX_ZERO].GetBlue(), endData[DATA_INDEX_ZERO].GetAlpha(),
        endData[DATA_INDEX_ONE].GetRed(), endData[DATA_INDEX_ONE].GetGreen(),
        endData[DATA_INDEX_ONE].GetBlue(), endData[DATA_INDEX_ONE].GetAlpha(),
        endData[DATA_INDEX_TWO].GetRed(), endData[DATA_INDEX_TWO].GetGreen(),
        endData[DATA_INDEX_TWO].GetBlue(), endData[DATA_INDEX_TWO].GetAlpha(),
        endData[DATA_INDEX_THREE].GetRed(), endData[DATA_INDEX_THREE].GetGreen(),
        endData[DATA_INDEX_THREE].GetBlue(), endData[DATA_INDEX_THREE].GetAlpha());
}
} // namespace Rosen
} // namespace OHOS
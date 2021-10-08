/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include <window_manager_type.h>

#include "driver.h"
#include "layout_header.h"

using namespace OHOS;
using namespace OHOS::WMServer;

namespace {
WindowMode WindowModeParser(const std::string &value)
{
    if (value == "unset") {
        return WINDOW_MODE_UNSET;
    } else if (value == "free") {
        return WINDOW_MODE_FREE;
    } else if (value == "full") {
        return WINDOW_MODE_FULL;
    }
    std::cerr << "invalid window mode (" << value << ")" << std::endl;
    return WINDOW_MODE_UNSET;
}
} // namespace

using AttributeProcessFunction = std::function<int32_t(const std::string &value, struct Layout &layout)>;
class Main {
public:
    Main() = default;
    ~Main() = default;

    int32_t Compile(std::ifstream &input);
    int32_t GenerateBinary(std::ofstream &output);
    static void RegAttrProcessFunc(const char *attr, AttributeProcessFunction func);

private:
    void ParseSCSS();
    void ParseAttr(const struct Driver::CSSBlock &block, struct Layout &layout);

    Driver driver;
    std::map<uint32_t, struct Layout> modeLayoutMap[WINDOW_MODE_MAX];
    static inline std::map<std::string, AttributeProcessFunction> attrProcessFuncs;
};

int32_t Main::Compile(std::ifstream &input)
{
    int ret = driver.parse(input);
    if (ret != 0) {
        std::cerr << "parse failed" << std::endl;
        return ret;
    }

    ParseSCSS();
    return 0;
}

int32_t Main::GenerateBinary(std::ofstream &output)
{
    int32_t size = 0;
    for (int32_t mode = 0; mode < WINDOW_MODE_MAX; mode++) {
        size += modeLayoutMap[mode].size();
    }
    output << size << std::endl;

    for (int32_t mode = 0; mode < WINDOW_MODE_MAX; mode++) {
        for (const auto &[type, layout] : modeLayoutMap[mode]) {
            output << mode << " " << type << " " << layout << std::endl;
        }
    }
    return 0;
}

void Main::ParseSCSS()
{
    for (const auto &[typeString, typeBlock] : driver.global.blocks) {
        auto it = windowTypeStrs.find(typeString);
        if (it == windowTypeStrs.end()) {
            std::cerr << "unknown type: " << typeString << std::endl;
            continue;
        }

        auto type = it->second;
        ParseAttr(typeBlock, modeLayoutMap[WINDOW_MODE_UNSET][type]);
        for (const auto &[modeString, modeBlock] : typeBlock.blocks) {
            WindowMode mode = WindowModeParser(modeString);
            auto modeWindow = modeLayoutMap[mode].find(type);
            if (modeWindow == modeLayoutMap[mode].end()) {
                modeLayoutMap[mode][type] = modeLayoutMap[WINDOW_MODE_UNSET][type];
            }
            ParseAttr(modeBlock, modeLayoutMap[mode][type]);
        }
    }
}

void Main::ParseAttr(const struct Driver::CSSBlock &block, struct Layout &layout)
{
    for (const auto &[attr, value] : block.declares) {
        std::string attribute = attr;
        if (attribute.find("-") != std::string::npos) {
            attribute.replace(attribute.find("-"), 1, "");
        }
        if (attribute.find("_") != std::string::npos) {
            attribute.replace(attribute.find("_"), 1, "");
        }
        auto it = attrProcessFuncs.find(attribute);
        if (it == attrProcessFuncs.end()) {
            std::cerr << "cannot process attribute: " << attribute << std::endl;
            continue;
        }

        auto ret = it->second(value, layout);
        if (ret != 0) {
            std::cerr << "value (" << attribute << ": " << value << ") return " << ret << std::endl;
        }
    }
}

void Main::RegAttrProcessFunc(const char *attr, AttributeProcessFunction func)
{
    attrProcessFuncs[attr] = func;
}

namespace {
bool IntegerParser(const std::string &value, int32_t &retval)
{
    std::regex numberRegex{"([0-9]+)"};
    std::smatch result;
    if (std::regex_match(value, result, numberRegex)) {
        std::stringstream ss;
        ss << result[1];
        ss >> retval;
        return true;
    }
    std::cerr << "invalid number (" << value << ")" << std::endl;
    return false;
}

bool NumberUnitParser(const std::string &value, double &retval)
{
    std::regex numberUnitRegex{"([0-9]+(\\.[0-9]+)?)%"};
    std::smatch result;
    if (std::regex_match(value, result, numberUnitRegex)) {
        std::stringstream ss;
        ss << result[1];
        ss >> retval;
    }
    return true;
}

bool PositionTypeParser(const std::string &value, Layout::PositionType &retval)
{
    if (value == "relative") {
        retval = Layout::PositionType::RELATIVE;
        return true;
    } else if (value == "static") {
        retval = Layout::PositionType::STATIC;
        return true;
    } else if (value == "fixed") {
        retval = Layout::PositionType::FIXED;
        return true;
    }
    std::cerr << "invalid position type (" << value << ")" << std::endl;
    return false;
}

bool XPositionTypeParser(const std::string &value, Layout::XPositionType &retval)
{
    if (value == "left") {
        retval = Layout::XPositionType::LFT;
        return true;
    } else if (value == "middle") {
        retval = Layout::XPositionType::MID;
        return true;
    } else if (value == "right") {
        retval = Layout::XPositionType::RGH;
        return true;
    } else if (value == "unset") {
        retval = Layout::XPositionType::UNSET;
        return true;
    }
    std::cerr << "invalid X position type (" << value << ")" << std::endl;
    return false;
}

bool YPositionTypeParser(const std::string &value, Layout::YPositionType &retval)
{
    if (value == "top") {
        retval = Layout::YPositionType::TOP;
        return true;
    } else if (value == "middle") {
        retval = Layout::YPositionType::MID;
        return true;
    } else if (value == "bottom") {
        retval = Layout::YPositionType::BTM;
        return true;
    } else if (value == "unset") {
        retval = Layout::YPositionType::UNSET;
        return true;
    }
    std::cerr << "invalid Y position type (" << value << ")" << std::endl;
    return false;
}

#define DEFINE_ATTRIBUTE_PROCESS_FUNCTION(attr, value, layout)\
int32_t AttributeProcess##attr(const std::string &value, \
                               struct Layout &layout); \
__attribute__((constructor)) void RegisterAttributeProcess##attr() \
{ \
    Main::RegAttrProcessFunc(#attr, AttributeProcess##attr); \
} \
 \
int32_t AttributeProcess##attr(const std::string &value, \
                               struct Layout &layout)

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(zindex, value, layout)
{
    if (IntegerParser(value, layout.zIndex)) {
        layout.isZIndexSetting = true;
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(position, value, layout)
{
    if (PositionTypeParser(value, layout.positionType)) {
        layout.isPositionTypeSetting = true;
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(horizonalign, value, layout)
{
    if (XPositionTypeParser(value, layout.pTypeX)) {
        layout.isPositionXTypeSetting = true;
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(verticalalign, value, layout)
{
    if (YPositionTypeParser(value, layout.pTypeY)) {
        layout.isPositionYTypeSetting = true;
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(left, value, layout)
{
    if (NumberUnitParser(value, layout.layout.x)) {
        layout.isLayoutSetting = true;
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(top, value, layout)
{
    if (NumberUnitParser(value, layout.layout.y)) {
        layout.isLayoutSetting = true;
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(width, value, layout)
{
    if (NumberUnitParser(value, layout.layout.w)) {
        layout.isLayoutSetting = true;
        return 0;
    }
    return 1;
}

DEFINE_ATTRIBUTE_PROCESS_FUNCTION(height, value, layout)
{
    if (NumberUnitParser(value, layout.layout.h)) {
        layout.isLayoutSetting = true;
        return 0;
    }
    return 1;
}
} // namespace

int main(int argc, const char *argv[])
{
    constexpr int32_t Args = 2;
    if (argc <= Args) {
        std::cerr << "invalid param" << std::endl;
        return -1;
    }

    constexpr int32_t inputFileArgIndex = 1;
    std::ifstream ifs{argv[inputFileArgIndex]};
    if (ifs.fail()) {
        std::cerr << "open file " << argv[inputFileArgIndex] << "failed with "
            << errno << ": " << strerror(errno) << std::endl;
        return -errno;
    }

    constexpr int32_t outputFileArgIndex = 2;
    std::ofstream ofs{argv[outputFileArgIndex]};
    if (ofs.fail()) {
        std::cerr << "open file " << argv[outputFileArgIndex] << "failed with "
            << errno << ": " << strerror(errno) << std::endl;
        return -errno;
    }

    Main main;
    int32_t ret = main.Compile(ifs);
    if (ret != 0) {
        return ret;
    }

    ret = main.GenerateBinary(ofs);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

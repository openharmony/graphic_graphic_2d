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

#ifndef HGM_TEST_BASE_H
#define HGM_TEST_BASE_H

#include <gtest/gtest.h>
#include <test_header.h>

#include "hgm_core.h"
#include "xml_parser.h"

namespace OHOS {
namespace Rosen {
const std::string TEST_XML_CONTENT = (R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <!-- Copyright (c) 2025 Device Co., Ltd.
        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at

            http://www.apache.org/licenses/LICENSE-2.0

        Unless required by applicable law or agreed to in writing, software
        distributed under the License is distributed on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        See the License for the specific language governing permissions and
        limitations under the License.
    -->
    <HgmConfig version="1.0" xmlns:xi="http://www.w3.org/2001/XInclude">
    <Param name="default_refreshrate_mode" value="-1"/>
    <Param name="p3_node_count_config" value="1"/>
    <Param name="is_covered_surface_close_p3_config" value="1"/>
    <Params name="refresh_rate_4settings">
        <Setting name="-1" id="-1"/>
        <Setting name="60" id="1"/>
        <Setting name="120" id="2"/>
    </Params>
    <Params name="additional_touch_rate_config">
        <Strategy name="aaa"/>
    </Params>
    <Params name="refreshRate_strategy_config">
        <Strategy name="8" min="60" max="120" dynamicMode="1"/>
        <Strategy name="12" min="120" max="120" dynamicMode="0"/>
    </Params>
    <Params name="refreshRate_virtual_display_config" switch="1">
    </Params>
    <Params name="safe_vote" switch="1">
    </Params>
    <Params name="screen_strategy_config">
        <Strategy name="screen0_LTPO" type="LTPO-DEFAULT"/>
    </Params>
    <Params name="screen_config" type="LTPO-DEFAULT">
        <Category name="supported_mode">
        <Mode name="aaa" value="1 2 3"/>
        <Mode name="bbb" value="1 2"/>
        </Category>
        <Setting id="-1" strategy="8">
        <Category name="LTPO_config">
            <Ltpo name="switch" value="1"/>
            <Ltpo name="maxTE" value="360"/>
            <Ltpo name="alignRate" value="100"/>
            <Ltpo name="pipelineOffsetPulseNum" value="100"/>
            <Ltpo name="adaptiveSync" value="0"/>
            <Ltpo name="vBlankIdleCorrectSwitch" value="100"/>
            <Ltpo name="lowRateToHighQuickSwitch" value="100"/>
            <Ltpo name="xxx" value="100"/>
        </Category>
        <Category name="rs_animation_power_config">
            <Setting name="aaa" value="60"/>
        </Category>
        <Category name="ui_power_config">
            <Setting name="aaa" value="60"/>
        </Category>
        <Category name="component_power_config">
            <Setting name="aaa" value="60"/>
        </Category>
        <Category name="property_animation_dynamic_settings">
            <DynamicSettings name="aaa">
            <SpeedThresholds name="1" min="0" max="-1" preferred_fps="60"/>
            </DynamicSettings>
        </Category>
        <Category name="ace_scene_dynamic_settings">
            <DynamicSettings name="aaa">
            <SpeedThresholds name="1" min="0" max="-1" preferred_fps="120"/>
            </DynamicSettings>
        </Category>
        <Category name="small_size_property_animation_dynamic_settings" area="100" length="100">
            <DynamicSettings name="aaa">
            <SpeedThresholds name="1" min="0" max="-1" preferred_fps="60"/>
            </DynamicSettings>
        </Category>
        <Category name="scene_list">
            <Scene name="aaa" strategy="12" priority="1"/>
        </Category>
        <Category name="game_scene_list">
            <Scene name="aaa" value="1"/>
        </Category>
        <Category name="anco_scene_list">
            <Scene name="aaa" value="1"/>
        </Category>
        <Category name="app_list" multi_app_strategy="focus">
            <App name="aaa" strategy="12"/>
        </Category>
        <Category name="app_types">
            <App name="aaa" strategy="12" />
        </Category>
        <Category name="app_page_url_config">
            <App name="aaa" strategy="12" />
        </Category>
        <Category name="performance_config">
            <Ltpo name="pluseNum" value="1" />
            <Ltpo name="pipelineDelayModeEnable" value="1" />
        </Category>
        <Category name="xxx">
            <App name="aaa" strategy="12" />
        </Category>
        </Setting>
    </Params>
    <Params name="rs_video_frame_rate_vote_config" switch="1">
        <App name="aaa" value="1"/>
    </Params>
    <Params name="source_tuning_for_yuv420">
        <App name="aaa" value="1"/>
    </Params>
    <Params name="rs_solid_color_layer_config">
        <App name="aaa" value="1"/>
    </Params>
    <Params name="hfbc_config">
        <App name="aaa" value="1"/>
    </Params>
    <Params name="timeout_strategy_config">
        <App name="aaa" value="1"/>
    </Params>
    <Params name="video_call_layer_config">
        <App name="aaa" value="1"/>
    </Params>
    <Params name="xxx">
        <App name="aaa" value="1"/>
    </Params>
    <Params name="vrate_control_config">
        <Vrate name="aaa" value="1"/>
    </Params>
    </HgmConfig>)");

inline xmlDoc* StringToXmlDoc(const std::string& xmlContent)
{
    xmlDoc* docPtr = xmlReadMemory(xmlContent.c_str(), xmlContent.size(), nullptr, nullptr, 0);
    return docPtr;
}

class HgmTestBase : public testing::Test {
public:
    ~HgmTestBase() = default;

    static void SetUpTestCase()
    {
        auto& hgmCore = HgmCore::Instance();
        std::unique_ptr<XMLParser> parser = std::make_unique<XMLParser>();
        parser->mParsedData_ = std::make_unique<PolicyConfigData>();
        ASSERT_NE(parser->mParsedData_, nullptr);
        parser->xmlDocument_ = StringToXmlDoc(TEST_XML_CONTENT);
        ASSERT_NE(parser->xmlDocument_, nullptr);
        parser->Parse();
        hgmCore.mPolicyConfigData_ = parser->GetParsedData();
        hgmCore.mParser_ = std::move(parser);
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_TEST_BASE_H

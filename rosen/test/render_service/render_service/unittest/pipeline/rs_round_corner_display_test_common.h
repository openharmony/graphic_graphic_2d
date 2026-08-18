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

#ifndef GRAPHIC_2D_ROSEN_TEST_RENDER_SERVICE_UNITTEST_PIPELINE_RS_ROUND_CORNER_DISPLAY_TEST_COMMON_H
#define GRAPHIC_2D_ROSEN_TEST_RENDER_SERVICE_UNITTEST_PIPELINE_RS_ROUND_CORNER_DISPLAY_TEST_COMMON_H

#include <filesystem>
#include "gtest/gtest.h"
#include "common/rs_singleton.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/round_corner_display/rs_message_bus.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/round_corner_display/rs_round_corner_config.h"
#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_bitmap_utils.h"
#include "feature/round_corner_display/rs_rcd_render_listener.h"
#include "feature/round_corner_display/rs_rcd_render_visitor.h"
#include "pipeline/rs_screen_render_node.h"
#include "rcd/rs_render_rcd_draw.h"
#include "render/rs_pixel_map_util.h"
#include "rs_render_surface_rcd_layer.h"
#include "engine/rs_uni_render_engine.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "surface_buffer_impl.h"
#include "rs_test_util.h"

namespace OHOS::Rosen {
class RSRoundCornerDisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
};

struct XMLProperty {
    std::string name;
    std::string value;
};

xmlNodePtr CreateNodeWithProperty(const std::string& nodeName, const XMLProperty& property);
xmlNodePtr CreateNodeWithProperties(const std::string& nodeName, const std::vector<XMLProperty>& properties);
xmlNodePtr CreateRCDLayer(const std::string& nodeName, const rs_rcd::RoundCornerLayer& layer);
bool LoadBitmapFromFile(const char* path, Drawing::Bitmap& bitmap);
void InitRcdRenderParamsInvalid01(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo);
void InitRcdRenderParamsInvalid02(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo);
void InitRcdRenderParamsInvalid03(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo);
void InitRcdRenderParamsInvalid04(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo);
} // OHOS::Rosen
#endif // GRAPHIC_2D_ROSEN_TEST_RENDER_SERVICE_UNITTEST_PIPELINE_RS_ROUND_CORNER_DISPLAY_TEST_COMMON_H

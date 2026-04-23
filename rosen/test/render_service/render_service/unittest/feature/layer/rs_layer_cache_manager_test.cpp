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

#include "feature/layer/rs_layer_cache_manager.h"

#include "gtest/gtest.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_canvas_drawing_render_node.h"

#include "pipeline/rs_context.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "render/rs_filter.h"
#include "skia_adapter/skia_canvas.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
constexpr NodeId DEFAULT_ID = 10086;

class RSLayerCacheManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSLayerCacheManagerTest::SetUpTestCase() {}
void RSLayerCacheManagerTest::TearDownTestCase() {}
void RSLayerCacheManagerTest::SetUp() {}
void RSLayerCacheManagerTest::TearDown() {}


} // namespace Rosen
} // namespace OHOS
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

#include "app_mgr_client.h"
#include <memory>
#include <parameters.h>

#include "gtest/gtest.h"
#include "limit_number.h"

#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "render_thread/rs_render_thread_stats.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_system_properties.h"
#include "ui/rs_surface_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "property/rs_properties.h"
#include "common/rs_obj_abs_geometry.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_gl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderThreadStatsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderThreadStatsTest::SetUpTestCase() {}
void RSRenderThreadStatsTest::TearDownTestCase() {}
void RSRenderThreadStatsTest::SetUp() {}
void RSRenderThreadStatsTest::TearDown() {}

/**
 * @tc.name: RenderFitToString
 * @tc.desc: test RenderFitToString
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, RenderFitToString, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    EXPECT_EQ(instance.RenderFitToString(Gravity::CENTER), "CENTER");
    EXPECT_EQ(instance.RenderFitToString(Gravity::TOP), "TOP");
    EXPECT_EQ(instance.RenderFitToString(Gravity::BOTTOM), "BOTTOM");
    EXPECT_EQ(instance.RenderFitToString(Gravity::LEFT), "LEFT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::RIGHT), "RIGHT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::TOP_LEFT), "TOP_LEFT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::TOP_RIGHT), "TOP_RIGHT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::BOTTOM_LEFT), "BOTTOM_LEFT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::BOTTOM_RIGHT), "BOTTOM_RIGHT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::RESIZE), "RESIZE");
    EXPECT_EQ(instance.RenderFitToString(Gravity::RESIZE_ASPECT), "RESIZE_ASPECT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::RESIZE_ASPECT_TOP_LEFT), "RESIZE_ASPECT_TOP_LEFT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT), "RESIZE_ASPECT_BOTTOM_RIGHT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::RESIZE_ASPECT_FILL), "RESIZE_ASPECT_FILL");
    EXPECT_EQ(instance.RenderFitToString(Gravity::RESIZE_ASPECT_FILL_TOP_LEFT), "RESIZE_ASPECT_FILL_TOP_LEFT");
    EXPECT_EQ(instance.RenderFitToString(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT), "RESIZE_ASPECT_FILL_BOTTOM_RIGHT");
    EXPECT_EQ(instance.RenderFitToString(static_cast<Gravity>(-1)), "Unknown");
}

/**
 * @tc.name: IsRenderFitInfoFull
 * @tc.desc: test IsRenderFitInfoFull
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, IsRenderFitInfoFull, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    for (int i = 0; i < RSRenderThreadStats::RENDERFIT_INFO_MAP_LIMIT; ++i) {
        info["package" + std::to_string(i)].insert(Gravity::CENTER);
    }
    EXPECT_TRUE(instance.IsRenderFitInfoFull(info));
    info.clear();
    for (int i = 0; i < RSRenderThreadStats::RENDERFIT_INFO_MAP_LIMIT - 1; ++i) {
        info["package" + std::to_string(i)].insert(Gravity::CENTER);
    }
    EXPECT_FALSE(instance.IsRenderFitInfoFull(info));
}

/**
 * @tc.name: AddRenderFitInfo
 * @tc.desc: test AddRenderFitInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, AddRenderFitInfo, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;
    instance.AddRenderFitInfo(info, recentUpdateInfo, "testPackage", Gravity::CENTER);
    EXPECT_EQ(info.size(), 1);
    EXPECT_EQ(recentUpdateInfo.size(), 1);
    EXPECT_TRUE(recentUpdateInfo["testPackage"]);
    instance.AddRenderFitInfo(info, recentUpdateInfo, "testPackage", Gravity::TOP);
    EXPECT_EQ(info.size(), 1);
    EXPECT_EQ(info["testPackage"].size(), 2);
    EXPECT_TRUE(recentUpdateInfo["testPackage"]);
    instance.AddRenderFitInfo(info, recentUpdateInfo, "testPackage", Gravity::CENTER);
    EXPECT_EQ(info.size(), 1);
    EXPECT_EQ(info["testPackage"].size(), 2);
    EXPECT_TRUE(recentUpdateInfo["testPackage"]);
}

/**
 * @tc.name: ReportRenderFitInfo001
 * @tc.desc: test ReportRenderFitInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ReportRenderFitInfo001, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;

    info["testPackage"].insert(Gravity::CENTER);
    recentUpdateInfo["testPackage"] = true;
    instance.rsRenderFitRenderThreadInfo_ = info;
    instance.recentUpdatedRenderFitRenderThreadInfo_ = recentUpdateInfo;
    instance.ReportRenderFitInfo();
    EXPECT_FALSE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: ReportRenderFitInfo002
 * @tc.desc: test ReportRenderFitInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ReportRenderFitInfo002, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    instance.ClearNodeRenderFitInfo();
    instance.ClearRecentUpdatedRenderFitInfo();
    instance.ReportRenderFitInfo();
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: ReportRenderFitInfo003
 * @tc.desc: test ReportRenderFitInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ReportRenderFitInfo003, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;

    for (int i = 0; i < RSRenderThreadStats::RENDERFIT_INFO_MAP_LIMIT; ++i) {
        info["package" + std::to_string(i)].insert(Gravity::CENTER);
        recentUpdateInfo["package" + std::to_string(i)] = true;
    }
    instance.rsRenderFitRenderThreadInfo_ = info;
    instance.recentUpdatedRenderFitRenderThreadInfo_ = recentUpdateInfo;
    instance.ReportRenderFitInfo();
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: ConvertInfoToString001
 * @tc.desc: test ConvertInfoToString
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ConvertInfoToString001, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;
    std::string result = instance.ConvertInfoToString(info, recentUpdateInfo);
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: ConvertInfoToString002
 * @tc.desc: test ConvertInfoToString
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ConvertInfoToString002, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;
    info["testPackage"].insert(Gravity::CENTER);
    std::string result = instance.ConvertInfoToString(info, recentUpdateInfo);
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: ConvertInfoToString003
 * @tc.desc: test ConvertInfoToString
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ConvertInfoToString003, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;
    info["testPackage"].insert(Gravity::CENTER);
    recentUpdateInfo["testPackage"] = true;
    std::string result = instance.ConvertInfoToString(info, recentUpdateInfo);
    EXPECT_EQ(result, "packageName:testPackage renderFit:CENTER");
}

/**
 * @tc.name: ConvertInfoToString004
 * @tc.desc: test ConvertInfoToString
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ConvertInfoToString004, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;
    info["testPackage"].insert(Gravity::CENTER);
    info["testPackage"].insert(Gravity::TOP);
    recentUpdateInfo["testPackage"] = true;
    std::string result = instance.ConvertInfoToString(info, recentUpdateInfo);
    EXPECT_EQ(result, "packageName:testPackage renderFit:CENTER,renderFit:TOP");
}

/**
 * @tc.name: ConvertInfoToString005
 * @tc.desc: test ConvertInfoToString
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ConvertInfoToString005, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;
    info["testPackage"].insert(Gravity::CENTER);
    recentUpdateInfo["testPackage"] = false;
    std::string result = instance.ConvertInfoToString(info, recentUpdateInfo);
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: ConvertInfoToString006
 * @tc.desc: test ConvertInfoToString
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ConvertInfoToString006, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;
    info["package1"].insert(Gravity::CENTER);
    info["package2"].insert(Gravity::TOP);
    recentUpdateInfo["package1"] = true;
    std::string result = instance.ConvertInfoToString(info, recentUpdateInfo);
    EXPECT_EQ(result, "packageName:package1 renderFit:CENTER");
}

/**
 * @tc.name: ConvertInfoToString007
 * @tc.desc: test ConvertInfoToString
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ConvertInfoToString007, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSRenderFitInfo info;
    RSRecentUpdatedRenderFitInfo recentUpdateInfo;
    info["package1"].insert(Gravity::CENTER);
    info["package2"].insert(Gravity::TOP);
    recentUpdateInfo["package1"] = true;
    recentUpdateInfo["package2"] = true;
    std::string result = instance.ConvertInfoToString(info, recentUpdateInfo);
    EXPECT_EQ(result, "packageName:package1 renderFit:CENTER; packageName:package2 renderFit:TOP");
}

/**
 * @tc.name: ClearNodeRenderFitInfo
 * @tc.desc: test ClearNodeRenderFitInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ClearNodeRenderFitInfo, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    instance.rsRenderFitRenderThreadInfo_["testPackage"].insert(Gravity::CENTER);
    EXPECT_FALSE(instance.rsRenderFitRenderThreadInfo_.empty());
    instance.ClearNodeRenderFitInfo();
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: ClearRecentUpdatedRenderFitInfo
 * @tc.desc: test ClearRecentUpdatedRenderFitInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, ClearRecentUpdatedRenderFitInfo, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    instance.recentUpdatedRenderFitRenderThreadInfo_["testPackage"] = true;
    EXPECT_FALSE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
    instance.ClearRecentUpdatedRenderFitInfo();
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: AddStaticInfo001
 * @tc.desc: test AddStaticInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, AddStaticInfo001, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSProperties property;
    property.SetBoundsWidth(100.0f);
    property.SetBoundsHeight(200.0f);
    property.SetFrameGravity(Gravity::CENTER);
    float frameWidth = 100.0f;
    float frameHeight = 200.0f;
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;

    instance.AddStaticInfo(frameWidth, frameHeight, property, scalingMode);
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: AddStaticInfo002
 * @tc.desc: test AddStaticInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, AddStaticInfo002, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSProperties property;
    property.SetBoundsWidth(200.0f);
    property.SetBoundsHeight(100.0f);
    property.SetFrameGravity(Gravity::CENTER);
    float frameWidth = 100.0f;
    float frameHeight = 200.0f;
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;
    instance.AddStaticInfo(frameWidth, frameHeight, property, scalingMode);
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: AddStaticInfo003
 * @tc.desc: test AddStaticInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, AddStaticInfo003, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSProperties property;
    property.SetBoundsWidth(200.0f);
    property.SetBoundsHeight(200.0f);
    property.SetFrameGravity(Gravity::CENTER);
    float frameWidth = 100.0f;
    float frameHeight = 200.0f;
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;
    instance.AddStaticInfo(frameWidth, frameHeight, property, scalingMode);
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: AddStaticInfo004
 * @tc.desc: test AddStaticInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, AddStaticInfo004, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSProperties property;
    property.SetBoundsWidth(200.0f);
    property.SetBoundsHeight(200.0f);
    property.SetFrameGravity(Gravity::CENTER);
    float frameWidth = 200.0f;
    float frameHeight = 100.0f;
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;
    instance.AddStaticInfo(frameWidth, frameHeight, property, scalingMode);
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: AddStaticInfo005
 * @tc.desc: test AddStaticInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, AddStaticInfo005, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSProperties property;
    property.SetBoundsWidth(200.0f);
    property.SetBoundsHeight(100.0f);
    property.SetFrameGravity(Gravity::CENTER);
    float frameWidth = 100.0f;
    float frameHeight = 200.0f;
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;
    instance.AddStaticInfo(frameWidth, frameHeight, property, scalingMode);
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: AddStaticInfo006
 * @tc.desc: test AddStaticInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, AddStaticInfo006, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSProperties property;
    property.SetBoundsWidth(200.0f);
    property.SetBoundsHeight(100.0f);
    property.SetFrameGravity(Gravity::RESIZE);
    float frameWidth = 100.0f;
    float frameHeight = 200.0f;
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;
    instance.AddStaticInfo(frameWidth, frameHeight, property, scalingMode);
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}

/**
 * @tc.name: AddStaticInfo007
 * @tc.desc: test AddStaticInfo
 * @tc.type:FUNC
 * @tc.require: issueICUJEY
 */
HWTEST_F(RSRenderThreadStatsTest, AddStaticInfo007, TestSize.Level1)
{
    RSRenderThreadStats& instance = RSRenderThreadStats::GetInstance();
    RSProperties property;
    property.SetBoundsWidth(200.0f);
    property.SetBoundsHeight(100.0f);
    property.SetFrameGravity(Gravity::RESIZE);
    float frameWidth = 100.0f;
    float frameHeight = 200.0f;
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_CROP;
    instance.AddStaticInfo(frameWidth, frameHeight, property, scalingMode);
    EXPECT_TRUE(instance.rsRenderFitRenderThreadInfo_.empty());
    EXPECT_TRUE(instance.recentUpdatedRenderFitRenderThreadInfo_.empty());
}
} // namespace OHOS::Rosen
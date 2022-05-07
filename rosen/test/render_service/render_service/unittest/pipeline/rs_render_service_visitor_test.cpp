/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/rs_render_service_visitor.h"

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceVisitorTest::SetUpTestCase() {}
void RSRenderServiceVisitorTest::TearDownTestCase() {}
void RSRenderServiceVisitorTest::SetUp() {}
void RSRenderServiceVisitorTest::TearDown() {}

/*
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, CreateAndDestory001, TestSize.Level1)
{
    RSRenderServiceVisitor rsRenderServiceVisitor;
}

/**
 * @tc.name: PrepareBaseRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareBaseRenderNode001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    RSRenderServiceVisitor rsRenderServiceVisitor;
    rsRenderServiceVisitor.PrepareBaseRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareDisplayRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareDisplayRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(node);
}

/**
 * @tc.name: PrepareDisplayRenderNode002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareDisplayRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(node);
}

/**
 * @tc.name: PrepareDisplayRenderNode003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareDisplayRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(node);
}

/**
 * @tc.name: PrepareDisplayRenderNode004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareDisplayRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(node);
}

/**
 * @tc.name: PrepareDisplayRenderNode005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareDisplayRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(node);
}

/**
 * @tc.name: PrepareDisplayRenderNode006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareDisplayRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(node);
}

/**
 * @tc.name: PrepareDisplayRenderNode007
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareDisplayRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(node);
}

/**
 * @tc.name: PrepareDisplayRenderNode008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareDisplayRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(node);
}

/**
 * @tc.name: PrepareSurfaceRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareSurfaceRenderNode001, TestSize.Level1)
{
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor.PrepareSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareCanvasRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode007
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode007
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: ProcessBaseRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessBaseRenderNode001, TestSize.Level1)
{
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor.ProcessBaseRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessDisplayRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessDisplayRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(node);
}

/**
 * @tc.name: ProcessDisplayRenderNode002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessDisplayRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(node);
}

/**
 * @tc.name: ProcessDisplayRenderNode003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessDisplayRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(node);
}

/**
 * @tc.name: ProcessDisplayRenderNode004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessDisplayRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(node);
}

/**
 * @tc.name: ProcessDisplayRenderNode005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessDisplayRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(node);
}

/**
 * @tc.name: ProcessDisplayRenderNode006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessDisplayRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(node);
}

/**
 * @tc.name: ProcessDisplayRenderNode007
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessDisplayRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(node);
}

/**
 * @tc.name: ProcessDisplayRenderNode008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessDisplayRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(node);
}

/**
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessSurfaceRenderNode001, TestSize.Level1)
{
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor.ProcessSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessCanvasRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode007
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode007
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor;
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor.ProcessRootRenderNode(node);
}
} // namespace OHOS::Rosen

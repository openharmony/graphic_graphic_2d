/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "pipeline/rs_divided_render_util.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDividedRenderUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDividedRenderUtilTest::SetUpTestCase() {}
void RSDividedRenderUtilTest::TearDownTestCase() {}
void RSDividedRenderUtilTest::SetUp() {}
void RSDividedRenderUtilTest::TearDown() {}

/*
 * @tc.name: CreateBufferDrawParam001
 * @tc.desc: Test CreateBufferDrawParam
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDividedRenderUtilTest, CreateBufferDrawParam001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(config);
    Drawing::Matrix matrix  = Drawing::Matrix();
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node, true);
    ASSERT_EQ(matrix, param.matrix);

    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    node->SetTotalMatrix(matrix);
    param = RSDividedRenderUtil::CreateBufferDrawParam(*node, false);
    ASSERT_EQ(matrix, param.matrix);
}

/*
 * @tc.name: CreateBufferDrawParam002
 * @tc.desc: Test CreateBufferDrawParam
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDividedRenderUtilTest, CreateBufferDrawParam002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(config);

    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node, false, true);
    ASSERT_EQ(nullptr, param.buffer);

    param = RSDividedRenderUtil::CreateBufferDrawParam(*node, false, false);
    ASSERT_EQ(nullptr, param.buffer);

    node = RSTestUtil::CreateSurfaceNode();
    param = RSDividedRenderUtil::CreateBufferDrawParam(*node, false, false);
    ASSERT_EQ(nullptr, param.buffer);

    node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    param = RSDividedRenderUtil::CreateBufferDrawParam(*node, false, false);
    ASSERT_NE(nullptr, param.buffer);
}
}
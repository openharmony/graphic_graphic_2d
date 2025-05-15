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

#include "gtest/gtest.h"

#include "render/rs_always_snapshot_shader_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSAlwaysSnapshotShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAlwaysSnapshotShaderFilterTest::SetUpTestCase() {}
void RSAlwaysSnapshotShaderFilterTest::TearDownTestCase() {}
void RSAlwaysSnapshotShaderFilterTest::SetUp() {}
void RSAlwaysSnapshotShaderFilterTest::TearDown() {}

/**
 * @tc.name: RSAlwaysSnapshotShaderFilterTest001
 * @tc.desc: Verify function GetShaderFilterType
 * @tc.type: FUNC
 * @tc.require: issueIC5TKO
 */
HWTEST_F(RSAlwaysSnapshotShaderFilterTest, RSAlwaysSnapshotShaderFilterTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSAlwaysSnapshotShaderFilter>();
    ASSERT_NE(filter, nullptr);
    EXPECT_FALSE(filter->GetShaderFilterType(), RSShaderFilter::ShaderFilterType::ALWAYS_SNAPSHOT);
}
} // namespace Rosen
} // namespace OHOS
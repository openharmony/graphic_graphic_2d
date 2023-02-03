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

#include "gtest/gtest.h"
#include "pipeline/rs_dirty_region_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDirtyRegionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSDirtyRegionManager> manager;
};

void RSDirtyRegionManagerTest::SetUpTestCase() {}
void RSDirtyRegionManagerTest::TearDownTestCase() {}
void RSDirtyRegionManagerTest::SetUp()
{
    manager = std::make_shared<RSDirtyRegionManager>();
}
void RSDirtyRegionManagerTest::TearDown() {}

/**
 * @tc.name: GetPixelAlignedRect001
 * @tc.desc: test results of GetPixelAlignedRect
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDirtyRegionManagerTest, GetPixelAlignedRect001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. GetPixelAlignedRect
     */
    RectI rect = RectI(0, 0, 10, 10);
    int32_t alignedBits = 1;
    ASSERT_EQ(manager->GetPixelAlignedRect(rect, alignedBits), rect);

    alignedBits = 11;
    ASSERT_EQ(manager->GetPixelAlignedRect(rect, alignedBits), RectI(0, 0, 11, 11));
}

/**
 * @tc.name: SetBufferAge001
 * @tc.desc: test results of SetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDirtyRegionManagerTest, SetBufferAge001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. SetBufferAge
     */
    int age = -1;
    ASSERT_FALSE(manager->SetBufferAge(age));
    
    age = 1;
    ASSERT_TRUE(manager->SetBufferAge(age));
}

/**
 * @tc.name: SetSurfaceSize001
 * @tc.desc: test results of SetSurfaceSize
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDirtyRegionManagerTest, SetSurfaceSize001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. SetSurfaceSize
     */
    int32_t width = -1;
    int32_t height = -1;
    ASSERT_FALSE(manager->SetSurfaceSize(width, height));
    
    width = 1;
    height = 1;
    ASSERT_TRUE(manager->SetSurfaceSize(width, height));
}
} // namespace OHOS::Rosen
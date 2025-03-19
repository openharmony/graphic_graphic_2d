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

#include <gtest/gtest.h>
#include <test_header.h>

#include "dirtyregion_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
constexpr int DEFAULT_ALIGN_BITS = 128;

class DirtyRegionParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DirtyRegionParamTest::SetUpTestCase() {}
void DirtyRegionParamTest::TearDownTestCase() {}
void DirtyRegionParamTest::SetUp() {}
void DirtyRegionParamTest::TearDown() {}

/**
 * @tc.name: ParamSettingAndGettingTest
 * @tc.desc: Verify the each of those params can be correctly set and gotten.
 * @tc.type: FUNC
 * @tc.require: issueIBOSX3
 */
HWTEST_F(DirtyRegionParamTest, ParamSettingAndGettingTest, Function | SmallTest | Level1)
{
    DirtyRegionParam dirtyRegionParam;
    dirtyRegionParam.SetDirtyRegionEnable(true);
    ASSERT_TRUE(dirtyRegionParam.IsDirtyRegionEnable());
    dirtyRegionParam.SetDirtyRegionEnable(false);
    ASSERT_FALSE(dirtyRegionParam.IsDirtyRegionEnable());

    dirtyRegionParam.SetExpandScreenDirtyRegionEnable(true);
    ASSERT_TRUE(dirtyRegionParam.IsExpandScreenDirtyRegionEnable());
    dirtyRegionParam.SetExpandScreenDirtyRegionEnable(false);
    ASSERT_FALSE(dirtyRegionParam.IsExpandScreenDirtyRegionEnable());

    dirtyRegionParam.SetMirrorScreenDirtyRegionEnable(true);
    ASSERT_TRUE(dirtyRegionParam.IsMirrorScreenDirtyRegionEnable());
    dirtyRegionParam.SetMirrorScreenDirtyRegionEnable(false);
    ASSERT_FALSE(dirtyRegionParam.IsMirrorScreenDirtyRegionEnable());

    dirtyRegionParam.SetAdvancedDirtyRegionEnable(true);
    ASSERT_TRUE(dirtyRegionParam.IsAdvancedDirtyRegionEnable());
    dirtyRegionParam.SetAdvancedDirtyRegionEnable(false);
    ASSERT_FALSE(dirtyRegionParam.IsAdvancedDirtyRegionEnable());

    dirtyRegionParam.SetTileBasedAlignEnable(true);
    ASSERT_TRUE(dirtyRegionParam.IsTileBasedAlignEnable());
    dirtyRegionParam.SetTileBasedAlignEnable(false);
    ASSERT_FALSE(dirtyRegionParam.IsTileBasedAlignEnable());

    dirtyRegionParam.SetTileBasedAlignBits(DEFAULT_ALIGN_BITS);
    ASSERT_EQ(dirtyRegionParam.GetTileBasedAlignBits(), DEFAULT_ALIGN_BITS);
}
} // namespace Rosen
} // namespace OHOS
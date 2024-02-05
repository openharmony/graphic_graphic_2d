/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include <hilog/log.h>
#include <memory>
#include <unistd.h>
#include "common/rs_occlusion_region.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRegionTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0XD001400, "RSRegionTest" };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRegionTest::SetUpTestCase() {}
void RSRegionTest::TearDownTestCase() {}
void RSRegionTest::SetUp() {}
void RSRegionTest::TearDown() {}

/**
 * @tc.name: OperatorSub
 * @tc.desc: Function sub
 * @tc.type: FUNC
 * @tc.require: issuesI5PYNC
 */
HWTEST_F(RSRegionTest, OperatorSub, Function | SmallTest | Level2)
{
    Occlusion::Rect rect1 { 0, 0, 100, 100 };
    Occlusion::Region resgion1 { rect1 };
    Occlusion::Rect rect2 { 50, 50, 150, 150 };
    Occlusion::Region resgion2 { rect2 };
    Occlusion::Region sub = resgion1.Sub(resgion2);

    Occlusion::Rect res {0, 0, 100, 100};
    EXPECT_EQ(sub.GetSize(), 2);
    EXPECT_EQ(sub.GetBound(), res);
}

/**
 * @tc.name: OperatorOr
 * @tc.desc: Function or
 * @tc.type: FUNC
 * @tc.require: issuesI5PYNC
 */
HWTEST_F(RSRegionTest, OperatorOr, Function | SmallTest | Level2)
{
    Occlusion::Rect rect1 { 0, 0, 100, 100 };
    Occlusion::Region resgion1 { rect1 };
    Occlusion::Rect rect2 { 50, 50, 150, 150 };
    Occlusion::Region resgion2 { rect2 };
    Occlusion::Region resOr = resgion1.Or(resgion2);

    Occlusion::Rect res {0, 0, 150, 150};
    EXPECT_EQ(resOr.GetSize(), 3);
    EXPECT_EQ(resOr.GetBound(), res);
}

/**
 * @tc.name: OperatorAnd
 * @tc.desc: Function and
 * @tc.type: FUNC
 * @tc.require: issuesI5PYNC
 */
HWTEST_F(RSRegionTest, OperatorAnd, Function | SmallTest | Level2)
{
    Occlusion::Rect rect1 { 0, 0, 100, 100 };
    Occlusion::Region resgion1 { rect1 };
    Occlusion::Rect rect2 { 50, 50, 150, 150 };
    Occlusion::Region resgion2 { rect2 };
    Occlusion::Region resAnd = resgion1.And(resgion2);

    Occlusion::Rect res {50, 50, 100, 100};
    EXPECT_EQ(resAnd.GetSize(), 1);
    EXPECT_EQ(resAnd.GetBound(), res);
}
} // namespace OHOS::Rosen

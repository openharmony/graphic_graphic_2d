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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "texgine_path.h"

using namespace testing;
using namespace testing::ext;

struct MockVars {
    std::shared_ptr<RSPath> skPath_ = std::make_shared<RSPath>();
};

namespace {
struct MockVars g_tphMockvars;

void InitTphMockVars(struct MockVars &&vars)
{
    g_tphMockvars = std::move(vars);
}

} // namespace

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexginePathTest : public testing::Test {
};

/**
 * @tc.name:SetAndGet
 * @tc.desc: Verify the SetAndGet
 * @tc.type:FUNC
 */
HWTEST_F(TexginePathTest, SetAndGet, TestSize.Level1)
{
    std::shared_ptr<TexginePath> tp = std::make_shared<TexginePath>();
    EXPECT_NO_THROW({
        InitTphMockVars({});
        tp->SetPath(g_tphMockvars.skPath_);
        EXPECT_EQ(tp->GetPath(), g_tphMockvars.skPath_);
    });
}

/**
 * @tc.name:MoveTo
 * @tc.desc: Verify the MoveTo
 * @tc.type:FUNC
 */
HWTEST_F(TexginePathTest, MoveTo, TestSize.Level1)
{
    std::shared_ptr<TexginePath> tp = std::make_shared<TexginePath>();
    std::shared_ptr<RSPath> sp = nullptr;
    TexginePoint p = {0.0, 0.0};
    EXPECT_NO_THROW({
        tp->MoveTo(p);
        tp->SetPath(sp);
        tp->MoveTo(p);
    });
}

/**
 * @tc.name:QuadTo
 * @tc.desc: Verify the QuadTo
 * @tc.type:FUNC
 */
HWTEST_F(TexginePathTest, QuadTo, TestSize.Level1)
{
    std::shared_ptr<TexginePath> tp = std::make_shared<TexginePath>();
    std::shared_ptr<RSPath> sp = nullptr;
    TexginePoint p1 = {0.0, 0.0};
    TexginePoint p2 = {0.0, 0.0};
    EXPECT_NO_THROW({
        tp->QuadTo(p1, p2);
        tp->SetPath(sp);
        tp->QuadTo(p1, p2);
    });
}

/**
 * @tc.name:LineTo
 * @tc.desc: Verify the LineTo
 * @tc.type:FUNC
 */
HWTEST_F(TexginePathTest, LineTo, TestSize.Level1)
{
    std::shared_ptr<TexginePath> tp = std::make_shared<TexginePath>();
    std::shared_ptr<RSPath> sp = nullptr;
    TexginePoint p = {0.0, 0.0};
    EXPECT_NO_THROW({
        tp->LineTo(p);
        tp->SetPath(sp);
        tp->LineTo(p);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

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

#include <fstream>
#include "gtest/gtest.h"
#include "limit_number.h"
#include "surface.h"
#include "pipeline/rs_uni_render_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderJudgementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderJudgementTest::SetUpTestCase() {}
void RSUniRenderJudgementTest::TearDownTestCase() {}
void RSUniRenderJudgementTest::SetUp() {}
void RSUniRenderJudgementTest::TearDown() {}

/**
 * @tc.name: TestUniRenderJudgement001
 * @tc.desc: GetUniRenderEnabledType test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSUniRenderJudgementTest, TestUniRenderJudgement001, TestSize.Level1)
{
    UniRenderEnabledType uniType = RSUniRenderJudgement::GetUniRenderEnabledType();
    ASSERT_TRUE(uniType == UniRenderEnabledType::UNI_RENDER_DISABLED ||
        uniType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL ||
        uniType == UniRenderEnabledType::UNI_RENDER_DYNAMIC_SWITCH);
}


/**
 * @tc.name: TestUniRenderJudgement002
 * @tc.desc: IsUniRender test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSUniRenderJudgementTest, TestUniRenderJudgement002, TestSize.Level1)
{
    UniRenderEnabledType uniType = RSUniRenderJudgement::GetUniRenderEnabledType();
    RSUniRenderJudgement::InitUniRenderConfig();
    bool isUni = RSUniRenderJudgement::IsUniRender();
    if (uniType == UniRenderEnabledType::UNI_RENDER_DISABLED) {
        ASSERT_EQ(false, isUni);
    } else {
        ASSERT_EQ(true, isUni);
    }
}
} // namespace OHOS::Rosen

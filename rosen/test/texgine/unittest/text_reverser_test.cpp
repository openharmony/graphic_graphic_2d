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

#include "mock/mock_any_span.h"
#include "text_reverser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class ControllerForTest {
public:
    static std::shared_ptr<TextSpan> GenerateTestSpan(bool rtl)
    {
        auto ts = std::make_shared<TextSpan>();
        ts->rtl_ = rtl;
        ts->cgs_ = CharGroups::CreateEmpty();
        return ts;
    }
};

class TextReverserTest : public testing::Test {
};

/**
 * @tc.name: ReverseRTLText
 * @tc.desc: Verify the ReverseRTLText
 * @tc.type:FUNC
 */
HWTEST_F(TextReverserTest, ReverseRTLText, TestSize.Level1)
{
    std::shared_ptr<AnySpan> a1 = std::make_shared<MockAnySpan>();
    std::shared_ptr<AnySpan> a2 = std::make_shared<MockAnySpan>();
    std::shared_ptr<TextSpan> l1 = ControllerForTest::GenerateTestSpan(false);
    std::shared_ptr<TextSpan> l2 = ControllerForTest::GenerateTestSpan(false);
    std::shared_ptr<TextSpan> l3 = ControllerForTest::GenerateTestSpan(false);
    std::shared_ptr<TextSpan> l4 = ControllerForTest::GenerateTestSpan(false);
    std::shared_ptr<TextSpan> r1 = ControllerForTest::GenerateTestSpan(true);
    std::shared_ptr<TextSpan> r2 = ControllerForTest::GenerateTestSpan(true);
    std::shared_ptr<TextSpan> r3 = ControllerForTest::GenerateTestSpan(true);
    std::shared_ptr<TextSpan> r4 = ControllerForTest::GenerateTestSpan(true);
    TextReverser tr;

    // 参数设置：全部都是RTL的span
    std::vector<VariantSpan> spans1 = {r1, r2, r3, r4};
    tr.ReverseRTLText(spans1);
    ASSERT_EQ(spans1, (std::vector<VariantSpan>{r4, r3, r2, r1}));

    // 参数设置：全部都是LTR的span
    std::vector<VariantSpan> spans2 = {l1, l2, l3, l4};
    tr.ReverseRTLText(spans2);
    ASSERT_EQ(spans2, (std::vector<VariantSpan>{l1, l2, l3, l4}));

    // 参数设置：既包含LTR也包含RTL
    std::vector<VariantSpan> spans3 = {l1, a1, l2, r1, l3, a2, r2, r3, r4, l4};
    tr.ReverseRTLText(spans3);
    ASSERT_EQ(spans3, (std::vector<VariantSpan>{l1, a1, l2, r1, l3, a2, r4, r3, r2, l4}));
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

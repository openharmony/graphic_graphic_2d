/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_mask_filter.h"
#include "effect/mask_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaMaskFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaMaskFilterTest::SetUpTestCase() {}
void SkiaMaskFilterTest::TearDownTestCase() {}
void SkiaMaskFilterTest::SetUp() {}
void SkiaMaskFilterTest::TearDown() {}

/**
 * @tc.name: SkiaMaskFilter001
 * @tc.desc: Test SkiaMaskFilter's functions
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaMaskFilterTest, SkiaMaskFilter001, TestSize.Level1)
{
    std::shared_ptr<SkiaMaskFilter> skiaMaskFilter = std::make_shared<SkiaMaskFilter>();
    skiaMaskFilter->InitWithBlur(BlurType::NORMAL, 1, true); // 1: sigma
    skiaMaskFilter->Serialize();
    skiaMaskFilter->Deserialize(nullptr);
    skiaMaskFilter->SetSkMaskFilter(nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
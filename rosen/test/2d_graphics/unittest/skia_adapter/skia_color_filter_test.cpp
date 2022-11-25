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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_color_filter.h"
#include "effect/color_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaColorFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaColorFilterTest::SetUpTestCase() {}
void SkiaColorFilterTest::TearDownTestCase() {}
void SkiaColorFilterTest::SetUp() {}
void SkiaColorFilterTest::TearDown() {}

/**
 * @tc.name: InitWithCompose001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaColorFilterTest, InitWithCompose001, TestSize.Level1)
{
    auto colorFilter1 = ColorFilter::FilterType::COMPOSE;
    auto colorFilter2 = ColorFilter::FilterType::MATRIX;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithCompose(ColorFilter(colorFilter1), ColorFilter(colorFilter2));
}

/**
 * @tc.name: Compose001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaColorFilterTest, Compose001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::FilterType::COMPOSE;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.Compose(ColorFilter(colorFilter));
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
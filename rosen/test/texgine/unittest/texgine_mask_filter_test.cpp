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

#include "texgine_mask_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineMaskFilterTest : public testing::Test {
};

/**
 * @tc.name:MakeBlur
 * @tc.desc: Verify the MakeBlur
 * @tc.type:FUNC
 */
HWTEST_F(TexgineMaskFilterTest, MakeBlur, TestSize.Level1)
{
    EXPECT_NO_THROW({
        std::shared_ptr<TexgineMaskFilter> tmf = TexgineMaskFilter::MakeBlur(
            TexgineMaskFilter::TexgineBlurStyle::K_NORMAL_SK_BLUR_STYLE, 1.0, true);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

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

#include "text_style.h"

#include <gtest/gtest.h>
#include <unicode/ubidi.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class FontFeaturesTest : public testing::Test {
};

/**
 * @tc.name: SetFeature
 * @tc.desc: Verify the FontFeatures
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(FontFeaturesTest, SetFeature, TestSize.Level1)
{
    std::shared_ptr<FontFeatures> fontFeatures = std::make_shared<FontFeatures>();
    std::string ftag = "num";
    int fvalue = 1;
    EXPECT_NO_THROW({
        fontFeatures->SetFeature(ftag, fvalue);
    });
}

/**
 * @tc.name: GetFeatures
 * @tc.desc: Verify the FontFeatures
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(FontFeaturesTest, GetFeatures, TestSize.Level1)
{
    std::shared_ptr<FontFeatures> fontFeatures = std::make_shared<FontFeatures>();
    std::string ftag = "num";
    int fvalue = 1;

    EXPECT_NO_THROW({
        fontFeatures->SetFeature(ftag, fvalue);
    });

    auto features = fontFeatures->GetFeatures();
    ASSERT_EQ(features[ftag], fvalue);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

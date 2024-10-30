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

#include "text_bundle_config_parser.h"
#include "gtest/gtest.h"

#ifdef OHOS_TEXT_ENABLE
#include "application_info.h"
#include "bundle_info.h"
#include "hap_module_info.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace SPText {
class TextBundleConfigParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::string metaData;
};

void TextBundleConfigParserTest::SetUpTestCase()
{
    metaData = "text_bundle_config_parser_test";
}

void TextBundleConfigParserTest::TearDownTestCase() {}

/*
 * @tc.name: BundleManagerTest001
 * @tc.desc: test for GetAdapterTextHeight
 * @tc.type: FUNC
 */
HWTEST_F(TextBundleConfigParserTest, BundleManagerTest001, TestSize.Level1)
{
    EXPECT_FALSE(TextBundleConfigParser::IsAdapterTextHeightEnabled());
}

#ifdef OHOS_TEXT_ENABLE
/*
 * @tc.name: GetSystemAbilityManagerTest001
 * @tc.desc: test for GetSystemAbilityManager
 * @tc.type: FUNC
 */
HWTEST_F(TextBundleConfigParserTest, GetSystemAbilityManagerTest001, TestSize.Level1)
{
    EXPECT_EQ(TextBundleConfigParser::GetSystemAbilityManager(), nullptr);
}

/*
 * @tc.name: IsMetaDataExistInModuleTest001
 * @tc.desc: test for IsMetaDataExistInModule
 * @tc.type: FUNC
 */
HWTEST_F(TextBundleConfigParserTest, IsMetaDataExistInModuleTest001, TestSize.Level1)
{
    EXPECT_FALSE(TextBundleConfigParser::IsMetaDataExistInModule(metaData));
    EXPECT_FALSE(TextBundleConfigParser::IsMetaDataExistInModule(""));
}
#endif
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
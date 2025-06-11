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
#include "gtest/gtest.h"
#include "render/rs_render_filter_base.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderFilterBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderFilterBaseTest::SetUpTestCase() {}
void RSRenderFilterBaseTest::TearDownTestCase() {}
void RSRenderFilterBaseTest::SetUp() {}
void RSRenderFilterBaseTest::TearDown() {}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GetType001, TestSize.Level1)
{
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    auto uiFilterType = rsRenderFilterParaBase->GetType();
    EXPECT_EQ(uiFilterType, RSUIFilterType::BLUR);
}

/**
 * @tc.name: IsValid001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, IsValid001, TestSize.Level1)
{
    auto rsRenderFilterParaBase1 = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::NONE);
    EXPECT_FALSE(rsRenderFilterParaBase1->IsValid());
    auto rsRenderFilterParaBase2 = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    EXPECT_TRUE(rsRenderFilterParaBase2->IsValid());
}

/**
 * @tc.name: Setter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, Setter001, TestSize.Level1)
{
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    auto rsRenderPropertyBase = std::make_shared<RSRenderProperty<bool>>();
    rsRenderFilterParaBase->Setter(RSUIFilterType::BLUR, rsRenderPropertyBase);
    EXPECT_NE(rsRenderFilterParaBase->properties_.size(), 0);
}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    std::string out;
    rsRenderFilterParaBase->GetDescription(out);
    EXPECT_TRUE(out.empty());
}

/**
 * @tc.name: Dump001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, Dump001, TestSize.Level1)
{
    std::string temp = ": [";
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    std::string out;
    rsRenderFilterParaBase->Dump(out);
    EXPECT_EQ(out, temp);
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    Parcel parcel;
    auto ret =rsRenderFilterParaBase->WriteToParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    Parcel parcel;
    auto ret =rsRenderFilterParaBase->ReadFromParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: GetRenderPropert001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GetRenderPropert001, TestSize.Level1)
{
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    auto renderPropert = rsRenderFilterParaBase->GetRenderProperty(RSUIFilterType::BLUR);
    EXPECT_EQ(renderPropert, nullptr);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    auto leafRenderProperties = rsRenderFilterParaBase->GetLeafRenderProperties();
    EXPECT_TRUE(leafRenderProperties.empty());
}

} // namespace OHOS::Rosen
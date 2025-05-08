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
#include "render/rs_render_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderBlurFilterTest::SetUpTestCase() {}
void RSRenderBlurFilterTest::TearDownTestCase() {}
void RSRenderBlurFilterTest::SetUp() {}
void RSRenderBlurFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBlurFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderBlurFilterPara = std::make_shared<RSRenderBlurFilterPara>(0);
    std::string out;
    rsRenderBlurFilterPara->GetDescription(out);
    std::cout << out << std::endl;
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBlurFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderBlurFilterPara = std::make_shared<RSRenderBlurFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderBlurFilterPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBlurFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderBlurFilterPara = std::make_shared<RSRenderBlurFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderBlurFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBlurFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderBlurFilterPara = std::make_shared<RSRenderBlurFilterPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderBlurFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}

} // namespace OHOS::Rosen
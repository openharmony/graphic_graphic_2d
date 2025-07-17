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
#include <gtest/gtest.h>
#include "pipeline/main_thread/rs_main_thread.h"
#include "feature/rdo/rs_rdo.h"
#include "pipeline/rs_test_util.h"
#include "system/rs_system_parameters.h"
#include <parameters.h>

using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
class RSRDOTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RSRDOTest::SetUpTestCase() {}
void RSRDOTest::TearDownTestCase() {}
void RSRDOTest::SetUp() {}
void RSRDOTest::TearDown() {}
 
#ifdef RS_ENABLE_RDO
/**
 * @tc.name: TestWhenRDOIsDisabled
 * @tc.desc: test TestRDO
 * @tc.type: FUNC
 * @tc.require:
 */
constexpr const char* RDOPARAM = "persist.graphic.profiler.renderservice.rdo.enable";
HWTEST_F(RSRDOTest, TestWhenRDOIsDisabled, TestSize.Level1)
{
    static std::string value = system::GetParameter(RDOPARAM, "undef");
    system::SetParameter(RDOPARAM, "false");
    int32_t result = EnableRSCodeCache();
    EXPECT_EQ(result, 0);
    void* result_ht = HelperThreadforBinXO(nullptr);
    EXPECT_EQ(result_ht, nullptr);
    system::SetParameter(RDOPARAM, value);
}
/**
 * @tc.name: TestWhenRDODisabledButNoCrash
 * @tc.desc: test TestRDO
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRDOTest, TestWhenRDODisabledButNoCrash, TestSize.Level1)
{
    static std::string value = system::GetParameter(RDOPARAM, "undef");
    system::SetParameter(RDOPARAM, "true");
    int32_t result = EnableRSCodeCache();
    EXPECT_EQ(result, 0);
    void* result_ht = HelperThreadforBinXO(nullptr);
    EXPECT_EQ(result_ht, nullptr);
    system::SetParameter(RDOPARAM, value);
}
#endif
 
}
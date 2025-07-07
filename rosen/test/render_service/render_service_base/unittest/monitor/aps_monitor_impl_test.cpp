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
#include "monitor/aps_monitor_impl.h"

using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
 
class ApsMonitorImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static ApsMonitorImpl* monitor_;
};
 
void ApsMonitorImplTest::SetUpTestCase()
{
    monitor_ = &ApsMonitorImpl::GetInstance();
}
void ApsMonitorImplTest::TearDownTestCase() {}
void ApsMonitorImplTest::SetUp() {}
void ApsMonitorImplTest::TearDown() {}
ApsMonitorImpl* ApsMonitorImplTest::monitor_ = nullptr;

/*
 * @tc.name: SetApsSurfaceNodeTreeChangeTest
 * @tc.desc: Test SetApsSurfaceNodeTreeChange
 * @tc.type: FUNC
 */
HWTEST_F(ApsMonitorImplTest, SetApsSurfaceNodeTreeChangeTest, TestSize.Level1)
{
    monitor_->isApsFuncsAvailable_ = true;
    monitor_->isApsFuncsLoad_ = false;
    monitor_->SetApsSurfaceNodeTreeChange(true, "test1");
    ASSERT_NE(monitor_->sendApsEventFunc_, nullptr);
 
    monitor_->isApsFuncsAvailable_ = false;
    monitor_->isApsFuncsLoad_ = true;
    monitor_->sendApsEventFunc_ = nullptr;
    monitor_->SetApsSurfaceNodeTreeChange(true, "test2");
    ASSERT_EQ(monitor_->sendApsEventFunc_, nullptr);
}
} // namespace OHOS::Rosen
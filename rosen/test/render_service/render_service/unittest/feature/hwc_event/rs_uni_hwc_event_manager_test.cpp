/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "feature/hwc_event/rs_uni_hwc_event_manager.h"
#include "gtest/gtest.h"
#include "pipeline/rs_pointer_window_manager.h"

using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
constexpr uint32_t DEFAULT_DEVID = 0;
constexpr uint32_t DEFAULT_EVENTID = 0;
class RSUniHwcEventManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RSUniHwcEventManagerTest::SetUpTestCase() {}
void RSUniHwcEventManagerTest::TearDownTestCase() {}
void RSUniHwcEventManagerTest::SetUp() {}
void RSUniHwcEventManagerTest::TearDown() {}
 
/**
 * @tc.name: OnHwcEvent001
 * @tc.desc: OnHwcEvent, when TUI enabled and disabled
 * @tc.type: FUNC
 * @tc.require: issue21895
 */
HWTEST_F(RSUniHwcEventManagerTest, CreateSurfaceNodeLayerInfo001, TestSize.Level1)
{
    auto& uniHwcEventManager = RSUniHwcEventManager::GetInstance();
    vector<int32_t> eventData;
    uniHwcEventManager.OnHwcEvent(DEFAULT_DEVID, HwcEvent::HWCEVENT_TUI_ENTER, eventData, nullptr);
    ASSERT_TRUE(RSPointerWindowManager::Instance().IsTuiEnabled());
    uniHwcEventManager.OnHwcEvent(DEFAULT_DEVID, HwcEvent::HWCEVENT_TUI_EXIT, eventData, nullptr);
    ASSERT_FALSE(RSPointerWindowManager::Instance().IsTuiEnabled());
    uniHwcEventManager.OnHwcEvent(DEFAULT_DEVID, DEFAULT_EVENTID, eventData, nullptr);
    ASSERT_FALSE(RSPointerWindowManager::Instance().IsTuiEnabled());
}
}
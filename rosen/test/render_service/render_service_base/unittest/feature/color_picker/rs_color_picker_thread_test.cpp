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

#include "feature/color_picker/rs_color_picker_thread.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorPickerThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorPickerThreadTest::SetUpTestCase() {}
void RSColorPickerThreadTest::TearDownTestCase() {}
void RSColorPickerThreadTest::SetUp() {}
void RSColorPickerThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: test results of PostTaskTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, PostTaskTest, TestSize.Level1)
{
    auto func = []() -> void {};
    RSColorPickerThread::Instance().PostTask(func);
    EXPECT_NE(RSColorPickerThread::Instance().handler_, nullptr);
}

/**
 * @tc.name: RSColorPickerThreadTestTest
 * @tc.desc: Test result of RegisterNodeDirtyCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, RegisterNodeDirtyCallbackTest, TestSize.Level1)
{
    auto callback = [](uint64_t nodeId) -> void {};
    RSColorPickerThread::Instance().RegisterNodeDirtyCallback(callback);
    EXPECT_NE(RSColorPickerThread::Instance().callback_, nullptr);
}
} // namespace OHOS::Rosen

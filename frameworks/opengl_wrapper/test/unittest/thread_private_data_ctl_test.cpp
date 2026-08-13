/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "thread_private_data_ctl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ThreadPrivateDataCtlTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: GetError001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(ThreadPrivateDataCtlTest, GetError001, Level1)
{
    ThreadPrivateDataCtl threadPrivateDataCtl;
    threadPrivateDataCtl.ClearPrivateData();

    auto result = threadPrivateDataCtl.GetError();
    ASSERT_EQ(EGL_SUCCESS, result);
    threadPrivateDataCtl.~ThreadPrivateDataCtl();
}

/**
 * @tc.name: GetError002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(ThreadPrivateDataCtlTest, GetError002, Level2)
{
    ThreadPrivateDataCtl threadPrivateDataCtl;
    threadPrivateDataCtl.SetError(0);

    auto result = threadPrivateDataCtl.GetError();
    ASSERT_NE(EGL_SUCCESS, result);
    threadPrivateDataCtl.~ThreadPrivateDataCtl();
}

/**
 * @tc.name: GetContext001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(ThreadPrivateDataCtlTest, GetContext001, Level1)
{
    ThreadPrivateDataCtl threadPrivateDataCtl;
    EGLContext ctx = nullptr;
    threadPrivateDataCtl.SetContext(ctx);
    ASSERT_EQ(ctx, threadPrivateDataCtl.GetContext());
    threadPrivateDataCtl.~ThreadPrivateDataCtl();
}

/**
 * @tc.name: GetContext002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(ThreadPrivateDataCtlTest, GetContext002, Level2)
{
    ThreadPrivateDataCtl threadPrivateDataCtl;
    threadPrivateDataCtl.SetError(0);
    ASSERT_EQ(EGL_NO_CONTEXT, threadPrivateDataCtl.GetContext());
    threadPrivateDataCtl.~ThreadPrivateDataCtl();
}

/**
 * @tc.name: GetGlHookTable001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(ThreadPrivateDataCtlTest, GetGlHookTable001, Level1)
{
    ThreadPrivateDataCtl threadPrivateDataCtl;
    GlHookTable *table = nullptr;
    threadPrivateDataCtl.SetGlHookTable(table);
    ASSERT_EQ(table, threadPrivateDataCtl.GetGlHookTable());
    threadPrivateDataCtl.~ThreadPrivateDataCtl();
}

/**
 * @tc.name: GetGlHookTable002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(ThreadPrivateDataCtlTest, GetGlHookTable002, Level2)
{
    ThreadPrivateDataCtl threadPrivateDataCtl;
    threadPrivateDataCtl.SetError(0);
    ASSERT_EQ(nullptr, threadPrivateDataCtl.GetGlHookTable());
    threadPrivateDataCtl.~ThreadPrivateDataCtl();
}
} // OHOS::Rosen
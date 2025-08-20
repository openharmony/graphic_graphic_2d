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
#include "vsync_receiver.h"

using namespace testing;
using namespace testing::ext;

static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

namespace OHOS::Rosen {
class VSyncCallBackListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<VSyncCallBackListener> vsyncCallBackListener_ = nullptr;
};

void VSyncCallBackListenerTest::SetUpTestCase()
{
    vsyncCallBackListener_ = std::make_shared<VSyncCallBackListener>();
    vsyncCallBackListener_->period_ = 16000000; // 16000000ns == 16ms
}

void VSyncCallBackListenerTest::TearDownTestCase()
{
    vsyncCallBackListener_ = nullptr;
}

void FdShutDownCallBackTest(int32_t inputTest) {}

bool ReadableCallbackTest(int32_t inputTest)
{
    return true;
}

/**
 * @tc.name: OnReadable001
 * @tc.desc: OnReadable Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, OnReadable001, Function | MediumTest | Level2)
{
    int32_t fileFd = -1;
    vsyncCallBackListener_->OnReadable(fileFd);
    ASSERT_EQ(vsyncCallBackListener_->readableCallback_, nullptr);
}

/**
 * @tc.name: OnShutdown001
 * @tc.desc: OnShutdown Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, OnShutdown001, Function | MediumTest | Level2)
{
    int32_t fileFd = -1;
    vsyncCallBackListener_->OnShutdown(fileFd);
    ASSERT_EQ(vsyncCallBackListener_->fdShutDownCallback_, nullptr);
}

/**
 * @tc.name: ReadFdInternal001
 * @tc.desc: ReadFdInternal Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, ReadFdInternal001, Function | MediumTest | Level2)
{
    int32_t fileFd = -1;
    int64_t data[3] = {0, 0, 0};
    ssize_t dataCount = 3;
    vsyncCallBackListener_->SetFdClosedFlagLocked(true);
    VsyncError ret = vsyncCallBackListener_->ReadFdInternal(fileFd, data, dataCount);
    ASSERT_EQ(ret, VSYNC_ERROR_API_FAILED);
}

/**
 * @tc.name: HandleVsyncCallbacks001
 * @tc.desc: HandleVsyncCallbacks Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, HandleVsyncCallbacks001, Function | MediumTest | Level2)
{
    int32_t fileFd = -1;
    int64_t data[3] = {0, 0, 0};
    ssize_t dataCount = 3;
    vsyncCallBackListener_->HandleVsyncCallbacks(data, dataCount, fileFd);
    ASSERT_EQ(vsyncCallBackListener_->RNVFlag_, false);
}

/**
 * @tc.name: PrintRequestTs001
 * @tc.desc: PrintRequestTs Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, PrintRequestTs001, Function | MediumTest | Level2)
{
    vsyncCallBackListener_->requestCount_ = 0;
    int64_t ts = SystemTime();
    vsyncCallBackListener_->PrintRequestTs(ts);
    ASSERT_EQ(vsyncCallBackListener_->requestCount_, 1);
}

/**
 * @tc.name: PrintRequestTs002
 * @tc.desc: PrintRequestTs Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, PrintRequestTs002, Function | MediumTest | Level2)
{
    vsyncCallBackListener_->requestCount_ = 1200; // PRINT_COUNT_MAX = 1200
    int64_t ts = SystemTime();
    vsyncCallBackListener_->PrintRequestTs(ts);
    ASSERT_EQ(vsyncCallBackListener_->requestCount_, 0);
    ts -= 450000000; // TIME_OUT_INTERVAL = 450000000
    vsyncCallBackListener_->PrintRequestTs(ts);
    ASSERT_EQ(vsyncCallBackListener_->requestCount_, 1);
}

/**
 * @tc.name: PrintRequestTs003
 * @tc.desc: PrintRequestTs Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, PrintRequestTs003, Function | MediumTest | Level2)
{
    vsyncCallBackListener_->requestCount_ = 0;
    vsyncCallBackListener_->name_ = "rs";
    int64_t ts = SystemTime();
    vsyncCallBackListener_->PrintRequestTs(ts);
    ASSERT_EQ(vsyncCallBackListener_->requestCount_, 0);
}

/**
 * @tc.name: CalculateExpectedEndLocked001
 * @tc.desc: CalculateExpectedEndLocked Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, CalculateExpectedEndLocked001, Function | MediumTest | Level2)
{
    int64_t ts = SystemTime();
    vsyncCallBackListener_->name_ = "rs";
    int64_t res = vsyncCallBackListener_->CalculateExpectedEndLocked(ts);
    int64_t timeEnd = vsyncCallBackListener_->period_ + ts - 5000000; // rs vsync offset is 5000000ns
    ASSERT_EQ(timeEnd, res);
    vsyncCallBackListener_->name_ = "testApp";
    res = vsyncCallBackListener_->CalculateExpectedEndLocked(ts);
    timeEnd = vsyncCallBackListener_->period_ + ts; // rs vsync offset is 5000000ns
    ASSERT_EQ(timeEnd, res);
}

/**
 * @tc.name: RegisterFdShutDownCallback001
 * @tc.desc: RegisterFdShutDownCallback Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, RegisterFdShutDownCallback001, Function | MediumTest | Level2)
{
    auto res = vsyncCallBackListener_->fdShutDownCallback_;
    vsyncCallBackListener_->RegisterFdShutDownCallback(FdShutDownCallBackTest);
    ASSERT_NE(vsyncCallBackListener_->fdShutDownCallback_, nullptr);
    vsyncCallBackListener_->RegisterFdShutDownCallback(res);
}

/**
 * @tc.name: RegisterReadableCallback001
 * @tc.desc: RegisterReadableCallback Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, RegisterReadableCallback001, Function | MediumTest | Level2)
{
    auto res = vsyncCallBackListener_->readableCallback_;
    vsyncCallBackListener_->RegisterReadableCallback(ReadableCallbackTest);
    ASSERT_NE(vsyncCallBackListener_->readableCallback_, nullptr);
    vsyncCallBackListener_->RegisterReadableCallback(res);
}

/**
 * @tc.name: SetFdClosedFlagLocked001
 * @tc.desc: SetFdClosedFlagLocked Test
 * @tc.type: FUNC
 * @tc.require: issueICPXGM
 */
HWTEST_F(VSyncCallBackListenerTest, SetFdClosedFlagLocked001, Function | MediumTest | Level2)
{
    bool fdClose = true;
    vsyncCallBackListener_->SetFdClosedFlagLocked(fdClose);
    ASSERT_EQ(vsyncCallBackListener_->fdClosed_, fdClose);
}
}
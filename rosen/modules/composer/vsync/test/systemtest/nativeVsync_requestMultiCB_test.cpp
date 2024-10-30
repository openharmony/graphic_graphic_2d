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
#include <gtest/gtest.h>
#include <mutex>
#include <unistd.h>
#include <vector>
#include "native_vsync.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
std::mutex g_mutex;
int g_counter = 0;
std::vector<int> datas;
static void OnVSync(long long time, void *data)
{
    std::unique_lock<std::mutex> locker(g_mutex);
    if (!data) {
        return;
    }
    vector<int>::iterator it = find(datas.begin(), datas.end(), *(int *)data);
    if (it != datas.end()) {
        datas.erase(it);
    }
    delete (int *)data;
}
}
class NativeVSyncMultCallbackTest : public testing::Test {
public:
    void TestMultiTimes(int times);
};

void NativeVSyncMultCallbackTest::TestMultiTimes(int times)
{
    char name[] = "TestMultiTimes";
    OH_NativeVSync *native_vsync = OH_NativeVSync_Create(name, sizeof(name));
    for (int i = 0; i < times; i++) {
        std::unique_lock<std::mutex> locker(g_mutex);
        int *userData = new int(g_counter++);
        OH_NativeVSync_FrameCallback callback = OnVSync;
        int ret = OH_NativeVSync_RequestFrameWithMultiCallback(native_vsync, callback, userData);
        if (ret == 0) {
            datas.push_back(*userData);
        }
        usleep(1); // 1us
    }
    usleep(200000); // 200000us
    ASSERT_EQ(datas.size(), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: RequestFrameWithMultiCallback
 */
HWTEST_F(NativeVSyncMultCallbackTest, RequestFrameWithMultiCallback_1_time, Function | MediumTest | Level2)
{
    TestMultiTimes(1); // test 1 time
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: RequestFrameWithMultiCallback
 */
HWTEST_F(NativeVSyncMultCallbackTest, RequestFrameWithMultiCallback_3_times, Function | MediumTest | Level2)
{
    TestMultiTimes(3); // test 3 times
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: RequestFrameWithMultiCallback
 */
HWTEST_F(NativeVSyncMultCallbackTest, RequestFrameWithMultiCallback_10_times, Function | MediumTest | Level2)
{
    TestMultiTimes(10); // test 10 times
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: RequestFrameWithMultiCallback
 */
HWTEST_F(NativeVSyncMultCallbackTest, RequestFrameWithMultiCallback_100_times, Function | MediumTest | Level2)
{
    TestMultiTimes(100); // test 100 times
}
}

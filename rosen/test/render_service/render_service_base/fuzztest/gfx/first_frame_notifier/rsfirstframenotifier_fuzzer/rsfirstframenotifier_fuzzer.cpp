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

#include "rsfirstframenotifier_fuzzer.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#include "ipc_callbacks/rs_first_frame_commit_callback_stub.h"
#include "transaction/rs_render_service_client.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>
#include <memory>

namespace OHOS {
namespace Rosen {                                            

namespace {
constexpr size_t STR_LEN = 10;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

/*
 * get a string from g_data
 */
std::string GetStringFromData(int strlen)
{
    if (strlen <= 0) {
        return "fuzz";
    }
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        char tmp = GetData<char>();
        if (tmp == '\0') {
            tmp = '1';
        }
        cstr[i] = tmp;
    }
    std::string str(cstr);
    return str;
}

class CustomFirstFrameCommitCallback : public RSFirstFrameCommitCallbackStub {
public:
    explicit CustomFirstFrameCommitCallback(const FirstFrameCommitCallback& callback) : cb_(callback) {}
    ~CustomFirstFrameCommitCallback() override {};

    void OnFirstFrameCommit(uint64_t screenId, int64_t timestamp) override
    {
        if (cb_ != nullptr) {
            cb_(screenId, timestamp);
        }
    }

private:
    FirstFrameCommitCallback cb_;
};

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    ScreenId screenId = GetData<uint64_t>();
    pid_t pid = GetData<int>();
    auto firstFrameCommitCallback = [](uint64_t, int64_t) {};
    sptr<CustomFirstFrameCommitCallback> cb = new CustomFirstFrameCommitCallback(firstFrameCommitCallback);
    RSFirstFrameNotifier& firstFrameNotifier = RSFirstFrameNotifier::GetInstance();
    firstFrameNotifier.AddFirstFrameCommitScreen(screenId);
    firstFrameNotifier.RegisterFirstFrameCommitCallback(pid, cb);
    firstFrameNotifier.ExecIfFirstFrameCommit(screenId);
    firstFrameNotifier.OnFirstFrameCommitCallback(screenId);
    firstFrameNotifier.RegisterFirstFrameCommitCallback(pid, nullptr);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

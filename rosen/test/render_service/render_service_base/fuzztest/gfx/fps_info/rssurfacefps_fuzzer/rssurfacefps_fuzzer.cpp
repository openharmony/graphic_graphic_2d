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

#include "rssurfacefps_fuzzer.h"
#include "gfx/fps_info/rs_surface_fps.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"

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

bool RSSurfaceFpsFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    std::string name = GetStringFromData(STR_LEN);
    std::string result = GetStringFromData(STR_LEN);
    uint64_t timestamp1 = GetData<uint64_t>();
    uint64_t timestamp2 = GetData<uint64_t>();
    uint64_t timestamp3 = GetData<uint64_t>();
    uint32_t seqNum1 = GetData<uint32_t>();
    uint32_t seqNum2 = GetData<uint32_t>();

    std::shared_ptr<RSSurfaceFps> surfaceFpsPtr = std::make_shared<RSSurfaceFps>(name);
    surfaceFpsPtr->RecordPresentTime(timestamp1, seqNum1);
    surfaceFpsPtr->RecordPresentTime(timestamp2, seqNum2);
    surfaceFpsPtr->RecordPresentTime(timestamp3, seqNum2);
    surfaceFpsPtr->Dump(result);
    surfaceFpsPtr->ClearDump();

    return true;
}

bool RSFirstFrameNotifierTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    pid_t pid = GetData<pid_t>();
    const sptr<RSIFirstFrameCommitCallback> callback;
    RSFirstFrameNotifier::GetInstance().RegisterFirstFrameCommitCallback(pid, callback);

    ScreenId screenId = GetData<ScreenId>();
    RSFirstFrameNotifier::GetInstance().OnFirstFrameCommitCallback(screenId);
    RSFirstFrameNotifier::GetInstance().ExecIfFirstFrameCommit(screenId);
    RSFirstFrameNotifier::GetInstance().AddFirstFrameCommitScreen(screenId);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSSurfaceFpsFuzzTest(data, size);
    OHOS::Rosen::RSFirstFrameNotifierTest(data, size);
    return 0;
}

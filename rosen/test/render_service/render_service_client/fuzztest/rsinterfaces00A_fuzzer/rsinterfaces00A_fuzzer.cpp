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

#include "rsinterfaces00A_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_GET_MEMORY_GRAPHIC = 0;
const uint8_t DO_GET_TOTAL_APP_MEM_SIZE = 1;
const uint8_t DO_SET_WINDOW_EXPECTED_REFRESH_RATE = 2;
const uint8_t DO_REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK = 3;
const uint8_t TARGET_SIZE = 4;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t STR_LEN = 10;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

namespace Mock {

} // namespace Mock

void DoGetMemoryGraphic()
{
    int pid = GetData<int>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetMemoryGraphic(pid);
}

void DoGetTotalAppMemSize()
{
    float cpuMemSize = GetData<float>();
    float gpuMemSize = GetData<float>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetTotalAppMemSize(cpuMemSize, gpuMemSize);
}

void DoSetWindowExpectedRefreshRate()
{
    uint64_t winId = GetData<uint64_t>();
    EventInfo eventInfo;
    eventInfo.eventName = GetStringFromData(STR_LEN);
    eventInfo.eventStatus = GetData<bool>();
    eventInfo.minRefreshRate = GetData<uint32_t>();
    eventInfo.maxRefreshRate = GetData<uint32_t>();
    eventInfo.description = GetStringFromData(STR_LEN);
    std::unordered_map<uint64_t, EventInfo> addVotes;
    addVotes.insert({winId, eventInfo});
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetWindowExpectedRefreshRate(addVotes);
    std::unordered_map<uint64_t, EventInfo> delVotes;
    std::string vsyncName = GetStringFromData(STR_LEN);
    delVotes.insert({vsyncName, eventInfo});
    rsInterfaces.SetWindowExpectedRefreshRate(delVotes);
}

void DoRegisterSelfDrawingNodeRectChangeCallback()
{
    SelfDrawingNodeRectChangeCallback callback =
        [](std::shared_ptr<RSSelfDrawingNodeRectData> SelfDrawingNodeRectData) {};
    RectConstraint constraint;
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RegisterSelfDrawingNodeRectChangeCallback(constraint, callback);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_MEMORY_GRAPHIC:
            OHOS::Rosen::DoGetMemoryGraphic();
            break;
        case OHOS::Rosen::DO_GET_TOTAL_APP_MEM_SIZE:
            OHOS::Rosen::DoGetTotalAppMemSize();
            break;
        case OHOS::Rosen::DO_SET_WINDOW_EXPECTED_REFRESH_RATE:
            OHOS::Rosen::DoSetWindowExpectedRefreshRate();
            break;
        case OHOS::Rosen::DO_REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterSelfDrawingNodeRectChangeCallback();
            break;
        default:
            return -1;
    }
    return 0;
}

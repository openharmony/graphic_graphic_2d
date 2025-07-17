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

#include "rsinterfaces00B_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_REPORT_EVENT_RESPONSE = 0;
const uint8_t DO_REPORT_EVENT_COMPLETE = 1;
const uint8_t DO_REPORT_EVENT_JANK_FRAME = 2;
const uint8_t DO_REPORT_RS_SCENE_JANK_START = 3;
const uint8_t DO_REPORT_RS_SCENE_JANK_END = 4;
const uint8_t DO_REPORT_EVENT_GAMESTATE = 5;
const uint8_t TARGET_SIZE = 6;

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

void DoReportEventResponse()
{
    DataBaseRs info;
    info.appPid = GetData<int32_t>();
    info.eventType = GetData<int32_t>();
    info.versionCode = GetData<int32_t>();
    info.uniqueId = GetData<int64_t>();
    info.inputTime = GetData<int64_t>();
    info.beginVsyncTime = GetData<int64_t>();
    info.endVsyncTime = GetData<int64_t>();
    info.isDisplayAnimator = GetData<bool>();
    info.sceneId = GetStringFromData(STR_LEN);
    info.versionName = GetStringFromData(STR_LEN);
    info.bundleName = GetStringFromData(STR_LEN);
    info.processName = GetStringFromData(STR_LEN);
    info.abilityName = GetStringFromData(STR_LEN);
    info.pageUrl = GetStringFromData(STR_LEN);
    info.sourceType = GetStringFromData(STR_LEN);
    info.note = GetStringFromData(STR_LEN);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.ReportEventResponse(info);
}

void DoReportEventComplete()
{
    DataBaseRs info;
    info.appPid = GetData<int32_t>();
    info.eventType = GetData<int32_t>();
    info.versionCode = GetData<int32_t>();
    info.uniqueId = GetData<int64_t>();
    info.inputTime = GetData<int64_t>();
    info.beginVsyncTime = GetData<int64_t>();
    info.endVsyncTime = GetData<int64_t>();
    info.isDisplayAnimator = GetData<bool>();
    info.sceneId = GetStringFromData(STR_LEN);
    info.versionName = GetStringFromData(STR_LEN);
    info.bundleName = GetStringFromData(STR_LEN);
    info.processName = GetStringFromData(STR_LEN);
    info.abilityName = GetStringFromData(STR_LEN);
    info.pageUrl = GetStringFromData(STR_LEN);
    info.sourceType = GetStringFromData(STR_LEN);
    info.note = GetStringFromData(STR_LEN);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.ReportEventComplete(info);
}

void DoReportEventJankFrame()
{
    DataBaseRs info;
    info.appPid = GetData<int32_t>();
    info.eventType = GetData<int32_t>();
    info.versionCode = GetData<int32_t>();
    info.uniqueId = GetData<int64_t>();
    info.inputTime = GetData<int64_t>();
    info.beginVsyncTime = GetData<int64_t>();
    info.endVsyncTime = GetData<int64_t>();
    info.isDisplayAnimator = GetData<bool>();
    info.sceneId = GetStringFromData(STR_LEN);
    info.versionName = GetStringFromData(STR_LEN);
    info.bundleName = GetStringFromData(STR_LEN);
    info.processName = GetStringFromData(STR_LEN);
    info.abilityName = GetStringFromData(STR_LEN);
    info.pageUrl = GetStringFromData(STR_LEN);
    info.sourceType = GetStringFromData(STR_LEN);
    info.note = GetStringFromData(STR_LEN);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.ReportEventJankFrame(info);
}

void DoReportRsSceneJankStart()
{
    AppInfo info;
    info.startTime = GetData<int64_t>();
    info.endTime = GetData<int64_t>();
    info.pid = GetData<int32_t>();
    info.versionName = GetStringFromData(STR_LEN);
    info.versionCode = GetData<int32_t>();
    info.bundleName = GetStringFromData(STR_LEN);
    info.processName = GetStringFromData(STR_LEN);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.ReportRsSceneJankStart(info);
}

void DoReportRsSceneJankEnd()
{
    AppInfo info;
    info.startTime = GetData<int64_t>();
    info.endTime = GetData<int64_t>();
    info.pid = GetData<int32_t>();
    info.versionName = GetStringFromData(STR_LEN);
    info.versionCode = GetData<int32_t>();
    info.bundleName = GetStringFromData(STR_LEN);
    info.processName = GetStringFromData(STR_LEN);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.ReportRsSceneJankEnd(info);
}

void DoReportGameStateData()
{
    GameStateData info = {
        .pid = GetData<int32_t>(),
        .uid = GetData<int32_t>(),
        .state = GetData<int32_t>(),
        .renderTid = GetData<int32_t>(),
        .bundleName = GetStringFromData(STR_LEN),
    };
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.ReportGameStateData(info);
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
        case OHOS::Rosen::DO_REPORT_EVENT_RESPONSE:
            OHOS::Rosen::DoReportEventResponse();
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_COMPLETE:
            OHOS::Rosen::DoReportEventComplete();
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_JANK_FRAME:
            OHOS::Rosen::DoReportEventJankFrame();
            break;
        case OHOS::Rosen::DO_REPORT_RS_SCENE_JANK_START:
            OHOS::Rosen::DoReportRsSceneJankStart();
            break;
        case OHOS::Rosen::DO_REPORT_RS_SCENE_JANK_END:
            OHOS::Rosen::DoReportRsSceneJankEnd();
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_GAMESTATE:
            OHOS::Rosen::DoReportGameStateData();
            break;
        default:
            return -1;
    }
    return 0;
}

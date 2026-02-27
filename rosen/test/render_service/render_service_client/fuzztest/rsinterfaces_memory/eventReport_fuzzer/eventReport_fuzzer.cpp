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

#include "eventReport_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_REPORT_EVENT_RESPONSE = 0;
const uint8_t DO_REPORT_EVENT_COMPLETE = 1;
const uint8_t DO_REPORT_EVENT_JANK_FRAME = 2;
const uint8_t DO_REPORT_RS_SCENE_JANK_START = 3;
const uint8_t DO_REPORT_RS_SCENE_JANK_END = 4;
const uint8_t TARGET_SIZE = 5;
constexpr uint32_t STR_LEN = 10;

void DoReportEventResponse(FuzzedDataProvider& fdp)
{
    DataBaseRs info;
    info.appPid = fdp.ConsumeIntegral<int32_t>();
    info.eventType = fdp.ConsumeIntegral<int32_t>();
    info.versionCode = fdp.ConsumeIntegral<int32_t>();
    info.uniqueId = fdp.ConsumeIntegral<int64_t>();
    info.inputTime = fdp.ConsumeIntegral<int64_t>();
    info.beginVsyncTime = fdp.ConsumeIntegral<int64_t>();
    info.endVsyncTime = fdp.ConsumeIntegral<int64_t>();
    info.isDisplayAnimator = fdp.ConsumeBool();
    info.sceneId = fdp.ConsumeRandomLengthString(STR_LEN);
    info.versionName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.bundleName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.processName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.abilityName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.pageUrl = fdp.ConsumeRandomLengthString(STR_LEN);
    info.sourceType = fdp.ConsumeRandomLengthString(STR_LEN);
    info.note = fdp.ConsumeRandomLengthString(STR_LEN);
    g_rsInterfaces->ReportEventResponse(info);
}

void DoReportEventComplete(FuzzedDataProvider& fdp)
{
    DataBaseRs info;
    info.appPid = fdp.ConsumeIntegral<int32_t>();
    info.eventType = fdp.ConsumeIntegral<int32_t>();
    info.versionCode = fdp.ConsumeIntegral<int32_t>();
    info.uniqueId = fdp.ConsumeIntegral<int64_t>();
    info.inputTime = fdp.ConsumeIntegral<int64_t>();
    info.beginVsyncTime = fdp.ConsumeIntegral<int64_t>();
    info.endVsyncTime = fdp.ConsumeIntegral<int64_t>();
    info.isDisplayAnimator = fdp.ConsumeBool();
    info.sceneId = fdp.ConsumeRandomLengthString(STR_LEN);
    info.versionName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.bundleName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.processName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.abilityName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.pageUrl = fdp.ConsumeRandomLengthString(STR_LEN);
    info.sourceType = fdp.ConsumeRandomLengthString(STR_LEN);
    info.note = fdp.ConsumeRandomLengthString(STR_LEN);
    g_rsInterfaces->ReportEventComplete(info);
}

void DoReportEventJankFrame(FuzzedDataProvider& fdp)
{
    DataBaseRs info;
    info.appPid = fdp.ConsumeIntegral<int32_t>();
    info.eventType = fdp.ConsumeIntegral<int32_t>();
    info.versionCode = fdp.ConsumeIntegral<int32_t>();
    info.uniqueId = fdp.ConsumeIntegral<int64_t>();
    info.inputTime = fdp.ConsumeIntegral<int64_t>();
    info.beginVsyncTime = fdp.ConsumeIntegral<int64_t>();
    info.endVsyncTime = fdp.ConsumeIntegral<int64_t>();
    info.isDisplayAnimator = fdp.ConsumeBool();
    info.sceneId = fdp.ConsumeRandomLengthString(STR_LEN);
    info.versionName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.bundleName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.processName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.abilityName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.pageUrl = fdp.ConsumeRandomLengthString(STR_LEN);
    info.sourceType = fdp.ConsumeRandomLengthString(STR_LEN);
    info.note = fdp.ConsumeRandomLengthString(STR_LEN);
    g_rsInterfaces->ReportEventJankFrame(info);
}

void DoReportRsSceneJankStart(FuzzedDataProvider& fdp)
{
    AppInfo info;
    info.startTime = fdp.ConsumeIntegral<int64_t>();
    info.endTime = fdp.ConsumeIntegral<int64_t>();
    info.pid = fdp.ConsumeIntegral<int32_t>();
    info.versionName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.versionCode = fdp.ConsumeIntegral<int32_t>();
    info.bundleName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.processName = fdp.ConsumeRandomLengthString(STR_LEN);
    g_rsInterfaces->ReportRsSceneJankStart(info);
}

void DoReportRsSceneJankEnd(FuzzedDataProvider& fdp)
{
    AppInfo info;
    info.startTime = fdp.ConsumeIntegral<int64_t>();
    info.endTime = fdp.ConsumeIntegral<int64_t>();
    info.pid = fdp.ConsumeIntegral<int32_t>();
    info.versionName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.versionCode = fdp.ConsumeIntegral<int32_t>();
    info.bundleName = fdp.ConsumeRandomLengthString(STR_LEN);
    info.processName = fdp.ConsumeRandomLengthString(STR_LEN);
    g_rsInterfaces->ReportRsSceneJankEnd(info);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_REPORT_EVENT_RESPONSE:
            OHOS::Rosen::DoReportEventResponse(fdp);
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_COMPLETE:
            OHOS::Rosen::DoReportEventComplete(fdp);
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_JANK_FRAME:
            OHOS::Rosen::DoReportEventJankFrame(fdp);
            break;
        case OHOS::Rosen::DO_REPORT_RS_SCENE_JANK_START:
            OHOS::Rosen::DoReportRsSceneJankStart(fdp);
            break;
        case OHOS::Rosen::DO_REPORT_RS_SCENE_JANK_END:
            OHOS::Rosen::DoReportRsSceneJankEnd(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}

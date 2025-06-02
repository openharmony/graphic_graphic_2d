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

#include "rsperfmonitorreporter_fuzzer.h"
#include "gfx/performance/rs_perfmonitor_reporter.h"

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

bool RSPerfmonitorReporterFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSPerfMonitorReporter perfMonitor;

    NodeId id1 = GetData<NodeId>();
    std::string nodeName = GetStringFromData(STR_LEN);
    uint16_t filterType = GetData<uint16_t>();
    float blurRadius = GetData<float>();
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    int32_t blurTime = GetData<int32_t>();
    bool isBlurType = GetData<bool>();
    perfMonitor.RecordBlurPerfEvent(id1, nodeName, filterType, blurRadius, width, height, blurTime, isBlurType);
    
    int64_t duration = GetData<int64_t>();
    perfMonitor.RecordBlurNode(nodeName, duration, isBlurType);
    perfMonitor.GetCurrentBundleName();

    std::chrono::time_point<high_resolution_clock> startTime =
        std::chrono::high_resolution_clock::now() - std::chrono::microseconds(GetData<uint16_t>());
    NodeId id2 = GetData<NodeId>();
    auto context = std::make_shared<RSContext>();
    int updateTimes = GetData<int>();
    perfMonitor.StartRendergroupMonitor();
    perfMonitor.EndRendergroupMonitor(startTime, id2, context, updateTimes);
    perfMonitor.ClearRendergroupDataMap(id2);

    std::string bundleName2 = GetStringFromData(STR_LEN);
    perfMonitor.SetCurrentBundleName(bundleName2.c_str());

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSPerfmonitorReporterFuzzTest(data, size);
    return 0;
}

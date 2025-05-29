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

#include "rssurfacefpsmanager_fuzzer.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"

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

bool RSSurfaceFpsManagerFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    std::string result0 = GetStringFromData(STR_LEN);
    std::string result1 = GetStringFromData(STR_LEN);
    std::string result2 = GetStringFromData(STR_LEN);
    std::string result3 = GetStringFromData(STR_LEN);
    std::string result4 = GetStringFromData(STR_LEN);
    std::string result5 = GetStringFromData(STR_LEN);
    std::string result6 = GetStringFromData(STR_LEN);
    std::string result7 = GetStringFromData(STR_LEN);
    std::string name0 = GetStringFromData(STR_LEN);
    std::string name1 = GetStringFromData(STR_LEN);
    std::string name2 = GetStringFromData(STR_LEN);
    std::string name3 = GetStringFromData(STR_LEN);
    std::string option0 = GetStringFromData(STR_LEN);
    std::string option1 = GetStringFromData(STR_LEN);
    std::string arg0 = GetStringFromData(STR_LEN);
    std::string arg1 = GetStringFromData(STR_LEN);
    NodeId id0 = GetData<NodeId>();
    NodeId id1 = GetData<NodeId>();
    NodeId id2 = GetData<NodeId>();
    NodeId id3 = GetData<NodeId>();
    NodeId id4 = GetData<NodeId>();
    NodeId id5 = GetData<NodeId>();
    pid_t pid0 = GetData<pid_t>();
    pid_t pid1 = GetData<pid_t>();
    pid_t pid2 = GetData<pid_t>();
    uint64_t timestamp = GetData<uint64_t>();
    uint32_t seqNum = GetData<uint32_t>();
    bool isUnique = GetData<bool>();

    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    surfaceFpsManager.RegisterSurfaceFps(id0, name0);
    surfaceFpsManager.RecordPresentTime(id1, timestamp, seqNum);
    surfaceFpsManager.GetSurfaceFps(id2);
    surfaceFpsManager.GetSurfaceFps(name1, isUnique);
    surfaceFpsManager.GetSurfaceFpsByPid(pid0);
    surfaceFpsManager.Dump(result0, id3);
    surfaceFpsManager.ClearDump(result1, id4);
    surfaceFpsManager.Dump(result2, name2);
    surfaceFpsManager.ClearDump(result3, name3);
    surfaceFpsManager.DumpByPid(result4, pid1);
    surfaceFpsManager.ClearDumpByPid(result5, pid2);
    surfaceFpsManager.DumpSurfaceNodeFps(result6, option0, arg0);
    surfaceFpsManager.ClearSurfaceNodeFps(result7, option1, arg1);
    surfaceFpsManager.UnregisterSurfaceFps(id5);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSSurfaceFpsManagerFuzzTest(data, size);
    return 0;
}

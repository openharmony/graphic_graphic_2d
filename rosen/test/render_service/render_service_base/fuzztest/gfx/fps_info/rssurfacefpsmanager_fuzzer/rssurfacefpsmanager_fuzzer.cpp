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
    std::string result = GetStringFromData(STR_LEN);
    std::string name = GetStringFromData(STR_LEN);
    std::string uname = GetStringFromData(STR_LEN);
    std::string option = GetStringFromData(STR_LEN);
    std::string arg = GetStringFromData(STR_LEN);
    NodeId id = GetData<NodeId>();
    NodeId uid = GetData<NodeId>();
    pid_t pid = GetData<pid_t>();
    pid_t upid = GetData<pid_t>();
    uint64_t timestamp = GetData<uint64_t>();
    uint32_t seqNum = GetData<uint32_t>();
    bool isUnique = GetData<bool>();

    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    surfaceFpsManager.RegisterSurfaceFps(id, name);
    surfaceFpsManager.RegisterSurfaceFps(id, name);
    surfaceFpsManager.RecordPresentTime(id, timestamp, seqNum);
    surfaceFpsManager.RecordPresentTime(uid, timestamp, seqNum);
    surfaceFpsManager.Dump(result, id);
    surfaceFpsManager.Dump(result, uid);
    surfaceFpsManager.Dump(result, name);
    surfaceFpsManager.Dump(result, uname);
    surfaceFpsManager.DumpByPid(result, pid);
    surfaceFpsManager.DumpByPid(result, upid);
    surfaceFpsManager.ClearDump(result, id);
    surfaceFpsManager.ClearDump(result, uid);
    surfaceFpsManager.ClearDump(result, name);
    surfaceFpsManager.ClearDump(result, uname);
    surfaceFpsManager.ClearDumpByPid(result, pid);
    surfaceFpsManager.ClearDumpByPid(result, upid);
    surfaceFpsManager.GetSurfaceFps(id);
    surfaceFpsManager.GetSurfaceFps(uid);
    surfaceFpsManager.GetSurfaceFps(name, isUnique);
    surfaceFpsManager.GetSurfaceFps(uname, isUnique);
    surfaceFpsManager.GetSurfaceFpsByPid(pid);
    surfaceFpsManager.GetSurfaceFpsByPid(upid);
    surfaceFpsManager.UnregisterSurfaceFps(id);
    surfaceFpsManager.UnregisterSurfaceFps(uid);
    surfaceFpsManager.DumpSurfaceNodeFps(result, option, arg);
    surfaceFpsManager.ClearSurfaceNodeFps(result, option, arg);
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

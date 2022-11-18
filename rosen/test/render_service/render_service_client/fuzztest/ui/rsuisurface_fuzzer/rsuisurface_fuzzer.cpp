/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rsuisurface_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

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

bool RSUIDirectorFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool shouldCreateRenderThread = GetData<bool>();
    RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    uint8_t alpha = GetData<uint8_t>();
    bool isRenderForced = GetData<bool>();
    bool hasContainerWindow = GetData<bool>();
    float density = GetData<float>();
    NodeId root = GetData<NodeId>();
    uint64_t timeStamp = GetData<uint64_t>();
    std::string abilityName = GetStringFromData(STR_LEN);
    std::string cacheFilePath = GetStringFromData(STR_LEN);
    bool isAppFreeze = GetData<bool>();

    std::shared_ptr<RSUIDirector> uiDirector = RSUIDirector::Create();
    uiDirector->Init(shouldCreateRenderThread);
    uiDirector->SetRSSurfaceNode(surfaceNode);
    uiDirector->SetAbilityBGAlpha(alpha);
    uiDirector->SetRTRenderForced(isRenderForced);
    uiDirector->SetContainerWindow(hasContainerWindow, density);
    uiDirector->SetRoot(root);
    uiDirector->SetTimeStamp(timeStamp, abilityName);
    uiDirector->SetCacheDir(cacheFilePath);
    uiDirector->RunningCustomAnimation(timeStamp);
    uiDirector->SetAppFreeze(isAppFreeze);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSUIDirectorFuzzTest(data, size);
    return 0;
}

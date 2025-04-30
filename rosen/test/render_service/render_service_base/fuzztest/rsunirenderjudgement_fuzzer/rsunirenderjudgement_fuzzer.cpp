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

#include "rsunirenderjudgement_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "pipeline/rs_uni_render_judgement.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_GET_UNI_RENDER_ENABLED_TYPE = 0;
const uint8_t DO_SAFE_GET_LINE = 1;
const uint8_t TARGET_SIZE = 2;

const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

void DoGetUniRenderEnabledType()
{
    RSUniRenderJudgement::GetUniRenderEnabledType();
    RSUniRenderJudgement::InitUniRenderConfig();
    RSUniRenderJudgement::IsUniRender();
    RSUniRenderJudgement::InitUniRenderWithConfigFile();
}

void DoSafeGetLine()
{
    std::string line;
    std::ifstream configFile("path_to_your_file.txt");
    RSUniRenderJudgement::SafeGetLine(configFile, line);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_UNI_RENDER_ENABLED_TYPE:
            OHOS::Rosen::DoGetUniRenderEnabledType();
            break;
        case OHOS::Rosen::DO_SAFE_GET_LINE:
            OHOS::Rosen::DoSafeGetLine();
            break;
        default:
            return -1;
    }
    return 0;
}

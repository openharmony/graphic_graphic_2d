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

#include "rsbasecommon_fuzzer.h"

#include <securec.h>
#include <cstddef>
#include <cstdint>

#ifdef NEW_SKIA
#include "include/core/SkM44.h"
#else
#include "include/core/SkMatrix44.h"
#endif
#include "include/utils/SkCamera.h"
#include "rs_thread_handler_generic.h"
#include "rs_thread_looper_generic.h"
#include "rs_thread_looper_impl.h"

#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_occlusion_region.h"

namespace OHOS {
namespace Rosen {
namespace {
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
bool RSColorFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    uint32_t rgba1 = GetData<uint32_t>();
    float alpha1 = GetData<float>();
    int16_t blue = GetData<int16_t>();
    int16_t green = GetData<int16_t>();
    int16_t red = GetData<int16_t>();
    int16_t alpha2 = GetData<int16_t>();

    // Test
    RSColor rscolor = RSColor(red, green, blue);

    (void)rscolor.FromArgbInt(rgba1);
    (void)rscolor.FromRgbaInt(rgba1);
    (void)rscolor.FromBgraInt(rgba1);
    rscolor.SetBlue(blue);
    rscolor.SetAlpha(alpha2);
    rscolor.SetGreen(green);
    rscolor.SetRed(red);
    rscolor.MultiplyAlpha(alpha1);

    return true;
}

bool RSObjAbsGeometryFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    Drawing::Matrix matrix;
    float offsetX = GetData<float>();
    float offsetY = GetData<float>();
    RectF rect;

    // Test
    RSObjAbsGeometry rsobjabsgeometry;
    rsobjabsgeometry.ConcatMatrix(matrix);
    rsobjabsgeometry.UpdateMatrix(&matrix, Drawing::Point(offsetX, offsetY));
    (void)rsobjabsgeometry.MapAbsRect(rect);

    return true;
}

bool RSObjOcclusionFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    int y1 = GetData<int>();
    int l1 = GetData<int>();
    int r1 = GetData<int>();
    int y2 = GetData<int>();
    int l2 = GetData<int>();
    int r2 = GetData<int>();
    Occlusion::Event event1 = Occlusion::Event(y1, Occlusion::Event::Type::OPEN, l1, r1);
    Occlusion::Event event2 = Occlusion::Event(y2, Occlusion::Event::Type::OPEN, l2, r2);
    float s1 = GetData<float>();
    float e1 = GetData<float>();
    std::shared_ptr<Occlusion::Node> node = std::make_shared<Occlusion::Node>(s1, e1);
    float s2 = GetData<float>();
    float e2 = GetData<float>();
    std::vector<Occlusion::Range> res = { Occlusion::Range(s2, e2) };
    bool isParentNodePos = GetData<bool>();
    bool isParentNodeNeg = GetData<bool>();

    // Test
    (void)EventSortByY(event1, event2);
    node->PushRange(res);
    node->GetAndRange(res, isParentNodePos, isParentNodeNeg);
    node->GetOrRange(res, isParentNodePos, isParentNodeNeg);
    node->GetXOrRange(res, isParentNodePos, isParentNodeNeg);
    node->GetSubRange(res, isParentNodePos, isParentNodeNeg);

    return true;
}

bool RSThreadHandlerGenericFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    RSThreadHandlerGeneric rsthreadhandlergeneric;
    std::shared_ptr<RSTaskMessage> taskHandle = std::make_shared<RSTaskMessage>();
    RSTaskMessage::RSTask task;
    int param = GetData<int>();
    int64_t nsecs = GetData<int64_t>();

    // Test
    (void)rsthreadhandlergeneric.CreateTask(task);
    rsthreadhandlergeneric.PostTask(taskHandle, param);
    rsthreadhandlergeneric.PostTaskDelay(taskHandle, nsecs, param);
    rsthreadhandlergeneric.CancelTask(taskHandle);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSColorFuzzTest(data, size);
    OHOS::Rosen::RSObjAbsGeometryFuzzTest(data, size);
    OHOS::Rosen::RSObjOcclusionFuzzTest(data, size);
    OHOS::Rosen::RSThreadHandlerGenericFuzzTest(data, size);
    return 0;
}

/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "rscpuoverdrawcanvaslistener_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"
#include <include/core/SkDrawable.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRegion.h>
#include <include/core/SkData.h>

using namespace OHOS::Rosen;
namespace OHOS {
namespace {
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos;
}
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
    size_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

#ifndef USE_ROSEN_DRAWING
void RSCpuOverdrawCanvasListenerFuzzTest0(RSCPUOverdrawCanvasListener& inListener)
{
    uint32_t inColor1 = GetData<uint32_t>();
    SkColor4f inSkColor = SkColor4f::FromColor(inColor1);
    SkPaint inSkpaint1 = SkPaint(inSkColor);
    SkScalar inSkScrlarX1 = GetData<float>();
    SkScalar inSkScrlarY1 = GetData<float>();
    SkScalar inSkScrlarW1 = GetData<float>();
    SkScalar inSkScrlarH1 = GetData<float>();
    SkRect inSkrect = SkRect::MakeXYWH(inSkScrlarX1, inSkScrlarY1, inSkScrlarW1, inSkScrlarH1);
    bool inIsUseCenter = GetData<bool>();
    SkScalar inSkScrlarStartAngle = GetData<float>();
    SkScalar inSkScrlarSweepAngle = GetData<float>();
    inListener.onDrawArc(inSkrect, inSkScrlarStartAngle, inSkScrlarSweepAngle, inIsUseCenter, inSkpaint1);
    inListener.onDrawBehind(inSkpaint1);

    SkScalar inSkScrlarX2 = GetData<float>();
    SkScalar inSkScrlarY2 = GetData<float>();
    SkScalar inSkScrlarW2 = GetData<float>();
    SkScalar inSkScrlarH2 = GetData<float>();
    SkRect inSkrect2 = SkRect::MakeXYWH(inSkScrlarX2, inSkScrlarY2, inSkScrlarW2, inSkScrlarH2);
    SkRRect inSkRrect = SkRRect::MakeRect(inSkrect);
    SkRRect inSkRrect2 = SkRRect::MakeRect(inSkrect2);
    inListener.onDrawDRRect(inSkRrect, inSkRrect2, inSkpaint1);
    inListener.onDrawOval(inSkrect, inSkpaint1);

    SkScalar inSkScrlarX3 = GetData<float>();
    SkScalar inSkScrlarY3 = GetData<float>();
    SkScalar inSkScrlarX4 = GetData<float>();
    SkScalar inSkScrlarY4 = GetData<float>();
    SkScalar inSkScrlarX5 = GetData<float>();
    SkScalar inSkScrlarY5 = GetData<float>();
    SkScalar inSkScrlarX6 = GetData<float>();
    SkScalar inSkScrlarY6 = GetData<float>();
    SkPoint inSkPoints[4] = {SkPoint::Make(inSkScrlarX3, inSkScrlarY3),
        SkPoint::Make(inSkScrlarX4, inSkScrlarY4),
        SkPoint::Make(inSkScrlarX5, inSkScrlarY5),
        SkPoint::Make(inSkScrlarX6, inSkScrlarY6),
    };
    uint32_t inSkBlendMode = GetData<uint32_t>();
    uint32_t inQuadAAFlags = GetData<uint32_t>();
    inListener.onDrawEdgeAAQuad(inSkrect2, inSkPoints, static_cast<SkCanvas::QuadAAFlags>(inQuadAAFlags),
        inSkColor, static_cast<SkBlendMode>(inSkBlendMode));
    inListener.onDrawRect(inSkrect2, inSkpaint1);
    inListener.onDrawRRect(inSkRrect, inSkpaint1);
}

void RSCpuOverdrawCanvasListenerFuzzTest1(RSCPUOverdrawCanvasListener& inListener)
{
    uint32_t inColor2 = GetData<uint32_t>();
    SkColor4f inSkColo2 = SkColor4f::FromColor(inColor2);
    SkPaint inSkpaint2 = SkPaint(inSkColo2);
    sk_sp<SkDrawable> inSkDrawable = SkDrawable::Deserialize(nullptr, 0);
    SkMatrix _skmatrix = SkMatrix::MakeAll(GetData<float>(), GetData<float>(), GetData<float>(),
                                GetData<float>(), GetData<float>(), GetData<float>(),
                                GetData<float>(), GetData<float>(), GetData<float>());
    inListener.onDrawDrawable(inSkDrawable.get(), &_skmatrix);

    SkScalar inSkScrlarX3 = GetData<float>();
    SkScalar inSkScrlarY3 = GetData<float>();
    SkScalar inSkScrlarW3 = GetData<float>();
    SkScalar inSkScrlarH3 = GetData<float>();
    SkRect inSkrect3 = SkRect::MakeXYWH(inSkScrlarX3, inSkScrlarY3, inSkScrlarW3, inSkScrlarH3);
    sk_sp<SkPicture> _skpicture = SkPicture::MakePlaceholder(inSkrect3);
    inListener.onDrawPicture(_skpicture.get(), &_skmatrix, &inSkpaint2);
    inListener.onDrawPaint(inSkpaint2);

    SkPath inSkPath;
    inListener.onDrawPath(inSkPath, inSkpaint2);
    
    uint32_t inLeft = GetData<uint32_t>();
    uint32_t inTop = GetData<uint32_t>();
    uint32_t inRight = GetData<uint32_t>();
    uint32_t inButtom = GetData<uint32_t>();
    SkIRect inSkIRrect = SkIRect::MakeLTRB(inLeft, inTop, inRight, inButtom);
    SkRegion _skregion(inSkIRrect);
    inListener.onDrawRegion(_skregion, inSkpaint2);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 0) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    SkCanvas inSkcanvs;
    RSCPUOverdrawCanvasListener nowListener(inSkcanvs);
    RSCpuOverdrawCanvasListenerFuzzTest0(nowListener);
    RSCpuOverdrawCanvasListenerFuzzTest1(nowListener);
    return true;
}

#else
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    return true;
}
#endif
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}


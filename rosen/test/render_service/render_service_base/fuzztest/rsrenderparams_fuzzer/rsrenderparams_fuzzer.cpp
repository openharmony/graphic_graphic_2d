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

#include "rsrenderparams_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "params/rs_render_params.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    NodeId id = 0;

    RSRenderParams rsRenderParams(id);
    float alpha = 1.0;
    rsRenderParams.SetAlpha(alpha);
    rsRenderParams.GetAlpha();
    rsRenderParams.SetAlphaOffScreen(false);
    const Drawing::Matrix matrix;
    rsRenderParams.SetMatrix(matrix);
    rsRenderParams.GetMatrix();
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    rsRenderParams.ApplyAlphaAndMatrixToCanvas(canvas, false);
    const Drawing::RectF boundsRect;
    rsRenderParams.SetBoundsRect(boundsRect);
    rsRenderParams.GetBounds();
    const Drawing::RectF frameRect;
    rsRenderParams.SetFrameRect(frameRect);
    rsRenderParams.GetFrameRect();
    const RectF localDrawRect;
    rsRenderParams.SetLocalDrawRect(localDrawRect);
    rsRenderParams.GetLocalDrawRect();
    rsRenderParams.SetHasSandBox(false);
    rsRenderParams.HasSandBox();
    rsRenderParams.SetShouldPaint(false);
    rsRenderParams.GetShouldPaint();
    rsRenderParams.SetContentEmpty(false);
    rsRenderParams.SetChildHasVisibleFilter(false);
    rsRenderParams.ChildHasVisibleFilter();
    rsRenderParams.SetChildHasVisibleEffect(false);
    rsRenderParams.ChildHasVisibleEffect();
    rsRenderParams.SetDrawingCacheChanged(false, false);
    rsRenderParams.GetDrawingCacheChanged();
    rsRenderParams.SetNeedUpdateCache(false);
    rsRenderParams.GetNeedUpdateCache();
    return true;
}

bool DoSetShadowRect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    NodeId id = 0;

    RSRenderParams rsRenderParams(id);
    float alpha = 1.0;
    const Drawing::Matrix matrix;
    Drawing::Rect rect;
    rsRenderParams.SetShadowRect(rect);
    rsRenderParams.GetShadowRect();
    rsRenderParams.SetNeedSync(false);
    Gravity gravity = Gravity::DEFAULT;
    rsRenderParams.SetFrameGravity(gravity);
    rsRenderParams.SetNeedFilter(false);
    RSRenderNodeType type = RSRenderNodeType::RS_NODE;
    rsRenderParams.SetNodeType(type);
    rsRenderParams.SetEffectNodeShouldPaint(false);
    rsRenderParams.SetHasGlobalCorner(false);
    rsRenderParams.SetHasBlurFilter(false);
    rsRenderParams.SetGlobalAlpha(alpha);
    rsRenderParams.NeedSync();
    std::unique_ptr<RSRenderParams> target;
    rsRenderParams.OnCanvasDrawingSurfaceChange(target);
    rsRenderParams.GetCanvasDrawingSurfaceChanged();
    rsRenderParams.SetCanvasDrawingSurfaceChanged(true);
    rsRenderParams.GetForegroundFilterCache();
    std::shared_ptr<RSFilter> foregroundFilterCache;
    rsRenderParams.SetForegroundFilterCache(foregroundFilterCache);
    rsRenderParams.GetCanvasDrawingSurfaceParams();
    int width = 1;
    int height = 2;
    rsRenderParams.SetCanvasDrawingSurfaceParams(width, height);
    rsRenderParams.ToString();
    rsRenderParams.SetParentSurfaceMatrix(matrix);
    rsRenderParams.GetParentSurfaceMatrix();
    rsRenderParams.GetLayerInfo();
    rsRenderParams.GetTotalMatrix();
    rsRenderParams.GetMirrorSourceDrawable();
    return true;
}

bool DoOpincSetCacheChangeFlag(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    NodeId id = 0;

    RSRenderParams rsRenderParams(id);
    rsRenderParams.OpincSetCacheChangeFlag(false, false);
    rsRenderParams.OpincGetCacheChangeState();
    rsRenderParams.OpincUpdateRootFlag(false);
    rsRenderParams.OpincGetRootFlag();
    rsRenderParams.SetRSFreezeFlag(false);
    rsRenderParams.GetRSFreezeFlag();
    rsRenderParams.SetFirstLevelNode(id);
    rsRenderParams.GetFirstLevelNodeId();
    rsRenderParams.SetUiFirstRootNode(id);
    rsRenderParams.GetUifirstRootNodeId();
    RSDrawingCacheType cacheType = RSDrawingCacheType::FORCED_CACHE;
    rsRenderParams.SetDrawingCacheType(cacheType);
    rsRenderParams.GetDrawingCacheType();
    rsRenderParams.SetDrawingCacheIncludeProperty(false);
    rsRenderParams.GetDrawingCacheIncludeProperty();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::Rosen::DoSetShadowRect(data, size);
    OHOS::Rosen::DoOpincSetCacheChangeFlag(data, size);
    return 0;
}
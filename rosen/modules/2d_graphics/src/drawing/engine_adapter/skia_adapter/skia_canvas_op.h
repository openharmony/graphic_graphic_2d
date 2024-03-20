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

#ifndef SKIACANVAS_OP_H
#define SKIACANVAS_OP_H

#include "include/utils/SkNWayCanvas.h"
#include "include/core/SkCanvasVirtualEnforcer.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaCanvasOp : public SkCanvasVirtualEnforcer<SkNWayCanvas> {
public:
    explicit SkiaCanvasOp(SkCanvas* canvas)
        : SkCanvasVirtualEnforcer<SkNWayCanvas>(canvas->imageInfo().width(),
        canvas->imageInfo().height()) {}

    virtual ~SkiaCanvasOp() = default;

    virtual bool OpCanCache(const SkRect& bound)
    {
        return false;
    }

    virtual std::vector<SkRect>& GetOpListDrawArea()
    {
        static std::vector<SkRect> defaultRects;
        return defaultRects;
    }

    virtual SkRect& GetOpUnionRect()
    {
        static SkRect defaultRect;
        return defaultRect;
    }

    virtual int GetOpsNum()
    {
        return 0;
    }

    virtual int GetOpsPercent()
    {
        return 0;
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif

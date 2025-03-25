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

#ifndef SAFUZZ_RANDOM_DRAWOP_H
#define SAFUZZ_RANDOM_DRAWOP_H

#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RandomDrawOp {
public:
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawOp();

private:
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawWithPaintOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawShadowStyleOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawPointOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawPointsOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawLineOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawRoundRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawNestedRoundRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawArcOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawPieOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawOvalOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawCircleOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawPathOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawBackgroundOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawShadowOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawRegionOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawVerticesOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawColorOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawImageNineOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawImageLatticeOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawAtlasOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawBitmapOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawImageOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawImageRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawRecordCmdOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawPictureOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawTextBlobOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawSymbolOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomClipRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomClipIRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomClipRoundRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomClipPathOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomClipRegionOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomSetMatrixOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomResetMatrixOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomConcatMatrixOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomTranslateOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomScaleOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomRotateOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomShearOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomFlushOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomClearOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomSaveOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomSaveLayerOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomRestoreOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDiscardOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomClipAdaptiveRoundRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawImageWithParmOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawPixelMapWithParmOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawPixelMapRectOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawFuncOpItem();
    static std::shared_ptr<Drawing::DrawOpItem> GetRandomDrawSurfaceBufferOpItem();
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_DRAWOP_H

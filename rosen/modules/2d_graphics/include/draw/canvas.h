/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef CANVAS_H
#define CANVAS_H

#include <iostream>
#include <string>
#include <vector>

#include "drawing/draw/core_canvas.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Canvas : public CoreCanvas {
public:
    Canvas() {}
    Canvas(int32_t width, int32_t height) : CoreCanvas(width, height) {}

    virtual Canvas* GetRecordingCanvas() const
    {
        return nullptr;
    }

    void AddCanvas(Canvas* canvas)
    {
        if (canvas != nullptr) {
            pCanvasList_.push_back(canvas);
        }
    }

    void RemoveAll()
    {
        pCanvasList_.clear();
    }
    // constructor adopt a raw canvas ptr, using for ArkUI, should remove after rosen modifier provide drawing Canvas.
    explicit Canvas(void* rawCanvas) : CoreCanvas(rawCanvas) {}
    virtual ~Canvas() {
        RemoveAll();
    }

    /*
     * @brief        Restores Canvas Matrix and clip value state to count.
     * @param count  Depth of state stack to restore.
     */
    void RestoreToCount(uint32_t count)
    {
        for (uint32_t i = this->GetSaveCount(); i > count; i--) {
            this->Restore();
        }
    }

    virtual bool GetRecordingState() const
    {
        return recordingState_;
    }

    virtual void SetRecordingState(bool flag)
    {
        recordingState_ = flag;
    }
protected:
    std::vector<Canvas*> pCanvasList_;
    bool recordingState_ = false;
};

class DRAWING_API OverDrawCanvas : public Canvas {
public:
    OverDrawCanvas(std::shared_ptr<Drawing::Canvas> canvas)
    {
        BuildOverDraw(canvas);
    }
    virtual ~OverDrawCanvas() {}
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::OVER_DRAW;
    }
};

class AutoCanvasRestore {
public:
    AutoCanvasRestore(Canvas& canvas, bool doSave) : canvas_(canvas)
    {
        saveCount_ = canvas_.GetSaveCount();
        if (doSave) {
            canvas_.Save();
        }
    }
    ~AutoCanvasRestore()
    {
        canvas_.RestoreToCount(saveCount_);
    }

    AutoCanvasRestore(AutoCanvasRestore&&) = delete;
    AutoCanvasRestore(const AutoCanvasRestore&) = delete;
    AutoCanvasRestore& operator=(AutoCanvasRestore&&) = delete;
    AutoCanvasRestore& operator=(const AutoCanvasRestore&) = delete;

private:
    Canvas& canvas_;
    uint32_t saveCount_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif

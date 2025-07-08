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
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordCmd;

class AutoCanvasMatrixBrush {
public:
    AutoCanvasMatrixBrush(Canvas* canvas,
        const Matrix* matrix, const Brush* brush, const Rect& bounds);
    ~AutoCanvasMatrixBrush();

    AutoCanvasMatrixBrush(AutoCanvasMatrixBrush&&) = delete;
    AutoCanvasMatrixBrush(const AutoCanvasMatrixBrush&) = delete;
    AutoCanvasMatrixBrush& operator=(AutoCanvasMatrixBrush&&) = delete;
    AutoCanvasMatrixBrush& operator=(const AutoCanvasMatrixBrush&) = delete;

private:
    Canvas* canvas_;
    uint32_t saveCount_;
    Paint paintPen_;
    Paint paintBrush_;
};

class DRAWING_API Canvas : public CoreCanvas {
public:
    static constexpr int64_t INVALID_STENCIL_VAL{-1};

    Canvas() {}
    Canvas(DrawingType type) : CoreCanvas(type) {}
    Canvas(int32_t width, int32_t height) : CoreCanvas(width, height) {}

    virtual Canvas* GetRecordingCanvas() const;

    void AddCanvas(Canvas* canvas);

    void RemoveAll();
    virtual ~Canvas();

    /*
     * @brief        Restores Canvas Matrix and clip value state to count.
     * @param count  Depth of state stack to restore.
     */
    void RestoreToCount(uint32_t count);

    /*
     * @brief  Draw recordcmd.
     * @param recordCmd  Record command.
     * @param matrix  Matrix to rotate, scale, translate, and so on; may be nullptr.
     * @param brush Brush to apply transparency, filtering, and so on; must be nullptr now.
     */
    virtual void DrawRecordCmd(const std::shared_ptr<RecordCmd> recordCmd,
        const Matrix* matrix = nullptr, const Brush* brush = nullptr);

    virtual bool GetRecordingState() const;

    virtual void SetRecordingState(bool flag);

    virtual void SetOffscreen(bool isOffscreen);

    virtual bool GetOffscreen() const;

    virtual void SetUICapture(bool isUICapture);

    virtual bool GetUICapture() const;

    inline int64_t GetStencilVal() const noexcept
    {
        return stencilVal_;
    }

    inline void SetStencilVal(int64_t stencilVal) noexcept
    {
        stencilVal_ = stencilVal;
    }

    inline int64_t GetMaxStencilVal() const noexcept
    {
        return maxStencilVal_;
    }

    inline void SetMaxStencilVal(int64_t maxStencilVal) noexcept
    {
        maxStencilVal_ = maxStencilVal;
    }
protected:
    std::vector<Canvas*> pCanvasList_;
    bool recordingState_ = false;
    bool isOffscreen_ = false;
    bool isUICapture_ = false;
    int64_t stencilVal_ = INVALID_STENCIL_VAL;
    int64_t maxStencilVal_ = 0;
};

class DRAWING_API OverDrawCanvas : public Canvas {
public:
    OverDrawCanvas(std::shared_ptr<Drawing::Canvas> canvas) : Canvas(DrawingType::OVER_DRAW)
    {
        BuildOverDraw(canvas);
    }
    virtual ~OverDrawCanvas() {}
    virtual DrawingType GetDrawingType() const override
    {
        return DrawingType::OVER_DRAW;
    }
    void SetGrContext(std::shared_ptr<GPUContext> gpuContext)
    {
        gpuContext_ = gpuContext;
    }
#ifdef RS_ENABLE_GPU
    std::shared_ptr<GPUContext> GetGPUContext() override
    {
        return gpuContext_;
    }
#endif
private:
    std::shared_ptr<GPUContext> gpuContext_ = nullptr;
};

class DRAWING_API NoDrawCanvas : public Canvas {
public:
    NoDrawCanvas(int32_t width, int32_t height) : Canvas(DrawingType::NO_DRAW)
    {
        BuildNoDraw(width, height);
    }
    ~NoDrawCanvas() override = default;
    DrawingType GetDrawingType() const override
    {
        return DrawingType::NO_DRAW;
    }
};

class DRAWING_API StateInheriteCanvas : public Canvas {
public:
    StateInheriteCanvas(int32_t width, int32_t height)
    {
        BuildStateInherite(width, height);
    }
    ~StateInheriteCanvas() override = default;
    DrawingType GetDrawingType() const override
    {
        return DrawingType::INHERITE_STATE;
    }
};

class DRAWING_API StateRecordCanvas : public Canvas {
public:
    StateRecordCanvas(int32_t width, int32_t height)
    {
        BuildStateRecord(width, height);
    }
    ~StateRecordCanvas() override = default;
    DrawingType GetDrawingType() const override
    {
        return DrawingType::STATE_RECORD;
    }
#ifdef RS_ENABLE_GPU
    std::shared_ptr<GPUContext> GetGPUContext() override
    {
        return nullptr;
    }
#endif
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

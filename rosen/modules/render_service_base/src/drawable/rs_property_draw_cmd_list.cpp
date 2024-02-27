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

#include "drawable/rs_property_draw_cmd_list.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_content.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen {

void RSPropertyDrawCmdList::OnSync()
{
    if (!needSyncDisplayList_) {
        return;
    }
    drawCmdList_ = std::move(stagingDrawCmdList_);
}

class RSPropertyDrawCmdListRecorder {
public:
    // not copyable and moveable
    RSPropertyDrawCmdListRecorder(const RSPropertyDrawCmdListRecorder&) = delete;
    RSPropertyDrawCmdListRecorder(const RSPropertyDrawCmdListRecorder&&) = delete;
    RSPropertyDrawCmdListRecorder& operator=(const RSPropertyDrawCmdListRecorder&) = delete;
    RSPropertyDrawCmdListRecorder& operator=(const RSPropertyDrawCmdListRecorder&&) = delete;

    explicit RSPropertyDrawCmdListRecorder(int width, int height)
    {
        // PLANNING: use RSRenderNode to determine the correct recording canvas size
        recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(width, height, true);
        // PLANNING: add something like RSPaintNoFilter that inherits from RSPaintFilterCanvas
        paintFilterCanvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
    }

    virtual ~RSPropertyDrawCmdListRecorder()
    {
        if (paintFilterCanvas_) {
            ROSEN_LOGE("Object destroyed without calling EndRecording.");
        }
    }

    const std::unique_ptr<RSPaintFilterCanvas>& GetPaintFilterCanvas() const
    {
        return paintFilterCanvas_;
    }

    const std::unique_ptr<ExtendRecordingCanvas>& GetRecordingCanvas() const
    {
        return recordingCanvas_;
    }

    std::shared_ptr<Drawing::DrawCmdList>&& EndRecordingAndReturnRecordingList()
    {
        auto displayList = recordingCanvas_->GetDrawCmdList();
        paintFilterCanvas_.reset();
        recordingCanvas_.reset();
        return std::move(displayList);
    }

protected:
    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas_;
    std::unique_ptr<RSPaintFilterCanvas> paintFilterCanvas_;
};

class RSPropertyDrawCmdListUpdater : public RSPropertyDrawCmdListRecorder {
public:
    explicit RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawCmdList* target)
        : RSPropertyDrawCmdListRecorder(width, height), target_(target)
    {}
    ~RSPropertyDrawCmdListUpdater() override
    {
        if (recordingCanvas_) {
            target_->stagingDrawCmdList_ = EndRecordingAndReturnRecordingList();
            target_->needSyncDisplayList_ = true;
        } else {
            ROSEN_LOGE("Update failed, recording canvas is null!");
        }
    }

private:
    RSPropertyDrawCmdList* target_;
};

RSPropertyDrawCmdList::Ptr RSCustomModifierDrawCmdList::OnGenerate(
    const RSRenderContent& content, RSModifierType type)
{
    auto& drawCmdModifiers = content.drawCmdModifiers_;
    auto itr = drawCmdModifiers.find(type);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return nullptr;
    }
    RSPropertyDrawCmdListRecorder generator(0, 0);
    auto& recordingCanvas = generator.GetRecordingCanvas();
    for (const auto& modifier : itr->second) {
        // TODO: re-record content of modifier onto new canvas
        if (auto property =
                std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier->GetProperty())) {
            auto& drawCmdList = property->GetRef();
            recordingCanvas->DrawDrawFunc([drawCmdList](Drawing::Canvas* canvas, const Drawing::Rect* rect) -> void {
                drawCmdList->Playback(*canvas, rect);
            });
        }
    }
    return std::make_shared<RSCustomModifierDrawCmdList>(generator.EndRecordingAndReturnRecordingList(), type);
}

bool RSCustomModifierDrawCmdList::OnUpdate(const RSRenderContent& content)
{
    auto& drawCmdModifiers = content.drawCmdModifiers_;
    auto itr = drawCmdModifiers.find(type_);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    for (const auto& modifier : itr->second) {
        // TODO: re-record content of modifier onto new canvas
        (void)modifier;
    }
    return true;
}

RSPropertyDrawCmdList::Ptr RSBackgroundDrawCmdList::OnGenerate(const RSRenderContent& content)
{
    RSPropertyDrawCmdListRecorder generator(0, 0);
    RSPropertiesPainter::DrawBackground(content.GetRenderProperties(), *generator.GetPaintFilterCanvas());
    return std::make_shared<RSPropertyDrawCmdList>(generator.EndRecordingAndReturnRecordingList());
};

bool RSBackgroundDrawCmdList::OnUpdate(const RSRenderContent& content)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    RSPropertiesPainter::DrawBackground(content.GetRenderProperties(), *updater.GetPaintFilterCanvas());
    return true;
}

RSPropertyDrawCmdList::Ptr RSBorderDrawCmdList::OnGenerate(const RSRenderContent& content)
{
    RSPropertyDrawCmdListRecorder generator(0, 0);
    RSPropertiesPainter::DrawBorder(content.GetRenderProperties(), *generator.GetPaintFilterCanvas());
    return std::make_shared<RSPropertyDrawCmdList>(generator.EndRecordingAndReturnRecordingList());
};

bool RSBorderDrawCmdList::OnUpdate(const RSRenderContent& content)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    RSPropertiesPainter::DrawBorder(content.GetRenderProperties(), *updater.GetPaintFilterCanvas());
    return true;
}

} // namespace OHOS::Rosen

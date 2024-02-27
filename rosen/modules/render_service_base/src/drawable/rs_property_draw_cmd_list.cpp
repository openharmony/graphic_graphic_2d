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

bool RSPropertyDrawCmdList::Update(const RSRenderContent& content)
{
    // template: generate new stagingDrawCmdList_ if needed
    return false;
};

void RSPropertyDrawCmdList::OnSync()
{
    if (!stagingDrawCmdList_) {
        return;
    }
    std::swap(drawCmdList_, stagingDrawCmdList_);
    stagingDrawCmdList_.reset();
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
        } else {
            ROSEN_LOGE("Update failed, recording canvas is null!");
        }
    }

private:
    RSPropertyDrawCmdList* target_;
};

std::shared_ptr<RSPropertyDrawCmdList> RSCustomModifierDrawCmdList::Generate(
    const RSRenderContent& content, RSModifierType type)
{
    auto& drawCmdModifiers = content.drawCmdModifiers_;

    auto itr = drawCmdModifiers.find(type);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return nullptr;
    }
    RSPropertyDrawCmdListRecorder generator(0, 0);
    for (const auto& modifier : itr->second) {
        // TODO: re-record content of modifier onto new canvas
        (void)modifier;
    }
    return std::make_shared<RSCustomModifierDrawCmdList>(generator.EndRecordingAndReturnRecordingList(), type);
}

bool RSCustomModifierDrawCmdList::Update(const RSRenderContent& content)
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

std::shared_ptr<RSPropertyDrawCmdList> RSBackgroundDrawCmdList::Generate(const RSRenderContent& content)
{
    RSPropertyDrawCmdListRecorder generator(0, 0);
    RSPropertiesPainter::DrawBackground(content.GetRenderProperties(), *generator.GetPaintFilterCanvas());
    return std::make_shared<RSPropertyDrawCmdList>(generator.EndRecordingAndReturnRecordingList());
};

bool RSBackgroundDrawCmdList::Update(const RSRenderContent& content)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    RSPropertiesPainter::DrawBackground(content.GetRenderProperties(), *updater.GetPaintFilterCanvas());
    return true;
}

std::shared_ptr<RSPropertyDrawCmdList> RSBorderDrawCmdList::Generate(const RSRenderContent& content)
{
    RSPropertyDrawCmdListRecorder generator(0, 0);
    RSPropertiesPainter::DrawBorder(content.GetRenderProperties(), *generator.GetPaintFilterCanvas());
    return std::make_shared<RSPropertyDrawCmdList>(generator.EndRecordingAndReturnRecordingList());
};

bool RSBorderDrawCmdList::Update(const RSRenderContent& content)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    RSPropertiesPainter::DrawBorder(content.GetRenderProperties(), *updater.GetPaintFilterCanvas());
    return true;
}

} // namespace OHOS::Rosen

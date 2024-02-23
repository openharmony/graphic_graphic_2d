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

#include "drawable/rs_property_drawable_ng.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_content.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen {

bool RSPropertyDrawableNG::Update(const RSRenderContent& content)
{
    // template: generate new stagingDrawCmdList_ if needed
    return false;
};
void RSPropertyDrawableNG::OnSync()
{
    if (!stagingDrawCmdList_) {
        return;
    }
    std::swap(drawCmdList_, stagingDrawCmdList_);
    stagingDrawCmdList_.reset();
}
void RSPropertyDrawableNG::Draw(RSPaintFilterCanvas& canvas) const
{
    if (drawCmdList_) {
        drawCmdList_->Playback(canvas);
    }
}

class RSPropertyDrawableRecorderHelper {
public:
    // not copyable and moveable
    RSPropertyDrawableRecorderHelper(const RSPropertyDrawableRecorderHelper&) = delete;
    RSPropertyDrawableRecorderHelper(const RSPropertyDrawableRecorderHelper&&) = delete;
    RSPropertyDrawableRecorderHelper& operator=(const RSPropertyDrawableRecorderHelper&) = delete;
    RSPropertyDrawableRecorderHelper& operator=(const RSPropertyDrawableRecorderHelper&&) = delete;

    explicit RSPropertyDrawableRecorderHelper(int width, int height)
    {
        recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(width, height, true);
        // PLANNING: add something like RSPaintNoFilter that inherits from RSPaintFilterCanvas
        paintFilterCanvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
    }

    virtual ~RSPropertyDrawableRecorderHelper()
    {
        if (paintFilterCanvas_) {
            ROSEN_LOGE("Recording is not finished.");
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

class RSPropertyDrawableUpdaterHelper : public RSPropertyDrawableRecorderHelper {
public:
    explicit RSPropertyDrawableUpdaterHelper(int width, int height, RSPropertyDrawableNG* target)
        : RSPropertyDrawableRecorderHelper(width, height), target_(target)
    {}
    ~RSPropertyDrawableUpdaterHelper() override
    {
        target_->stagingDrawCmdList_ = EndRecordingAndReturnRecordingList();
    }

private:
    RSPropertyDrawableNG* target_;
};

std::shared_ptr<RSPropertyDrawableNG> RSBackgroundColorDrawableNG::Generate(const RSRenderContent& content)
{
    RSPropertyDrawableRecorderHelper helper(0, 0);
    RSPropertiesPainter::DrawBackground(content.GetRenderProperties(), *helper.GetPaintFilterCanvas());
    return std::make_shared<RSPropertyDrawableNG>(helper.EndRecordingAndReturnRecordingList());
};

bool RSBackgroundColorDrawableNG::Update(const RSRenderContent& content)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawableUpdaterHelper updater(0, 0, this);
    RSPropertiesPainter::DrawBackground(content.GetRenderProperties(), *updater.GetPaintFilterCanvas());
    return true;
}

std::shared_ptr<RSPropertyDrawableNG> RSBorderDrawableNG::Generate(const RSRenderContent& content)
{
    RSPropertyDrawableRecorderHelper helper(0, 0);
    RSPropertiesPainter::DrawBorder(content.GetRenderProperties(), *helper.GetPaintFilterCanvas());
    return std::make_shared<RSPropertyDrawableNG>(helper.EndRecordingAndReturnRecordingList());
};

bool RSBorderDrawableNG::Update(const RSRenderContent& content)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawableUpdaterHelper updater(0, 0, this);
    RSPropertiesPainter::DrawBorder(content.GetRenderProperties(), *updater.GetPaintFilterCanvas());
    return true;
}

} // namespace OHOS::Rosen

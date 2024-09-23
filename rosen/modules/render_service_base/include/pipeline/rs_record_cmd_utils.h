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

#ifndef RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RECORD_CMD_UTILS_H
#define RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RECORD_CMD_UTILS_H

#include <memory>
#include "recording/record_cmd.h"

namespace OHOS {

namespace Rosen {
class ExtendRecordingCanvas;
class RSB_EXPORT RSRecordCmdUtils {
public:
    RSRecordCmdUtils();
    ~RSRecordCmdUtils() {};
    /**
     * @brief Returns the canvas that records the drawing commands.
     * @param bounds the cull rect used when recording this recordcmd. Any drawing the falls outside
     * of this rect is undefined, and may be drawn or it may not.
     * @return pointer to ExtendRecordingCanvas, it will be released after FinishRecording is called,
     * and can't be used any more.
     */
    Drawing::Canvas* BeginRecording(Drawing::Rect& bounds);

    /**
     * @brief Returns the recordcmd that records the drawing commands.
     * @return A shared pointer to RecordCmd
     */
    std::shared_ptr<Drawing::RecordCmd> FinishRecording();
private:
    std::shared_ptr<ExtendRecordingCanvas> extendRecordingCanvas_;
    Drawing::Rect cullRect_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RECORD_CMD_UTILS_H

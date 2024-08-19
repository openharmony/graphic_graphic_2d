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

#ifndef RECORD_CMD_H
#define RECORD_CMD_H

#include "draw/canvas.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawCmdList;
class DRAWING_API RecordCmd {
public:
    RecordCmd(const std::shared_ptr<DrawCmdList>& cmdList, const Rect& bounds);

    ~RecordCmd() {};

    void Playback(Canvas* canvas);

    std::shared_ptr<DrawCmdList> GetDrawCmdList() const { return drawCmdList_; }

    const Rect& GetCullRect() { return cullRect_; };

private:
    std::shared_ptr<DrawCmdList> drawCmdList_ = nullptr;
    Rect cullRect_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif

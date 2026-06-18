/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef NODE_MEM_RELEASE_PARAM_H
#define NODE_MEM_RELEASE_PARAM_H

#include <map>

#include "feature_param.h"

namespace OHOS::Rosen {
class NodeMemReleaseParam : public FeatureParam {
public:
    NodeMemReleaseParam() = default;
    ~NodeMemReleaseParam() = default;

    static bool IsCanvasDrawingNodeDMAMemEnabled();
    static bool IsRsRenderNodeGCMemReleaseEnabled();
    static bool IsCanvasDrawingNodeBufferEnabled();
    static bool IsCanvasBufferEnabled(const std::string& processName);

protected:
    static void SetCanvasDrawingNodeDMAMemEnabled(bool isEnable);
    static void SetRsRenderNodeGCMemReleaseEnabled(bool isEnable);
    static void SetCanvasDrawingNodeBufferEnabled(bool isEnable);
    static void AddToCanvasBufferBlacklist(std::string processName, std::string value);

private:
    inline static bool isCanvasDrawingNodeDMAMemEnabled_ = true;
    inline static bool isRsRenderNodeGCMemReleaseEnabled_ = true;
    inline static bool isCanvasDrawingNodeBufferEnabled_ = true;
    inline static std::shared_ptr<std::map<std::string, std::string>> canvasBufferBlacklist_ = nullptr;

    friend class NodeMemReleaseParamParse;
};
} // namespace OHOS::Rosen
#endif // NODE_MEM_RELEASE_PARAM_H
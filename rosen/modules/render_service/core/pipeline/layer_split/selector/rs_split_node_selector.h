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
#ifndef RS_SPLIT_NODE_SELECTOR_H
#define RS_SPLIT_NODE_SELECTOR_H

#include <memory>
#if defined(ROSEN_OHOS)
namespace OHOS::Rosen {
class RSRenderNode;
class RSLayerSplitterPlanner;
class RSLayerSplitterProcessor;

namespace DrawableV2 {
class RSRenderNodeDrawable;
class RSScreenRenderNodeDrawable;
}

class SplitNodeSelector {
public:
    virtual ~SplitNodeSelector() = default;

    virtual std::shared_ptr<RSRenderNode> SelectParentNode() = 0;
    virtual void RecordSplitNode(std::shared_ptr<RSRenderNode> node) = 0;
    virtual std::shared_ptr<RSLayerSplitterProcessor> MakeProcessor() = 0;
    virtual std::shared_ptr<RSLayerSplitterPlanner> MakePlanner() = 0;
    virtual std::shared_ptr<RSRenderNode> GetCurrParentNode() = 0;
    virtual std::shared_ptr<RSRenderNode> GetLastParentNode() = 0;
    virtual void SetCurrParentNode(std::shared_ptr<RSRenderNode> currParentNode) = 0;
};

} // namespace OHOS::Rosen
#endif
#endif // RS_SPLIT_NODE_SELECTOR_H
/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_TREE_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_TREE_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <refbase.h>

#include "ipc/igraphic_dumper_client_listener.h"

namespace OHOS {
class GraphicDumperTree;
using TreeNodePtr = std::shared_ptr<GraphicDumperTree>;
using TreeNodeVisitFunc = std::function<void(const TreeNodePtr &v)>;
using TreeNodeMap = std::map<const std::string, TreeNodePtr>;

class GraphicDumperTree {
public:
    TreeNodePtr GetSetNode(const std::string str);
    bool HasNode(const std::string str);
    bool IsEmptyNode();
    void EraseNode(const std::string &tag);
    std::string GetTag();
    void SetValue(const std::string str);
    std::string GetValue();
    void AddListenerId(uint32_t &listenerId);
    void RemoveListenerId(uint32_t &listenerId);
    std::vector<uint32_t> GetListenerIds() const;
    void Foreach(TreeNodeVisitFunc func) const;

private:
    std::string tag_ = "";
    std::string value_ = "";
    std::unique_ptr<TreeNodeMap> nodeMap_ = nullptr;
    std::vector<uint32_t> listenerIds_ = {};
};
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_TREE_H

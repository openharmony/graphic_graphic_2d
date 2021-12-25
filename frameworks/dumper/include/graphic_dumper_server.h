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
#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_SERVER_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_SERVER_H

#include <any>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <thread>

#include <ipc_skeleton.h>
#include <refbase.h>

#include "graphic_dumper_tree.h"
#include "ipc/igraphic_dumper_client_listener.h"
#include "ipc/igraphic_dumper_command.h"
#include "ipc/igraphic_dumper_info_listener.h"
#include "ipc/igraphic_dumper_service.h"

namespace OHOS {
struct LogBuffer {
    bool canSave = false;
    bool needSave = false;
    bool sync = false;
    uint32_t index = 0;
    uint32_t size[2] = {0};
    std::mutex mutex[2];
    std::vector<std::unique_ptr<std::vector<std::string>>> vec;
    std::string mask = {};
};

struct LogBlock {
    uint32_t size;
    uint32_t length;
    uint8_t data[0];
};

struct ConfigTags {
    uint32_t id = 0;
    sptr<IGraphicDumperClientListener> listener = nullptr;
    std::vector<std::string> tags = {};
};

class GraphicDumperServer : public RefBase {
public:
    static sptr<GraphicDumperServer> GetInstance();
    int32_t Init();

    GSError AddConfigListener(const std::string &tag, sptr<IGraphicDumperClientListener> &listener);
    GSError RemoveConfigListener(sptr<IRemoteObject> object);
    GSError GetConfig(const std::string &k, std::string &v);
    GSError SetConfig(const std::string &k, const std::string &v);
    GSError Dump(const std::string &tag);

    GSError AddInfoListener(sptr<IGraphicDumperInfoListener> &listener);
    GSError RemoveInfoListener(const wptr<IRemoteObject> &object);
    GSError InfoHandle(const std::string &tag, const std::string &info);
    GSError GetLog(const std::string &tag, const std::string &info);

private:
    GraphicDumperServer() = default;
    virtual ~GraphicDumperServer() = default;
    static inline sptr<GraphicDumperServer> instance_;

    int32_t StartServer(int32_t systemAbility, sptr<IRemoteObject> obj);
    int32_t ReadDefaultConfig();
    void AddConfigDeathRecipient(sptr<IRemoteObject> &object);
    bool HaveConfigDeathRecipient(sptr<IRemoteObject> &object);
    bool HaveObject(const sptr<IRemoteObject> &obj);
    void VisitEach(const TreeNodePtr &node, std::function<void(const TreeNodePtr&)> func);
    void RemoveNode(std::vector<std::string> &vec, TreeNodePtr &sub, uint32_t &listenerId);
    void DispenseConfig(const TreeNodePtr &node, const std::string &k, const std::string &v);
    void DispenseDump(const TreeNodePtr &node, const std::string &tag);

    void AddInfoDeathRecipient(sptr<IRemoteObject> &object);
    bool HaveInfoDeathRecipient(sptr<IRemoteObject> &object);
    void SendToInfoListener(const std::string &info);
    static void SaveLog(std::any server);
    GSError LogHandle(const std::string &tag, const std::string &info);

private:
    std::mutex infoListenersMutex_;
    std::map<sptr<IRemoteObject>, sptr<IGraphicDumperInfoListener>> infoListeners_;
    std::map<sptr<IRemoteObject>, sptr<IRemoteObject::DeathRecipient>> infoListenerDeathRecipientMap_;

    uint32_t objectId_ = 0;
    std::map<uint32_t, ConfigTags> configTagsMap_;
    std::map<sptr<IRemoteObject>, uint32_t> objectIdMap_;
    std::mutex treeMutex_;
    const TreeNodePtr root = std::make_shared<GraphicDumperTree>();

    LogBuffer logBuf_;
    uint32_t logBlockSize_ = 0;
    std::mutex logBlockVectorMutex_;
    std::vector<std::unique_ptr<uint8_t[]>> logBlockVector_;
    std::unique_ptr<std::thread> thread_ = nullptr;
    std::map<sptr<IRemoteObject>, sptr<IRemoteObject::DeathRecipient>> configListenerDeathRecipientMap_;
};
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_SERVER_H

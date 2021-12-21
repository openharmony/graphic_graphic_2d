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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_IMPL_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_IMPL_H

#include <atomic>
#include <mutex>
#include <string>
#include <map>
#include <memory>

#include <graphic_dumper_helper.h>

#include "ipc/graphic_dumper_service_proxy.h"
#include "ipc/graphic_dumper_client_listener_stub.h"

namespace OHOS {
namespace {
class GraphicDumperClientListener;
}

using ConfigFuncMapPtr = std::unique_ptr<std::map<int32_t, OnConfigChangeFunc>>;
using DumpFuncMapPtr = std::unique_ptr<std::map<int32_t, OnDumpFunc>>;

class GraphicDumperHelperImpl : public GraphicDumperHelper {
    friend class GraphicDumperClientListener;
public:
    static sptr<GraphicDumperHelper> GetInstance();

    virtual GSError SendInfo(const std::string &tag, const char *fmt, ...) override;
    virtual int32_t AddConfigChangeListener(const std::string &tag, OnConfigChangeFunc func) override;
    virtual GSError RemoveConfigChangeListener(const int32_t listenerId) override;
    virtual int32_t AddDumpListener(const std::string &tag, OnDumpFunc func) override;
    virtual GSError RemoveDumpListener(const int32_t listenerId) override;

    void SetConnectState(bool state);

private:
    virtual ~GraphicDumperHelperImpl() = default;
    static inline sptr<GraphicDumperHelper> currentHelper = nullptr;
    static void SetNoopInstance();

    GSError Init();
    GSError InitSA(int32_t systemAbilityId);
    GSError AddClientListener(const std::string &tag);
    void DispenseOnConfigChange(const std::string &tag, const std::string &val);
    void DispenseOnDump(const std::string &tag);

    std::mutex initMutex_;
    std::atomic_int64_t requestConnectTime = 0;
    std::atomic_bool serverConnected = false;
    sptr<IGraphicDumperService> service_ = nullptr;

    std::mutex clientListenerMutex_;
    sptr<IGraphicDumperClientListener> listener_ = nullptr;

    int32_t onConfigChangeFuncId_ = 0;
    std::mutex onConfigChangeMutex_;
    std::map<std::string, ConfigFuncMapPtr> onConfigChangeMap_;

    int32_t onDumperFuncId_ = 0;
    std::mutex onDumperFuncsMutex_;
    std::map<std::string, DumpFuncMapPtr> onDumpFuncsMap_;

    struct InfoStruct {
        std::string tag;
        std::string info;
    };
    std::mutex cacheInfoMutex_;
    std::vector<InfoStruct> cacheInfo_;
};

class GDumperServiceDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    GDumperServiceDeathRecipient() = default;
    virtual ~GDumperServiceDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;
};

class GraphicDumperHelperNoop : public GraphicDumperHelper {
public:
    virtual GSError SendInfo(const std::string &tag, const char *fmt, ...)
    {
        return GSERROR_CONNOT_CONNECT_SERVER;
    }
    virtual int32_t AddConfigChangeListener(const std::string &tag, OnConfigChangeFunc func)
    {
        return 0;
    }
    virtual GSError RemoveConfigChangeListener(const int32_t listenerId)
    {
        return GSERROR_CONNOT_CONNECT_SERVER;
    }
    virtual int32_t AddDumpListener(const std::string &tag, OnDumpFunc func)
    {
        return 0;
    }
    virtual GSError RemoveDumpListener(const int32_t listenerId)
    {
        return GSERROR_CONNOT_CONNECT_SERVER;
    }
};

namespace {
class GraphicDumperClientListener : public GraphicDumperClientListenerStub {
public:
    GraphicDumperClientListener(sptr<GraphicDumperHelperImpl>& helper);
    virtual ~GraphicDumperClientListener() = default;

    void OnConfigChange(const std::string &tag, const std::string &val) override;
    void OnDump(const std::string &tag) override;

private:
    sptr<GraphicDumperHelperImpl> helperImpl_ = nullptr;
};
} // namespace
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_IMPL_H

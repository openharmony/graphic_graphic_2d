/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <memory>
#include <parameters.h>
#include <platform/common/rs_log.h>
#include <securec.h>
#include <string>
#include <sys/mman.h>
#include <sys/syscall.h>

#include "system/rs_system_parameters.h"

#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
#ifdef RS_ENABLE_RDO
#define MAX_PATH_LENGTH 128
constexpr const char *RDOPARAM = "persist.graphic.profiler.renderservice.rdo.enable";
constexpr const char *RDOINITPARAM = "persist.graphic.profiler.renderservice.rdo.init.successed";
static char g_codeCachePath[MAX_PATH_LENGTH] = "/system/lib64/librender_service_codeCache.so";
static char g_linkInfoPath[MAX_PATH_LENGTH] = "/system/etc/librender_service_linkInfo.bin";
static char g_binXOLoaderPath[MAX_PATH_LENGTH] = "/vendor/lib64/libbinxo_ld.so";
 
#ifndef NSIG
#define NSIG 64
#endif
static struct sigaction g_oldSigactionList[NSIG] = {};
static const int RDO_SIGCHAIN_CRASH_SIGNAL_LIST[] = {
    SIGILL, SIGABRT, SIGBUS, SIGFPE, SIGSEGV, SIGSTKFLT, SIGSYS, SIGTRAP};
constexpr size_t kSignalListSize = sizeof(RDO_SIGCHAIN_CRASH_SIGNAL_LIST) / sizeof(RDO_SIGCHAIN_CRASH_SIGNAL_LIST[0]);

static void SetRDOParam(const char* value)
{
    system::SetParameter(RDOPARAM, value);
}
 
static bool IsRDOEnable()
{
    std::string rdoFlagValue = system::GetParameter(RDOPARAM, "undef");
    RS_LOGI("[RDO] Is RDO Enable: %{public}s", rdoFlagValue.c_str());
    std::string rdoInitValue = system::GetParameter(RDOINITPARAM, "undef");
    RS_LOGI("[RDO] Is RDO init successfully: %{public}s", rdoInitValue.c_str());
    return (rdoFlagValue == "true" && rdoInitValue == "true");
}
 
static void ResetAndRethrowSignalIfNeed(int signo, siginfo_t* si)
{
    if (g_oldSigactionList[signo].sa_sigaction == nullptr) {
        signal(signo, SIG_DFL);
    } else if (sigaction(signo, &(g_oldSigactionList[signo]), nullptr) != 0) {
        RS_LOGI("[RDO] Failed to reset signo(%{public}d).", signo);
        signal(signo, SIG_DFL);
    }
 
    if (syscall(SYS_rt_tgsigqueueinfo, syscall(SYS_getpid), syscall(SYS_gettid), signo, si) != 0) {
        RS_LOGI("[RDO] Failed to rethrow signo(%{public}d), errno(%{public}d).", signo, errno);
    } else {
        RS_LOGI("[RDO] Current process(%{public}ld) rethrow signo(%{public}d).", syscall(SYS_getpid), signo);
    }
}
 
static void RDOSigchainHandler(int signo, siginfo_t* si, void* context)
{
    SetRDOParam("false");
    ResetAndRethrowSignalIfNeed(signo, si);
}
 
static void InstallSigActionHandler(int signo)
{
    struct sigaction action;
    memset_s(&action, sizeof(action), 0, sizeof(action));
    sigfillset(&action.sa_mask);
    action.sa_sigaction = RDOSigchainHandler;
    action.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
    if (sigaction(signo, &action, &(g_oldSigactionList[signo])) != 0) {
        RS_LOGI("[RDO] Failed to register signal(%{public}d)", signo);
    }
}
 
static void RDOInstallSignalHandler()
{
    for (size_t i = 0; i < kSignalListSize; i++) {
        int32_t signo = RDO_SIGCHAIN_CRASH_SIGNAL_LIST[i];
        InstallSigActionHandler(signo);
    }
}
 
void* HelperThreadforBinXO(void* arg)
{
    // For feature RDO:
    // step1: dlopen libbinxo_ld.so
    // step2: use func in libbinxo_ld.so to read linkinfo from librs_linkInfo.bin
    // step3: use func in libbinxo_ld.so to load librs_codeCache.so
    // step4: use func in libbinxo_ld.so to modify target so plt to enable rdo
    RS_LOGI("[RDO] Start RDO");

    if (!IsRDOEnable()) {
        RS_LOGI("[RDO] RDO is not enabled");
        return nullptr;
    }
    system::SetParameter(RDOINITPARAM, "false");
    RDOInstallSignalHandler();
    RS_LOGI("[RDO] RDOInstallSignalHandler true");
    char canonicalPath[PATH_MAX] = { 0 };
    if (realpath(g_binXOLoaderPath, canonicalPath) == nullptr) {
        RS_LOGI("[RDO] Failed to canonicalize path");
        return nullptr;
    }
    void* handle = dlopen(canonicalPath, RTLD_NOW);
    if (!handle) {
        RS_LOGI("[RDO] dlopen libbinxo_ld.so failed");
        return nullptr;
    }

    void (*readInfo)(const char *) =
        reinterpret_cast<void (*)(const char *)>(dlsym(handle, "_ZN11BinXOLoader12ReadLinkInfoEPc"));
    void (*loadCC)(const char *) =
        reinterpret_cast<void (*)(const char *)>(dlsym(handle, "_ZN11BinXOLoader13LoadCodeCacheEPc"));
    void (*linkAll)() = reinterpret_cast<void (*)()>(dlsym(handle, "_ZN11BinXOLoader7LinkAllEv"));

    if (!readInfo || !loadCC || !linkAll) {
        RS_LOGI("[RDO] Failed to load rdo function");
        dlclose(handle);
        return nullptr;
    }
    // Read linkinfo from  librs_linkInfo.bin
    readInfo(g_linkInfoPath);
    // Load librender_service_codeCache.so
    loadCC(g_codeCachePath);
    // Link all to enable rdo
    linkAll();

    dlclose(handle);

    system::SetParameter(RDOINITPARAM, "true");
    return nullptr;
}
 
__attribute__((visibility("default"))) int32_t EnableRSCodeCache()
{
    // Create a helper thread to load modifications for feature RDO.
    pthread_t id = 0;
    int ret = pthread_create(&id, nullptr, HelperThreadforBinXO, nullptr);
    if (ret != 0) {
        RS_LOGE("[RDO] pthread_create failed with error code %d", ret);
    }
    pthread_setname_np(id, "binxo_helper_thread");
    pthread_detach(id);
    return ret;
}
#endif
} // namespace Rosen
} // namespace OHOS
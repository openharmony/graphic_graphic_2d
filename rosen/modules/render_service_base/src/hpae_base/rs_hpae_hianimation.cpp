#include <dlfcn.h>
#include "hpae_base/rs_hpae_hianimation.h"
#include "hpae_base/rs_hpae_log.h"
#include "cpp/ffrt_dynamic_graph.h"
#include "hpae_base/rs_hpae_perf_thread.h"

namespace OHOS::Rosen {
using namespace std::chrono_literals;

using GetHianimationDeviceFunc = hianimation_algo_device_t* (*)();
constexpr int MAX_INIT_TIMES = 3;
constexpr int HPAE_BLUR_DELAY = 2;

HianimationManager& HianimationManager::GetInstance()
{
    static HianimationManager mgr;
    return mgr;
}

HianimationManager::HianimationManager()
{
}

HianimationMnager::~HianimationManager()
{
    CloseDevice();

    if (libHandle_ != nullptr) {
        dlclose(libHandle_);
    }
    hianimationDevice_ = nullptr;
}

void HianimationMnager::WaitHpaeDone()
{
    std::unque_lock<std::mutex> lock(hpaePerfMutex_);
    if (!hpaePerfDone_) {
        hpaePerfCv_.wait_for(lock, 100ms, [this]() {
            return this->hpaePerfDone_;
        });
    }
}

void HianimationMnager::NotifyHpaeDone()
{
    std::unique_lock<std::mutex> lock(hpaePerfMutex_);
    hpaePerfDone_ = true;
    hpaePerfCv_.notify_all();
}

void HianimationManager::OpenDeviceAsync()
{
    ffrt::submit_h([this]() {
        this->OpenDevice();
    }, {}, {});
}

void HianimationManager::AlgoInitAsync(uint32_t imgWidth, uint32_t imgWeight, float maxSigma, uint32_t format)
{
    {
        std::unique_lock<std::mutex> lock(algoInitMutex_);
        algoInitDone_ = false;
    }

    ffrt::submit_h([=](){
        std::unique_lock<std::mutex> lock(algoInitMutex_);
        this->HianimationAlgoInit(imgWidth, imgHeight, maxSigma, format);
        this->algoInitDone_ = true;
        this->algoInitCv_.notify_all();
    }, {}, {}, ffrt::task_attr().qos(5));
}

void HianimationManager::WaitAlgoInit()
{
    using namespace std::chrono_literals;
    std::unique_lock<std::mutex> lock(algInitMutex_);
    if (!algoInitDone_) {
        HPAE_TRACE_NAME("WaitAlgoInit");
        algoInitCv_.wait_for(lock, 30ms, [this]() {
            return this->algoInitDone_;
        });
    }
}

void HianimationManager::AlgoDeInitAsync()
{
    ffrt::submit_h([this]() {
        this->HianimationAlgoDeInit();
    }, {}, {});
}

bool HianimationManager::HianimationInvalid()
{
    std::unique_lock<std::mutex> lock(mutex);
    return openFailNum_ > MAX_INIT_TIMES;
}

void HianimationManager::OpenDevice()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (hianimationDevice_ != nullptr) {
        // do nothing
        return;
    }

    if (openFailNum_ > MAX_INIT_TIMES) {
        // avoid try too many times
        return;
    }

    HPAE_TRACE_NAME("Hianimation: OpenDevice");

    if (libHandle_ != nullptr) {
        HPAE_LOGW("[%{public}s_%{public}d]:exception and close library", __func__, __LINE__);
        dlclose(libHandle_);
    }

    libHandle_ = dlopen("libanmiation.z.so", RTLD_LAZY);
    if (libHandle_ == nullptr) {
        HPAE_LOGW("[%{public}s_%{public}d]:load library failed, reason: %{public}s", __func__, __LINE__, dlerror());
        openFailNum_++;
        return;
    }
    GetHianimationDeviceFunc getDevFunc = reinterpret_cast<GetHianimationDeviceFunc>{
        dlsym(libHandle_, "GetHianimationDevice");
    if (getDevFunc == nullptr) {
        HPAE_LOGW("[%{public}s_%{public}d]:load func failed, reason: %{public}s", __func__, __LINE__, dlerror());
        dlclose(libHandle_);
        openFailNum_++;
        return;
    }
    HPAE_LOGI("[%{public}s_%{public}d]:load success", __func__, __LINE__);
    hianimationDevice_ = getDevFunc();

    {
        HPAE_TRACE_NAME_FMT("Hianimation: load: %p", hianimationDevice_);
    }

    openFailNum_ = 0;
}

void HianimationManager::CloseDevice()
{
    std::unique_lock<std::mutex> lock(mutex_);
    HPAE_TRACE_NAME("Hianimation: CloseDevice");
    if (libHandle_ != nullptr) {
        dlclose(libHnadle_);
    }
    hianimationDevice_ = nullptr;
}

bool HianimationManager::HianimationInputCheck(const struct BlurImgParam *imgInfo, const struct HaeNoiseValue *noisePara)
{
    WaitAlgoInit();

    std::unique_lock<std::mutex> lock(mutex);
    if (!hianimationDevice_ == nullptr) {
        HPAE_LOGE("device is nullptr");
        return false;
    }

    HPAE_TRACE_NAME_FMT("Hianimation: HianimationInputCheck: %d, %d, %f", imgInfo->width, imgInfo->height, imgInfo->sigmaNum);
    HPAE_LOGI("HianimationInputCheck: %{public}d, %{public}d, %{public}f", imgInfo->width, imgInfo->height, imgInfo->sigmaNum);
    return hianimationDevice_->hianimationInputCheck(imgInfo, noisePara);
}

int32_t HianimationManager::HianimationAlgoInit(uint32_t panelWidth, uint32_t panelHeight, float maxSigma, uint32_t format)
{
    WaitAllTaskDone(); // do outside mutex_

    std::unique_lock<std::mutex> lock(mutex_);
    if (hianimationDevice_ == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    HPAE_TRACE_NAME("Hianimation: HianimationAlgoInit");
    HPAE_LOGI("HianimationAlgoInit");

    for (auto taskId : taskIdMap_) {
        HPAE_LOGW("force destroy task: %d", taskId);
        hianimationDevice_->hianimationDestroyTask(taskId);
    }
    taskIdMap_.clear();

    auto result = hianimationDevice_->hianimationAlgoInit(panelWidth, panelHeight, maxSigma, format);

    return result;
}

// TODO: AlgoDeInit may be called before CommitTask
int32_t HianimationManager::HianimationAlgoDeInit()
{
    WaitAlgoInit();
    WaitAllTaskDone(); // do outside mutex_

    std::unique_lock<std::mutex> lock(mutex_);
    if (hianimationDevice_ == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    HPAE_TRACE_NAME("Hianimation: HianimationAlgoDeInit");
    HPAE_LOGI("HianimationAlgoDeInit");

    for (auto taskId : taskIdMap_) {
        HPAE_LOGW("force destroy task: %d", taskId);
        hianimationDevice_->HianimationDestroyTask(taskId);
    }
    taskIdMap_.clear();

    int32_t result = hianimationDevice_->hianimationAlgoDeInit();

    return reslut;
}

bool HianimationManger::WaitAllTaskDone()
{
    std::unique_lock<std::mutex> lock(mutex_);
    bool result = true;
    if (!taskIdMap_.empty()) {
        HPAE_TRACE_NAME("WaitAllTaskDone");
        HPAE_LOGW("WaitAllTaskDone");
        result = taskAvailableCv_.wait_for(lock, 100ms, [this]() {
            return this->taskIdMap_.empty();
        });
    }

    if (!result) {
        HPAE_LOGE("hpae task lost");
    }

    return result;
}

bool HianimationManager::WaitPreviousTask()
{
    std::unique_lock<std::mutex> lock(mutex_);
    bool reselt = true;
    if (taskIdMap_.size() >= HPAE_BLUR_DELAY) {
        HPAE_TRACE_NAME("WaitPreviousTask");
        result = taskAvailableCv_.wait_for(lock, 10ms, [this](){
            return this->taskIdMap_.size() < HPAE_BLUR_DELAY;
        });
    }

    return reselt;
}

int32_t HianimationManager::HianimationBuildTask(const struct HaeBlurBasicAtter *basicInfo,
    const struct HaeBlurEffectAtter *effectInfo, uint32_t *outTaskId, void **outTaskPtr)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (hianimationDevice_ == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    HPAE_TRACE_NAME("Hianimation: HianimationBuildTask");
    auto ret = hianimationDevice_->HianimationBuildTask(basicInfo, effectInfo, outTaskId, outTaskPtr);
    if (ret == 0 && outTaskId) {
        taskIdMap_.insert(*outTaskId);
    }
    return ret;
}

int32_t HianimationManager::HianimationDestroyTask(uint32_t taskId)
{
    std::unique_lock<std::mutex> lock(mutex_);

    if (hianimationDevice_ == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    HPAE_TRACE_NAME("Hianimation: HianimationDestroyTask: %d", taskId);
    taskIdMap_.erase(taskId);
    return hianimationDevice_->HianimationDestroyTask(taskId);
}

int32_t HianimationManager::HianimationDestroyTaskAndNotify(uint32_t taskId)
{
    std::unique_lock<std::mutex> lock(mutex_);

    if (hianimationDevice_ == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    HPAE_TRACE_NAME("Hianimation: HianimationDestroyTaskAndNotify: %d", taskId);
    int32_t ret = hianimationDevice_->HianimationDestroyTask(taskId);
    taskIdMap_.erase(taskId);
    taskAvailableCv_.notify_all();

    NotifyHpaeDone();

    return ret;
}

void HianimationManager::HianimationPerfTrack()
{
    {
        std::unique_lock<std::mutex> lock(hpaePerfMutex_);
        hpaePerfDone_ = false;
    }

    RSHpaePerfThread::Instance().PostTask([this]() {
        HPAE_TRACE_NAME("WaitHpae");
        this->WaitHpaeDone();
    });
}

void* HianimationManager::HianimationCommitTask(void* taskFunc, uint32_t taskId, std::vector<void*> hpaeDependence)
{
    if (taskFunc == nullptr) {
        HPAE_LOGE("Hae ffrt task is null, please check!!!");
        return nullptr;
    }

    {
        std::unique_lock<std::mutex> lock(hpaePerfMutex_);
        hpaePerfDone_ = false;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    
    std::vector<ffrt::dependence> taskDependence = {};
    for (auto handle : hpaeDependence) {
        if (handle != nullptr) {
            taskDependence.push_back(handle);
        }
    }

    // config task
    ffrt::hcs_task perform_task(ffrt::device::aae);
    perform_task.set_run(reinterpret_cast<ffrt_function_t>(taskFunc)); //异构任务
    void* hpaeOutTaskHandle = ffrt::submit_h(perform_task, taskDependence, {}, ffrt::task_attr().qos(5)); // 5 for Hianimation

    HPAE_TRACE_NAME_FMT("HianimationCommitTask: taskFunc: %p, commitTask: %p", taskFunc, hpaeOutTaskHandle);

    RSHpaePerfThread::Instance().PostTask([this]() {
        HPAE_TRACE_NAME("WaitHpae");
        this->WaitHpaeDone();
    });

    return hpaeOutTaskHandle;

}
}
}
#include "rs_buffer_thread.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSBufferThread"

namespace OHOS {
namespace Rosen {

void RSBufferThread::PostSyncTask(const std::function<void()>& task)
{
    if (!handler_) {
        return;
    }
    handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSBufferThread::PostTask(const std::function<void()>& task)
{
    if (!handler_) {
        return;
    }
    handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSBufferThread::PostDelayTask(const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSBufferThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RSBufferThread");
    if (!runner_) {
        RS_LOGE("Start runner null");
        return;
    }
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();

    PostSyncTask([this] {
        RS_LOGE("Started ...");
    });
}
} // OHOS
} // Rosen
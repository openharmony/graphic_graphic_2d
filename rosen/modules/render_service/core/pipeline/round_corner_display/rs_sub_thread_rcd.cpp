#include "rs_sub_thread_rcd.h"
#include <string>
#include "memory/rs_tag_tracker.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
    RSSubThreadRCD::~RSSubThreadRCD()
    {
        RS_LOGI("~RSSubThreadRCD");
    }

    void RSSubThreadRCD::Start(RenderContext *context)
    {
        if (renderContext_ != nullptr) {
            RS_LOGI("RSSubThreadRCD already start!");
            return;
        }
        RS_LOGI("RSSubThreadRCD Started");
        std::string name = "RoundCornerDisplaySubThread";
        runner_ = AppExecFwk::EventRunner::Create(name);
        handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
        renderContext_ = context;
    }

    void RSSubThreadRCD::PostTask(const std::function<void()>& task)
    {
        if (handler_) {
            handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
    }
} // namespace Rosen
} // namespace OHOS
#include <string>

#define ROSEN_TRACE_BEGIN(tag, name) OHOS::Rosen::RosenTraceBegin(name)
#define RS_TRACE_BEGIN(name) OHOS::Rosen::RosenTraceBegin(name)
#define ROSEN_TRACE_END(tag) OHOS::Rosen::RosenTraceEnd()
#define RS_TRACE_END() OHOS::Rosen::RosenTraceEnd()
#define RS_TRACE_NAME(name) OHOS::Rosen::ScopedTrace ___tracer(name)
#define RS_TRACE_NAME_FMT(fmt, ...)
#define RS_ASYNC_TRACE_BEGIN(name, value)
#define RS_ASYNC_TRACE_END(name, value)
#define RS_TRACE_INT(name, value)
#define RS_TRACE_FUNC() RS_TRACE_NAME(__FUNCTION__)

namespace OHOS {
namespace Rosen {

void RosenTraceBegin(const char* name);
void RosenTraceBegin(std::string name);
void RosenTraceEnd();
class ScopedTrace {
    public:
      ScopedTrace(const char *name);

      ScopedTrace(std::string name);

      ~ScopedTrace();
};
} // namespace Rosen
} // namespace OHOS
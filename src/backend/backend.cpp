#include "backend.h"

namespace Hulk::Backend {

BackendResult run_backend(const BackendOptions& options) {
    BackendDriver driver;
    return driver.run(options);
}

} // namespace Hulk::Backend

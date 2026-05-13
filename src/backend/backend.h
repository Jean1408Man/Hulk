#ifndef HULK_BACKEND_BACKEND_H
#define HULK_BACKEND_BACKEND_H

#include "backend_driver.h"

namespace Hulk::Backend {

BackendResult run_backend(const BackendOptions& options);

} // namespace Hulk::Backend

#endif

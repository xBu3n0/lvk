#ifndef UTILS_HPP
#define UTILS_HPP

/** It's not the ideal form, but it works for now */
#ifdef NDEBUG
const bool enable_validation_layer = false;
#else
const bool enable_validation_layer = true;
#endif

#include "device/device.hpp"
#include "extension/extension.hpp"
#include "file/file.hpp"
#include "layer/layer.hpp"
#include "messenger/messenger.hpp"
#include "queue/queue.hpp"
#include "swapchain/swapchain.hpp"

#endif

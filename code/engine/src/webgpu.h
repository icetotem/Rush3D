#pragma once

#include <webgpu/webgpu.h>
#include "Platform.h"

namespace webgpu
{

    WGPUDevice create(rush::WindowHandle window, WGPUBackendType type = WGPUBackendType_Force32);

    WGPUSwapChain createSwapChain(WGPUDevice device);

    WGPUTextureFormat getSwapChainFormat(WGPUDevice device);

}

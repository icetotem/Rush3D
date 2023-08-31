#include "stdafx.h"
#include "RContex.h"

namespace rush
{

    dawn::native::Adapter RContex::adapter;
    wgpu::Device RContex::device;
    wgpu::Queue RContex::queue;

}
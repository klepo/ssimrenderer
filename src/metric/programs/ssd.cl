#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

inline void atom_add_float(volatile __global float *source, const float operand) {
    union {
        unsigned int intValue;
        float floatValue;
    } newValue;
    union {
        unsigned int intValue;
        float floatValue;
    } prevValue;
    do {
        prevValue.floatValue = *source;
        newValue.floatValue = prevValue.floatValue + operand;
    } while (atom_cmpxchg((volatile __global unsigned int *)source, prevValue.intValue, newValue.intValue) != prevValue.intValue);
}

__kernel void ssd(read_only image2d_t image0, read_only image2d_t image1, __global float *result, int array_size, __local float *tmp_buffer)
{
    int global_x = (int) get_global_id(0);
    int local_x = (int) get_local_id(0);
    int local_w = (int) get_local_size(0);

    if (global_x < array_size) {
        float sum = 0;
        int width = get_image_width(image0);
        for (int i = 0; i < width; i++) {
            int2 coords = {i, global_x};
            float value0 = read_imagef(image0, sampler, coords).x;
            float value1 = read_imagef(image1, sampler, coords).x;
            float diff = value0 - value1;
            sum += diff * diff;
        }
        tmp_buffer[local_x] = sum;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    for (int i = local_w >> 1; i > 0; i >>= 1) {
        if ((local_x < i) && (global_x + i < array_size)) {
            tmp_buffer[local_x] += tmp_buffer[local_x + i];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (local_x == 0) {
        //atom_add(result, tmp_a[0]);
        atom_add_float(result, tmp_buffer[0]);
    }
}

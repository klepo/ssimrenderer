constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

kernel void ssd(read_only image2d_t image0, read_only image2d_t image1, global float *resultArray, int width)
{
    int y = get_global_id(0);

    float sum = 0;

    for (int i = 0; i < width; i++) {
        int2 coords = {i, y};
        float value0 = read_imagef(image0, sampler, coords).x;
        float value1 = read_imagef(image1, sampler, coords).x;
        float diff = value0 - value1;
        sum += diff * diff;
    }

    resultArray[y] = sum;
}

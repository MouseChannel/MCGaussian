## MC Gaussian

use Vulkan Compute-Shader to rewrite [3D Gaussian splatting](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/) 

### roadmap

- :white_check_mark: Minimal example to verify feasibility(2024-4-10🎊works!)
- :ballot_box_with_check: **SOTA** GPU sort,by [Nvidia 2022 research](https://research.nvidia.com/publication/2022-06_onesweep-faster-least-significant-digit-radix-sort-gpus)  
        -  (2024-4-21)multi-pass sort make it **4x** faster than single pass,but still slower than sota
- :white_large_square: Camera control(in develop current now)
- :white_large_square: OpenXR support

### showcase

|                         | origin CUDA |  Compute Shader | 
|---------------------|-----------------|-----------------|
| quality   |  ![origincuda](https://github.com/MouseChannel/MCGS/blob/main/showcase/origincuda.png)  | ![output](https://github.com/MouseChannel/MCGS/blob/main/showcase/output.png)  | 
|PSNR |------------------------   |   $+\infty$(same in every single pixel😏)  | 
| FPS      | 227 fps / 4.4ms      | 65 fps / 15.2ms  ( :arrow_heading_down:245%)    | 

<!-- > GPU sort cost 55ms/66.2ms(83%),so It must be optimized!! -->
> GPU sort cost 11.5ms/15.2ms(75.6%),so It must be optimized!!

 


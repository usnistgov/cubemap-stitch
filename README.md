# cubemap-stitch

This repository provides a utility to stitch cubemaps into other types of texture maps (equirectangular, dual paraboloid, and octahedral maps), and stitch the other types of maps back into cube maps. The utility allows for flexibility in the image size of the conversion - the user can specify the desired image width, and the height is commputed (cube, paraboloid, and octahedral mappings are square, and spherical maps are generated to be 16:9). Moreover, the utility is sampling-agnostic, so the user can select whether to use uniform or jittered sampling over the pixels, as well as the number of samples to use per pixel. The usage for the tool is as follows:

./hev-mapConverter [arg1] [arg2] ... [arg12] [arg13]

  [arg1] - This argument specifies the type of transformation to do. Accepts either the string 'convToCube' or the string 'convFromCube'.

  [arg2] - This argument specifies which type of texture map to be converted to/from. Accepts the strings 'equirectangular', 'paraboloid', and 'octahedral'.

  [arg3] - This argument specifies the number of samples to be used. We recommand either '36' or '49' to prevent aliasing without using an excessive number of samples.

  [arg4] - This argument specifies the type of sampling to use. Accepts either 'uniform' or 'jittered'.

  [arg5] ... arg[10] - These arguments are the file names of the cube map images (to either be created or used depending on arg1). The input order is [left] [right] [back] [front] [floor] [ceiling].

  [arg11] - This argument specifies the output image width (i.e. '1080' for 1080p images and '3840' for 4k images).

  [arg12] - This argument specifies the projection file name (to either be created or used depending on arg1).

  [arg13] - This argument specifies a second projection file name (dual paraboloid and octahedral maps require two files for the projection). Note that this argument should be omitted for conversions that involve equirectangular maps.

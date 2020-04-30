# tninypt
C-based Path tracing renderer

There are 3 versions for the main file (rt.c, rtmp.c, rtmp4.c), with only the first one being cross-platform, the other two requiring a POSIX-compliant OS like Unix or Linux, because they are using concurrent processes to accelerate the execution.

For the rt.c version , once compiled, you have to redirect the standart output to a .ppm file using something like `./exec --> output.ppm`.
The other version are writing directly to the `out.ppm` file.

To change the scene, edit pt.c.

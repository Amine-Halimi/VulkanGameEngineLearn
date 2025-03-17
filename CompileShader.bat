set "glslcLocation=C:\VulkanSDK\1.3.296.0\Bin\glslc.exe"

echo %glslcLocation%
%glslcLocation% shaders\simpleVertexShader.vert -o shaders\simpleVertexShader.vert.spv
%glslcLocation% shaders\simpleFragmentShader.frag -o shaders\simpleFragmentShader.frag.spv
pause
@echo off

set "VSCMD_START_DIR=%CD%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

set tools_dir=tools
set bh=%tools_dir%\bin2hex.exe
set bh_cpp=%tools_dir%\bin2hex.c
if not exist %bh% (
    cl.exe /EHsc /nologo /Fe%tools_dir%\ /Fo%tools_dir%\ %bh_cpp%
)

set PATH=%tools_dir%;%PATH%

set glsl=glsl\
set cl=%VULKAN_SDK%\Bin\glslangValidator.exe
set tmpf=spirv\data.spv
set outf=+spirv\shader_data.c

echo %bh%

mkdir spirv

del /Q spirv\shader_data.c
del /Q "%tmpf%"

@rem compile individual shaders

for %%f in (%glsl%*.vert) do (
    "%cl%" -S vert -V -o "%tmpf%" "%%f"
    "%bh%" "%tmpf%" %outf% %%~nf_vert_spv
    del /Q "%tmpf%"
)

for %%f in (%glsl%*.frag) do (
    "%cl%" -S frag -V -o "%tmpf%" "%%f"
    "%bh%" "%tmpf%" %outf% %%~nf_frag_spv
    del /Q "%tmpf%"
)


@rem single-texture vertex, identity (1.0) colors 

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_CLX_IDENT
"%bh%" "%tmpf%" %outf% vert_tx0_ident1

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_CLX_IDENT -DUSE_FOG
"%bh%" "%tmpf%" %outf% vert_tx0_ident1_fog

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_CLX_IDENT -DUSE_ENV
"%bh%" "%tmpf%" %outf% vert_tx0_ident1_env

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_CLX_IDENT -DUSE_FOG -DUSE_ENV
"%bh%" "%tmpf%" %outf% vert_tx0_ident1_env_fog

@rem single-texture vertex with fixed (rgb+a) colors

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_FIXED_COLOR
"%bh%" "%tmpf%" %outf% vert_tx0_fixed

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_FIXED_COLOR -DUSE_FOG
"%bh%" "%tmpf%" %outf% vert_tx0_fixed_fog

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_FIXED_COLOR -DUSE_ENV
"%bh%" "%tmpf%" %outf% vert_tx0_fixed_env

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_FIXED_COLOR -DUSE_FOG -DUSE_ENV
"%bh%" "%tmpf%" %outf% vert_tx0_fixed_env_fog


@rem double-texture vertex, identity (1.0) colors 

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_CLX_IDENT -DUSE_TX1
"%bh%" "%tmpf%" %outf% vert_tx1_ident1

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_CLX_IDENT -DUSE_TX1 -DUSE_FOG
"%bh%" "%tmpf%" %outf% vert_tx1_ident1_fog

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_CLX_IDENT -DUSE_TX1 -DUSE_ENV
"%bh%" "%tmpf%" %outf% vert_tx1_ident1_env

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_CLX_IDENT -DUSE_TX1 -DUSE_FOG -DUSE_ENV
"%bh%" "%tmpf%" %outf% vert_tx1_ident1_env_fog

@rem double-texture vertex, fixed (rgb+a) colors 

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_FIXED_COLOR -DUSE_TX1
"%bh%" "%tmpf%" %outf% vert_tx1_fixed

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_FIXED_COLOR -DUSE_TX1 -DUSE_FOG
"%bh%" "%tmpf%" %outf% vert_tx1_fixed_fog

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_FIXED_COLOR -DUSE_TX1 -DUSE_ENV
"%bh%" "%tmpf%" %outf% vert_tx1_fixed_env

"%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl -DUSE_FIXED_COLOR -DUSE_TX1 -DUSE_FOG -DUSE_ENV
"%bh%" "%tmpf%" %outf% vert_tx1_fixed_env_fog

@rem single-texture fragment, identity (1.0) color

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_CLX_IDENT -DUSE_ATEST
"%bh%" "%tmpf%" %outf% frag_tx0_ident1

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_CLX_IDENT -DUSE_ATEST -DUSE_FOG
"%bh%" "%tmpf%" %outf% frag_tx0_ident1_fog

@rem single-texture fragment, fixed (rgb+a) color

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_FIXED_COLOR -DUSE_ATEST
"%bh%" "%tmpf%" %outf% frag_tx0_fixed

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_FIXED_COLOR -DUSE_ATEST -DUSE_FOG
"%bh%" "%tmpf%" %outf% frag_tx0_fixed_fog

@rem double-texture fragment, identity colors (1.0)

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_CLX_IDENT -DUSE_TX1
"%bh%" "%tmpf%" %outf% frag_tx1_ident1

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_CLX_IDENT -DUSE_TX1 -DUSE_FOG
"%bh%" "%tmpf%" %outf% frag_tx1_ident1_fog

@rem double-texture fragment, fixed (rgb+a) colors

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_FIXED_COLOR -DUSE_TX1
"%bh%" "%tmpf%" %outf% frag_tx1_fixed

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_FIXED_COLOR -DUSE_TX1 -DUSE_FOG
"%bh%" "%tmpf%" %outf% frag_tx1_fixed_fog

@rem single-texture fragment, depth-fragment

"%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl -DUSE_CLX_IDENT -DUSE_ATEST -DUSE_DF
"%bh%" "%tmpf%" %outf% frag_tx0_df


@rem compile lighting shader variations from templates

"%cl%" -S vert -V -o "%tmpf%" %glsl%light_vert.tmpl
"%bh%" "%tmpf%" %outf% vert_light

"%cl%" -S vert -V -o "%tmpf%" %glsl%light_vert.tmpl -DUSE_FOG
"%bh%" "%tmpf%" %outf% vert_light_fog

"%cl%" -S frag -V -o "%tmpf%" %glsl%light_frag.tmpl
"%bh%" "%tmpf%" %outf% frag_light

"%cl%" -S frag -V -o "%tmpf%" %glsl%light_frag.tmpl -DUSE_FOG 
"%bh%" "%tmpf%" %outf% frag_light_fog

"%cl%" -S frag -V -o "%tmpf%" %glsl%light_frag.tmpl -DUSE_LINE
"%bh%" "%tmpf%" %outf% frag_light_line

"%cl%" -S frag -V -o "%tmpf%" %glsl%light_frag.tmpl -DUSE_LINE -DUSE_FOG
"%bh%" "%tmpf%" %outf% frag_light_line_fog

@rem template shader identifiers and flags
set "sh[0]="
set "sh_id[0]=cpu_"

set "tx[0]="
set "tx[1]=-DUSE_TX1"
set "tx[2]=-DUSE_TX2"
set "tx_id[0]=tx0"
set "tx_id[1]=tx1"
set "tx_id[2]=tx2"

set "cl[1]=-DUSE_CL1"
set "cl[2]=-DUSE_CL2"
set "cl_id[1]=cl"
set "cl_id[2]=cl"

set "env[0]="
set "env[1]=-DUSE_ENV"
set "env_id[0]="
set "env_id[1]=_env"

set "fog[0]="
set "fog[1]=-DUSE_FOG"
set "fog_id[0]="
set "fog_id[1]=_fog"

SETLOCAL EnableDelayedExpansion

@rem compile generic shader variations from templates
@rem vertex shader
for /L %%i in ( 0,1,0 ) do (                @rem shading mode, wip features 
    for /L %%j in ( 0,1,2 ) do (            @rem tx   
        for /L %%k in ( 0,1,1 ) do (        @rem +env
            for /L %%m in ( 0,1,1 ) do (    @rem +fog
                call :compile_vertex_shader %%i, %%j, %%k, %%m
            )
        )
    )
)

@rem fragment shader
for /L %%i in ( 0,1,0 ) do (                @rem shading mode, wip features
    for /L %%j in ( 0,1,2 ) do (            @rem tx 
        for /L %%k in ( 0,1,1 ) do (        @rem +fog
            call :compile_fragment_shader %%i, %%j, %%k
        )
    )
)

del /Q "%tmpf%"

pause

:compile_fragment_shader
    set "flags=!sh[%1]! !tx[%2]! !fog[%3]!"
    set "name=!sh_id[%1]!!tx_id[%2]!!fog_id[%3]!"
    if %2 equ 0 ( set "flags=%flags% -DUSE_ATEST" )

    "%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl %flags%
    "%bh%" "%tmpf%" %outf% frag_%name%

    @rem +cl
    if %2 equ 0 goto continue
        "%cl%" -S frag -V -o "%tmpf%" %glsl%gen_frag.tmpl !sh[%1]! !tx[%2]! !cl[%2]! !fog[%3]!
        "%bh%" "%tmpf%" %outf% frag_!sh_id[%1]!!tx_id[%2]!_!cl_id[%2]!!fog_id[%3]!
    :continue
exit /B

:compile_vertex_shader
    "%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl !sh[%1]! !tx[%2]! !env[%3]! !fog[%4]!
    "%bh%" "%tmpf%" %outf% vert_!sh_id[%1]!!tx_id[%2]!!env_id[%3]!!fog_id[%4]!

    @rem +cl
    if %2 equ 0 goto continue
        "%cl%" -S vert -V -o "%tmpf%" %glsl%gen_vert.tmpl !sh[%1]! !tx[%2]! !cl[%2]! !env[%3]! !fog[%4]!
        "%bh%" "%tmpf%" %outf% vert_!sh_id[%1]!!tx_id[%2]!_!cl_id[%2]!!env_id[%3]!!fog_id[%4]!
    :continue
exit /B
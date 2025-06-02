function(add_shaders)
    add_custom_target(GEN_SHADERS ALL
            COMMAND python ${CMAKE_SOURCE_DIR}/Scripts/Python/GenerateShaders.py
            -dxc_path "${CMAKE_SOURCE_DIR}/Engine/External/DXIL/bin/dxc.exe"
            -source_folder "${CMAKE_SOURCE_DIR}/Engine/Shaders/"
            -output_folder "${CMAKE_BINARY_DIR}/Editor/Shaders/"
            COMMENT "Generating Shaders"
    )
    add_dependencies(Engine GEN_SHADERS)
endfunction()
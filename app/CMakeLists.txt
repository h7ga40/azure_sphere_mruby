#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

project(app C)

include_directories(../mruby-2.1.1/include)
add_definitions(-DMRB_METHOD_T_STRUCT -DMRB_USE_CUSTOM_RO_DATA_P -DMRB_CONSTRAINED_BASELINE_PROFILE)
link_directories(../mruby-2.1.1/build/arm-poky-linux-musleabi/lib)

azsphere_configure_tools(TOOLS_REVISION "20.04")
azsphere_configure_api(TARGET_API_SET "5")

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/client_rb.c
  MAIN_DEPENDENCY client.rb
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND mrbc -e -Bclient_code -o client_rb.c client.rb
)

set(
    CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=app.map"
)

add_executable(${PROJECT_NAME} main.c stub.c client_rb.c)
target_link_libraries(${PROJECT_NAME} applibs pthread gcc_s c azureiot curl mruby)

set_source_files_properties(
    PROPERTIES OBJECT_OUTPUTS "app.map"
)

azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../hardware/seeed_mt3620_mdb" TARGET_DEFINITION "sample_hardware.json")

azsphere_target_add_image_package(${PROJECT_NAME})

cmake_minimum_required(VERSION 3.10)

project(mruby C)

add_custom_target(libmruby ALL
  SOURCES ${PROJECT_SOURCE_DIR}/build/arm-poky-linux-musleabi/lib/libmruby.a
)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/build/arm-poky-linux-musleabi/lib/libmruby.a
  MAIN_DEPENDENCY build_config.rb
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND rake
)

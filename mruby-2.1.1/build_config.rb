MRuby::Build.new do |conf|
  # load specific toolchain settings

  # Gets set by the VS command prompts.
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    toolchain :visualcpp
  else
    toolchain :gcc
  end

  # Turn on `enable_debug` for better debugging
  # enable_debug

  # Use mrbgems
  # conf.gem 'examples/mrbgems/ruby_extension_example'
  # conf.gem 'examples/mrbgems/c_extension_example' do |g|
  #   g.cc.flags << '-g' # append cflags in this gem
  # end
  # conf.gem 'examples/mrbgems/c_and_ruby_extension_example'
  # conf.gem :core => 'mruby-eval'
  # conf.gem :mgem => 'mruby-onig-regexp'
  # conf.gem :github => 'mattn/mruby-onig-regexp'
  # conf.gem :git => 'git@github.com:mattn/mruby-onig-regexp.git', :branch => 'master', :options => '-v'

  # include the default GEMs
  conf.gembox 'default'
  # C compiler settings
  # conf.cc do |cc|
  #   cc.command = ENV['CC'] || 'gcc'
  #   cc.flags = [ENV['CFLAGS'] || %w()]
  #   cc.include_paths = ["#{root}/include"]
  #   cc.defines = %w()
  #   cc.option_include_path = %q[-I"%s"]
  #   cc.option_define = '-D%s'
  #   cc.compile_options = %Q[%{flags} -MMD -o "%{outfile}" -c "%{infile}"]
  # end

  # mrbc settings
  # conf.mrbc do |mrbc|
  #   mrbc.compile_options = "-g -B%{funcname} -o-" # The -g option is required for line numbers
  # end

  # Linker settings
  # conf.linker do |linker|
  #   linker.command = ENV['LD'] || 'gcc'
  #   linker.flags = [ENV['LDFLAGS'] || []]
  #   linker.flags_before_libraries = []
  #   linker.libraries = %w()
  #   linker.flags_after_libraries = []
  #   linker.library_paths = []
  #   linker.option_library = '-l%s'
  #   linker.option_library_path = '-L%s'
  #   linker.link_options = "%{flags} -o "%{outfile}" %{objs} %{libs}"
  # end

  # Archiver settings
  # conf.archiver do |archiver|
  #   archiver.command = ENV['AR'] || 'ar'
  #   archiver.archive_options = 'rs "%{outfile}" %{objs}'
  # end

  # Parser generator settings
  # conf.yacc do |yacc|
  #   yacc.command = ENV['YACC'] || 'bison'
  #   yacc.compile_options = %q[-o "%{outfile}" "%{infile}"]
  # end

  # gperf settings
  # conf.gperf do |gperf|
  #   gperf.command = 'gperf'
  #   gperf.compile_options = %q[-L ANSI-C -C -p -j1 -i 1 -g -o -t -N mrb_reserved_word -k"1,3,$" "%{infile}" > "%{outfile}"]
  # end

  # file extensions
  # conf.exts do |exts|
  #   exts.object = '.o'
  #   exts.executable = '' # '.exe' if Windows
  #   exts.library = '.a'
  # end

  # file separetor
  # conf.file_separator = '/'

  # bintest
  # conf.enable_bintest
end
=begin
MRuby::Build.new('host-debug') do |conf|
  # load specific toolchain settings

  # Gets set by the VS command prompts.
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    toolchain :visualcpp
  else
    toolchain :gcc
  end

  enable_debug

  # include the default GEMs
  conf.gembox 'default'

  # C compiler settings
  conf.cc.defines = %w(MRB_ENABLE_DEBUG_HOOK)

  # Generate mruby debugger command (require mruby-eval)
  conf.gem :core => "mruby-bin-debugger"

  # bintest
  # conf.enable_bintest
end

MRuby::Build.new('test') do |conf|
  # Gets set by the VS command prompts.
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    toolchain :visualcpp
  else
    toolchain :gcc
  end

  enable_debug
  conf.enable_bintest
  conf.enable_test

  conf.gembox 'default'
end
=end
#MRuby::Build.new('bench') do |conf|
#  # Gets set by the VS command prompts.
#  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
#    toolchain :visualcpp
#  else
#    toolchain :gcc
#    conf.cc.flags << '-O3'
#  end
#
#  conf.gembox 'default'
#end

# Define cross build settings
# MRuby::CrossBuild.new('32bit') do |conf|
#   toolchain :gcc
#
#   conf.cc.flags << "-m32"
#   conf.linker.flags << "-m32"
#
#   conf.build_mrbtest_lib_only
#
#   conf.gem 'examples/mrbgems/c_and_ruby_extension_example'
#
#   conf.test_runner.command = 'env'
# end

# Cross build for arm-poky-linux-musleabi
MRuby::CrossBuild.new('arm-poky-linux-musleabi') do |conf|
  toolchain :gcc

  conf.cc.command = "arm-poky-linux-musleabi-gcc"
  conf.cxx.command = "arm-poky-linux-musleabi-g++"
  conf.linker.command = "arm-poky-linux-musleabi-gcc"
  conf.archiver.command = "arm-poky-linux-musleabi-ar"

  conf.cc.flags << %w(-B "C:/Program Files (x86)/Microsoft Azure Sphere SDK/Sysroots/5/tools/gcc" -march=armv7ve -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a7 --sysroot="C:/Program Files (x86)/Microsoft Azure Sphere SDK/Sysroots/5" -g -std=c11 -Wstrict-prototypes -Wno-pointer-sign -ggdb -Og -fPIC -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-omit-frame-pointer -fno-exceptions -Wall -Wswitch -Wempty-body -Wconversion -Wreturn-type -Wparentheses -Wno-format -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -Werror=implicit-function-declaration -fstack-protector-strong)
  conf.cc.flags << %w(-isystem "#{root}/../hardware/seeed_mt3620_mdb/inc" -isystem "C:/Program Files (x86)/Microsoft Azure Sphere SDK/Sysroots/5/usr/include")

  conf.cc.defines << %w(_POSIX_C_SOURCE)
  conf.cc.defines << %w(MRB_METHOD_T_STRUCT MRB_USE_CUSTOM_RO_DATA_P MRB_CONSTRAINED_BASELINE_PROFILE)
  conf.cc.defines << %w(AZURE_SPHERE)

  #conf.build_mrbtest_lib_only

  conf.gem "#{root}/mrbgems/mruby-compiler"
  conf.gem "#{root}/mrbgems/mruby-error"
  #conf.gem "#{root}/mrbgems/mruby-fiber"
  conf.gem "#{root}/mrbgems/mruby-io"
  conf.gem "#{root}/mrbgems/mruby-math"
  conf.gem "#{root}/mrbgems/mruby-pack"
  conf.gem "#{root}/mrbgems/mruby-print"
  conf.gem "#{root}/mrbgems/mruby-random"
  #conf.gem "#{root}/mrbgems/mruby-socket"
  conf.gem "#{root}/mrbgems/mruby-sprintf"
  conf.gem "#{root}/mrbgems/mruby-struct"
  conf.gem "#{root}/mrbgems/mruby-time"
  conf.gem "#{root}/mrbgems/mruby-array-ext"
  conf.gem "#{root}/mrbgems/mruby-enum-ext"
  conf.gem "#{root}/mrbgems/mruby-hash-ext"
  conf.gem "#{root}/mrbgems/mruby-numeric-ext"
  conf.gem "#{root}/mrbgems/mruby-object-ext"
  conf.gem "#{root}/mrbgems/mruby-string-ext"

  conf.gem "#{root}/../mrbgems/mruby-iijson"
  conf.gem "#{root}/../mrbgems/mruby-azure-iot"
  conf.gem "#{root}/../mrbgems/mruby-others"

  conf.bins = []
end

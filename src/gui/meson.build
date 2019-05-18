###################
# GUI library #
###################

libredshiftkgtk_gui_incs = [
  include_directories('../')
]

cc = meson.get_compiler('c')
libm_dep = cc.find_library('m', required : false)

libredshiftgtk_gui_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk+-3.0', version: '>= 3.22'),
  libm_dep
]

libredshiftgtk_gui_sources = files(
  'redshiftgtk-radial-slider.c',
  'redshiftgtk-window.c'
)

libredshiftgtk_gui = static_library(
               'redshiftgtk-gui',
              sources: libredshiftgtk_gui_sources,
  include_directories: libredshiftkgtk_gui_incs,
         dependencies: libredshiftgtk_gui_deps,
              install: false,
)

libredshiftgtk_gui_dep = declare_dependency(
            link_with: libredshiftgtk_gui,
         dependencies: libredshiftgtk_gui_deps,
  include_directories: libredshiftkgtk_gui_incs,
)


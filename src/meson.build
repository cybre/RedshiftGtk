
#######################
# Include directories #
#######################

redshiftkgtk_incs = [
  include_directories('backend'),
  include_directories('gui')
]


###################
# Private library #
###################

cc = meson.get_compiler('c')
libm_dep = cc.find_library('m', required : false)

libredshiftgtk_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk+-3.0', version: '>= 3.22'),
  libm_dep
]

libredshiftgtk_sources = files(
  'backend/redshiftgtk-backend.c',
  'backend/redshiftgtk-redshift-wrapper.c',
  'gui/redshiftgtk-radial-slider.c',
  'gui/redshiftgtk-window.c'
)

libredshiftgtk = static_library(
               'redshiftgtk-private',
              sources: libredshiftgtk_sources,
  include_directories: redshiftkgtk_incs,
         dependencies: libredshiftgtk_deps,
              install: false,
)

libredshiftgtk_dep = declare_dependency(
            link_with: libredshiftgtk,
         dependencies: libredshiftgtk_deps,
  include_directories: redshiftkgtk_incs,
)

##############
# Executable #
##############

redshiftgtk_sources = [
  'main.c'
]

gnome = import('gnome')

redshiftgtk_sources += gnome.compile_resources('redshiftgtk-resources', gresource,
    source_dir: data_dir,
        c_name: 'redshiftgtk',
  dependencies: resource_data
)

executable('redshiftgtk',
       sources: redshiftgtk_sources,
  dependencies: libredshiftgtk_dep,
       install: true
)

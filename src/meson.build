subdir('backend')
subdir('gui')
subdir('tests')

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

redshiftgtk_deps = [
  libredshiftgtk_backend_dep,
  libredshiftgtk_gui_dep
]

executable('redshiftgtk',
       sources: redshiftgtk_sources,
  dependencies: redshiftgtk_deps,
       install: true
)

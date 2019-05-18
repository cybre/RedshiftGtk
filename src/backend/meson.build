###################
# Backend library #
###################

libredshiftkgtk_backend_incs = [
  include_directories('../')
]

libredshiftgtk_backend_deps = [
  dependency('gio-2.0', version: '>= 2.50')
]

libredshiftgtk_backend_sources = files(
  'redshiftgtk-backend.c',
  'redshiftgtk-redshift-wrapper.c'
)

libredshiftgtk_backend = static_library(
               'redshiftgtk-backend',
              sources: libredshiftgtk_backend_sources,
  include_directories: libredshiftkgtk_backend_incs,
         dependencies: libredshiftgtk_backend_deps,
              install: false
)

libredshiftgtk_backend_dep = declare_dependency(
            link_with: libredshiftgtk_backend,
         dependencies: libredshiftgtk_backend_deps,
  include_directories: libredshiftkgtk_backend_incs
)

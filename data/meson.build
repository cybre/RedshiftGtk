desktop_file = i18n.merge_file(
  input: 'com.github.cybre.RedshiftGtk.desktop.in',
  output: 'com.github.cybre.RedshiftGtk.desktop',
  type: 'desktop',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'applications')
)

desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: [desktop_file]
  )
endif

appstream_file = i18n.merge_file(
  input: 'com.github.cybre.RedshiftGtk.appdata.xml.in',
  output: 'com.github.cybre.RedshiftGtk.appdata.xml',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'appdata')
)

gresource = files('redshiftgtk.gresource.xml')

resource_data = files(
  'ui/redshiftgtk-window.ui',
  'images/slider-day.png',
  'images/slider-night.png'
)


appstream_util = find_program('appstream-util', required: false)
if appstream_util.found()
  test('Validate appstream file', appstream_util,
    args: ['validate', appstream_file]
  )
endif

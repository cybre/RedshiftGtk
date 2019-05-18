test_env = [
  'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
  'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
  'G_DEBUG=gc-friendly',
  'MALLOC_CHECK_=2',
]

test_cflags = [
  '-DTEST_DATA_DIR="@0@/data/"'.format(meson.current_source_dir()),
  '-I' + join_paths(meson.source_root(), 'src'),
]


test_redshift_wrapper= executable('test-redshift-wrapper', 'test-redshift-wrapper.c',
        c_args: test_cflags,
  dependencies: libredshiftgtk_backend_dep,
)
test('test-redshift-wrapper', test_redshift_wrapper, env: test_env)

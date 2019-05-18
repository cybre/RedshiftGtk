#include "backend/redshiftgtk-backend.h"
#include "backend/redshiftgtk-redshift-wrapper.h"

void
load_test_conf (RedshiftGtkBackend *backend)
{
        g_autoptr (GError) error = NULL;
        g_autofree gchar *data_config_path = NULL;

        data_config_path = g_build_filename (TEST_DATA_DIR, "redshift.conf", NULL);
        redshiftgtk_redshift_wrapper_set_config_path (backend,
                                                      g_strdup (data_config_path),
                                                      &error);

        g_assert_no_error (error);
}

static void
test_redshift_wrapper_basic (void)
{
        gchar *config_path = NULL;
        g_autofree gchar *data_config_path = NULL;
        g_autoptr (GError) error = NULL;

        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        g_assert (backend != NULL);
        g_assert (REDSHIFTGTK_IS_BACKEND (backend));

        data_config_path = g_build_filename (TEST_DATA_DIR, "redshift.conf", NULL);
        redshiftgtk_redshift_wrapper_set_config_path (backend,
                                                      data_config_path,
                                                      &error);

        g_assert_no_error (error);

        config_path = redshiftgtk_redshift_wrapper_get_config_path (backend);
        g_assert_cmpstr (config_path, ==, data_config_path);
}

static void
test_redshift_wrapper_get_temperature_day (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);
        gdouble temperature = redshiftgtk_backend_get_temperature (backend,
                                                                   TIME_PERIOD_DAY);
        g_assert_cmpfloat (temperature, ==, 5500);

}

static void
test_redshift_wrapper_get_temperature_night (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);
        gdouble temperature = redshiftgtk_backend_get_temperature (backend,
                                                                   TIME_PERIOD_NIGHT);
        g_assert_cmpfloat (temperature, ==, 4500);

}

static void
test_redshift_wrapper_get_location_provider (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        LocationProvider provider = redshiftgtk_backend_get_location_provider (backend);
        g_assert (provider == LOCATION_PROVIDER_MANUAL);
}

static void
test_redshift_wrapper_get_latitude (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        gdouble latitude = redshiftgtk_backend_get_latitude (backend);

        g_assert_cmpfloat (latitude, ==, 45.38);
}

static void
test_redshift_wrapper_get_longtitude (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        gdouble longtitude = redshiftgtk_backend_get_longtitude (backend);

        g_assert_cmpfloat (longtitude, ==, 20.38);
}

static void
test_redshift_wrapper_get_brightness_day (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        gdouble brightness = redshiftgtk_backend_get_brightness (backend,
                                                                 TIME_PERIOD_DAY);

        g_assert_cmpfloat (brightness, ==, 0.8);
}

static void
test_redshift_wrapper_get_brightness_night (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        gdouble brightness = redshiftgtk_backend_get_brightness (backend,
                                                                 TIME_PERIOD_NIGHT);

        g_assert_cmpfloat (brightness, ==, 0.9);
}

static void
test_redshift_wrapper_get_gamma_day (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        GArray *gamma = redshiftgtk_backend_get_gamma (backend,
                                                       TIME_PERIOD_DAY);
        g_assert (gamma != NULL);

        gdouble red, green, blue;
        red = g_array_index (gamma, gdouble, 0);
        g_assert_cmpfloat (red, ==, 0.1);
        green = g_array_index (gamma, gdouble, 1);
        g_assert_cmpfloat (green, ==, 0.2);
        blue = g_array_index (gamma, gdouble, 2);
        g_assert_cmpfloat (blue, ==, 0.3);
}

static void
test_redshift_wrapper_get_gamma_night (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        GArray *gamma = redshiftgtk_backend_get_gamma (backend,
                                                       TIME_PERIOD_NIGHT);
        g_assert (gamma != NULL);

        gdouble red, green, blue;
        red = g_array_index (gamma, gdouble, 0);
        g_assert_cmpfloat (red, ==, 0.4);
        green = g_array_index (gamma, gdouble, 1);
        g_assert_cmpfloat (green, ==, 0.5);
        blue = g_array_index (gamma, gdouble, 2);
        g_assert_cmpfloat (blue, ==, 0.6);
}

static void
test_redshift_wrapper_get_adjustment_method (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        AdjustmentMethod method = redshiftgtk_backend_get_adjustment_method (backend);

        g_assert (method == ADJUSTMENT_METHOD_RANDR);
}

static void
test_redshift_wrapper_get_smooth_transition (void)
{
        RedshiftGtkBackend *backend = redshiftgtk_redshift_wrapper_new ();
        load_test_conf (backend);

        gboolean transition = redshiftgtk_backend_get_smooth_transition (backend);

        g_assert (transition == TRUE);
}

gint
main (gint   argc,
      gchar *argv[])
{
        g_test_init (&argc, &argv, NULL);
        g_test_add_func ("/Backend/RedshiftWrapper/basic",
                         test_redshift_wrapper_basic);
        g_test_add_func ("/Backend/RedshiftWrapper/get-temperature-day",
                         test_redshift_wrapper_get_temperature_day);
        g_test_add_func ("/Backend/RedshiftWrapper/get-temperature-night",
                         test_redshift_wrapper_get_temperature_night);
        g_test_add_func ("/Backend/RedshiftWrapper/get-location-provider",
                         test_redshift_wrapper_get_location_provider);
        g_test_add_func ("/Backend/RedshiftWrapper/get-latitude",
                         test_redshift_wrapper_get_latitude);
        g_test_add_func ("/Backend/RedshiftWrapper/get-longtitude",
                         test_redshift_wrapper_get_longtitude);
        g_test_add_func ("/Backend/RedshiftWrapper/get-brightness-day",
                         test_redshift_wrapper_get_brightness_day);
        g_test_add_func ("/Backend/RedshiftWrapper/get-brightness-night",
                         test_redshift_wrapper_get_brightness_night);
        g_test_add_func ("/Backend/RedshiftWrapper/get-gamma-day",
                         test_redshift_wrapper_get_gamma_day);
        g_test_add_func ("/Backend/RedshiftWrapper/get-gamma-night",
                         test_redshift_wrapper_get_gamma_night);
        g_test_add_func ("/Backend/RedshiftWrapper/get-adjustment-method",
                         test_redshift_wrapper_get_adjustment_method);
        g_test_add_func ("/Backend/RedshiftWrapper/get-smooth-transition",
                         test_redshift_wrapper_get_smooth_transition);
        return g_test_run ();
}


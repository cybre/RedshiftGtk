#include "backend/redshiftgtk-backend.h"
#include "backend/redshiftgtk-redshift-wrapper.h"

typedef struct {
        RedshiftGtkBackend *backend;
        gchar *data_config_path;
} ObjectFixture;

static void
redshift_wrapper_fixture_set_up (ObjectFixture *fixture,
                                 gconstpointer  user_data)
{
        g_autoptr (GError) error = NULL;

        fixture->backend = redshiftgtk_redshift_wrapper_new ();
        g_assert (fixture->backend != NULL);
        g_assert (REDSHIFTGTK_IS_BACKEND (fixture->backend));

        fixture->data_config_path = g_build_filename (TEST_DATA_DIR, "redshift.conf", NULL);
        redshiftgtk_redshift_wrapper_set_config_path (fixture->backend,
                                                      fixture->data_config_path,
                                                      &error);
        g_assert_no_error (error);
}


static void
redshift_wrapper_fixture_tear_down (ObjectFixture *fixture,
                                    gconstpointer  user_data)
{
        g_clear_object (&fixture->backend);
}

static void
test_redshift_wrapper_get_config_path (ObjectFixture *fixture,
                                       gconstpointer  user_data)
{
        gchar *config_path = redshiftgtk_redshift_wrapper_get_config_path (fixture->backend);
        g_assert_cmpstr (config_path, ==, fixture->data_config_path);
}

static void
test_redshift_wrapper_get_temperature (ObjectFixture *fixture,
                                       gconstpointer  user_data)
{
        TimePeriod period = (TimePeriod)user_data;
        gdouble temperature = redshiftgtk_backend_get_temperature (fixture->backend,
                                                                   period);
        switch (period) {
        case TIME_PERIOD_DAY:
                g_assert_cmpfloat (temperature, ==, 5500);
                break;
        case TIME_PERIOD_NIGHT:
                g_assert_cmpfloat (temperature, ==, 4500);
                break;
        }
}

static void
test_redshift_wrapper_get_location_provider (ObjectFixture *fixture,
                                             gconstpointer  user_data)
{
        LocationProvider provider = redshiftgtk_backend_get_location_provider (fixture->backend);
        g_assert (provider == LOCATION_PROVIDER_MANUAL);
}

static void
test_redshift_wrapper_get_latitude (ObjectFixture *fixture,
                                    gconstpointer  user_data)
{
        gdouble latitude = redshiftgtk_backend_get_latitude (fixture->backend);
        g_assert_cmpfloat (latitude, ==, 45.38);
}

static void
test_redshift_wrapper_get_longtitude (ObjectFixture *fixture,
                                      gconstpointer  user_data)
{
        gdouble longtitude = redshiftgtk_backend_get_longtitude (fixture->backend);
        g_assert_cmpfloat (longtitude, ==, 20.38);
}

static void
test_redshift_wrapper_get_brightness (ObjectFixture *fixture,
                                      gconstpointer  user_data)
{

        TimePeriod period = (TimePeriod)user_data;
        gdouble brightness = redshiftgtk_backend_get_brightness (fixture->backend,
                                                                 period);
        switch (period) {
        case TIME_PERIOD_DAY:
                g_assert_cmpfloat (brightness, ==, 0.8);
                break;
        case TIME_PERIOD_NIGHT:
                g_assert_cmpfloat (brightness, ==, 0.9);
                break;
        }
}

static void
test_redshift_wrapper_get_gamma (ObjectFixture *fixture,
                                 gconstpointer  user_data)
{
        TimePeriod period = (TimePeriod)user_data;
        GArray *gamma = redshiftgtk_backend_get_gamma (fixture->backend,
                                                       period);
        g_assert (gamma != NULL);

        gdouble red, green, blue;
        red = g_array_index (gamma, gdouble, 0);
        green = g_array_index (gamma, gdouble, 1);
        blue = g_array_index (gamma, gdouble, 2);

        switch (period) {
        case TIME_PERIOD_DAY:
                g_assert_cmpfloat (red, ==, 0.1);
                g_assert_cmpfloat (green, ==, 0.2);
                g_assert_cmpfloat (blue, ==, 0.3);
                break;
        case TIME_PERIOD_NIGHT:
                g_assert_cmpfloat (red, ==, 0.4);
                g_assert_cmpfloat (green, ==, 0.5);
                g_assert_cmpfloat (blue, ==, 0.6);
                break;
        }
}

static void
test_redshift_wrapper_get_adjustment_method (ObjectFixture *fixture,
                                             gconstpointer  user_data)
{
        AdjustmentMethod method = redshiftgtk_backend_get_adjustment_method (fixture->backend);
        g_assert (method == ADJUSTMENT_METHOD_RANDR);
}

static void
test_redshift_wrapper_get_smooth_transition (ObjectFixture *fixture,
                                             gconstpointer  user_data)
{
        gboolean transition = redshiftgtk_backend_get_smooth_transition (fixture->backend);
        g_assert (transition == TRUE);
}

gint
main (gint   argc,
      gchar *argv[])
{
        g_test_init (&argc, &argv, NULL);

        g_test_add ("/Backend/RedshiftWrapper/get-config-path",
                    ObjectFixture,
                    NULL,
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_config_path,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-temperature-day",
                    ObjectFixture,
                    GINT_TO_POINTER (TIME_PERIOD_DAY),
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_temperature,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-temperature-night",
                    ObjectFixture,
                    GINT_TO_POINTER (TIME_PERIOD_NIGHT),
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_temperature,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-location-provider",
                    ObjectFixture,
                    GINT_TO_POINTER (TIME_PERIOD_NIGHT),
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_location_provider,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-latitude",
                    ObjectFixture,
                    NULL,
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_latitude,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-longtitude",
                    ObjectFixture,
                    NULL,
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_longtitude,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-brightness-day",
                    ObjectFixture,
                    GINT_TO_POINTER (TIME_PERIOD_DAY),
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_brightness,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-brightness-night",
                    ObjectFixture,
                    GINT_TO_POINTER (TIME_PERIOD_NIGHT),
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_brightness,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-gamma-day",
                    ObjectFixture,
                    GINT_TO_POINTER (TIME_PERIOD_DAY),
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_gamma,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-gamma-night",
                    ObjectFixture,
                    GINT_TO_POINTER (TIME_PERIOD_NIGHT),
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_gamma,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-adjustment-method",
                    ObjectFixture,
                    NULL,
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_adjustment_method,
                    redshift_wrapper_fixture_tear_down);

        g_test_add ("/Backend/RedshiftWrapper/get-smooth-transition",
                    ObjectFixture,
                    NULL,
                    redshift_wrapper_fixture_set_up,
                    test_redshift_wrapper_get_smooth_transition,
                    redshift_wrapper_fixture_tear_down);

        return g_test_run ();
}


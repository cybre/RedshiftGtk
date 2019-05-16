/* redshiftgtk-redshift-wrapper.c
 *
 * Copyright 2019 Stefan Ric
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <gio/gio.h>
#include <pwd.h>
#include <errno.h>
#include <glib/gi18n.h>

#include "redshiftgtk-redshift-wrapper.h"
#include "enums.h"

#define DEFAULT_SETTINGS_GROUP "redshift"
#define MANUAL_SETTINGS_GROUP "manual"

#define DEFAULT_DAY_TEMPERATURE 6500
#define DEFAULT_NIGHT_TEMPERATURE 4500
#define DEFAULT_BRIGHTNESS 1.0
#define DEFAULT_SMOOTH_TRANSITION FALSE

struct _RedshiftGtkRedshiftWrapper
{
        GObject parent_instance;

        RedshiftState redshift_state;
        GSubprocess *process;
        GKeyFile *config;
        gchar *config_path;
};

static void
redshiftgtk_backend_iface_init (RedshiftGtkBackendInterface *iface);

G_DEFINE_TYPE_WITH_CODE (RedshiftGtkRedshiftWrapper,
                         redshiftgtk_redshift_wrapper,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (REDSHIFTGTK_TYPE_BACKEND,
                                                redshiftgtk_backend_iface_init))

void
create_file_if_not_exists (gchar   *path,
                           GError **error)
{
        g_autoptr (GFile) file = NULL;

        file = g_file_new_for_path (path);
        g_file_create (file, G_FILE_CREATE_NONE, NULL, error);

        /* Invalidate the error if the file already exists */
        if (*error && g_error_matches (*error, G_IO_ERROR, G_IO_ERROR_EXISTS)) {
                *error = NULL;
        }
}

static void
redshiftgtk_redshift_wrapper_dispose (GObject *object)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (object);

        g_clear_object (&self->process);
        g_free (self->config_path);
}

static void
redshiftgtk_redshift_wrapper_class_init (RedshiftGtkRedshiftWrapperClass *klass)
{
        GObjectClass *obj_class = G_OBJECT_CLASS (klass);

        obj_class->dispose = redshiftgtk_redshift_wrapper_dispose;
}

static void
redshiftgtk_redshift_wrapper_init (RedshiftGtkRedshiftWrapper *self)
{
        g_autoptr (GError) error = NULL;
        const gchar* user_config_path = NULL;

        self->redshift_state = REDSHIFT_STATE_UNDEFINED;
        self->process = NULL;

        user_config_path = g_get_user_config_dir ();

        if (!user_config_path) {
                g_warning ("redshiftgtk_redshift_wrapper_init\n\
        g_get_user_config_dir: Could not get user config directory\n");
                return;
        }

        self->config_path = g_build_filename (user_config_path, "redshift.conf", NULL);
        self->config = g_key_file_new ();

        create_file_if_not_exists (self->config_path, &error);
        if (error) {
                g_warning ("redshiftgtk_redshift_wrapper_init\n\
        create_file_if_not_exists: %s\n", error->message);
                return;
        }

        g_key_file_load_from_file (self->config,
                                   self->config_path,
                                   G_KEY_FILE_NONE, &error);

        if (error) {
                g_warning ("redshiftgtk_redshift_wrapper_init\n\
        g_key_file_load_from_file: %s\n", error->message);
        }
}

RedshiftGtkBackend*
redshiftgtk_redshift_wrapper_new ()
{
        return g_object_new (REDSHIFTGTK_TYPE_REDSHIFT_WRAPPER, NULL);
}

static void
redshiftgtk_redshift_wrapper_stop (RedshiftGtkBackend *backend)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);

        /* Kill using all methods just to be sure */
        g_subprocess_new (G_SUBPROCESS_FLAGS_NONE, NULL,
                          "redshift", "-x", NULL);
        g_subprocess_new (G_SUBPROCESS_FLAGS_NONE, NULL,
                          "redshift", "-x", "-m", "randr", NULL);
        g_subprocess_new (G_SUBPROCESS_FLAGS_NONE, NULL,
                          "redshift", "-x", "-m", "vidmode", NULL);
        g_subprocess_new (G_SUBPROCESS_FLAGS_NONE, NULL,
                          "killall", "-s", "KILL", "redshift", NULL);
        g_subprocess_new (G_SUBPROCESS_FLAGS_NONE, NULL,
                          "killall", "-s", "KILL", "redshift-gtk", NULL);
        if (self->process) {
                g_subprocess_force_exit (self->process);
        }

        self->redshift_state = REDSHIFT_STATE_STOPPED;
}

static void
redshiftgtk_redshift_wrapper_start (RedshiftGtkBackend *backend,
                                    GError            **error)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);

        if (self->redshift_state != REDSHIFT_STATE_STOPPED) {
                redshiftgtk_redshift_wrapper_stop (backend);
        }

        self->process = g_subprocess_new (G_SUBPROCESS_FLAGS_NONE, error,
                                          "redshift",
                                          NULL);

        self->redshift_state = REDSHIFT_STATE_RUNNING;
}

static gdouble
redshiftgtk_redshift_wrapper_get_temperature (RedshiftGtkBackend *backend,
                                              TimePeriod          period)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);
        const gchar* key;

        switch (period) {
        case TIME_PERIOD_DAY:
                key = "temp-day";
                break;
        case TIME_PERIOD_NIGHT:
                key = "temp-night";
                break;
        default:
                return 0;
        }

        g_autoptr (GError) error = NULL;
        gdouble temperature = g_key_file_get_double (self->config,
                                                     DEFAULT_SETTINGS_GROUP,
                                                     key, &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_temperature\n\
        g_key_file_get_double: %s\n",
                         error->message);
                if (period == TIME_PERIOD_DAY) {
                        return DEFAULT_DAY_TEMPERATURE;
                } else if (period == TIME_PERIOD_NIGHT) {
                        return DEFAULT_NIGHT_TEMPERATURE;
                }
        }

        return temperature;
}

void
redshiftgtk_redshift_wrapper_set_temperature (RedshiftGtkBackend *backend,
                                              TimePeriod          period,
                                              gdouble             temperature)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);
        const gchar* key;

        switch (period) {
        case TIME_PERIOD_DAY:
                key = "temp-day";
                break;
        case TIME_PERIOD_NIGHT:
                key = "temp-night";
                break;
        default:
                return;
        }

        g_key_file_set_double (self->config, DEFAULT_SETTINGS_GROUP,
                               key, temperature);
}

LocationProvider
redshiftgtk_redshift_wrapper_get_location_provider (RedshiftGtkBackend *backend)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        g_autoptr (GError) error = NULL;
        g_autofree const gchar *provider;

        provider = g_key_file_get_string (self->config,
                                          DEFAULT_SETTINGS_GROUP,
                                          "location-provider", &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_location_provider\n\
        g_key_file_get_string: %s\n",
                         error->message);
                return LOCATION_PROVIDER_AUTO;
        }

        if (g_strcmp0 (provider, "manual") == 0) {
                return LOCATION_PROVIDER_MANUAL;
        }

        return LOCATION_PROVIDER_AUTO;
}

void
redshiftgtk_redshift_wrapper_set_location_provider (RedshiftGtkBackend *backend,
                                                    LocationProvider    provider)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);
        gchar *provider_string = NULL;

        switch (provider) {
        case LOCATION_PROVIDER_MANUAL:
                provider_string = "manual";
                break;
        default:
                provider_string = "geoclue2";
        }

        g_key_file_set_string (self->config, DEFAULT_SETTINGS_GROUP,
                               "location-provider", provider_string);
}

gdouble
redshiftgtk_redshift_wrapper_get_latitude (RedshiftGtkBackend *backend)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        g_autoptr (GError) error = NULL;
        gdouble latitude = g_key_file_get_double (self->config,
                                                  MANUAL_SETTINGS_GROUP,
                                                  "lat", &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_latitude\n\
        g_key_file_get_double: %s\n",
                         error->message);
                return 0;
        }

        return latitude;
}

void
redshiftgtk_redshift_wrapper_set_latitude (RedshiftGtkBackend *backend,
                                           gdouble             latitude)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        g_key_file_set_string (self->config, MANUAL_SETTINGS_GROUP,
                               "lat", g_strdup_printf ("%.2f", latitude));
}

gdouble
redshiftgtk_redshift_wrapper_get_longtitude (RedshiftGtkBackend *backend)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        g_autoptr (GError) error = NULL;
        gdouble longtitude = g_key_file_get_double (self->config,
                                                    MANUAL_SETTINGS_GROUP,
                                                    "lon", &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_longtitude\n\
        g_key_file_get_double: %s\n",
                         error->message);
                return 0;
        }

        return longtitude;
}

void
redshiftgtk_redshift_wrapper_set_longtitude (RedshiftGtkBackend *backend,
                                             gdouble             longtitude)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        g_key_file_set_string (self->config, MANUAL_SETTINGS_GROUP,
                               "lon", g_strdup_printf ("%.2f", longtitude));
}

gdouble
redshiftgtk_redshift_wrapper_get_brightness (RedshiftGtkBackend *backend,
                                             TimePeriod          period)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);
        const gchar* key;

        switch (period) {
        case TIME_PERIOD_DAY:
                key = "brightness-day";
                break;
        case TIME_PERIOD_NIGHT:
                key = "brightness-night";
                break;
        default:
                return DEFAULT_BRIGHTNESS;
        }

        g_autoptr (GError) error = NULL;
        gdouble brightness = g_key_file_get_double (self->config,
                                                    DEFAULT_SETTINGS_GROUP,
                                                    key, &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_brightness\n\
        g_key_file_get_double: %s\n",
                         error->message);
                return DEFAULT_BRIGHTNESS;
        }

        return brightness;
}

void
redshiftgtk_redshift_wrapper_set_brightness (RedshiftGtkBackend *backend,
                                             TimePeriod          period,
                                             gdouble             brightness)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);
        const gchar* key;

        switch (period) {
        case TIME_PERIOD_DAY:
                key = "brightness-day";
                break;
        case TIME_PERIOD_NIGHT:
                key = "brightness-night";
                break;
        default:
                return;
        }

        g_key_file_set_string (self->config, DEFAULT_SETTINGS_GROUP,
                               key, g_strdup_printf ("%.1f", brightness));
}

GArray*
redshiftgtk_redshift_wrapper_get_gamma (RedshiftGtkBackend *backend,
                                        TimePeriod          period)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        GArray *gamma = NULL;
        const gchar *gamma_string, *key;
        gdouble gamma_double, red, green, blue;
        g_autoptr (GError) error = NULL;
        g_auto (GStrv) gamma_parts = NULL;

        switch (period) {
        case TIME_PERIOD_DAY:
                key = "gamma-day";
                break;
        case TIME_PERIOD_NIGHT:
                key = "gamma-night";
                break;
        default:
                return NULL;
        }

        gamma = g_array_sized_new (FALSE, FALSE, sizeof (gdouble), 3);

        /* Try to retrieve the gamma in number format (gamma = 1.0) */
        gamma_double = g_key_file_get_double (self->config,
                                              DEFAULT_SETTINGS_GROUP,
                                              key, &error);

        /* If it is indeed single double value, return now */
        if (!error) {
                g_array_append_val (gamma, gamma_double);
                g_array_append_val (gamma, gamma_double);
                g_array_append_val (gamma, gamma_double);

                return gamma;
        }
        /* Otherwise continue to R:G:B string format */

        /* Reset error pointer */
        error = NULL;

        /* Get the gamma in R:G:B string format */
        gamma_string = g_key_file_get_string (self->config,
                                              DEFAULT_SETTINGS_GROUP,
                                              key, &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_gamma\n\
        g_key_file_get_string: %s\n", error->message);
                return NULL;
        }

        g_return_val_if_fail (strstr (gamma_string, ":") != NULL, NULL);

        /* Split our R:G:B string into parts */
        gamma_parts = g_strsplit (gamma_string, ":", 3);
        g_return_val_if_fail (gamma_parts != NULL, NULL);

        /* Make sure we have 3 values */
        int parts = -1;
        while (gamma_parts[++parts] != NULL);
        g_return_val_if_fail (parts == 3, NULL);

        /* Convert our strings to doubles */
        sscanf (gamma_parts[0], "%lf", &red);
        sscanf (gamma_parts[1], "%lf", &green);
        sscanf (gamma_parts[2], "%lf", &blue);

        /* Add to array */
        g_array_append_val (gamma, red);
        g_array_append_val (gamma, green);
        g_array_append_val (gamma, blue);

        return gamma;
}

void
redshiftgtk_redshift_wrapper_set_gamma (RedshiftGtkBackend *backend,
                                        TimePeriod          period,
                                        gdouble             red,
                                        gdouble             green,
                                        gdouble             blue)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);
        const gchar *key;

        switch (period) {
        case TIME_PERIOD_DAY:
                key = "gamma-day";
                break;
        case TIME_PERIOD_NIGHT:
                key = "gamma-night";
                break;
        default:
                return;
        }

        /* R:G:B */
        const gchar *gamma = g_strdup_printf ("%.1f:%.1f:%.1f", red, green, blue);

        g_key_file_set_string (self->config, DEFAULT_SETTINGS_GROUP,
                               key, gamma);
}

AdjustmentMethod
redshiftgtk_redshift_wrapper_get_adjustment_method (RedshiftGtkBackend *backend)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        g_autoptr (GError) error = NULL;
        gchar *method = g_key_file_get_string (self->config,
                                               DEFAULT_SETTINGS_GROUP,
                                               "adjustment-method", &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_adjustment_method\n\
        g_key_file_get_string: %s\n",
                         error->message);
                return ADJUSTMENT_METHOD_AUTO;
        }

        if (g_strcmp0 (method, "randr") == 0) {
                return ADJUSTMENT_METHOD_RANDR;
        } else if (g_strcmp0 (method, "vidmode") == 0) {
                return ADJUSTMENT_METHOD_VIDMODE;
        }

        /* Fallback to auto */
        return ADJUSTMENT_METHOD_AUTO;
}

void redshiftgtk_redshift_wrapper_set_adjustment_method (RedshiftGtkBackend *backend,
                                                         AdjustmentMethod    method)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);
        const gchar *method_string;

        switch (method) {
        case ADJUSTMENT_METHOD_RANDR:
                method_string = "randr";
                break;
        case ADJUSTMENT_METHOD_VIDMODE:
                method_string = "vidmode";
                break;
        default:
                g_key_file_remove_key (self->config, DEFAULT_SETTINGS_GROUP,
                                       "adjustment-method", NULL);
                return;
        }

        g_key_file_set_string (self->config, DEFAULT_SETTINGS_GROUP,
                               "adjustment-method", method_string);
}

gboolean
redshiftgtk_redshift_wrapper_get_smooth_transition (RedshiftGtkBackend *backend)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);
        g_autoptr (GError) error = NULL;

        gboolean transition = g_key_file_get_double (self->config,
                                                    DEFAULT_SETTINGS_GROUP,
                                                    "fade", &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_smooth_transition\n\
        g_key_file_get_double: %s\n",
                         error->message);
                return DEFAULT_SMOOTH_TRANSITION;
        }

        return transition;
}

void
redshiftgtk_redshift_wrapper_set_smooth_transition (RedshiftGtkBackend *backend,
                                                    gboolean            transition)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        g_key_file_set_integer (self->config, DEFAULT_SETTINGS_GROUP,
                                "fade", transition);
}

gboolean
redshiftgtk_redshift_wrapper_get_autostart (RedshiftGtkBackend *self)
{
        g_autoptr (GFile) file = NULL;
        g_autoptr (GError) error = NULL;
        const gchar *user_config_path = NULL;
        g_autofree gchar *launcher = NULL;

        user_config_path = g_get_user_config_dir ();
        if (!user_config_path) {
                return FALSE;
        }

        launcher = g_build_filename (user_config_path,
                                     "autostart",
                                     "redshiftgtk.desktop",
                                     NULL);

        file = g_file_new_for_path (launcher);

        if (!g_file_query_exists (file, NULL)) {
                return FALSE;
        }

        GKeyFile *desktop = g_key_file_new ();
        g_key_file_load_from_file (desktop,
                                   launcher,
                                   G_KEY_FILE_NONE, &error);

        if (error) {
                g_debug ("redshiftgtk_redshift_wrapper_get_autostart\n\
        g_key_file_load_from_file: %s\n", error->message);
                return FALSE;
        }

        g_autofree gchar *value = g_key_file_get_string (desktop,
                                                         "Desktop Entry",
                                                         "Hidden",
                                                         &error);

        if (!error) {
                /* Return true if Hidden is false */
                return (g_strcmp0 (value, "false") == 0);
        } else {
                g_debug ("redshiftgtk_redshift_wrapper_get_autostart\n\
        g_key_file_get_string: %s\n", error->message);
        }

        value = g_key_file_get_string (desktop,
                                       "Desktop Entry",
                                       "X-GNOME-Autostart-enabled",
                                       &error);

        if (!error) {
                /* Return true if X-GNOME-Autostart-enabled is true */
                return (g_strcmp0 (value, "false") == 0);
        } else {
                g_debug ("redshiftgtk_redshift_wrapper_get_autostart\n\
        g_key_file_get_string: %s\n", error->message);
        }

        /* We couldn't get either of the keys. Default to FALSE */
        return FALSE;
}

void
redshiftgtk_redshift_wrapper_autostart_file_create_cb (GObject *source_object,
                                                       GAsyncResult *result,
                                                       gpointer user_data)
{
        g_autoptr (GFile) file = NULL;
        g_autoptr (GFileOutputStream) stream = NULL;
        g_autoptr (GDataOutputStream) output = NULL;
        g_autoptr (GError) error = NULL;
        gboolean autostart = FALSE;
        const gchar *autostart_value = NULL;
        const gchar *hidden_value = NULL;

        file = G_FILE (source_object);
        autostart = GPOINTER_TO_INT (user_data);
        stream = g_file_create_finish (file, result, &error);

        if (error) {
                g_warning ("redshiftgtk_redshift_wrapper_set_autostart_file_create\n\
        g_file_create_finish: %s\n", error->message);
                return;
        }

        hidden_value = (autostart) ? "false" : "true";
        autostart_value = (autostart) ? "true" : "false";

        output = g_data_output_stream_new (G_OUTPUT_STREAM (stream));
        g_data_output_stream_put_string (output, g_strdup_printf ("[Desktop Entry]\n\
Name=RedshiftGtkAutostart\n\
Exec=redshift\n\
Type=Application\n\
Hidden=%s\n\
X-GNOME-Autostart-enabled=%s", hidden_value, autostart_value), NULL, &error);

        if (error) {
                g_warning ("redshiftgtk_redshift_wrapper_set_autostart_file_create\n\
        g_data_output_stream_put_string: %s\n", error->message);
                g_file_delete (file, NULL, NULL);
        }
}

void
redshiftgtk_redshift_wrapper_set_autostart (RedshiftGtkBackend *self,
                                            gboolean            autostart,
                                            GError            **error)
{
        GFile *file = NULL;
        const gchar *user_config_path = NULL;
        g_autofree gchar *launcher = NULL;

        user_config_path = g_get_user_config_dir ();
        if (!user_config_path) {
                return;
        }

        launcher = g_build_filename (user_config_path,
                                     "autostart",
                                     "redshiftgtk.desktop",
                                     NULL);

        file = g_file_new_for_path (launcher);

        if (g_file_query_exists (file, NULL)) {
                GKeyFile *desktop = g_key_file_new ();
                g_key_file_load_from_file (desktop,
                                           launcher,
                                           G_KEY_FILE_NONE, error);

                if (*error) {
                        return;
                }

                if (autostart) {
                        g_key_file_set_string (desktop,
                                                "Desktop Entry",
                                                "Hidden",
                                                "false");
                        g_key_file_set_string (desktop,
                                                "Desktop Entry",
                                                "X-GNOME-Autostart-enabled",
                                                "true");
                } else {
                        g_key_file_set_string (desktop,
                                                "Desktop Entry",
                                                "Hidden",
                                                "true");
                        g_key_file_set_string (desktop,
                                                "Desktop Entry",
                                                "X-GNOME-Autostart-enabled",
                                                "false");
                }

                g_key_file_save_to_file (desktop, launcher, error);

                /* It already exists, don't write into it again */
                return;
        }

        /* Make sure ~/.config/autostart exists */
        if (g_mkdir_with_parents (g_path_get_dirname (launcher), 0775) == -1) {
                int errsv = errno;
                g_set_error (error, G_IO_ERROR,
                             g_io_error_from_errno (errsv),
                             _("Could not create parent directories: %s"),
                             g_strerror (errsv));
                return;
        }

        g_file_create_async (file,
                             G_FILE_CREATE_NONE,
                             G_PRIORITY_DEFAULT,
                             NULL,
                             redshiftgtk_redshift_wrapper_autostart_file_create_cb,
                             GINT_TO_POINTER (autostart));
}

void
redshiftgtk_redshift_wrapper_apply_changes (RedshiftGtkBackend *backend,
                                            GError            **error)
{
        RedshiftGtkRedshiftWrapper *self = REDSHIFTGTK_REDSHIFT_WRAPPER (backend);
        g_assert (self->config != NULL);

        create_file_if_not_exists (self->config_path, error);

        if (*error) {
                return;
        }

        g_key_file_save_to_file (self->config, self->config_path, error);
}

/* Connect our methods to the interface */
static void
redshiftgtk_backend_iface_init (RedshiftGtkBackendInterface *iface)
{
        iface->start = redshiftgtk_redshift_wrapper_start;
        iface->stop = redshiftgtk_redshift_wrapper_stop;
        iface->get_temperature = redshiftgtk_redshift_wrapper_get_temperature;
        iface->set_temperature = redshiftgtk_redshift_wrapper_set_temperature;
        iface->get_location_provider = redshiftgtk_redshift_wrapper_get_location_provider;
        iface->set_location_provider = redshiftgtk_redshift_wrapper_set_location_provider;
        iface->get_latitude = redshiftgtk_redshift_wrapper_get_latitude;
        iface->set_latitude = redshiftgtk_redshift_wrapper_set_latitude;
        iface->get_longtitude = redshiftgtk_redshift_wrapper_get_longtitude;
        iface->set_longtitude = redshiftgtk_redshift_wrapper_set_longtitude;
        iface->get_brightness = redshiftgtk_redshift_wrapper_get_brightness;
        iface->set_brightness = redshiftgtk_redshift_wrapper_set_brightness;
        iface->get_gamma = redshiftgtk_redshift_wrapper_get_gamma;
        iface->set_gamma = redshiftgtk_redshift_wrapper_set_gamma;
        iface->get_adjustment_method = redshiftgtk_redshift_wrapper_get_adjustment_method;
        iface->set_adjustment_method = redshiftgtk_redshift_wrapper_set_adjustment_method;
        iface->get_smooth_transition = redshiftgtk_redshift_wrapper_get_smooth_transition;
        iface->set_smooth_transition = redshiftgtk_redshift_wrapper_set_smooth_transition;
        iface->get_autostart = redshiftgtk_redshift_wrapper_get_autostart;
        iface->set_autostart = redshiftgtk_redshift_wrapper_set_autostart;
        iface->apply_changes = redshiftgtk_redshift_wrapper_apply_changes;
}

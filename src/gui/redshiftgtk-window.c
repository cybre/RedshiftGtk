/* redshiftgtk-window.c
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

#include <glib/gi18n.h>

#include "redshiftgtk-config.h"
#include "redshiftgtk-window.h"
#include "redshiftgtk-radial-slider.h"
#include "redshiftgtk-backend.h"
#include "redshiftgtk-redshift-wrapper.h"

typedef RedshiftGtkRadialSlider RadialSlider;

typedef void (*TryAgainDialogCallback) (RedshiftGtkWindow*);

struct _RedshiftGtkWindow
{
        GtkApplicationWindow parent_instance;

        /* Template widgets */
        GtkBox          *main_box;
        GtkOverlay      *day_overlay;
        GtkOverlay      *night_overlay;
        GtkStack        *location_stack;
        GtkSpinButton   *latitude_spinner;
        GtkSpinButton   *longtitude_spinner;
        GtkSpinButton   *day_brightness_spinner;
        GtkSpinButton   *day_gamma_r_spinner;
        GtkSpinButton   *day_gamma_g_spinner;
        GtkSpinButton   *day_gamma_b_spinner;
        GtkSpinButton   *night_brightness_spinner;
        GtkSpinButton   *night_gamma_r_spinner;
        GtkSpinButton   *night_gamma_g_spinner;
        GtkSpinButton   *night_gamma_b_spinner;
        GtkComboBoxText *method_combobox;
        GtkSwitch       *transition_switch;
        GtkSwitch       *autostart_switch;
        GtkButton       *stop_button;
        GtkButton       *apply_button;
        GtkButton       *cancel_button;

        /* Other widgets */
        RadialSlider    *day_temp_slider;
        RadialSlider    *night_temp_slider;

        /* Backend */
        RedshiftGtkBackend *backend;
};

G_DEFINE_TYPE (RedshiftGtkWindow, redshiftgtk_window,
               GTK_TYPE_APPLICATION_WINDOW)

static void
redshiftgtk_window_dispose (GObject *obj)
{
        RedshiftGtkWindow *self = REDSHIFTGTK_WINDOW (obj);

        g_clear_object (&self->backend);

        G_OBJECT_CLASS(redshiftgtk_window_parent_class)->dispose(obj);
}

static void
redshiftgtk_window_class_init (RedshiftGtkWindowClass *klass)
{
        GObjectClass   *object_class = G_OBJECT_CLASS (klass);
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

        object_class->dispose = redshiftgtk_window_dispose;

        gtk_widget_class_set_template_from_resource (widget_class,
                "/com/github/cybre/RedshiftGtk/ui/redshiftgtk-window.ui");
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              main_box);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              day_overlay);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              night_overlay);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              location_stack);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              latitude_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              longtitude_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              day_brightness_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              day_gamma_r_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              day_gamma_g_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              day_gamma_b_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              night_brightness_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              night_gamma_r_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              night_gamma_g_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              night_gamma_b_spinner);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              method_combobox);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              transition_switch);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              autostart_switch);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              stop_button);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              apply_button);
        gtk_widget_class_bind_template_child (widget_class, RedshiftGtkWindow,
                                              cancel_button);
}

void
redshiftgtk_window_populate_controls (RedshiftGtkWindow *self)
{
        /* Location provider */
        LocationProvider location_provider;
        location_provider = redshiftgtk_backend_get_location_provider (self->backend);
        switch (location_provider) {
        case LOCATION_PROVIDER_MANUAL:
                gtk_stack_set_visible_child_name (self->location_stack, "manual");
                break;
        default:
                gtk_stack_set_visible_child_name (self->location_stack, "automatic");
        }

        /* Latitude and longtitude */
        gtk_spin_button_set_value (self->latitude_spinner,
                                   redshiftgtk_backend_get_latitude (self->backend));
        gtk_spin_button_set_value (self->longtitude_spinner,
                                   redshiftgtk_backend_get_longtitude (self->backend));

        /* Day brightness */
        gtk_spin_button_set_value (self->day_brightness_spinner,
                redshiftgtk_backend_get_brightness (self->backend,
                                                    TIME_PERIOD_DAY));

        /* Day gamma */
        g_autoptr (GArray) gamma = redshiftgtk_backend_get_gamma (self->backend,
                                                                  TIME_PERIOD_DAY);
        if (gamma) {
                gtk_spin_button_set_value (self->day_gamma_r_spinner,
                                           g_array_index (gamma, gdouble, 0));
                gtk_spin_button_set_value (self->day_gamma_g_spinner,
                                           g_array_index (gamma, gdouble, 1));
                gtk_spin_button_set_value (self->day_gamma_b_spinner,
                                           g_array_index (gamma, gdouble, 2));
        }

        /* Night brightness */
        gtk_spin_button_set_value (self->night_brightness_spinner,
                redshiftgtk_backend_get_brightness (self->backend,
                                                    TIME_PERIOD_NIGHT));

        /* Night gamma */
        gamma = redshiftgtk_backend_get_gamma (self->backend, TIME_PERIOD_NIGHT);
        if (gamma) {
                gtk_spin_button_set_value (self->night_gamma_r_spinner,
                                           g_array_index (gamma, gdouble, 0));
                gtk_spin_button_set_value (self->night_gamma_g_spinner,
                                           g_array_index (gamma, gdouble, 1));
                gtk_spin_button_set_value (self->night_gamma_b_spinner,
                                           g_array_index (gamma, gdouble, 2));
        }

        /* Adjustment method */
        AdjustmentMethod method = redshiftgtk_backend_get_adjustment_method (self->backend);
        gtk_combo_box_set_active (GTK_COMBO_BOX (self->method_combobox), method);

        /* Smooth transition policy */
        gtk_switch_set_active (self->transition_switch,
                               redshiftgtk_backend_get_smooth_transition (self->backend));

        /* Autostart policy */
        gtk_switch_set_active (self->autostart_switch,
                               redshiftgtk_backend_get_autostart (self->backend));
}

void
adjustment_value_changed_cb (GtkAdjustment *adjustment, gpointer data)
{
        GtkSpinButton *entry = data;
        gdouble value = gtk_adjustment_get_value (adjustment);
        gtk_spin_button_set_value (entry, value);
}

void
spinner_value_changed_cb (GtkSpinButton *spinner, gpointer data)
{
        RadialSlider *slider = data;
        redshiftgtk_radial_slider_update(slider);
}

void
try_again_dialog_response_cb (GtkDialog *dialog,
                              gint       response_id,
                              gpointer   user_data)
{
        TryAgainDialogCallback callback = (TryAgainDialogCallback)user_data;
        RedshiftGtkWindow *self = REDSHIFTGTK_WINDOW (gtk_window_get_transient_for (GTK_WINDOW (dialog)));

        gtk_widget_destroy (GTK_WIDGET (dialog));

        if (response_id == GTK_RESPONSE_REJECT)
                return;

        callback (self);
}

void
redshiftgtk_window_show_try_again_dialog (RedshiftGtkWindow *self,
                                          const gchar *message,
                                          const gchar *error_message,
                                          TryAgainDialogCallback callback)
{
        GtkWidget *dialog;
        GtkWidget *content_area;
        GtkWidget *label;

        dialog = gtk_dialog_new_with_buttons (message,
                                              GTK_WINDOW (self),
                                              GTK_DIALOG_MODAL,
                                              _("Try Again"),
                                              GTK_RESPONSE_ACCEPT,
                                              _("Cancel"),
                                              GTK_RESPONSE_REJECT,
                                              NULL);

        content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
        gtk_widget_set_margin_top (content_area, 3);
        gtk_widget_set_margin_start (content_area, 3);
        gtk_widget_set_margin_end (content_area, 3);
        gtk_widget_set_margin_bottom (content_area, 3);

        label = gtk_label_new (NULL);
        gtk_label_set_markup (GTK_LABEL (label),
                              g_strdup_printf ("<span font=\"12\">%s</span>",
                                               error_message));
        gtk_widget_set_margin_top (label, 20);
        gtk_widget_set_margin_start (label, 20);
        gtk_widget_set_margin_end (label, 20);
        gtk_widget_set_margin_bottom (label, 20);
        gtk_container_add (GTK_CONTAINER (content_area), label);

        gtk_widget_show_all (dialog);

        g_signal_connect (GTK_DIALOG (dialog), "response",
                          G_CALLBACK (try_again_dialog_response_cb),
                          callback);
}

void
backend_set_autostart_cb (RedshiftGtkWindow *self)
{
        g_autoptr (GError) error = NULL;

        redshiftgtk_backend_set_autostart (self->backend,
                gtk_switch_get_active (self->autostart_switch),
                &error);

        if (error) {
                g_warning ("redshiftgtk_backend_set_autostart: %s\n", error->message);
                redshiftgtk_window_show_try_again_dialog (self,
                                                          _("Could not enable autostart"),
                                                          error->message,
                                                          &backend_set_autostart_cb);
                gtk_switch_set_active (self->autostart_switch, FALSE);
        }
}

void
backend_apply_changes_cb (RedshiftGtkWindow *self)
{
        g_autoptr (GError) error = NULL;

        redshiftgtk_backend_apply_changes (self->backend, &error);

        if (error) {
                g_warning ("redshiftgtk_backend_apply_changes: %s\n", error->message);
                redshiftgtk_window_show_try_again_dialog (self,
                                                          _("Could not apply changes"),
                                                          error->message,
                                                          &backend_set_autostart_cb);
        }
}

void
backend_start_cb (RedshiftGtkWindow *self)
{
        g_autoptr (GError) error = NULL;

        redshiftgtk_backend_start (self->backend, &error);

        if (error) {
                g_warning ("redshiftgtk_backend_start: %s\n", error->message);
                redshiftgtk_window_show_try_again_dialog (self,
                                                          _("Could not start redshift"),
                                                          error->message,
                                                          &backend_start_cb);
        }
}

void
apply_button_clicked_cb (GtkWidget *widget, gpointer data)
{
        RedshiftGtkWindow *self = data;

        /* Stop before applying settings */
        redshiftgtk_backend_stop (self->backend);

        /* Day temperature */
        redshiftgtk_backend_set_temperature(self->backend,
                TIME_PERIOD_DAY,
                redshiftgtk_radial_slider_get_value(self->day_temp_slider));

        /* Night temperature */
        redshiftgtk_backend_set_temperature(self->backend,
                TIME_PERIOD_NIGHT,
                redshiftgtk_radial_slider_get_value(self->night_temp_slider));

        /* Location provider */
        if (g_strcmp0 (gtk_stack_get_visible_child_name (self->location_stack), "manual") == 0)
                redshiftgtk_backend_set_location_provider (self->backend,
                                                           LOCATION_PROVIDER_MANUAL);
        else
                redshiftgtk_backend_set_location_provider (self->backend,
                                                           LOCATION_PROVIDER_AUTO);

        /* Latitude and longtitude */
        redshiftgtk_backend_set_latitude (self->backend,
                gtk_spin_button_get_value (self->latitude_spinner));
        redshiftgtk_backend_set_longtitude (self->backend,
                gtk_spin_button_get_value (self->longtitude_spinner));

        /* Day brightenss */
        redshiftgtk_backend_set_brightness (self->backend,
                TIME_PERIOD_DAY,
                gtk_spin_button_get_value (self->day_brightness_spinner));

        /* Day gamma */
        redshiftgtk_backend_set_gamma (self->backend,
                TIME_PERIOD_DAY,
                gtk_spin_button_get_value (self->day_gamma_r_spinner),
                gtk_spin_button_get_value (self->day_gamma_g_spinner),
                gtk_spin_button_get_value (self->day_gamma_b_spinner));

        /* Night brightness */
        redshiftgtk_backend_set_brightness (self->backend,
                TIME_PERIOD_NIGHT,
                gtk_spin_button_get_value (self->night_brightness_spinner));

        /* Night gamma */
        redshiftgtk_backend_set_gamma (self->backend,
                TIME_PERIOD_NIGHT,
                gtk_spin_button_get_value (self->night_gamma_r_spinner),
                gtk_spin_button_get_value (self->night_gamma_g_spinner),
                gtk_spin_button_get_value (self->night_gamma_b_spinner));

        /* Adjustment method */
        AdjustmentMethod method = gtk_combo_box_get_active (
                GTK_COMBO_BOX (self->method_combobox));
        redshiftgtk_backend_set_adjustment_method (self->backend, method);

        /* Smooth transition policy */
        redshiftgtk_backend_set_smooth_transition (self->backend,
                gtk_switch_get_active (self->transition_switch));

        /* Autostart policy */
        backend_set_autostart_cb (self);

        /* Apply settings */
        backend_apply_changes_cb (self);

        /* Start again with new settings */
        backend_start_cb (self);
}

void
cancel_button_clicked_cb (GtkWidget *widget, gpointer data)
{
        RedshiftGtkWindow *self = data;

        gtk_window_close (GTK_WINDOW (self));
}

void
stop_button_clicked_cb (GtkWidget *widget, gpointer data)
{
        RedshiftGtkWindow *self = data;
        redshiftgtk_backend_stop (self->backend);
}

void
window_scale_factor_changed_cb (RedshiftGtkWindow *self,
                                gpointer data)
{
        gdouble factor = gtk_widget_get_scale_factor (GTK_WIDGET(self));

        if (factor == 1.0) {
                redshiftgtk_radial_slider_set_image_path (self->day_temp_slider,
                        "/com/github/cybre/RedshiftGtk/images/slider-day.png");
                redshiftgtk_radial_slider_set_image_path (self->night_temp_slider,
                        "/com/github/cybre/RedshiftGtk/images/slider-night.png");
        } else if (factor == 2.0) {
                redshiftgtk_radial_slider_set_image_path (self->day_temp_slider,
                        "/com/github/cybre/RedshiftGtk/images@2x/slider-day.png");
                redshiftgtk_radial_slider_set_image_path (self->night_temp_slider,
                        "/com/github/cybre/RedshiftGtk/images@2x/slider-night.png");
        }

}

/* Rule them all */
static void
redshiftgtk_window_init (RedshiftGtkWindow *self)
{
        GtkAdjustment *day_adjustment, *night_adjustment;
        RadialSlider *radial;
        GtkWidget *day_entry, *night_entry;
        GtkStyleContext *style_ctx;
        GdkScreen *screen;
        g_autoptr(GtkCssProvider) provider = NULL;
        gdouble value;
        const gchar *image_resource_path;

        gtk_widget_init_template (GTK_WIDGET (self));
        self->backend = redshiftgtk_redshift_wrapper_new();

        /* Have it always be initialized */
        image_resource_path = "/com/github/cybre/RedshiftGtk/images/";

        gdouble factor = gtk_widget_get_scale_factor (GTK_WIDGET (self));
        if (factor == 2.0)
                image_resource_path = "/com/github/cybre/RedshiftGtk/images@2x/";

        screen = gdk_screen_get_default ();
        provider = gtk_css_provider_new ();
        gtk_css_provider_load_from_resource (provider,
                "/com/github/cybre/RedshiftGtk/style/custom.css");
        gtk_style_context_add_provider_for_screen (screen,
                GTK_STYLE_PROVIDER (provider),
                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        /* Find them */
        /* Day temperature */
        value = redshiftgtk_backend_get_temperature(self->backend,
                                                    TIME_PERIOD_DAY);
        day_adjustment = gtk_adjustment_new (value, 1000.00, 12000.00,
                                             50.00, 100.0, 0);
        radial = redshiftgtk_radial_slider_new (day_adjustment, 256.0);
        redshiftgtk_radial_slider_set_image_path (radial,
                g_strconcat (image_resource_path, "slider-day.png", NULL));
        redshiftgtk_radial_slider_set_track_width (radial, 10.0);
        redshiftgtk_radial_slider_set_knob_radius (radial, 15.0);
        redshiftgtk_radial_slider_set_render_fill (radial, FALSE);
        redshiftgtk_radial_slider_set_render_value (radial, FALSE);
        gtk_container_add (GTK_CONTAINER (self->day_overlay),
                           GTK_WIDGET (radial));
        self->day_temp_slider = radial;

        day_entry = gtk_spin_button_new (day_adjustment, 50, 0);
        style_ctx = gtk_widget_get_style_context (day_entry);
        gtk_style_context_add_class (style_ctx, "custom-vertical");
        gtk_orientable_set_orientation(GTK_ORIENTABLE (day_entry),
                                       GTK_ORIENTATION_VERTICAL);
        gtk_widget_set_halign (day_entry, GTK_ALIGN_CENTER);
        gtk_widget_set_valign (day_entry, GTK_ALIGN_CENTER);
        gtk_overlay_add_overlay (self->day_overlay, GTK_WIDGET (day_entry));

        /* Night temperature */
        value = redshiftgtk_backend_get_temperature(self->backend,
                                                    TIME_PERIOD_NIGHT);
        night_adjustment = gtk_adjustment_new (value, 1000.00, 12000.00,
                                               50.00, 100.0, 0);
        radial = redshiftgtk_radial_slider_new (night_adjustment, 256.0);
        redshiftgtk_radial_slider_set_image_path (radial,
                g_strconcat (image_resource_path, "slider-night.png", NULL));
        redshiftgtk_radial_slider_set_track_width (radial, 10.0);
        redshiftgtk_radial_slider_set_knob_radius (radial, 15.0);
        redshiftgtk_radial_slider_set_render_fill (radial, FALSE);
        redshiftgtk_radial_slider_set_render_value (radial, FALSE);
        gtk_container_add (GTK_CONTAINER (self->night_overlay),
                           GTK_WIDGET (radial));
        self->night_temp_slider = radial;

        night_entry = gtk_spin_button_new (night_adjustment, 50, 0);
        style_ctx = gtk_widget_get_style_context (night_entry);
        gtk_style_context_add_class (style_ctx, "custom-vertical");
        gtk_orientable_set_orientation(GTK_ORIENTABLE (night_entry),
                                       GTK_ORIENTATION_VERTICAL);
        gtk_widget_set_halign (night_entry, GTK_ALIGN_CENTER);
        gtk_widget_set_valign (night_entry, GTK_ALIGN_CENTER);
        gtk_overlay_add_overlay (self->night_overlay, GTK_WIDGET (night_entry));

        /* Set initial values */
        redshiftgtk_window_populate_controls (self);

        /* Bring them all */
        gtk_widget_show_all (GTK_WIDGET (self));

        /* In the darkness bind them */
        g_signal_connect (G_OBJECT (day_adjustment), "value-changed",
                          G_CALLBACK (adjustment_value_changed_cb),
                          day_entry);

        g_signal_connect (G_OBJECT (day_entry), "value-changed",
                          G_CALLBACK (spinner_value_changed_cb),
                          self->day_temp_slider);

        g_signal_connect (G_OBJECT (night_adjustment), "value-changed",
                          G_CALLBACK (adjustment_value_changed_cb),
                          night_entry);

        g_signal_connect (G_OBJECT (night_entry), "value-changed",
                          G_CALLBACK (spinner_value_changed_cb),
                          self->night_temp_slider);

        g_signal_connect (G_OBJECT (self->stop_button), "clicked",
                          G_CALLBACK (stop_button_clicked_cb),
                          self);

        g_signal_connect (G_OBJECT (self->apply_button), "clicked",
                          G_CALLBACK (apply_button_clicked_cb),
                          self);

        g_signal_connect (G_OBJECT (self->cancel_button), "clicked",
                          G_CALLBACK (cancel_button_clicked_cb),
                          self);

        g_signal_connect (G_OBJECT (self), "notify::scale-factor",
                          G_CALLBACK (window_scale_factor_changed_cb),
                          NULL);
}

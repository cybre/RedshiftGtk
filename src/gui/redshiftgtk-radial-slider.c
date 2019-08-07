/* redshiftgtk-radial-slider.c
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

#define _GNU_SOURCE

#include <math.h>

#include "redshiftgtk-radial-slider.h"

enum {
        PROP_ADJUSTMENT = 1,
        PROP_WIDGET_SIZE = 2,
        PROP_TRACK_WIDTH = 3,
        PROP_KNOB_RADIUS = 4,
        PROP_RENDER_FILL = 5,
        PROP_RENDER_VALUE = 6,
        N_PROPS
};

struct _RedshiftGtkRadialSliderPrivate {
        GtkAdjustment *adjustment;
        gdouble widget_size;
        gdouble track_width;
        gdouble knob_radius;
        gboolean render_fill;
        gboolean render_value;

        /* Track some stuff */
        gdouble radius;
        gdouble target;
        gdouble max_diff;
        gdouble center_point;
        GdkPixbuf *bg_pixbuf;
        GdkPixbuf *knob_pixbuf;
        gdouble map_slope;
};

static GParamSpec *obj_properties[N_PROPS] = {
        NULL,
};

G_DEFINE_TYPE_WITH_PRIVATE (RedshiftGtkRadialSlider, redshiftgtk_radial_slider,
                            GTK_TYPE_DRAWING_AREA)

static void
redshiftgtk_radial_slider_load_pixbuf (GdkPixbuf **dest_pixbuf,
                                       gchar     *path)
{
        g_autoptr(GError) error = NULL;
        GdkPixbuf *image = NULL;

        image = gdk_pixbuf_new_from_resource (path, &error);

        if (error)
                printf ("redshiftgtk_radial_slider_load_pixbuf : %s\n",
                        error->message);

        *dest_pixbuf = gdk_pixbuf_copy (image);
        g_clear_object (&image);
}

void
redshiftgtk_radial_slider_update (RedshiftGtkRadialSlider *self)
{
        gdouble value, min_value;

        /* Update the knob position/fill arc */
        value = gtk_adjustment_get_value (self->priv->adjustment);
        min_value = gtk_adjustment_get_lower (self->priv->adjustment);
        self->priv->target = (value - min_value) / self->priv->map_slope;

        self->priv->radius = self->priv->center_point - 20;

        gtk_widget_queue_draw (GTK_WIDGET (self));
}

/**
 * Set GObject properties
 */
static void
redshiftgtk_radial_slider_set_property (GObject *object, guint id,
                                        const GValue *value, GParamSpec *spec)
{
        RedshiftGtkRadialSlider *self = REDSHIFTGTK_RADIAL_SLIDER (object);

        switch (id) {
        case PROP_ADJUSTMENT:
                self->priv->adjustment = g_value_get_object (value);
                gdouble lower = gtk_adjustment_get_lower (self->priv->adjustment);
                gdouble upper = gtk_adjustment_get_upper (self->priv->adjustment);
                /* We only need to calculate this once */
                self->priv->map_slope = (upper - lower) / 360;
                break;
        case PROP_WIDGET_SIZE:
                self->priv->widget_size = g_value_get_double (value);
                self->priv->center_point = self->priv->widget_size / 2.0;
                gtk_widget_set_size_request (GTK_WIDGET (self),
                                             self->priv->widget_size,
                                             self->priv->widget_size);
                break;
        case PROP_TRACK_WIDTH:
                self->priv->track_width = g_value_get_double (value);
                break;
        case PROP_KNOB_RADIUS:
                self->priv->knob_radius = g_value_get_double (value);
                break;
        case PROP_RENDER_FILL:
                self->priv->render_fill = g_value_get_boolean (value);
                break;
        case PROP_RENDER_VALUE:
                self->priv->render_value = g_value_get_boolean (value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, id, spec);
                break;
        }
        redshiftgtk_radial_slider_update (self);
}

/**
 * Get GObject properties
 */
static void
redshiftgtk_radial_slider_get_property (GObject *object,
                                        guint id,
                                        GValue *value,
                                        GParamSpec *spec)
{
        RedshiftGtkRadialSlider *self = REDSHIFTGTK_RADIAL_SLIDER (object);

        switch (id) {
        case PROP_ADJUSTMENT:
                g_value_set_object (value, self->priv->adjustment);
                break;
        case PROP_WIDGET_SIZE:
                g_value_set_double (value, self->priv->widget_size);
                break;
        case PROP_TRACK_WIDTH:
                g_value_set_double (value, self->priv->track_width);
                break;
        case PROP_KNOB_RADIUS:
                g_value_set_double (value, self->priv->knob_radius);
                break;
        case PROP_RENDER_FILL:
                g_value_set_boolean (value, self->priv->render_fill);
                break;
        case PROP_RENDER_VALUE:
                g_value_set_boolean (value, self->priv->render_value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, id, spec);
                break;
        }
}

/**
 * redshiftgtk_radial_slider_new:
 *
 * Construct a new RedshiftGtkRadialSlider widget
 */
RedshiftGtkRadialSlider*
redshiftgtk_radial_slider_new (GtkAdjustment  *adjustment,
                               gdouble widget_size)
{
        g_assert (adjustment != NULL);

        return g_object_new (REDSHIFTGTK_TYPE_RADIAL_SLIDER,
                             "adjustment",  adjustment,
                             "widget-size", widget_size,
                             NULL);
}

/**
 * Handle cleanup
 */
static void
redshiftgtk_radial_slider_dispose (GObject *obj)
{
        RedshiftGtkRadialSlider *self = REDSHIFTGTK_RADIAL_SLIDER (obj);

        g_clear_object (&self->priv->bg_pixbuf);
        g_clear_object (&self->priv->knob_pixbuf);

        G_OBJECT_CLASS(redshiftgtk_radial_slider_parent_class)->dispose(obj);
}

static gint
round_by (gint n, gint by)
{
    gint a = (n / by) * by;
    gint b = a + by;

    return (n - a > b - n)? b : a;
}

static gboolean
redshiftgtk_radial_slider_motion_notify_event (GtkWidget *widget,
                                               GdkEventMotion *event)
{
        RedshiftGtkRadialSlider *self = NULL;
        gint x, y, step_inc, target_value, rounded_target_value;
        gdouble target, diff, arctangent, min_value, max_value;
        GdkModifierType state;

        self = REDSHIFTGTK_RADIAL_SLIDER (widget);

        gdk_window_get_device_position (event->window,
                                        event->device,
                                        &x, &y, &state);

        /* Figure out the angle based on the pointer position */
        arctangent = atan2 (x - self->priv->center_point,
                      y - self->priv->center_point);
        target = -arctangent / (M_PI / 180) + 180;
        diff = fabs (target - self->priv->target);

        min_value = gtk_adjustment_get_lower (self->priv->adjustment);
        max_value = gtk_adjustment_get_upper (self->priv->adjustment);
        step_inc = gtk_adjustment_get_step_increment (self->priv->adjustment);

        /* Map the angle (0..360) to (min_value..max_value) */
        target_value = min_value + self->priv->map_slope * target;
        rounded_target_value = round_by (target_value, step_inc);
        if (diff < self->priv->max_diff && target_value < max_value) {
                self->priv->target = target;
                gtk_adjustment_set_value (self->priv->adjustment,
                                          rounded_target_value);
        }

        gtk_widget_queue_draw (widget);

        return GDK_EVENT_PROPAGATE;
}

static gboolean
redshiftgtk_radial_slider_scroll_event (GtkWidget *widget,
                                        GdkEventScroll *event)
{
        RedshiftGtkRadialSlider *self = NULL;
        gint value, page_inc;

        self = REDSHIFTGTK_RADIAL_SLIDER (widget);

        value = gtk_adjustment_get_value (self->priv->adjustment);
        page_inc = gtk_adjustment_get_page_increment (self->priv->adjustment);

        if (event->direction == GDK_SCROLL_UP)
                value += page_inc;
        else if (event->direction == GDK_SCROLL_DOWN)
                value -= page_inc;
        else
                return GDK_EVENT_PROPAGATE;

        gtk_adjustment_set_value (self->priv->adjustment, value);
        redshiftgtk_radial_slider_update (self);

        return GDK_EVENT_STOP;
}

static void
redshiftgtk_radial_slider_size_allocate (GtkWidget *widget,
                                         GtkAllocation *allocation)
{
        RedshiftGtkRadialSlider *self = REDSHIFTGTK_RADIAL_SLIDER (widget);

        allocation->width = self->priv->widget_size;
        allocation->height = self->priv->widget_size;

        GTK_WIDGET_CLASS (redshiftgtk_radial_slider_parent_class)
                ->size_allocate (widget, allocation);
}

/**
 * Do the actual drawing
 */
static gboolean
redshiftgtk_radial_slider_draw (GtkWidget *widget, cairo_t *cr)
{

        RedshiftGtkRadialSlider *self = NULL;
        gdouble knob_radius, track_width, radius, real_radius, knob_x, knob_y;
        GdkRGBA fg, track, knob = { 0 };

        self = REDSHIFTGTK_RADIAL_SLIDER (widget);
        knob_radius = self->priv->knob_radius;
        track_width = self->priv->track_width;
        radius = self->priv->radius - (track_width / 2);

        cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL);

        /* Default colours */
        /* TODO: Make it possible to change these */
        gdk_rgba_parse (&track, "#282828");
        gdk_rgba_parse (&fg, "#0083AD");
        gdk_rgba_parse (&knob, "#E2E2E2");

        gdouble scale_factor = gtk_widget_get_scale_factor (widget);

        /* Render the image if it exists, otherwise render the track */
        cairo_save (cr);
        if (self->priv->bg_pixbuf) {
                /* Scale the transformation matrix so that we could
                 * render the background image in full resolution on HiDpi
                 * displays
                 */
                cairo_scale (cr, 1.0 / scale_factor, 1.0 / scale_factor);
                gdk_cairo_set_source_pixbuf (cr, self->priv->bg_pixbuf,
                                             0, 0);
                cairo_paint (cr);
        } else {
                cairo_translate (cr, self->priv->center_point,
                                 self->priv->center_point - radius);
                cairo_rotate (cr, -90 * M_PI / 180);
                cairo_set_source_rgba (cr, track.red, track.green,
                                       track.blue, track.alpha);
                cairo_set_line_width (cr, track_width);
                cairo_arc (cr, -radius, 0, radius, 0, M_PI * 2);
                cairo_stroke (cr);
                cairo_move_to (cr, -self->priv->track_width, 0);
                cairo_line_to (cr, self->priv->track_width, 0);
                cairo_set_source_rgba (cr, 255, 255, 255, 1);
                cairo_set_line_width (cr, 4);
                cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
                cairo_stroke (cr);
        }
        cairo_restore (cr);

        if (self->priv->render_fill) {
                /* Render the slider arc */
                cairo_save (cr);
                cairo_translate (cr, self->priv->center_point,
                                 self->priv->center_point - radius);
                cairo_rotate (cr, -90 * M_PI / 180);
                cairo_set_source_rgba (cr, fg.red, fg.green, fg.blue, fg.alpha);
                cairo_set_line_width (cr, track_width+1);
                cairo_arc (cr, -radius, 0, radius,
                           0, self->priv->target * M_PI/180);
                cairo_stroke (cr);
                cairo_restore (cr);
        }

        /* Render the knob */
        real_radius = self->priv->widget_size / 2.0;
        knob_x = round ((self->priv->radius - (track_width / 2.0)) *
                 sin (self->priv->target * M_PI / 180.0)) + real_radius;
        knob_y = round ((self->priv->radius - (track_width / 2.0)) *
                 -cos (self->priv->target * M_PI / 180.0)) + real_radius;

        cairo_save (cr);
        if (self->priv->knob_pixbuf) {
                /* Scale the transformation matrix so that we could
                 * render the background image in full resolution on HiDpi
                 * displays
                 */
                cairo_scale (cr, 1.0 / scale_factor, 1.0 / scale_factor);
                gdk_cairo_set_source_pixbuf (cr, self->priv->knob_pixbuf,
                                             knob_x * scale_factor - knob_radius,
                                             knob_y * scale_factor - knob_radius);
                cairo_paint (cr);
        } else {
                cairo_set_source_rgba (cr, knob.red, knob.green, knob.blue, knob.alpha);
                cairo_set_line_width (cr, 1);
                cairo_arc (cr, knob_x, knob_y, knob_radius, 0, 2.0 * M_PI);
                cairo_stroke_preserve (cr);
                cairo_fill (cr);
        }
        cairo_restore (cr);

        if (self->priv->render_value) {
                /* Draw the value */
                cairo_text_extents_t extents;
                g_autofree gchar *text = g_strdup_printf("%.0f",
                        gtk_adjustment_get_value (self->priv->adjustment));
                cairo_save (cr);
                cairo_set_font_size (cr, 35);
                cairo_set_source_rgb (cr, 255, 255, 255);
                cairo_text_extents (cr, text, &extents);
                cairo_move_to (cr, self->priv->center_point - extents.width / 2.0,
                               self->priv->center_point + extents.height / 2.0);
                cairo_show_text (cr, text);
                cairo_restore (cr);
        }

        return GDK_EVENT_STOP;
}

/**
 * Class initialisation
 */
static void
redshiftgtk_radial_slider_class_init (RedshiftGtkRadialSliderClass *klass)
{
        GObjectClass *obj_class = G_OBJECT_CLASS (klass);
        GtkWidgetClass *wid_class = GTK_WIDGET_CLASS (klass);

        /* gobject vtable hookup */
        obj_class->dispose = redshiftgtk_radial_slider_dispose;
        obj_class->get_property = redshiftgtk_radial_slider_get_property;
        obj_class->set_property = redshiftgtk_radial_slider_set_property;

        /* widget vtable hookup */
        wid_class->draw = redshiftgtk_radial_slider_draw;
        wid_class->motion_notify_event
                = redshiftgtk_radial_slider_motion_notify_event;
        wid_class->scroll_event = redshiftgtk_radial_slider_scroll_event;
        wid_class->size_allocate = redshiftgtk_radial_slider_size_allocate;

        /**
         * RedshiftGtkRadialSlider:adjustment:
         *
         * The GtkAdjustment of this scale widget
         */
        obj_properties[PROP_ADJUSTMENT] =
            g_param_spec_object ("adjustment",
                                 "Adjustment",
                                 "The GtkAdjustment of this scale widget",
                                 GTK_TYPE_ADJUSTMENT,
                                 G_PARAM_READWRITE);

        /**
         * RedshiftGtkRadialSlider:widget-size:
         *
         * The size (width and height) of the widget
         */
        obj_properties[PROP_WIDGET_SIZE] =
            g_param_spec_double ("widget-size",
                                 "Widget size",
                                 "The size (width and height) of the widget",
                                 64.0,
                                 512.0,
                                 256.0,
                                 G_PARAM_READWRITE);

        /**
         * RedshiftGtkRadialSlider:track-width:
         *
         * The width of the /track
         */
        obj_properties[PROP_TRACK_WIDTH] =
            g_param_spec_double ("track-width",
                                 "Track width",
                                 "The width of the track",
                                 2.0,
                                 30.0,
                                 10.0,
                                 G_PARAM_READWRITE);

        /**
         * RedshiftGtkRadialSlider:knob-radius:
         *
         * The radius of the control knob
         */
        obj_properties[PROP_KNOB_RADIUS] =
            g_param_spec_double ("knob-radius",
                                 "Knob radius",
                                 "The radius of the control knob",
                                 5.0,
                                 30.0,
                                 15.0,
                                 G_PARAM_READWRITE);

        /**
         * RedshiftGtkRadialSlider:render-fill:
         *
         * Controls whether we draw a "progress" fill
         */
        obj_properties[PROP_RENDER_FILL] =
            g_param_spec_boolean ("render-fill",
                                 "Render fill",
                                 "Controls whether we draw a \"progress\" fill",
                                 TRUE,
                                 G_PARAM_READWRITE);

        /**
         * RedshiftGtkRadialSlider:render-value:
         *
         * Controls whether we draw the value
         */
        obj_properties[PROP_RENDER_VALUE] =
            g_param_spec_boolean ("render-value",
                                 "Render value",
                                 "Controls whether we draw the value",
                                 TRUE,
                                 G_PARAM_READWRITE);

        g_object_class_install_properties (obj_class, N_PROPS, obj_properties);
}

/**
 * Object methods
 */

GtkAdjustment*
redshiftgtk_radial_slider_get_adjustment (RedshiftGtkRadialSlider *self)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        return self->priv->adjustment;
}

void
redshiftgtk_radial_slider_set_adjustment (RedshiftGtkRadialSlider *self,
                                          GtkAdjustment *adjustment)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        g_assert (adjustment != NULL && GTK_IS_ADJUSTMENT (adjustment));

        self->priv->adjustment = adjustment;

        gdouble lower = gtk_adjustment_get_lower (self->priv->adjustment);
        gdouble upper = gtk_adjustment_get_upper (self->priv->adjustment);
        /* We only need to calculate this once */
        self->priv->map_slope = (upper - lower) / 360;
        redshiftgtk_radial_slider_update (self);
}

void
redshiftgtk_radial_slider_set_widget_size (RedshiftGtkRadialSlider *self,
                                           gdouble widget_size)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        self->priv->widget_size = widget_size;
        self->priv->center_point = widget_size / 2.0;
        gtk_widget_set_size_request (GTK_WIDGET (self),
                                     self->priv->widget_size,
                                     self->priv->widget_size);
        redshiftgtk_radial_slider_update (self);
}

void
redshiftgtk_radial_slider_set_track_width (RedshiftGtkRadialSlider *self,
                                           gdouble track_width)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        self->priv->track_width = track_width;
        redshiftgtk_radial_slider_update (self);
}

void redshiftgtk_radial_slider_set_knob_radius (RedshiftGtkRadialSlider *self,
                                                gdouble knob_radius)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        self->priv->knob_radius = knob_radius;
        redshiftgtk_radial_slider_update (self);
}

void
redshiftgtk_radial_slider_set_render_fill (RedshiftGtkRadialSlider *self,
                                           gboolean render_fill)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        self->priv->render_fill = render_fill;
        redshiftgtk_radial_slider_update (self);
}

void
redshiftgtk_radial_slider_set_render_value (RedshiftGtkRadialSlider *self,
                                           gboolean render_value)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        self->priv->render_value = render_value;
        gtk_widget_queue_draw (GTK_WIDGET (self));

}

void
redshiftgtk_radial_slider_set_bg_path (RedshiftGtkRadialSlider *self,
                                       gchar* image_path)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        g_assert (image_path != NULL);

        redshiftgtk_radial_slider_load_pixbuf (&(self->priv->bg_pixbuf), image_path);
        redshiftgtk_radial_slider_update (self);
}

void
redshiftgtk_radial_slider_set_knob_path (RedshiftGtkRadialSlider *self,
                                         gchar* image_path)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        g_assert (image_path != NULL);

        redshiftgtk_radial_slider_load_pixbuf (&(self->priv->knob_pixbuf), image_path);
        self->priv->knob_radius = gdk_pixbuf_get_width (self->priv->knob_pixbuf) / 2;
        redshiftgtk_radial_slider_update (self);
}

gdouble
redshiftgtk_radial_slider_get_value (RedshiftGtkRadialSlider *self)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        return gtk_adjustment_get_value (self->priv->adjustment);
}

void
redshiftgtk_radial_slider_set_value (RedshiftGtkRadialSlider *self,
                                     gdouble value)
{
        g_assert (self != NULL && REDSHIFTGTK_IS_RADIAL_SLIDER (self));
        g_assert (value <= gtk_adjustment_get_upper (self->priv->adjustment));
        g_assert (value >= gtk_adjustment_get_lower (self->priv->adjustment));
        gtk_adjustment_set_value (self->priv->adjustment, value);
}

/**
 * Instaniation
 */
static void
redshiftgtk_radial_slider_init (RedshiftGtkRadialSlider *self)
{
        self->priv = redshiftgtk_radial_slider_get_instance_private (self);
        self->priv->target = 0;
        self->priv->max_diff = 200;
        self->priv->bg_pixbuf = NULL;
        self->priv->knob_pixbuf = NULL;
        self->priv->map_slope = 0;

        gtk_widget_set_events (GTK_WIDGET (self), GDK_BUTTON_PRESS_MASK
                | GDK_BUTTON1_MOTION_MASK
                | GDK_SCROLL_MASK);
}

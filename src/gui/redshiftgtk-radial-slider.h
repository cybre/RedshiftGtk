/* redshiftgtk-radial-slider.h
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

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _RedshiftGtkRadialSliderPrivate RedshiftGtkRadialSliderPrivate;

#define REDSHIFTGTK_TYPE_RADIAL_SLIDER redshiftgtk_radial_slider_get_type ()
G_DECLARE_FINAL_TYPE (RedshiftGtkRadialSlider, redshiftgtk_radial_slider,
                      REDSHIFTGTK, RADIAL_SLIDER, GtkDrawingArea)

struct _RedshiftGtkRadialSliderClass {
        GtkDrawingAreaClass parent_class;
};

struct _RedshiftGtkRadialSlider {
        GtkDrawingArea parent;
        RedshiftGtkRadialSliderPrivate *priv;
};

RedshiftGtkRadialSlider*
redshiftgtk_radial_slider_new (GtkAdjustment *adjustment,
                               gdouble        size);

GtkAdjustment*
redshiftgtk_radial_slider_get_adjustment   (RedshiftGtkRadialSlider *self);
void
redshiftgtk_radial_slider_set_adjustment   (RedshiftGtkRadialSlider *self,
                                            GtkAdjustment           *adjustment);
void
redshiftgtk_radial_slider_set_widget_size  (RedshiftGtkRadialSlider *self,
                                            gdouble                  widget_size);
void
redshiftgtk_radial_slider_set_track_width  (RedshiftGtkRadialSlider *self,
                                            gdouble                  track_width);
void
redshiftgtk_radial_slider_set_knob_radius  (RedshiftGtkRadialSlider *self,
                                            gdouble                  knob_radius);
void
redshiftgtk_radial_slider_set_render_fill  (RedshiftGtkRadialSlider *self,
                                           gboolean                 render_fill);
void
redshiftgtk_radial_slider_set_render_value (RedshiftGtkRadialSlider *self,
                                            gboolean                 render_text);
void
redshiftgtk_radial_slider_set_bg_path      (RedshiftGtkRadialSlider *self,
                                            gchar                   *image_path);
void
redshiftgtk_radial_slider_set_knob_path    (RedshiftGtkRadialSlider *self,
                                            gchar                   *image_path);
gdouble
redshiftgtk_radial_slider_get_value        (RedshiftGtkRadialSlider *self);
void
redshiftgtk_radial_slider_set_value        (RedshiftGtkRadialSlider *self,
                                            gdouble                  value);
void redshiftgtk_radial_slider_update      (RedshiftGtkRadialSlider *self);

G_END_DECLS

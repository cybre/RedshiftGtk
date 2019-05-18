/* redshiftgtk-backend.h
 *
 * Copyright 2019 Stefan Ric
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <glib-object.h>

#include "../enums.h"

G_BEGIN_DECLS

#define REDSHIFTGTK_TYPE_BACKEND redshiftgtk_backend_get_type ()
G_DECLARE_INTERFACE (RedshiftGtkBackend, redshiftgtk_backend,
                     REDSHIFTGTK, BACKEND, GObject)

struct _RedshiftGtkBackendInterface
{
        GTypeInterface parent_iface;

        void     (*start)                      (RedshiftGtkBackend *self,
                                                GError           **error);
        void     (*stop)                       (RedshiftGtkBackend *self);
        gdouble  (*get_temperature)            (RedshiftGtkBackend *self,
                                                TimePeriod          period);
        void     (*set_temperature)            (RedshiftGtkBackend *self,
                                                TimePeriod          period,
                                                gdouble             temperature);
        LocationProvider
                 (*get_location_provider)      (RedshiftGtkBackend *self);
        void     (*set_location_provider)      (RedshiftGtkBackend *self,
                                                LocationProvider    provider);
        gdouble  (*get_latitude)               (RedshiftGtkBackend *self);
        void     (*set_latitude)               (RedshiftGtkBackend *self,
                                                gdouble             latitude);
        gdouble  (*get_longtitude)             (RedshiftGtkBackend *self);
        void     (*set_longtitude)             (RedshiftGtkBackend *self,
                                                gdouble             longtitude);
        gdouble  (*get_brightness)             (RedshiftGtkBackend *self,
                                                TimePeriod          period);
        void     (*set_brightness)             (RedshiftGtkBackend *self,
                                                TimePeriod          period,
                                                gdouble             brightness);
        GArray*  (*get_gamma)                  (RedshiftGtkBackend *self,
                                                TimePeriod          period);
        void     (*set_gamma)                  (RedshiftGtkBackend *self,
                                                TimePeriod          period,
                                                gdouble             red,
                                                gdouble             green,
                                                gdouble             blue);
        AdjustmentMethod
                 (*get_adjustment_method)      (RedshiftGtkBackend *self);
        void     (*set_adjustment_method)      (RedshiftGtkBackend *self,
                                                AdjustmentMethod    method);
        gboolean (*get_smooth_transition)      (RedshiftGtkBackend *self);
        void     (*set_smooth_transition)      (RedshiftGtkBackend *self,
                                                gboolean            transition);
        gboolean (*get_autostart)              (RedshiftGtkBackend *self);
        void     (*set_autostart)              (RedshiftGtkBackend *self,
                                                gboolean            autostart,
                                                GError            **error);
        void     (*apply_changes)              (RedshiftGtkBackend *self,
                                                GError            **error);
};

void redshiftgtk_backend_start                 (RedshiftGtkBackend *self,
                                                GError            **error);
void redshiftgtk_backend_stop                  (RedshiftGtkBackend *self);
gdouble
     redshiftgtk_backend_get_temperature       (RedshiftGtkBackend *self,
                                                TimePeriod          period);
void redshiftgtk_backend_set_temperature       (RedshiftGtkBackend *self,
                                                TimePeriod          period,
                                                gdouble             temperature);
LocationProvider
     redshiftgtk_backend_get_location_provider (RedshiftGtkBackend *self);
void redshiftgtk_backend_set_location_provider (RedshiftGtkBackend *self,
                                                LocationProvider    provider);
gdouble redshiftgtk_backend_get_latitude       (RedshiftGtkBackend *self);
void    redshiftgtk_backend_set_latitude       (RedshiftGtkBackend *self,
                                                gdouble             latitude);
gdouble redshiftgtk_backend_get_longtitude     (RedshiftGtkBackend *self);
void    redshiftgtk_backend_set_longtitude     (RedshiftGtkBackend *self,
                                                gdouble             longtitude);
gdouble redshiftgtk_backend_get_brightness     (RedshiftGtkBackend *self,
                                                TimePeriod          period);
void    redshiftgtk_backend_set_brightness     (RedshiftGtkBackend *self,
                                                TimePeriod          period,
                                                gdouble             brightness);
GArray* redshiftgtk_backend_get_gamma          (RedshiftGtkBackend *self,
                                                TimePeriod          period);
void redshiftgtk_backend_set_gamma             (RedshiftGtkBackend *self,
                                                TimePeriod          period,
                                                gdouble red,
                                                gdouble green,
                                                gdouble blue);
AdjustmentMethod
     redshiftgtk_backend_get_adjustment_method (RedshiftGtkBackend *self);
void redshiftgtk_backend_set_adjustment_method (RedshiftGtkBackend *self,
                                                AdjustmentMethod    method);
gboolean
     redshiftgtk_backend_get_smooth_transition (RedshiftGtkBackend *self);
void redshiftgtk_backend_set_smooth_transition (RedshiftGtkBackend *self,
                                                gboolean            transition);
gboolean redshiftgtk_backend_get_autostart     (RedshiftGtkBackend *self);
void redshiftgtk_backend_set_autostart         (RedshiftGtkBackend *self,
                                                gboolean            autostart,
                                                GError            **error);
void redshiftgtk_backend_apply_changes         (RedshiftGtkBackend *self,
                                                GError            **error);

G_END_DECLS

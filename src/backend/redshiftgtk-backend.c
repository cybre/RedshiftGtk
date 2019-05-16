/* redshiftgtk-backend.c
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

#include "redshiftgtk-backend.h"

G_DEFINE_INTERFACE (RedshiftGtkBackend, redshiftgtk_backend, G_TYPE_OBJECT)

static void
redshiftgtk_backend_default_init (RedshiftGtkBackendInterface *iface)
{
    /* add properties and signals to the interface here */
}

/**
 * redshiftgtk_backend_start
 *
 * Start redshift
 */
void
redshiftgtk_backend_start (RedshiftGtkBackend *self,
                           GError            **error)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));
        g_assert (error == NULL || *error == NULL);

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->start != NULL);

        iface->start (self, error);
}

/**
 * redshiftgtk_backend_stop
 *
 * Stop any running redshift instance
 * and remove its effects from the screen
 */
void
redshiftgtk_backend_stop (RedshiftGtkBackend *self)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->stop != NULL);

        iface->stop (self);
}

/**
 * redshiftgtk_backend_get_temperature
 *
 * Return the temperature for the specified time period
 */
gdouble
redshiftgtk_backend_get_temperature (RedshiftGtkBackend *self,
                                     TimePeriod          period)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_temperature != NULL);

        return iface->get_temperature (self, period);
}

/**
 * redshiftgtk_backend_set_temperature
 *
 * Set the temperature for the specified time period
 */
void
redshiftgtk_backend_set_temperature (RedshiftGtkBackend *self,
                                     TimePeriod          period,
                                     gdouble             temperature)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_temperature != NULL);

        iface->set_temperature (self, period, temperature);
}

/**
 * redshiftgtk_backend_get_location_provider
 *
 * Return the current location provider
 */
LocationProvider
redshiftgtk_backend_get_location_provider (RedshiftGtkBackend *self)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_location_provider != NULL);

        return iface->get_location_provider (self);
}

/**
 * redshiftgtk_backend_set_location_provider
 *
 * Set the new location provider
 */
void
redshiftgtk_backend_set_location_provider (RedshiftGtkBackend *self,
                                           LocationProvider    provider)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_location_provider != NULL);

        iface->set_location_provider (self, provider);
}

/**
 * redshiftgtk_backend_get_latitude
 *
 * Return the latitude for the <manual> location provider
 */
gdouble
redshiftgtk_backend_get_latitude (RedshiftGtkBackend *self)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_latitude != NULL);

        return iface->get_latitude (self);
}

/**
 * redshiftgtk_backend_set_latitude
 *
 * Set the latitude for the <manual> location provider
 */
void
redshiftgtk_backend_set_latitude (RedshiftGtkBackend *self,
                                  gdouble             latitude)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_latitude != NULL);

        iface->set_latitude (self, latitude);
}

/**
 * redshiftgtk_backend_get_longtitude
 *
 * Return the longtitude for the <manual> location provider
 */
gdouble
redshiftgtk_backend_get_longtitude (RedshiftGtkBackend *self)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_longtitude != NULL);

        return iface->get_longtitude (self);
}

/**
 * redshiftgtk_backend_set_longtitude
 *
 * Set the longtitude for the <manual> location provider
 */
void
redshiftgtk_backend_set_longtitude (RedshiftGtkBackend *self,
                                    gdouble             longtitude)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_longtitude != NULL);

        iface->set_longtitude (self, longtitude);
}

/**
 * redshiftgtk_backend_get_brightness
 *
 * Return the current brightness for the specified time period
 */
gdouble
redshiftgtk_backend_get_brightness (RedshiftGtkBackend *self,
                                    TimePeriod          period)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_brightness != NULL);

        return iface->get_brightness (self, period);
}

/**
 * redshiftgtk_backend_set_brightness
 *
 * Set the brightness for the specified time period
 */
void
redshiftgtk_backend_set_brightness (RedshiftGtkBackend *self,
                                    TimePeriod          period,
                                    gdouble             brightness)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_brightness != NULL);

        iface->set_brightness (self, period, brightness);
}

/**
 * redshiftgtk_backend_get_gamma
 *
 * Return the current gamma for the specified time period
 *
 * Returns an ordered GArray with three gdouble elements (R,G,B)
 */
GArray*
redshiftgtk_backend_get_gamma (RedshiftGtkBackend *self,
                               TimePeriod          period)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_gamma != NULL);

        return iface->get_gamma (self, period);
}

/**
 * redshiftgtk_backend_set_gamma
 *
 * Set the gamma values for the specified time period
 */
void
redshiftgtk_backend_set_gamma (RedshiftGtkBackend *self,
                               TimePeriod          period,
                               gdouble             red,
                               gdouble             green,
                               gdouble             blue)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_gamma != NULL);

        iface->set_gamma (self, period, red, green, blue);
}

/**
 * redshiftgtk_backend_get_adjustment_method
 *
 * Return the current adjustment method
 */
AdjustmentMethod
redshiftgtk_backend_get_adjustment_method (RedshiftGtkBackend *self)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_adjustment_method != NULL);

        return iface->get_adjustment_method (self);
}

/**
 * redshiftgtk_backend_set_adjustment_method
 *
 * Set the adjustment method
 */
void
redshiftgtk_backend_set_adjustment_method (RedshiftGtkBackend *self,
                                           AdjustmentMethod    method)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_adjustment_method != NULL);

        iface->set_adjustment_method (self, method);
}

/**
 * redshiftgtk_backend_get_smooth_transition
 *
 * Return the current effect smooth transition policy
 */
gboolean
redshiftgtk_backend_get_smooth_transition (RedshiftGtkBackend *self)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_smooth_transition != NULL);

        return iface->get_smooth_transition (self);
}

/**
 * redshiftgtk_backend_set_smooth_transition
 *
 * Set the effect smooth transition policy
 */
void
redshiftgtk_backend_set_smooth_transition (RedshiftGtkBackend *self,
                                           gboolean            transition)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_smooth_transition != NULL);

        iface->set_smooth_transition (self, transition);
}

gboolean
redshiftgtk_backend_get_autostart (RedshiftGtkBackend *self)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->get_autostart != NULL);

        return iface->get_autostart (self);
}

void
redshiftgtk_backend_set_autostart (RedshiftGtkBackend *self,
                                   gboolean            autostart,
                                   GError            **error)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));
        g_assert (error == NULL || *error == NULL);

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->set_autostart != NULL);

        iface->set_autostart (self, autostart, error);
}

/**
 * redshiftgtk_backend_apply_changes
 *
 * Apply all changes if needed
 */
void
redshiftgtk_backend_apply_changes (RedshiftGtkBackend *self,
                                   GError            **error)
{
        RedshiftGtkBackendInterface *iface;

        g_assert (REDSHIFTGTK_IS_BACKEND (self));
        g_assert (error == NULL || *error == NULL);

        iface = REDSHIFTGTK_BACKEND_GET_IFACE (self);
        g_assert (iface->apply_changes != NULL);

        iface->apply_changes (self, error);
}

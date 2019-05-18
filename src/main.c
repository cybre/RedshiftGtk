/* main.c
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

#include <gui/redshiftgtk-window.h>
#include "redshiftgtk-config.h"

static void
on_activate (GtkApplication *app)
{
        GtkWindow *window;

        g_assert (GTK_IS_APPLICATION (app));

        /* Get the current window or create one if necessary. */
        window = gtk_application_get_active_window (app);
        if (!window) {
                window = g_object_new (REDSHIFTGTK_TYPE_WINDOW,
                                       "application", app,
                                       "default-width", 600,
                                       "default-height", 700,
                                       NULL);
        }

        /* Ask the window manager/compositor to present the window. */
        gtk_window_present (window);
}

int
main (int argc, char *argv[])
{
        g_autoptr (GtkApplication) app = NULL;

        /* Set up gettext translations */
        bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
        bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
        textdomain (GETTEXT_PACKAGE);

        app = gtk_application_new ("com.github.cybre.RedshiftGtk", G_APPLICATION_FLAGS_NONE);
        g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);

        return g_application_run (G_APPLICATION (app), argc, argv);
}

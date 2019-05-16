/* redshiftgtk-window.h
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

#define REDSHIFTGTK_TYPE_WINDOW (redshiftgtk_window_get_type())

G_DECLARE_FINAL_TYPE (RedshiftGtkWindow, redshiftgtk_window,
                      REDSHIFTGTK, WINDOW, GtkApplicationWindow)

G_END_DECLS

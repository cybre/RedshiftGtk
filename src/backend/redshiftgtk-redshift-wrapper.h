/* redshiftgtk-redshift-wrapper.h
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

#include <glib-object.h>

#include "redshiftgtk-backend.h"

G_BEGIN_DECLS

#define REDSHIFTGTK_TYPE_REDSHIFT_WRAPPER redshiftgtk_redshift_wrapper_get_type()
G_DECLARE_FINAL_TYPE (RedshiftGtkRedshiftWrapper, redshiftgtk_redshift_wrapper,
                      REDSHIFTGTK, REDSHIFT_WRAPPER, GObject)

RedshiftGtkBackend*
redshiftgtk_redshift_wrapper_new ();

G_END_DECLS

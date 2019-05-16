/* enums.h
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

typedef enum {
        ADJUSTMENT_METHOD_AUTO = 0,
        ADJUSTMENT_METHOD_RANDR = 1,
        ADJUSTMENT_METHOD_VIDMODE = 2
} AdjustmentMethod;

typedef enum {
        REDSHIFT_STATE_UNDEFINED = 0,
        REDSHIFT_STATE_STOPPED = 1,
        REDSHIFT_STATE_RUNNING = 2
} RedshiftState;

typedef enum {
        LOCATION_PROVIDER_AUTO = 0,
        LOCATION_PROVIDER_MANUAL = 1
} LocationProvider;

typedef enum {
        TIME_PERIOD_DAY = 0,
        TIME_PERIOD_NIGHT = 1
} TimePeriod;

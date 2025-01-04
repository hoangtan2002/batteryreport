/* batteryreport-window.h
 *
 * Copyright 2024 tan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define BATTERYREPORT_TYPE_WINDOW (batteryreport_window_get_type())

G_DECLARE_FINAL_TYPE (BatteryreportWindow, batteryreport_window, BATTERYREPORT, WINDOW, AdwApplicationWindow)

G_END_DECLS

static void
batteryreport_window_show_not_found_page (BatteryreportWindow *self);

static void
batterreport_window_read_device_info (gpointer data, gpointer user_data);

static void
batterreport_window_write_device_info_to_buffer (gpointer data, gpointer user_data);

AdwExpanderRow*
batteryreport_window_new_row (char* device_name, const char* icon_name);

void
batterryeport_window_add_to_expander (AdwExpanderRow  *row,
                                      char            *property,
                                      char            *value);

void
batteryreport_window_makr_as_degraded (AdwExpanderRow* row);

static void
batteryreport_window_export_report (GtkButton* button,
                                    gpointer user_data);
static void
batteryreport_window_write_report (GObject* source,
                                   GAsyncResult* result,
                                   gpointer user_data);


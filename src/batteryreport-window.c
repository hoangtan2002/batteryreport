/* batteryreport-window.c
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

#include "config.h"

#include "batteryreport-window.h"
#include <libupower-glib/upower.h>

struct _BatteryreportWindow
{
	AdwApplicationWindow  parent_instance;

	/* Template widgets */
        AdwNavigationView *navigation_view;
        AdwPreferencesGroup* battery_group;
        AdwPreferencesGroup* ac_power_group;
        GtkButton *save_button;

        UpClient* client_object;
};

G_DEFINE_FINAL_TYPE (BatteryreportWindow, batteryreport_window, ADW_TYPE_APPLICATION_WINDOW)

static void
batteryreport_window_class_init (BatteryreportWindowClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class, "/com/tan/batteryreport/batteryreport-window.ui");
	gtk_widget_class_bind_template_child (widget_class, BatteryreportWindow, navigation_view);
        gtk_widget_class_bind_template_child (widget_class, BatteryreportWindow, battery_group);
        gtk_widget_class_bind_template_child (widget_class, BatteryreportWindow, ac_power_group);
        gtk_widget_class_bind_template_child (widget_class, BatteryreportWindow, save_button);
}

static void
batteryreport_window_init (BatteryreportWindow *self)
{
        g_autoptr (GPtrArray) up_client_devices;
	gtk_widget_init_template (GTK_WIDGET (self));
        self->client_object = up_client_new ();
        if(!self->client_object){
                batteryreport_window_show_not_found_page (self);
                return;
        }
        up_client_devices = up_client_get_devices2 (self->client_object);
        g_print("Num of installed devices: %d\n",up_client_devices->len);
        g_ptr_array_foreach (up_client_devices,
                             batterreport_window_read_device_info,
                             self);
}

static void
batteryreport_window_show_not_found_page (BatteryreportWindow *self)
{
        adw_navigation_view_push_by_tag(self->navigation_view,"battery-not-found-page");
}

static void
batterreport_window_read_device_info (gpointer data, gpointer user_data)
{
        UpDevice *device = UP_DEVICE (data);
        BatteryreportWindow* self = BATTERYREPORT_WINDOW (user_data);
        AdwExpanderRow* row;

        gboolean is_online;

        guint device_type;
        guint device_tech;

        int device_charge_cycles = 0;
        int device_health = 0;

        double device_nominal_cap = 0;
        double device_current_cap = 0;

        char* device_name = NULL;
        char* icon_name;
        char* device_model;
        char* device_vendor;
        char* device_nominal_cap_str;
        char* device_current_cap_str;
        char* device_charge_cycles_str;
        char* device_health_str;

        g_object_get (device,"kind",&device_type,NULL);
        g_object_get (device,"icon-name",&icon_name,NULL);

        g_object_get (device,"model",&device_model,NULL);
        g_object_get (device,"vendor",&device_vendor,NULL);
        g_object_get (device,"icon_name",&icon_name,NULL);
        g_object_get (device,"technology",&device_tech,NULL);
        g_object_get (device,"energy-full-design",&device_nominal_cap, NULL);
        g_object_get (device,"energy-full",&device_current_cap, NULL);
        g_object_get (device,"charge-cycles",&device_charge_cycles, NULL);
        g_object_get (device,"online",&is_online,NULL);

        switch (device_type) {
        case UP_DEVICE_KIND_BATTERY:
                device_health = (int)(100*(device_current_cap/device_nominal_cap));
                device_name = (char*) calloc ((strlen(device_vendor) + strlen(device_model) + 1), sizeof(char));
                device_nominal_cap_str = (char*) calloc (20,sizeof(char));
                device_current_cap_str = (char*) calloc (20,sizeof(char));
                device_charge_cycles_str = (char*) calloc (20,sizeof(char));
                device_health_str = (char*) calloc (20,sizeof(char));

                sprintf(device_nominal_cap_str,"%.2f Wh",device_nominal_cap);
                sprintf(device_current_cap_str,"%.2f Wh",device_current_cap);
                sprintf(device_charge_cycles_str,"%d cycles", device_charge_cycles);
                sprintf(device_health_str, "%d%%",device_health);

                strcpy(device_name,device_vendor);
                strcat(device_name," ");
                strcat(device_name,device_model);
                row = batteryreport_window_new_row (device_name,icon_name);

                switch (device_tech) {
                case UP_DEVICE_TECHNOLOGY_LITHIUM_ION:
                        batterryeport_window_add_to_expander (row, "Technology", "Lithium ion");
                        break;
                case UP_DEVICE_TECHNOLOGY_LITHIUM_POLYMER:
                        batterryeport_window_add_to_expander (row, "Technology", "Lithium Polymer");
                        break;
                default:
                        batterryeport_window_add_to_expander (row,"Technology:", "Unknown");
                        break;
                }
                batterryeport_window_add_to_expander (row, "Design Capacity", device_nominal_cap_str);
                batterryeport_window_add_to_expander (row, "Current Capacity", device_current_cap_str);
                batterryeport_window_add_to_expander (row, "Health", device_health_str);
                if (device_charge_cycles > 0) batterryeport_window_add_to_expander (row, "Charge cycles", device_charge_cycles_str);
                adw_preferences_group_add(self->battery_group,GTK_WIDGET (row));
                break;
        case UP_DEVICE_KIND_LINE_POWER:
                device_name = "AC Adapter";
                row = batteryreport_window_new_row (device_name,icon_name);
                batterryeport_window_add_to_expander (row, "Status", is_online == TRUE ? "Plugged in" : "Unplugged");
                adw_preferences_group_add(self->ac_power_group,GTK_WIDGET (row));
                break;
        default: break;
        }


        //g_print ("Object path:%s\n",up_device_get_object_path (device));
        g_print ("Device:%s\n",up_device_to_text (device));

}

AdwExpanderRow*
batteryreport_window_new_row (char* device_name, const char* icon_name)
{
        AdwExpanderRow *row;
        GtkImage *icon;
        row = ADW_EXPANDER_ROW (adw_expander_row_new ());
        icon = GTK_IMAGE (gtk_image_new_from_icon_name (icon_name));
        adw_preferences_row_set_title (ADW_PREFERENCES_ROW (row),device_name);
        adw_expander_row_add_prefix (row,GTK_WIDGET (icon));
        return row;
}

void
batterryeport_window_add_to_expander(AdwExpanderRow  *row,
                                    char            *property,
                                    char            *value)
{
        AdwActionRow* nested_row;
        nested_row = ADW_ACTION_ROW (adw_action_row_new ());
        adw_preferences_row_set_title (ADW_PREFERENCES_ROW (nested_row), property);
        adw_action_row_set_subtitle (nested_row, value);
        gtk_widget_add_css_class (GTK_WIDGET (nested_row), "property");
        adw_expander_row_add_row (row, GTK_WIDGET (nested_row));
}

static void
batteryreport_window_export_report (GtkButton* button,
                                    gpointer user_data)
{

}



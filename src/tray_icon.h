#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "battery.h"


// Constants
const int ICON_SIZE = 96;

typedef struct {
    GtkStatusIcon *status_icon;
    unsigned char *rgba_data;
} ThreadData;


// Icon 
void convert_argb_to_rgba(const unsigned char *argb, unsigned char *rgba, int width, int height, int stride) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const unsigned char *pixel = argb + y * stride + x * 4;
            unsigned char *new_pixel = rgba + (y * width + x) * 4;

            new_pixel[0] = pixel[2]; // R
            new_pixel[1] = pixel[1]; // G
            new_pixel[2] = pixel[0]; // B
            new_pixel[3] = pixel[3]; // A
        }
    }
}

void draw_font(cairo_t *cr, const int number, const int status) {
    // conver number to string
    char number_str[16];
    snprintf(number_str, sizeof(number_str), "%d", number);

    // Get text dimensions
    cairo_set_font_size(cr, ICON_SIZE * 0.4);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, number_str, &extents);

    // Center text
    int line_width = ICON_SIZE / 12;
    double x = (ICON_SIZE - extents.width) / 2 - extents.x_bearing  - line_width / 2;
    double y = (ICON_SIZE - extents.height) / 2 - extents.y_bearing;

    // Draw text
    if (status == 1) {
        cairo_set_source_rgb(cr, 244 / 255.0, 192 / 255.0, 149 / 255.0);
    } else {
        cairo_set_source_rgb(cr, 1, 1, 1);
    }
    cairo_select_font_face(cr, "Mono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, number_str);
}

void draw_battery(cairo_t *cr, const int number) {

    // draw main rectangle
    int line_width = ICON_SIZE / 12;
    int one_fifth = ICON_SIZE * 0.2;
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, line_width);
    cairo_rectangle(cr, line_width / 2, one_fifth, ICON_SIZE - (2 * line_width), ICON_SIZE * 0.6);
    cairo_set_source_rgb(cr, 19 / 255.0, 64 / 255.0, 116 / 255.0);
    cairo_fill(cr);
    cairo_stroke(cr);


    // draw second rectangle
    cairo_move_to(cr, ICON_SIZE - line_width / 2, ICON_SIZE / 2 - one_fifth / 2);
    cairo_line_to(cr, ICON_SIZE - line_width / 2, ICON_SIZE / 2 + one_fifth / 2);
    cairo_stroke(cr);

    // fill battery
    int max_available = ICON_SIZE - (2 * line_width);
    int start_x = line_width / 2;
    int start_y = one_fifth ;
    int width = number * max_available / 100;
    int height = one_fifth * 3 + 1;

    cairo_rectangle(cr, start_x, start_y, width, height);
    cairo_set_source_rgb(cr, 103 / 255.0, 146 / 255.0, 137 / 255.0);
    cairo_fill(cr);
    cairo_stroke(cr);
}

GdkPixbuf* create_batt_icon(unsigned char *rgba_data, const int percentage, const int status) {
    // Create surface and context
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ICON_SIZE, ICON_SIZE);
    cairo_t *cr = cairo_create(surface);

    // Fill with transparent color
    cairo_set_source_rgba(cr, 0, 0, 0, 0); // Transparent
    cairo_paint(cr);

    // Draw battery
    draw_battery(cr, percentage);

    // Draw text
    draw_font(cr, percentage, status);


    cairo_destroy(cr);

    int stride = cairo_image_surface_get_stride(surface);
    const unsigned char *argb_data = cairo_image_surface_get_data(surface);
    convert_argb_to_rgba(argb_data, rgba_data, ICON_SIZE, ICON_SIZE, stride);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(
        rgba_data,
        GDK_COLORSPACE_RGB,
        TRUE,
        8,
        ICON_SIZE,
        ICON_SIZE,
        ICON_SIZE * 4,
        NULL,
        NULL
    );

    cairo_surface_destroy(surface);
    return pixbuf;
}

gboolean update_icon(gpointer data) {
    ThreadData *thread_data = (ThreadData *)data;

    char* device = "/sys/class/power_supply/BAT1/capacity";
    char* status_device = "/sys/class/power_supply/BAT1/status";
    GdkPixbuf *pixbuf = create_batt_icon(thread_data->rgba_data, get_battery_level(device), get_battery_status(status_device));
    gtk_status_icon_set_from_pixbuf(thread_data->status_icon, pixbuf);

    // save to file
    // gdk_pixbuf_save(pixbuf, "icon.png", "png", NULL, NULL);

    g_object_unref(pixbuf);
    return G_SOURCE_REMOVE;
}

// Thread
void* thread_function(void *data) {
    ThreadData *thread_data = (ThreadData *)data;

    while (1)
    {
        g_idle_add(update_icon, thread_data); // Плануємо оновлення іконки в основному потоці
        sleep(3);
    }
    
    return NULL;
}

int start(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    unsigned char *rgba_data = (unsigned char *)malloc(ICON_SIZE * ICON_SIZE * 4);
    GtkStatusIcon *status_icon = gtk_status_icon_new();
    gtk_status_icon_set_tooltip_text(status_icon, "Updating icon...");
    gtk_status_icon_set_visible(status_icon, TRUE);

    ThreadData thread_data = {status_icon, rgba_data};

    pthread_t thread;
    if (pthread_create(&thread, NULL, thread_function, &thread_data) != 0) {
        perror("Failed to create thread");
        return 1;
    }

    gtk_main();

    pthread_join(thread, NULL);
    free(rgba_data);
    return 0;
}

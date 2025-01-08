#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


const int ICON_SIZE = 64;

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

// Функція для створення Pixbuf із числом
GdkPixbuf* create_pixbuf_with_number(unsigned char *rgba_data, char *text) {
    // Create surface and context
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ICON_SIZE, ICON_SIZE);
    cairo_t *cr = cairo_create(surface);

    // Fill with transparent color
    cairo_set_source_rgba(cr, 0, 0, 0, 0); // Transparent
    cairo_paint(cr);

    // Text color
    cairo_set_source_rgb(cr, 0, 0, 0);

    // Font config
    cairo_select_font_face(cr, "Mono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 32);

    // Отримання розмірів тексту
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);

    // Розрахунок координат для центрування тексту
    double x = (ICON_SIZE - extents.width) / 2 - extents.x_bearing;
    double y = (ICON_SIZE - extents.height) / 2 - extents.y_bearing;

    // Малювання числа
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text);

    // Завершення малювання
    cairo_destroy(cr);

    int stride = cairo_image_surface_get_stride(surface);
    const unsigned char *argb_data = cairo_image_surface_get_data(surface);
    convert_argb_to_rgba(argb_data, rgba_data, ICON_SIZE, ICON_SIZE, stride);

    // Створення GdkPixbuf
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(
        rgba_data,
        GDK_COLORSPACE_RGB,
        TRUE,
        8,
        ICON_SIZE,
        ICON_SIZE,
        ICON_SIZE * 4,
        (GdkPixbufDestroyNotify)free,
        NULL
    );

    // Очищення ресурсу surface
    cairo_surface_destroy(surface);
    return pixbuf;
}

// // Callback для обробки кліка на іконку
// void on_status_icon_activate(GtkStatusIcon *status_icon, gpointer user_data) {
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Пусте вікно");
//     gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

//     // Закриття вікна
//     g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);
//     gtk_widget_show_all(window);
// }

int main(int argc, char *argv[]) {
    unsigned char *rgba_data = malloc(ICON_SIZE * ICON_SIZE * 4);


    gtk_init(&argc, &argv);

    char *text = "12";

    // Створення іконки у системному треї
    GtkStatusIcon *status_icon = gtk_status_icon_new_from_pixbuf(create_pixbuf_with_number(rgba_data, text));
    gtk_status_icon_set_tooltip_text(status_icon, "Натисніть для відкриття вікна");
    gtk_status_icon_set_visible(status_icon, TRUE);

    sleep(1);
    // // Підключення сигналу активації (клік на іконку)
    // g_signal_connect(status_icon, "activate", G_CALLBACK(on_status_icon_activate), NULL);
    gtk_main();

    sleep(3);

    char *text2 = "34";
    gtk_status_icon_set_from_pixbuf(status_icon, create_pixbuf_with_number(rgba_data, text2));

    free(rgba_data);
    return 0;
}

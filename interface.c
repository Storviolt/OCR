#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Image_Process/black_and_white.h"
#include "Image_Process/grayscale.h"
#include "Image_Process/preprocess.h"
#include "Image_Process/rotate_manually.h"
#include "Image_Process/rotate_automatically.h"

GtkWindow *main_window;
GtkBuilder *builder;
SDL_Surface *current_surface = NULL;

void update_image_display(GtkImage *image_display) {
    const char *temp_file = "/tmp/current_image.png";
    IMG_SavePNG(current_surface, temp_file);
    gtk_image_set_from_file(image_display, temp_file);
}

void on_load_image_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    GtkImage *image_display = GTK_IMAGE(user_data);

    dialog = gtk_file_chooser_dialog_new("Open Image",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *file_path = \
        gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (current_surface) SDL_FreeSurface(current_surface);

        current_surface = IMG_Load(file_path);
        if (current_surface) {
            update_image_display(image_display);
        }

        g_free(file_path);
    }

    gtk_widget_destroy(dialog);
}

void on_grayscale_image_clicked(GtkButton *button, gpointer user_data) {
    GtkImage *image_display = GTK_IMAGE(user_data);
    if (current_surface) {
        apply_grayscale(current_surface);
        update_image_display(image_display);
    }
}

void on_rotate_image_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *angle_entry = GTK_WIDGET(user_data);//Get the angle entry widget
    GtkImage *image_display = \
    GTK_IMAGE(gtk_builder_get_object(builder, "ImageDisplay"));

    if (current_surface) {
        const char *angle_text = gtk_entry_get_text(GTK_ENTRY(angle_entry));
        if (angle_text && *angle_text) {
            double angle = atof(angle_text); // Convert string to double
            apply_rotation(&current_surface, angle);
            update_image_display(image_display);
        } else {
            g_print("Invalid angle input\n");
        }
    }
}

void on_bw_image_clicked(GtkButton *button, gpointer user_data) {
    GtkImage *image_display = GTK_IMAGE(user_data);
    if (current_surface) {
        apply_black_and_white(current_surface);
        update_image_display(image_display);
    }
}
void on_save_image_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    GtkImage *image_display = GTK_IMAGE(user_data);
    
    // Open a file chooser dialog
    dialog = gtk_file_chooser_dialog_new("Save Image",
                                         GTK_WINDOW(main_window),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL);

    // Set default file name
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "image.png");

    // Show the dialog and wait for a response
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *file_path =\
        gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        // Save the image to the selected file path
        if (current_surface) {
            IMG_SavePNG(current_surface, file_path);
        }

        // Free the file path string
        g_free(file_path);
    }

    // Destroy the dialog after use
    gtk_widget_destroy(dialog);
}
void on_preprocess_image_clicked(GtkButton *button, gpointer user_data) {
    GtkImage *image_display = GTK_IMAGE(user_data);
    if (current_surface) {
        apply_preprocess(current_surface);
        update_image_display(image_display);
    }
}
void on_rotate_automatically_clicked(GtkButton *button, gpointer user_data) {
    GtkImage *image_display = GTK_IMAGE(user_data);
    if (current_surface) {
        apply_rotate_automatically(&current_surface);
        update_image_display(image_display);
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *load_button;
    GtkWidget *grayscale_button;
    GtkWidget *bw_button;
    GtkWidget *preprocess_button;
    GtkWidget *rotate_button;
    GtkWidget *rotate_auto_button;
    GtkWidget *rotate_angle_entry;
    GtkWidget *save_button;
    GtkWidget *image_display;
    gtk_init(&argc, &argv);

    // Initialize global builder
    builder = gtk_builder_new_from_file("interface.glade"); 

    main_window = GTK_WINDOW(gtk_builder_get_object(builder, "MainWindow"));
    load_button = GTK_WIDGET(gtk_builder_get_object(builder, "Load_image"));
    grayscale_button = \
    GTK_WIDGET(gtk_builder_get_object(builder, "Grayscale_image"));
    bw_button = GTK_WIDGET(gtk_builder_get_object(builder, "BW_image"));
    preprocess_button = \
    GTK_WIDGET(gtk_builder_get_object(builder, "Preprocess_image"));
    rotate_button = GTK_WIDGET(gtk_builder_get_object(builder,"Rotate_image"));
    rotate_auto_button = \
    GTK_WIDGET(gtk_builder_get_object(builder, "Rotate_auto_image"));
    rotate_angle_entry =\
    GTK_WIDGET(gtk_builder_get_object(builder, "RotateAngleEntry"));
    save_button = GTK_WIDGET(gtk_builder_get_object(builder, "Save_image"));
    image_display =GTK_WIDGET(gtk_builder_get_object(builder, "ImageDisplay"));

    gtk_window_set_title(main_window, "Word.ia");
    g_signal_connect(save_button, "clicked", \
    G_CALLBACK(on_save_image_clicked), image_display);
    g_signal_connect(load_button, "clicked", \
    G_CALLBACK(on_load_image_clicked), image_display);
    g_signal_connect(rotate_auto_button, "clicked", \
    G_CALLBACK(on_rotate_automatically_clicked), image_display);
    g_signal_connect(grayscale_button, "clicked", \
    G_CALLBACK(on_grayscale_image_clicked), image_display);
    g_signal_connect(bw_button, "clicked", \
    G_CALLBACK(on_bw_image_clicked), image_display);
    g_signal_connect(preprocess_button, "clicked", \
    G_CALLBACK(on_preprocess_image_clicked), image_display);
    g_signal_connect(rotate_button, "clicked", \
    G_CALLBACK(on_rotate_image_clicked), rotate_angle_entry);
    g_signal_connect(main_window, "destroy", \
    G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(GTK_WIDGET(main_window));
    gtk_main();

    if (current_surface) SDL_FreeSurface(current_surface);
    SDL_Quit();
    return 0;
}
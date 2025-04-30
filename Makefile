CC = gcc
CFLAGS = -g `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image`
LDFLAGS = `pkg-config --libs gtk+-3.0 sdl2 SDL2_image` -lm

# Targets
.PHONY: all clean

all: Word_ia program save_word get_coordinates paint_result detect_pos

Word_ia: interface.c
	$(CC) $(CFLAGS) -o Word_ia interface.c \
		Image_Process/black_and_white.c \
		Image_Process/preprocess.c \
		Image_Process/rotate_manually.c \
		Image_Process/rotate_automatically.c \
		Image_Process/grayscale.c \
		$(LDFLAGS)

detect_pos: detect_pos.c load_save.c painter.c
	$(CC) $(CFLAGS) -o detect_pos detect_pos.c load_save.c painter.c $(LDFLAGS)

save_word: save_word.c
	$(CC) $(CFLAGS) -o save_word save_word.c $(LDFLAGS)

paint_result: paint_result.c load_save.c save_letter.c painter.c
	$(CC) $(CFLAGS) -o paint_result paint_result.c load_save.c save_letter.c painter.c $(LDFLAGS)

get_coordinates: solver.c coordinates.c
	$(CC) $(CFLAGS) -o get_coordinates solver.c coordinates.c $(LDFLAGS)

program: lots.c
	$(CC) $(CFLAGS) -o program lots.c $(LDFLAGS)


clean:
	rm -rf Word_ia neurons save_word get_coordinates paint_result detect_pos \
		coordinates.txt output_binary.png output_grid.png \
		output_word_list.png final_output.png LETTER program


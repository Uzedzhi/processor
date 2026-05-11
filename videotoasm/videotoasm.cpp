#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "sassert.h"
#include "../helpers/helpers.h"
#include "assembly.h"
#include "error_manage.h"
#include "videotoasm.h"
#include "better_output.h"

struct pixel_t
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

error_t error = {};

void read_ppm(pixel_t *pixels, pixel_t *pixels_prev, const char *file_name, int num_of_pixels)
{
    sassert(file_name, ERR_PTR_NULL);
    sassert(pixels, ERR_PTR_NULL);
    sassert(pixels_prev, ERR_PTR_NULL);

    FILE *fp = fopen(file_name, "rb");
    sassert(fp, ERR_PTR_NULL);

    char magic[10] = {};
    int width = 0, height = 0, max_val = 0;
    fscanf(fp, "%2s", magic);
    fscanf(fp, "%d %d", &width, &height);
    fscanf(fp, "%d", &max_val);
    fgetc(fp);

    size_t read_count = fread(pixels, sizeof(pixel_t), num_of_pixels, fp);
    fclose(fp);
}

int check_existing_frames(const char *dirname)
{
    sassert(dirname, ERR_PTR_NULL);

    DIR *dir = opendir(dirname);
    if (!dir)
        return 0;

    dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL)
        count++;
    closedir(dir);
    return count;
}

bool not_changed_from_last_time(pixel_t pixel, pixel_t pixel_prev)
{
    return pixel.r == pixel_prev.r && pixel.g == pixel_prev.g && pixel.b == pixel_prev.b;
}

bool is_same_pixels(pixel_t pixel1, pixel_t pixel2)
{
    return pixel1.r == pixel2.r && pixel1.g == pixel2.g && pixel1.b == pixel2.b;
}

const char ascii_bright[] = " .'`^\",:;Il!i<>~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const size_t ascii_bright_len = sizeof(ascii_bright) - 1;
const size_t RAMP_LEN = sizeof(ascii_bright) - 1;

unsigned char rgb_to_ansi_color(pixel_t pixel)
{
    int r = (pixel.r > 128) ? 1 : 0;
    int g = (pixel.g > 128) ? 1 : 0;
    int b = (pixel.b > 128) ? 1 : 0;
    int bright = ((pixel.r + pixel.g + pixel.b) / 3 > 128) ? 8 : 0;
    return bright + (r * 4) + (g * 2) + b;
}

unsigned char rgb_to_brightness(pixel_t pixel)
{
    return (unsigned char)(0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b);
}

#define place_trailing_pixels_func(n)        \
    {                                        \
        fprintf(fp, "\tPUSH %d\n"            \
                    "\tCALL :inc_rax\n",     \
                n);                          \
        count_trailing_unchanged_pixels = 0; \
    }

error_t compile_frame(FILE *fp, pixel_t *pixels, pixel_t *pixels_prev, char *frame, int num_of_frame, int num_of_pixels)
{
    sassert(frame, ERR_PTR_NULL);
    sassert(fp, ERR_PTR_NULL);
    sassert(pixels, ERR_PTR_NULL);
    read_ppm(pixels, pixels_prev, frame, num_of_pixels);

    fprintf(fp, ":draw_%d {\n", num_of_frame);
    size_t count_trailing_unchanged_pixels = 0;
    size_t count_same_pixels_in_a_row = 1;
    unsigned long prev_color = pixels[0].r + pixels[0].g * 256 + pixels[0].b * 256 * 256;
    for (int i = 0; i < num_of_pixels; i++)
    {
        if (count_same_pixels_in_a_row == 1 && not_changed_from_last_time(pixels[i], pixels_prev[i]))
        {
            count_trailing_unchanged_pixels++;
            continue;
        }
        else if (count_same_pixels_in_a_row == 1)
        {
            place_trailing_pixels_func(count_trailing_unchanged_pixels);
        }

        int brightness = rgb_to_brightness(pixels[i]);
        unsigned long color = pixels[i].r + pixels[i].g * 256 + pixels[i].b * 256 * 256;

        if (prev_color == color)
        {
            count_same_pixels_in_a_row++;
        }
        else
        {
            if (count_same_pixels_in_a_row > 1)
            {
                fprintf(fp, "\tPUSH %ld\n"
                            "\tPUSH %ld\n"
                            "\tCALL :draw_in_a_row\n",
                        prev_color, count_same_pixels_in_a_row);
            }
            else
            {
                fprintf(fp, "\tPUSH %ld\n"
                            "\tCALL :draw_and_inc\n",
                        color);
            }
            count_same_pixels_in_a_row = 1;
        }
        prev_color = color;
        count_trailing_unchanged_pixels = 0;
    }
    fprintf(fp, "\tDRAW 1\n\tRET\n}\n");
    memcpy(pixels_prev, pixels, num_of_pixels);
    return error;
}

error_t compile_header_stuff(FILE *fp, int num_of_frames, int width, int height, char *audio_name, bool has_audio, int fps)
{
    sassert(fp, ERR_PTR_NULL);
    sassert(audio_name, ERR_PTR_NULL);

    fprintf(fp, "; assembly for video by dimooooon\n"
                "; width = %d, height = %d, fps = %d, has audio = %d\n",
            width, height, fps, has_audio);
    if (has_audio)
        fprintf(fp, "; audio file name = %s\n", audio_name);
    fprintf(fp, "\n; main {\n"
                "\tPUSH 0\n"
                "\tPOPR RAX\n\n");
    for (int i = 1; i < num_of_frames; i++)
    {
        fprintf(fp, "\tCALL :draw_%d\n"
                    "\tPUSH 0\n"
                    "\tPOPR RAX\n"
                    "\t\n\n",
                i);
    }

    fprintf(fp, "\tHLT\n}\n"
                ":inc_rax {\n"
                "\tPUSHR RAX\n"
                "\tADD\n"
                "\tPOPR RAX\n"
                "\tRET\n"
                "}\n"
                ":inc_rbx_by_1 {\n"
                "\tPUSHR RBX\n"
                "\tPUSH 1\n"
                "\tADD\n"
                "\tPOPR RBX\n"
                "\tRET\n"
                "}\n"
                ":draw_in_a_row {\n"
                "\tPOPR RCX\n"
                "\tPOPR DED\n"
                "\t:cycle_start\n"
                "\tPUSHR DED\n"
                "\tPOPM [RAX]\n"
                "\tPUSH 1\n"
                "\tCALL :inc_rax\n"
                "\tCALL :inc_rbx_by_1\n"
                "\tPUSHR RBX\n"
                "\tPUSHR RCX\n"
                "\tJB :cycle_start\n"
                "\tPUSH 0\n"
                "\tPOPR RBX\n"
                "\tRET\n"
                "}\n"
                ":draw_and_inc {\n"
                "\tPOPM [RAX]\n"
                "\tPUSH 1\n"
                "\tCALL :inc_rax\n"
                "\tRET\n"
                "}\n");
    return error;
}

error_t compile_video_file(char *input_file, char *output_file, int width, int height, char *start, char *end, int fps)
{
    sassert(input_file, ERR_PTR_NULL);
    sassert(output_file, ERR_PTR_NULL);

    if (error.is_error == true)
        return error;
    if (!is_file_exists(input_file))
    {
        add_error(ERR_FILE_DOES_NOT_EXIST, "%s", input_file);
        return error;
    }

    char frames_path[MAX_STR_SIZE] = {};
    char dirname[MAX_STR_SIZE] = {};
    char audio_file[MAX_STR_SIZE] = {};
    bool has_audio = true;

    strncpy(dirname, input_file, strchr(input_file, '.') - input_file);
    strncpy(audio_file, dirname, strlen(dirname));
    snprintf(frames_path, MAX_STR_SIZE, "%s/frame0001.ppm", dirname);

    pixel_t *pixels = (pixel_t *)calloc(width * height, sizeof(pixel_t));
    sassert(pixels, ERR_PTR_NULL);

    pixel_t *pixels_prev = (pixel_t *)calloc(width * height, sizeof(pixel_t));
    sassert(pixels_prev, ERR_PTR_NULL);

    FILE *fp_compile = fopen(output_file, "w");
    sassert(fp_compile, ERR_PTR_NULL);

    char command[MAX_STR_SIZE] = {};
    DIR *dir = opendir(dirname);

    BEGIN
    if (!dir)
    {
        snprintf(command, MAX_STR_SIZE - 1, "mkdir %s", dirname);
        if (system(command) != 0)
        {
            add_error(ERR_SYSTEM_FAILED, "%s", command);
            break;
        }
    }
    closedir(dir);

    printf("making .wav file\n");
    snprintf(command, MAX_STR_SIZE - 1, "ffmpeg -i \"%s\" -vn \"%s.wav\"", input_file, dirname);
    if (system(command) != 0)
    {
        printf("no audio file detected, starting without it\n");
        has_audio = false;
    }
    else
        printf(GREEN "successfully made audio file\n" WHITE);
    snprintf(command, MAX_STR_SIZE - 1, "ffmpeg -i \"%s\" -ss \"%s\" -to \"%s\"  -vf \"scale=%d:%d,fps=%d\" -pix_fmt rgb24 \"%s/frame%%04d.ppm\"", input_file, start, end, width, height, fps, dirname);
    printf("dividing video into frames\n");
    if (system(command) != 0)
    {
        add_error(ERR_SYSTEM_FAILED, "%s", command);
        break;
    }
    printf(GREEN "successfully divided\n" WHITE);

    size_t num_of_frames = check_existing_frames(dirname);
    if (num_of_frames == 0)
    {
        add_error(ERR_VIDEO_DIVIDE_FAIL, "none");
        break;
    }

    int count = 1;
    char next_frame[MAX_STR_SIZE] = {};
    compile_header_stuff(fp_compile, num_of_frames - 1, width, height, audio_file, has_audio, fps);

    while (count < num_of_frames - 1)
    {
        snprintf(next_frame, MAX_STR_SIZE - 1, "%s/frame%04d.ppm", dirname, count);
        compile_frame(fp_compile, pixels, pixels_prev, next_frame, count, width * height);

        count++;
        printf("%d %d <%s>\n", count, num_of_frames, next_frame);
    }
    END

        fclose(fp_compile);
    free(pixels);
    return error;
}

void print_help()
{
    printf(MAGENTA "type 1) video you want to compile\n"
                   "    2) file where you want to compile it\n"
                   "    3) width in pixels\n"
                   "    4) height in pixels\n"
                   "    5) from where begin cut\n"
                   "    6) when end cut\n"
                   "    7) fps\n" WHITE);
}

int main(int argc, char *argv[])
{
    if (argc != 8)
    {
        print_help();
        return 0;
    }
    if (atoi(argv[3]) == 0 || atoi(argv[4]) == 0 || atoi(argv[7]) == 0)
    {
        printf("please enter correct width and height");
        return 0;
    }

    compile_video_file(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6], atoi(argv[7]));
    if (error.is_error == true)
    {
        print_error(error_text);
        return error.code;
    }
    else
        printf(GREEN "done compiling video\n" WHITE);
    return 0;
}
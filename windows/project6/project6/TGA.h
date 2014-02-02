#ifndef TGA_HPP
#define TGA_HPP

#pragma pack(push, 1)

struct tga_head
{
    unsigned char  id_length;
    unsigned char  color_map_type;
    unsigned char  image_type;
    unsigned short color_map_offset;
    unsigned short color_map_length;
    unsigned char  color_map_size;
    unsigned short image_x_origin;
    unsigned short image_y_origin;
    unsigned short image_width;
    unsigned short image_height;
    unsigned char  image_depth;
    unsigned char  image_descriptor;
};

#pragma pack(pop)

inline int save_tga(const char *filename, int w, int h, int d, void *p)
{
    tga_head head = { 0, 0, 2, 0, 0, 0, 0, 0, w, h, d, (d == 32 ? 8 : 0) };

    if (d == 24 || d == 32)
    {
        if (FILE *stream = fopen(filename, "wb"))
        {
            if (fwrite(&head, sizeof (tga_head), 1, stream) == 1)
            {
                if (fwrite(p, d / 8, w * h, stream) == w * h)
                {
                    fclose(stream);
                    return 0;
                }
            }
            fclose(stream);
        }
    }
    return -1;
}

inline void *load_tga(const char *filename, int& w, int& h, int& d)
{
    tga_head head;

    if (FILE *stream = fopen(filename, "rb"))
    {
        if (fread(&head, sizeof (tga_head), 1, stream) == 1)
        {
            if (head.image_type == 2)
            {
                w = int(head.image_width);
                h = int(head.image_height);
                d = int(head.image_depth);

                if (fseek(stream, head.id_length, SEEK_CUR) == 0)
                {
                    if (void *p = calloc(w * h, d / 8))
                    {
                        if (fread(p, d / 8, w * h, stream) == w * h)
                        {
                            fclose(stream);
                            return p;
                        }
                    }
                }
            }
        }
        fclose(stream);
    }
    return 0;
}

#endif
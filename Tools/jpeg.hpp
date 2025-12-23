#pragma once
#include <cstdio>
#include <jpeglib.h>
#include <cstdlib>

inline bool readJPEG(const char* filename, unsigned char*& img, int& width, int& height) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // Ouvrir le fichier JPEG
    FILE* infile;
    #if defined(_WIN32)
        errno_t err = fopen_s(&infile, filename, "rb");
        if (err != 0 || infile == NULL) {
            printf("Erreur d'ouverture du fichier JPEG %s\n",filename);
            return false;
        }
    #elif defined(__APPLE__)
        infile = fopen(filename, "rb");
        if (infile == NULL) {
            printf("Erreur d'ouverture du fichier JPEG: %s\n",filename);
            return false;
        }
    #endif
    
    // Initialiser le décodeur JPEG
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);

    // Lire l'en-tête du fichier JPEG
    (void)jpeg_read_header(&cinfo, TRUE);

    cinfo.out_color_space = JCS_RGB;

    // Commencer la décompression
    (void)jpeg_start_decompress(&cinfo);

    // Allouer la mémoire pour stocker une ligne de pixels
    JSAMPARRAY buffer;
    buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo, JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1);

    // Lire chaque ligne de l'image et la stocker dans un tableau unidimensionnel
    img = new unsigned char[cinfo.output_width * cinfo.output_height * cinfo.output_components];
    
    while (cinfo.output_scanline < cinfo.output_height) {
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);
        
        // Calculer l'index de la ligne (inversée ou non)
      /*  int row = flipVertical ? 
            (cinfo.output_height - cinfo.output_scanline) : 
            (cinfo.output_scanline - 1);*/

        int row =  (cinfo.output_height - cinfo.output_scanline);
        
        for (JDIMENSION i = 0; i < cinfo.output_width * cinfo.output_components; i++) {
            img[row * cinfo.output_width * cinfo.output_components + i] = buffer[0][i];
        }
    }

    // Stocker la largeur et la hauteur de l'image
    width = cinfo.output_width;
    height = cinfo.output_height;

    // Terminer la décompression et libérer les ressources
    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    // Retourner true pour indiquer que l'opération a réussi
    return true;
}

void init_all1();
void precalculate_YCbCr_tables1();
void exitmessage2(char *error_message);
void load_bitmap1(char *bitmap_name, WORD *Ximage_original, WORD *Yimage_original);
void load_bitmap2(char *bitmap_name, WORD *Ximage_original, WORD *Yimage_original);
void load_data_units_from_RGB_buffer_cmp(WORD xpos,WORD ypos);
double main_compare();
double comparebmp(char *BmpFile, char *BmpFile_ref);

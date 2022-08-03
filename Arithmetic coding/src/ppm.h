#pragma once

typedef unsigned long long ull;
enum {
    NUMBER_OF_ELEMENTS = 256,
    BIT_SIZE = 4,
    CODE_SIZE = 8,
    N = 1
};
typedef struct ContextModel{
    int esc;
    ull Total_Freq;
    ull count[NUMBER_OF_ELEMENTS];
    //struct ContextModel* next[256];
}ContextModel;
ContextModel cm[NUMBER_OF_ELEMENTS + 1];  //struct ContextModel* next[256];
ContextModel *stack[N + 1];
int StackPointer;
int context [N];

void compress_ppm(char *ifile, char *ofile);
void decompress_ppm(char *ifile, char *ofile);
void initialization (void);
void new_scale(ContextModel *CM);
void update(int symbol);
void write_bit(FILE * ofp, int bit, int check);
int read_bit_ppm(FILE *filein);
void bits_plus_follow_ppm(FILE * ofp, int bit, int *bits_to_follow);
void arithmetic_encode(FILE *ofp, ull borders, ull range, ull divider, int *bits_to_follow, ull *low, ull *high);
int encode_symbol(FILE *ofp, ContextModel *CM, int c, int *bits_to_follow, ull *low, ull *high);
void arithmetic_finish(FILE *ofp, ull low, int bits_to_follow);
void arithmetic_decode(FILE *ifp, ull borders, ull range, ull divider, ull *low, ull *high, ull *value);
int decode_symbol(FILE *ifp, ContextModel *CM, int *c, ull *value);
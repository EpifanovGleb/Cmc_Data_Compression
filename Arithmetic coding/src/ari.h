#pragma once
typedef unsigned long long ull;
enum {
    NUM_OF_SYMBOLS = 257,       // Alphabet`s power (256 symbols + EOF)
    SYMBOL_EOF = 256,           // Code for EOF
    BUF_SIZE = 8192,
    NUMBER_OF_BITS = 32,
    SIZE = 2,
    AGGRESSION_1 = 5,
    AGGRESSION_2 = 50
};
void compress_ari(char *ifile, char *ofile);
void decompress_ari(char *ifile, char *ofile);
void write_bit(FILE *ofp, int bit, int check);
void bits_plus_follow(FILE *ofp, int bit, int *bits_to_follow_1);
int read_bit (FILE *filein);
void update_table (ull borders[SIZE][NUM_OF_SYMBOLS + 1], ull frequency[SIZE][NUM_OF_SYMBOLS + 1], int my_ch, int const agr[]);



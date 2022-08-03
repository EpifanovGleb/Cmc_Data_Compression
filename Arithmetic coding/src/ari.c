#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <memory.h>
#include <sys/types.h>
#include "ari.h"

const double Eps = 0.000001;
ull MAX_VALUE = 0xffffffffffffffffU;
ull MAX_FREQUENCY = 0xffffffffU;
enum {
    DIVIDER = 2,
};
double my_sqrt(double value) {
    if (value <= 0) return 0;
    double left = 1, right = value, middle, approx;
    do {
        middle = left + (right-left)/DIVIDER;
        approx = middle*middle;
        if (approx > value)
            right = middle;
        else
            left = middle;
    } while(abs(approx - value) > Eps);

    return middle;
}
double func(ull frequency[SIZE][NUM_OF_SYMBOLS + 1], int a) {
    ull freq[NUM_OF_SYMBOLS + 1];
    for(int k = 0;k <= NUM_OF_SYMBOLS; k++) {
        freq[k] = frequency[a][k];
    }
    int i;
    double sum = 0;

    for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
        sum += (double)((double)freq[i])*((double)freq[i]);
    }
    sum = my_sqrt(sum);
    return sum;
}

void update_table (ull borders[SIZE][NUM_OF_SYMBOLS + 1], ull frequency[SIZE][NUM_OF_SYMBOLS + 1],
                    int my_ch, int const agr[]) {
    int i, k, j;
    for (j = 0; j < SIZE; j++) {
        if (borders[j][NUM_OF_SYMBOLS] + agr[j] >= MAX_FREQUENCY) {
            ull temp = 0;
            bool_t = 1;
            for (i = 0; i <= NUM_OF_SYMBOLS; i++) {
                if (frequency[j][i] > 1) {
                    temp = frequency[j][i] & 1;
                    frequency[j][i] = frequency[j][i]/2;
                    temp += frequency[j][i];
                    for (k = i + 1; k <= NUM_OF_SYMBOLS; k++) {
                        borders[j][k] -= temp;
                    }
                }
            }
        }
        for (k = my_ch + 1; k <= NUM_OF_SYMBOLS; k++) {
            borders[j][k] += agr[j];
        }
        frequency[j][my_ch] += agr[j];

    }
}
void write_bit_1(FILE *ofp, int bit, int check) {
    static unsigned char temp = 0;
    static int count = 8;
    count--;
    temp <<= 1;
    temp |= bit;
    if (check) {
        while (count) {
            temp <<= 1;
            count--;
        }
        putc(temp, ofp);
        return;
    }
    if (!count) {
        putc(temp, ofp);
        count = 8;
    }
}
void bits_plus_follow(FILE *ofp, int bit, int *bits_to_follow_1) {
    write_bit_1(ofp, bit, 0);
    for (; *bits_to_follow_1 > 0; (*bits_to_follow_1)--) {
        write_bit_1(ofp, !bit, 0);
    }
}

int read_bit (FILE *filein) {
    static int i = 8;
    static unsigned int temp = 0;
    if (i == 8) {
        if ((temp = getc(filein)) == EOF) {
            return 0;
        };
        i = 0;
    }
    i++;
    int bit = (int) (temp & 0x80);
    temp <<= 1;
    return (bit >> 7);
}

void compress_ari(char *ifile, char *ofile) {
    FILE *ifp = (FILE *)fopen(ifile, "rb");
    FILE *ofp = (FILE *)fopen(ofile, "wb");

    fseek(ifp,0,SEEK_END);
    long size = ftell(ifp);
    if (size == 0) {
        fclose(ifp);
        fclose(ofp);
        return;
    }
    fseek(ifp,0,SEEK_SET);

    int agr[SIZE]; int array[SIZE]={0};
    agr[0] = AGGRESSION_1; agr[1] = AGGRESSION_2;
    ull borders[SIZE][NUM_OF_SYMBOLS + 1];        // array of right borders for each symbol
    ull frequency[SIZE][NUM_OF_SYMBOLS + 1];      // array of frequencies for each symbol
    ull temp_arr[SIZE][NUM_OF_SYMBOLS + 1] = {0};
    ull temp_arr_sqr[SIZE][NUM_OF_SYMBOLS + 1] = {0};

    ull First_qtr = (MAX_VALUE / 4 + 1); ull Half = 2 * First_qtr; ull Third_qtr = 3 * First_qtr;
    ull high = MAX_VALUE, low = 0;
    double result = 0, previous = 0;
    size_t n;
    int i, j, bits_to_follow = 0, my_ch, ind = 0, counter = 0, check = 1;
    unsigned char buff[BUF_SIZE];

    for (i = 0; i < SIZE; i++) {
        for(j = 0; j <= NUM_OF_SYMBOLS; j++) {
            frequency[i][j] = 1;
            borders[i][j] = j;
        }
    }
    for(j = 0; j < SIZE; j++) {
        frequency[j][0] = 0;
    }
    while (1) {
        n = fread(buff, 1, sizeof(buff), ifp);
        if (n) {
            for (j = 0; j <= n; j++) {
                if (feof(ifp) && j == n) my_ch = SYMBOL_EOF;
                else if (j == n) break;
                else my_ch = buff[j];

                ull temporary = low, current = 0, tmp = 0;
                low = temporary + borders[ind][my_ch] * (high - temporary + 1) / borders[ind][NUM_OF_SYMBOLS];
                high = temporary + borders[ind][my_ch+ 1] * (high - temporary + 1) / borders[ind][NUM_OF_SYMBOLS] - 1;

                for (;;) {
                    if (high < Half) {
                        bits_plus_follow(ofp, 0, &bits_to_follow);
                    } else if (low >= Half) {
                        bits_plus_follow(ofp, 1, &bits_to_follow);
                        low -= Half;
                        high -= Half;
                    } else if (low >= First_qtr && high < Third_qtr) {
                        bits_to_follow++;
                        low -= First_qtr;
                        high -= First_qtr;
                    } else break;
                    low += low;
                    high += high + 1;
                }
                counter++;
                if (my_ch == SYMBOL_EOF) break;
                update_table(borders, frequency, my_ch, agr);


                double summary_1 = 0, summary_2 = 0;
                for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
                    summary_1 += (double) frequency[0][i];
                    summary_2 += (double) frequency[1][i];
                }
                summary_1 /= (double) NUM_OF_SYMBOLS;
                summary_2 /= (double) NUM_OF_SYMBOLS;
                for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
                    temp_arr[0][i] = frequency[0][i] - summary_1;
                    temp_arr[1][i] = frequency[1][i] - summary_2;
                }

                for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
                    summary_1 += (double)temp_arr[0][i]*(double)temp_arr[0][i];
                    summary_2 += (double)temp_arr[1][i]*(double)temp_arr[1][i];
                }
                summary_1 /= (double) NUM_OF_SYMBOLS;
                summary_2 /= (double) NUM_OF_SYMBOLS;
                summary_1 = my_sqrt(summary_1);
                summary_2 = my_sqrt(summary_2);
                for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
                    temp_arr[0][i] = temp_arr[0][i] / summary_1;
                    temp_arr[1][i] = temp_arr[1][i] / summary_2;
                }
                for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
                    temp_arr[0][i] -= temp_arr[1][i];
                }
                result = func(temp_arr, 0);
                if (counter > 100 && check > 0) {
                    if (result > previous) {
                        ind = 1;
                        counter = 50;
                        check = -10;

                    } else {
                        ind = 0;
                        counter = 0;
                    }
                }
                if (check > 0) {
                    if (previous == 0) {
                        previous = result;
                    }
                    previous = (result + previous) / 2;
                }

            }
        }
        else break;
    }
    bits_to_follow++;                                         // finish encode
    if (low < First_qtr) {
        bits_plus_follow(ofp, 0, &bits_to_follow);
    } else {
        bits_plus_follow(ofp, 1, &bits_to_follow);
    }
    write_bit_1(ofp, 0, 1);
    fclose(ifp);
    fclose(ofp);
}

void decompress_ari(char *ifile, char *ofile) {
    FILE *ifp = (FILE *) fopen(ifile, "rb");
    FILE *ofp = (FILE *) fopen(ofile, "wb");

    fseek(ifp,0,SEEK_END);                              // check for empty file
    long size = ftell(ifp); ;
    if (size == 0) {
        fclose(ifp);
        fclose(ofp);
        return;
    }
    fseek(ifp,0,SEEK_SET);

    int agr[SIZE];
    int array[SIZE] = {0};
    int i, j, index = 0, ind = 0, counter = 0, check = 1;
    agr[0] = AGGRESSION_1; agr[1] = AGGRESSION_2;
    ull borders[SIZE][NUM_OF_SYMBOLS + 1];                   // array of right borders for each symbol
    ull frequency[SIZE][NUM_OF_SYMBOLS + 1];
    ull temp_arr[SIZE][NUM_OF_SYMBOLS + 1];
    for (i = 0; i < SIZE; i++) {
        for(j = 0; j <= NUM_OF_SYMBOLS; j++) {
            temp_arr[i][j] = 0;
        }
    }
    for(i = 0; i < SIZE; i++) {
        for(j = 0; j <= NUM_OF_SYMBOLS; j++) {
            frequency[i][j] = 1;
            borders[i][j] = j;
        }
    }
    for(j = 0; j < SIZE; j++) {
        frequency[j][0] = 0;
    }
    double result = 0, previous = 0;
    ull First_qtr = (MAX_VALUE + 1) / 4; ull Half = 2 * First_qtr;
    ull Third_qtr = 3 * First_qtr;
    ull high = MAX_VALUE, low = 0, value = 0, code;             //Start position

    for (i = 1; i < NUMBER_OF_BITS + 1; i++) {
        value = 2 * value + read_bit(ifp);                      // reading NUMBER_OF_BITS bits from the file, where

    }
    while (1) {
        code = ((value - low + 1) * borders[ind][NUM_OF_SYMBOLS] - 1) / (high - low + 1);
        for (index = 0; borders[ind][index + 1] <= code; index++);
        if (index == SYMBOL_EOF ) break;

        ull temporary = low, current = 0, tmp = 0;
        low = temporary + borders[ind][index] * (high - temporary + 1) / borders[ind][NUM_OF_SYMBOLS];
        high = temporary + borders[ind][index + 1] * (high - temporary + 1) / borders[ind][NUM_OF_SYMBOLS] - 1;
        for (;;) {
            if (high < Half) {
                ;
            } else if (low >= Half) {
                value -= Half;
                low -= Half;
                high -= Half;
            } else if (low >= First_qtr && high < Third_qtr) {
                value -= First_qtr;
                low -= First_qtr;
                high -= First_qtr;
            } else break;

            low += low;
            high += high + 1;
            value = 2*value + read_bit(ifp);
        }
        ++counter;
        putc(index, ofp);
        update_table(borders, frequency, index, agr);
        double summary_1 = 0, summary_2 = 0;
        for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
            summary_1 += (double) frequency[0][i];
            summary_2 += (double) frequency[1][i];
        }
        summary_1 /= (double) NUM_OF_SYMBOLS;
        summary_2 /= (double) NUM_OF_SYMBOLS;
        for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
            temp_arr[0][i] = frequency[0][i] - summary_1;
            temp_arr[1][i] = frequency[1][i] - summary_2;
        }

        for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
            summary_1 += (double)temp_arr[0][i]*(double)temp_arr[0][i];
            summary_2 += (double)temp_arr[1][i]*(double)temp_arr[1][i];
        }
        summary_1 /= (double) NUM_OF_SYMBOLS;
        summary_2 /= (double) NUM_OF_SYMBOLS;
        summary_1 = my_sqrt(summary_1);
        summary_2 = my_sqrt(summary_2);
        for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
            temp_arr[0][i] = temp_arr[0][i] / summary_1;
            temp_arr[1][i] = temp_arr[1][i] / summary_2;
        }
        for (i = 1; i <= NUM_OF_SYMBOLS; i++) {
            temp_arr[0][i] -= temp_arr[1][i];
        }
        result = func(temp_arr, 0);
        if (counter > 100 && check > 0) {
            if (result > previous) {
                ind = 1;
                counter = 50;
                check = -10;

            } else {
                ind = 0;
                counter = 0;
            }
        }
        if (check > 0) {
            if (previous == 0) {
                previous = result;
            }
            previous = (result + previous) / DIVIDER;
        }
    }
    fclose(ifp);
    fclose(ofp);
}

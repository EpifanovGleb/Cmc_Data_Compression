#include <stdio.h>
#include "ppm.h"
const int MAX_TOT_FR = 0x66666f2U;
const ull TOTAL_VALUE = 0xffffffffU;

void initialization(void) {
    context[0] = 0;
    StackPointer = 0;
    cm[NUMBER_OF_ELEMENTS].Total_Freq = NUMBER_OF_ELEMENTS;
    cm[NUMBER_OF_ELEMENTS].esc = 1;
    for (int j = 0; j < NUMBER_OF_ELEMENTS; j++) {
        cm[NUMBER_OF_ELEMENTS].count[j] = 1;
        cm[j].Total_Freq = 0;
        cm[j].esc = 0;
    }
    for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {
        for (int j = 0; j < NUMBER_OF_ELEMENTS; j++) {
            cm[i].count[j] = 0;
        }
    }
}

void new_scale(ContextModel *CM)
{
    CM->Total_Freq = 0;
    for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {
        CM->count[i] -= CM->count[i] / 2;
        CM->Total_Freq += CM->count[i];
    }
}

void update(int symbol)
{
    while (StackPointer > 0) {
        StackPointer--;
        if ((stack[StackPointer]->Total_Freq + stack[StackPointer]->esc) >= MAX_TOT_FR) {
            new_scale(stack[StackPointer]);
        }
        if (!stack[StackPointer]->count[symbol]) {
            stack[StackPointer]->esc += 1;
        }
        stack[StackPointer]->count[symbol] += 10;
        stack[StackPointer]->Total_Freq += 10;
    }
}

int read_bit_ppm(FILE *filein) {
    static int i = CODE_SIZE;
    static unsigned int temp = 0;
    if (i == CODE_SIZE) {
        if ((temp = getc(filein)) == EOF) return 0;
        i = 0;
    }
    i++;
    int bit = (int) (temp & 0x80);
    temp <<= 1;
    return (bit >> (CODE_SIZE - 1));
}

void write_bit(FILE * ofp, int bit, int check) {
    static unsigned char temp = 0;
    static int count = CODE_SIZE;
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
        count = CODE_SIZE;
    }
}

void bits_plus_follow_ppm(FILE * ofp, int bit, int *bits_to_follow) {
    write_bit(ofp, bit, 0);
    for (; *bits_to_follow > 0; (*bits_to_follow)--)
        write_bit(ofp, !bit, 0);
}
void arithmetic_encode(FILE *ofp, ull borders, ull range, ull divider, int *bits_to_follow, ull *low, ull *high)
{
    ull First_qtr = (TOTAL_VALUE + 1) / 4, Half = 2 * First_qtr, Third_qtr = 3 * First_qtr;
    ull temporary = *low;
    *low = temporary + borders * (*high - temporary + 1) / divider;
    *high = temporary + (borders + range) * (*high - temporary + 1) / divider - 1;
    for (;;) {
        if (*high < Half)
            bits_plus_follow_ppm(ofp, 0, bits_to_follow);
        else if (*low >= Half) {
            bits_plus_follow_ppm(ofp, 1, bits_to_follow);
            *low -= Half;
            *high -= Half;
        } else if ((*low >= First_qtr) && (*high < Third_qtr)) {
            *low -= First_qtr;
            *high -= First_qtr;
            (*bits_to_follow)++;
        } else {
            break;
        }
        *low += *low;
        *high += *high + 1;
    }
}

void ari_finish_encode(FILE * ofp, ull low, int bits_to_follow)
{
    bits_to_follow++;
    if (low < (TOTAL_VALUE + 1) / 4) {
        bits_plus_follow_ppm(ofp, 0, &bits_to_follow);
    } else {
        bits_plus_follow_ppm(ofp, 1, &bits_to_follow);
    }
    write_bit(ofp,0, 1);
    fflush(ofp);
}
int encode_symbol(FILE *ofp, ContextModel *CM, int c, int *bits_to_follow, ull *low, ull *high)
{
    stack[StackPointer] = CM;
    StackPointer++;
    if (CM->count[c]) {
        ull frequency_under = 0;
        for (int j = 0; j < c; j++) {
            frequency_under += CM->count[j];
        }
        arithmetic_encode( ofp, frequency_under, CM->count[c], CM->Total_Freq + CM->esc, bits_to_follow, low, high);
        return 1;
    } else {
        if (CM->esc) {
            arithmetic_encode(ofp, CM->Total_Freq, CM->esc, CM->Total_Freq + CM->esc, bits_to_follow, low, high);
        }
        return 0;
    }
}
void compress_ppm(char *input, char *output)
{
    FILE *ifp = fopen(input, "rb");
    FILE *ofp = fopen(output, "wb");

    fseek(ifp,0,SEEK_END);
    long size = 0;
    size = ftell(ifp);
    if (size == 0) {
        fclose(ifp);
        fclose(ofp);
        return;
    }
    fseek(ifp,0,SEEK_SET);

    int c, success, bits_to_follow = 0;
    ull low = 0, high = TOTAL_VALUE;
    initialization();

    while ((c = getc(ifp)) != EOF) {
        success = encode_symbol(ofp,&cm[context[0]], c, &bits_to_follow, &low, &high);
        if (!success) {
            encode_symbol(ofp, &cm[NUMBER_OF_ELEMENTS], c, &bits_to_follow, &low, &high);
        }
        update(c);
        context[0] = c;
    }
    if (cm[context[0]].esc == 0) {
        stack[StackPointer++] = &cm[context[0]];
        update(context[0]);
    }
    arithmetic_encode(ofp, cm[context[0]].Total_Freq, cm[context[0]].esc, cm[context[0]].Total_Freq + cm[context[0]].esc, &bits_to_follow, &low, &high);
    arithmetic_encode(ofp, cm[256].Total_Freq, cm[256].esc, cm[NUMBER_OF_ELEMENTS].Total_Freq+ cm[NUMBER_OF_ELEMENTS].esc, &bits_to_follow, &low, &high);
    ari_finish_encode(ofp, low, bits_to_follow);
}

void arithmetic_decode(FILE *ifp, ull borders, ull range, ull divider, ull *low, ull *high, ull *value)
{
    ull First_qtr = (TOTAL_VALUE + 1) / 4, Half = 2 * First_qtr, Third_qtr = 3 * First_qtr;
    ull temporary = *low;
    *low = temporary + borders * (*high - temporary + 1)/ divider;
    *high = temporary + (borders + range) * (*high - temporary + 1) / divider - 1;
    for (;;) {
        if (*high < Half) {
            ;
        } else if (*low >= Half) {
            *low -= Half;
            *high -= Half;
            *value -= Half;
        } else if ((*low >= First_qtr) && (*high < Third_qtr)) {
            *low -= First_qtr;
            *high -= First_qtr;
            *value -= First_qtr;
        } else break;

        *low += *low;
        *high += *high + 1;
        *value = 2 * (*value) + read_bit_ppm(ifp);
    }
}

int decode_symbol(FILE *ifp, ContextModel *CM, int *c, ull *value)
{
    stack[StackPointer++] = CM;
    static ull high = TOTAL_VALUE, low = 0;
    if (!CM->esc) {
        return 0;
    }
    ull frequency = ((*value - low + 1) * (CM->Total_Freq + CM->esc) - 1) / (high - low + 1);
    if (frequency < CM->Total_Freq) {
        ull frequency_under = 0;
        int i = 0;
        for (;;) {
            if ((frequency_under + CM->count[i]) <= frequency) {
                frequency_under += CM->count[i];
            } else break;
            i++;
        }
        arithmetic_decode(ifp, frequency_under, CM->count[i], CM->Total_Freq + CM->esc, &low, &high, value);
        *c = i;
        return 1;
    } else {
        arithmetic_decode(ifp, CM->Total_Freq, CM->esc, CM->Total_Freq + CM->esc, &low, &high, value);
        return 0;
    }
}

void decompress_ppm(char *input, char *output)
{
    FILE *ifp=fopen(input, "rb");
    FILE *ofp=fopen(output, "wb");

    fseek(ifp,0,SEEK_END);
    long size = 0;
    size = ftell(ifp);
    if (size == 0) {
        fclose(ifp);
        fclose(ofp);
        return;
    }
    fseek(ifp,0,SEEK_SET);

    int ch, success, i;
    ull value = 0;
    initialization();
    for (i = 1; i <= BIT_SIZE; i++) {
        value = value << CODE_SIZE;
        value += getc(ifp);
    }
    for (;;) {
        success = decode_symbol(ifp, &cm[context[0]], &ch, &value);
        if (!success) {
            success = decode_symbol(ifp, &cm[NUMBER_OF_ELEMENTS], &ch, &value);
            if (!success) break;
        }
        update(ch);
        putc(ch, ofp);
        context[0] = ch;
    }
    fclose(ifp);
    fclose(ofp);
}

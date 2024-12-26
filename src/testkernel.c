#define OH 28
#define OW 28
#define KH 3
#define KW 3
#define NCIN 128
#define NCOUT 128

#define N 10
int in[OH][OW][NCIN];
int w[KH][KW][NCIN][NCOUT];
int out[OH][OW][NCOUT];

int main(void)
{
    int oh;
    int ow;
    int kh;
    int kw;
    int ncin;
    int ncout;

    for(oh = 0; oh < OH; oh++) {
        for(ow = 0; ow < OW; ow++) {
            for(kh = 0; kh < KH; kh++) {
                for(kw = 0; kw < KW; kw++) {
                    for(ncin = 0; ncin < NCIN; ncin++) {
                        for(ncout = 0; ncout < NCOUT; ncout++) {
                            in[oh][ow][ncin] = 3;
                            w[kh][kw][ncin][ncout] = 7;
                            out[oh][ow][ncout] = 0;
                        }
                    }
                }
            }
        }
    }


    for(oh = 0; oh < OH; oh++) {
        for(ow = 0; ow < OW; ow++) {
            for(kh = 0; kh < KH; kh++) {
                for(kw = 0; kw < KW; kw++) {
                    for(ncin = 0; ncin < NCIN; ncin++) {
                        for(ncout = 0; ncout < NCOUT; ncout++) {
                            out[oh][ow][ncout] += in[oh][ow][ncin] * w[kh][kw][ncin][ncout];
                        }
                    }
                }
            }
        }
    }
    return 0;
}
#ifndef FFT_H
#define FFT_H

#include <complex.h>

typedef short sample_t;

// ブロックサイズが 2^k かチェック
int pow2check(long N);

// FFT／IFFT の関数
void fft(complex double *x, complex double *y, long n);
void ifft(complex double *y, complex double *x, long n);

// サンプル ⇔ 複素数変換
void sample_to_complex(sample_t *s, complex double *X, long n);
void complex_to_sample(complex double *X, sample_t *s, long n);

// FFTとIFFTを一括実行
void fft_filter(sample_t *samples, long n);

#endif /* FFT_H */

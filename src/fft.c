#include "fft.h"
#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// エラー時にメッセージ出力して終了
static void die(const char *s) {
    perror(s);
    exit(1);
}

// ブロックサイズ n が 2^k かどうかチェック
int pow2check(long N) {
    long n = N;
    while (n > 1) {
        if (n % 2) return 0;
        n /= 2;
    }
    return 1;
}

// 再帰的に FFT／IFFT を実行する内部関数
static void fft_r(complex double *x, complex double *y, long n, complex double w) {
    if (n == 1) {
        y[0] = x[0];
    } else {
        complex double W = 1.0;
        for (long i = 0; i < n/2; i++) {
            y[i]       = x[i] + x[i + n/2];
            y[i + n/2] = W * (x[i] - x[i + n/2]);
            W *= w;
        }
        // 偶数成分・奇数成分に分けて再帰
        fft_r(y,       x,       n/2, w * w);
        fft_r(y + n/2, x + n/2, n/2, w * w);
        // 結果を interleave
        for (long i = 0; i < n/2; i++) {
            y[2*i]     = x[i];
            y[2*i + 1] = x[i + n/2];
        }
    }
}

// FFT: x -> y
void fft(complex double *x, complex double *y, long n) {
    double arg = 2.0 * M_PI / n;
    complex double w = cos(arg) - I * sin(arg);
    fft_r(x, y, n, w);
    // 正規化
    for (long i = 0; i < n; i++) {
        y[i] /= n;
    }
}

// IFFT: y -> x
void ifft(complex double *y, complex double *x, long n) {
    double arg = 2.0 * M_PI / n;
    complex double w = cos(arg) + I * sin(arg);
    fft_r(y, x, n, w);
}

// 16bit サンプル配列を複素数配列に変換
void sample_to_complex(sample_t *s, complex double *X, long n) {
    for (long i = 0; i < n; i++) {
        X[i] = s[i];
    }
}

// 複素数配列を 16bit サンプル配列に変換（虚部は破棄）
void complex_to_sample(complex double *X, sample_t *s, long n) {
    for (long i = 0; i < n; i++) {
        s[i] = (sample_t)creal(X[i]);
    }
}
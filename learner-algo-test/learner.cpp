#include "messages.h"
#include "learner.h"

#include <vector>

using namespace std;

Learner::Learner(int i_learner_id, int i_totalReplicas, int i_partialMessageCount, int i_partialModulus) {
    learner_id = i_learner_id;
    totalReplicas = i_totalReplicas;
    partialMessageCount = i_partialMessageCount;
    partialModulus = i_partialModulus;

    journal = vector<string>(0);
}



uint32_t Learner::modpow(uint32_t a, uint32_t e, uint32_t p) {
    uint32_t r = 1;
    while (e > 0) {
        if (e & 1) r = (r * a) % p;
        a = (a * a) % p;
        e >>= 1;
    }
    return r;
}

uint32_t Learner::modinv(uint32_t x, uint32_t p) {
    return modpow(x, p - 2, p);
}

vector<vector<uint16_t>> Learner::invertMatrix(vector<vector<uint16_t>> A, int p) {
    int n = A.size();

    // Form augmented matrix [A | I]
    vector<vector<int32_t>> aug(n, vector<int32_t>(2*n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            aug[i][j] = A[i][j] % p;

            aug[i][n+j] = 0;
            if (i == j) aug[i][n+j] = 1;
        }
            
    }

    // Gauss-Jordan
    for (int col = 0; col < n; col++) {

        // Find pivot row
        int pivot = col;
        while (pivot < n && aug[pivot][col] == 0) pivot++;
        if (pivot == n) throw runtime_error("Matrix is not invertible mod p");

        swap(aug[col], aug[pivot]);

        // Normalize pivot row
        int32_t inv = modinv(aug[col][col], p);
        for (int j = 0; j < 2*n; j++)
            aug[col][j] = (aug[col][j] * inv) % p;

        // Eliminate other rows
        for (int i = 0; i < n; i++) {
            if (i == col) continue;
            int32_t factor = aug[i][col];
            for (int j = 0; j < 2*n; j++) {
                int32_t temp = (aug[i][j] - factor * aug[col][j]) % p;
                if (temp < 0) temp += p;
                aug[i][j] = temp;
            }
        }

    }

    // Extract inverse matrix
    vector<vector<uint16_t>> invA(n, vector<uint16_t>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            invA[i][j] = aug[i][n+j];

    return invA;
}

vector<vector<uint16_t>> Learner::gen_A() { 
    
    int n = totalReplicas;
    int m = partialMessageCount;
    int p = partialModulus;

    vector<vector<uint16_t>> A(n,vector<uint16_t>(m));

    for (int i = 0; i < n; i++) {

        vector<uint16_t> a_i(m);

        for (int j = 0; j < m; j++) {

            int exp = j;
            int base = i+1;
            int mod = p;
            int result = 1;

            while (exp > 0) {
                if (exp & 1) {
                    result = (result * base) % mod;
                }

                base = (base * base) % mod;
                exp >>= 1;
            }

            A[i][j] = result;

        }

    }

    return A;

}
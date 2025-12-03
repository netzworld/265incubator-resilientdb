#include <iostream>
#include <vector>
#include <cstdint>

#include "replica.h"
#include "messages.h"

using namespace std;

vector<vector<uint16_t>> gen_A(int n, int m, int p) {    
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

uint32_t modpow(uint32_t a, uint32_t e, uint32_t p) {
    uint32_t r = 1;
    while (e > 0) {
        if (e & 1) r = (r * a) % p;
        a = (a * a) % p;
        e >>= 1;
    }
    return r;
}

uint32_t modinv(uint32_t x, uint32_t p) {
    return modpow(x, p - 2, p);
}

vector<vector<uint16_t>> invertMatrix(vector<vector<uint16_t>> A, int p) {
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

string decode(vector<vector<uint32_t>> Fs, int n, int m, int p) {
    vector<vector<uint16_t>> A = gen_A(n, m, p);

    vector<vector<uint16_t>> Ap;
    for (auto i = 0; i < m; i++) {
        Ap.push_back(A[i]);
    }

    vector<vector<uint16_t>> Ainv = invertMatrix(Ap, p);

    for (auto i = 0; i < Ainv.size(); i++) {
        for (auto j = 0; j < Ainv[i].size(); j++) {
            cout << Ainv[i][j];
            cout << " ";
        }
        cout << endl;
    }

    string hehe = "";
    for (auto i_iter = 0; i_iter < Fs[0].size(); i_iter++) { // iterate through each block of m bytes
        
        for (auto b_iter = 0; b_iter < m; b_iter++) { // iterate through each byte

            uint32_t mid = 0;
            for (int m_iter = 0; m_iter < m; m_iter++) { // do dot product

                mid = (mid + Ainv[b_iter][m_iter] * Fs[m_iter][i_iter]) % p;

            }

            hehe += (char)mid;
            cout << mid << endl;

        }

    }

    return hehe;
}




int main() {
    string input_data = "12345678901234";

    Replica r0(0, 4, 3, 257);
    UpdateLearnerMessage um1 = r0.createUpdateLearnerMessage(input_data);
    Replica r1(1, 4, 3, 257);
    UpdateLearnerMessage um2 = r1.createUpdateLearnerMessage(input_data);
    Replica r2(2, 4, 3, 257);
    UpdateLearnerMessage um3 = r2.createUpdateLearnerMessage(input_data);

    vector<vector<uint32_t>> Fs;

    Fs.push_back(um1.bytes);
    Fs.push_back(um2.bytes);
    Fs.push_back(um3.bytes);

    for(auto i : Fs){
        for (auto j : i) {
            cout << j << " ";
        }
        cout << endl;
    }
    cout << "FS generated" << endl;
    
    string hehe = decode(Fs, 4, 3, 257);

    cout << hehe << endl;
}
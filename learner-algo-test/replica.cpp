#include "messages.h"
#include "replica.h"

#include <vector>

using namespace std;

Replica::Replica(int i_replica_id, int i_totalReplicas, int i_partialMessageCount, int i_partialModulus) {
    replica_id = i_replica_id;
    totalReplicas = i_totalReplicas;
    partialMessageCount = i_partialMessageCount;
    partialModulus = i_partialModulus;
}

UpdateLearnerMessage Replica::createUpdateLearnerMessage(string data) {

    int n = totalReplicas;
    uint32_t m = partialMessageCount;
    int p = partialModulus;

    UpdateLearnerMessage msg;

    msg.message_length = data.size();
    msg.origin_rep_id = replica_id;

    while (data.size() % m != 0) {
        data += "0";
    }

    vector<vector<uint16_t>> A = gen_A();

    // vector<uint32_t> F_i;

    uint32_t iter = 0;
    uint32_t c_ik = 0;
    for (int d = 0; d < data.size(); d++) { // data MUST BE A MULTIPLE OF m
      c_ik = (c_ik + A[replica_id][iter] * (uint32_t)(unsigned char)data[d]) % p;
      iter++;
      if (iter == m) {
        msg.bytes.push_back(c_ik);

        c_ik = 0;
        iter = 0;
      }
    }

    return msg;
}

vector<vector<uint16_t>> Replica::gen_A() { 
    
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
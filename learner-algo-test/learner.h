#include "messages.h"

using namespace std;

class Learner {
public:
    Learner(int learner_id, int totalReplicas, int partialMessageCount, int partialModulus);

    int receiveUpdateMessage(UpdateLearnerMessage um);

private:
    int learner_id;
    int totalReplicas;
    int partialMessageCount;
    int partialModulus;

    vector<string> journal;

    vector<vector<uint16_t>> gen_A();
    vector<vector<uint16_t>> invertMatrix(vector<vector<uint16_t>> A, int p);
    uint32_t modinv(uint32_t x, uint32_t p);
    uint32_t modpow(uint32_t a, uint32_t e, uint32_t p);
};
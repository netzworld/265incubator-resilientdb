#include "messages.h"

using namespace std;

class Replica {
public:
    Replica(int replica_id, int totalReplicas, int partialMessageCount, int partialModulus);

    
    UpdateLearnerMessage createUpdateLearnerMessage(string data);

private:
    int replica_id;
    int totalReplicas;
    int partialMessageCount;
    int partialModulus;

    vector<vector<uint16_t>> gen_A();
};
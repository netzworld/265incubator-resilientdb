import sys, os, time, subprocess, random, time
subprocess.run("bazel build :pybind_kv_so", shell=True)
sys.path.append(os.getcwd())
from kv_operation import set_value, get_value, get_value_readonly

RED   = "\033[31m"
GREEN = "\033[32m"
RESET = "\033[0m"
entries = 30
delay = 4

for i in range(delay, 0, -1):
    print(f'Test begins in: {i}')
    time.sleep(1)

for i in range(entries):
    print(set_value(f"test{i}", f"Value {i}"))

# Learner reads
avg_time_readonly = 0.0
for i in random.sample(range(entries), 10):
    start = time.time()
    print(f"Key 'test' value: {get_value_readonly('test')}")
    end = time.time()
    response_time = end - start
    avg_time_readonly = avg_time_readonly + response_time

avg_time_readonly = avg_time_readonly / 10
print(f'Readonly request average response time: {avg_time_readonly}')

# Fallback reads
avg_time_fallback = 0.0
for i in random.sample(range(entries), 10):
    start = time.time()
    print(f"Key 'test{i}' value: {get_value_readonly(f'test{i}')}")
    end = time.time()
    response_time = end - start
    avg_time_fallback = avg_time_fallback + response_time

avg_time_fallback = avg_time_fallback / 10
print(f'PBFT fallback request average response time: {avg_time_fallback}')

if avg_time_fallback > 0:
    improvement_pct = (avg_time_fallback - avg_time_readonly) / avg_time_fallback * 100.0
    color = GREEN if improvement_pct > 0 else RED
    print(f"Learner was {color}{improvement_pct:.2f}%{RESET} faster that PBFT fallback.")
else:
    print("PBFT fallback average time was 0, cannot compute percentage.")

# PBFT pure reads
avg_time = 0.0
for i in random.sample(range(entries), 10):
    start = time.time()
    print(f"Key 'test{i}' value: {get_value(f'test{i}')}")
    end = time.time()
    response_time = end - start
    avg_time = avg_time + response_time

avg_time = avg_time / 10
print(f'PBFT request average response time: {avg_time}')


if avg_time > 0:
    improvement_pct = (avg_time - avg_time_readonly) / avg_time * 100.0
    color = GREEN if improvement_pct > 0 else RED
    print(f"Learner was {color}{improvement_pct:.2f}%{RESET} faster than pure PBFT")
else:
    print("PBFT average time was 0, cannot compute percentage.")
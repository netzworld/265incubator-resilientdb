import sys, os, time, subprocess, random, time
subprocess.run("bazel build :pybind_kv_so", shell=True)
sys.path.append(os.getcwd())
from kv_operation import set_value, get_value, get_value_readonly

RED   = "\033[31m"
GREEN = "\033[32m"
RESET = "\033[0m"
entries = 100
delay = 3

# Comparison reporter: prints percent improvement and speedup vs a baseline.
def report(label, learner_time, baseline_time):
    if baseline_time <= 0:
        print(f"{label}: baseline zero, cannot compare")
        return
    if learner_time <= 0:
        print(f"{label}: learner zero, cannot compare")
        return
    speedup = baseline_time / learner_time
    improvement_pct = (baseline_time - learner_time) / baseline_time * 100.0
    color = GREEN if improvement_pct > 0 else RED
    print(f"{label}: {color}{improvement_pct:.2f}%{RESET} improvement ({speedup:.1f}x faster)")

for i in range(delay, 0, -1):
    print(f'Test begins in: {i}')
    time.sleep(1)

# populate db
for i in range(entries):
    set_value(f"test{i}", f"Value {i}")

# Learner reads
avg_time_readonly = 0.0
for i in random.sample(range(entries), 10):
    start = time.time()
    print(f"Key 'test' value: {get_value_readonly(f'test')}")
    end = time.time()
    response_time = end - start
    avg_time_readonly = avg_time_readonly + response_time

avg_time_readonly = avg_time_readonly / 10
print(f'Readonly request average response time: {avg_time_readonly}')

# Fallback reads
avg_time_fallback = 0.0
for i in random.sample(range(entries), 10):
    start = time.time()
    print(f"Key 'test-fallback' value: {get_value_readonly(f'test-fallback')}")
    end = time.time()
    response_time = end - start
    avg_time_fallback = avg_time_fallback + response_time

avg_time_fallback = avg_time_fallback / 10
print(f'PBFT fallback request average response time: {avg_time_fallback}')

report("Learner vs PBFT fallback", avg_time_readonly, avg_time_fallback)

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


report("Learner vs pure PBFT", avg_time_readonly, avg_time)
if avg_time > 0:
    fallback_overhead_pct = (avg_time_fallback - avg_time) / avg_time * 100.0
    color = RED if fallback_overhead_pct > 0 else GREEN
    slower_or_faster = "slower" if fallback_overhead_pct > 0 else "faster"
    print(f"PBFT fallback was {color}{fallback_overhead_pct:.2f}%{RESET} {slower_or_faster} than pure PBFT")

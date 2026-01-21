import pandas as pd
import matplotlib.pyplot as plt

files = {
    "sum_seq": "../results_sum_seq.csv",
    "sum_rr": "../results_sum_rr.csv",
    "sum_rr_fs": "../results_sum_rr_fs.csv",
    "sum_fs": "../results_sum_fs.csv",
    "sum_loc": "../results_sum_loc.csv",
    "sum_loc_reduce": "../results_sum_loc_reduce.csv",
    "sum_reduce_cpp": "../results_sum_reduce_cpp.csv",
}


plt.figure()

for label, path in files.items():
    df = pd.read_csv(path)
    plt.plot(df["T"], df["time_ms"], marker="o", label=label)

plt.xlabel("Threads")
plt.ylabel("Time (ms)")
plt.title("Execution Time vs Threads")
plt.legend()
plt.grid(True)
plt.savefig("compare_time_vs_threads.png", dpi=300)
plt.close()


plt.figure()

for label, path in files.items():
    df = pd.read_csv(path)
    plt.plot(df["T"], df["speedup"], marker="o", label=label)

plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Threads")
plt.legend()
plt.grid(True)
plt.savefig("compare_speedup_vs_threads.png", dpi=300)
plt.close()


plt.figure()

for label, path in files.items():
    df = pd.read_csv(path)
    plt.plot(df["T"], df["efficiency"], marker="o", label=label)

plt.xlabel("Threads")
plt.ylabel("Efficiency")
plt.title("Efficiency vs Threads")
plt.legend()
plt.grid(True)
plt.savefig("compare_efficiency_vs_threads.png", dpi=300)
plt.close()

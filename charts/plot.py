import pandas as pd
import matplotlib.pyplot as plt

plt.style.use("dark_background")

df = pd.read_csv("../results.csv")

# ---- TIME PLOT ----
plt.figure()
plt.plot(df["T"], df["time_ms"], marker="o", color="red")
plt.xlabel("Threads")
plt.ylabel("Time (ms)")
plt.title("Execution Time vs Threads")
plt.grid(True)
plt.savefig("time_vs_threads.png", dpi=300)
plt.close()

# ---- SPEEDUP PLOT ----
plt.figure()
plt.plot(df["T"], df["speedup"], marker="o", color="red")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Threads")
plt.grid(True)
plt.savefig("speedup_vs_threads.png", dpi=300)
plt.close()

# ---- EFFICIENCY PLOT ----
plt.figure()
plt.plot(df["T"], df["efficiency"], marker="o", color="red")
plt.xlabel("Threads")
plt.ylabel("Efficiency")
plt.title("Efficiency vs Threads")
plt.grid(True)
plt.savefig("efficiency_vs_threads.png", dpi=300)
plt.close()

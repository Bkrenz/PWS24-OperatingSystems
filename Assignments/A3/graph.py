import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('results.csv')
df['time'] = df['time'].div(1000).round(3)
df.dropna()

plt.plot(df['threads'], df['time'], 'ro')
plt.title('Threaded Primes Run Times')
plt.xlabel('# of Threads')
plt.ylabel('Time (s)')
plt.show()
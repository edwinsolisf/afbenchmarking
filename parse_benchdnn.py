import pandas as pd
import numpy as np

def main():
    lines = None
    with open('./benchdnn_results.txt', 'r') as f:
        lines = f.readlines()[:-3]
        lines.insert(0, "driver,engine,mintime,avgtime,maxtime,createtime")
    with open('./bench.csv', 'w') as f:
        f.writelines(lines)

    df = pd.read_csv('./bench.csv')
    res = df['avgtime']
    print("Results")
    print(f"average: {np.average(res)} ms")
    print(f"stddev: {np.std(res)} ms")
    print(f"min: {np.min(res)} ms")
    print(f"max: {np.max(res)} ms")
    print(f"median: {np.median(res)} ms")
    print(f"99%: {np.percentile(res, 0.99)} ms")

if __name__ == '__main__':
    main()
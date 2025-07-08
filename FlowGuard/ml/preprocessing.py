import pandas as pd
import numpy as np
from scipy.stats import skew, kurtosis
from datetime import datetime


INPUT_FILE = 'flow_log.csv'
OUTPUT_FILE = 'ml/data/processed_flow_data.csv'

def load_and_clean_data(path):
    df = pd.read_csv(path, names=["flow", "timestamp"])

    
    df["timestamp"] = pd.to_datetime(df["timestamp"], errors='coerce')
    df = df.dropna()  

    
    df = df.drop_duplicates(subset="flow", keep="first")

   
    z = np.abs((df["flow"] - df["flow"].mean()) / df["flow"].std())
    df = df[z < 3]  

    return df

def extract_features(df):
    flow_series = df["flow"]
    features = {
        "mean_flow": flow_series.mean(),
        "std_flow": flow_series.std(),
        "min_flow": flow_series.min(),
        "max_flow": flow_series.max(),
        "skewness": skew(flow_series),
        "kurtosis": kurtosis(flow_series),
        "range": flow_series.max() - flow_series.min(),
        "iqr": flow_series.quantile(0.75) - flow_series.quantile(0.25),
    }
    return pd.DataFrame([features])

if __name__ == "__main__":
    print("Loading and preprocessing data...")
    df = load_and_clean_data(INPUT_FILE)

    print(f"Cleaned {len(df)} rows.")
    features_df = extract_features(df)

   
    features_df.to_csv(OUTPUT_FILE, index=False)
    print(f"Features saved to {OUTPUT_FILE}")

import pandas as pd
from sklearn.ensemble import IsolationForest
import numpy as np
import joblib
import os
import sys

class FlowAnomalyDetector:
    def __init__(self, model_path="ml/model.joblib"):
        self.model_path = model_path
        self.model = None

    def train(self, csv_path="ml/data/example.csv"):
        if not os.path.exists(csv_path):
            raise FileNotFoundError(f"No data found at {csv_path}")

        data = pd.read_csv(csv_path)
        if 'flow' not in data.columns:
            raise ValueError("CSV must contain a 'flow' column")

        X = data[['flow']].values
        self.model = IsolationForest(contamination=0.05, random_state=42)
        self.model.fit(X)

        joblib.dump(self.model, self.model_path)
        print(f"[ML] Model trained and saved to {self.model_path}")

    def load(self):
        if not os.path.exists(self.model_path):
            raise FileNotFoundError("Trained model not found. Train the model first.")
        self.model = joblib.load(self.model_path)
        print("[ML] Model loaded.")

    def predict(self, flow_value: float):
        if self.model is None:
            raise RuntimeError("Model is not loaded.")
        result = self.model.predict([[flow_value]])  # 1 (inlier) -1 (outlier)
        return "consistent" if result[0] == 1 else "inconsistent"


if __name__ == "__main__":
    detector = FlowAnomalyDetector()

    # Train from file (only once needed)
    # detector.train()

    #
    detector.load()
    
    test_values = [48.5, 52.1, 120.0, 3.2]
    for val in test_values:
        verdict = detector.predict(val)
        print(f"[Flow Value: {val}] → {verdict}")


if __name__ == "__main__":
    detector = FlowAnomalyDetector()
    detector.load()

    if len(sys.argv) != 2:
        print("Usage: python ml_model.py <flow_value>")
        sys.exit(1)

    try:
        flow_val = float(sys.argv[1])
        result = detector.predict(flow_val)
        print(result)  
    except Exception as e:
        print(f"error: {e}")
        sys.exit(1)
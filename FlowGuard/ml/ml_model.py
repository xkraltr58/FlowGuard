import pandas as pd
from sklearn.ensemble import IsolationForest
import numpy as np
import joblib
import os
import sys


def log_real_data(flow_value: float, csv_path="/home/naci/FlowGuard/ml/data/example.csv"):
    os.makedirs(os.path.dirname(csv_path), exist_ok=True)

    if not os.path.exists(csv_path):
        with open(csv_path, 'w') as f:
            f.write("flow\n")

    with open(csv_path, 'a') as f:
        f.write(f"{flow_value}\n")


class FlowAnomalyDetector:
    def __init__(self, model_path=None):
        base_dir = os.path.dirname(os.path.abspath(__file__))  
        if model_path is None:
            self.model_path = os.path.join(base_dir, "model.joblib")
        else:
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

    def predict(self, flow_value: float):
        if self.model is None:
            raise RuntimeError("Model is not loaded.")

        X = np.array([[flow_value]])
        result = self.model.predict(X)[0]  # 1 (inlier) / -1 (outlier)
        score = self.model.decision_function(X)[0]  

        log_real_data(flow_value)

       
        
        confidence = 1 / (1 + np.exp(-score))  

        status = "consistent" if result == 1 else "inconsistent"
        return status, confidence



if "--train" in sys.argv:
    detector = FlowAnomalyDetector()
    detector.train()
    sys.exit(0)


if __name__ == "__main__":
    detector = FlowAnomalyDetector()
    detector.load()

    if len(sys.argv) != 2:
        print("Usage: python ml_model.py <flow_value>")
        sys.exit(1)

    try:
        flow_val = float(sys.argv[1])
        status, confidence = detector.predict(flow_val)
        print(f"{status}|{confidence:.4f}")
    except Exception as e:
        print(f"error: {e}")
        sys.exit(1)

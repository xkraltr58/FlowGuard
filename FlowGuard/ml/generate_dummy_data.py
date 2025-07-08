import pandas as pd
import numpy as np
import os

# Generate 100 synthetic flow values around 42.5 ± 1.0
flows = np.random.normal(loc=42.5, scale=1.0, size=100)

df = pd.DataFrame({'flow': flows})

# Save to CSV
os.makedirs("ml/data", exist_ok=True)
df.to_csv("ml/data/example.csv", index=False)

print("[INFO] Dummy example.csv created with 100 flow values.")

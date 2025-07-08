import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
import joblib


INPUT_FILE = 'ml/data/processed_flow_data.csv'
MODEL_OUTPUT = 'ml/models/flow_model.pkl'


def train_model():
    print("Loading processed features...")
    df = pd.read_csv(INPUT_FILE)

    
    df['label'] = [0] * (len(df) - 5) + [1] * 5  

    X = df.drop(columns=['label'])
    y = df['label']

    print("Splitting dataset...")
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    print(" Training Random Forest...")
    clf = RandomForestClassifier(n_estimators=100, random_state=42)
    clf.fit(X_train, y_train)

    print("Training complete. Evaluating...")
    y_pred = clf.predict(X_test)
    print(classification_report(y_test, y_pred))

    print(f"Saving model to {MODEL_OUTPUT}...")
    joblib.dump(clf, MODEL_OUTPUT)
    print(" Model saved.")

if __name__ == '__main__':
    train_model()

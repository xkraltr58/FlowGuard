# FlowGuard
Real-time flow monitoring and anomaly detection system with ROS 2 integration and ML-based consistency analysis.


# FlowGuard

FlowGuard is a real-time flow monitoring and anomaly detection system designed for embedded and robotic applications. It processes serial sensor data, applies data validation using CRC-8, extracts features, and performs machine learning-based consistency analysis. The results are published to ROS 2 topics, enabling seamless integration into autonomous systems or industrial pipelines.

---

## 📦 Features

- **Robust Serial Communication**: Reads and parses flow sensor data via serial port (USB/UART).
- **CRC-8 Data Validation**: Detects data corruption with checksum validation.
- **Feature Extraction**: Computes domain-specific features for anomaly detection.
- **Machine Learning Integration**:
  - Supports advanced models (Random Forest, AdaBoost, Isolation Forest).
  - Trained to detect inconsistent or faulty flow patterns.
- **ROS 2 Integration**:
  - Publishes processed flow values to `/flow_value` topic.
  - Publishes flow status (`consistent` / `inconsistent`) to `/flow_status`.
- **Alarm System**: Dedicated ROS 2 node triggers alerts when anomalous flow is detected.
- **Modular Codebase**: Clean structure with separation of concerns:
  - `preprocessing.py`
  - `train_model.py`
  - `evaluate_model.py`
  - `ml_model.py`
  - `ros2_interface/`

---

## 🏗️ System Architecture

```plaintext
[Sensor] 
   ↓
[Serial Reader + CRC8] 
   ↓
[Feature Extraction]
   ↓
[ML Model (trained)]
   ↓
[ROS 2 Publisher]
   ├──> /flow_value : float
   └──> /flow_status : string (consistent / inconsistent)
                       ↓
                 [ROS 2 Alarm Subscriber]

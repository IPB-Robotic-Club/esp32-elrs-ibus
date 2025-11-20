# CRSF & IBus Protocol Decoder Using ESP32

### **Hardware**
| **Protocol**       | **Reciever Type** | **Description**                                   |
|------------------|---------------------------|---------------------------------------------------|
| **CRSF**      | ExpressLRS                | an open-source RC link protocol that uses LoRa modulation for high update rates (up to 1000Hz), ultra-low latency (~3ms), and long-range communication over 2.4GHz or 900MHz frequencies. |
| **IBus**       | Flysky FS-IA6B                | a digital serial communication protocol developed by FlySky for RC receivers, transmitting up to 18 channels of control data at 115200 baud with low latency over a single bi-directional UART line |

---
### **Wiring Diagram**
```mermaid
flowchart LR
  subgraph Wiring Standalone ELRS
    ESP32[ESP32]
    ELRS[ELRS]
  end

  ELRS <-- GND --> ESP32
  ELRS <-- 5V --> ESP32
  ELRS <-- TX GPIO --> ESP32

```

```mermaid
flowchart LR
  subgraph Wiring ELRS with FC
    ESP32[ESP32]
    ELRS[ELRS]
    FC[FC]
  end

  ELRS <-- GND --> ESP32
  ELRS <-- TX GPIO --> ESP32
  ELRS <-- TX RX --> FC
  ELRS <-- 5V --> FC
  ELRS <-- GND --> FC

```


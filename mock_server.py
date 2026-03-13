import socket
import json
import time
import math

HOST = '127.0.0.1'
PORT = 26001

def generate_telemetry(distance_cm):
    if distance_cm > 0:
        base_speed_cms = math.sqrt(distance_cm) * 8.0
        v_est = base_speed_cms * 0.9 
    else:
        base_speed_cms = 0
        v_est = 0

    return {
        "monitoring": {
            "odometer": {
                "v_est": int(v_est),
                "v_max": 20000,
                "timestamp": int(time.time())
            },
            "speedAndDistance": {
                "dEbi": int(distance_cm),
                "vEbi": int(base_speed_cms),
                "dSbi1": int(distance_cm - 5000),
                "dSbi2": int(distance_cm - 10000),
                "vSbi": int(base_speed_cms * 0.95),
                "dWarning": int(distance_cm),
                "vWarning": int(base_speed_cms * 0.90),
                "dPermitted": int(distance_cm),
                "vPermitted": int(base_speed_cms * 0.85),
                "dIndication": int(distance_cm + 2000)
            },
            "trainPosition": {
                "max_safe_front_end": int(distance_cm + 5000), 
                "solr_ref": {
                    "d_est_front_end": int(distance_cm), 
                    "is_valid": True
                }
            }
        }
    }

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen()
        print(f"Mock Server {PORT} portunda dinliyor...")
        
        while True: 
            conn, addr = s.accept()
            print(f"Bağlantı sağlandı: {addr}")
            with conn:
                distance_cm = 1000000.0 
                while distance_cm >= 0:
                    payload = generate_telemetry(distance_cm)
                    json_string = json.dumps(payload) + "##"
                    
                    try:
                        conn.sendall(json_string.encode('utf-8'))
                    except BrokenPipeError:
                        break
                    except Exception as e:
                        break
                        
                    distance_cm -= 3000.0 
                    
                    # YENİ EĞRİ SİMÜLASYONU: 2KM'ye gelince hedefi 8KM'ye uzat
                    if distance_cm < 200000.0 and distance_cm > 196000.0:
                        print(">> YENİ YETKİ (MA) GELDİ! Eğri sıfırlanıyor...")
                        distance_cm = 800000.0 
                    
                    time.sleep(0.1) 
                print("Simülasyon bitti.")

if __name__ == '__main__':
    start_server()
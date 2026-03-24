import socket
import json
import time
import math

HOST = '127.0.0.1'
PORT = 26001

def generate_telemetry(distance_cm, cycle_count):
    # 4000 metreden (400,000 cm) sonra frenleme eğrisi başlıyor
    braking_start_cm = 400000.0

    if distance_cm > braking_start_cm:
        # Görseldeki düz/tavan (Ceiling) hızı kısmı
        base_speed_cms = math.sqrt(braking_start_cm) * 8.0 
    else:
        # Parabolik fren eğrisi kısmı
        base_speed_cms = math.sqrt(distance_cm) * 8.0 if distance_cm > 0 else 0

    # Trenin anlık hızı (v_est), limitlerden biraz daha düşük simüle ediliyor
    v_est = base_speed_cms * 0.80

    payload = {
        "monitoring": {
            "odometer": {
                "v_est": int(v_est),
                "v_max": 20000,
                "timestamp": int(time.time())
            },
            "speedAndDistance": {
                # Mesafeleri biraz ofsetli gönderiyoruz ki görseldeki gibi sıyrılıp insinler
                "dEbi": int(distance_cm),
                "vEbi": int(base_speed_cms * 1.05),
                
                "dSbi1": int(distance_cm - 1000),
                "dSbi2": int(distance_cm - 2000),
                "vSbi": int(base_speed_cms * 1.0),
                
                "dWarning": int(distance_cm - 3000),
                "vWarning": int(base_speed_cms * 0.95),
                
                "dPermitted": int(distance_cm - 4000),
                "vPermitted": int(base_speed_cms * 0.90),
                
                "dIndication": int(distance_cm - 5000),
                "vIndication": int(base_speed_cms * 0.85) # Y Ekseni İçin Eklendi
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

    # Fallback testini kasten devredışı bıraktım, kesintisiz o güzel eğrileri görebilmen için
    return payload

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
                distance_cm = 1000000.0 # 10 KM'den başlıyor
                cycle_count = 1
                while distance_cm >= 0:
                    payload = generate_telemetry(distance_cm, cycle_count)
                    json_string = json.dumps(payload) + "##"
                    
                    try:
                        conn.sendall(json_string.encode('utf-8'))
                    except BrokenPipeError:
                        break
                    except Exception as e:
                        break
                        
                    distance_cm -= 3000.0 # Hızla yaklaşıyor
                    cycle_count += 1
                    time.sleep(0.1) 
                print("Simülasyon bitti.")

if __name__ == '__main__':
    start_server()
import socket
import json
import time
import math

HOST = '127.0.0.1'
PORT = 5000

def generate_telemetry(distance_cm):
    # Fren eğrisi simülasyonu (Birimler: cm ve cm/s)
    if distance_cm > 0:
        # 1.000.000 cm'de (10km) hız yaklaşık 7000 cm/s (252 km/h) olacak
        speed_ebi_cms = math.sqrt(distance_cm) * 7.0 
        
        # Permitted hızı EBI'den yaklaşık 5 m/s (500 cm/s) daha düşük olsun
        speed_permitted_cms = max(0, speed_ebi_cms - 500)
    else:
        speed_ebi_cms = 0
        speed_permitted_cms = 0

    return {
        "speedAndDistance": {
            "dEbi": int(distance_cm),
            "vEbi": int(speed_ebi_cms),
            "dPermitted": int(distance_cm),
            "vPermitted": int(speed_permitted_cms)
        }
    }

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen()
        print(f"Mock Server dinliyor: {HOST}:{PORT}")
        print("Mira uygulamasının bağlanması bekleniyor...")
        
        conn, addr = s.accept()
        with conn:
            print(f"Bağlantı sağlandı: {addr}")
            
            # 10 Kilometre = 1.000.000 Santimetre
            distance_cm = 1000000.0 
            
            while distance_cm >= 0:
                payload = generate_telemetry(distance_cm)
                
                # TCP yapışmasını önlemek için \n ekliyoruz
                json_string = json.dumps(payload) + '\n'
                
                try:
                    conn.sendall(json_string.encode('utf-8'))
                    print(f"Mesafe: {distance_cm} cm, EBI Hız: {payload['speedAndDistance']['vEbi']} cm/s")
                except (ConnectionResetError, BrokenPipeError):
                    print("Mira uygulaması bağlantıyı kopardı.")
                    break
                    
                distance_cm -= 5000.0 # Her adımda 50 metre (5000 cm) yaklaş
                time.sleep(0.1)  # 10 Hz hızında veri bas

if __name__ == '__main__':
    start_server()
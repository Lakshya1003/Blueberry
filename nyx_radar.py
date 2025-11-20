# nyx_radar.py
# Listens on UDP port and draws live accumulating polar scatter plot
import socket
import matplotlib.pyplot as plt
import numpy as np
import threading
import time

LISTEN_IP = "0.0.0.0"   # or your laptop IP e.g. "192.168.4.2"
LISTEN_PORT = 12345

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((LISTEN_IP, LISTEN_PORT))
sock.settimeout(0.5)

angles = []
dists = []

lock = threading.Lock()

def listener_thread():
    while True:
        try:
            data, addr = sock.recvfrom(1024)
        except socket.timeout:
            continue
        except Exception as e:
            print("Socket error:", e)
            continue
        text = data.decode('utf-8').strip()
        # expected "angle,distance"
        try:
            a_str, d_str = text.split(',')
            angle = float(a_str)
            dist = int(d_str)
            if dist < 0:
                # out of range, skip or show as large radius
                continue
            with lock:
                angles.append(np.deg2rad(angle))
                dists.append(dist / 10.0)  # convert mm to cm for plotting
        except Exception as e:
            # ignore parse errors
            # print("Parse error:", text, e)
            pass

t = threading.Thread(target=listener_thread, daemon=True)
t.start()

plt.ion()
fig = plt.figure(figsize=(6,6))
ax = fig.add_subplot(111, projection='polar')
scatter = None

ax.set_theta_zero_location('N')  # 0 at top (north)
ax.set_theta_direction(-1)       # clockwise angles
ax.set_rmax(200)                 # adjust max radius in cm (200cm = 2m)

while True:
    with lock:
        if len(angles) > 0:
            ax.clear()
            ax.set_theta_zero_location('N')
            ax.set_theta_direction(-1)
            ax.set_rmax(200)
            ax.scatter(angles, dists, s=8, c='r')
            plt.title("NYX 2D Radar (accumulating)")
            plt.pause(0.01)
        else:
            plt.pause(0.1)

import socket
import time
import os
import subprocess

def set_system_time(adjusted_time):
    try:
        # Formatear el tiempo ajustado en el formato 'YYYY-MM-DD HH:MM:SS' para el comando `date`
        adjusted_time_str = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(adjusted_time))
        
        # Crear el comando para ajustar la hora del sistema
        command = 'cmd.exe /c time {}'.format(adjusted_time_str.split()[1])
        #date_command = 'cmd.exe /c date {}'.format(adjusted_time_str.split()[0])
        
        # Ejecutar los comandos para ajustar la fecha y hora
        subprocess.call(command, shell=True)
        #subprocess.call(date_command, shell=True)
        
        print("Hora del sistema actualizada correctamente.")
    except Exception as e:
        print("Error al ajustar la hora del sistema: {}".format(e))

def format_time_diff(seconds):
    hours, remainder = divmod(seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    return "{:02}:{:02}:{:02}".format(int(hours), int(minutes), int(seconds))

def time_client(server_host='192.168.100.5', server_port=6666):
    # Crear un socket TCP/IP (SOCK_STREAM)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((server_host, server_port))
        
        # Enviar la solicitud y registrar el tiempo de envío T0
        t0 = time.time()
        
        t0_formatted = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(t0))
       # Enviar la solicitud con la hora local del cliente
        message = "Dame la Hora Global, la mia es {}".format(t0_formatted)
        s.sendall(message.encode())
        print("Hora del cliente (T0): {}".format(t0_formatted))
        
        # Recibir la hora del servidor y registrar el tiempo de recepción T1
        data = s.recv(1024)
        t1 = time.time()
        
        # Calcular el RTT y la hora ajustada
        server_time = float(data.decode())
        rtt = t1 - t0
        adjusted_time = server_time + rtt / 2
        
        # Ajustar la hora del sistema
        set_system_time(adjusted_time)
        
        # Formatear las horas para imprimir en un formato más entendible
        server_time_formatted = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(server_time))
        adjusted_time_formatted = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(adjusted_time))
        rtt_formatted = format_time_diff(rtt)
        time_diff_formatted = format_time_diff(adjusted_time - t1)
       
        
        print("Hora del servidor: {}".format(server_time_formatted))
        print("Tiempo de ida y vuelta (RTT): {} segundos".format(rtt_formatted))
        print("Hora ajustada: {}".format(adjusted_time_formatted))
        print("Diferencia con la hora local: {} segundos".format(time_diff_formatted))

if __name__ == "__main__":
    time_client()

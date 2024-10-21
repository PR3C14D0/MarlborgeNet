# -*- coding: utf-8 -*-
import socket
import ssl

# Crear un contexto SSL sin verificación de certificado
context = ssl.create_default_context()
context.check_hostname = False
context.verify_mode = ssl.CERT_NONE  # Desactiva la verificación del certificado

# Crear el socket TCP
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Envolver el socket con SSL
ssl_client_socket = context.wrap_socket(client_socket, server_hostname='127.0.0.1')

# Conectarse al servidor SSL
ssl_client_socket.connect(('127.0.0.1', 60120))

# Enviar datos al servidor
ssl_client_socket.sendall(b'\x01')

# Recibir la respuesta del servidor
response = ssl_client_socket.recv(1024)

# Enviar más datos
ssl_client_socket.sendall(response)

heartbeat = ssl_client_socket.recv(1024)
print(heartbeat)


# Cerrar la conexión SSL
ssl_client_socket.close()

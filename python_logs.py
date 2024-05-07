import serial
from datetime import datetime

PORT = "COM3" # numer portu

# parametry odczytu: numer portu, prędkość, itp.
arduino = serial.Serial(port = PORT, baudrate = 9600, timeout = 0.1)

while True:

	# odczytaj linię
	msg = arduino.readline()

	# usuń białe znaki np. /n
	msg = msg.strip()

	f = open("logs.log", "a")

	# sprawdź czy wiadomość nie jest pusta
	if msg:
		now = datetime.now()
		f.write(f"[ {now} ]: {msg.decode("utf-8")}\n")

	f.close()


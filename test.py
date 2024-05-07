f = open("plik.txt", "a")
for i in range(1, 11):
	f.write(f"{i} linijka pliku\n")
f.close()

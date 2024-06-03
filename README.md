# System antywłamaniowy

## Uwaga
Ze względu na kwestie bezpieczeństwa przeglądarki domyślnie nie są w stanie otwierać/odczytywać plików za pomocą skryptu js.
Aby skrypt ze statystykami działał należy włączyć komendą lokalny serwer python:
1. Przechodzimy w `explorer.exe` do katalogu z plikiem `index.html`
2. Klikamy w górny pasek do edycji ścieżki katalogu
3. Po podświetleniu ścieżki na niebiesko wpisujemy `cmd` i klikamy `Enter`
4. Wpisujemy komendę ```python -m http.server 8000``` **Nie zamykamy konsoli!**
5. Otwieramy przeglądarkę i w pasku wyszukiwania wpisujemy `localhost:8000`, klikamy `Enter`
6. Wybieramy z listy plików `index.html`

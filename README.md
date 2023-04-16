# ShimmeringDawn
Board bring-up i pisanje modula za Linux operativni sistem
Buildati Linux OS
  - Tokom boot runtimea iscrtati sliku
  - Pri bootanju pokrenuti testnu aplikaciju
  - omoguciti qt biblioteku u okviru Linux slike
  - napisati odgovarajuce DTO fajlove koji definisu dodatni hardver na ploci, ovo ukljucuje nekoliko dugmica i enkodera

Razvoj je moguce odraditi na Raspberriju, a zatim kod prilagoditi specificnom hardveru


Napisati kernel modul koji koristeci INTERRUPT routine detektuje pritisak dugmica i skladno tome koji taster je pritisnut emulira pritisak odgovarajuceg tastera tastature.
Npr. pritisnut je prvi taster, to treba da kernel ocita kao da je pritisnut taster 0 na tastaturi i skladno tome drugi broj za svaki sljedeci taster (ili drugo slovo)
  - Ocitati DTO
  - Dokumentovati kod
Razvoj je moguce odraditi na Raspberriju, a zatim kod prilagoditi specificnom hardveru


Napisati kernel modul koji koristeci INTERRUPT rutine detektuje zakretaj enkodera i to u kom pravcu se pomjeraj desio i prijavljuje to kernel kao dogadjaj ekvivalentan skrolovanju na misu
  - Ocitati DTO
  - Dokumentovati kod
Razvoj je moguce odraditi na Raspberriju, a zatim kod prilagoditi specificnom hardveru


Napisati testnu aplikaciju koja za svaki taster i enkoder ima UI element kojim se validira funkconalnost datog tastera ili enkodera. APlikacija treba da koristi qt biblioteku. Koristiti progres bar za enkoder, pritisak tastera prikazati promjenom boje nekog dijela ekrana
	- Dokumentovati kod
	- Na ekranu postaviti dugme za gasenje i dugme za restart (touch screen)

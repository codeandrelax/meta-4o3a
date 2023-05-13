# StationManagerYOCTO

Board bring-up i pisanje modula za Linux operativni sistem

Buildati Linux OS za NXP i.MX8 8M
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

# KORISNI LINKOVI

https://www.variscite.com/product/system-on-module-som/cortex-a53-krait/var-som-mx8m-nano-nxp-i-mx-8m-nano/?utm_source=google&utm_medium=cpc&utm_campaign=291971056&utm_content=651046960526&utm_term=&gclid=Cj0KCQjwmN2iBhCrARIsAG_G2i6JdotD3rt7zlenG8P5_CO0bHn0vWSfMGHIu9___WM57jci2HttU3UaAlCrEALw_wcB
https://variwiki.com/index.php?title=VAR-SOM-MX8M-NANO_B2QT&release=mx8mn-b2qt-kirkstone-5.15-2.0.x-v1.0
https://doc.qt.io/Boot2Qt/b2qt-customization.html
https://raymii.org/s/tutorials/Yocto_boot2qt_for_the_Raspberry_Pi_4_both_Qt_6_and_Qt_5.html#toc_9
http://www.java2s.com/Code/Cpp/Qt/Addclickedeventforpushbutton.htm

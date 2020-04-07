

#German

##Beschreibung
In diesem Projekt seht ihr meine DIY Rolladensteuerung für das bestehende Ikea System. Mit leichten anpassungen wäre es aber auch möglich, andere Systeme zu Automatisieren.
z.B welche von Lidl, Aldi oder sonstigen Marken.

##Teile
Ihr müsst euch die unten genannten Teile bestellen, dabei habt ihr die Auswahl zwischen den Arduino mega 2650 + Ethernet Shield oder dem wemos d1 mini, der zwar weniger
ports und kein ethernet bietet, dafür aber wesentlich günstiger ist und wifi mit an bord hat.

Ansonsten müssen die unten verlinkten teile einmal Ausgedruckt werden und nach dem Bild verlötet werden, danach sollten die Rolladen über MQTT ansteuerbar sein.
Dabei sendet man einen wert zwischen 31 und -31. Ist der wert Positiv (>0 ), fährt der Motor in die eine Richtung (Rollo hoch), ist der Wert negativ, fährt der Motor in die andere
(Rollor runter)

##Wichtig
Mittlweile wird die aktuelle Position des Rollos im Speicher des Arduinos gehalten, damit man nicht versentlich weiter runter (oder hoch) fährt, obwohl das Rollo schon seine
endposition erreicht hat.
Dazu ist es allerdings zwingend notwendig, beim ersten in Betrieb nehmen, das Rollo komplett nach unten zu fahren, und per MQTT auf das Topic "Buero/Fenster/Blinds/all/save"
zu posten. dadurch wird automatisch die Position gespeichert.

Sollten noch fragen offen sein, bitte zuerst das Video anschauen:
{URL}


# DIY Ikea Arduino Blind Controller

this is how i got my arduino blind controller to work.

## First step: scematics:

![scematics](link-to-image)

##Parts:
You need the printable parts for the ikea blinds, provided in git and these parts:
- Stepper Motor + Motor Shield https://amzn.to/2RlKakJ
- Arduino Mega 2650 https://amzn.to/2JOvDtW
- Ethernet Shield https://amzn.to/2XpeYEW
- Step down converter https://amzn.to/3aQY0TT
- Relais https://amzn.to/2xWdIic
- jumper wires https://amzn.to/2Vh4yVA

thats it.

you can also build a cheaper one with this parts:
-wemos d1 mini https://amzn.to/2V9FTSG

if you decide to use this one, you dont need the Arduino mega and the ethernet shield, cause you can use the buildin wifi.

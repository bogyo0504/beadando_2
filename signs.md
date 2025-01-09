# Jelölések:


A pipeline egyszerű representációja a következő:

~~~

-.--,---o, -. , -', | ,-|-, |-,-| ,-'-, 

	.-----c-. 
	|       | 
	'-----o-' 
	
	
	normal: " ", "-"
	source: "o"
	sink: "c"
	valve: "0"
	
	a dupla kanyar jelölése: -:-  (-' és .---) , -;- (-. és '---)
    a kereszteződés jelölése: -|- 
	
	
	

~~~

A készlet a következőképpen néz ki:

~~~

3 -|--- (egy Tile)
2 ----- (egy másik Tile)
3 ---oG(egy másik Tile)
1 -.   (egy másik Tile)

~~~

Egy fájl a következőképpen néz ki:

~~~
3 -|--- (egy Tile)
2 ----- (egy másik Tile)
3 ---oG(egy másik Tile)
1 -.   (egy másik Tile)
[Phases]
BG (első fázis)
Y (második fázis)
RYG (harmadik fázis)
[Pipes]
  -oB               ---cB`
  -oY               ---cY`
  -oR               ---cR`
  -oG               ---cG`

(ez egy 5x4-es pálya)
~~~
make: wcclient

wcclient: wcclient.c
	gcc -o wcclient wcclient.c

clean:
	rm  wcclient

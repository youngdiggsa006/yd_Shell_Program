#Make file for the CMSC257 Project 2
# 

#Make environment

my257sh : 
	gcc -pthread shellex.c csapp.c -o sh257

clean:
	rm -v sh257

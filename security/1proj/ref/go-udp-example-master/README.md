# udp socket programming example

to try out the example, [first install go](http://golang.org/doc/install)

for this project, you probably don't need to set up your `GOPATH`,
but if you want to do other go programming, its a good idea.
There is a good section on [workspace setup here](http://golang.org/doc/code.html)


# The example
To run the example, build both programs:
```
go build listen.go
go build client.go
```

Now, run the listen program in one terminal, and then the client in another.
They should connect to eachother and send a message like so:

On the listeners side:
```
whyrusleeping@idril ~/c/t/udpsample> ./listen 
got message of size 39 bytes from 127.0.0.1:58417
got a message!
seq: 17, mes: Hello World
```

And from the clients perspective:
```
whyrusleeping@idril ~/c/t/udpsample> ./client 
wrote 39 bytes!
```


Any questions, email me at why@ipfs.io and i'll try and help out!

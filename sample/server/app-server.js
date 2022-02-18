var express = require('express');
var socket = require('socket.io');
var path = require('path');
var app = express();
var http = require('http')
var server = http.createServer(app);
var io = socket(server);

server.listen(5000, () => {
	console.log('Listening on *:5000');
});

// Serve the build
// app.get('/', function (req, res) {
// 	res.sendFile('index.html');
// });


io.on('connection', socket => {

	usersConnected += 1;
	//io.emit('userConnectChange', usersConnected );
	console.log(`Connection to Client established.`);


	socket.on('updateAttributes', () => {
	})

	socket.on('disconnect', () => {
		console.log(`Connection terminated.`);
	 });
});


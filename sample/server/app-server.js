var express = require('express');
var { Server } = require('socket.io');
var path = require('path');
var app = express();
var http = require('http');

const httpServer = http.createServer(app);
const io = new Server(httpServer, {
  cors: {
    origin: 'http//:localhost:3000',
  },
});
const { execFile } = require('child_process');

// Serve the build
// app.get('/', function (req, res) {
// 	res.sendFile('index.html');
// });

io.on('connection', (socket) => {
  console.log(`Connection to Client established.`);

  socket.on('sc_update_to_author', (libSCdataJSON) => {
	  console.log(JSON.parse(libSCdataJSON))
    let libSCdata = JSON.parse(libSCdataJSON);
    const child = execFile(
      path.join(__dirname, "libsc/outputs/libsc_sample"),
      [
        path.join(__dirname, "libsc/outputs/modelCache"),
        'microengine',
        //libSCdata.attributes
      ],
	  {
		  env: {"LD_LIBRARY_PATH": path.join(__dirname, "/libsc/bin/macos/") }
	  },
      (error, stdout, stderr) => {
        if (error) {
          throw error;
        }
        console.log(stdout);
      }
    );
  });

  socket.on('disconnect', () => {
    console.log(`Connection terminated.`);
  });
});


httpServer.listen(5000, () => {
	console.log('Listening on *:5000');
  });
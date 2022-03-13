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
const { spawn } = require('child_process');

// Serve the build
// app.get('/', function (req, res) {
// 	res.sendFile('index.html');
// });

io.on('connection', (socket) => {
  console.log(`Connection to Client established.`);

  socket.on('sc_update_to_author', (libSCdataJSON) => {
    console.log(libSCdataJSON);
    const child = spawn(
      path.join(__dirname, 'libsc/outputs/libsc_sample.x86_64'),
      [path.join(__dirname, 'libsc/outputs/modelCache'), 'microengine', libSCdataJSON],
      {
        env: { LD_LIBRARY_PATH: path.join(__dirname, '/libsc/bin/macos/') },
      }
    );

	let lineBuffer = "";

    child.stdout.on('data', (data) => {
      lineBuffer += data.toString();
	
      var lines = lineBuffer.split('\n');

      for (var i = 0; i < lines.length - 1; i++) {
        var line = lines[i];

        socket.emit('libscstdout', line);
      }

      lineBuffer = lines[lines.length - 1];
    });

    child.stdout.on('end', () => {
      console.log(lineBuffer);
      socket.emit('libscstdout', lineBuffer);
    });
  });

  // Probably want to send over the model name we are working with too, so we can
  // locate it in the cache. Working with one model now, but could have multiple
  // loaded into the viewer
  socket.on('viewer-models-loaded', () => {});

  socket.on('disconnect', () => {
    console.log(`Connection terminated.`);
  });
});

httpServer.listen(5000, () => {
  console.log('Listening on *:5000');
});

var fs = require("fs");
var MidiConvert = require('@pioug/MidiConvert');
var file = ".\\MIDISong.mid" ;
var app = require('express').express();
const raspi = require('raspi');
const I2C = require('raspi-i2c').I2C;

var address = 0x12;
var wire = new i2c(address, {device: '/dev/i2c-1'});
//var MIDI_manager = require(".\\FTP.js")();

/*app.get("/files",function(res,req){
  //lire tout les fichier dans le repository
  //stringify le result
  //renvoi la liste
})*/

var initResult = function(jsonMidi){
  var result = [];
  var size = 0;

  /*jsonMidi.parts.forEach(function(part){
      size += part.length;
  });*/
  var max = 0;
  for(var k = 0 ; k<jsonMidi.parts.length;k++){
    var begin = parseInt( jsonMidi.parts[k][jsonMidi.parts[k].length-1].time.substring(0,jsonMidi.parts[k][jsonMidi.parts[k].length-1].time.length-1),10);
    var duration = parseInt( jsonMidi.parts[k][jsonMidi.parts[k].length-1].duration.substring(0,jsonMidi.parts[k][jsonMidi.parts[k].length-1].duration.length-1),10);

    if(max<begin+duration){
      max = begin+duration;
    }
  size = max;
  }
  console.log(size);
  for(var i = 0;i<size;i++){
    var tab = []
    for(var j = 0 ; j<4;j++){
      tab[j]=0
    }
    result.push(tab);
  }
  return result;
}

var filtrePianoJsonMidi = function(jsonMidi){
  var promise = new Promise(function(resolve, reject) {
    var result= {
      "parts":[],
      "transport":{
        "instruments":[],
      }
    }
    for(var i = 0;i<jsonMidi.transport.instruments.length;i++){
        if(jsonMidi.transport.instruments[i]<=10 && jsonMidi.transport.instruments[i]>0){
            result.transport.instruments.push(jsonMidi.transport.instruments[i]);
            result.parts.push(jsonMidi.parts[i]);
        }
      }

    result.transport.bpm = jsonMidi.transport.bpm;
    resolve(result);
  })
  return promise;
}


var parseJsonMidi = function(JsonMidi){
    var promise = new Promise(function(resolve, reject) {
    var result = initResult(JsonMidi);


    var noteCode = ['C','D','E','F'];
    JsonMidi.parts.forEach(function(parts){
      parts.forEach(function(part){
        noteCode.forEach(function(note){


        if(note == part.noteName[0]){

          var begin = parseInt(part.time.substring(0,part.time.length-1),10);
          var duration = parseInt(part.duration.substring(0,part.duration.length-1),10);
          console.log(duration+begin);

          for(var i = begin-1; i < begin + duration-4 ;i++){
            result[i][noteCode.indexOf(note)] = 1;
          }

        }
      })
    })
  })
  console.log(JSON.stringify(result));
    resolve(result);
  });
  return promise;
}


var getLed = function(filePath){
  fs.readFile(filePath, 'binary', function(err, buffer) {
    if (err) return;
    var jsonMidi = MidiConvert.parse(buffer);
    filtrePianoJsonMidi(jsonMidi)
    .then(function(piano){
      return parseJsonMidi(piano);
    });
  });
}


var data ;
var values;
var tempo;


function start(i2c){

var finished = Boolean(false);
//create an array
var array1 = new Array("start");
//assign the value "start"
//array1.push('start');
//assign the array to the buffer
const starting_buffer = new Buffer.from('start','utf8');

//send message that says to start (value 2)
i2c.writeSync(0x12,starting_buffer);

//while not finished, we send data
//while(i2c.readByteSync(0x12)){
}

function send(i2c,data){
//values for the leds
values = new Uint8Array(4);
for(var i = 0; i < values.length; i++){

values[i] = data[i];
console.log(values[i]);

}
const buf = new Buffer(values.buffer);
i2c.writeSync(0x12,buf);
//i2c.writeByteSync(0x12,values);

}


function stop(i2c){

const stopping_buffer = new Buffer('stop','utf8');
i2c.writeSync(0x12,stopping_buffer);

}

else console.log("please check the parameters");


});
var interval = 20;

var Start = function(jsonMidi){
  var enregi = [];
  var led = getLed(file);
  var timeI = 0;
  var clock = 60000000/jsonMidi.transport.bpm;
  start(i2c);
  sendData(led,interval,timeI,clock,enregi);
}

var sendData= function(ledTab,interval,timeI,clock,enregi){
  send(i2c,ledTab[timeI]);
    readWord(0x12, function(err,dat){
      enregi.push(dat,timeI);
      if(timeI<50000){
        setTimeout(sendData(ledtab,interval,timeI+1,clock,enregi), clock);

      }
      else{
        end(enregi);
      }
  }
});

var end = function(enregi){
  stop(i2c);
  handle(enregi);
}

var handle = function(enregi){
  console.log(enregi);
}

raspi.init(() => {
const i2c = new I2C();
  fs.readFile(file, 'binary', function(err, buffer) {
    json  = MidiConvert.parse(buffer);
    Start(json);
    }
  }
}

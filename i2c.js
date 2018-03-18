
/*

install with npm : npm install raspi-i2c

il faudrait faire 2 fichiers :
	- start.js
	- stop.js

sur le portail lorsque on lance la partie :
on appel : "node start.js tempo valeurs

avec tempo : entier 
valeurs : tableaux de int (0 ou 1)

simulation :(fonctionelle)

To Start : call "node i2c.js 1"
To SendData : " call "node i2c.js 2 110101"

*/

//modules

const raspi = require('raspi');
const I2C = require('raspi-i2c').I2C;

//example de données (1ere : temp, les 6 autres les données)
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

function sendData(i2c){

//values for the leds
values = new Uint8Array(6);
for(var i = 0; i < values.length; i++){

values[i] = data[i];
console.log(values[i]);
}
const buf = new Buffer(values.buffer);

i2c.writeSync(0x12,buf);
//i2c.writeByteSync(0x12,values);

}

function stop(i2c){

const stopping_buffer = new Buffer.from('stop','utf8');
i2c.writeSync(0x12,stopping_buffer);
}

//var buff = new Buffer([0,0,1,1,0,1]);

//start =>  0
//stop =>  1

raspi.init(() => {
const i2c = new I2C();
//  console.log(i2c.readByteSync(0x12)); // Read one byte from the device at address 12 i2c.
//wordToByteArray();

if(process.argv[2] == 1) start(i2c);
if(process.argv[2] == 2) {
data = (process.argv[3]).split('');
console.log(data.toString());
sendData(i2c);
}
if(process.argv[2] == 3) stop(i2c);


//i2c.writeByteSync(0x12,64);
//i2c.writeWordSync(0x12,tab);


});



/*

install with npm : npm install raspi-i2c

il faudrait faire 2 fichiers :
	- start.js
	- stop.js

sur le portail lorsque on lance la partie :
on appel : "node start.js tempo valeurs

avec tempo : entier 
valeurs : tableaux de int (0 ou 1)

*/

//modules

const raspi = require('raspi');
const I2C = require('raspi-i2c').I2C;

//example de données (1ere : temp, les 6 autres les données)
const data = [2,0,0,1,1,0,1];
var values;
var tempo;

function ConvertBinaryToInt(array){


}

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



//values for the leds
values = new Uint8Array(7);
for(var i = 0; i < values.length; i++){

values[i] = data[i];
console.log(values[i]);
}
const buf = new Buffer(values.buffer);

i2c.writeSync(0x12,buf);
//i2c.writeByteSync(0x12,values);




}

//var buff = new Buffer([0,0,1,1,0,1]);

//start =>  0
//stop =>  1

raspi.init(() => {
const i2c = new I2C();
//  console.log(i2c.readByteSync(0x12)); // Read one byte from the device at address 12 i2c.
//wordToByteArray();
start(i2c);

//i2c.writeByteSync(0x12,64);
//i2c.writeWordSync(0x12,tab);


});


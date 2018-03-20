
/*

install with npm :
npm install .
npm install raspi-i2c
npm install raspi

3 modes :
	- start (1)
	- sendData(2)
	- stop (3)

commandline :
	- to Start : "node i2c.js 1"
	- to Send specific data : " call "node i2c.js 2 110101"
	- to Stop : "node i2c.js 3"

*/

const raspi = require('raspi');
const I2C = require('raspi-i2c').I2C;

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


raspi.init(() => {
const i2c = new I2C();

if(process.argv[2] == 1){
start(i2c);
}
else if(process.argv[2] == 2){
data = process.argv[3].split('');
console.log(data.toString());
sendData(i2c);
}
else if(process.argv[2] == 3){
stop(i2c);
}
else console.log("please check the parameters");
/*

//WITH A SWITCH IT DOESN'T WORK ...

switch(process.argv[2]){
case '1' :
start(i2c);
break;
case '2':
data = (process.argv[3]).split('');
console.log(data.toString());
sendData(i2c);
case '3' :
stop(i2c);
break;
default:
console.log("please check the parameters");
}

*/

});

